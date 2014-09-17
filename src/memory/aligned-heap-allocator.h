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


#ifndef MEMORY_ALIGNED_HEAP_ALLOCATOR_H
#define MEMORY_ALIGNED_HEAP_ALLOCATOR_H

#include "../utils/utils.h"

namespace yatsc {
// Allocate algined memory block that is not restricted to CPU alignment.
// This allocator purpose is allocate the memory that has computable address like 0x1C9 00000...
class AlignedHeapAllocator: private Static {
 public:
  // Allocate aligned memory block.
  static void* Allocate(size_t alignement);

  
  // Deallocate allocated memory block.
  static void Deallocate(void* addr);
};
}

#endif // MEMORY_ALIGNED_HEAP_ALLOCATOR_H
