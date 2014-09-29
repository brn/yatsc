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


namespace yatsc {namespace heap {
inline CentralArena::~CentralArena() {
  LocalArena* current = local_arena_;
  
  while (1) {
    LocalArena* tmp = current->next();
    VirtualHeapAllocator::Unmap(tmp, 1);
    if (tmp == nullptr) {
      break;
    }
    current = tmp;
  }
}


YATSC_INLINE void* CentralArena::Allocate(size_t size) YATSC_NOEXCEPT {
  LocalArena* arena = GetLocalArena();
  return arena->Allocate(YATSC_ALIGN_OFFSET(size, yatsc::kAlignment));
}


YATSC_INLINE void CentralArena::Dealloc(void* ptr) YATSC_NOEXCEPT {
  auto h = reinterpret_cast<HeapHeader*>(reinterpret_cast<uintptr_t>(ptr) & ChunkHeader::kAddrMask);
  ChunkHeader* chunk_header = h->chunk_header();
  chunk_header->Dealloc(ptr);
}


YATSC_INLINE void CentralArena::Dealloc(volatile void* ptr) YATSC_NOEXCEPT {
  Dealloc(const_cast<void*>(ptr));
}


YATSC_INLINE LocalArena* CentralArena::GetLocalArena() YATSC_NOEXCEPT {
  void* ptr = tls_->Get();
  if (ptr == nullptr) {
    return FindUnlockedArena();
  }
  return reinterpret_cast<LocalArena*>(ptr);
}


YATSC_INLINE LocalArena* CentralArena::FindUnlockedArena() YATSC_NOEXCEPT {
  if (local_arena_.load(std::memory_order_acquire) == nullptr) {
    return StoreNewLocalArena();
  }

  LocalArena* current = local_arena_;
  
  while (1) {
    if (current->AcquireLock()) {
      return current;
    }
    current = current->next();
  }

  if (current == nullptr) {
    current = StoreNewLocalArena();
  }

  return current;
}


YATSC_INLINE LocalArena* CentralArena::StoreNewLocalArena() YATSC_NOEXCEPT {
  Byte* block = reinterpret_cast<Byte*>(VirtualHeapAllocator::Map(
      nullptr,
      sizeof(ChunkHeader) * kMaxSmallObjectCount + sizeof(LocalArena),
      VirtualHeapAllocator::Prot::WRITE,
      VirtualHeapAllocator::Flags::ANONYMOUS | VirtualHeapAllocator::Flags::PRIVATE));
  
  LocalArena* arena = new (block) LocalArena(block + sizeof(LocalArena));

  LocalArena* arena_head = local_arena_.load(std::memory_order_relaxed);
  arena->set_next(arena_head);
  
  while (local_arena_.compare_exchange_weak(arena_head, arena)) {
    arena->set_next(arena_head);
  }

  tls_->Set(arena);
  return arena;
}


YATSC_INLINE void* LocalArena::Allocate(size_t size) YATSC_NOEXCEPT {
  return AllocateIfNecessary(size)->Distribute();
}


YATSC_INLINE ChunkHeader* LocalArena::AllocateIfNecessary(size_t size) YATSC_NOEXCEPT {
  const size_t index = (size / yatsc::kAlignment) - 1;
  if (index < kMaxSmallObjectCount) {
    ChunkHeader* chunk_header = small_bin_.Find(size);
    if (chunk_header == nullptr) {
      chunk_header = ChunkHeader::New(size, reinterpret_cast<ChunkHeader*>(memory_block_) + index);
      small_bin_.Insert(size, chunk_header);
    }
    return chunk_header;
  }
  return AllocateLargeObject(size);
}


YATSC_INLINE ChunkHeader* LocalArena::AllocateLargeObject(size_t size) YATSC_NOEXCEPT {
  ChunkHeader* chunk_header = large_bin_.Find(size);
  if (chunk_header == nullptr) {
    void* heap = VirtualHeapAllocator::Map(nullptr, size + sizeof(ChunkHeader),
                                           VirtualHeapAllocator::Prot::WRITE | VirtualHeapAllocator::Prot::READ,
                                           VirtualHeapAllocator::Flags::ANONYMOUS | VirtualHeapAllocator::Flags::PRIVATE);
    chunk_header = ChunkHeader::NewLarge(size, heap);
    large_bin_.Insert(size, chunk_header);
  }
  return chunk_header;
}
}}
