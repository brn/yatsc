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


#ifndef UTILS_MMAP_VIRTUAL_ALLOC_H_
#define UTILS_MMAP_VIRTUAL_ALLOC_H_

#include <windows.h>
#include "./utils.h"

namespace yatsc {

class MapAllocator : private Static {
 public:
  static YATSC_INLINE void* Allocate(size_t size) {
    void* heap = VirtualAlloc(NULL, size, MEM_COMMIT, PAGE_READWRITE);
    if (heap == NULL) {
      std::string st;
      GetLastError(&st);
      FATAL("Failed to VirtualAlloc.\nReason: " << st.c_str());
    }
    return heap;
  }


  static YATSC_INLINE void Deallocate(void* area, size_t size) {
    VirtualFree(area, size, MEM_DECOMMIT);
  }
};

}

#endif
