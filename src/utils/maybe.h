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

#include <type_traits>
#include "./utils.h"

namespace yatsc {

// Value container for Maybe.
// value requirements:
//   T must be copy constructible.
//   T must be move constructible.
//
template <typename T>
class Result {
 public:
  // Raw type of T.
  typedef typename std::remove_reference<typename std::remove_const<T>::type>::type Type;

  // Raw const type of T.
  typedef const Type ConstType;


  // Concept check.
  static_assert(std::is_move_constructible<T>::value, "The type T of Result<T> must be move constructible.");
  static_assert(std::is_copy_constructible<T>::value, "The type T of Result<T> must be copy constructible.");
  

  // Just constructor.
  YATSC_INLINE explicit Result(Type value) {
    value_(value);
  }


  // Just(move) constructor
  YATSC_INLINE explicit Result(Type&& value) {
    value_(std::move(value));
  }


  // Nothing constructor.
  Result() {}


  // Copy constructor.
  YATSC_INLINE Result(const Result<T>& result) {
    if (!result.exists()) {return;}
    value_(result.value());
  }


  // Move constructor.
  YATSC_INLINE Result(Result<T>&& result) {
    if (!result.exists()) {return;}
    value_(std::move(result.value()));
  }


  // Copy constructor for U that is compatible type of T.
  template <typename U>
  YATSC_INLINE Result(const Result<U>& result) {
    static_assert(std::is_base_of<T, U>::value,
                  "The value of Result<U> must be the derived class of The value of Result<T>.");
    if (!result.exists()) {return;}
    value_(result.value());
  }


  // Move constructor for U that is compatible type of T.
  template <typename U>
  YATSC_INLINE Result(Result<U>&& result) {
    static_assert(std::is_base_of<T, U>::value,
                  "The value of Result<U> must be the derived class of The value of Result<T>.");
    if (!result.exists()) {return;}
    value_(std::move(result.value()));
  }


  // Copy assginment operator.
  YATSC_INLINE Result& operator = (const Result<T>& result) {
    if (!result.exists()) {return; *this;}
    value_(result.value());
    return *this;
  }


  // Move assginment operator.
  YATSC_INLINE Result& operator = (Result<T>&& result) {
    if (!result.exists()) {return; *this;}
    value_(std::move(result.value()));
    return *this;
  }


  // Copy assginment operator for U that is compatible type of T.
  template <typename U>
  YATSC_INLINE Result& operator = (const Result<U>& result) {
    static_assert(std::is_base_of<T, U>::value,
                  "The value of Result<U> must be the derived class of The value of Result<T>.");
    if (!result.exists()) {return; *this;}
    value_(result.value());
    return *this;
  }


  // Move assginment operator for U that is compatible type of T.
  template <typename U>
  YATSC_INLINE Result& operator = (Result<U>&& result) {
    static_assert(std::is_base_of<T, U>::value,
                  "The value of Result<U> must be the derived class of The value of Result<T>.");
    if (!result.exists()) {return; *this;}
    value_(std::move(result.value()));
    return *this;
  }


  // Getter for the value_.
  YATSC_GETTER(Type&, value, *value_);


  // Const getter for the value_.
  YATSC_CONST_GETTER(ConstType&, value, *value_);


  // Return Result<T> is Just or not.
  YATSC_CONST_GETTER(bool, exists, value_.initialized());
  
  
 private:
  LazyInitializer<Type> value_;
};



template <typename T>
class Result<T*> {
 public:
  typedef T* Type;

  typedef const typename std::remove_const<T>::type* ConstType;
  
  YATSC_INLINE explicit Result(T* ptr)
      : ptr_(ptr) {}


  YATSC_INLINE Result()
      : ptr_(nullptr) {}


  YATSC_INLINE Result(const Result<T>& result)
      : ptr_(result.value()) {}


  YATSC_INLINE Result(Result<T>&& result)
      : ptr_(result.value()) {}


  template <typename U>
  YATSC_INLINE Result(const Result<U>& result)
      : ptr_(result.value()) {}


  template <typename U>
  YATSC_INLINE Result(Result<U>&& result)
      : ptr_(result.value()) {}


