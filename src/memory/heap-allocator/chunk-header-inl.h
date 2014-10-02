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


namespace yatsc { namespace heap {

// Allocate a new memory from the free list and remove front of the free list.
// If the free list is empty, allocate a new memory that is aligned 1MB from virtual memory.
// This method is thread safe.
YATSC_INLINE void* ChunkHeader::Distribute() {
  if (free_list_.load() == nullptr) {
    // If heap is not filled.
    if (heap_list_->used() < max_allocatable_size_) {
      // Calc next positon.
      Byte* block = reinterpret_cast<Byte*>(heap_list_) + heap_list_->used();
      // Update heap used value.
      heap_list_->set_used(heap_list_->used() + size_class_);
      return block;
    } else {
      // If heap is exhausted allocate new memory.
      Byte* block = InitHeap(AllocateBlock());
      auto heap_header = reinterpret_cast<HeapHeader*>(block - sizeof(HeapHeader));
      heap_header->set_used(size_class_);
      return block;
    }
  }
  
  // Swap free list.
  auto free_list_head = free_list_.load(std::memory_order_acquire);
  while (!free_list_.compare_exchange_weak(free_list_head, free_list_head->next())) {}
  return reinterpret_cast<void*>(free_list_head);
}


// Deallocate memory.
// This method only add memory to the free list.
// The given block must be a return value of the Distribute().
// This method is thread safe.
YATSC_INLINE void ChunkHeader::Dealloc(void* block) {
  // Simply cast block to FreeHeader,
  // because this memory block is unused until detached from free list.
  FreeHeader* free_header = reinterpret_cast<FreeHeader*>(block);
  free_header->set_next(free_list_.load(std::memory_order_acquire));
  
  FreeHeader* free_list_head = free_list_.load(std::memory_order_relaxed);
  while (!free_list_.compare_exchange_weak(free_list_head, free_header)) {
    free_header->set_next(free_list_head);
  }
}


// Append an allocated memory block to the heap_list_ property
// to be able to deallocate all block when ChunkHeader::Delete called.
YATSC_INLINE Byte* ChunkHeader::InitHeap(Byte* block) {
  // Create HeapHeader from block.
  HeapHeader* heap_header = new (block) HeapHeader(this);
  heap_header->set_used(sizeof(HeapHeader));
  ASSERT(true, heap_header->chunk_header() == this);
  
  // First memory block is located after HeapHeader.
  Byte* begin = block + sizeof(HeapHeader);
  
  // Append heap to heap_list_ to deallocate after all.
  if (heap_list_ == nullptr) {
    heap_list_ = heap_header;
  } else {
    heap_header->set_next(heap_list_);
    heap_list_ = heap_header;
  }
  
  return begin;
}


// Allocate new 1MB aligned memory block by AlignedHeapAllocator.
YATSC_INLINE Byte* ChunkHeader::AllocateBlock() {
  return reinterpret_cast<Byte*>(AlignedHeapAllocator::Allocate(64 KB, kAlignment));
}

}} // yatsc::heap
