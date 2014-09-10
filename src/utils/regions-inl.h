/*
 * The MIT License (MIT)
 * 
 * Copyright (c) 2013 Taketoshi Aono(brn)
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */


#ifndef UTILS_POOL_INL_H_
#define UTILS_POOL_INL_H_


#include <stdlib.h>
#include <type_traits>
#include <thread>


namespace yatsc {


// The header of allocated memory block.
// This area is used until destruct.
class Regions::Header {
 public:

  
  // Return the size of block except header.
  YATSC_INLINE size_t size() YATSC_NOEXCEPT {
    return size_ & kTagRemoveBit;
  }


  
  // Set the block size that is value block size.
  YATSC_INLINE void set_size(size_t size) YATSC_NOEXCEPT {
    bool array = IsMarkedAsArray();
    bool dealloced = IsMarkedAsDealloced();
    bool ptr = IsMarkedAsPtr();
    size_ = size;
    if (array) MarkAsArray();
    if (dealloced) MarkAsDealloced();
    if (ptr) MarkAsPtr();
  }
  

  
  // Return the front of memory block.
  YATSC_INLINE Byte* ToBegin() YATSC_NOEXCEPT {
    return reinterpret_cast<Byte*>(this);
  }



  // Return Regions::FreeHeader.
  YATSC_INLINE Regions::FreeHeader* ToFreeHeader() YATSC_NOEXCEPT {
    return reinterpret_cast<FreeHeader*>(reinterpret_cast<Byte*>(this) + kHeaderSize);
  }



  // Return next block.
  // This method simply advance pointer by the memory block size
  // so it does not perform any check.
  YATSC_INLINE Header* next_addr() YATSC_NOEXCEPT {
    return reinterpret_cast<Header*>(ToValue<Byte*>() + size());
  }


  // Return value block.
  template <typename T = RegionalObject*>
  YATSC_INLINE typename std::remove_pointer<T>::type* ToValue() YATSC_NOEXCEPT {
    return reinterpret_cast<typename std::remove_pointer<T>::type*>(ToBegin() + kSizeTSize);
  }



  // Mark memory block as not used.
  YATSC_INLINE void MarkAsDealloced() YATSC_NOEXCEPT {
    size_ |= kDeallocedBit;
  }



  // Mark memory block used.
  YATSC_INLINE void UnmarkDealloced() YATSC_NOEXCEPT {
    size_ &= kDeallocedMask;
  }



  // Return this memory block is marked as not used.
  YATSC_INLINE bool IsMarkedAsDealloced() YATSC_NOEXCEPT {
    return (size_ & kDeallocedBit) == kDeallocedBit;
  }



  // Mark this memory block allocated as array.
  YATSC_INLINE void MarkAsArray() YATSC_NOEXCEPT {
    size_ |= kArrayBit;
  }



  // Return this memory block allocated as array.
  YATSC_INLINE bool IsMarkedAsArray() YATSC_NOEXCEPT {
    return (size_ & kArrayBit) == kArrayBit;
  }


  // Mark this member block allocated as bulk ptr.
  YATSC_INLINE void MarkAsPtr() YATSC_NOEXCEPT {
    size_ |= kPtrBit;
  }


  // Return this member block allocated as bulk ptr.
  YATSC_INLINE bool IsMarkedAsPtr() YATSC_NO_SE {
    return (size_ & kPtrBit) == kPtrBit;
  }

 private:
  size_t size_;
};



// This header that is only available if memory block is not used is
// record the next pointer of the free list.
// In used block this header area is used as a user allocation block.
class Regions::FreeHeader {
 public:    

  /**
   * Return next FreeHeader in the free list.
   * @return next FreeHeader in the free list.
   */
  YATSC_INLINE FreeHeader* ToNextPtr() YATSC_NOEXCEPT {
    return reinterpret_cast<FreeHeader*>(next_);
  }


  /**
   * Set next ptr.
   */
  YATSC_INLINE void set_next_ptr(Byte* next_ptr) YATSC_NOEXCEPT {
    next_ = next_ptr;
  }


  /**
   * Return front of FreeHeader.
   */
  YATSC_INLINE Byte* ToBegin() YATSC_NOEXCEPT {
    return reinterpret_cast<Byte*>(this);
  }


