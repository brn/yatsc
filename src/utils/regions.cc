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

#include "regions.h"


namespace rasp {


//Regions constructor.
Regions::Regions(size_t size)
    : size_(size) {
  ASSERT(true, size <= kMaxAllocatableSize);
  central_arena_ = central_arena_once_init_(&allocator_);
  deleted_.clear();
}


// Get heap from the pool.
// The heap structure is bellow
// |1-BIT SENTINEL-FLAG|1-BIT Allocatable FLAG|14-BIT SIZE BIT|FREE-MEMORY|
// This method return FREE-MEMORY area.
rasp::Regions::Header* Regions::Chunk::GetBlock(size_t reserve) RASP_NOEXCEPT  {
  ASSERT(true, HasEnoughSize(reserve));

  if (reserve < kFreeHeaderSize) {
    reserve = kFreeHeaderSize;
  }
  
  Byte* ret = block_ + used_;
  
  size_t reserved_size = RASP_ALIGN_OFFSET((kHeaderSize + reserve), kAlignment);
  used_ += reserved_size;
  tail_block_ = ret;
  
  Header* header = reinterpret_cast<Header*>(ret);
  header->set_size(reserve);
  return header;
}


void Regions::Chunk::Destruct() {
  if (0u == used_ || tail_block_ == nullptr) {
    return;
  }
  Header* header = reinterpret_cast<Header*>(block_);
  while (1) {
    bool exit = IsTail(header->ToBegin());
    if (!header->IsMarkedAsDealloced()) {
      DestructRegionalObject(header);
    }
    if (exit) {
      break;
    }
    header = header->next_addr();
  }
}


Regions& Regions::operator = (Regions&& regions) {
  central_arena_ = regions.central_arena_;
  size_ = regions.size_;
  allocator_ = std::move(regions.allocator_);
  regions.deleted_.test_and_set();
  deleted_.clear();
  regions.deleted_.test_and_set();
  regions.central_arena_ = nullptr;
  return (*this);
}



Regions::FreeChunkStack* Regions::LocalArena::InitHugeFreeChunkStack(int index) {
  if (huge_free_chunk_map_.count(index) != 0) {
    return huge_free_chunk_map_[index];
  }
  FreeChunkStack* new_free_chunk_stack = new(mmap_.Commit(sizeof(ChunkList))) FreeChunkStack();
  huge_free_chunk_map_[index] = new_free_chunk_stack;
  return new_free_chunk_stack;
}


void Regions::CentralArena::Destroy() RASP_NOEXCEPT {
  LocalArena* arena = arena_head_;
  while (arena != nullptr) {
    auto chunk_list = arena->chunk_list();
    IterateChunkList(chunk_list);
    arena = arena->next();
  }
  tls_->Free();
  tls_->~Slot();
}


void Regions::CentralArena::IterateChunkList(Regions::ChunkList* chunk_list) RASP_NOEXCEPT {
  if (chunk_list->head() != nullptr) {
    auto chunk = chunk_list->head();
    while (chunk != nullptr) {
      ASSERT(true, chunk != nullptr);
      auto tmp = chunk;
      chunk = chunk->next();
      Chunk::Delete(tmp);
    }
  }
}


void Regions::CentralArena::Dealloc(void* object) RASP_NOEXCEPT {
  Byte* block = reinterpret_cast<Byte*>(object);
  block -= Regions::kHeaderSize;
  Regions::Header* header = reinterpret_cast<Regions::Header*>(block);
  RASP_CHECK(false, header->IsMarkedAsDealloced());
  DestructRegionalObject(header);
  header->MarkAsDealloced();
  ASSERT(true, header->IsMarkedAsDealloced());
  int index = FindBestFitBlockIndex(header->size());
  LocalArena* arena = TlsAlloc();
  arena->free_chunk_stack(index)->Unshift(header);
}


const int Regions::kValueOffset = kSizeTSize;
const size_t Regions::kFreeHeaderSize = sizeof(Regions::FreeHeader);
const size_t Regions::kHeaderSize = sizeof(Regions::Header);
} //namespace rasp

