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
Byte kChunkHeaderStack[sizeof(ChunkHeader) * kMaxSmallObjectCount] = {0};


// Delete ChunkHeader.
// Deallocate all heaps.
void ChunkHeader::Delete(ChunkHeader* chunk_header) {
  HeapHeader* heap_header = chunk_header->heap_list_;
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
ChunkHeader* ChunkHeader::New(size_t size_class, void* ptr) {
  Byte* block = AllocateBlock(size_class);

  ChunkHeader* header = new (ptr) ChunkHeader(size_class);

  // Add memory block to free list.
  header->InitHeap(block);
  return header;
}


// Create a new ChunkeHeader and allocate memory block
// that allocated from mmap.
ChunkHeader* ChunkHeader::NewLarge(size_t size, void* ptr) {
  Byte* block = reinterpret_cast<Byte*>(ptr);
  ChunkHeader* header = new (block) ChunkHeader(size);

  // Add memory block to free list.
  header->InitHeap(block + sizeof(ChunkHeader));
  return header;
}


// Bit mask to calcurate HeapHeader address from each small heap.
const size_t ChunkHeader::kAddrMask = 0xFFFFFF00;

// Memory alignment.
const size_t ChunkHeader::kAlignment = 4 KB;
}}
