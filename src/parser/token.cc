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

namespace rasp {

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
  Token::JS_LEFT_PAREN,    // 40 (
  Token::JS_RIGHT_PAREN,   // 41 )
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
  Token::JS_COLON,         // 58 :
  Token::ILLEGAL,          // 59 ;
  Token::ILLEGAL,          // 60 <
  Token::ILLEGAL,          // 61 =
  Token::ILLEGAL,          // 62 >
  Token::JS_QUESTION_MARK, // 63 ?
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
  Token::JS_LEFT_BRACKET,  // 91 [
  Token::ILLEGAL,          // 92 BACK_SLASH
  Token::JS_RIGHT_BRACKET, // 93 ]
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
  Token::JS_LEFT_BRACE,    // 123 {
  Token::ILLEGAL,          // 124 |
  Token::JS_RIGHT_BRACE,   // 125 }
  Token::ILLEGAL,          // 126 ~
  Token::ILLEGAL           // 127 DEL
};


// Borrowed from v8 javascript engine.
#define KEYWORDS(KEYWORD_GROUP, KEYWORD)                                \
  KEYWORD_GROUP('b')                                                    \
  KEYWORD("break", Token::JS_BREAK)                                     \
  KEYWORD_GROUP('c')                                                    \
  KEYWORD("case", Token::JS_CASE)                                       \
  KEYWORD("catch", Token::JS_CATCH)                                     \
  KEYWORD("class", Token::JS_CLASS)                                     \
  KEYWORD("const", Token::JS_CONST)                                     \
  KEYWORD("continue", Token::JS_CONTINUE)                               \
  KEYWORD_GROUP('d')                                                    \
  KEYWORD("debugger", Token::JS_DEBUGGER)                               \
  KEYWORD("default", Token::JS_DEFAULT)                                 \
  KEYWORD("delete", Token::JS_DELETE)                                   \
  KEYWORD("do", Token::JS_DO)                                           \
  KEYWORD_GROUP('e')                                                    \
  KEYWORD("else", Token::JS_ELSE)                                       \
  KEYWORD("enum", Token::JS_ENUM)                                       \
  KEYWORD("export", Token::JS_EXPORT)                                   \
  KEYWORD("extends", Token::JS_EXTENDS)                                 \
  KEYWORD_GROUP('f')                                                    \
  KEYWORD("false", Token::JS_FALSE)                                     \
  KEYWORD("finally", Token::JS_FINALLY)                                 \
  KEYWORD("for", Token::JS_FOR)                                         \
  KEYWORD("function", Token::JS_FUNCTION)                               \
  KEYWORD_GROUP('i')                                                    \
  KEYWORD("if", Token::JS_IF)                                           \
  KEYWORD("implements", Token::JS_IMPLEMENTS)                           \
  KEYWORD("import", Token::JS_IMPORT)                                   \
  KEYWORD("in", Token::JS_IN)                                           \
  KEYWORD("instanceof", Token::JS_INSTANCEOF)                           \
  KEYWORD("interface", Token::JS_INTERFACE)                             \
  KEYWORD_GROUP('l')                                                    \
  KEYWORD("let", es_harmony? Token::JS_LET: Token::FUTURE_STRICT_RESERVED_WORD) \
  KEYWORD_GROUP('m')                                                    \
  KEYWORD("module", Token::JS_MODULE)                                   \
  KEYWORD_GROUP('n')                                                    \
  KEYWORD("new", Token::JS_NEW)                                         \
  KEYWORD("null", Token::JS_NULL)                                       \
  KEYWORD_GROUP('N')                                                    \
  KEYWORD("NaN", Token::JS_NAN)                                         \
  KEYWORD_GROUP('p')                                                    \
  KEYWORD("package", Token::FUTURE_STRICT_RESERVED_WORD)                \
  KEYWORD("private", Token::JS_PRIVATE)                                 \
  KEYWORD("protected", Token::FUTURE_STRICT_RESERVED_WORD)              \
  KEYWORD("public", Token::JS_PUBLIC)                                   \
  KEYWORD_GROUP('r')                                                    \
  KEYWORD("return", Token::JS_RETURN)                                   \
  KEYWORD_GROUP('s')                                                    \
  KEYWORD("static", Token::JS_STATIC)                                   \
  KEYWORD("super", Token::JS_SUPER)                                     \
  KEYWORD("switch", Token::JS_SWITCH)                                   \
  KEYWORD_GROUP('t')                                                    \
  KEYWORD("this", Token::JS_THIS)                                       \
  KEYWORD("throw", Token::JS_THROW)                                     \
  KEYWORD("true", Token::JS_TRUE)                                       \
  KEYWORD("try", Token::JS_TRY)                                         \
  KEYWORD("typeof", Token::JS_TYPEOF)                                   \
  KEYWORD_GROUP('u')                                                    \
  KEYWORD("undefined", Token::JS_UNDEFINED)                             \
  KEYWORD_GROUP('v')                                                    \
  KEYWORD("var", Token::JS_VAR)                                         \
  KEYWORD("void", Token::JS_VOID)                                       \
  KEYWORD_GROUP('w')                                                    \
  KEYWORD("while", Token::JS_WHILE)                                     \
  KEYWORD("with", Token::JS_WITH)                                       \
  KEYWORD_GROUP('y')                                                    \
  KEYWORD("yield", Token::JS_YIELD)


