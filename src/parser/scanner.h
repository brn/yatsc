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

#ifndef PARSER_SCANNER_H_
#define PARSER_SCANNER_H_

#include <sstream>
#include "character.h"
#include "token.h"
#include "utfstring.h"
#include "../compiler-option.h"


namespace yatsc {
class TokenException: std::exception {
 public:
  TokenException(const char* message)
      : std::exception(),
        message_(message) {}

    
  const char* what() const throw() {
    return message_.c_str();
  }
    
 private:
  std::string message_;
    
};


template <typename UCharInputIterator>
class Scanner {
 public:
  /**
   * @param source The source file content.
   */
  Scanner(UCharInputIterator it,
          UCharInputIterator end,
          const CompilerOption& compilation_option);

  /**
   * Scan the source file from the current position to the next token position.
   */
  const TokenInfo* Scan();


  /**
   * Lookahead a token.
   */
  const TokenInfo* Peek();


  YATSC_INLINE bool has_line_terminator_before_next() YATSC_NO_SE {
    return has_line_terminator_before_next_;
  }
  
  
  YATSC_INLINE const char* message() const {
    return message_.c_str();
  }

  
  YATSC_INLINE const UtfString& last_multi_line_comment() YATSC_NO_SE {
    return last_multi_line_comment_;
  }


  YATSC_INLINE size_t current_position() YATSC_NO_SE {
    return current_position_;
  }

  
  YATSC_INLINE size_t line_number() YATSC_NO_SE {
    return line_number_;
  }


  YATSC_INLINE std::string GetLineSource(size_t line_start_col, size_t start_col, size_t end_col) {
    UCharInputIterator it = line_head_;
    std::stringstream st;
    for (size_t i = 0; i < end_col; i++, ++it) {
      if (i >= line_start_col) {
        st << it->ToAscii();
      }
    }
    st << "\n";
    for (size_t i = 0; i < end_col - line_start_col; i++) {
      if (i >= start_col) {
        st << '^';
      } else {
        st << '-';
      }
    }
    return std::move(st.str());
  }
  
  
 private:

  const TokenInfo* DoScan();

  void LineFeed() {
    line_number_++;
    current_position_ = 1;
    line_head_ = it_;
  }
  
  
  /**
   * Scan string literal.
   */
  void ScanStringLiteral();

  /**
   * Scan digit literal(includes Hex, Double, Integer)
   */
  void ScanDigit();

  /**
   * Scan identifier.
   */
  void ScanIdentifier();

  
  void ScanInteger();

  
  void ScanHex();

  
  void ScanOperator();

  
  void ScanArithmeticOperator(Token type1, Token type2, Token normal, bool has_let = true);


  void ScanOctalLiteral();


  void ScanBinaryLiteral();
  
  
  bool ScanUnicodeEscapeSequence(UtfString*);


  bool ConsumeLineBreak();


  UC16 ScanHexEscape(const UChar& uchar, int len, bool* success);
  

  void ScanLogicalOperator(Token type1, Token type2, Token type3) {
    if (lookahead1_ == char_) {
      return BuildToken(type1);
    }
    if (lookahead1_ == unicode::u8('=')) {
      return BuildToken(type2);
    }
    BuildToken(type3);
  }
  

  void ScanBitwiseOrComparationOperator(
      Token type1, Token type2, Token type3, Token normal, Token equal_comparator, bool has_u = false);

  
  void ScanEqualityComparatorOrArrowGlyph(bool not = false);


  bool ScanAsciiEscapeSequence(UtfString* str);


  template <typename T>
  int ToHexValue(const T& uchar) const {
    int ret = 0;
    if (uchar >= unicode::u8('0') && uchar <= unicode::u8('9')) {
      ret = static_cast<int>(uchar - unicode::u8('0'));
    } else if (uchar >= unicode::u8('a') && uchar <= unicode::u8('f')) {
      ret = static_cast<int>(uchar - unicode::u8('a') + 10);
    } else if (uchar >= unicode::u8('A') && uchar <= unicode::u8('F')) {
      ret = static_cast<int>(uchar - unicode::u8('A') + 10);
    } else {
      return -1;
    }
    return ret;
  }
  

  YATSC_INLINE bool IsEnd() const {
    return it_ == end_;
  }

  
  bool SkipWhiteSpace() {
    bool skip = false;
    while(Character::IsWhiteSpace(char_, lookahead1_) || char_ == unicode::u8(';') ||
          Character::IsSingleLineCommentStart(char_, lookahead1_) ||
          Character::IsMultiLineCommentStart(char_, lookahead1_)) {
      if (Character::GetLineBreakType(char_, lookahead1_) != Character::LineBreakType::NONE ||
          char_ == unicode::u8(';')) {
        has_line_terminator_before_next_ = true;
      }
      skip = true;
      if (!ConsumeLineBreak() && !SkipSingleLineComment() && !SkipMultiLineComment()) {
        Advance();
      }
    }
    return skip;
  }


  bool SkipSingleLineComment() {
    bool skip = false;
    if (Character::IsSingleLineCommentStart(char_, lookahead1_)) {
      while (char_ != unicode::u8('\0') &&
             Character::GetLineBreakType(char_, lookahead1_) == Character::LineBreakType::NONE) {
        Advance();
      }
      skip = true;
    }
    return skip;
  }


  bool SkipMultiLineComment() {
    bool skip = false;
    if (Character::IsMultiLineCommentStart(char_, lookahead1_)) {
      UtfString str;
      while (!Character::IsMultiLineCommentEnd(char_, lookahead1_)) {
        Character::LineBreakType lt = Character::GetLineBreakType(char_, lookahead1_);
        if (lt != Character::LineBreakType::NONE) {
          LineFeed();
          has_line_terminator_before_next_ = true;
        }

        if (lt == Character::LineBreakType::CRLF) {
          str += char_;
          Advance();
          str += char_;
          Advance();          
        } else {
          str += char_;
          Advance();
        }
      }
      str += char_;
      Advance();
      str += char_;
      Advance();
      skip = true;
      last_multi_line_comment_ = std::move(str);
    }
    return skip;
  }
  

  void UpdateTokenInfo() {
    message_.clear();
    token_info_.ClearValue();
    token_info_.set_start_col(current_position());
    token_info_.set_line_number(line_number());
  }
  

  void Error(const char* message) {
    UpdateTokenInfo();
    token_info_.set_type(Token::ILLEGAL);
    TokenException te(message);
    throw te;
  }


  YATSC_INLINE void Illegal() {
    std::stringstream ss;
    ss << "Illegal token.\n" << GetLineSource(0, token_info_.start_col(), current_position_);
    std::string message = std::move(ss.str());
    return Error(message.c_str());
  }
  

  void BuildToken(Token type, UtfString utf_string) {
    UpdateTokenInfo();
    token_info_.set_value(std::move(utf_string));
    token_info_.set_type(type);
  }


  void BuildToken(Token type) {
    UpdateTokenInfo();
    token_info_.set_type(type);
  }
  

  void Advance();

  
  bool has_line_terminator_before_next_;
  size_t current_position_;
  size_t line_number_;
  UCharInputIterator line_head_;
  UCharInputIterator it_;
  UCharInputIterator end_;
  TokenInfo token_info_;
  UChar char_;
  UChar lookahead1_;
  UtfString last_multi_line_comment_;
  std::string message_;
  const CompilerOption& compiler_option_;
};
}


#include "scanner-inl.h"
#endif
