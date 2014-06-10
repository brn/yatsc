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


#ifndef UTILS_MMAP_INL_H_
#define UTILS_MMAP_INL_H_

#include <thread>
#include "./utils.h"
#include "../config.h"


#if defined(HAVE_MMAP)
#include "mmap-mmap.h"
#elif defined(HAVE_VIRTUALALLOC)
#include "mmap-virtual-alloc.h"
#endif


namespace rasp {


RASP_INLINE void* Mmap::Commit(size_t size) {
  return mmap_.Commit(size);
}


RASP_INLINE void Mmap::UnCommit() {
  if (!uncommited_.test_and_set()) {
    mmap_.UnCommit();
  }
}


Mmap::Mmap() {
  uncommited_.clear();
}

Mmap::~Mmap() {
  UnCommit();
}


uint64_t Mmap::commited_size() RASP_NO_SE {
  return mmap_.commited();
}


uint64_t Mmap::real_commited_size() RASP_NO_SE {
  return mmap_.real_commited();
}



void* Mmap::InternalMmap::Commit(size_t size) {
  ScopedSpinLock lock(spin_lock_);
  size_t needs = RASP_ALIGN_OFFSET((kPointerSize + size), kAlignment);
  if (current_map_size_ < needs || (current_map_size_ - used_) < needs || heap_ == nullptr) {
    return Alloc(needs);
  }
  Byte* ret = reinterpret_cast<Byte*>(current_) + used_;
  used_ += size;
  commited_ += size;
  return static_cast<void*>(ret);
}


void Mmap::InternalMmap::UnCommit() {
  Header* area = reinterpret_cast<Header*>(heap_);
  while (area != nullptr) {
    Header* tmp = area->ToNextPtr();
    size_t size = area->size();
    void* block = area->ToBegin();
    MapAllocator::Deallocate(block, size);
    area = tmp;
  }
}


void* Mmap::InternalMmap::Alloc(size_t size) {
  size_t map_size = kDefaultByte;
  if (size > kDefaultByte) {
    map_size = size;
  }
  map_size += sizeof(Header);
  current_map_size_ = RASP_ALIGN_OFFSET(map_size, rasp::SystemInfo::GetPageSize());
  void* heap;
  if (heap_ == nullptr) {
    heap = current_ = heap_ = MapAllocator::Allocate(map_size);
  } else {
    heap = MapAllocator::Allocate(map_size);
    reinterpret_cast<Header*>(current_)->set_next(reinterpret_cast<Header*>(heap));
    current_ = heap;
  }
  real_ += map_size;
  used_ = 0u;
  return AddHeader(heap, map_size);
}


void* Mmap::InternalMmap::AddHeader(void* heap, size_t size) {
  Header* header = reinterpret_cast<Header*>(heap);
  header->set_next(nullptr);
  header->set_size(size);
  if (last_ != nullptr) {
    last_->set_next(header);
  }
  used_ += size;
  commited_ += size;
  last_ = header;
  return static_cast<void*>(header->ToValue());
}

} // namespace rasp

#endif
