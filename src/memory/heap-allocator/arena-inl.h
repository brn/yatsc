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


inline Chunk* CentralArena::GetLocalArena(size_t size) {
  LocalArena* arena = reinterpret_cast<LocalArena*>(tls_->Get());
  if (arena == nullptr) {
    if (arena_head_ != nullptr) {
      arena = FindUnlockedArena();
    }
    if (arena == nullptr) {
      arena = new LocalArena(this);
      arena->AcquireLock();
      StoreNewLocalArena(arena);
    }

    tls_->Set(arena);
  }
  
  return arena;
}


inline Chunk* CentralArena::FindUnlockedArena() {
  LocalArena* head = local_arena_;
  LocalArena* current = local_arena_;
  
  while (1) {
    if (current->AcquireLock()) {
      return current;
    }
    current = current->next();
    if (current == nullptr) {
      return nullptr;
    }
  }
}


inline Chunk* CentralArena::StoreNewLocalArena(LocalArena* arena) {
  ScopedSpinLock lock(local_arena_lock_);
  LocalArena* head = local_arena_;
  if (local_arena_ != nullptr) {
    local_arena_ = arena;
  } else {
    local_arena_->set_next(arena);
  }
  local_arena->set_next(nullptr);
}


inline Chunk* LocalArena::AllocateIfNecessary(size_t size) {
  size_t index = FindJustFitBlockIndex(size);
  if (index < chunk_array_.size()) {
    if (chunk_array_[index] == nullptr) {
      Chunk* chunk = Chunk::New(size);
      chunk_array_[index] = chunk;
      return chunk;
    }
    return chunk_array_[index];
  }
  return AllocateLargeObject(size);
}
}}
