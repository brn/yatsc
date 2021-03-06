// The MIT License (MIT)
// 
// Copyright (c) 2013 Taketoshi Aono(brn)
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.


#ifndef PARSER_SCANNER_H_
#define PARSER_SCANNER_H_

#include <sstream>
#include "character.h"
#include "token.h"
#include "utfstring.h"
#include "lineterminator-state.h"
#include "literalbuffer.h"
#include "../utils/stl.h"
#include "../compiler-option.h"
#include "../compiler/module-info.h"


#if defined(DEBUG) || defined(UNIT_TEST)
#define TOKEN_ERROR(message)                                            \
  (void)[&]{StringStream ss;ss << message << '\n' << __FILE__ << ":" << __LINE__ << "\n";Error(ss.str().c_str());}()
#else
#define TOKEN_ERROR(message)                                            \
  (void)[&]{StringStream ss;ss << message << '\n';Error(ss.str().c_str());}()
#endif

#define ILLEGAL_TOKEN() TOKEN_ERROR("Illegal Token");


namespace yatsc {

template <typename UCharInputIterator>
class Scanner: public ErrorReporter, private Uncopyable, private Unmovable {
 public:
  /**
   * @param source The source file content.
   */
  Scanner(UCharInputIterator it,
          UCharInputIterator end,
          LiteralBuffer* literal_buffer,
          const CompilerOption& compilation_option);
  

  /**
   * Scan the source file from the current position to the next token position.
   */
  Token* Scan();


  Token* Peek();


  YATSC_CONST_GETTER(size_t, line_number, scanner_source_position_.current_line_number())


  // Check whether current token is regular expression or not.
  // If current token is regular expression return TS_REGULAR_EXPR,
  // if not regular expr return nullptr.
  Token* CheckRegularExpression(Token* token_info);


  void EnableNestedGenericTypeScanMode() {
    generic_type_++;
  }

  
  void DisableNestedGenericTypeScanMode() {
    generic_type_--;
  }

  
  bool IsGenericMode() {return generic_type_ > 0;}

  
  int nested_generic_count() {return generic_type_;}

  
  template <typename T>
  void SetErrorCallback(T callback) {error_callback_ = callback;}

  
  template <typename T>
  void SetReferencePathCallback(T callback) {reference_path_callback_ = callback;}
  
 private:

  class ScannerSourcePosition {
   public:
    ScannerSourcePosition()
        : start_position_(0),
          current_position_(0),
          current_line_number_(1),
          start_line_number_(1) {}


    YATSC_CONST_GETTER(size_t, start_position, start_position_)
    YATSC_CONST_GETTER(size_t, current_position, current_position_)
    YATSC_CONST_GETTER(size_t, current_line_number, current_line_number_)
    YATSC_CONST_GETTER(size_t, start_line_number, start_line_number_)

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

 public:
  
  class RecordedCharPosition {
   public:
    RecordedCharPosition(const ScannerSourcePosition& ssp, const UCharInputIterator& it,
                         const UChar& uchar, const UChar& lookahead)
        : ssp_(ssp),
          ucii_(it),
          uchar_(uchar),
          lookahead_(lookahead){}


    RecordedCharPosition(const RecordedCharPosition& rcp) = default;
    

    YATSC_CONST_GETTER(ScannerSourcePosition, ssp, ssp_)
    YATSC_CONST_GETTER(UCharInputIterator, ucii, ucii_)
    YATSC_CONST_GETTER(UChar, current, uchar_)
    YATSC_CONST_GETTER(UChar, lookahead, lookahead_)

   private:
    ScannerSourcePosition ssp_;
    UCharInputIterator ucii_;
    UChar uchar_;
    UChar lookahead_;
  };

  
  RecordedCharPosition char_position() YATSC_NO_SE {
    return RecordedCharPosition(scanner_source_position_, it_, char_, lookahead1_);
  }

  void RestoreScannerPosition(const RecordedCharPosition& rcp);

 private:

