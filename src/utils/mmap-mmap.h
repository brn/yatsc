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


#ifndef UTILS_MMAP_MMAP_H_
#define UTILS_MMAP_MMAP_H_


#include <sys/mman.h>
#include "./utils.h"

#ifdef HAVE_VM_MAKE_TAG
#include <mach/vm_statistics.h>
#define FD VM_MAKE_TAG(255)
#else
#define FD -1
#endif


#ifdef HAVE_VM_PROTECT
#include <mach/mach.h>
#include <mach/mach_traps.h>
namespace yatsc {
YATSC_INLINE void VmProtect(void* addr, size_t size) {
  kern_return_t ret = vm_protect(
      mach_task_self(),
      reinterpret_cast<vm_address_t>(addr),
      static_cast<vm_size_t>(size),
      false,
      VM_PROT_READ | VM_PROT_WRITE);
  
  if (ret == KERN_PROTECTION_FAILURE) {
    FATAL("[mmap]Memory protection failed.");
  } else if (ret == KERN_INVALID_ADDRESS) {
    FATAL("[mmap]Memory address is invalid.");
  }
}
}
#else
namespace yatsc {
YATSC_INLINE void VmProtect(void* addr, size_t size) {}
}
#endif


namespace yatsc {

class MapAllocator : private Static {
 public:
  static YATSC_INLINE void* Allocate(size_t size) {
    void* heap = mmap(0, size, PROT_READ | PROT_WRITE, MAP_ANON | MAP_PRIVATE, FD, 0);
    if (heap == MAP_FAILED) {
      std::string message;
      Strerror(&message, errno);
      FATAL(message.c_str());
    }
    return heap;
  }


  static YATSC_INLINE void Deallocate(void* area, size_t size) {
    munmap(area, size);
  }
};

}

#endif
