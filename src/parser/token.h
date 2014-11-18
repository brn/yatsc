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
#include "./literalbuffer.h"
#include "./utfstring.h"
#include "./lineterminator-state.h"
#include "./uchar.h"
#include "../utils/utils.h"
#include "../utils/os.h"
#include "../compiler-option.h"


namespace yatsc {

#define TOKEN_LIST(DECLARE, DECLARE_FIRST, DECLARE_LAST)  \
  DECLARE_FIRST(Backquote)                                \
  DECLARE(Break)                                          \
  DECLARE(Case)                                           \
  DECLARE(Catch)                                          \
  DECLARE(Class)                                          \
  DECLARE(Comma)                                          \
  DECLARE(Const)                                          \
  DECLARE(Continue)                                       \
  DECLARE(Debugger)                                       \
  DECLARE(Default)                                        \
  DECLARE(Delete)                                         \
  DECLARE(Do)                                             \
  DECLARE(Dot)                                            \
  DECLARE(Else)                                           \
  DECLARE(Enum)                                           \
  DECLARE(Export)                                         \
  DECLARE(Extends)                                        \
  DECLARE(False)                                          \
  DECLARE(Finally)                                        \
  DECLARE(For)                                            \
  DECLARE(Function)                                       \
  DECLARE(If)                                             \
  DECLARE(Implements)                                     \
  DECLARE(Import)                                         \
  DECLARE(In)                                             \
  DECLARE(Instanceof)                                     \
  DECLARE(Interface)                                      \
  DECLARE(Let)                                            \
  DECLARE(Module)                                         \
  DECLARE(Nan)                                            \
  DECLARE(New)                                            \
  DECLARE(Null)                                           \
  DECLARE(Package)                                        \
  DECLARE(Private)                                        \
  DECLARE(Protected)                                      \
  DECLARE(Public)                                         \
  DECLARE(RegularExpr)                                    \
  DECLARE(Rest)                                           \
  DECLARE(Return)                                         \
  DECLARE(Static)                                         \
  DECLARE(Super)                                          \
  DECLARE(Switch)                                         \
  DECLARE(This)                                           \
  DECLARE(Throw)                                          \
  DECLARE(True)                                           \
  DECLARE(Try)                                            \
  DECLARE(Typeof)                                         \
  DECLARE(Undefined)                                      \
  DECLARE(Var)                                            \
  DECLARE(Void)                                           \
  DECLARE(While)                                          \
  DECLARE(With)                                           \
  DECLARE(Yield)                                          \
  DECLARE(Increment)                                      \
  DECLARE(Decrement)                                      \
  DECLARE(Equal)                                          \
  DECLARE(ShiftLeft)                                      \
  DECLARE(ShiftRight)                                     \
  DECLARE(LessEqual)                                      \
  DECLARE(GreaterEqual)                                   \
  DECLARE(Eq)                                             \
  DECLARE(NotEqual)                                       \
  DECLARE(NotEq)                                          \
  DECLARE(UShiftRight)                                    \
  DECLARE(Plus)                                           \
  DECLARE(Minus)                                          \
  DECLARE(Mul)                                            \
  DECLARE(Div)                                            \
  DECLARE(Mod)                                            \
  DECLARE(Greater)                                        \
  DECLARE(Less)                                           \
  DECLARE(BitOr)                                          \
  DECLARE(BitAnd)                                         \
  DECLARE(BitNor)                                         \
  DECLARE(BitXor)                                         \
  DECLARE(Assign)                                         \
  DECLARE(Not)                                            \
  DECLARE(AddLet)                                         \
  DECLARE(SubLet)                                         \
  DECLARE(DivLet)                                         \
  DECLARE(ModLet)                                         \
  DECLARE(MulLet)                                         \
  DECLARE(LogicalAnd)                                     \
  DECLARE(LogicalOr)                                      \
  DECLARE(ShiftLeftLet)                                   \
  DECLARE(ShiftRightLet)                                  \
  DECLARE(UShiftRightLet)                                 \
  DECLARE(NorLet)                                         \
  DECLARE(AndLet)                                         \
  DECLARE(OrLet)                                          \
  DECLARE(XorLet)                                         \
  DECLARE(Identifier)                                     \
  DECLARE(NumericLiteral)                                 \
  DECLARE(OctalLiteral)                                   \
  DECLARE(BinaryLiteral)                                  \
  DECLARE(StringLiteral)                                  \
  DECLARE(RegexpLiteral)                                  \
  DECLARE(LineBreak)                                      \
  DECLARE(Set)                                            \
  DECLARE(Get)                                            \
  DECLARE(RestParameter)                                  \
  DECLARE(LeftParen)                                      \
  DECLARE(RightParen)                                     \
  DECLARE(Colon)                                          \
  DECLARE(QuestionMark)                                   \
  DECLARE(LeftBracket)                                    \
  DECLARE(RightBracket)                                   \
  DECLARE(LeftBrace)                                      \
  DECLARE(RightBrace)                                     \
  DECLARE(ArrowGlyph)                                     \
  DECLARE(TemplateLiteral)                                \
  DECLARE(LineTerminator)                                 \
  DECLARE(FutureStrictReservedWord)                       \
  DECLARE(FutureReservedWord)                             \
  DECLARE(Eof)                                            \
  DECLARE_LAST(Illegal)


enum class TokenKind: uint16_t {
#define DECLARE_ENUM(TokenName) k##TokenName,
#define DECLARE_FIRST(TokenName) k##TokenName = 0,
#define DECLARE_LAST(TokenName) k##TokenName
  TOKEN_LIST(DECLARE_ENUM, DECLARE_FIRST, DECLARE_LAST)
#undef DECLARE_ENUM
#undef DECLARE_FIRST
#undef DECLARE_LAST
};


namespace tokenhelper {
#if defined(UNIT_TEST) || defined(DEBUG)
static const char* kTokenStringList[] = {
#define DECLARE(TokenName) #TokenName,
#define DECLARE_LAST(TokenName) #TokenName
  TOKEN_LIST(DECLARE, DECLARE, DECLARE_LAST)
#undef DECLARE
#undef DECLARE_LAST
};
}
#endif



class Token {
 public:

