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

#include <atomic>
#include "../../utils/utils.h"
#include "../virtual-heap-allocator.h"
#include "../aligned-heap-allocator.h"

namespace yatsc { namespace heap {

// This class represent memory block header class
// to allocate memory and deallocate memory.
// The chunk header is used to manage all linked memory block,
//
// [[ChunkHeader(size_class,etc...)][HeapHeader-------]->[HeapHeader-------]->...]
//   ^---------<--reference--<-------^
//   ^-----------------<--reference--<--------------------^
//
// All memory block that is allocated is aligned 1 MB,
// so we decide HeapHeader address from an each heap by bitwise_and with kAddrMask.
// The ChunkHeader class self is managed by Red-Black-Tree,
// all rbtree property like color, left and right is embeded in this class.
class ChunkHeader {
  class FreeHeader;
  typedef std::atomic<FreeHeader*> AtomicFreeList;
 public:

  // Red-Black-Tree Color property.
  enum class Color: uint8_t {
    kRed = 0,
    kBlack
  };


  ~ChunkHeader() = default;
  

  // The factory for to create new ChunkHeader for given size_class.
  static ChunkHeader* New(size_t size_class);


  // Delete ChunkHeader.
  static void Delete(ChunkHeader* chunk_header);


  // Allocate a new memory block that is sized by size_class_ property.
  // The return value of this method must not be given free and delete.
  YATSC_INLINE void* Distribute();


  // Release allocated memory block.
  // The argument must be the return value of the Distribute method.
  void Dealloc(void* area);


  // Getter and setter for size_class_.
  // size_class_ mean is allocation target size.
  YATSC_CONST_PROPERTY(size_t, size_class, size_class_);


  // The header for the each heap.
  // Heap header has reference to ChunkHeader and embeded linked list ptr.
  class HeapHeader {
   public:
    HeapHeader(ChunkHeader* chunk_header)
        : chunk_header_(chunk_header),
          next_(nullptr) {}

    // Getter for ChunkHeader.
    YATSC_GETTER(ChunkHeader*, chunk_header, chunk_header_);

    
    // Getter and setter for next pointer.
    YATSC_PROPERTY(HeapHeader*, next, next_);


    YATSC_PROPERTY(size_t, used, used_);
    
   private:
    
    ChunkHeader* chunk_header_;
    HeapHeader* next_;
    size_t used_;
  };


  // The mask that is calculate front of the memory block address.
  static const size_t kAddrMask;
  
 private:


  // Header that is used on unused memory block.
  // This header is not use extra memory but
  // locate in heap block body.
  class FreeHeader {
   public:
    YATSC_PROPERTY(FreeHeader*, next, next_);

   private:
    FreeHeader* next_;
  };
  

  // Simple new instantiation is not allowed.
  // Instead use ChunkHeader::New
  ChunkHeader(size_t size_class)
      : size_class_(size_class),
        color_(ChunkHeader::Color::kBlack),
        heap_list_(nullptr) {
    free_list_.store(nullptr, std::memory_order_relaxed);
  }
  

  // Initialize HeapHeader.
  YATSC_INLINE Byte* InitHeap(Byte* block);


  // Allocate new memory block.
  static YATSC_INLINE Byte* AllocateBlock();
  
  
  size_t size_class_;
  Color color_;
  AtomicFreeList free_list_;
  HeapHeader* heap_list_;
  
  static const size_t kAlignment;
};

}}

#include "./chunk-header-inl.h"

#endif // MEMORY_HEAP_ALLOCATOR_CHUNK_HEADER_H
