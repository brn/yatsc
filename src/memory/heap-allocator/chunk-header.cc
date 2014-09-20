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


#include "./chunk-header.h"

namespace yatsc { namespace heap {

// To use allocate ChunkHeader.
// Max memory size used by ChunkHeader is known. So we prepare on stack.
Byte kChunkHeaderStack[sizeof(ChunkHeader) * 300] = {0};


// ChunkHeader size.
static const size_t kChunkHeaderSize = sizeof(ChunkHeader);


// Delete ChunkHeader.
// Deallocate all heaps.
void ChunkHeader::Delete(ChunkHeader* chunk_header) {
  HeapHeader* heap_header = chunk_header->heap_list_;;
  HeapHeader* head = heap_header->next();
  
  if (head != nullptr) {
    heap_header = head;
  }
  
  while (heap_header != head) {
    HeapHeader* next = heap_header->next();
    AlignedHeapAllocator::Deallocate(heap_header);
    heap_header = next;
  }
  
  chunk_header->heap_list_ = nullptr;
  chunk_header->free_list_ = nullptr;
}


// Create a new ChunkeHeader and allocate memory block
// that is aligned by 1MB from virtual memmory.
// The ChunkHeader is allocated by the stack memory.
ChunkHeader* ChunkHeader::New(size_t size_class) {
  Byte* block = AllocateBlock();

  // Calc index from size_class.
  size_t index = size_class / yatsc::kAlignment;

  // If index is 1, this size class must be the first area of the kChunkHeaderStack,
  // so set to zero.
  if (index == 1) {index = 0;}
  
  // All ChunkHeader is divided by index, so we calculate and get
  // index on the kChunkHeaderSize.
  // This memory allocation is lock free, because ChunkHeader index is unique.
  void* mem = kChunkHeaderStack + (index * kChunkHeaderSize);
  ChunkHeader* header = new (mem) ChunkHeader(size_class);

  // Add memory block to free list.
  header->InitHeap(block);
  return header;
}


// Bit mask to calcurate HeapHeader address from each small heap.
const size_t ChunkHeader::kAddrMask = 0xFFFFFF00000;

// Memory alignment.
const size_t ChunkHeader::kAlignment = 1 MB;
}}
