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


#ifndef PARSER_UTF_STRING_H_
#define PARSER_UTF_STRING_H_

#include <string>
#include <utility>
#include "./uchar.h"
#include "../utils/unicode.h"
#include "../utils/utils.h"
#include "unicode-iterator-adapter.h"

namespace rasp {

class UtfValueCache {
 public:
  UtfValueCache() = default;


  UtfValueCache(const UtfValueCache& utf_value_cache)
      : utf8_value_(utf_value_cache.utf8_value_),
        utf16_value_(utf_value_cache.utf16_value_) {}

  
  UtfValueCache(UtfValueCache&& utf_value_cache) RASP_NOEXCEPT
      : utf8_value_(std::move(utf_value_cache.utf8_value_)),
        utf16_value_(std::move(utf_value_cache.utf16_value_)) {}


  UtfValueCache& operator = (UtfValueCache&& utf_value_cache) {
    utf8_value_ = std::move(utf_value_cache.utf8_value_);
    utf16_value_ = std::move(utf_value_cache.utf16_value_);
    return (*this);
  }


  UtfValueCache& operator = (const UtfValueCache& utf_value_cache) {
    utf8_value_ = utf_value_cache.utf8_value_;
    utf16_value_ = utf_value_cache.utf16_value_;
    return (*this);
  }
  
  
  RASP_INLINE void append_utf8_value(const char* utf8) {
    utf8_value_.append(utf8);
  }


  RASP_INLINE const Utf8String& utf8_value() RASP_NO_SE {
    return utf8_value_;
  }
  

  RASP_INLINE void append_utf16_value(const UC16 uc) {
    utf16_value_.append(1, uc);
  }


  RASP_INLINE const Utf16String& utf16_value() const {
    return utf16_value_;
  }


  RASP_INLINE void Append(const Utf8String& utf8_value, const Utf16String& utf16_value) {
    utf8_value_.append(utf8_value);
    utf16_value_.append(utf16_value);
  }


  RASP_INLINE void Clear() {
    utf8_value_.clear();
    utf16_value_.clear();
  }
  
  
 private:
  Utf8String utf8_value_;
  Utf16String utf16_value_;
};


class Utf8Value {
 public:
  Utf8Value(const char* utf8, size_t size)
      : utf8_(utf8),
        size_(size){}

  
  Utf8Value(const Utf8Value& uv)
      : utf8_(uv.utf8_),
        size_(uv.size_){}

  
  RASP_INLINE const char* value() RASP_NO_SE {
    return utf8_;
  }

  
  RASP_INLINE size_t size() RASP_NO_SE {
    return size_;
  }
  
 private:
  const char* utf8_;
  size_t size_;
};


class Utf16Value {
 public:
  Utf16Value(const UC16* utf16, size_t size)
      : utf16_(utf16),
        size_(size){}

  
  Utf16Value(const Utf16Value& uv)
      : utf16_(uv.utf16_),
        size_(uv.size_){}

  
  RASP_INLINE const UC16* value() const {
    return utf16_;
  }

  
  RASP_INLINE size_t size() const {
    return size_;
  }
  
 private:
  const UC16* utf16_;
  size_t size_;
};


class UtfString {
 public:
  UtfString() = default;
  
  
  UtfString(const char* source) {
    std::string st(source);
    Initialize(st);
  }


  UtfString(const std::string& source) {
    Initialize(source);
  }


  UtfString(std::string&& source) {
    Initialize(source);
  }
  
  
  ~UtfString() = default;

  
  UtfString(const UtfString& utf_string)
      : utf_value_cache_(utf_string.utf_value_cache_){}

  
  UtfString(UtfString&& utf_string)
      : utf_value_cache_(std::move(utf_string.utf_value_cache_)){}


  void Initialize(const std::string& str) {
    typedef std::string::const_iterator Iterator;
    Iterator end = str.end();
    UnicodeIteratorAdapter<Iterator> it(str.begin());
    while (it != end) {
      (*this) += *it;
      ++it;
    }
  }


  RASP_INLINE void Clear() {
    utf_value_cache_.Clear();
  }
  

  inline UtfString& operator = (UtfString&& utf_string) {
    utf_value_cache_ = std::move(utf_string.utf_value_cache_);
    return (*this);
  }


  inline UtfString& operator = (const UtfString& utf_string) {
    utf_value_cache_ = utf_string.utf_value_cache_;
    return (*this);
  }


  inline const UtfString& operator += (const UtfString& utf_string) {
    utf_value_cache_.Append(utf_string.utf_value_cache_.utf8_value(),
                            utf_string.utf_value_cache_.utf16_value());
    return (*this);
  }


  inline const UtfString& operator += (const UChar& uchar) {
    Append(uchar);
    return (*this);
  }


  inline const UtfString operator + (const UtfString& utf_string) {
    UtfString copied_utf_string((*this));
    copied_utf_string.utf_value_cache_ = utf_value_cache_;
    copied_utf_string.utf_value_cache_.Append(
        utf_string.utf_value_cache_.utf8_value(),
        utf_string.utf_value_cache_.utf16_value());
    return copied_utf_string;
  }


  inline const UtfString operator + (const UChar& uchar) {
    UtfString copied_utf_string((*this));
    copied_utf_string.utf_value_cache_ = utf_value_cache_;
    copied_utf_string.Append(uchar);
    return copied_utf_string;
  }


  RASP_INLINE Utf8Value ToUtf8Value() const {
    return Utf8Value(utf_value_cache_.utf8_value().c_str(), utf8_length());
  };


  RASP_INLINE Utf16Value ToUtf16Value() const {
    return Utf16Value(utf_value_cache_.utf16_value().c_str(), utf16_length());
  };


  RASP_INLINE size_t utf8_length() const {
    return utf_value_cache_.utf8_value().size();
  }


  RASP_INLINE size_t utf16_length() const {
    return utf_value_cache_.utf16_value().size();
  }
  
 private:

  void Append(const UChar& uchar) {
    utf_value_cache_.append_utf8_value(uchar.utf8());
    if (uchar.IsSurrogatePair()) {
      utf_value_cache_.append_utf16_value(uchar.ToHighSurrogate());
      utf_value_cache_.append_utf16_value(uchar.ToLowSurrogate());
    } else {
      utf_value_cache_.append_utf16_value(uchar.uchar());
    }
  }

  
  UtfValueCache utf_value_cache_;
};

} //namespace rasp

#endif
