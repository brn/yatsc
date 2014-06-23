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
Scanner<UCharInputIterator>::Scanner(UCharInputIterator it,
                                      UCharInputIterator end,
                                      const CompilerOption& compiler_option)
    : has_line_terminator_before_next_(false),
      current_position_(0),
      line_number_(1),
      line_head_(it),
      it_(it),
      end_(end),
      compiler_option_(compiler_option) {
  Advance();
  SkipWhiteSpace();
}


template<typename UCharInputIterator>
void Scanner<UCharInputIterator>::Advance()  {
  if (it_ == end_) {
    char_ = UChar::Null();
    return;
  }

  char_ = *it_;
  current_position_++;
  ++it_;
  
  if (it_ == end_) {
    lookahead1_ = UChar::Null();
    return;
  }
  
  lookahead1_ = *it_;
}


template<typename UCharInputIterator>
const TokenInfo* Scanner<UCharInputIterator>::Scan() {
  has_line_terminator_before_next_ = false;
  last_multi_line_comment_.Clear();
  
  if (!char_.IsAscii()) {
    Illegal();
    return &token_info_;
  }
  
  if (char_ == unicode::u8('\0')) {
    BuildToken(Token::END_OF_INPUT);
  } else if (char_ == unicode::u8(';')) {
    return Scan();
  } else if (Character::IsPuncture(char_)) {
    BuildToken(TokenInfo::GetPunctureType(char_));
    Advance();
  } else if (Character::IsIdentifierStart(char_) ||
             Character::IsUnicodeEscapeSequenceStart(char_, lookahead1_)) {
    ScanIdentifier();
  } else if (Character::IsStringLiteralStart(char_)) {
    ScanStringLiteral();
    Advance();
  } else if (Character::IsDigitStart(char_, lookahead1_)) {
    ScanDigit();
  } else if (Character::IsOperatorStart(char_)) {
    ScanOperator();
    Advance();
  } else {
    Illegal();
  }
  
  SkipWhiteSpace();
  return &token_info_;
}


template<typename UCharInputIterator>
UC16 Scanner<UCharInputIterator>::ScanHexEscape(const UChar& uchar, int len, bool* success) {
  UC16 result = 0;
  for (int i = 0; i < len; ++i) {
    const int d = ToHexValue(uchar);
    if (d < 0) {
      *success = false;
      return d;
    }
    *success = true;
    result = result * 16 + d;
    Advance();
  }
  return result;
}


template<typename UCharInputIterator>
void Scanner<UCharInputIterator>::ScanStringLiteral() {
  UChar quote = char_;
  UtfString v;
  bool escaped = false;
  while (1) {
    Advance();
    if (char_ == quote) {
      if (!escaped) {
        break;
      }
      escaped = false;
    } else if (char_ == unicode::u8('\0')) {
      return Error("Unterminated string literal.");
    } else if (char_ == unicode::u8('\\')) {
      if (!escaped) {
        if (lookahead1_ == unicode::u8('u')) {
          if (!ScanUnicodeEscapeSequence(&v)) {
            return;
          }
        } else if (lookahead1_ == unicode::u8('x')) {
          if (!ScanAsciiEscapeSequence(&v)) {
            return;
          }
        } else {
          escaped = !escaped; 
        }
      } else {
        escaped = !escaped;
      }
    }
    v += char_;
  }

  BuildToken(Token::TS_STRING_LITERAL, std::move(v));
}


template<typename UCharInputIterator>
void Scanner<UCharInputIterator>::ScanDigit() {
  if (char_ == unicode::u8('0') && lookahead1_ == unicode::u8('x')) {
    return ScanHex();
  }

  if (char_ == unicode::u8('0')) {
    if (Character::IsNumericLiteral(lookahead1_)) {
      if (!LanguageModeUtil::IsOctalLiteralAllowed(compiler_option_)) {
        return Error("Octal literals are not allowed in strict mode.");
      }
      return ScanOctalLiteral();
    } else if (lookahead1_ == unicode::u8('o') || lookahead1_ == unicode::u8('O')) {
      if (!LanguageModeUtil::IsBinaryLiteralAllowed(compiler_option_)) {
        return Error("Binary literals are allowed only in harmony mode.");
      }
      return ScanBinaryLiteral();
    }
  }

  
  if ((char_ == unicode::u8('.') && Character::IsNumericLiteral(lookahead1_)) ||
      Character::IsNumericLiteral(char_)) {
    return ScanInteger();
  }
  Illegal();
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
  BuildToken(Token::TS_NUMERIC_LITERAL, std::move(v));
}


