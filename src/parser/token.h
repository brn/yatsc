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

#ifndef PARSER_TOKEN_H_
#define PARSER_TOKEN_H_

#include <string>
#include "./utfstring.h"
#include "./uchar.h"
#include "../utils/utils.h"
#include "../utils/os.h"

#define FAST_VALUE_LENGTH 20

namespace yatsc {

#define TOKEN_LIST(DECLARE, DECLARE_FIRST, DECLARE_LAST)  \
  DECLARE_FIRST(TS_BREAK)                                 \
  DECLARE(TS_CASE)                                        \
  DECLARE(TS_CATCH)                                       \
  DECLARE(TS_CLASS)                                       \
  DECLARE(TS_CONST)                                       \
  DECLARE(TS_CONTINUE)                                    \
  DECLARE(TS_DEBUGGER)                                    \
  DECLARE(TS_DECLARE)                                     \
  DECLARE(TS_DEFAULT)                                     \
  DECLARE(TS_DELETE)                                      \
  DECLARE(TS_DO)                                          \
  DECLARE(TS_ELSE)                                        \
  DECLARE(TS_ENUM)                                        \
  DECLARE(TS_EXPORT)                                      \
  DECLARE(TS_EXTENDS)                                     \
  DECLARE(TS_FALSE)                                       \
  DECLARE(TS_FINALLY)                                     \
  DECLARE(TS_FOR)                                         \
  DECLARE(TS_FUNCTION)                                    \
  DECLARE(TS_IF)                                          \
  DECLARE(TS_IMPLEMENTS)                                  \
  DECLARE(TS_IMPORT)                                      \
  DECLARE(TS_IN)                                          \
  DECLARE(TS_INSTANCEOF)                                  \
  DECLARE(TS_INTERFACE)                                   \
  DECLARE(TS_LET)                                         \
  DECLARE(TS_MODULE)                                      \
  DECLARE(TS_NAN)                                         \
  DECLARE(TS_NEW)                                         \
  DECLARE(TS_NULL)                                        \
  DECLARE(TS_PACKAGE)                                     \
  DECLARE(TS_PRIVATE)                                     \
  DECLARE(TS_PROTECTED)                                   \
  DECLARE(TS_PUBLIC)                                      \
  DECLARE(TS_RETURN)                                      \
  DECLARE(TS_STATIC)                                      \
  DECLARE(TS_SUPER)                                       \
  DECLARE(TS_SWITCH)                                      \
  DECLARE(TS_THIS)                                        \
  DECLARE(TS_THROW)                                       \
  DECLARE(TS_TRUE)                                        \
  DECLARE(TS_TRY)                                         \
  DECLARE(TS_TYPEOF)                                      \
  DECLARE(TS_UNDEFINED)                                   \
  DECLARE(TS_VAR)                                         \
  DECLARE(TS_VOID)                                        \
  DECLARE(TS_WHILE)                                       \
  DECLARE(TS_WITH)                                        \
  DECLARE(TS_YIELD)                                       \
  DECLARE(TS_INCREMENT)                                   \
  DECLARE(TS_DECREMENT)                                   \
  DECLARE(TS_EQUAL)                                       \
  DECLARE(TS_SHIFT_LEFT)                                  \
  DECLARE(TS_SHIFT_RIGHT)                                 \
  DECLARE(TS_LESS_EQUAL)                                  \
  DECLARE(TS_GREATER_EQUAL)                               \
  DECLARE(TS_EQ)                                          \
  DECLARE(TS_NOT_EQUAL)                                   \
  DECLARE(TS_NOT_EQ)                                      \
  DECLARE(TS_U_SHIFT_RIGHT)                               \
  DECLARE(TS_PLUS)                                        \
  DECLARE(TS_MINUS)                                       \
  DECLARE(TS_MUL)                                         \
  DECLARE(TS_DIV)                                         \
  DECLARE(TS_MOD)                                         \
  DECLARE(TS_GREATER)                                     \
  DECLARE(TS_LESS)                                        \
  DECLARE(TS_BIT_OR)                                      \
  DECLARE(TS_BIT_AND)                                     \
  DECLARE(TS_BIT_NOR)                                     \
  DECLARE(TS_BIT_XOR)                                     \
  DECLARE(TS_ASSIGN)                                      \
  DECLARE(TS_NOT)                                         \
  DECLARE(TS_ADD_LET)                                     \
  DECLARE(TS_SUB_LET)                                     \
  DECLARE(TS_DIV_LET)                                     \
  DECLARE(TS_MOD_LET)                                     \
  DECLARE(TS_MUL_LET)                                     \
  DECLARE(TS_LOGICAL_AND)                                 \
  DECLARE(TS_LOGICAL_OR)                                  \
  DECLARE(TS_SHIFT_LEFT_LET)                              \
  DECLARE(TS_SHIFT_RIGHT_LET)                             \
  DECLARE(TS_U_SHIFT_RIGHT_LET)                           \
  DECLARE(TS_NOR_LET)                                     \
  DECLARE(TS_AND_LET)                                     \
  DECLARE(TS_OR_LET)                                      \
  DECLARE(TS_XOR_LET)                                     \
  DECLARE(TS_FUNCTION_GLYPH)                              \
  DECLARE(TS_IDENTIFIER)                                  \
  DECLARE(TS_NUMERIC_LITERAL)                             \
  DECLARE(TS_OCTAL_LITERAL)                               \
  DECLARE(TS_BINARY_LITERAL)                              \
  DECLARE(TS_STRING_LITERAL)                              \
  DECLARE(TS_REGEXP_LITERAL)                              \
  DECLARE(TS_LINE_BREAK)                                  \
  DECLARE(TS_SET)                                         \
  DECLARE(TS_GET)                                         \
  DECLARE(TS_REST_PARAMETER)                              \
  DECLARE(TS_LEFT_PAREN)                                  \
  DECLARE(TS_RIGHT_PAREN)                                 \
  DECLARE(TS_COLON)                                       \
  DECLARE(TS_QUESTION_MARK)                               \
  DECLARE(TS_LEFT_BRACKET)                                \
  DECLARE(TS_RIGHT_BRACKET)                               \
  DECLARE(TS_LEFT_BRACE)                                  \
  DECLARE(TS_RIGHT_BRACE)                                 \
  DECLARE(TS_ARROW_GLYPH)                                 \
  DECLARE(LINE_TERMINATOR)                                \
  DECLARE(FUTURE_STRICT_RESERVED_WORD)                    \
  DECLARE(FUTURE_RESERVED_WORD)                           \
  DECLARE(END_OF_INPUT)                                   \
  DECLARE_LAST(ILLEGAL)


enum class Token: uint16_t {
#define DECLARE_ENUM(TokenName) TokenName,
#define DECLARE_FIRST(TokenName) TokenName = 0,
#define DECLARE_LAST(TokenName) TokenName
  TOKEN_LIST(DECLARE_ENUM, DECLARE_FIRST, DECLARE_LAST)
#undef DECLARE_ENUM
#undef DECLARE_FIRST
#undef DECLARE_LAST
};


namespace tokenhelper {
#ifdef UNIT_TEST
static const char* kTokenStringList[] = {
#define DECLARE(TokenName) #TokenName,
#define DECLARE_LAST(TokenName) #TokenName
  TOKEN_LIST(DECLARE, DECLARE, DECLARE_LAST)
#undef DECLARE_ENUM
#undef DECLARE_FIRST
#undef DECLARE_LAST
};
}
#endif



class TokenInfo {
 public:

