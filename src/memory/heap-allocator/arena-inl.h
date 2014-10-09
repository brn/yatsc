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

#ifndef MEMORY_HEAP_ALLOCATOR_ARENA_INL_H 
#define MEMORY_HEAP_ALLOCATOR_ARENA_INL_H

#include <thread>

namespace yatsc {namespace heap {


// Allocate new memory block from TLS LocalArena.
// This method return the heap that has the specified size and aligned by ChunkHeader::kAlignment.
// If given size is more than ChunkHeader::kChunkMaxAllocatableSmallObjectSize,
// allocate large memory block from CentralArena.
// This method is thread safe.
YATSC_INLINE void* CentralArena::Allocate(size_t size) YATSC_NOEXCEPT {
  // Get thread specific LocalArena.
  LocalArena* arena = GetLocalArena();
  const size_t actual_size = YATSC_ALIGN_OFFSET(size, yatsc::kAlignment);

  void* ret = arena->Allocate(actual_size);
  // Now check return value of LocalArena::Allocate,
  // because if given size is more than ChunkHeader::kChunkMaxAllocatableSmallObjectSize,
  // return nullptr.
  if (ret != nullptr) {
    return ret;
  }

  // If returned pointer is nullptr, this mean specified size is over ChunkHeader::kChunkMaxAllocatableSmallObjectSize,
  // ChunkHeader is not able to treat that large size heap,
  // so allocated from CentralArena.
  return AllocateLargeObject(actual_size);
}


// Deallocate memory block that allocated from CentralArena::Allocate.
// The given pointer must be LocalArena heap or CentralArena large heap.
// If specified pointer is small object pointer that has aligned by ChunkHeader::kAlignemnt,
// we calculate front of the heap by masking ChunkHeader::kAddrMask,
// if specified pointer is large object pointer that has large object flag in right most bit of pointer,
// simply unmap virtual memory.
YATSC_INLINE void CentralArena::Dealloc(void* ptr) YATSC_NOEXCEPT {
  ASSERT(true, ptr != nullptr);
  
  // If pointer address is odd, this pointer is allocated from CentralArena::AllocateLargeObject.
  if ((reinterpret_cast<uintptr_t>(ptr) & 1) == 1) {
    // Remove tag bit from pointer.
    // ...10101 -> ...10100
    auto area = reinterpret_cast<Byte*>(reinterpret_cast<uintptr_t>(ptr) & ~1);

    // Get LargeHeader from pointer address.
    // [LargeHeader][HeapBody]
    auto large_header = reinterpret_cast<LargeHeader*>(reinterpret_cast<Byte*>(area) - sizeof(LargeHeader));

    // CentralArena is not thread safe, so we lock large_bin_.
    lock_.lock();
    large_bin_.Delete(large_header->size());
    lock_.unlock();

    // Simply unmap.
    VirtualHeapAllocator::Unmap(large_header, large_header->size());
  } else {

    // Get head of the heap by masking memory address.
    // All the small heap has address like follows
    // .e.g
    // [0x103e60000] head
    // ... [0x10e6a6320] ptr
    // 0x10e6a6320
    // ------^^^^^ Masking heare.
    auto h = reinterpret_cast<HeapHeader*>(reinterpret_cast<uintptr_t>(ptr) & ChunkHeader::kAddrMask);
    ASSERT(true, h != nullptr);
    ChunkHeader* chunk_header = h->chunk_header();
    ASSERT(true, chunk_header != nullptr);
    chunk_header->Dealloc(ptr);
  }
}


// Dealloc specialization for volatile pointer.
YATSC_INLINE void CentralArena::Dealloc(volatile void* ptr) YATSC_NOEXCEPT {
  Dealloc(const_cast<void*>(ptr));
}


// Called from LocalArena, when lock is acquired.
// When this method is called, simply decrement the released_arena_count_.
YATSC_INLINE void CentralArena::NotifyLockAcquired() YATSC_NOEXCEPT {
  if (released_arena_count_.load() == 0) {
    released_arena_count_.fetch_sub(1);
  }
}


// Called from LocalArena, when lock is released,
// When this method is called, simply decrement the released_arena_count_.
YATSC_INLINE void CentralArena::NotifyLockReleased() YATSC_NOEXCEPT {
  released_arena_count_.fetch_add(1);
}


// Return thread specific LocalArena from TLS.
// But if we not allocated LocalArena yet,
// find released LocalArena from the CentralArena's arena list or Allocate new LocalArena.
YATSC_INLINE LocalArena* CentralArena::GetLocalArena() YATSC_NOEXCEPT {
  void* ptr = tls_->Get();
  if (ptr == nullptr) {
    ptr = FindUnlockedArena();
    tls_->Set(ptr);
  }
  return reinterpret_cast<LocalArena*>(ptr);
}


// Get heap space that used to allocate LocalArena and ChunkHeader.
YATSC_INLINE void* CentralArena::GetInternalHeap(size_t additional) {
  return VirtualHeapAllocator::Map(nullptr, sizeof(ChunkHeader) * kChunkHeaderAllocatableCount + additional,
                                   VirtualHeapAllocator::Prot::WRITE | VirtualHeapAllocator::Prot::READ,
                                   VirtualHeapAllocator::Flags::ANONYMOUS | VirtualHeapAllocator::Flags::PRIVATE);
}


// Allocate from ChunkHeader heap,
// If specified size is smaller than ChunkHeader::kChunkMaxAllocatableSmallObjectSize,
// return new heap, if not return nullptr.
YATSC_INLINE void* LocalArena::Allocate(size_t size) YATSC_NOEXCEPT {
  ChunkHeader* header = AllocateIfNecessary(size);
  if (header != nullptr) {
    return header->Distribute();
  }
  return nullptr;
}


// Update internal heap.
YATSC_INLINE void LocalArena::ResetPool() {
  auto internal_heap = reinterpret_cast<InternalHeap*>(CentralArena::GetInternalHeap(sizeof(InternalHeap)));
  internal_heap->set_next(internal_heap_);
  internal_heap_ = internal_heap;
}
}}

#endif
