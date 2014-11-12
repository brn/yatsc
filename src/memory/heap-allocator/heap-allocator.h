//
// The MIT License (MIT)
//
// Copyright (c) 2013 Taketoshi Aono(brn)
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.



#ifndef MEMORY_HEAP_ALLOCATOR_HEAP_ALLOCATOR_H
#define MEMORY_HEAP_ALLOCATOR_HEAP_ALLOCATOR_H

#include <atomic>
#include "./arena.h"
#include "../../utils/utils.h"

namespace yatsc {

class Heap;

namespace heap {

// Reference counter for yatsc::Handle.
class HeapReferenceCounter {
 public:
  HeapReferenceCounter(void* ptr)
      : ptr_(ptr) {
    ref_.store(1, std::memory_order_relaxed);
  }

  
  virtual ~HeapReferenceCounter() {};

  // Add reference.
  YATSC_INLINE void AddReference() YATSC_NO_SE;

  // Release reference.
  template <typename T>
  YATSC_INLINE void ReleaseReference() YATSC_NO_SE;

  // Return internal
  template <typename T>
  YATSC_INLINE T* Get() YATSC_NOEXCEPT {return reinterpret_cast<T*>(ptr_);}


  // Return internal
  template <typename T>
  YATSC_INLINE const T* Get() YATSC_NO_SE {return reinterpret_cast<T*>(ptr_);}


  template <typename T>
  bool operator == (T t) YATSC_NO_SE {return t == ptr_;}

 private:
  void* ptr_;
  mutable std::atomic<uint32_t> ref_;
};


class HeapReference: public HeapReferenceCounter {
 public:
  HeapReference()
      : HeapReferenceCounter(this) {}

  
  virtual ~HeapReference(){}
};
}


// Heap handle that manage object life time by reference count.
// This class cannot instantiate on heap.
template <typename T>
class Handle {
  template <typename Type>
  friend class Handle;

  friend class Heap;
 public:

  // Create null Handle.
  // Null Handle is not allowed any operation that touch the internal pointer.
  Handle()
      : ref_count_(nullptr) {}


  Handle(heap::HeapReferenceCounter* ref_count)
      : ref_count_(ref_count) {}


  ~Handle();


  // Copy Handle.
  Handle(const Handle<T>& heap_handle);
  

  // Copy Handle that has derived type of T.
  template <typename U>
  Handle(const Handle<U>& heap_handle);


  // Move Handle.
  Handle(Handle<T>&& hh);
  

  // Move Handle that has derived type of T
  template <typename U>
  Handle(Handle<U>&& hh);


  // Copy Handle.
  Handle<T>& operator = (const Handle<T>& heap_handle);
  

  // Copy Handle that has derived type of T.
  template <typename U>
  Handle<T>& operator = (const Handle<U>& heap_handle);


  // Move Handle.
  Handle<T>& operator = (Handle<T>&& heap_handle);
  

  // Move Handle that has derived type of T
  template <typename U>
  Handle<T>& operator = (Handle<U>&& heap_handle);


  void Clear() {ref_count_ = nullptr;}


  // Equality comparator of Handle<U>.
  template <typename U>
  YATSC_INLINE bool operator == (const Handle<U>& heap_handle) YATSC_NO_SE {
    return heap_handle.ref_count_ == ref_count_;
  }


  // Equality comparator of Handle<T>.
  YATSC_INLINE bool operator == (const Handle<T>& heap_handle) YATSC_NO_SE {
    return heap_handle.ref_count_ == ref_count_;
  }


  // Equality comparator of Handle<U>.
  template <typename U>
  YATSC_INLINE bool operator == (const U* ptr) YATSC_NO_SE {
    return *ref_count_ == ptr;
  }


  // Equality comparator of Handle<T>.
  YATSC_INLINE bool operator == (const T* ptr) YATSC_NO_SE {
    return *ref_count_ == ptr;
  }
  

  // Cast to bool.
  YATSC_INLINE operator bool() YATSC_NO_SE {return ref_count_ != nullptr;}


  YATSC_INLINE T* Get() YATSC_NOEXCEPT {return ref_count_->Get<T>();}


  YATSC_INLINE const T* Get() YATSC_NO_SE {return ref_count_->Get<T>();}
  

  // Access to the internal pointer.
  YATSC_INLINE T* operator ->() YATSC_NOEXCEPT {
    ASSERT(true, ref_count_ != nullptr);
    return ref_count_->Get<T>();
  }


  // Access to the internal pointer.
  YATSC_INLINE const T* operator ->() YATSC_NO_SE {
    ASSERT(true, ref_count_ != nullptr);
    return ref_count_->Get<T>();
  }


