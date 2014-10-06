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


#ifndef MEMORY_HEAP_ALLOCATOR_ARENA_H 
#define MEMORY_HEAP_ALLOCATOR_ARENA_H

#include <vector>
#include "../aligned-heap-allocator.h"
#include "./chunk-header.h"
#include "../../utils/spinlock.h"
#include "../../utils/utils.h"
#include "../../utils/tls.h"
#include "../../utils/intrusive-rbtree.h"

namespace yatsc {namespace heap {
class LocalArena;

static size_t kChunkHeaderAllocatableCount = 70;

class LargeHeader: public RbTreeNode<size_t, LargeHeader*> {
 public:
  LargeHeader(size_t size)
      : size_(size) {}
  
  YATSC_CONST_PROPERTY(size_t, size, size_);
 private:
  size_t size_;
};

typedef std::atomic<LocalArena*> AtomicLocalArena;

class CentralArena {
 public:
  CentralArena();
  ~CentralArena();

  YATSC_INLINE void* Allocate(size_t size) YATSC_NOEXCEPT;

  
  YATSC_INLINE void Dealloc(void* ptr) YATSC_NOEXCEPT;

  
  YATSC_INLINE void Dealloc(volatile void* ptr) YATSC_NOEXCEPT;


  static void* GetInternalHeap(size_t additional = 0);

  
  YATSC_INLINE void NotifyLockAcquired() YATSC_NOEXCEPT;

  
  YATSC_INLINE void NotifyLockReleased() YATSC_NOEXCEPT;
  
 private:

  YATSC_INLINE LocalArena* GetLocalArena() YATSC_NOEXCEPT;

  YATSC_INLINE LocalArena* FindUnlockedArena() YATSC_NOEXCEPT;

  YATSC_INLINE LocalArena* StoreNewLocalArena() YATSC_NOEXCEPT;

  YATSC_INLINE void* AllocateLargeObject(size_t size) YATSC_NOEXCEPT;
  
  AtomicLocalArena local_arena_;
  ThreadLocalStorage::Slot* tls_;
  LazyInitializer<ThreadLocalStorage::Slot> tls_once_init_;
  IntrusiveRbTree<size_t, LargeHeader*> large_bin_;
  SpinLock lock_;
  std::atomic_int released_arena_count_;
};


class LocalArena {
 public:
  LocalArena(CentralArena* arena, Byte* block)
      : central_arena_(arena),
        used_internal_heap_(0),
        memory_block_(block) {}

  ~LocalArena();

  YATSC_INLINE void* Allocate(size_t size) YATSC_NOEXCEPT;

  
  YATSC_INLINE bool AcquireLock() YATSC_NOEXCEPT {
    bool ret = !lock_.test_and_set();
    if (ret) {
      central_arena_->NotifyLockAcquired();
    }
    return ret;
  }

  
  YATSC_INLINE void ReleaseLock() YATSC_NOEXCEPT {
    lock_.clear();
    central_arena_->NotifyLockReleased();
  }


  LocalArena* next() YATSC_NOEXCEPT {
    return next_.load(std::memory_order_acquire);
  }


  void set_next(LocalArena* arena) YATSC_NOEXCEPT {
    next_.store(arena, std::memory_order_release);
  }
  
  
 private:

  YATSC_INLINE ChunkHeader* AllocateIfNecessary(size_t size) YATSC_NOEXCEPT;

  
  YATSC_INLINE void ResetPool();

  CentralArena* central_arena_;
  size_t used_internal_heap_;
  Byte* memory_block_;
  std::atomic_flag lock_;
  AtomicLocalArena next_;
  IntrusiveRbTree<size_t, ChunkHeader*> small_bin_;
};
}}

#include "./arena-inl.h"

#endif // MEMORY_HEAP_ALLOCATOR_ARENA_H