  TokenInfo() :
      type_(Token::END_OF_INPUT),
      start_col_(0),
      line_number_(1) {}
  

  TokenInfo(const TokenInfo& token_info)
      : vector_(token_info.vector_),
        type_(token_info.type_),
        start_col_(token_info.start_col_),
        line_number_(token_info.line_number_){}

  
  ~TokenInfo() = default;


  YATSC_INLINE void set_value(UtfString&& vector) {
    vector_ = std::move(vector);
  }
  
  
  YATSC_INLINE const UtfString& value() const {
    return vector_;
  }


  YATSC_INLINE void set_type(Token type) {
    type_ = type;
  }
  
  
  YATSC_INLINE Token type() const {
    return type_;
  }


  YATSC_INLINE void set_start_col(size_t start_col) {
    start_col_ = start_col;
  }
  
  
  YATSC_INLINE size_t start_col() const {
    return start_col_;
  }


  YATSC_INLINE void set_line_number(size_t line_number) {
    line_number_ = line_number;
  }
  
  
  YATSC_INLINE size_t line_number() const {
    return line_number_;
  }


#ifdef UNIT_TEST
  const char* ToString() const;
#endif
  

  static Token GetIdentifierType(const char* maybe_keyword, bool es_harmony = false);

  static Token GetPunctureType(const UChar& uchar);
  
 private:  
  UtfString vector_;
  Token type_;
  size_t start_col_;
  size_t line_number_;
};
}

#undef FAST_VALUE_LENGTH
#endif
