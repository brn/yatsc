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


#ifndef MEMORY_HEAP_ALLOCATOR_HEAP_ALLOCATOR_H
#define MEMORY_HEAP_ALLOCATOR_HEAP_ALLOCATOR_H

#include "../../utils/utils.h"

namespace yatsc {namespace heap {


template <typename T>
class HeapHandle {
 public:

  template <typename U>
  HeapHandle(U* ptr, void* ptr)
      : ptr_(ptr),
        ref_(new (ptr) std::atomic_int()) {
    ref_->load(0, std::memory_order_relaxed);
  }


  ~HeapHandle() {}


  HeapHandle(const HeapHandle& heap_handle);


  HeapHandle(HeapHandle&& hh) = delete;


  T* operator ->() {
    return ptr_;
  }


  const T& operator *() {
    return *ptr_;
  }


 private:  
  T* ptr_;
  std::atomic_int* ref_;
};


class Heap: private Static {
 public:
  static void* New(size_t size) {
    return central_arena_.Allocate(size);
  }


  template <typename T, typename ... Args>
  static HeapHandle<T> NewHandle(Args ... args) {
    void* ptr = New(sizeof(T) + sizeof(std::atomic_int));
    return HeapHandle(new (ptr) T(args...), reinterpret_cast<Byte*>(ptr) + sizeof(T));
  }


  static void Delete(void* ptr) {
    central_arena_.Dealloc(ptr);
  }


  static void Delete(volatile void* ptr) {
    central_arena_.Dealloc(ptr);
  }
  

 private:
  CentralArena central_arena_;
};


template <typename T>
class StandardAllocator: public std::allocator<T> {
 public:
  typedef size_t  size_type;
  typedef ptrdiff_t difference_type;
  typedef T* pointer;
  typedef const T* const_pointer;
  typedef T& reference;
  typedef const T& const_reference;
  typedef T value_type;

  template <class U>
  struct rebind { 
    typedef StandardAllocator<U> other;
  };


  StandardAllocator() = default;

  
  StandardAllocator(const StandardAllocator& allocator) = default;


  template <typename U>
  StandardAllocator(const StandardAllocator<U>& allocator) {}
    

  /**
   * Allocate new memory.
   */
  pointer allocate(size_type num, const void* hint = 0) {
    return Heap::New(sizeof(T) * num);
  }

  /**
   * Initialize already allocated block.
   */
  void construct(pointer p, const T& value) {
    new (static_cast<void*>(p)) T(value);
  }

  /**
   * Return object address.
   */
  pointer address(reference value) const { 
    return &value;
  }

  /**
   * Return const object address.
   */
  const_pointer address(const_reference value) const { 
    return &value;
  }

  /**
   * Remove pointer.
   */
  void destroy(pointer p) {
    p->~T();
  }

  /**
   * Do nothing.
   */
  void deallocate(pointer p, size_type n) {
    Heap::Delete(p);
  }

  /**
   * Return the max size of allocatable.
   */
  size_type max_size() const throw() {
    return std::numeric_limits<size_t>::max() / sizeof(T);
  }
};

}}


#include "./heap-allocator-inl.h"

#endif
