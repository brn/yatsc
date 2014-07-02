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
#include "error_reporter.h"
#include "lineterminator-state.h"
#include "../compiler-option.h"


namespace yatsc {
class TokenException: public std::exception {
 public:
  TokenException(const char* message)
      : std::exception(),
        message_(message) {}


  TokenException(const TokenException& token_exception)
      : std::exception(token_exception),
        message_(token_exception.message_) {}


  TokenException& operator = (const TokenException& token_exception) {
    std::exception::operator = (token_exception);
    message_ = token_exception.message_;
    return *this;
  }

    
  const char* what() const throw() {
    return message_.c_str();
  }
    
 private:
  std::string message_;
    
};


template <typename UCharInputIterator>
class Scanner: private Uncopyable, private Unmovable {
 public:
  /**
   * @param source The source file content.
   */
  Scanner(UCharInputIterator it,
          UCharInputIterator end,
          ErrorReporter* error_reporter,
          const CompilerOption& compilation_option);

  /**
   * Scan the source file from the current position to the next token position.
   */
  TokenInfo* Scan();


  /**
   * Peek next token.
   */
  TokenInfo* Peek() {
    return &next_token_info_;
  }


  YATSC_CONST_GETTER(size_t, line_number, scanner_source_position_.current_line_number());


  class RegularExpressionScope: private Unmovable {
   public:
    RegularExpressionScope(Scanner<UCharInputIterator>* scanner)
        : scanner_(scanner) {
      scanner_->allow_regular_expression_ = true;
    }

    RegularExpressionScope(const RegularExpressionScope& scope)
        : scanner_(scope.scanner_) {}


    RegularExpressionScope& operator = (const RegularExpressionScope& scope) {
      scanner_ = scope.scanner_;
    }

    
    ~RegularExpressionScope() {
      scanner_->allow_regular_expression_ = false;
    }


    YATSC_INLINE void Escape() YATSC_NOEXCEPT {scanner_->allow_regular_expression_ = false;}

   private:
    Scanner<UCharInputIterator>* scanner_;
  };
  
  
 private:

  class ScannerSourcePosition {
   public:
    ScannerSourcePosition()
        : start_position_(0),
          current_position_(0),
          current_line_number_(1),
          start_line_number_(1) {}


    YATSC_CONST_GETTER(size_t, start_position, start_position_);
    YATSC_CONST_GETTER(size_t, current_position, current_position_);
    YATSC_CONST_GETTER(size_t, current_line_number, current_line_number_);
    YATSC_CONST_GETTER(size_t, start_line_number, start_line_number_);

    YATSC_INLINE void AdvancePosition(size_t pos) YATSC_NOEXCEPT {current_position_ += pos;}

    YATSC_INLINE void AdvanceLine() YATSC_NOEXCEPT {
      current_line_number_++;
      current_position_ = 1;
    }

    YATSC_INLINE void UpdateStartPosition() YATSC_NOEXCEPT {
      start_position_ = current_position_;
      start_line_number_ = current_line_number_;
    }

   private:
    
    size_t start_position_;
    size_t current_position_;
    size_t current_line_number_;
    size_t start_line_number_;
  };


  void DoScan();

  void LineFeed() {
    scanner_source_position_.AdvanceLine();
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


  void ScanRegularExpression();


  bool ConsumeLineBreak();


  UC16 ScanHexEscape(const UChar& uchar, int len, bool* success);
  

  void ScanLogicalOperator(Token type1, Token type2, Token type3) {
    if (lookahead1_ == char_) {
      return BuildToken(type1);
    }
    if (lookahead1_ == unicode::u8('=')) {
      Advance();
      return BuildToken(type2);
    }
    BuildToken(type3);
  }
  

  void ScanBitwiseOrComparationOperator(
      Token type1, Token type2, Token type3, Token normal, Token equal_comparator, bool has_u = false);

  
  void ScanEqualityComparatorOrArrowGlyph(bool not = false);


  bool ScanAsciiEscapeSequence(UtfString* str);


  template <typename T>
  int ToHexValue(const T& uchar) YATSC_NO_SE;
  

  YATSC_INLINE bool IsEnd() const {
    return it_ == end_;
  }

  
  bool SkipWhiteSpace();


  bool SkipSingleLineComment();


  bool SkipMultiLineComment();
  

  void UpdateTokenInfo() {
    current_token_info_->set_multi_line_comment(last_multi_line_comment_);
    current_token_info_->ClearValue();
    current_token_info_->set_source_position(CreateSourcePosition());
  }
  

  void Error(const char* message) {
    error_reporter() << message;
    error_reporter_->Throw<TokenException>(CreateSourcePosition());
  }


  YATSC_INLINE void Illegal() {
    Error("Illegal token.");
  }


  YATSC_INLINE SourcePosition CreateSourcePosition() {
    return SourcePosition(scanner_source_position_.start_position(),
                          scanner_source_position_.current_position(),
                          scanner_source_position_.start_line_number(),
                          scanner_source_position_.current_line_number());
  }
  

  void BuildToken(Token type, UtfString utf_string) {
    UpdateTokenInfo();
    current_token_info_->set_value(std::move(utf_string));
    current_token_info_->set_type(type);
  }


  void BuildToken(Token type) {
    UpdateTokenInfo();
    current_token_info_->set_type(type);
  }


  YATSC_INLINE ErrorReporter& error_reporter() {
    return *error_reporter_;
  }
  

  void Advance();


  bool unscaned_;
  bool allow_regular_expression_;
  ScannerSourcePosition scanner_source_position_;
  LineTerminatorState line_terminator_state_;
  UCharInputIterator it_;
  UCharInputIterator end_;
  TokenInfo token_info_;
  TokenInfo next_token_info_;
  TokenInfo* current_token_info_;
  UChar char_;
  UChar lookahead1_;
  UtfString last_multi_line_comment_;
  ErrorReporter* error_reporter_;
  const CompilerOption& compiler_option_;
};
}


#include "scanner-inl.h"
#endif
