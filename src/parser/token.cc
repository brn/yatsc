/*
 * The MIT License (MIT)
 * 
 * Copyright (c) Taketoshi Aono(brn)
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


#include <type_traits>
#include "../utils/utils.h"
#include "token.h"

namespace yatsc {

// Ascii range puncture list.
// For fast lookup.
static const Token kPunctures[] = {
  Token::ILLEGAL,          // 0
  Token::ILLEGAL,          // 1
  Token::ILLEGAL,          // 2
  Token::ILLEGAL,          // 3
  Token::ILLEGAL,          // 4
  Token::ILLEGAL,          // 5
  Token::ILLEGAL,          // 6
  Token::ILLEGAL,          // 7
  Token::ILLEGAL,          // 8
  Token::ILLEGAL,          // 9
  Token::ILLEGAL,          // 10
  Token::ILLEGAL,          // 11
  Token::ILLEGAL,          // 12
  Token::ILLEGAL,          // 13
  Token::ILLEGAL,          // 14
  Token::ILLEGAL,          // 15
  Token::ILLEGAL,          // 16
  Token::ILLEGAL,          // 17
  Token::ILLEGAL,          // 18
  Token::ILLEGAL,          // 19
  Token::ILLEGAL,          // 20
  Token::ILLEGAL,          // 21
  Token::ILLEGAL,          // 22
  Token::ILLEGAL,          // 23
  Token::ILLEGAL,          // 24
  Token::ILLEGAL,          // 25
  Token::ILLEGAL,          // 26
  Token::ILLEGAL,          // 27
  Token::ILLEGAL,          // 28
  Token::ILLEGAL,          // 29
  Token::ILLEGAL,          // 30
  Token::ILLEGAL,          // 31
  Token::ILLEGAL,          // 32
  Token::ILLEGAL,          // 33 !
  Token::ILLEGAL,          // 34 '
  Token::ILLEGAL,          // 35 #
  Token::ILLEGAL,          // 36 $
  Token::ILLEGAL,          // 37 %
  Token::ILLEGAL,          // 38 &
  Token::ILLEGAL,          // 39 '
  Token::TS_LEFT_PAREN,    // 40 (
  Token::TS_RIGHT_PAREN,   // 41 )
  Token::ILLEGAL,          // 42 *
  Token::ILLEGAL,          // 43 +
  Token::ILLEGAL,          // 44 ,
  Token::ILLEGAL,          // 45 -
  Token::ILLEGAL,          // 46 .
  Token::ILLEGAL,          // 47 /
  Token::ILLEGAL,          // 48 0
  Token::ILLEGAL,          // 49 1
  Token::ILLEGAL,          // 50 2
  Token::ILLEGAL,          // 51 3
  Token::ILLEGAL,          // 52 4
  Token::ILLEGAL,          // 53 5
  Token::ILLEGAL,          // 54 6
  Token::ILLEGAL,          // 55 7
  Token::ILLEGAL,          // 56 8
  Token::ILLEGAL,          // 57 9
  Token::TS_COLON,         // 58 :
  Token::ILLEGAL,          // 59 ;
  Token::ILLEGAL,          // 60 <
  Token::ILLEGAL,          // 61 =
  Token::ILLEGAL,          // 62 >
  Token::TS_QUESTION_MARK, // 63 ?
  Token::ILLEGAL,          // 64 @
  Token::ILLEGAL,          // 65 A
  Token::ILLEGAL,          // 66 B
  Token::ILLEGAL,          // 67 C
  Token::ILLEGAL,          // 68 D
  Token::ILLEGAL,          // 69 E
  Token::ILLEGAL,          // 70 F
  Token::ILLEGAL,          // 71 G
  Token::ILLEGAL,          // 72 H
  Token::ILLEGAL,          // 73 I
  Token::ILLEGAL,          // 74 J
  Token::ILLEGAL,          // 75 K
  Token::ILLEGAL,          // 76 L
  Token::ILLEGAL,          // 77 M
  Token::ILLEGAL,          // 78 N
  Token::ILLEGAL,          // 79 O
  Token::ILLEGAL,          // 80 P
  Token::ILLEGAL,          // 81 Q
  Token::ILLEGAL,          // 82 R
  Token::ILLEGAL,          // 83 S
  Token::ILLEGAL,          // 84 T
  Token::ILLEGAL,          // 85 U
  Token::ILLEGAL,          // 86 V
  Token::ILLEGAL,          // 87 W
  Token::ILLEGAL,          // 88 X
  Token::ILLEGAL,          // 89 Y
  Token::ILLEGAL,          // 90 Z
  Token::TS_LEFT_BRACKET,  // 91 [
  Token::ILLEGAL,          // 92 BACK_SLASH
  Token::TS_RIGHT_BRACKET, // 93 ]
  Token::ILLEGAL,          // 94 ^
  Token::ILLEGAL,          // 95 _
  Token::ILLEGAL,          // 96 `
  Token::ILLEGAL,          // 97 a
  Token::ILLEGAL,          // 98 b
  Token::ILLEGAL,          // 99 c
  Token::ILLEGAL,          // 100 d
  Token::ILLEGAL,          // 101 e
  Token::ILLEGAL,          // 102 f
  Token::ILLEGAL,          // 103 g
  Token::ILLEGAL,          // 104 h
  Token::ILLEGAL,          // 105 i
  Token::ILLEGAL,          // 106 j
  Token::ILLEGAL,          // 107 k
  Token::ILLEGAL,          // 108 l
  Token::ILLEGAL,          // 109 m
  Token::ILLEGAL,          // 110 n
  Token::ILLEGAL,          // 111 o
  Token::ILLEGAL,          // 112 p
  Token::ILLEGAL,          // 113 q
  Token::ILLEGAL,          // 114 r
  Token::ILLEGAL,          // 115 s
  Token::ILLEGAL,          // 116 t
  Token::ILLEGAL,          // 117 u
  Token::ILLEGAL,          // 118 v
  Token::ILLEGAL,          // 119 w
  Token::ILLEGAL,          // 120 x
  Token::ILLEGAL,          // 121 y
  Token::ILLEGAL,          // 122 z
  Token::TS_LEFT_BRACE,    // 123 {
  Token::ILLEGAL,          // 124 |
  Token::TS_RIGHT_BRACE,   // 125 }
  Token::ILLEGAL,          // 126 ~
  Token::ILLEGAL           // 127 DEL
};


// Borrowed from v8 javascript engine.
#define KEYWORDS(KEYWORD_GROUP, KEYWORD)                                \
  KEYWORD_GROUP('b')                                                    \
  KEYWORD("break", Token::TS_BREAK)                                     \
  KEYWORD_GROUP('c')                                                    \
  KEYWORD("case", Token::TS_CASE)                                       \
  KEYWORD("catch", Token::TS_CATCH)                                     \
  KEYWORD("class", Token::TS_CLASS)                                     \
  KEYWORD("const", LanguageModeUtil::IsES6(co)? Token::TS_CONST: Token::FUTURE_RESERVED_WORD) \
  KEYWORD("continue", Token::TS_CONTINUE)                               \
  KEYWORD_GROUP('d')                                                    \
  KEYWORD("debugger", Token::TS_DEBUGGER)                               \
  KEYWORD("declare", Token::TS_DECLARE)                                 \
  KEYWORD("default", Token::TS_DEFAULT)                                 \
  KEYWORD("delete", Token::TS_DELETE)                                   \
  KEYWORD("do", Token::TS_DO)                                           \
  KEYWORD_GROUP('e')                                                    \
  KEYWORD("else", Token::TS_ELSE)                                       \
  KEYWORD("enum", Token::TS_ENUM)                                       \
  KEYWORD("export", Token::TS_EXPORT)                                   \
  KEYWORD("extends", Token::TS_EXTENDS)                                 \
  KEYWORD_GROUP('f')                                                    \
  KEYWORD("false", Token::TS_FALSE)                                     \
  KEYWORD("finally", Token::TS_FINALLY)                                 \
  KEYWORD("for", Token::TS_FOR)                                         \
  KEYWORD("function", Token::TS_FUNCTION)                               \
  KEYWORD_GROUP('i')                                                    \
  KEYWORD("if", Token::TS_IF)                                           \
  KEYWORD("implements", Token::TS_IMPLEMENTS)                           \
  KEYWORD("import", Token::TS_IMPORT)                                   \
  KEYWORD("in", Token::TS_IN)                                           \
  KEYWORD("instanceof", Token::TS_INSTANCEOF)                           \
  KEYWORD("interface", Token::TS_INTERFACE)                             \
  KEYWORD_GROUP('l')                                                    \
  KEYWORD("let", LanguageModeUtil::IsES6(co)? Token::TS_LET: Token::TS_IDENTIFIER) \
  KEYWORD_GROUP('m')                                                    \
  KEYWORD("module", ModuleTypeUtil::IsModuleKeywordAllowed(co)? Token::TS_MODULE: Token::TS_IDENTIFIER) \
  KEYWORD_GROUP('n')                                                    \
  KEYWORD("new", Token::TS_NEW)                                         \
  KEYWORD("null", Token::TS_NULL)                                       \
  KEYWORD_GROUP('N')                                                    \
  KEYWORD("NaN", Token::TS_NAN)                                         \
  KEYWORD_GROUP('p')                                                    \
  KEYWORD("package", LanguageModeUtil::IsFutureReservedWord(co)?        \
          Token::FUTURE_STRICT_RESERVED_WORD: Token::TS_IDENTIFIER)     \
  KEYWORD("private", Token::TS_PRIVATE)                                 \
  KEYWORD("protected", Token::TS_PROTECTED)                             \
  KEYWORD("public", Token::TS_PUBLIC)                                   \
  KEYWORD_GROUP('r')                                                    \
  KEYWORD("return", Token::TS_RETURN)                                   \
  KEYWORD_GROUP('s')                                                    \
  KEYWORD("static", Token::TS_STATIC)                                   \
  KEYWORD("super", Token::TS_SUPER)                                     \
  KEYWORD("switch", Token::TS_SWITCH)                                   \
  KEYWORD_GROUP('t')                                                    \
  KEYWORD("this", Token::TS_THIS)                                       \
  KEYWORD("throw", Token::TS_THROW)                                     \
  KEYWORD("true", Token::TS_TRUE)                                       \
  KEYWORD("try", Token::TS_TRY)                                         \
  KEYWORD("typeof", Token::TS_TYPEOF)                                   \
  KEYWORD_GROUP('u')                                                    \
  KEYWORD("undefined", Token::TS_UNDEFINED)                             \
  KEYWORD_GROUP('v')                                                    \
  KEYWORD("var", Token::TS_VAR)                                         \
  KEYWORD("void", Token::TS_VOID)                                       \
  KEYWORD_GROUP('w')                                                    \
  KEYWORD("while", Token::TS_WHILE)                                     \
  KEYWORD("with", Token::TS_WITH)                                       \
  KEYWORD_GROUP('y')                                                    \
  KEYWORD("yield", LanguageModeUtil::IsES6(co)? Token::TS_YIELD: Token::TS_IDENTIFIER)


// Get Identifier type from string.
Token TokenInfo::GetIdentifierType(const char* maybe_keyword, const CompilerOption& co) {
  const size_t input_length = Strlen(maybe_keyword);
  const int min_length = 2;
  const int max_length = 10;
  if (input_length < min_length || input_length > max_length) {
    return Token::TS_IDENTIFIER;
  }
  
  // Borrowed from v8 javascript engine.
  switch (maybe_keyword[0]) {
    default:
#define KEYWORD_GROUP_CASE(ch)                  \
      break;                                    \
    case ch:
#define KEYWORD(keyword, token)                                         \
      {                                                                 \
        const int keyword_length = sizeof(keyword) - 1;                 \
        static_assert(keyword_length >= min_length, "The length of the keyword must be greater than 2"); \
        static_assert(keyword_length <= max_length, "The length of the keyword mst be less than 10"); \
        if (input_length == keyword_length &&                           \
            maybe_keyword[1] == keyword[1] &&                           \
            (keyword_length <= 2 || maybe_keyword[2] == keyword[2]) &&  \
            (keyword_length <= 3 || maybe_keyword[3] == keyword[3]) &&  \
            (keyword_length <= 4 || maybe_keyword[4] == keyword[4]) &&  \
            (keyword_length <= 5 || maybe_keyword[5] == keyword[5]) &&  \
            (keyword_length <= 6 || maybe_keyword[6] == keyword[6]) &&  \
            (keyword_length <= 7 || maybe_keyword[7] == keyword[7]) &&  \
            (keyword_length <= 8 || maybe_keyword[8] == keyword[8]) &&  \
            (keyword_length <= 9 || maybe_keyword[9] == keyword[9])) {  \
          return token;                                                 \
        }                                                               \
      }
      KEYWORDS(KEYWORD_GROUP_CASE, KEYWORD)
        }
  return Token::TS_IDENTIFIER;
}


// Get puncture type from char.
Token TokenInfo::GetPunctureType(const UChar& uchar) {
  return kPunctures[uchar.ToUC8Ascii()];
}


uint8_t GetOperandPriority(Token t) {
  switch (t) {
    case Token::TS_MOD:
    case Token::TS_DIV:
    case Token::TS_MUL:
      return 1;
    case Token::TS_PLUS:
    case Token::TS_MINUS:
      return 2;
    case Token::TS_SHIFT_RIGHT:
    case Token::TS_U_SHIFT_RIGHT:
    case Token::TS_SHIFT_LEFT:
      return 3;
    case Token::TS_LESS:
    case Token::TS_GREATER:
    case Token::TS_GREATER_EQUAL:
    case Token::TS_LESS_EQUAL:
    case Token::TS_IN:
    case Token::TS_INSTANCEOF:
      return 4;
    case Token::TS_EQ:
    case Token::TS_EQUAL:
    case Token::TS_NOT_EQ:
    case Token::TS_NOT_EQUAL:
      return 5;
    case Token::TS_BIT_AND:
      return 6;
    case Token::TS_BIT_XOR:
      return 7;
    case Token::TS_BIT_OR:
      return 8;
    case Token::TS_LOGICAL_AND:
      return 9;
    case Token::TS_LOGICAL_OR:
      return 10;
    default:
      return 0;
  }
}


TokenInfo TokenInfo::kNullToken;
}
