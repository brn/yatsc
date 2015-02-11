/*
 * The MIT License (MIT)
 * 
 * Copyright (c) 2013 Taketoshi Aono(brn)
 * 
 * Permission is hereby granted, free of Charge, to any person obtaining a copy
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

#ifndef PARSER_SCANNER_INL_H_
#define PARSER_SCANNER_INL_H_

#include <cstdio>
#include <sstream>
#include "token.h"
#include "../utils/utils.h"


namespace yatsc {

template<typename UCharInputIterator>
Scanner<UCharInputIterator>::Scanner(
    UCharInputIterator it,
    UCharInputIterator end,
    LiteralBuffer* literal_buffer,
    const CompilerOption& compiler_option)
    : unscaned_(true),
      generic_type_(0),
      it_(it),
      end_(end),
      literal_buffer_(literal_buffer),
      compiler_option_(compiler_option) {}


template<typename UCharInputIterator>
void Scanner<UCharInputIterator>::Advance()  {
  if (it_ == end_) {
    char_ = UChar::Null();
    return;
  }
  
  char_ = *it_;
  scanner_source_position_.AdvancePosition(char_.utf8_length());
  ++it_;
  if (it_ == end_) {
    lookahead1_ = UChar::Null();
    return;
  }
  
  lookahead1_ = *it_;
}


template<typename UCharInputIterator>
Token* Scanner<UCharInputIterator>::Scan() {
  BeforeScan();
  
  if (!char_.IsAscii()) {
    ILLEGAL_TOKEN();
    UtfString str;
    while (!char_.IsAscii()) {
      str += char_;
      Advance();
    }
    BuildToken(TokenKind::kIdentifier, str);
  } else {  
    while (!DoScan() &&
           char_ != unicode::u32('\0')) {
      Advance();
      AfterScan();
    }
  }

  AfterScan();
  return &token_info_;
}


template <typename UCharInputIterator>
bool Scanner<UCharInputIterator>::DoScan() {
  if (char_ == unicode::u32('\0')) {
    BuildToken(TokenKind::kEof);
    return true;
  } else if (char_ == unicode::u32(';')) {
    BuildToken(TokenKind::kLineTerminator);
    Advance();
    return true;
  } else if (Character::IsPuncture(char_)) {
    BuildToken(Token::GetPunctureType(char_));
    Advance();
    return true;
  } else if (Character::IsIdentifierStart(char_) ||
             Character::IsUnicodeEscapeSequenceStart(char_, lookahead1_)) {
    ScanIdentifier();
    return true;
  } else if (Character::IsStringLiteralStart(char_)) {
    ScanStringLiteral();
    Advance();
    return true;
  } else if (Character::IsDigitStart(char_, lookahead1_)) {
    ScanDigit();
    return true;
  } else if (Character::IsOperatorStart(char_)) {
    ScanOperator();
    Advance();
    return true;
  }
  return false;
}


template<typename UCharInputIterator>
UC16 Scanner<UCharInputIterator>::ScanHexEscape(const UChar& uchar, int len, bool* success) {
  int result = 0;
  for (int i = 0; i < len; ++i) {
    if (i != 0) {
      Advance();
    }
    const int d = ToHexValue(uchar);
    if (d < 0) {
      *success = false;
      return 0;
    }
    *success = true;
    result = result * 16 + d;
  }
  return static_cast<UC16>(result);
}


template<typename UCharInputIterator>
void Scanner<UCharInputIterator>::ScanStringLiteral() {
  UChar quote = char_;
  UtfString v;
  bool escaped = false;
  bool unicode_scan_success = true;
  while (1) {
    
    if (unicode_scan_success) {
      Advance();
    }
    unicode_scan_success = true;
    
    if (char_ == quote) {
      if (!escaped) {
        break;
      }
      escaped = false;
    } else if (char_ == unicode::u32('\0') ||
               (Character::GetLineBreakType(char_, lookahead1_) != Character::LineBreakType::NONE && !escaped)) {
      TOKEN_ERROR("unterminated string literal.");
      if (Character::GetLineBreakType(char_, lookahead1_) != Character::LineBreakType::NONE) {
        line_terminator_state_.set_line_break_before_next();
        ConsumeLineBreak();
      }
      break;
    } else if (char_ == unicode::u32('\\')) {
      if (!escaped) {
        if (lookahead1_ == unicode::u32('u')) {
          unicode_scan_success = ScanUnicodeEscapeSequence(&v, true);
          continue;
        } else if (lookahead1_ == unicode::u32('x') ||
                   lookahead1_ == unicode::u32('X')) {
          ScanAsciiEscapeSequence(&v);
          continue;
        } else {
          escaped = !escaped; 
        }
      } else {
        escaped = !escaped;
      }
    } else {
      escaped = false;
    }
    v += char_;
  }

  BuildToken(TokenKind::kStringLiteral, v);
}


template<typename UCharInputIterator>
void Scanner<UCharInputIterator>::ScanDigit() {
  if (char_ == unicode::u32('0') && (lookahead1_ == unicode::u32('x') || lookahead1_ == unicode::u32('X'))) {
    return ScanHex();
  }

  if (char_ == unicode::u32('0')) {
    if (Character::IsNumericLiteral(lookahead1_)) {
      if (!LanguageModeUtil::IsOctalLiteralAllowed(compiler_option_)) {
        TOKEN_ERROR("Octal literals are not allowed in language mode " << LanguageModeUtil::ToString(compiler_option_));
      }
      return ScanOctalLiteral();
    } else if (lookahead1_ == unicode::u32('o') || lookahead1_ == unicode::u32('O')) {
      if (!LanguageModeUtil::IsBinaryLiteralAllowed(compiler_option_)) {
        TOKEN_ERROR("Binary literals are allowed in language mode " << LanguageModeUtil::ToString(compiler_option_));
      }
      return ScanBinaryLiteral();
    }
  }

  
  if ((char_ == unicode::u32('.') && Character::IsNumericLiteral(lookahead1_)) ||
      Character::IsNumericLiteral(char_)) {
    return ScanInteger();
  }

  UtfString v;
  while (Character::IsNumericLiteral(char_)) {
    v += char_;
    Advance();
  }
  ILLEGAL_TOKEN();
  BuildToken(TokenKind::kIllegal, v);
}


template<typename UCharInputIterator>
void Scanner<UCharInputIterator>::ScanHex() {
  UtfString v;
  v += char_;
  Advance();
  v += char_;
  Advance();
  while (Character::IsHexRange(char_)) {
    v += char_;
    Advance();
  }
  BuildToken(TokenKind::kNumericLiteral, v);
}


template<typename UCharInputIterator>
void Scanner<UCharInputIterator>::ScanOctalLiteral() {
  UtfString str;
  while (Character::IsNumericLiteral(char_)) {
    str += char_;
    Advance();
  }
  BuildToken(TokenKind::kOctalLiteral, str);
}


template<typename UCharInputIterator>
void Scanner<UCharInputIterator>::ScanBinaryLiteral() {
  UtfString str;
  str += char_;
  Advance();
  str += char_;
  Advance();
  if (!Character::IsBinaryCharacter(char_)) {
    TOKEN_ERROR("Invalid binary literal token.");
    Skip(str);
    BuildToken(TokenKind::kIllegal);
    return;
  }
  while (Character::IsBinaryCharacter(char_)) {
    str += char_;
    Advance();
  }
  BuildToken(TokenKind::kBinaryLiteral, str);
}


template<typename UCharInputIterator>
void Scanner<UCharInputIterator>::ScanInteger() {
  UtfString v;
  v += char_;
  bool js_double = char_ == unicode::u32('.');
  bool exponent = false;
  bool exponent_operator = false;
  Advance();
  
  while (1) {
    if (Character::IsNumericLiteral(char_)) {
      v += char_;
      if (exponent && !exponent_operator) {
        v += UChar::FromAscii('+');
      }
      exponent = false;
      exponent_operator = false;
    } else if (exponent && (char_ == unicode::u32('+') || char_ == unicode::u32('-'))) {
      v += char_;
      exponent_operator = true;
    } else if (exponent) {
      return ILLEGAL_TOKEN();
    } else if (!exponent &&
               char_ == unicode::u32('.') &&
               !js_double && Character::IsNumericLiteral(lookahead1_)) {
      v += char_;
      Advance();
      v += char_;
      js_double = true;
    } else if (char_ == unicode::u32('.') && js_double) {
      return ILLEGAL_TOKEN();
    } else if (char_ == unicode::u32('e') || char_ == unicode::u32('E')) {
      exponent = true;
      v += char_;
    } else {
      break;
    }
    Advance();
  }
  if (exponent || exponent_operator) {
    return ILLEGAL_TOKEN();
  }
  
  BuildToken(TokenKind::kNumericLiteral, v);
}


template<typename UCharInputIterator>
void Scanner<UCharInputIterator>::ScanIdentifier() {
  UtfString v;  
  while (Character::IsInIdentifierRange(char_) || char_ == unicode::u32('\\')) {
    if (char_ == unicode::u32('\\')) {
      if (ScanUnicodeEscapeSequence(&v, false)) {
        Advance();
      }
    } else {
      v += char_;
      Advance();
    }
  }
  TokenKind type = Token::GetIdentifierType(v.utf8_value(), compiler_option_);
  BuildToken(type, v);
}


template<typename UCharInputIterator>
void Scanner<UCharInputIterator>::ScanOperator() {
  switch (char_.ToAscii()) {
    case '+':
      return ScanArithmeticOperator(TokenKind::kIncrement, TokenKind::kAddLet, TokenKind::kPlus);
    case '-':
      return ScanArithmeticOperator(TokenKind::kDecrement, TokenKind::kSubLet, TokenKind::kMinus);
    case '*':
      return ScanArithmeticOperator(TokenKind::kIllegal, TokenKind::kMulLet, TokenKind::kMul, false);
    case '/':
      return ScanArithmeticOperator(TokenKind::kIllegal, TokenKind::kDivLet, TokenKind::kDiv, false);
    case '%':
      return ScanArithmeticOperator(TokenKind::kIllegal, TokenKind::kModLet, TokenKind::kMod, false);
    case '~':
      return ScanArithmeticOperator(TokenKind::kIllegal, TokenKind::kNorLet, TokenKind::kBitNor, false);
    case '^':
      return ScanArithmeticOperator(TokenKind::kIllegal, TokenKind::kXorLet, TokenKind::kBitXor, false);
    case '&':
      return ScanLogicalOperator(TokenKind::kLogicalAnd, TokenKind::kAndLet, TokenKind::kBitAnd);
    case '|':
      return ScanLogicalOperator(TokenKind::kLogicalOr, TokenKind::kOrLet, TokenKind::kBitOr);
    case ',':
      return BuildToken(TokenKind::kComma);
    case '`':
      return BuildToken(TokenKind::kBackquote);
    case '.':
      if (lookahead1_ == char_) {
        Advance();
        if (lookahead1_ == char_) {
          Advance();
          return BuildToken(TokenKind::kRest);
        }
        ILLEGAL_TOKEN();
      }
      return BuildToken(TokenKind::kDot);
    case '=':
      return ScanEqualityComparatorOrArrowGlyph();
    case '!':
      if (lookahead1_ == unicode::u32('=')) {
        Advance();
        return ScanEqualityComparatorOrArrowGlyph(true);
      }
      return BuildToken(TokenKind::kNot);
    case '<':
      return ScanBitwiseOrComparationOperator(
          TokenKind::kShiftLeft, TokenKind::kShiftLeftLet, TokenKind::kIllegal, TokenKind::kLess, TokenKind::kLessEqual);
    case '>':
      if (IsGenericMode()) {
        return BuildToken(TokenKind::kGreater);
      }
      return ScanBitwiseOrComparationOperator(
          TokenKind::kShiftRight, TokenKind::kShiftRightLet,
          TokenKind::kUShiftRight, TokenKind::kGreater, TokenKind::kGreaterEqual, true);
    default:
      ILLEGAL_TOKEN();
  }
}


template<typename UCharInputIterator>
void Scanner<UCharInputIterator>::ScanArithmeticOperator(TokenKind type1, TokenKind type2, TokenKind normal, bool has_let) {
  if (has_let && lookahead1_ == char_) {
    BuildToken(type1);
    return Advance();
  }
  if (lookahead1_ == unicode::u32('=')) {
    BuildToken(type2);
    return Advance();
  }
  BuildToken(normal);
}


template<typename UCharInputIterator>
void Scanner<UCharInputIterator>::ScanBitwiseOrComparationOperator(
    TokenKind type1, TokenKind type2, TokenKind type3, TokenKind normal, TokenKind equal_comparator, bool has_u) {
  if (lookahead1_ == char_) {
    Advance();
    if (lookahead1_ == unicode::u32('=')) {
      Advance();
      return BuildToken(type2);
    } else if (has_u && lookahead1_ == char_) {
      Advance();
      if (lookahead1_ == unicode::u32('=')) {
        Advance();
        return BuildToken(TokenKind::kUShiftRightLet);
      } else {
        return BuildToken(type3);
      }
    }
    return BuildToken(type1);
  } else if (lookahead1_ == unicode::u32('=')) {
    Advance();
    return BuildToken(equal_comparator);
  }
  BuildToken(normal);
}


// Check regular expression.
template<typename UCharInputIterator>
Token* Scanner<UCharInputIterator>::CheckRegularExpression(Token* token_info) {  
  
  // Prepare for scanning.
  BeforeScan();
  UtfString expr;

  if (token_info->type() == TokenKind::kDiv) {
  
    // In this method, char_ point next token of TS_DIV,
    // so we now assign a '/'.
    expr += UChar::FromAscii('/');
    bool escaped = false;
  
    while (1) {
      expr += char_;
      if (char_ == unicode::u32('\\')) {
        escaped = !escaped;
      } else if (char_ == unicode::u32('/')) {
        if (escaped) {
          escaped = false;
        } else {
          Advance();
          while (char_ == unicode::u32('g') ||
                 char_ == unicode::u32('i') ||
                 char_ == unicode::u32('m')) {
            expr += char_;
            Advance();
          }
          BuildToken(TokenKind::kRegularExpr, expr);
          break;
        }
      } else if (Character::GetLineBreakType(char_, lookahead1_) != Character::LineBreakType::NONE ||
                 char_.IsInvalid()) {
        TOKEN_ERROR("unterminated regular expression");
        return nullptr;
      } else {
        escaped = false;
      }
      Advance();
    }

    // Teardown.
    AfterScan();
    return &token_info_;
  }
  return nullptr;
}

template<typename UCharInputIterator>
void Scanner<UCharInputIterator>::RestoreScannerPosition(
    const RecordedCharPosition& rcp) {
  it_ = rcp.ucii();
  char_ = rcp.current();
  lookahead1_ = rcp.lookahead();
  scanner_source_position_ = rcp.ssp();
}


template<typename UCharInputIterator>
void Scanner<UCharInputIterator>::ScanEqualityComparatorOrArrowGlyph(bool not) {
  if (lookahead1_ == char_) {
    Advance();
    if (!not && lookahead1_ == char_) {
      Advance();
      return BuildToken(TokenKind::kEq);
    }
    return BuildToken(not? TokenKind::kNotEq: TokenKind::kEqual);
  } else if (!not && lookahead1_ == unicode::u32('>')) {
    Advance();
    return BuildToken(TokenKind::kArrowGlyph);
  }
  BuildToken(not? TokenKind::kNotEqual: TokenKind::kAssign);
}


template<typename UCharInputIterator>
bool Scanner<UCharInputIterator>::ScanUnicodeEscapeSequence(UtfString* v, bool in_string_literal) {
  Advance();
  if (char_ != unicode::u32('u')) {
    TOKEN_ERROR("UnicodeEscapeSequence not started with 'u'");
    return false;
  } else {
    Advance();
  }
  bool success;
  UC16 uc16 = ScanHexEscape(char_, 4, &success);
  if (!success) {
    Advance();
    TOKEN_ERROR("not allowed token in unicode escape sequence.");
    return false;
  } else if (!in_string_literal && (uc16 == '\\' || !Character::IsIdentifierStartChar(uc16))) {
    Advance();
    TOKEN_ERROR("invalid identifier.");
    return false;
  }
  UC8Bytes bytes = utf16::Convertor::Convert(uc16, 0);
  if (bytes.size() == 0) {
    Advance();
    TOKEN_ERROR("invalid unicode escape sequence.");
    return false;
  }
  (*v) += UChar(uc16, bytes);
  return true;
}


template <typename UCharInputIterator>
bool Scanner<UCharInputIterator>::ScanAsciiEscapeSequence(UtfString* v) {
  Advance(); // backslash
  Advance(); // x
  bool success;
  UC8 uc8 = unicode::u8(ScanHexEscape(char_, 2, &success));
  if (!success || !utf8::IsAscii(uc8)) {
    TOKEN_ERROR("invalid ascii escape sequence.");
    return false;
  }
  (*v) += UChar::FromAscii(static_cast<char>(uc8));
  return true;
}


template <typename UCharInputIterator>
template <typename T>
int Scanner<UCharInputIterator>::ToHexValue(const T& uchar) YATSC_NO_SE {
  int ret = 0;
  if (uchar >= unicode::u32('0') && uchar <= unicode::u32('9')) {
    ret = static_cast<int>(uchar - unicode::u32('0'));
  } else if (uchar >= unicode::u32('a') && uchar <= unicode::u32('f')) {
    ret = static_cast<int>(uchar - unicode::u32('a') + 10);
  } else if (uchar >= unicode::u32('A') && uchar <= unicode::u32('F')) {
    ret = static_cast<int>(uchar - unicode::u32('A') + 10);
  } else {
    return -1;
  }
  return ret;
}


template <typename UCharInputIterator>
bool Scanner<UCharInputIterator>::SkipWhiteSpace() {
  bool skip = false;
  while(Character::IsWhiteSpace(char_, lookahead1_) ||
        Character::IsSingleLineCommentStart(char_, lookahead1_) ||
        Character::IsMultiLineCommentStart(char_, lookahead1_) ||
        Character::GetLineBreakType(char_, lookahead1_) != Character::LineBreakType::NONE) {
    
    if (Character::GetLineBreakType(char_, lookahead1_) != Character::LineBreakType::NONE) {
      line_terminator_state_.set_line_break_before_next();
    }
    skip = true;
    if (!ConsumeLineBreak() && !SkipSingleLineComment() && !SkipMultiLineComment()) {
      Advance();
    }
  }
  if (char_ == unicode::u32(';')) {
    line_terminator_state_.set_line_terminator_before_next();
  }
  if (char_ == unicode::u32('\0')) {
    line_terminator_state_.set_line_break_before_next();
  }
  return skip;
}


template <typename UCharInputIterator>
bool Scanner<UCharInputIterator>::SkipWhiteSpaceOnly() {
  bool skip = false;
  while(Character::IsWhiteSpace(char_, lookahead1_) &&
        Character::GetLineBreakType(char_, lookahead1_) == Character::LineBreakType::NONE) {
    Advance();
    skip = true;
  }
  return skip;
}


template <typename UCharInputIterator>
bool Scanner<UCharInputIterator>::SkipSingleLineComment() {
  bool skip = false;
  
  if (Character::IsSingleLineCommentStart(char_, lookahead1_)) {
    Advance();
    if (Character::IsSingleLineCommentStart(char_, lookahead1_)) {
      Advance();
      Advance();
      SkipTripleSlashComment();
    } else {
      while (char_ != unicode::u32('\0') &&
             Character::GetLineBreakType(char_, lookahead1_) == Character::LineBreakType::NONE) {
        Advance();
      }
    }
    skip = true;
  }
  return skip;
}


template <typename UCharInputIterator>
void Scanner<UCharInputIterator>::SkipTripleSlashComment() {
  SkipWhiteSpaceOnly();
  Token info = token_info_;
  if (char_ == unicode::u32('<')) {
    Advance();
    if (char_ == unicode::u32('r') &&
        lookahead1_ == unicode::u32('e')) {
      ScanIdentifier();
      if (token_info_.value()->Equals("reference")) {
        SkipWhiteSpaceOnly();
        if (char_ == unicode::u32('p') && lookahead1_ == unicode::u32('a')) {
          ScanIdentifier();
          if (token_info_.value()->Equals("path")) {
            if (char_ == unicode::u32('=')) {
              Advance();
              ScanStringLiteral();
              Advance();
              SkipWhiteSpaceOnly();
              if (char_ == unicode::u32('/')) {
                Advance();
                SkipWhiteSpaceOnly();
                if (char_ == unicode::u32('>')) {
                  if (reference_path_callback_) {
                    reference_path_callback_(token_info_.value());
                  }
                }
              }
            }
          }
        }
      }
    }
  }
  
  while (char_ != unicode::u32('\0') &&
         Character::GetLineBreakType(char_, lookahead1_) == Character::LineBreakType::NONE) {
    Advance();
  }
  token_info_ = info;
}


template <typename UCharInputIterator>
bool Scanner<UCharInputIterator>::SkipMultiLineComment() {
  bool skip = false;
  if (Character::IsMultiLineCommentStart(char_, lookahead1_)) {
    UtfString str;
    str += char_;
    Advance(); // /
    str += char_;
    Advance(); // *
    while (!Character::IsMultiLineCommentEnd(char_, lookahead1_)) {
      Character::LineBreakType lt = Character::GetLineBreakType(char_, lookahead1_);
      if (lt != Character::LineBreakType::NONE) {
        LineFeed();
        line_terminator_state_.set_line_break_before_next();
      }

      if (lt == Character::LineBreakType::CRLF) {
        str += char_;
        Advance();
        str += char_;
        Advance();
      } else if (char_ == unicode::u16('\0')) {
        TOKEN_ERROR("unterminated multi line comment.");
        return true;
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


template <typename UCharInputIterator>
bool Scanner<UCharInputIterator>::SkipSignature() {
  bool skip = false;
  if (Character::IsUtfSignature(char_)) {
    Advance();
    skip = true;
  }
  return skip;
}


template <typename UCharInputIterator>
bool Scanner<UCharInputIterator>::ConsumeLineBreak() {
  bool is_break = false;
  Character::LineBreakType lt = Character::GetLineBreakType(char_, lookahead1_);
  if (lt == Character::LineBreakType::CRLF) {
    Advance();
    Advance();
    is_break = true;
  } else if (lt == Character::LineBreakType::LF ||
             lt == Character::LineBreakType::CR) {
    Advance();
    is_break = true;
  }
  
  if (is_break) {
    LineFeed();
  }
  return is_break;
}

} //namespace yatsc

#endif
