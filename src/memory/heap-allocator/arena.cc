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

#include "./arena.h"


namespace yatsc {namespace heap {

// Release lock of the thread local storage when thread is finished.
inline void TlsFree(void* arena) {
  auto local_arena = reinterpret_cast<LocalArena*>(arena);
  local_arena->ReleaseLock();
}


CentralArena::CentralArena() {
  local_arena_.store(nullptr, std::memory_order_relaxed);
  tls_ = tls_once_init_(&TlsFree);
  released_arena_count_.store(0, std::memory_order_relaxed);
}


// Unmap all heap of LocalArena and unmap all LocalArena.
CentralArena::~CentralArena() {
  LocalArena* current = local_arena_;
  
  while (1) {
    LocalArena* tmp = current->next();
    current->~LocalArena();
    VirtualHeapAllocator::Unmap(current, 1);
    if (tmp == nullptr) {
      break;
    }
    current = tmp;
  }
}


// Search released arena from arena list,
// if released_arena_count_ is zero, simply allocate new LocalArena.
// If lock is acquired, return that arena,
// but not, allocate new arena.
LocalArena* CentralArena::FindUnlockedArena() YATSC_NOEXCEPT {
  // If released_arena_count_ is zero,
  // that mean free arena is not exists.
  // So we simply allocate new LocalArena.
  if (released_arena_count_.load() == 0) {
    return StoreNewLocalArena();
  }
  
  LocalArena* current = local_arena_.load(std::memory_order_relaxed);
  
  while (current != nullptr) {
    // Released arena is found.
    if (current->AcquireLock()) {
      return current;
    }
    current = current->next();
  }

  // If released arena is not found,
  // allocate new LocalArena.
  return StoreNewLocalArena();
}


// Allocate new LocalArena from the internal heap.
// All allocated LocalArena is prepend to CentralArena::local_arena_ linked list by atomic.
// We simply do CAS operation because ABA problem is never occur in this case.
LocalArena* CentralArena::StoreNewLocalArena() YATSC_NOEXCEPT {
  Byte* block = reinterpret_cast<Byte*>(GetInternalHeap(sizeof(LocalArena) + sizeof(InternalHeap)));
  
  LocalArena* arena = new (block) LocalArena(this, new(block + sizeof(LocalArena)) InternalHeap(block + sizeof(LocalArena) + sizeof(InternalHeap)));
  arena->AcquireLock();

  LocalArena* arena_head = local_arena_.load(std::memory_order_acquire);

  // Run CAS operation.
  // This case ABA problem is not the matter,
  // because the local_arena_ allowed only one-way operation that is append new LocalArena.
  // So head of local_arena_ is change only when new local arena is appended.
  do {
    arena->set_next(arena_head);
  } while (!local_arena_.compare_exchange_weak(arena_head, arena));

  return arena;
}


// Allocate new large heap.
// The large heap is directly allocted from VirtualMemory and managed by CentralArena.
// The returned heap has odd address to distinct small object pointer when deallocate.
void* CentralArena::AllocateLargeObject(size_t size) YATSC_NOEXCEPT {
  // This method is not thread safe,
  // so simply lock.
  ScopedSpinLock lock(lock);
  
  LargeHeader* large_header = large_bin_.Find(size);
  
  // If large_header is never allocated.
  void* heap = VirtualHeapAllocator::Map(nullptr, size + sizeof(LargeHeader),
                                         VirtualHeapAllocator::Prot::WRITE | VirtualHeapAllocator::Prot::READ,
                                         VirtualHeapAllocator::Flags::ANONYMOUS | VirtualHeapAllocator::Flags::PRIVATE);
  auto new_large_header = new(heap) LargeHeader(size);

  if (large_header != nullptr) {
    new_large_header->set_next(large_header);
  }

  large_bin_.Insert(size, new_large_header);
  return reinterpret_cast<void*>(reinterpret_cast<uintptr_t>(reinterpret_cast<Byte*>(new_large_header) + sizeof(LargeHeader)) | 1);
}


// Remove all ChunkHeader.
LocalArena::~LocalArena() {
  auto current = internal_heap_;
  while (current != nullptr) {
    auto header = reinterpret_cast<ChunkHeader*>(current->block());
    for (size_t i = 0; i < current->used(); i++) {
      ChunkHeader::Delete(header);
      header = (header + i);
    }
    auto tmp = current->next();
    VirtualHeapAllocator::Unmap(current, sizeof(ChunkHeader) * kChunkHeaderAllocatableCount + sizeof(InternalHeap));
    current = tmp;
  }
}


// Allocate new ChunkHeader if not yet allocated,
// ChunkHeader is allocated from the internal_heap_ of LocalArena.
ChunkHeader* LocalArena::AllocateIfNecessary(size_t size) YATSC_NOEXCEPT {
  if (size < ChunkHeader::kChunkMaxAllocatableSmallObjectSize) {
    ChunkHeader* chunk_header = small_bin_.Find(size);
    if (chunk_header == nullptr) {
      // If allocated size is overed internal heap max size,
      // allocate new internal heap.
      if ((internal_heap_->used() + 1) == kChunkHeaderAllocatableCount) {
        ResetPool();
      }
      
      chunk_header = ChunkHeader::New(size, reinterpret_cast<ChunkHeader*>(internal_heap_->block()) + internal_heap_->used());
      internal_heap_->use();
      
      small_bin_.Insert(size, chunk_header);
    }
    return chunk_header;
  }
  return nullptr;
}

}}
