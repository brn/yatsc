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


namespace yatsc {namespace heap {
inline CentralArena::~CentralArena() {
  LocalArena* head = local_arena_;
  LocalArena* current = local_arena_;
  
  while (1) {
    LocalArena* tmp = current->next();
    delete current;
    
    if (tmp == nullptr) {
      break;
    }
    current = tmp;
  }
}


YATSC_INLINE LocalArena* GetLocalArena() {
  void* ptr = tls_->Get();
  if (ptr == nullptr) {
    return FindUnlockedArena();
  }
  return reinterpret_cast<LocalArena*>(ptr);
}


YATSC_INLINE LocalArena* CentralArena::FindUnlockedArena() {
  LocalArena* head = local_arena_;
  LocalArena* current = local_arena_;
  
  while (1) {
    if (current->AcquireLock()) {
      return current;
    }
    current = current->next();
  }

  if (current == nullptr) {
    return StoreNewLocalArena();
  }
}


YATSC_INLINE LocalArena* CentralArena::StoreNewLocalArena() {
  Byte* block = reinterpret_cast<Byte*>(VirtualHeapAllocator::Allocate(
      nullptr,
      kMaxAllocatableSmallObjectSize + sizeof(LocalArena),
      VirtualHeapAllocator::Prot::WRITE,
      VirtualHeapAllocator::Flags::ANONYMOUS | VirtualHeapAllocator::Flag::PRIVATE));
  
  LocalArena* arena = new (block) LocalArena(this, block + sizeof(LocalArena));

  LocalArena* arena_head = local_arena_.load(std::memory_order_relaxed);
  arena->set_next(arena);
  
  while (local_arena_.compare_exchange_weak(arena_head, arena)) {
    arena->set_next(arena_head);
  }

  tls_->Set(arena);
  return arena;
}


YATSC_INLINE Chunk* LocalArena::AllocateIfNecessary(size_t size) {
  if (size < kMaxAllocatableSmallObjectSize) {
    size_t index = FindJustFitBlockIndex(size);
    Chunk* chunk = Chunk::New(size);
    chunk_array_[index] = chunk;
    return chunk;
  }
  return AllocateLargeObject(size);
}
}}
