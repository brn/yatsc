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


namespace yatsc {
#define TEMPLATE template<typename Signature>


TEMPLATE
YATSC_INLINE Notificator<Signature>::Notificator() {}


TEMPLATE
YATSC_INLINE Notificator<Signature>::Notificator(const Notificator<Signature>& notificator) {
  listeners_ = notificator.listeners_;
}


TEMPLATE
YATSC_INLINE const Notificator<Signature>& Notificator<Signature>::operator = (const Notificator<Signature>& notificator) {
  listeners_ = notificator.listeners_;
  return (*this);
}


TEMPLATE
template <typename Listener>
YATSC_INLINE void Notificator<Signature>::AddListener(const char* key, Listener listener) {
  ScopedSpinLock lock(lock_);
  //Listener adapter is allocated as the heap object,
  //because this object treat as the base class type ListenerAdapterBase.
  //Object lifetime is controlled by the shared_ptr
  //so a notificator instance that create ListenerAdapter is destroyed,
  //ListenerAdapter is destroyed too.
  listeners_.insert(ListenerSet(key, Heap::NewHandle<ListenerFunction>(listener)));
}


TEMPLATE
template <typename Listener>
YATSC_INLINE void Notificator<Signature>::operator += (std::pair<const char*, Listener> listener_pack) {
  AddListener(listener_pack.first, listener_pack.second);
}


TEMPLATE
YATSC_INLINE void Notificator<Signature>::RemoveListener(const char* key) {
  ListenersIterator it = listeners_.find(key);
  if (it != listeners_.end()) {
    listeners_.erase(it);
  }
}


TEMPLATE
YATSC_INLINE void Notificator<Signature>::operator -= (const char* key) {
  RemoveListener(key);
}


TEMPLATE
template<typename ... Args>
YATSC_INLINE void Notificator<Signature>::NotifyAll(Args ... args) YATSC_NO_SE {
  for (auto it: listeners_) {
    (*(it.second))(std::forward<Args>(args)...);
  }
}


TEMPLATE
template<typename ... Args>
YATSC_INLINE void Notificator<Signature>::NotifyForKey(const char* key, Args ... args) YATSC_NO_SE {
  ListenersRange listener_range = listeners_.equal_range(key);
  for (auto it = listener_range.first; it != listener_range.second; ++it) {
    (*(it->second))(std::forward<Args>(args)...);
  }
}


TEMPLATE
YATSC_INLINE void Notificator<Signature>::swap(Notificator<Signature>& notificator) {
  listeners_.swap(notificator.listeners_);
}

#undef TEMPLATE

}
