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

#ifndef PARSER_LITERLBUFFER_H
#define PARSER_LITERLBUFFER_H

#include "../utils/stl.h"
#include "../utils/utils.h"
#include "./utfstring.h"

namespace yatsc {

class Literal {
 public:
  Literal(const UtfString& utf_string)
      : id_ (Unique::id()),
        value_(utf_string) {}


  Literal(Literal&& literal)
      : id_(literal.id_),
        value_(std::move(literal.value_)) {}

  
  Literal(const Literal& literal)
      : id_(literal.id_),
        value_(literal.value_) {}


  YATSC_CONST_GETTER(Unique::Id, id, id_);


  bool Equals(Literal* literal) YATSC_NO_SE {
    return literal->id_ == id_;
  }


  bool Equals(const Literal* literal) YATSC_NO_SE {
    return literal->id_ == id_;
  }


  bool Equals(Literal& literal) YATSC_NO_SE {
    return literal.id_ == id_;
  }


  bool Equals(const Literal& literal) YATSC_NO_SE {
    return literal.id_ == id_;
  }


  template <typename T>
  bool Equals(T value) YATSC_NO_SE {
    return value_ == value;
  }
  

  const Utf8String& utf8_string() YATSC_NO_SE {return value_.utf8_string();}

  
  const Utf16String& utf16_string() YATSC_NO_SE {return value_.utf16_string();}


  const char* utf8_value() YATSC_NO_SE {return value_.utf8_value();}

  
  const UC16* utf16_value() YATSC_NO_SE {return value_.utf16_value();}

  
  size_t utf8_length() YATSC_NO_SE {return value_.utf8_length();}

  
  size_t utf16_length() YATSC_NO_SE {return value_.utf8_length();}
  
 private:
  Unique::Id id_;
  UtfString value_;
};


class LiteralBuffer: private Unmovable, private Uncopyable {
  typedef HashMap<Utf16String, Literal> UtfStringBuffer;
 public:
  LiteralBuffer() = default;

  
  Literal* InsertValue(const UtfString& utf_string) {
    ScopedSpinLock lock(lock_);
    UtfStringBuffer::iterator found = buffer_.find(utf_string.utf16_string());
    if (found != buffer_.end()) {
      return &(found->second);
    }
    auto ret = buffer_.insert(std::make_pair(utf_string.utf16_string(), Literal(utf_string)));
    return &(ret.first->second);
  }
  
 private:
  SpinLock lock_;
  UtfStringBuffer buffer_;
};
}

#endif
