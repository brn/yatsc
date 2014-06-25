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


#ifndef PARSER_CHRACTER_H_
#define PARSER_CHRACTER_H_

#include "uchar.h"
#include "../utils/utils.h"
#include "../utils/unicode.h"

namespace yatsc {
class Character : private Static {
 public:

  enum class LineBreakType : uint8_t {
    LF = 0,
    CR,
    CRLF,
    NONE
  };
  
  static bool IsWhiteSpace(const UChar& uchar, const UChar& lookahead) {
    return uchar.IsAscii() &&
        (uchar == unicode::u8(0x09) ||
         uchar == unicode::u8(0x0b) ||
         uchar == unicode::u8(0x0c) ||
         uchar == unicode::u8(0x20) ||
         uchar == unicode::u8(255) ||
         GetLineBreakType(uchar, lookahead) != LineBreakType::NONE ||
         uchar == unicode::u32(0x2028) ||
         uchar == unicode::u32(0x1680) ||
         uchar == unicode::u32(0x180E) ||
         (uchar >= unicode::u32(0x2000) && uchar <= unicode::u32(0x200A)) ||
         uchar == unicode::u32(0x2028) ||
         uchar == unicode::u32(0x2029) ||
         uchar == unicode::u32(0x202F) ||
         uchar == unicode::u32(0x205F) ||
         uchar == unicode::u32(0x3000));
  }


  YATSC_INLINE static bool IsPuncture(const UChar& uchar) {
    return GetCharType(uchar.ToUC8Ascii()) == CharType::PUNCTURES;
  }
  

  YATSC_INLINE static bool IsOperatorStart(const UChar& uchar) {
    return GetCharType(uchar.ToUC8Ascii()) == CharType::OPERATORS;
  }


  static bool IsDigitStart(const UChar& uchar, const UChar& lookahead) {
    return (uchar == unicode::u8('.') && lookahead.IsAscii() && IsNumericLiteral(lookahead)) ||
        (uchar == unicode::u8('0') && lookahead.IsAscii() && lookahead == unicode::u8('x')) ||
        IsNumericLiteral(uchar);
  }

  
  static bool IsStringLiteralStart(const UChar& uchar) {
    return GetCharType(uchar.ToUC8Ascii()) == CharType::QUOTE;
  }

  
  static bool IsHexRange(const UChar& uchar) {
    return IsNumericLiteral(uchar) ||
        (uchar >= unicode::u8('a') && uchar <= unicode::u8('f')) ||
        (uchar >= unicode::u8('A') && uchar <= unicode::u8('F'));
  }


  YATSC_INLINE static bool IsIdentifierStart(const UChar& uchar) {
    return IsIdentifierStartChar(uchar);
  }


  template <typename T>
  static bool IsIdentifierStartChar(T uchar) {
    return GetCharType(static_cast<UC8>(uchar)) == CharType::IDENTIFIER;
  }


  YATSC_INLINE static bool IsUnicodeEscapeSequenceStart(const UChar& uchar, const UChar& lookahead) {
    return uchar == unicode::u8('\\') && lookahead == unicode::u8('u');
  }


  YATSC_INLINE static bool IsInIdentifierRange(const UChar& uchar) {
    return IsIdentifierStart(uchar) || IsNumericLiteral(uchar);
  }

  
  YATSC_INLINE static bool IsNumericLiteral(const UChar& uchar) {
    return GetCharType(uchar.ToUC8Ascii()) == CharType::NUMERIC;
  }


  YATSC_INLINE static bool IsBinaryCharacter(const UChar& uchar) {
    return uchar == unicode::u8('0') || uchar == unicode::u8('1');
  }


  YATSC_INLINE static bool IsSingleLineCommentStart(const UChar& uchar, const UChar& lookahead) {
    return uchar == unicode::u8('/') && lookahead == unicode::u8('/');
  }


  YATSC_INLINE static bool IsMultiLineCommentStart(const UChar& uchar, const UChar& lookahead) {
    return uchar == unicode::u8('/') && lookahead == unicode::u8('*');
  }


  YATSC_INLINE static bool IsMultiLineCommentEnd(const UChar& uchar, const UChar& lookahead) {
    return uchar == unicode::u8('*') && lookahead == unicode::u8('/');
  }


  static LineBreakType GetLineBreakType(const UChar& uchar, const UChar& lookahead) {
    if (uchar == unicode::u8('\n')) {
      return LineBreakType::LF;
    }
    if (uchar == unicode::u8('\r')) {
      if (lookahead == unicode::u8('\n')) {
        return LineBreakType::CRLF;
      }
      return LineBreakType::CR;
    }
    return LineBreakType::NONE;
  }
  
