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

#include <atomic>
#include "./arena.h"
#include "../../utils/utils.h"

namespace yatsc {

namespace heap {

class HeapReferenceCounter {
 public:
  HeapReferenceCounter(void* ptr)
      : ptr_(ptr) {
    ref_.store(1, std::memory_order_relaxed);
  }

  
  virtual ~HeapReferenceCounter() {};

  
  YATSC_INLINE void AddReference();


  template <typename T>
  YATSC_INLINE void ReleaseReference();


  template <typename T>
  YATSC_INLINE T* Get() {return reinterpret_cast<T*>(ptr_);}

 private:
  void* ptr_;
  std::atomic<uint32_t> ref_;
};


class HeapReference: public HeapReferenceCounter {
 public:
  HeapReference()
      : HeapReferenceCounter(this) {}

  
  virtual ~HeapReference(){}
};
}


template <typename T>
class Handle {
  template <typename Type>
  friend class Handle;
 public:

  Handle(heap::HeapReferenceCounter* ref_count)
      : ref_count_(ref_count) {}


  Handle()
      : ref_count_(nullptr) {}


  ~Handle();


  Handle(const Handle<T>& heap_handle);
  

  template <typename U>
  Handle(const Handle<U>& heap_handle);


  Handle(Handle<T>&& hh);
  

  template <typename U>
  Handle(Handle<U>&& hh);


  Handle<T>& operator = (const Handle<T>& heap_handle);
  

  template <typename U>
  Handle<T>& operator = (const Handle<U>& heap_handle);


  Handle<T>& operator = (Handle<T>&& heap_handle);
  

  template <typename U>
  Handle<T>& operator = (Handle<U>&& heap_handle);


  YATSC_INLINE T* operator ->() {
    ASSERT(true, ref_count_ != nullptr);
    return ref_count_->Get<T>();
  }


  YATSC_INLINE T& operator *() {
    ASSERT(true, ref_count_ != nullptr);
    return *(ref_count_->Get<T>());
  }


  template <typename U>
  YATSC_INLINE T* operator[] (U index) {
    ASSERT(true, ref_count_ != nullptr);
    return ref_count_->Get<T>()[index];
  }


  YATSC_INLINE T* operator ++ () {
    ASSERT(true, ref_count_ != nullptr);
    return ref_count_->Get<T>()++;
  }


  YATSC_INLINE T* operator -- () {
    ASSERT(true, ref_count_ != nullptr);
    return ref_count_->Get<T>()--;
  }


  template <typename U>
  YATSC_INLINE T* operator + (U x) {
    ASSERT(true, ref_count_ != nullptr);
    return ref_count_->Get<T>() + x;
  }


  template <typename U>
  YATSC_INLINE T* operator - (U x) {
    ASSERT(true, ref_count_ != nullptr);
    return ref_count_->Get<T>() - x;
  }


 private:
  heap::HeapReferenceCounter* ref_count_;
};


class Heap: private Static {
 public:
  static void* NewPtr(size_t size) {
    return central_arena_.Allocate(size);
  }


  template <typename T, typename ... Args>
  static Handle<T> NewHandle(Args ... args) {
    void* ptr = NewPtr(sizeof(T) + sizeof(heap::HeapReferenceCounter));
    auto ref_count = new (reinterpret_cast<Byte*>(ptr) + sizeof(T)) heap::HeapReferenceCounter(
        new (ptr) T(args...));
    return Handle<T>(ref_count);
  }


  template <typename T, typename ... Args>
  static Handle<T> NewIntrusive(Args ... args) {
    return Handle<T>(New<T>(std::forward<Args>(args)...));
  }


  template <typename T, typename ... Args>
  static T* New(Args ... args) {
    return new (central_arena_.Allocate(sizeof(T))) T(args...);
  }


  template <typename T>
  static void Destruct(T* ptr) {
    ptr->~T();
    Delete(ptr);
  }
  

  static void Delete(void* ptr) {
    central_arena_.Dealloc(ptr);
  }


  static void Delete(volatile void* ptr) {
    central_arena_.Dealloc(ptr);
  }
  

 private:
  static heap::CentralArena central_arena_;
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
    return Heap::NewPtr(sizeof(T) * num);
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

}


#include "./heap-allocator-inl.h"

#endif
