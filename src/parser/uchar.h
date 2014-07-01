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

#ifndef PARSER_UCHAR_H_
#define PARSER_UCHAR_H_

#include <string>
#include <stdexcept>
#include "../utils/utils.h"
#include "../utils/os.h"
#include "../utils/unicode.h"


namespace yatsc {
/**
 * Utf-32 representation class.
 */
class UChar {
 public:

  YATSC_INLINE static UChar Null() {
    UC8Bytes b {{'\0'}};
    return UChar(unicode::u32('\0'), b);
  }
  
  /**
   * Constructor
   * @param c utf-32 byte.
   * @param utf8 utf-8 byte sequence.
   */
  YATSC_INLINE explicit UChar(UC32 c, const UC8Bytes& utf8):
      uchar_(c),
      utf8_(utf8){};


  /**
   * Constructor
   * Represent invalid utf-8 sequence.
   */
  YATSC_INLINE UChar(): uchar_(0) {}
  

  /**
   * Copy constructor.
   */
  YATSC_INLINE UChar(const UChar& uchar):
      uchar_(uchar.uchar_), utf8_(uchar.utf8_) {}
  

  ~UChar() = default;


  /**
   * Assignment
   */
  YATSC_INLINE const UChar& operator = (const UChar& uchar) {
    uchar_ = uchar.uchar_;
    utf8_ = uchar.utf8_;
    return *(this);
  }
  

  /**
   * Converters
   */
  YATSC_INLINE explicit operator bool () const {return !IsInvalid();}
  YATSC_INLINE explicit operator UC16 () const {return uchar();}
  YATSC_INLINE explicit operator UC8 () const {return ToUC8Ascii();}
  YATSC_INLINE explicit operator int () const {return uchar();}
  YATSC_INLINE explicit operator const char* () const {return utf8();}
  YATSC_INLINE bool operator == (const UChar& uc) const {
    return uc.uchar_ == uchar_;
  }


  YATSC_INLINE bool operator == (const UC8 uc) const {
    return uc == ToAscii();
  }


  YATSC_INLINE bool operator == (const UC32 uc) const {
    return uc == uchar_;
  }


  YATSC_INLINE bool operator != (const UChar& uc) const {
    return uc.uchar_ == uchar_;
  }
  
  
  YATSC_INLINE bool operator != (const UC8 uc) const {
    return uc != ToAscii();
  }


  YATSC_INLINE bool operator != (const UC32 uc) const {
    return uc != uchar_;
  }


  inline const UChar operator + (const UC8 uc) const {
    char next = ToAscii() + static_cast<char>(uc);
    UC8Bytes b{{next, '\0'}};
    return UChar(next, b);
  }


  inline const UChar operator - (const UC8 uc) const {
    UC8 c = ToAscii();
    if (c < uc) {
      throw std::out_of_range("Attempted to subtract by invalid ascii character.");
    }
    char next = ToAscii() - static_cast<char>(uc);
    UC8Bytes b{{next, '\0'}};
    return UChar(next, b);
  }
  

  YATSC_INLINE bool operator > (const UChar& uchar) const {
    return uchar_ > uchar.uchar_;
  }


  YATSC_INLINE bool operator >= (const UChar& uchar) const {
    return uchar_ >= uchar.uchar_;
  }


  YATSC_INLINE bool operator < (const UChar& uchar) const {
    return uchar_ < uchar.uchar_;
  }


  YATSC_INLINE bool operator <= (const UChar& uchar) const {
    return uchar_ <= uchar.uchar_;
  }
  

  YATSC_INLINE bool operator > (const UC8 uc) const {
    return ToAscii() > uc;
  }


  YATSC_INLINE bool operator >= (const UC8 uc) const {
    return ToAscii() >= uc;
  }


  YATSC_INLINE bool operator < (const UC8 uc) const {
    return ToAscii() < uc;
  }


  YATSC_INLINE bool operator <= (const UC8 uc) const {
    return ToAscii() <= uc;
  }


  YATSC_INLINE bool operator > (const UC32 uc) const {
    return uchar_ > uc;
  }


  YATSC_INLINE bool operator >= (const UC32 uc) const {
    return uchar_ >= uc;
  }


  YATSC_INLINE bool operator < (const UC32 uc) const {
    return uchar_ < uc;
  }


  YATSC_INLINE bool operator <= (const UC32 uc) const {
    return uchar_ <= uc;
  }

  
  /**
   * Return ascii char.
   * @return ascii char.
   */
  YATSC_INLINE char ToAscii() const {return static_cast<char>(uchar_);}


  /**
   * Return ascii UC8 char.
   * @return ascii UC8 char.
   */
  YATSC_INLINE UC8 ToUC8Ascii() const {return static_cast<UC8>(uchar_);}
  

  /**
   * Check whether utf-16 byte sequence is surrogate pair or not.
   * @return true(if surrogate pair) false(if not surrogate pair)
   */
  YATSC_INLINE bool IsSurrogatePair() const {
    return utf16::IsSurrogatePairUC32(uchar_);
  }
  

  /**
   * Convert utf-16 byte sequence to high surrogate byte.
   * @return utf-16 byte which represent high surrogate byte.
   */
  YATSC_INLINE UC16 ToHighSurrogate() const {
    return utf16::ToHighSurrogateUC32(uchar_);
  }


  /**
   * Convert utf-16 byte sequence to low surrogate byte.
   * @return utf-16 byte which represent low surrogate byte.
   */
  YATSC_INLINE UC16 ToLowSurrogate() const {
    return utf16::ToLowSurrogateUC32(uchar_);
  }
  

  /**
   * Check whether utf-16 byte sequence is invalid or not.
   * @return true(if invalid) false(if valid)
   */
  YATSC_INLINE bool IsInvalid() const {
    return uchar_ == 0;
  }


  /**
   * Check whether utf-16 byte sequence is within ascii range or not.
   * @return true(if within ascii range) false(if out of ascii range)
   */
  YATSC_INLINE bool IsAscii() const {return utf8::IsAscii(uchar_);}


  /**
   * Return utf-16 byte sequence.
   * @return utf-16 byte
   */
  YATSC_INLINE UC16 uchar() const {return static_cast<UC16>(uchar_);}


  /**
   * Return utf-8 char array that represent this utf-16 byte sequence.
   * @return utf-8 char array.
   */
  YATSC_INLINE const char* utf8() const {return utf8_.data();}


  YATSC_INLINE size_t utf16_length() const {
    return IsSurrogatePair()? 2: 1;
  }


  YATSC_INLINE size_t utf8_length() const {
    for (size_t i = 0; i < utf8_.size(); i++) {
      if (utf8_[i] == '\0') {
        return i;
      }
    }
    return utf8_.size();
  }

 private:
  
  UC32 uchar_;
  UC8Bytes utf8_;
};
}


#endif
