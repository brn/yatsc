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


#ifndef ENVIRONMENT_H
#define ENVIRONMENT_H

#include "./tls.h"
#include "./regions.h"
#include "../parser/literalbuffer.h"

namespace yatsc {
class Environment : private Uncopyable {
 public:
  static Environment* Create() YATSC_NOEXCEPT {
    Environment* environment = reinterpret_cast<Environment*>(tls_.Get());
    if (environment == nullptr) {
      environment = new Environment();
      tls_.Set(environment);
    }
    return environment;
  }

  template <typename T, typename ... Args>
  YATSC_INLINE T* New(Args ... args) {
    return regions_.New<T>(std::forward<Args>(args)...);
  }


  template <typename T>
  YATSC_INLINE T* NewPtr(size_t num) {
    return regions_.NewPtr<T>(num);
  }


  void Dealloc(void* p) {
    regions_.Dealloc(p);
  }


  YATSC_INLINE LiteralBuffer* literal_buffer() YATSC_NOEXCEPT {
    return &literal_buffer_;
  }


  YATSC_INLINE Regions* regions() YATSC_NOEXCEPT {
    return &regions_;
  }

 private:
  Environment() = default;

  LiteralBuffer literal_buffer_;
  
  static ThreadLocalStorage::Slot tls_;
  static Regions regions_;
};


template <typename T>
class RegionsStandardAllocator: public std::allocator<T> {
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
    typedef RegionsStandardAllocator<U> other;
  };


  RegionsStandardAllocator()
      : env_(Environment::Create()){}

  
  RegionsStandardAllocator(const RegionsStandardAllocator& allocator)
      : env_(allocator.env_) {}


  template <typename U>
  RegionsStandardAllocator(const RegionsStandardAllocator<U>& allocator)
      : env_(allocator.env_) {}
    

  /**
   * Allocate new memory.
   */
  pointer allocate(size_type num, const void* hint = 0) {
    return env_->NewPtr<value_type>(sizeof(T) * num);
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
    //env_->Dealloc(p);
  }

  /**
   * Return the max size of allocatable.
   */
  size_type max_size() const throw() {
    return std::numeric_limits<size_t>::max() / sizeof(T);
  }

 private:
  Environment* env_;
};
}

#endif
