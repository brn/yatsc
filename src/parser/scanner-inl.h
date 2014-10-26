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

#define SYNTAX_ERROR(message) {                 \
  std::stringstream ss;                         \
  ss << message;                                \
  std::string&& s = std::move(ss.str());        \
  return Error(s.c_str());                      \
}

namespace yatsc {
template<typename UCharInputIterator>
template <typename ReferencePathCallback>
Scanner<UCharInputIterator>::Scanner(UCharInputIterator it,
                                     UCharInputIterator end,
                                     ErrorReporter* error_reporter,
                                     LiteralBuffer* literal_buffer,
                                     const CompilerOption& compiler_option,
                                     Handle<ModuleInfo> module_info,
                                     ReferencePathCallback reference_path_callback)
: generic_type_(0),
  it_(it),
  end_(end),
  error_reporter_(error_reporter),
  literal_buffer_(literal_buffer),
  compiler_option_(compiler_option),
  module_info_(module_info),
  reference_path_callback_(reference_path_callback){
  
  Advance();
  SkipWhiteSpace();
}


template<typename UCharInputIterator>
Scanner<UCharInputIterator>::Scanner(UCharInputIterator it,
                                     UCharInputIterator end,
                                     ErrorReporter* error_reporter,
                                     LiteralBuffer* literal_buffer,
                                     const CompilerOption& compiler_option,
                                     Handle<ModuleInfo> module_info)
: generic_type_(0),
  it_(it),
  end_(end),
  error_reporter_(error_reporter),
  literal_buffer_(literal_buffer),
  compiler_option_(compiler_option),
  module_info_(module_info) {
  
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
  scanner_source_position_.AdvancePosition(1);
  ++it_;
  if (it_ == end_) {
    lookahead1_ = UChar::Null();
    return;
  }
  
  lookahead1_ = *it_;
}


template<typename UCharInputIterator>
TokenInfo* Scanner<UCharInputIterator>::Scan() {
  BeforeScan();
  
  if (!char_.IsAscii()) {
    Illegal();
  }
  
  if (char_ == unicode::u8('\0')) {
    BuildToken(Token::END_OF_INPUT);
  } else if (char_ == unicode::u8(';')) {
    BuildToken(Token::LINE_TERMINATOR);
    Advance();
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

  AfterScan();
  return &token_info_;
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
          if (!ScanUnicodeEscapeSequence(&v, true)) {
            return;
          }
          continue;
        } else if (lookahead1_ == unicode::u8('x')) {
          if (!ScanAsciiEscapeSequence(&v)) {
            return;
          }
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

  BuildToken(Token::TS_STRING_LITERAL, v);
}


template<typename UCharInputIterator>
void Scanner<UCharInputIterator>::ScanDigit() {
  if (char_ == unicode::u8('0') && lookahead1_ == unicode::u8('x')) {
    return ScanHex();
  }

  if (char_ == unicode::u8('0')) {
    if (Character::IsNumericLiteral(lookahead1_)) {
      if (!LanguageModeUtil::IsOctalLiteralAllowed(compiler_option_)) {
        SYNTAX_ERROR("Octal literals are not allowed in language mode " << LanguageModeUtil::ToString(compiler_option_));
      }
      return ScanOctalLiteral();
    } else if (lookahead1_ == unicode::u8('o') || lookahead1_ == unicode::u8('O')) {
      if (!LanguageModeUtil::IsBinaryLiteralAllowed(compiler_option_)) {
        SYNTAX_ERROR("Binary literals are allowed in language mode " << LanguageModeUtil::ToString(compiler_option_));
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
  BuildToken(Token::TS_NUMERIC_LITERAL, v);
}


template<typename UCharInputIterator>
void Scanner<UCharInputIterator>::ScanOctalLiteral() {
  UtfString str;
  while (Character::IsNumericLiteral(char_)) {
    str += char_;
    Advance();
  }
  BuildToken(Token::TS_OCTAL_LITERAL, str);
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
  BuildToken(Token::TS_BINARY_LITERAL, str);
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
  
  BuildToken(Token::TS_NUMERIC_LITERAL, v);
}


template<typename UCharInputIterator>
void Scanner<UCharInputIterator>::ScanIdentifier() {
  UtfString v;  
  while (Character::IsInIdentifierRange(char_) || char_ == unicode::u8('\\')) {
    if (char_ == unicode::u8('\\')) {
      if (!ScanUnicodeEscapeSequence(&v, false)) {
        return;
      }
      Advance();
    } else {
      v += char_;
      Advance();
    }
  }
  Token type = TokenInfo::GetIdentifierType(v.utf8_value(), compiler_option_);
  BuildToken(type, v);
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
      return ScanArithmeticOperator(Token::ILLEGAL, Token::TS_XOR_LET, Token::TS_BIT_XOR, true);
    case '&':
      return ScanLogicalOperator(Token::TS_LOGICAL_AND, Token::TS_AND_LET, Token::TS_BIT_AND);
    case '|':
      return ScanLogicalOperator(Token::TS_LOGICAL_OR, Token::TS_OR_LET, Token::TS_BIT_OR);
    case ',':
      return BuildToken(Token::TS_COMMA);
    case '`':
      return BuildToken(Token::TS_BACKQUOTE);
    case '.':
      if (lookahead1_ == char_) {
        Advance();
        if (lookahead1_ == char_) {
          Advance();
          return BuildToken(Token::TS_REST);
        }
        Illegal();
      }
      return BuildToken(Token::TS_DOT);
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
      if (IsGenericMode()) {
        return BuildToken(Token::TS_GREATER);
      }
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
      Advance();
      if (lookahead1_ == unicode::u8('=')) {
        Advance();
        return BuildToken(Token::TS_U_SHIFT_RIGHT_LET);
      } else {
        return BuildToken(type3);
      }
    }
    return BuildToken(type1);
  } else if (lookahead1_ == unicode::u8('=')) {
    Advance();
    return BuildToken(equal_comparator);
  }
  BuildToken(normal);
}


// Check regular expression.
template<typename UCharInputIterator>
TokenInfo* Scanner<UCharInputIterator>::CheckRegularExpression(TokenInfo* token_info) {  
  
  // Prepare for scanning.
  BeforeScan();
  UtfString expr;

  if (token_info->type() == Token::TS_DIV) {
  
    // In this method, char_ point next token of TS_DIV,
    // so we now assign a '/'.
    expr += UChar::FromAscii('/');
    bool escaped = false;
  
    while (1) {
      expr += char_;
      if (char_ == unicode::u8('\\')) {
        escaped = !escaped;
      } else if (char_ == unicode::u8('/')) {
        if (escaped) {
          escaped = false;
        } else {
          Advance();
          while (char_ == unicode::u8('g') ||
                 char_ == unicode::u8('i') ||
                 char_ == unicode::u8('m')) {
            expr += char_;
            Advance();
          }
          BuildToken(Token::TS_REGULAR_EXPR, expr);
          break;
        }
      } else if (Character::GetLineBreakType(char_, lookahead1_) != Character::LineBreakType::NONE ||
                 char_.IsInvalid()) {
        Error("Unterminated regular expression");
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
      return BuildToken(Token::TS_EQ);
    }
    return BuildToken(not? Token::TS_NOT_EQ: Token::TS_EQUAL);
  } else if (!not && lookahead1_ == unicode::u8('>')) {
    Advance();
    return BuildToken(Token::TS_ARROW_GLYPH);
  }
  BuildToken(not? Token::TS_NOT_EQUAL: Token::TS_ASSIGN);
}


template<typename UCharInputIterator>
bool Scanner<UCharInputIterator>::ScanUnicodeEscapeSequence(UtfString* v, bool in_string_literal) {
  Advance();
  if (char_ != unicode::u8('u')) {
    Error("UnicodeEscapeSequence not started with 'u'");
    return false;
  }
  Advance();
  bool success;
  UC16 uc16 = ScanHexEscape(char_, 4, &success);
  if (!success) {
    Error("Not allowed token in UnicodeEscapeSequence.");
    return false;
  } else if (!in_string_literal && (uc16 == '\\' || !Character::IsIdentifierStartChar(uc16))) {
    Error("Not allowed token in UnicodeEscapeSequence.");
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
  (*v) += UChar::FromAscii(static_cast<char>(uc8));
  return true;
}


template <typename UCharInputIterator>
template <typename T>
int Scanner<UCharInputIterator>::ToHexValue(const T& uchar) YATSC_NO_SE {
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


template <typename UCharInputIterator>
bool Scanner<UCharInputIterator>::SkipWhiteSpace() {
  bool skip = false;
  while(Character::IsWhiteSpace(char_, lookahead1_) ||
        Character::IsSingleLineCommentStart(char_, lookahead1_) ||
        Character::IsMultiLineCommentStart(char_, lookahead1_)) {
    if (Character::GetLineBreakType(char_, lookahead1_) != Character::LineBreakType::NONE) {
      line_terminator_state_.set_line_break_before_next();
    }
    skip = true;
    if (!ConsumeLineBreak() && !SkipSingleLineComment() && !SkipMultiLineComment()) {
      Advance();
    }
  }
  if (char_ == unicode::u8(';')) {
    line_terminator_state_.set_line_terminator_before_next();
  }
  if (char_ == unicode::u8('\0')) {
    line_terminator_state_.set_line_break_before_next();
  }
  return skip;
}


template <typename UCharInputIterator>
bool Scanner<UCharInputIterator>::SkipSingleLineComment() {
  bool skip = false;
  if (Character::IsSingleLineCommentStart(char_, lookahead1_)) {
    Advance();
    if (Character::IsSingleLineCommentStart(char_, lookahead1_)) {
      UtfString str;
      Advance();
      Advance();
      while (Character::IsWhiteSpace(char_, lookahead1_)) {Advance();}
      if (char_ == unicode::u8('<')) {
        Advance();
        while (Character::IsWhiteSpace(char_, lookahead1_)) {Advance();}
        if (char_ == unicode::u8('r') &&
            lookahead1_ == unicode::u8('e')) {
          Advance();
          Advance();
          if (char_ == unicode::u8('f') &&
              lookahead1_ == unicode::u8('e')) {
            Advance();
            Advance();
            if (char_ == unicode::u8('r') &&
                lookahead1_ == unicode::u8('e')) {
              Advance();
              Advance();
              if (char_ == unicode::u8('n') &&
                  lookahead1_ == unicode::u8('c')) {
                Advance();
                Advance();
                if (char_ == unicode::u8('e')) {
                  Advance();
                  while (Character::IsWhiteSpace(char_, lookahead1_)) {Advance();}
                  if (char_ == unicode::u8('p') &&
                      lookahead1_ == unicode::u8('a')) {
                    Advance();
                    Advance();
                    if (char_ == unicode::u8('t') &&
                        lookahead1_ == unicode::u8('h')) {
                      Advance();
                      Advance();
                      if (char_ == unicode::u8('=')) {
                        Advance();
                        if (char_ == unicode::u8('\'') ||
                            char_ == unicode::u8('"')) {
                          try {
                            ScanStringLiteral();
                            if (char_ == unicode::u8('\'') ||
                                char_ == unicode::u8('"')) {
                              Advance();
                              while (Character::IsWhiteSpace(char_, lookahead1_)) {Advance();}
                              if (char_ == unicode::u8('/')) {
                                Advance();
                                while (Character::IsWhiteSpace(char_, lookahead1_)) {Advance();}
                                if (char_ == unicode::u8('>')) {
                                  Advance();
                                  if (reference_path_callback_) {
                                    reference_path_callback_(token_info_.value());
                                  }
                                  goto SKIP;
                                }
                              }
                            }
                          } catch(const TokenException& e) {goto SKIP;}
                        }
                      }
                    }
                  }
                }
              }
            }
          }
        }
      }
      goto SKIP;
    } else {
   SKIP:
      while (char_ != unicode::u8('\0') &&
             Character::GetLineBreakType(char_, lookahead1_) == Character::LineBreakType::NONE) {
        Advance();
      }
    }
    skip = true;
  }
  return skip;
}


template <typename UCharInputIterator>
bool Scanner<UCharInputIterator>::SkipMultiLineComment() {
  bool skip = false;
  if (Character::IsMultiLineCommentStart(char_, lookahead1_)) {
    UtfString str;
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

#undef SYNTAX_ERROR
} //namespace yatsc

#endif
