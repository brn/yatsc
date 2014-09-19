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
// If free list is empty, allocate a new memory that is aligned 1MB from virtual memory.
// This method is thread safe.
YATSC_INLINE void* ChunkHeader::Distribute() {
  // Protect from race condition of the Dealloc.
  ScopedSpinLock lock(lock_);
  if (free_list_ == nullptr) {
    // If heap is not filled.
    if (heap_list_->used() < (kAlignment - size_class_)) {
      // Calc next positon.
      Byte* block = reinterpret_cast<Byte*>(heap_list_) + (sizeof(HeapHeader) + heap_list_->used());
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
  } else {
    // Swap free list.
    auto tmp = free_list_;
    free_list_ = tmp->next();
    return reinterpret_cast<void*>(tmp);
  }
}


// Deallocate memory.
// This method only add memory to the free list.
// The given block must be a return value of the Distribute().
// This method is thread safe.
YATSC_INLINE void ChunkHeader::Dealloc(void* block) {
  // Simply cast block to FreeHeader.
  // Because this memory block is unused until detached from free list.
  FreeHeader* free_header = reinterpret_cast<FreeHeader*>(block);

  // Protect free list.
  ScopedSpinLock lock(lock_);
  if (free_list_ == nullptr) {
    free_list_ = free_header;
    free_list_->set_next(nullptr);
  } else {
    // Free block is prepended to the free list,
    // because allocate-deallocate cycle is cause frequently,
    // so more efficient if last used memory is cached reuse it.
    free_header->set_next(free_list_);
    free_list_ = free_header;
  }

  // Calculate HeapHeader address from pointer,
  // and decreese the used size.
  auto pointer = reinterpret_cast<uintptr_t>(block);
  HeapHeader* header = reinterpret_cast<HeapHeader*>(pointer & kAddrMask);
  header->set_used(header->used() - size_class_);
}


// Split memory block by the size class and
// add all small block to free list.
YATSC_INLINE Byte* ChunkHeader::InitHeap(Byte* block) {
  // Create HeapHeader from block.
  HeapHeader* heap_header = new (block) HeapHeader(this);

  // First memory block is located after HeapHeader.
  Byte* begin = block + sizeof(HeapHeader);

  // Append heap to heap_list_ to deallocate after all.
  if (heap_list_ == nullptr) {
    heap_list_ = heap_header;
  } else {
    heap_list_->set_next(heap_header);
    heap_header->set_next(heap_header);
    heap_list_ = heap_header;
  }
  
  return begin;
}


// Allocate new 1MB aligned memory block by AlignedHeapAllocator.
YATSC_INLINE Byte* ChunkHeader::AllocateBlock() {
  return reinterpret_cast<Byte*>(AlignedHeapAllocator::Allocate(kAlignment));
}

}} // yatsc::heap
