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
#include "./sourceposition.h"
#include "./utfstring.h"
#include "./lineterminator-state.h"
#include "./uchar.h"
#include "../utils/utils.h"
#include "../utils/os.h"


namespace yatsc {

#define TOKEN_LIST(DECLARE, DECLARE_FIRST, DECLARE_LAST)  \
  DECLARE_FIRST(TS_BREAK)                                 \
  DECLARE(TS_CASE)                                        \
  DECLARE(TS_CATCH)                                       \
  DECLARE(TS_CLASS)                                       \
  DECLARE(TS_COMMA)                                       \
  DECLARE(TS_CONST)                                       \
  DECLARE(TS_CONTINUE)                                    \
  DECLARE(TS_DEBUGGER)                                    \
  DECLARE(TS_DECLARE)                                     \
  DECLARE(TS_DEFAULT)                                     \
  DECLARE(TS_DELETE)                                      \
  DECLARE(TS_DO)                                          \
  DECLARE(TS_DOT)                                         \
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
  DECLARE(TS_REGULAR_EXPR)                                \
  DECLARE(TS_REST)                                        \
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
  DECLARE(NULL_TOKEN)                                     \
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
#undef DECLARE
#undef DECLARE_LAST
};
}
#endif



class TokenInfo {
 public:

  // Default constructor.
  TokenInfo() :
      type_(Token::NULL_TOKEN) {}
  

  // Copy constructor
  TokenInfo(const TokenInfo& token_info)
      : utf_string_(token_info.utf_string_),
        multi_line_comment_(token_info.multi_line_comment_),
        line_terminator_state_(token_info.line_terminator_state_),
        type_(token_info.type_),
        source_position_(token_info.source_position_) {}

  
  ~TokenInfo() = default;
  

  /**
   * Set token value.
   * @param utf_string The token value that is unicode encoded string.
   */
  YATSC_INLINE void set_value(UtfString&& utf_string) {
    utf_string_ = std::move(utf_string);
  }


  /**
   * Remove token value.
   */
  YATSC_INLINE void ClearValue() {
    utf_string_.Clear();
  }


  YATSC_INLINE void set_multi_line_comment(UtfString multi_line_comment) {
    multi_line_comment_ = multi_line_comment;
  }

  
  /**
   * Remove token value.
   */
  YATSC_INLINE void ClearComment() {
    multi_line_comment_.Clear();
  }
  

  /**
   * Return token value.
   * @returns The token value that is unicode encoded string.
   */
  YATSC_INLINE const UtfString& value() YATSC_NO_SE {
    return utf_string_;
  }


  YATSC_INLINE const UtfString& comment() YATSC_NO_SE {
    return multi_line_comment_;
  }


  // Getter and setter for type_.
  YATSC_CONST_PROPERTY(Token, type, type_);


  // Setter for source_position_.
  YATSC_SETTER(SourcePosition, source_position, source_position_);
  
  
  // Getter for source_position_.
  YATSC_CONST_GETTER(const SourcePosition&, source_position, source_position_);


  YATSC_SETTER(LineTerminatorState, line_terminator_state, line_terminator_state_);
  

  YATSC_CONST_GETTER(bool, has_line_terminator_before_next, line_terminator_state_.has_line_terminator_before_next());


  YATSC_CONST_GETTER(bool, has_line_break_before_next, line_terminator_state_.has_line_break_before_next());

#ifdef UNIT_TEST
  const char* ToString() const;
#endif
  
  /**
   * Get a type of the identifier like TS_VAR
   * @param maybe_keyword An identifier value.
   * @param es_harmony Harmony option.
   */
  static Token GetIdentifierType(const char* maybe_keyword, bool es_harmony = false);


  /**
   * Get a type of the puncture like TS_LEFT_BRACE.
   * @param uchar The unicode char.
   */
  static Token GetPunctureType(const UChar& uchar);

  
  static TokenInfo kNullToken;
  
 private:  
  UtfString utf_string_;
  UtfString multi_line_comment_;
  Token type_;
  LineTerminatorState line_terminator_state_;
  SourcePosition source_position_;
};
}


#undef TOKEN_LIST
#endif