  // Get reference type of the T.
  YATSC_INLINE T& operator *() YATSC_NOEXCEPT {
    ASSERT(true, ref_count_ != nullptr);
    return *(ref_count_->Get<T>());
  }


  // Get reference type of the T.
  YATSC_INLINE const T& operator *() YATSC_NO_SE {
    ASSERT(true, ref_count_ != nullptr);
    return *(ref_count_->Get<T>());
  }


  // Index access of the reference type of T.
  template <typename U>
  YATSC_INLINE T* operator[] (U index) YATSC_NOEXCEPT {
    ASSERT(true, ref_count_ != nullptr);
    return ref_count_->Get<T>()[index];
  }


  // Index access of the reference type of T.
  template <typename U>
  YATSC_INLINE const T* operator[] (U index) YATSC_NO_SE {
    ASSERT(true, ref_count_ != nullptr);
    return ref_count_->Get<T>()[index];
  }


  // Call operator++ of reference type of T.
  YATSC_INLINE T* operator ++ () {
    ASSERT(true, ref_count_ != nullptr);
    return ref_count_->Get<T>()++;
  }


  // Call operator-- of reference type of T.
  YATSC_INLINE T* operator -- () {
    ASSERT(true, ref_count_ != nullptr);
    return ref_count_->Get<T>()--;
  }


  // Call operator+ of reference type of T.
  template <typename U>
  YATSC_INLINE T* operator + (U x) YATSC_NOEXCEPT {
    ASSERT(true, ref_count_ != nullptr);
    return ref_count_->Get<T>() + x;
  }


  template <typename U>
  YATSC_INLINE const T* operator + (U x) YATSC_NO_SE {
    ASSERT(true, ref_count_ != nullptr);
    return ref_count_->Get<T>() + x;
  }


  // Call operator- of reference type of T.
  template <typename U>
  YATSC_INLINE T* operator - (U x) YATSC_NOEXCEPT {
    ASSERT(true, ref_count_ != nullptr);
    return ref_count_->Get<T>() - x;
  }


  // Call operator- of reference type of T.
  template <typename U>
  YATSC_INLINE const T* operator - (U x) YATSC_NO_SE {
    ASSERT(true, ref_count_ != nullptr);
    return ref_count_->Get<T>() - x;
  }

  
  // Cast to Handle<U>.
  template <typename U>
  YATSC_INLINE Handle<U> To() YATSC_NOEXCEPT {
    return std::move(Handle<U>(ref_count_));
  }


  template <typename U>
  YATSC_INLINE Handle<T>& operator << (U&& v) {
    *(ref_count_->Get<T>()) << v;
    return *this;
  }
  
 private:
  heap::HeapReferenceCounter* ref_count_;
};


// Main allocator of the heap.
class Heap: private Static {
 public:
  // Allocate raw pointer that has specified size from the CentralArena.
  static void* NewPtr(size_t size) {
    return central_arena_.Allocate(size);
  }


  // Allocate from heap that has combined size of
  // size of T and size of HeapReferenceCounter,
  // and instantiate T and return Handle<T>.
  template <typename T, typename ... Args>
  static Handle<T> NewHandle(Args ... args) {
    void* ptr = NewPtr(sizeof(T) + sizeof(heap::HeapReferenceCounter));
    auto ref_count = new (reinterpret_cast<Byte*>(ptr) + sizeof(T)) heap::HeapReferenceCounter(
        new (ptr) T(args...));
    return Handle<T>(ref_count);
  }


  // Allocate and instantiate T that must be derived type of HeapReference.
  template <typename T, typename ... Args>
  static Handle<T> NewIntrusive(Args ... args) {
    return Handle<T>(New<T>(std::forward<Args>(args)...));
  }


  // Allocate and instantiate T.
  template <typename T, typename ... Args>
  static T* New(Args ... args) {
    return new (central_arena_.Allocate(sizeof(T))) T(args...);
  }


  // Call destructor of T and free pointer.
  template <typename T>
  static void Destruct(T* ptr) {
    ptr->~T();
    Delete(ptr);
  }
  

  // Free pointer.
  static void Delete(void* ptr) {
    central_arena_.Dealloc(ptr);
  }


  // Volatile pointer specialization version of Delete(void*).
  static void Delete(volatile void* ptr) {
    central_arena_.Dealloc(ptr);
  }
  

 private:
  static heap::CentralArena central_arena_;
};


class UnsafeZoneAllocator {
 public:
  UnsafeZoneAllocator(size_t size = 1 KB)
      : size_(size),
        zone_(nullptr) {Grow();}