  // Default constructor.
  Token() :
      literal_(nullptr),
      type_(TokenKind::kEof) {}
  

  // Copy constructor
  Token(const Token& token_info)
      : literal_(token_info.literal_),
        multi_line_comment_(token_info.multi_line_comment_),
        type_(token_info.type_),
        line_terminator_state_(token_info.line_terminator_state_),
        source_position_(token_info.source_position_) {}


  Token(Token&& token_info)
      : literal_(std::move(token_info.literal_)),
        multi_line_comment_(std::move(token_info.multi_line_comment_)),
        type_(token_info.type_),
        line_terminator_state_(token_info.line_terminator_state_),
        source_position_(token_info.source_position_) {}


  Token& operator = (const Token& token_info) {
    literal_ = token_info.literal_;
    multi_line_comment_ = token_info.multi_line_comment_;
    type_ = token_info.type_;
    line_terminator_state_ = token_info.line_terminator_state_;
    source_position_ = token_info.source_position_;
    return *this;
  }


  Token& operator = (Token&& token_info) {
    literal_ = std::move(token_info.literal_);
    multi_line_comment_ = std::move(token_info.multi_line_comment_);
    type_ = token_info.type_;
    line_terminator_state_ = token_info.line_terminator_state_;
    source_position_ = token_info.source_position_;
    return *this;
  }
      
  
  
  ~Token() = default;
  

  /**
   * Set token value.
   * @param utf_string The token value that is unicode encoded string.
   */
  YATSC_INLINE void set_value(const Literal* literal) {
    literal_ = literal;
  }


  /**
   * Remove token value.
   */
  YATSC_INLINE void ClearValue() {
    literal_ = nullptr;
  }


  YATSC_INLINE void set_multi_line_comment(Handle<UtfString> multi_line_comment) {
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
  YATSC_INLINE const Literal* value() YATSC_NO_SE {
    return literal_;
  }

  /**
   * Return token value.
   * @returns The token value that is unicode encoded string.
   */
  YATSC_INLINE const char* utf8_value() YATSC_NO_SE {
    return literal_->utf8_value();
  }


  /**
   * Return token value.
   * @returns The token value that is unicode encoded string.
   */
  YATSC_INLINE const UC16* utf16_value() YATSC_NO_SE {
    return literal_->utf16_value();
  }


  YATSC_INLINE const UtfString* comment() YATSC_NO_SE {
    return multi_line_comment_.Get();
  }


  YATSC_INLINE bool Is(const TokenKind kind) YATSC_NO_SE {return kind == type_;}


  // Getter and setter for type_.
  YATSC_CONST_PROPERTY(TokenKind, type, type_);


  // Setter for source_position_.
  YATSC_SETTER(SourcePosition, source_position, source_position_);
  
  
  // Getter for source_position_.
  YATSC_CONST_GETTER(const SourcePosition&, source_position, source_position_);


  YATSC_SETTER(LineTerminatorState, line_terminator_state, line_terminator_state_);
  

  YATSC_CONST_GETTER(bool, has_line_terminator_before_next, line_terminator_state_.has_line_terminator_before_next());


  YATSC_CONST_GETTER(bool, has_line_break_before_next, line_terminator_state_.has_line_break_before_next());

#ifdef UNIT_TEST
  String ToStringWithValue() const {
    StringStream ss;
    ss << tokenhelper::kTokenStringList[static_cast<uint16_t>(type_)];
    if (literal_ != nullptr && literal_->utf8_length() > 0) {
      ss << "[" << literal_->utf8_value() << "]";
    }
    return std::move(ss.str());
  }


  String ToString() const {
    return String(tokenhelper::kTokenStringList[static_cast<uint16_t>(type_)]);
  }

  static const char* ToString(TokenKind token_kind) {
    return tokenhelper::kTokenStringList[static_cast<uint16_t>(token_kind)];
  }
#endif
  
  /**
   * Get a type of the identifier like Var
   * @param maybe_keyword An identifier value.
   * @param es_harmony Harmony option.
   */
  static TokenKind GetIdentifierType(const char* maybe_keyword, const CompilerOption& co);


  /**
   * Get a type of the puncture like LeftBrace.
   * @param uchar The unicode char.
   */
  static TokenKind GetPunctureType(const UChar& uchar);

  
  static Token kNullToken;


  static uint8_t GetOperandPriority(Token t);

  
  static bool IsKeyword(TokenKind type);
  
 private:  
  const Literal* literal_;
  Handle<UtfString> multi_line_comment_;
  TokenKind type_;
  LineTerminatorState line_terminator_state_;
  SourcePosition source_position_;
};
}


#undef TOKEN_LIST
#endif
