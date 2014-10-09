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


#ifndef MEMORY_VIRTUAL_HEAP_ALLOCATOR_H
#define MEMORY_VIRTUAL_HEAP_ALLOCATOR_H

#include "../utils/utils.h"

namespace yatsc {

class VirtualHeapAllocator: private Static {
 public:
  
  // The mmap prot arguments enum.
  class Prot: private Static {
   public:
    static uint8_t NONE;
    static uint8_t READ;
    static uint8_t WRITE;
    static uint8_t EXEC;
  };


  class Flags: private Static {
   public:
    static uint8_t NONE;
    static uint8_t ANONYMOUS;
    static uint8_t SHARED;
    static uint8_t PRIVATE;
    static uint8_t FIXED;
  };


  class Type: private Static {
   public:
    static uint8_t COMMIT;
    static uint8_t DECOMMIT;
    static uint8_t RESERVE;
    static uint8_t RELEASE;
  };


  // Reserve or commit new virtual memory block.
  inline static void* Map(void* addr, size_t size, uint8_t prot, uint8_t flags, uint8_t type = Type::RESERVE);

  // Unmap reserved memory block.
  inline static void Unmap(void* addr, size_t size, uint8_t type = Type::RELEASE);

 private:
  static std::atomic<uint64_t> used_;
};
}


#ifdef HAVE_MMAP
#include "./virtual-heap-allocator-posix-inl.h"
#elif defined(HAVE_VIRTUALALLOC)
#include "./virtual-heap-allocator-win32-inl.h"
#endif

#endif // MEMORY_VIRTUAL_HEAP_ALLOCATOR_H