  /**
   * Return the header of this memory block.
   * @return The header of this memory block.
   */
  YATSC_INLINE Regions::Header* ToHeader() YATSC_NOEXCEPT {
    return reinterpret_cast<Header*>(ToBegin() - kFreeHeaderSize);
  }

  
 private:
  Byte* next_;
};



/**
 * Destroy all allocated chunks.
 */
void Regions::Destroy() YATSC_NOEXCEPT {
  // Check atomically.
  if (!deleted_.test_and_set()) {
    central_arena_->Destroy();
  }
}



/**
 * Release allocated memory block.
 * @param object The pointer which returned from Regions::Allocate(size_t)
 * or Regions::AllocateArray(size_t)
 */
void Regions::Dealloc(void* object) YATSC_NOEXCEPT {
  central_arena_->Dealloc(object);
}


template <typename T, typename ... Args>
T* Regions::New(Args ... args) {
  static_assert(std::is_base_of<RegionalObject, T>::value == true,
                "The type argument of the yatsc::Reigons::New must be derived class of yatsc::RegionalObject.");
  return new(Allocate(sizeof(T))) T(args...);
}


template <typename T>
T* Regions::NewPtr(size_t num) {
  return static_cast<T*>(AllocatePtr(num));
}


template <typename T, typename ... Args>
T* Regions::NewArray(size_t size, Args ... args) {
  static_assert(std::is_base_of<RegionalObject, T>::value == true,
                "The type argument of the yatsc::Reigons::New must be derived class of yatsc::RegionalObject.");
  YATSC_CHECK(true, size > 0);

  // In case of array, we allocate extra space which holds array size and each type size.
  size_t alloc_size = YATSC_ALIGN_OFFSET(((sizeof(T) * size) + (kSizeTSize * 2)), kAlignment);
  Byte* area = reinterpret_cast<Byte*>(AllocateArray(alloc_size));

  // The size of array.
  size_t* size_ptr = reinterpret_cast<size_t*>(area);
  *size_ptr = size;

  // The value of sizeof(T)
  size_t* object_size = reinterpret_cast<size_t*>(area + kSizeTSize);
  *object_size = sizeof(T);

  // Rest is object heap.
  T* array_zone = reinterpret_cast<T*>(area + (kSizeTSize * 2));
  
  // initialize one by one.
  for (unsigned i = 0u; i < size; i++) {
    new(array_zone + i) T(args...);
  }
  return array_zone;
}



/**
 * Allocate memory block from pool.
 * @param size The size which want to allocate.
 * @return Unused new memory block.
 */
void* Regions::Allocate(size_t size) {
  return DistributeBlock(size)->ToValue<void>();
}


/**
 * Allocate memory block from pool.
 * @param size The size which want to allocate.
 * @return Unused new memory block.
 */
void* Regions::AllocatePtr(size_t size) {
  Header* header = DistributeBlock(size, true);
  header->MarkAsPtr();
  ASSERT(true, header->IsMarkedAsPtr());
  return header->ToValue<void>();
}


/**
 * Allocate memory block from pool.
 * This method mark header as Array.
 * @param size The size which want to allocate.
 * @return Unused new memory block.
 */
void* Regions::AllocateArray(size_t size) {
  Header* header = DistributeBlock(size);
  header->MarkAsArray();
  ASSERT(true, header->IsMarkedAsArray());
  return header->ToValue<void>();
}


/**
 * Destruct memory block by the proper method.
 * @param header The memory block which want to destruct.
 */
void Regions::DestructRegionalObject(Regions::Header* header) {
  if (header->IsMarkedAsPtr()) {
    return;
  }
  if (!header->IsMarkedAsArray()) {
    header->ToValue()->~RegionalObject();
  } else {
    // case array.
    Byte* area = header->ToValue<Byte*>();
    // Get the array size from embedded size bit.
    size_t array_size = *reinterpret_cast<size_t*>(area);
    // Get the type size from embedded size bit.
    size_t object_size = *reinterpret_cast<size_t*>(area + kSizeTSize);
    Byte* array_zone = reinterpret_cast<Byte*>(area + (kSizeTSize * 2));
    for (unsigned i = 0u; i < array_size; i++) {
      reinterpret_cast<RegionalObject*>(array_zone + (object_size * i))->~RegionalObject();
    }
  }
}


Regions::Header* Regions::DistributeBlock(size_t size, bool ptr) {
  const size_t kRegionalObjectSize = sizeof(RegionalObject);

  if (!ptr && kRegionalObjectSize > size) {
    size = kRegionalObjectSize;
  }
  return central_arena_->Commit(size, size_);
}


// Create Chunk from byte block.
// Chunk and heap block is create from one big memory block.
// The structure is below
// |8-BIT VERIFY BIT|Chunk MEMORY BLOCK|The memory block managed BY Chunk|
Regions::Chunk* Regions::Chunk::New(size_t size, Mmap* allocator) {
  ASSERT(true, size <= kMaxAllocatableSize);
  static const size_t kChunkSize = sizeof(Chunk);
  const size_t aligned_size = YATSC_ALIGN_OFFSET(size, kAlignment);

#if defined(DEBUG)
  // All heap size we want.
  const size_t heap_size = YATSC_ALIGN_OFFSET((kVerificationTagSize + kChunkSize + aligned_size), kAlignment);
#else
  // All heap size we want.
  const size_t heap_size = YATSC_ALIGN_OFFSET((kChunkSize + aligned_size), kAlignment);
#endif
  Byte* ptr = reinterpret_cast<Byte*>(allocator->Commit(heap_size));

#if defined(DEBUG)
  // Verification bit.
  VerificationTag* tag = reinterpret_cast<VerificationTag*>(ptr);
  (*tag) = kVerificationBit;
  void* chunk_area = PtrAdd(ptr, kVerificationTagSize);
#else
  void* chunk_area = ptr;
#endif
  
  // Instantiate Chunk from the memory block.
  return new(chunk_area) Chunk(reinterpret_cast<Byte*>(PtrAdd(chunk_area, kChunkSize)), aligned_size);
}


void Regions::Chunk::Delete(Chunk* chunk) YATSC_NOEXCEPT {
  chunk->Destruct();
  chunk->~Chunk();
}


bool Regions::Chunk::HasEnoughSize(size_t needs) YATSC_NO_SE {
  needs = needs > kFreeHeaderSize? needs: kFreeHeaderSize;
  return block_size_ >= used_ + (YATSC_ALIGN_OFFSET((kValueOffset + needs), kAlignment));
}
// Chunk inline end


// ChunkList inline begin
inline Regions::Header* Regions::ChunkList::AllocChunkIfNecessary(size_t size, Mmap* mmap, Regions::CentralArena* arena) {
  if (head_ == nullptr) {
    current_ = head_ = Regions::Chunk::New(YATSC_ALIGN_OFFSET((100 KB), kAlignment), mmap);
  }
  
  if (!current_->HasEnoughSize(size)) {
    Header* header = arena->FindFreeChunk(size);
    if (header != nullptr) {
      return header;
    }
    
    static const size_t kHundredKilloByte = 100 KB;
    size_t real_size = size + kValueOffset;
    size_t alloc_size = kHundredKilloByte > real_size? kHundredKilloByte: real_size;
    current_->set_next(Regions::Chunk::New(YATSC_ALIGN_OFFSET(alloc_size, kAlignment), mmap));
    current_ = current_->next();
  }
  return current_->GetBlock(size);
}
// ChunkList inline end


// CentralArena inline begin
inline Regions::Header* Regions::CentralArena::Commit(size_t size, size_t default_size) {
  ASSERT(true, size > 0);
  int index = FindBestFitBlockIndex(size);
  LocalArena* local_arena = TlsAlloc();

  if (local_arena->has_free_chunk(index)) {
    return local_arena->free_chunk_stack(index)->Shift();
  }

  ChunkList* chunk_list = local_arena->chunk_list();
  return chunk_list->AllocChunkIfNecessary(size, local_arena->allocator(), this);
}


void Regions::CentralArena::CollectGarbage(Regions::LocalArena* arena) {
  FreeChunkStack* free_chunk_stack = arena->free_chunk_stack();
  for (int i = 0; i < kMaxSmallObjectsCount; i++) {
    FreeHeader* free_header = free_chunk_stack[i].head();
    FreeChunkStack* central_free_chunk = &(central_free_chunk_stack_[i]);
    while (free_header != nullptr) {
      FreeHeader* next = free_header->ToNextPtr();
      central_free_chunk->Unshift(free_header->ToHeader());
      free_header = next;
    }
    free_chunk_stack[i].Clear();
  }

  FreeMap(arena);
  arena->ReleaseLock();
}


void Regions::CentralArena::FreeMap(Regions::LocalArena* arena) {
  ScopedSpinLock lock_(central_huge_free_map_lock_);
  HugeChunkMap* map = arena->huge_free_chunk_map();
  huge_free_chunk_map_->insert(map->begin(), map->end());
  map->clear();
}


Regions::Header* Regions::CentralArena::FindFreeChunk(size_t size) {
  int index = FindBestFitBlockIndex(size);
  Header* ret = nullptr;
  
  if (index < kMaxSmallObjectsCount) {
    ret = central_free_chunk_stack_[index].Shift();
  } else {
    ScopedSpinLock lock(central_huge_free_map_lock_);
    if (huge_free_chunk_map_->count(index) > 0) {
      FreeChunkStack* free_chunk_stack = huge_free_chunk_map_->operator[](index);
      if (free_chunk_stack->HasHead()) {
        ret = free_chunk_stack->Shift();
      }
      if (!free_chunk_stack->HasHead()) {
        huge_free_chunk_map_->erase(index);
      }
    }
  }

  return ret;
};


inline int Regions::CentralArena::FindBestFitBlockIndex(size_t size) YATSC_NOEXCEPT {
  YATSC_CHECK(true, size > 0);
  return (size / kAlignment) - 1;
}


Regions::LocalArena* Regions::CentralArena::FindUnlockedArena() YATSC_NOEXCEPT {
  LocalArena* arena = arena_head_;
  while (arena != nullptr) {
    if (arena->AcquireLock()) {
      return arena;
    }
    arena = arena->next();
  }
  return nullptr;
}


Regions::LocalArena* Regions::CentralArena::TlsAlloc() {
  LocalArena* arena = reinterpret_cast<LocalArena*>(tls_->Get());
  if (arena == nullptr) {
    if (arena_head_ != nullptr) {
      arena = FindUnlockedArena();
    }
    if (arena == nullptr) {
      void* block = mmap_->Commit(sizeof(LocalArena));
      arena = new(block) LocalArena(this, huge_chunk_allocator_);
      arena->AcquireLock();
      StoreNewLocalArena(arena);
    }

    tls_->Set(arena);
  }
  
  return arena;
}


void Regions::CentralArena::StoreNewLocalArena(Regions::LocalArena* arena) YATSC_NOEXCEPT {
  ScopedSpinLock lock(central_free_arena_lock_);
  if (arena_head_ == nullptr) {
    arena_head_ = arena_tail_ = arena;
  } else {
    arena_tail_->set_next(arena);
    arena_tail_ = arena;
  }
}
// CentralArena inline end


// FreeChunkStack inline begin
void Regions::FreeChunkStack::Unshift(Regions::Header* header) YATSC_NOEXCEPT {
  ScopedSpinLock lock(tree_lock_);
  ASSERT(true, header->IsMarkedAsDealloced());
  FreeHeader* block = header->ToFreeHeader();
  if (free_head_ == nullptr) {
    free_head_ = block;
    free_head_->set_next_ptr(nullptr);
  } else {
    block->set_next_ptr(free_head_->ToBegin());
    free_head_ = block;
  }
  ASSERT(true, free_head_->ToHeader()->IsMarkedAsDealloced());
}


YATSC_INLINE Regions::Header* Regions::FreeChunkStack::Shift() YATSC_NOEXCEPT {
  ScopedSpinLock lock(tree_lock_);
  if (free_head_ == nullptr) {return nullptr;}
  Header* header = free_head_->ToHeader();
  ASSERT(true, header->IsMarkedAsDealloced());
  free_head_ = free_head_->ToNextPtr();
  header->UnmarkDealloced();
  ASSERT(false, header->IsMarkedAsDealloced());
  return header;
}
// FreeChunkStack inline end


// LocalArena inline begin
Regions::LocalArena::LocalArena(
    Regions::CentralArena* central_arena, HugeChunkAllocator* huge_chunk_allocator)
    : central_arena_(central_arena),
      huge_free_chunk_map_(*huge_chunk_allocator),
      next_(nullptr) {
  lock_.clear();
}


Regions::LocalArena::~LocalArena() {}


void Regions::LocalArena::Return() {
  central_arena_->CollectGarbage(this);
}
// LocalArena inline end

} // namespace yatsc

#endif