 private:
  enum class CharType: uint8_t {
    OPERATORS,
    PUNCTURES,
    IDENTIFIER,
    QUOTE,
    NUMERIC,
    OTHER
  };

  
  static CharType GetCharType(UC8 ch) {
    static const CharType kChars[] = {
      CharType::OTHER,      // 0
      CharType::OTHER,      // 1
      CharType::OTHER,      // 2
      CharType::OTHER,      // 3
      CharType::OTHER,      // 4
      CharType::OTHER,      // 5
      CharType::OTHER,      // 6
      CharType::OTHER,      // 7
      CharType::OTHER,      // 8
      CharType::OTHER,      // 9
      CharType::OTHER,      // 10
      CharType::OTHER,      // 11
      CharType::OTHER,      // 12
      CharType::OTHER,      // 13
      CharType::OTHER,      // 14
      CharType::OTHER,      // 15
      CharType::OTHER,      // 16
      CharType::OTHER,      // 17
      CharType::OTHER,      // 18
      CharType::OTHER,      // 19
      CharType::OTHER,      // 20
      CharType::OTHER,      // 21
      CharType::OTHER,      // 22
      CharType::OTHER,      // 23
      CharType::OTHER,      // 24
      CharType::OTHER,      // 25
      CharType::OTHER,      // 26
      CharType::OTHER,      // 27
      CharType::OTHER,      // 28
      CharType::OTHER,      // 29
      CharType::OTHER,      // 30
      CharType::OTHER,      // 31
      CharType::OTHER,      // 32
      CharType::OPERATORS,  // 33 !
      CharType::QUOTE,      // 34 '
      CharType::OTHER,      // 35 #
      CharType::IDENTIFIER, // 36 $
      CharType::OPERATORS,  // 37 %
      CharType::OPERATORS,  // 38 &
      CharType::QUOTE,      // 39 '
      CharType::PUNCTURES,  // 40 (
      CharType::PUNCTURES,  // 41 )
      CharType::OPERATORS,  // 42 *
      CharType::OPERATORS,  // 43 +
      CharType::OPERATORS,  // 44 ,
      CharType::OPERATORS,  // 45 -
      CharType::OPERATORS,  // 46 .
      CharType::OPERATORS,  // 47 /
      CharType::NUMERIC,    // 48 0
      CharType::NUMERIC,    // 49 1
      CharType::NUMERIC,    // 50 2
      CharType::NUMERIC,    // 51 3
      CharType::NUMERIC,    // 52 4
      CharType::NUMERIC,    // 53 5
      CharType::NUMERIC,    // 54 6
      CharType::NUMERIC,    // 55 7
      CharType::NUMERIC,    // 56 8
      CharType::NUMERIC,    // 57 9
      CharType::PUNCTURES,  // 58 :
      CharType::OTHER,      // 59 ;
      CharType::OPERATORS,  // 60 <
      CharType::OPERATORS,  // 61 =
      CharType::OPERATORS,  // 62 >
      CharType::PUNCTURES,  // 63 ?
      CharType::OTHER,      // 64 @
      CharType::IDENTIFIER, // 65 A
      CharType::IDENTIFIER, // 66 B
      CharType::IDENTIFIER, // 67 C
      CharType::IDENTIFIER, // 68 D
      CharType::IDENTIFIER, // 69 E
      CharType::IDENTIFIER, // 70 F
      CharType::IDENTIFIER, // 71 G
      CharType::IDENTIFIER, // 72 H
      CharType::IDENTIFIER, // 73 I
      CharType::IDENTIFIER, // 74 J
      CharType::IDENTIFIER, // 75 K
      CharType::IDENTIFIER, // 76 L
      CharType::IDENTIFIER, // 77 M
      CharType::IDENTIFIER, // 78 N
      CharType::IDENTIFIER, // 79 O
      CharType::IDENTIFIER, // 80 P
      CharType::IDENTIFIER, // 81 Q
      CharType::IDENTIFIER, // 82 R
      CharType::IDENTIFIER, // 83 S
      CharType::IDENTIFIER, // 84 T
      CharType::IDENTIFIER, // 85 U
      CharType::IDENTIFIER, // 86 V
      CharType::IDENTIFIER, // 87 W
      CharType::IDENTIFIER, // 88 X
      CharType::IDENTIFIER, // 89 Y
      CharType::IDENTIFIER, // 90 Z
      CharType::PUNCTURES,  // 91 [
      CharType::OTHER,      // 92 BACK_SLASH
      CharType::PUNCTURES,  // 93 ]
      CharType::OPERATORS,  // 94 ^
      CharType::IDENTIFIER, // 95 _
      CharType::OTHER,      // 96 `
      CharType::IDENTIFIER, // 97 a
      CharType::IDENTIFIER, // 98 b
      CharType::IDENTIFIER, // 99 c
      CharType::IDENTIFIER, // 100 d
      CharType::IDENTIFIER, // 101 e
      CharType::IDENTIFIER, // 102 f
      CharType::IDENTIFIER, // 103 g
      CharType::IDENTIFIER, // 104 h
      CharType::IDENTIFIER, // 105 i
      CharType::IDENTIFIER, // 106 j
      CharType::IDENTIFIER, // 107 k
      CharType::IDENTIFIER, // 108 l
      CharType::IDENTIFIER, // 109 m
      CharType::IDENTIFIER, // 110 n
      CharType::IDENTIFIER, // 111 o
      CharType::IDENTIFIER, // 112 p
      CharType::IDENTIFIER, // 113 q
      CharType::IDENTIFIER, // 114 r
      CharType::IDENTIFIER, // 115 s
      CharType::IDENTIFIER, // 116 t
      CharType::IDENTIFIER, // 117 u
      CharType::IDENTIFIER, // 118 v
      CharType::IDENTIFIER, // 119 w
      CharType::IDENTIFIER, // 120 x
      CharType::IDENTIFIER, // 121 y
      CharType::IDENTIFIER, // 122 z
      CharType::PUNCTURES,  // 123 {
      CharType::OPERATORS,  // 124 |
      CharType::PUNCTURES,  // 125 }
      CharType::OPERATORS,  // 126 ~
      CharType::OTHER       // 127 DEL
    };

    return (ch <= 127 && ch > 0)? kChars[ch] : CharType::OTHER;
  }
};
} //namespace yatsc

#endif
