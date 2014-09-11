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

#ifndef UTILS_UNICODE_H_
#define UTILS_UNICODE_H_

#include <assert.h>
#include <stdint.h>
#include <array>
#include <string>
#include "utils.h"
#include "regions.h"

namespace yatsc {
typedef uint32_t UC32;
typedef uint16_t UC16;
typedef uint8_t UC8;
typedef std::array<char, 5> UC8Bytes;
typedef std::basic_string<char, std::char_traits<char>, RegionsStandardAllocator<char>> Utf8String;
typedef std::basic_string<UC16, std::char_traits<UC16>, RegionsStandardAllocator<UC16>> Utf16String;

/**
 * Utility class of the unicode string conversion.
 */
namespace unicode {
const UC32 kUtf16Max = 0xFFFF;
const UC32 kSurrogateBits = 10;
const UC32 kHighSurrogateMin = 0xD800;
const UC32 kHighSurrogateMax = 0xDBFF;
const UC32 kHighSurrogateMask = (1 << kSurrogateBits) - 1;
const UC32 kHighSurrogateOffset = kHighSurrogateMin - (0x10000 >> 10);

const UC32 kLowSurrogateMin = 0xDC00;
const UC32 kLowSurrogateMax = 0xDFFF;
const UC32 kLowSurrogateMask = (1 << kSurrogateBits) - 1;
const UC32 kSurrogateMin = kHighSurrogateMin;
const UC32 kSurrogateMax = kLowSurrogateMax;
const UC32 kSurrogateMask = (1 << (kSurrogateBits + 1)) - 1;

const UC32 kUnicodeMin = 0x000000;
const UC32 kUnicodeMax = 0x10FFFF;

const UC8 kAsciiMax = 0x7F;
  
template <typename T>
YATSC_INLINE UC32 u32(T uc) {
  return static_cast<UC32>(uc);
}


template <typename T>
YATSC_INLINE UC32 u16(T uc) {
  return static_cast<UC32>(uc);
}
  

template <typename T>
YATSC_INLINE UC8 u8(T uc) {
  return static_cast<UC8>(uc);
}


/**
 * Bit mask.
 * @return masked bit.
 */
template<std::size_t N, typename CharT>
YATSC_INLINE CharT Mask(CharT ch) {
  return Bitmask<N, UC32>::lower & ch;
}
} //namespace unicode


/**
 * Utility class of the utf-16 char.
 */
namespace utf16 {

/**
 * Check whether a utf-32 char is surrogate pair or not.
 * @param uc utf-32 byte.
 * @return true(if surrogate pair) false(if not surrogate pair).
 */
YATSC_INLINE bool IsSurrogatePairUC32(UC32 uc) {
  return uc > unicode::kUtf16Max;
}


/**
 * Check whether a utf-16 char is surrogate pair or not.
 * @param uc utf-16 byte.
 * @return true(if surrogate pair) false(if not surrogate pair).
 */
YATSC_INLINE bool IsSurrogatePairUC16(UC16 uc) {
  return (uc & ~unicode::kSurrogateMask) == unicode::kSurrogateMin;
}

  
/**
 * Convert to high surrogate pair byte from a utf-32 surrogate pair byte.
 * @param uc utf-32 byte.
 * @return UC16 high surrogate pair byte expression.
 */
YATSC_INLINE UC16 ToHighSurrogateUC32(UC32 uc) {
  return static_cast<UC16>((uc >> unicode::kSurrogateBits) + unicode::kHighSurrogateOffset);
}


/**
 * Convert to low surrogate pair byte from a utf-32 surrogate pair byte.
 * @param uc utf-32 byte.
 * @return UC16 low surrogate pair byte expression.
 */
YATSC_INLINE UC16 ToLowSurrogateUC32(UC32 uc) {
  return static_cast<UC16>((uc & unicode::kLowSurrogateMask) + unicode::kLowSurrogateMin);
}


/**
 * Check whether a utf-32 char is not surrogate pair or not.
 * @param uc utf-32 byte.
 * @return true(if not surrogate pair) false(if surrogate pair)
 */
YATSC_INLINE bool IsOutOfSurrogateRange(UC32 uc) {
  return uc < unicode::kHighSurrogateMin || unicode::kLowSurrogateMax < uc;
}


/**
 * Check whether a utf-16 char is high surrogate pair or not.
 * @param uc utf-16 byte.
 * @return true(if high surrogate pair) false(if not high surrogate pair or not surrogate pair)
 */
YATSC_INLINE bool IsHighSurrogateUC16(UC16 uc) {
  if (!IsSurrogatePairUC16(uc)) return false;
  return (uc & ~unicode::kHighSurrogateMask) == unicode::kHighSurrogateMin;
}


/**
 * Check whether a utf-16 char is low surrogate pair or not.
 * @param uc utf-16 byte.
 * @return true(if low surrogate pair) false(if not low surrogate pair or not surrogate pair)
 */
YATSC_INLINE bool IsLowSurrogateUC16(UC16 uc) {
  if (!IsSurrogatePairUC16(uc)) return false;
  return (uc & ~unicode::kLowSurrogateMask) == unicode::kLowSurrogateMin;
}


class Convertor : private Static {
 public:
  static UC8Bytes Convert(UC16 uc, UC16 trail) {
    UC32 result = unicode::Mask<16>(uc);
    if (IsSurrogatePairUC16(result)) {
      if (IsHighSurrogateUC16(result)) {
        if (trail == 0) {
          return UC8Bytes();
        }
        const UC32 low = unicode::Mask<16>(trail);
        if (!IsLowSurrogateUC16(low)) {
          return UC8Bytes();
        }
        result = UC16ToUC32SurrogatePair(result, low);
      } else {
        return UC8Bytes();
      }
    }
    return ConvertUtf32ToUtf8(result);
  }
  