// Only enabled in unit tests.
#ifdef UNIT_TEST

// Get token type string expression.
const char* TokenInfo::ToString() const {
  static const char* kTokenStringList[] = {
    "JS_BREAK",
    "JS_CASE",
    "JS_CATCH",
    "JS_CLASS",
    "JS_CONST",
    "JS_CONTINUE",
    "JS_DEBUGGER",
    "JS_DEFAULT",
    "JS_DELETE",
    "JS_DO",
    "JS_ELSE",
    "JS_ENUM",
    "JS_EXPORT",
    "JS_EXTENDS",
    "JS_FALSE",
    "JS_FINALLY",
    "JS_FOR",
    "JS_FUNCTION",
    "JS_IF",
    "JS_IMPLEMENTS",
    "JS_IMPORT",
    "JS_IN",
    "JS_INSTANCEOF",
    "JS_INTERFACE",
    "JS_LET",
    "JS_MODULE",
    "JS_NAN",
    "JS_NEW",
    "JS_NULL",
    "JS_PACKAGE",
    "JS_PRIVATE",
    "JS_PROTECTED",
    "JS_PUBLIC",
    "JS_RETURN",
    "JS_STATIC",
    "JS_SUPER",
    "JS_SWITCH",
    "JS_THIS",
    "JS_THROW",
    "JS_TRUE",
    "JS_TRY",
    "JS_TYPEOF",
    "JS_UNDEFINED",
    "JS_VAR",
    "JS_VOID",
    "JS_WHILE",
    "JS_WITH",
    "JS_YIELD",
    "JS_INCREMENT",
    "JS_DECREMENT",
    "JS_EQUAL",
    "JS_SHIFT_LEFT",
    "JS_SHIFT_RIGHT",
    "JS_LESS_EQUAL",
    "JS_GREATER_EQUAL",
    "JS_EQ",
    "JS_NOT_EQUAL",
    "JS_NOT_EQ",
    "JS_U_SHIFT_RIGHT",
    "JS_PLUS",
    "JS_MINUS",
    "JS_MUL",
    "JS_DIV",
    "JS_MOD",
    "JS_GREATER",
    "JS_LESS",
    "JS_BIT_OR",
    "JS_BIT_AND",
    "JS_BIT_NOR",
    "JS_BIT_XOR",
    "JS_ASSIGN",
    "JS_NOT",
    "JS_ADD_LET",
    "JS_SUB_LET",
    "JS_DIV_LET",
    "JS_MOD_LET",
    "JS_MUL_LET",
    "JS_LOGICAL_AND",
    "JS_LOGICAL_OR",
    "JS_SHIFT_LEFT_LET",
    "JS_SHIFT_RIGHT_LET",
    "JS_U_SHIFT_RIGHT_LET",
    "JS_NOR_LET",
    "JS_AND_LET",
    "JS_OR_LET",
    "JS_XOR_LET",
    "JS_FUNCTION_GLYPH",
    "JS_IDENTIFIER",
    "JS_NUMERIC_LITERAL",
    "JS_OCTAL_LITERAL",
    "JS_BINARY_LITERAL",
    "JS_STRING_LITERAL",
    "JS_REGEXP_LITERAL",
    "JS_LINE_BREAK",
    "JS_SET",
    "JS_GET",
    "JS_REST_PARAMETER",
    "JS_LEFT_PAREN",
    "JS_RIGHT_PAREN",
    "JS_COLON",
    "JS_QUESTION_MARK",
    "JS_LEFT_BRACKET",
    "JS_RIGHT_BRACKET",
    "JS_LEFT_BRACE",
    "JS_RIGHT_BRACE",
    "JS_ARROW_GLYPH",
    "LINE_TERMINATOR",
    "FUTURE_STRICT_RESERVED_WORD",
    "FUTURE_RESERVED_WORD",
    "END_OF_INPUT",
    "ILLEGAL"
  };
  return kTokenStringList[static_cast<uint16_t>(type_)];
}
#endif


// Get Identifier type from string.
Token TokenInfo::GetIdentifierType(const char* maybe_keyword, bool es_harmony) {
  const int input_length = Strlen(maybe_keyword);
  const int min_length = 2;
  const int max_length = 10;
  if (input_length < min_length || input_length > max_length) {
    return Token::JS_IDENTIFIER;
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
  return Token::JS_IDENTIFIER;
}


// Get puncture type from char.
Token TokenInfo::GetPunctureType(const UChar& uchar) {
  return kPunctures[uchar.ToUC8Ascii()];
}
}