template<typename UCharInputIterator>
void Scanner<UCharInputIterator>::ScanOctalLiteral() {
  UtfString str;
  while (Character::IsNumericLiteral(char_)) {
    str += char_;
    Advance();
  }
  BuildToken(Token::TS_OCTAL_LITERAL, std::move(str));
}


template<typename UCharInputIterator>
void Scanner<UCharInputIterator>::ScanBinaryLiteral() {
  UtfString str;
  str += char_;
  Advance();
  str += char_;
  Advance();
  if (!Character::IsBinaryCharacter(char_)) {
    return Error("Invalid binary literal token.");
  }
  while (Character::IsBinaryCharacter(char_)) {
    str += char_;
    Advance();
  }
  BuildToken(Token::TS_BINARY_LITERAL, std::move(str));
}


template<typename UCharInputIterator>
void Scanner<UCharInputIterator>::ScanInteger() {
  UtfString v;
  v += char_;
  bool js_double = char_ == unicode::u8('.');
  bool exponent = false;
  bool exponent_operator = false;
  Advance();
  
  while (1) {
    if (Character::IsNumericLiteral(char_)) {
      v += char_;
      if (exponent && !exponent_operator) {
        return Illegal();
      }
      exponent = false;
      exponent_operator = false;
    } else if (exponent && (char_ == unicode::u8('+') || char_ == unicode::u8('-'))) {
      v += char_;
      exponent_operator = true;
    } else if (exponent) {
      return Illegal();
    } else if (!exponent &&
               char_ == unicode::u8('.') &&
               !js_double && Character::IsNumericLiteral(lookahead1_)) {
      v += char_;
      Advance();
      v += char_;
      js_double = true;
    } else if (char_ == unicode::u8('.') && js_double) {
      return Illegal();
    } else if (char_ == unicode::u8('e') || char_ == unicode::u8('E')) {
      exponent = true;
      v += char_;
    } else {
      break;
    }
    Advance();
  }
  if (exponent || exponent_operator) {
    return Illegal();
  }
  
  BuildToken(Token::TS_NUMERIC_LITERAL, std::move(v));
}


template<typename UCharInputIterator>
void Scanner<UCharInputIterator>::ScanIdentifier() {
  UtfString v;  
  while (Character::IsInIdentifierRange(char_) || char_ == unicode::u8('\\')) {
    if (char_ == unicode::u8('\\')) {
      if (!ScanUnicodeEscapeSequence(&v)) {
        return;
      }
    } else {
      v += char_;
      Advance();
    }
  }
  Utf8Value utf8_value = v.ToUtf8Value();
  Token type = TokenInfo::GetIdentifierType(utf8_value.value(),
                                            LanguageModeUtil::IsHarmony(compiler_option_));
  BuildToken(type, std::move(v));
}


