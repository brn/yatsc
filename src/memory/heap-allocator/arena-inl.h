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

YATSC_INLINE void* CentralArena::Allocate(size_t size) YATSC_NOEXCEPT {
  LocalArena* arena = GetLocalArena();
  const size_t actual_size = YATSC_ALIGN_OFFSET(size, yatsc::kAlignment);
  void* ret = arena->Allocate(actual_size);
  if (ret != nullptr) {
    return ret;
  }
  return AllocateLargeObject(actual_size);
}


YATSC_INLINE void CentralArena::Dealloc(void* ptr) YATSC_NOEXCEPT {
  ASSERT(true, ptr != nullptr);
  if ((reinterpret_cast<uintptr_t>(ptr) & 1) == 1) {
    auto area = reinterpret_cast<Byte*>(reinterpret_cast<uintptr_t>(ptr) & ~1);
    auto large_header = reinterpret_cast<LargeHeader*>(reinterpret_cast<Byte*>(area) - sizeof(LargeHeader));
    lock_.lock();
    large_bin_.Delete(large_header->size());
    lock_.unlock();
    VirtualHeapAllocator::Unmap(large_header, large_header->size());
  } else {
    auto h = reinterpret_cast<HeapHeader*>(reinterpret_cast<uintptr_t>(ptr) & ChunkHeader::kAddrMask);
    ASSERT(true, h != nullptr);
    ChunkHeader* chunk_header = h->chunk_header();
    ASSERT(true, chunk_header != nullptr);
    chunk_header->Dealloc(ptr);
  }
}


YATSC_INLINE void CentralArena::Dealloc(volatile void* ptr) YATSC_NOEXCEPT {
  Dealloc(const_cast<void*>(ptr));
}


YATSC_INLINE void CentralArena::NotifyLockAcquired() YATSC_NOEXCEPT {
  if (released_arena_count_.load() == 0) {
    released_arena_count_.fetch_sub(1);
  }
}


YATSC_INLINE void CentralArena::NotifyLockReleased() YATSC_NOEXCEPT {
  released_arena_count_.fetch_add(1);
}


YATSC_INLINE LocalArena* CentralArena::GetLocalArena() YATSC_NOEXCEPT {
  void* ptr = tls_->Get();
  if (ptr == nullptr) {
    ptr = FindUnlockedArena();
    tls_->Set(ptr);
  }
  return reinterpret_cast<LocalArena*>(ptr);
}


YATSC_INLINE void* CentralArena::GetInternalHeap(size_t additional) {
  return VirtualHeapAllocator::Map(nullptr, sizeof(ChunkHeader) * kChunkHeaderAllocatableCount + additional,
                                   VirtualHeapAllocator::Prot::WRITE | VirtualHeapAllocator::Prot::READ,
                                   VirtualHeapAllocator::Flags::ANONYMOUS | VirtualHeapAllocator::Flags::PRIVATE);
}


YATSC_INLINE void* LocalArena::Allocate(size_t size) YATSC_NOEXCEPT {
  ChunkHeader* header = AllocateIfNecessary(size);
  if (header != nullptr) {
    return header->Distribute();
  }
  return nullptr;
}


YATSC_INLINE void LocalArena::ResetPool() {
  auto internal_heap = reinterpret_cast<InternalHeap*>(CentralArena::GetInternalHeap(sizeof(InternalHeap)));
  internal_heap->set_next(internal_heap_);
  internal_heap_ = internal_heap;
}
}}

#endif
