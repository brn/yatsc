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


namespace yatsc {namespace heap {

YATSC_INLINE void HeapReferenceCounterBase::AddReference() YATSC_NO_SE {
  std::atomic_fetch_add_explicit(&ref_, 1u, std::memory_order_relaxed);
}


template <typename T>
bool HeapReferenceCounter<T>::ReleaseReference() YATSC_NO_SE {
  if (std::atomic_fetch_sub_explicit(&ref_, 1u, std::memory_order_release) == delete_timing_) {
    std::atomic_thread_fence(std::memory_order_acquire);
    Heap::Destruct(reinterpret_cast<T*>(ptr_));
    return true;
  }
  return false;
}
} // end heap


template <typename T>
Handle<T>::~Handle() {
  if (ref_count_ == nullptr) {return;}
  if (ref_count_->ReleaseReference()) {
    ref_count_ = nullptr;
  }
}


template <typename T>
Handle<T>::Handle(const Handle<T>& hh) {
  if (hh.ref_count_ != nullptr) {
    hh.ref_count_->AddReference();
  }
  ref_count_ = hh.ref_count_;
}


template <typename T>
template <typename U>
Handle<T>::Handle(const Handle<U>& hh) {
  if (hh.ref_count_ != nullptr) {
    hh.ref_count_->AddReference();
  }
  ref_count_ = hh.ref_count_;
}


template <typename T>
Handle<T>::Handle(Handle<T>&& hh) {
  ref_count_ = hh.ref_count_;
  hh.ref_count_ = nullptr;
}


template <typename T>
template <typename U>
Handle<T>::Handle(Handle<U>&& hh) {
  ref_count_ = hh.ref_count_;
  hh.ref_count_ = nullptr;
}


template <typename T>
Handle<T>& Handle<T>::operator = (const Handle<T>& hh) {
  if (hh.ref_count_ != nullptr) {
    hh.ref_count_->AddReference();
  }
  ref_count_ = hh.ref_count_;
  return *this;
}


template <typename T>
template <typename U>
Handle<T>& Handle<T>::operator = (const Handle<U>& hh) {
  if (hh.ref_count_ != nullptr) {
    hh.ref_count_->AddReference();
  }
  ref_count_ = hh.ref_count_;
  return *this;
}


template <typename T>
Handle<T>& Handle<T>::operator = (Handle<T>&& hh) {
  ref_count_ = hh.ref_count_;
  hh.ref_count_ = nullptr;
  return *this;
}


template <typename T>
template <typename U>
Handle<T>& Handle<T>::operator = (Handle<U>&& hh) {
  ref_count_ = hh.ref_count_;
  hh.ref_count_ = nullptr;
  return *this;
}


YATSC_INLINE void* UnsafeZoneAllocator::NewPtr(size_t size) {
  size = YATSC_ALIGN_OFFSET(size, kAlignment);
  YATSC_CHECK(true, size_ > size);
  if (!zone_->HasEnoughSize(size)) {
    Grow();
  }
  return reinterpret_cast<void*>(zone_->GetHeap(size));
}


template <typename T, typename ... Args>
YATSC_INLINE T* UnsafeZoneAllocator::New(Args ... args) {
  void* ret = NewPtr(sizeof(T));
  return new (ret) T(args...);
}


YATSC_INLINE void UnsafeZoneAllocator::Grow() {
  void* ret = Heap::NewPtr(sizeof(Zone) + size_);
  Zone* head = new (ret) Zone(reinterpret_cast<Byte*>(ret) + sizeof(Zone), size_);
  head->set_next(zone_);
  zone_ = head;
}

}
