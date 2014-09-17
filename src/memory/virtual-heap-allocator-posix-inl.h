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

#include <sys/mman.h>


// Map virtual memory block with the mmap.
inline void* VirtualHeapAllocator::Map(void* addr, size_t size, uint8_t prot, uint8_t flags) {
  int mmap_prot = 0;
  int mmap_flags = 0;

  if (prot != VirtualHeapAllocator::Prot::NONE) {
    if ((prot & VirtualHeapAllocator::Prot::READ) == VirtualHeapAllocator::Prot::READ) {
      mmap_prot |= PROT_READ;
    }

    if ((prot & VirtualHeapAllocator::Prot::WRITE) == VirtualHeapAllocator::Prot::WRITE) {
      mmap_prot |= PROT_WRITE;
    }

    if ((prot & VirtualHeapAllocator::Prot::EXEC) == VirtualHeapAllocator::Prot::EXEC) {
      mmap_prot |= PROT_EXEC;
    }
  } else {
    mmap_prot = PROT_NONE;
  }

  
  if (flags != VirtualHeapAllocator::Flags::NONE) {
    if ((flags & VirtualHeapAllocator::Flags::ANONYMOUS) == VirtualHeapAllocator::Flags::ANONYMOUS) {
      mmap_flags |= MAP_ANON;
    }

    if ((flags & VirtualHeapAllocator::Flags::SHARED) == VirtualHeapAllocator::Flags::SHARED) {
      mmap_flags |= MAP_SHARED;
    }

    if ((flags & VirtualHeapAllocator::Flags::PRIVATE) == VirtualHeapAllocator::Flags::PRIVATE) {
      mmap_flags |= MAP_PRIVATE;
    }
    if (addr != nullptr) {
      mmap_flags |= MAP_FIXED;
    }
  }

  void* ret = mmap(addr, size, mmap_prot, mmap_flags, -1, 0);
  if (ret == NULL) {
    return nullptr;
  }
  return ret;
}


// Unmap virtual memory block with the munmap.
inline void VirtualHeapAllocator::Unmap(void* addr, size_t size) {
  munmap(addr, size);
}
