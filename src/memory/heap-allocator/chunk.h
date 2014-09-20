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


#ifndef MEMORY_HEAP_ALLOCATOR_CHUNK_HEADER_H
#define MEMORY_HEAP_ALLOCATOR_CHUNK_HEADER_H

#include "./aligned-heap-allocator.h"

namespace yatsc { namespace heap {

class ChunkHeader {
 public:
  enum class Color: uint8_t {
    RED = 0,
    BLACK
  };
  
  static Chunk* New(size_t size);

  YATSC_CONST_PROPERTY(size, size_, size_t);

  YATSC_CONST_PROPERTY(size_class, size_class_, size_t);
  
 private:

  class BinaryHeap {
   public:
    void Insert(HeapHeader* header);
    HeapHeader* header() {return tree_.size() > 0? tree_[0]: nullptr;};
    void Update();
   private:
    std::vector<HeapHeader*> tree_;
  };
  

  class HeapHeader {
   public:

    YATSC_PROPERTY(left, left_, HeapHeader*);
    YATSC_PROPERTY(right, right_, HeapHeader*);
    YATSC_GETTER(chunk_header, chunk_header_, ChunkHeader*);
    YATSC_PROPERTY(used, used_, size_t);
    
   private:
    ChunkHeader* chunk_header_;
    size_t used_;
  };
  
  
  size_t size_;
  size_t size_class_;
  Color color_;
  BinaryHeap binary_heap_;
};

}}

#endif // MEMORY_HEAP_ALLOCATOR_CHUNK_HEADER_H
