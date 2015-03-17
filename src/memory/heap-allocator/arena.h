//
// The MIT License (MIT)
//
// Copyright (c) 2013 Taketoshi Aono(brn)
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.


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

// The minimum number of the allowed ChunkHeader allocated size.
static size_t kChunkHeaderAllocatableCount = 70;


// Large heap header that manage each large heap block.
class LargeHeader: public RbTreeNode<size_t, LargeHeader*> {
 public:
  LargeHeader(size_t size)
      : size_(size),
        next_(nullptr) {}
  
  YATSC_CONST_PROPERTY(size_t, size, size_)


  YATSC_PROPERTY(LargeHeader*, next, next_)
 private:
  size_t size_;
  LargeHeader* next_;
};


// Local arena linked list type.
typedef std::atomic<LocalArena*> AtomicLocalArena;


// The central arena. This arena controll all LocalArena.
// [MainThread][CentralArena]
//   [Thread1]->[LocalArena]
//   [Thread2]->[LocalArena]
//   [Thread3]->[LocalArena]
// ...
// All LocalArena that held by each thread is allocated from CentralArena
// and managed by CentralArena.
class CentralArena {
 public:
  CentralArena();
  ~CentralArena();


  // Allocate new heap space that has the specified size.
  YATSC_INLINE void* Allocate(size_t size) YATSC_NOEXCEPT;


  // Deallocate heap that allocated by Allocate(size_t).
  YATSC_INLINE void Dealloc(void* ptr) YATSC_NOEXCEPT;


  // Specialized method for volatile pointer.
  YATSC_INLINE void Dealloc(volatile void* ptr) YATSC_NOEXCEPT;


  // Allocate ChunkHeader heap that has the specified size.
  YATSC_INLINE static void* GetInternalHeap(size_t additional = 0);


  // If LocalArena lock is acquired, change released_arena_count_ state.
  YATSC_INLINE void NotifyLockAcquired() YATSC_NOEXCEPT;


  // If LocalArena lock is released, change released_arena_count_ state.
  YATSC_INLINE void NotifyLockReleased() YATSC_NOEXCEPT;
  
 private:

  // Return thread specific LocalArena.
  YATSC_INLINE LocalArena* GetLocalArena() YATSC_NOEXCEPT;
  

  // Search and return unlocked arena.
  LocalArena* FindUnlockedArena() YATSC_NOEXCEPT;
  

  // Create new LocalArena.
  LocalArena* StoreNewLocalArena() YATSC_NOEXCEPT;
  

  // Allocate Large size heap that has specified size.
  void* AllocateLargeObject(size_t size) YATSC_NOEXCEPT;
  
  
  AtomicLocalArena local_arena_;
  LazyInitializer<ThreadLocalStorage::Slot> tls_;
  IntrusiveRbTree<size_t, LargeHeader*> large_bin_;
  SpinLock lock_;
  std::atomic_int released_arena_count_;
};


// The ChunkHeader heap zone information.
class InternalHeap {
 public:
  InternalHeap(Byte* memory_block)
      : memory_block_(memory_block),
        next_(nullptr),
        used_(0) {}


  // Getter and setter for memory_block_.
  YATSC_PROPERTY(Byte*, block, memory_block_)
  

  // Getter and setter for next_.
  YATSC_PROPERTY(InternalHeap*, next, next_)


  // Increment used block count.
  YATSC_INLINE void use() {
    used_++;
  }


  // Return current allocated ChunkHeader count.
  YATSC_INLINE size_t used() {
    return used_;
  }
  
 private:
  Byte* memory_block_;
  InternalHeap* next_;
  size_t used_;
};


// The thread specific arena.
class LocalArena {
 public:
  LocalArena(CentralArena* arena, InternalHeap* internal_heap)
      : central_arena_(arena),
        internal_heap_(internal_heap) {}

  ~LocalArena();


  // Allocate new heap that has specified size.
  YATSC_INLINE void* Allocate(size_t size) YATSC_NOEXCEPT;


  // Lock this arena if not locked and
  // notify central arena that locked.
  YATSC_INLINE bool AcquireLock() YATSC_NOEXCEPT {
    bool ret = !lock_.test_and_set();
    if (ret) {
      central_arena_->NotifyLockAcquired();
    }
    return ret;
  }


  // Unlock this arena and
  // notify central arena that unlocked.
  YATSC_INLINE void ReleaseLock() YATSC_NOEXCEPT {
    lock_.clear();
    central_arena_->NotifyLockReleased();
  }


  // Get next LocalArena pointer.
  YATSC_INLINE LocalArena* next() YATSC_NOEXCEPT {
    return next_.load();
  }


  // Set next LocalArena pointer.
  YATSC_INLINE void set_next(LocalArena* arena) YATSC_NOEXCEPT {
    next_.store(arena);
  }
  
  
 private:

  // Return ChunkHeader that is already allocated or new allocated.
  ChunkHeader* AllocateIfNecessary(size_t size) YATSC_NOEXCEPT;


  // Reallocate ChunkHeader heap space.
  YATSC_INLINE void ResetPool();

  CentralArena* central_arena_;
  InternalHeap* internal_heap_;
  std::atomic_flag lock_;
  AtomicLocalArena next_;
  IntrusiveRbTree<size_t, ChunkHeader*> small_bin_;
};
}}

#include "./arena-inl.h"

#endif // MEMORY_HEAP_ALLOCATOR_ARENA_H
