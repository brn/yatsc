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

#include "./unicode.h"
#include "../utils/bytelen.h"

namespace yatsc {
Unicode::Unicode(const char* source)
    : current_position_(0),
      line_number_(1),
      source_(source),
      piece_size_(0){
  source_size_ = BYTELEN(source_);
}


UChar Unicode::Next() {
  size_t byte_count = GetUTF8ByteCount(Peek(0));
  uint32_t next = ConvertUtf8ToUcs2(byte_count);
  piece_[piece_size_] = '\0';
  piece_size_ = 0;
  if (next > SURROGATE_MAX) {
    printf("surrogate\n");
    // surrogate pair only ch > 0xFFFF
    // because NextUCS4FromUTF8 checks code point is valid
    /**result++ = ToHighSurrogate(res);
     *result++ = ToLowSurrogate(res);*/
    return UChar();
  } else if (next != 0) {
    return UChar(static_cast<uint16_t>(next), piece_);
  }
  return UChar();
}


uint32_t Unicode::ConvertUtf8ToUcs2(size_t byte_count) {
  switch (byte_count) {
    case 1:
      return ConvertAscii();
    case 2:
      return Convert2Byte();
    case 3:
      return Convert3Byte();
    case 4:
      return Convert4Byte();
    default:
      return 0;
  }
}


uint8_t Unicode::Advance() {
  if (current_position_ >= source_size_) {
    return '\0';
  }
  uint8_t next = source_[current_position_];
  current_position_++;
  if (next == '\n') {
    line_number_++;
  }
  return next;
}


uint32_t Unicode::Convert2Byte() {
  const uint8_t MINIMUN_RANGE = 0x00000080;
  char c = Peek();
  if (c != '\0') {
    uint32_t next = Mask<5>(Append(Advance())) << 6;
    c = Peek();
    if (c != '\0') {
      if (IsTrail(c)) {
        next = next | Mask<6>(Append(Advance()));
        if (next > MINIMUN_RANGE) {
          return next;
        }
      }
    }
  }
  return 0;
}


uint32_t Unicode::Convert3Byte() {
  const int MINIMUN_RANGE = 0x00000800;
  char c = Advance();
  if (c != '\0') {
    uint32_t next = Mask<4>(Append(c)) << 12;
    c = Advance();
    if (c != '\0') {
      if (IsTrail(c)) {
        next = next | Mask<6>(Append(c)) << 6;
        c = Advance();
        if (c != '\0') {
          if (IsTrail(c)) {
            next = next | Mask<6>(Append(c));
            if (next > MINIMUN_RANGE) {
              if (next < HIGH_SURROGATE_MIN || LOW_SURROGATE_MAX < next) {
                return next;
              }
            }
          }
        }
      }
    }
  }
  return 0;
}


uint32_t Unicode::Convert4Byte() {
  const int MINIMUN_RANGE = 0x000010000;
  char c = Peek();
  if (c != '\0') {
    uint32_t next = Mask<3>(Append(Advance())) << 18;
    c = Peek();
    if (c != '\0') {
      if (IsTrail(c)) {
        next = next | Mask<6>(Append(Advance())) << 12;
        c = Peek();
        if (c != '\0') {
          if (IsTrail(c)) {
            next = next | Mask<6>(Append(Advance())) << 6;
            c = Peek();
            if (c != '\0') {
              if (IsTrail(c)) {
                next = next | Mask<6>(Append(Advance()));
                if (next >= MINIMUN_RANGE) {
                  if (next <= 0x10FFFF) {
                    return next;
                  }
                }
              }
            }
          }
        }
      }
    }
  }
  return 0;
}


const size_t Unicode::LENGTH_ARRAY[16] = {
  0,0,0,0,0,0,0,0,0,0,0,0,2,0,3,4
};
}