template<typename UCharInputIterator>
void Scanner<UCharInputIterator>::ScanOperator() {
  switch (char_.ToAscii()) {
    case '+':
      return ScanArithmeticOperator(Token::TS_INCREMENT, Token::TS_ADD_LET, Token::TS_PLUS);
    case '-':
      return ScanArithmeticOperator(Token::TS_DECREMENT, Token::TS_SUB_LET, Token::TS_MINUS);
    case '*':
      return ScanArithmeticOperator(Token::ILLEGAL, Token::TS_MUL_LET, Token::TS_MUL, false);
    case '/':
      return ScanArithmeticOperator(Token::ILLEGAL, Token::TS_DIV_LET, Token::TS_DIV, false);
    case '%':
      return ScanArithmeticOperator(Token::ILLEGAL, Token::TS_MOD_LET, Token::TS_MOD, false);
    case '~':
      return ScanArithmeticOperator(Token::ILLEGAL, Token::TS_NOR_LET, Token::TS_BIT_NOR, false);
    case '^':
      return ScanArithmeticOperator(Token::ILLEGAL, Token::TS_XOR_LET, Token::TS_BIT_XOR, false);
    case '&':
      return ScanLogicalOperator(Token::TS_LOGICAL_AND, Token::TS_AND_LET, Token::TS_BIT_AND);
    case '|':
      return ScanLogicalOperator(Token::TS_LOGICAL_OR, Token::TS_OR_LET, Token::TS_BIT_OR);
    case '=':
      return ScanEqualityComparatorOrArrowGlyph();
    case '!':
      if (lookahead1_ == unicode::u8('=')) {
        Advance();
        return ScanEqualityComparatorOrArrowGlyph(true);
      }
      return BuildToken(Token::TS_NOT);
    case '<':
      return ScanBitwiseOrComparationOperator(
          Token::TS_SHIFT_LEFT, Token::TS_SHIFT_LEFT_LET, Token::ILLEGAL, Token::TS_LESS, Token::TS_LESS_EQUAL);
    case '>':
      return ScanBitwiseOrComparationOperator(
          Token::TS_SHIFT_RIGHT, Token::TS_SHIFT_RIGHT_LET,
          Token::TS_U_SHIFT_RIGHT, Token::TS_GREATER, Token::TS_GREATER_EQUAL, true);
    default:
      Illegal();
  }
}


template<typename UCharInputIterator>
void Scanner<UCharInputIterator>::ScanArithmeticOperator(Token type1, Token type2, Token normal, bool has_let) {
  if (has_let && lookahead1_ == char_) {
    BuildToken(type1);
    return Advance();
  }
  if (lookahead1_ == unicode::u8('=')) {
    BuildToken(type2);
    return Advance();
  }
  BuildToken(normal);
}


template<typename UCharInputIterator>
void Scanner<UCharInputIterator>::ScanBitwiseOrComparationOperator(
    Token type1, Token type2, Token type3, Token normal, Token equal_comparator, bool has_u) {
  if (lookahead1_ == char_) {
    Advance();
    if (lookahead1_ == unicode::u8('=')) {
      Advance();
      return BuildToken(type2);
    } else if (has_u && lookahead1_ == char_) {
      return BuildToken(type3);
    }
    return BuildToken(type1);
  } else if (lookahead1_ == unicode::u8('=')) {
    return BuildToken(equal_comparator);
  }
  BuildToken(normal);
}


template<typename UCharInputIterator>
void Scanner<UCharInputIterator>::ScanEqualityComparatorOrArrowGlyph(bool not) {
  if (lookahead1_ == char_) {
    Advance();
    if (!not && lookahead1_ == char_) {
      return BuildToken(Token::TS_EQ);
    }
    return BuildToken(not? Token::TS_NOT_EQ: Token::TS_EQUAL);
  } else if (!not && lookahead1_ == unicode::u8('>')) {
    return BuildToken(Token::TS_ARROW_GLYPH);
  }
  BuildToken(not? Token::TS_NOT_EQUAL: Token::TS_ASSIGN);
}


template<typename UCharInputIterator>
bool Scanner<UCharInputIterator>::ScanUnicodeEscapeSequence(UtfString* v) {
  Advance();
  if (char_ != unicode::u8('u')) {
    Error("Illegal Token");
    return false;
  }
  Advance();
  bool success;
  UC16 uc16 = ScanHexEscape(char_, 4, &success);
  if (!success || uc16 == '\\' || !Character::IsIdentifierStartChar(uc16)) {
    Illegal();
    return false;
  }
  UC8Bytes bytes = utf16::Convertor::Convert(uc16, 0);
  if (bytes.size() == 0) {
    Illegal();
    return false;
  }
  (*v) += UChar(uc16, bytes);
  return true;
}


template <typename UCharInputIterator>
bool Scanner<UCharInputIterator>::ScanAsciiEscapeSequence(UtfString* v) {
  Advance();
  if (char_ != unicode::u8('x')) {
    Error("Illegal Token");
    return false;
  }
  Advance();
  bool success;
  UC8 uc8 = unicode::u8(ScanHexEscape(char_, 2, &success));
  if (!success || !utf8::IsAscii(uc8)) {
    Illegal();
    return false;
  }
  UC8Bytes bytes{{static_cast<char>(uc8), '\0'}};
  (*v) += UChar(uc8, bytes);
  return true;
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