  ~UnsafeZoneAllocator();


  YATSC_INLINE void* NewPtr(size_t size);


  template <typename T, typename ... Args>
  YATSC_INLINE T* New(Args ... args);


 private:
  class Zone {
   public:
    Zone(Byte* heap, size_t size)
        : heap_(heap),
          size_(size),
          used_(0),
          next_(nullptr) {}

    ~Zone() = default;


    YATSC_GETTER(Byte*, heap, heap_);


    YATSC_CONST_GETTER(size_t, size, size_);


    YATSC_PROPERTY(Zone*, next, next_);


    YATSC_INLINE bool HasEnoughSize(size_t size) const {
      return (size_ - size) > used_;
    }


    YATSC_INLINE void* GetHeap(size_t size) {
      auto ret = heap_ + used_;
      used_ += size;
      return ret;
    }

   private:
    Byte* heap_;
    size_t size_;
    size_t used_;
    Zone* next_;
  };

  
  void Grow();
  

  size_t size_;
  Zone* zone_;
};


// std::allocator implementation of the yatsc::Heap.
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
  StandardAllocator(const StandardAllocator<U>&) {}
    


  // Allocate new memory.
  pointer allocate(size_type num, const void*) YATSC_NO_SE {
    return allocate(num);
  }

  
  pointer allocate(size_type num) YATSC_NO_SE {
    return reinterpret_cast<pointer>(Heap::NewPtr(sizeof(T) * num));
  }

  
  // Initialize already allocated block.  
  void construct(pointer p, const T& value) YATSC_NOEXCEPT {
    new (static_cast<void*>(p)) T(value);
  }

  
  // Return object address.  
  pointer address(reference value) YATSC_NO_SE { 
    return &value;
  }

  
  // Return const object address.
  const_pointer address(const_reference value) YATSC_NO_SE { 
    return &value;
  }

  
  // Remove pointer.  
  void destroy(pointer ptr) YATSC_NO_SE {
    (void)ptr;
    ptr->~T();
  }

  
  // Do nothing.  
  void deallocate(pointer p, size_type) YATSC_NO_SE {
    Heap::Delete(p);
  }

  
  // Return the max size of allocatable.
  size_type max_size() YATSC_NO_SE {
    return std::numeric_limits<size_t>::max() / sizeof(T);
  }
};


template <typename T>
class UnsafeZoneStdAllocator: public std::allocator<T> {
  template <typename U>
  friend class UnsafeZoneStdAllocator;
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
    typedef UnsafeZoneStdAllocator<U> other;
  };


  UnsafeZoneStdAllocator(UnsafeZoneAllocator* unsafe_zone_allocator)
      : unsafe_zone_allocator_(unsafe_zone_allocator) {}


  UnsafeZoneStdAllocator(size_t size)
      : unsafe_zone_allocator_(Heap::New<UnsafeZoneAllocator>(size)) {}

  
  UnsafeZoneStdAllocator(const UnsafeZoneStdAllocator& allocator)
      : unsafe_zone_allocator_(allocator.unsafe_zone_allocator_) {}


  template <typename U>
  UnsafeZoneStdAllocator(const UnsafeZoneStdAllocator<U>& allocator)
      : unsafe_zone_allocator_(allocator.unsafe_zone_allocator_) {}
    


  // Allocate new memory.
  pointer allocate(size_type num, const void*) YATSC_NO_SE {
    return allocate(num);
  }

  
  pointer allocate(size_type num) YATSC_NO_SE {
    return reinterpret_cast<pointer>(unsafe_zone_allocator_->NewPtr(sizeof(T) * num));
  }

  
  // Initialize already allocated block.  
  void construct(pointer p, const T& value) YATSC_NOEXCEPT {
    new (static_cast<void*>(p)) T(value);
  }

  
  // Return object address.  
  pointer address(reference value) YATSC_NO_SE { 
    return &value;
  }

  
  // Return const object address.
  const_pointer address(const_reference value) YATSC_NO_SE { 
    return &value;
  }

  
  // Remove pointer.  
  void destroy(pointer ptr) YATSC_NO_SE {
    (void)ptr;
    ptr->~T();
  }

  
  // Do nothing.  
  void deallocate(pointer p, size_type) YATSC_NO_SE {}

  
  // Return the max size of allocatable.
  size_type max_size() YATSC_NO_SE {
    return std::numeric_limits<size_t>::max() / sizeof(T);
  }


 private:
  UnsafeZoneAllocator* unsafe_zone_allocator_;
};

}


#include "./heap-allocator-inl.h"

#endif
