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

#ifndef YATSC_UTILS_MAYBE_H
#define YATSC_UTILS_MAYBE_H

namespace yatsc {

template <typename T>
class Maybe {
 public:
  YATSC_INLINE Maybe(T target, bool valid)
      : value_(target),
        valid_(valid) {}

  YATSC_INLINE Maybe()
      : valid_(false) {}


  YATSC_INLINE Maybe(const Maybe& maybe)
      : value_(maybe.value_),
        valid_(maybe.valid_) {}


  template <typename Other>
  YATSC_INLINE Maybe(const Maybe<Other>& maybe)
      : value_(maybe.value_),
        valid_(maybe.valid_) {}


  YATSC_INLINE Maybe(Maybe&& maybe)
      : value_(std::move(maybe.value_)),
        valid_(maybe.valid_) {}


  template <typename Other>
  YATSC_INLINE Maybe(Maybe<Other>&& maybe)
      : value_(std::move(maybe.value_)),
        valid_(maybe.valid_) {}


  YATSC_INLINE Maybe<T>& operator = (const Maybe& maybe) {
    value_ = maybe.value_;
    valid_ = maybe.valid_;
    return *this;
  }


  template <typename Other>
  YATSC_INLINE Maybe<T>& operator = (const Maybe<Other>& maybe) {
    value_ = maybe.value_;
    valid_ = maybe.valid_;
    return *this;
  }


  YATSC_INLINE Maybe<T>& operator = (Maybe&& maybe) {
    value_ = std::move(maybe.value_);
    valid_ = maybe.valid_;
    return *this;
  }


  template <typename Other>
  YATSC_INLINE Maybe<T>& operator = (Maybe<Other>&& maybe) {
    value_ = std::move(maybe.value_);
    valid_ = maybe.valid_;
    return *this;
  }


  YATSC_INLINE operator bool() YATSC_NO_SE {return valid_;}

  
  YATSC_INLINE T value() {return value_;}


  YATSC_INLINE bool just() YATSC_NO_SE {return valid_;}

  
  YATSC_INLINE bool nothing() YATSC_NO_SE {return !valid_;}


  template <typename Result, typename Fn>
  YATSC_INLINE Maybe<Result> operator >>=(Fn fn) {
    if (valid_) {
      return Maybe<Result>(fn(value_));
    }
    return Maybe<Result>();
  }
  

 private:
  T value_;
  bool valid_;
};


template <typename T>
Maybe<T> Just(T t) {
  return Maybe<T>(t, true);
}


template <typename T>
Maybe<T> Nothing() {
  return Maybe<T>();
}


template <typename T>
Maybe<T> Nothing(T value) {
  return Maybe<T>(value, false);
}

}

#endif
