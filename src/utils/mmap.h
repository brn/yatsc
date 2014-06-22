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


#ifndef UTILS_MMAP_H_
#define UTILS_MMAP_H_

#include <atomic>
#include "systeminfo.h"
#include "spinlock.h"

namespace {
static const size_t kDefaultByte = yatsc::SystemInfo::GetPageSize();
}

namespace yatsc {
class Mmap {
  class InternalMmap;
 public:
  inline Mmap();


  inline ~Mmap();


  Mmap(Mmap&& mmap) {
    std::swap(*this, mmap);
    mmap.uncommited_.test_and_set();
  }


  Mmap& operator = (Mmap&& mmap) {
    std::swap(*this, mmap);
    mmap.uncommited_.test_and_set();
    return *this;
  }
  
  
  YATSC_INLINE void* Commit(size_t size);
  YATSC_INLINE void UnCommit();
  YATSC_INLINE uint64_t commited_size() YATSC_NO_SE;
  YATSC_INLINE uint64_t real_commited_size() YATSC_NO_SE;
  

  template <class T>
  class MmapStandardAllocator {
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
      typedef MmapStandardAllocator<U> other;
    };

    explicit MmapStandardAllocator(Mmap* mmap)
        : mmap_(mmap){}


    MmapStandardAllocator(const MmapStandardAllocator& allocator)
        : mmap_(allocator.mmap_){}


    template <typename U>
    MmapStandardAllocator(const MmapStandardAllocator<U>& allocator)
        : mmap_(allocator.mmap()){}
    

    /**
     * Allocate new memory.
     */
    pointer allocate(size_type num, const void* hint = 0) {
      return reinterpret_cast<pointer>(mmap_->Commit(sizeof(T) * num));
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
    void deallocate(pointer p, size_type n) {}

    /**
     * Return the max size of allocatable.
     */
    size_type max_size() const throw() {
      return std::numeric_limits<size_t>::max() / sizeof(T);
    }


    Mmap* mmap() const {return mmap_;}

   private:
    Mmap* mmap_;
  };
  
 private:
  class InternalMmap {
   private:
  
    class Header {
     public:
      YATSC_INLINE Header* ToNextPtr() YATSC_NO_SE {
        return next_;
      }


      YATSC_INLINE void set_next(Header* byte) YATSC_NOEXCEPT {
        next_ = byte;
      }


      YATSC_INLINE Byte* ToValue() YATSC_NO_SE {
        return ToBegin() + sizeof(Header);
      }
  

      YATSC_INLINE Byte* ToBegin() YATSC_NO_SE {
        return reinterpret_cast<Byte*>(const_cast<Header*>(this));
      }


      YATSC_INLINE size_t size() YATSC_NO_SE {
        return size_;
      }


      YATSC_INLINE void set_size(size_t size) YATSC_NOEXCEPT {
        size_ = size;
      }

     private:
      Header* next_;
      size_t size_;
    };

  
   public:
    YATSC_INLINE InternalMmap():
        current_map_size_(kDefaultByte),
        used_(0u),
        heap_(nullptr),
        current_(nullptr),
        last_(nullptr),
        commited_(0),
        real_(0){
      lock_.clear();
    }


    ~InternalMmap() = default;


    YATSC_INLINE uint64_t commited() YATSC_NO_SE {
      return commited_;
    }


    YATSC_INLINE uint64_t real_commited() YATSC_NO_SE {
      return real_;
    }
  
  
    YATSC_INLINE void* Commit(size_t size);


    YATSC_INLINE void UnCommit();

   private:
  
    YATSC_INLINE void* Alloc(size_t size);


    YATSC_INLINE void* AddHeader(void* heap, size_t size);
  

    SpinLock spin_lock_;
    std::atomic_flag lock_;
    size_t current_map_size_;
    size_t used_;
    void* heap_;
    void* current_;
    Header* last_;
    uint64_t commited_;
    uint64_t real_;
  };
  
  InternalMmap mmap_;
  std::atomic_flag uncommited_;
};

}

#include "mmap-inl.h"
#endif
