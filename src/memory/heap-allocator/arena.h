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
#include '../aligned-heap-allocator.h'
#include '../../utils/utils.h'

namespace yatsc {namespace heap {
class LocalArena;

class CentralArena {
  typedef std::atomic<LocalArena*> AtomicLocalArenaList;
 public:
  CentralArena();
  ~CentralArena();

  YATSC_INLINE LocalArena* GetLocalArena();
  
 private:

  YATSC_INLINE LocalArena* FindUnlockedArena();

  YATSC_INLINE void StoreNewLocalArena();
  
  AtomicLocalArenaList local_arena_;
  ThreadLocalStorage::Slot* tls_;
  LazyInitializer<ThreadLocalStorage::Slot> tls_once_init_;
};


class LocalArena {
 public:
  LocalArena(CentralArena* central, Byte* block)
      : central_arena_(central),
        memory_block_(block) {}
  
  ~LocalArena();

  YATSC_INLINE Chunk* AllocateIfNecessary(size_t size);
  
  size_t FindJustFitBlockIndex(size_t size);

  
  YATSC_INLINE bool AcquireLock() YATSC_NOEXCEPT {
    return !lock_.test_and_set();
  }

  
  YATSC_INLINE void ReleaseLock() YATSC_NOEXCEPT {
    lock_.clear();
  }


  YATSC_PROPERTY(LocalArena* next, next_);
  
 private:

  CentralArena* central_arena_;
  Byte* memory_block_;
  std::atomic_flag lock_;
  LocalArena* next_;
};
}}

#endif // MEMORY_HEAP_ALLOCATOR_ARENA_H