  void LineFeed() {
    scanner_source_position_.AdvanceLine();
  }

  
  void BeforeScan() {
    if (unscaned_) {
      unscaned_ = false;
      Advance();
      SkipSignature();
      SkipWhiteSpace();
    }
    error_ = false;
    line_terminator_state_.Clear();
    scanner_source_position_.UpdateStartPosition();
  }


  bool ErrorOccurred() const {return error_;}


  void AfterScan() {
    last_multi_line_comment_.Clear();
    SkipWhiteSpace();
    token_info_.set_line_terminator_state(line_terminator_state_);
  }


  bool DoScan();
  
  
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

  
  void ScanArithmeticOperator(TokenKind type1, TokenKind type2, TokenKind normal, bool has_let = true);


  void ScanOctalLiteral();


  void ScanBinaryLiteral();
  
  
  bool ScanUnicodeEscapeSequence(UtfString*, bool in_string_literal);


  void ScanRegularExpression();


  bool ConsumeLineBreak();


  UC16 ScanHexEscape(const UChar& uchar, int len, bool* success);
  

  void ScanLogicalOperator(TokenKind type1, TokenKind type2, TokenKind type3) {
    if (lookahead1_ == char_) {
      Advance();
      return BuildToken(type1);
    }
    if (lookahead1_ == unicode::u32('=')) {
      Advance();
      return BuildToken(type2);
    }
    BuildToken(type3);
  }
  

  void ScanBitwiseOrComparationOperator(
      TokenKind type1, TokenKind type2, TokenKind type3, TokenKind normal, TokenKind equal_comparator, bool has_u = false);

  
  void ScanEqualityComparatorOrArrowGlyph(bool not = false);


  bool ScanAsciiEscapeSequence(UtfString* str);


  template <typename T>
  int ToHexValue(const T& uchar) YATSC_NO_SE;
  

  YATSC_INLINE bool IsEnd() const {
    return it_ == end_;
  }

  
  bool SkipWhiteSpace();


  bool SkipWhiteSpaceOnly();


  bool SkipSingleLineComment();


  void SkipTripleSlashComment();


  bool SkipMultiLineComment();


  bool SkipSignature();
  

  void UpdateToken() {
    if (last_multi_line_comment_.utf8_length() > 0) {
      token_info_.set_multi_line_comment(Heap::NewHandle<UtfString>(last_multi_line_comment_));
    }

    last_multi_line_comment_.Clear();
    token_info_.ClearValue();
    token_info_.set_source_position(CreateSourcePosition());
  }
  

  void Error(const char* message) {
    error_ = true;
    if (error_callback_) {
      error_callback_(message, CreateSourcePosition());
    }
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
  

  void BuildToken(TokenKind type, const UtfString& utf_string) {
    UpdateToken();
    auto literal = literal_buffer_->InsertValue(utf_string);
    token_info_.set_value(literal);
    token_info_.set_type(type);
  }


  void BuildToken(TokenKind type) {
    UpdateToken();
    token_info_.set_type(type);
  }
  

  void Advance();


  void Skip() {
    while (!Character::IsWhiteSpace(char_, lookahead1_) &&
           Character::GetLineBreakType(char_, lookahead1_) == Character::LineBreakType::NONE &&
           char_ != unicode::u32('\0')) {
      Advance();
    }
  }

  void Skip(UtfString& utf_string) {
    while (!Character::IsWhiteSpace(char_, lookahead1_) &&
           char_ != unicode::u32('\0')) {
      Advance();
      utf_string += char_;
    }
  }


  bool unscaned_;
  bool error_;
  int generic_type_;
  ScannerSourcePosition scanner_source_position_;
  LineTerminatorState line_terminator_state_;
  UCharInputIterator it_;
  UCharInputIterator end_;
  Token token_info_;
  Token peeked_info_;
  UChar prev_;
  UChar char_;
  UChar lookahead1_;
  UtfString last_multi_line_comment_;
  LiteralBuffer* literal_buffer_;
  const CompilerOption& compiler_option_;
  std::function<void(const Literal*)> reference_path_callback_;
  std::function<void(const char* message, const SourcePosition& source_position)> error_callback_;
};
}


#include "scanner-inl.h"

#undef TOKEN_ERROR
#undef ILLEGAL_TOKEN
#endif