  YATSC_INLINE Result& operator = (const Result<T>& result) {
    ptr_ = result.value();
    return *this;
  }


  YATSC_INLINE Result& operator = (Result<T>&& result) {
    ptr_ = result.value();
    return *this;
  }


  template <typename U>
  YATSC_INLINE Result& operator = (const Result<U>& result) {
    ptr_ = result.value();
    return *this;
  }


  template <typename U>
  YATSC_INLINE Result& operator = (Result<U>&& result) {
    ptr_ = result.value();
    return *this;
  }


  YATSC_GETTER(T*, value, ptr_);


  YATSC_CONST_GETTER(const T*, value, ptr_);


  YATSC_CONST_GETTER(bool, exists, ptr_ != nullptr);
  
  
 private:
  T* ptr_;
};



template <typename T>
class Maybe {
  template <typename Other>
  friend class Maybe;
 public:
  explicit YATSC_INLINE Maybe(T target) {
    result_(target);
  }

  
  YATSC_INLINE Maybe() {
    result_();
  }


  YATSC_INLINE Maybe(const Maybe& maybe) {
    result_(maybe.result());
  }


  template <typename Other>
  YATSC_INLINE Maybe(const Maybe<Other>& maybe) {
    result_(maybe.result());
  }


  YATSC_INLINE Maybe(Maybe&& maybe) {
    result_(std::move(maybe.result()));
  }


  template <typename Other>
  YATSC_INLINE Maybe(Maybe<Other>&& maybe) {
    result_(std::move(maybe.result()));
  }


  YATSC_INLINE Maybe<T>& operator = (const Maybe& maybe) {
    result_(maybe.result());
    return *this;
  }


  template <typename Other>
  YATSC_INLINE Maybe<T>& operator = (const Maybe<Other>& maybe) {
    result_(maybe.result());
    return *this;
  }


  YATSC_INLINE Maybe<T>& operator = (Maybe&& maybe) {
    result_(maybe.result());
    return *this;
  }


  template <typename Other>
  YATSC_INLINE Maybe<T>& operator = (Maybe<Other>&& maybe) {
    result_(maybe.result());
    return *this;
  }


  YATSC_INLINE typename Result<T>::Type value() {return result_->value();}


  YATSC_INLINE typename Result<T>::ConstType value() const {return result_->value();}


  YATSC_INLINE operator bool() YATSC_NO_SE {return just();}


  YATSC_INLINE bool just() YATSC_NO_SE {return result_->exists();}

  
  YATSC_INLINE bool nothing() YATSC_NO_SE {return !just();}


  YATSC_INLINE typename Result<T>::Type or(typename Result<T>::Type&& alternate) {
    if (!result_->exists()) {
      return std::move(alternate);
    }
    return value();
  }


  YATSC_INLINE typename Result<T>::Type or(typename Result<T>::Type&& alternate) const {
    if (!result_->exists()) {
      return std::move(alternate);
    }
    return value();
  }
  

  YATSC_INLINE typename Result<T>::Type or(typename Result<T>::Type& alternate) {
    if (!result_->exists()) {
      return alternate;
    }
    return value();
  }


  YATSC_INLINE typename Result<T>::Type or(typename Result<T>::Type& alternate) const {
    if (!result_->exists()) {
      return alternate;
    }
    return value();
  }


  template <typename ResultType, typename Fn>
  YATSC_INLINE Maybe<ResultType> fmap(Fn fn) {
    if (just()) {
      return fn(value());
    }
    return Maybe<ResultType>();
  }


  template <typename Fn>
  YATSC_INLINE Maybe<T> fmap(Fn fn) {
    if (just()) {
      return fn(value());
    }
    return Maybe<T>();
  }


  template <typename Fn>
  YATSC_INLINE Maybe<T> operator >>= (Fn fn) {
    return fmap(fn);
  }
  

 private:
  YATSC_INLINE const Result<T>& result() const {
    return *result_;
  }


  YATSC_INLINE Result<T>& result() {
    return *result_;
  }
  
  
  LazyInitializer<Result<T>> result_;
};


template <typename T>
Maybe<T> Just(T t) {
  return Maybe<T>(t);
}


template <typename T>
Maybe<T> Nothing() {
  return Maybe<T>();
}

}

#endif