 private:

  YATSC_INLINE static UC32 UC16ToUC32SurrogatePair(UC16 high, UC16 low) {
    using namespace unicode;
    return (u32(high & kHighSurrogateMask) << kSurrogateBits)
        + u32(low & kLowSurrogateMask) + 0x10000;
  }

  
  static UC8Bytes ConvertUtf32ToUtf8(UC32 uc) {
    using namespace unicode;
    UC8Bytes b;
    if (uc < 0x80) {
      // 0000 0000-0000 007F | 0xxxxxxx
      b[0] = u8(uc);
      b[1] = '\0';
    } else if (uc < 0x800) {
      // 0000 0080-0000 07FF | 110xxxxx 10xxxxxx
      b[0] = u8((uc >> 6) | 0xC0);
      b[1] = u8((uc & 0x3F) | 0x80);
      b[2] = '\0';
    } else if (uc < 0x10000) {
      // 0000 0800-0000 FFFF | 1110xxxx 10xxxxxx 10xxxxxx
      b[0] = u8((uc >> 12) | 0xE0);
      b[1] = u8(((uc >> 6) & 0x3F) | 0x80);
      b[2] = u8((uc & 0x3F) | 0x80);
      b[3] = '\0';
    } else {
      assert(uc <= kUnicodeMax);
      // 0001 0000-0010 FFFF | 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
      b[0] = u8((uc >> 18) | 0xF0);
      b[1] = u8(((uc >> 12) & 0x3F) | 0x80);
      b[2] = u8(((uc >> 6) & 0x3F) | 0x80);
      b[3] = u8((uc & 0x3F) | 0x80);
      b[4] = '\0';
    }
    return b;
  }
};

} //namespace utf16


/**
 * The utility class of the utf-8 char.
 */
namespace utf8 {
/**
 * Return byte size of the utf-8 sequence.
 * @param uc utf-8 byte.
 * @return The byte size of utf-8 sequence.
 */
YATSC_INLINE size_t GetByteCount(UC8 uc) {
  static const std::array<UC8, UINT8_MAX + 1> kLengthMap = { {
      1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,  // 00000000 -> 00011111
      1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,  // 00100000 -> 00111111
      1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,  // 01000000 -> 01011111
      1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,  // 01100000 -> 01111111  ASCII range end
      0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,  // 10000000 -> 10011111  invalid
      0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,  // 10100000 -> 10111111  invalid
      2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,  // 11000000 -> 11011111  2 bytes range
      3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,                                  // 11100000 -> 11101111  3 bytes range
      4,4,4,4,4,4,4,4,                                                  // 11110000 -> 11110111  4 bytes range
      0,0,0,0,0,0,0,0                                                   // 11111000 -> 11111111  invalid
    } };
  return kLengthMap[uc];
}
    

/**
 * Check whether utf-8 byte is null or not.
 * @param uc utf-8 byte
 * @return true(if utf-8 byte is not null) false(if utf-8 byte is null)
 */
YATSC_INLINE bool IsNotNull(UC8 uc) {
  return uc != '\0';
}


/**
 * Checking whether utf-8 char is valid or not.
 * 1. Checking null char(\0)
 * 2. Checking the utf-8 byte that within the ascii range is not expressed as
 * more than one byte sequence.
 * @param uc utf-8 byte
 * @return true(if utf-8 byte is valid) false(if utf-8 byte is invalid)
 */
YATSC_INLINE bool IsValidSequence(UC8 uc) {
  return IsNotNull(uc) && (uc & 0xC0) == 0x80;
}


/**
 * Check whether utf-8 byte is within the ascii range.
 * @param uc utf-8 byte or char or int.
 * @return true(if utf-8 byte is ascii) false(if utf-8 byte is not ascii)
 */
template <typename T>
YATSC_INLINE bool IsAscii(T uc) {return uc >= 0 && uc < unicode::kAsciiMax;}
} //namespace utf8

} //namespace yatsc
#endif
