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
Byte kChunkHeaderStack[sizeof(ChunkHeader) * 300] = {0};

ChunkHeader::~ChunkHeader() {
  HeapHeader* heap_header = heap_list_;
  while (heap_header != nullptr) {
    HeapHeader* next = heap_header->next();
    AlignedHeapAllocator::Deallocate(heap_header);
    heap_header = next;
  }
}


ChunkHeader* ChunkHeader::New(size_t size_class) {
  static const size_t kChunkHeaderSize = sizeof(ChunkHeader);
  Byte* block = AllocateBlock();
  size_t index = size_class / yatsc::kAlignment;
  if (index == 1) {index = 0;}
  void* mem = kChunkHeaderStack + (index * kChunkHeaderSize);
  ChunkHeader* header = new (mem) ChunkHeader(size_class);
  header->AssignFreeList(block);
  return header;
}


void ChunkHeader::AssignFreeList(Byte* block) {
  HeapHeader* heap_header = new (block) HeapHeader(this);
  Byte* begin = block + sizeof(HeapHeader);
  FreeHeader* free_block = reinterpret_cast<FreeHeader*>(begin);
  FreeHeader* head = free_block;
  size_t used_size = size_class_;
  
  while (used_size < kAlignment) {
    free_block->set_next(reinterpret_cast<FreeHeader*>(begin + used_size));
    free_block = free_block->next();
    used_size += size_class_;
  }

  free_block->set_next(nullptr);
  
  if (free_list_ == nullptr) {
    free_list_ = head;
  } else {
    free_list_->set_next(head);
  }
  if (heap_list_ == nullptr) {
    heap_list_ = heap_header;
  } else {
    heap_list_->set_next(heap_header);
  }
}

const size_t ChunkHeader::kAddrMask = 0xFFFFFF00000;

const size_t ChunkHeader::kAlignment = 1 MB;
}}
