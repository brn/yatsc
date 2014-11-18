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

namespace yatsc {

// Ascii range puncture list.
// For fast lookup.
static const TokenKind kPunctures[] = {
  TokenKind::kIllegal,          // 0
  TokenKind::kIllegal,          // 1
  TokenKind::kIllegal,          // 2
  TokenKind::kIllegal,          // 3
  TokenKind::kIllegal,          // 4
  TokenKind::kIllegal,          // 5
  TokenKind::kIllegal,          // 6
  TokenKind::kIllegal,          // 7
  TokenKind::kIllegal,          // 8
  TokenKind::kIllegal,          // 9
  TokenKind::kIllegal,          // 10
  TokenKind::kIllegal,          // 11
  TokenKind::kIllegal,          // 12
  TokenKind::kIllegal,          // 13
  TokenKind::kIllegal,          // 14
  TokenKind::kIllegal,          // 15
  TokenKind::kIllegal,          // 16
  TokenKind::kIllegal,          // 17
  TokenKind::kIllegal,          // 18
  TokenKind::kIllegal,          // 19
  TokenKind::kIllegal,          // 20
  TokenKind::kIllegal,          // 21
  TokenKind::kIllegal,          // 22
  TokenKind::kIllegal,          // 23
  TokenKind::kIllegal,          // 24
  TokenKind::kIllegal,          // 25
  TokenKind::kIllegal,          // 26
  TokenKind::kIllegal,          // 27
  TokenKind::kIllegal,          // 28
  TokenKind::kIllegal,          // 29
  TokenKind::kIllegal,          // 30
  TokenKind::kIllegal,          // 31
  TokenKind::kIllegal,          // 32
  TokenKind::kIllegal,          // 33 !
  TokenKind::kIllegal,          // 34 '
  TokenKind::kIllegal,          // 35 #
  TokenKind::kIllegal,          // 36 $
  TokenKind::kIllegal,          // 37 %
  TokenKind::kIllegal,          // 38 &
  TokenKind::kIllegal,          // 39 '
  TokenKind::kLeftParen,        // 40 (
  TokenKind::kRightParen,       // 41 )
  TokenKind::kIllegal,          // 42 *
  TokenKind::kIllegal,          // 43 +
  TokenKind::kIllegal,          // 44 ,
  TokenKind::kIllegal,          // 45 -
  TokenKind::kIllegal,          // 46 .
  TokenKind::kIllegal,          // 47 /
  TokenKind::kIllegal,          // 48 0
  TokenKind::kIllegal,          // 49 1
  TokenKind::kIllegal,          // 50 2
  TokenKind::kIllegal,          // 51 3
  TokenKind::kIllegal,          // 52 4
  TokenKind::kIllegal,          // 53 5
  TokenKind::kIllegal,          // 54 6
  TokenKind::kIllegal,          // 55 7
  TokenKind::kIllegal,          // 56 8
  TokenKind::kIllegal,          // 57 9
  TokenKind::kColon,            // 58 :
  TokenKind::kIllegal,          // 59 ;
  TokenKind::kIllegal,          // 60 <
  TokenKind::kIllegal,          // 61 =
  TokenKind::kIllegal,          // 62 >
  TokenKind::kQuestionMark,     // 63 ?
  TokenKind::kIllegal,          // 64 @
  TokenKind::kIllegal,          // 65 A
  TokenKind::kIllegal,          // 66 B
  TokenKind::kIllegal,          // 67 C
  TokenKind::kIllegal,          // 68 D
  TokenKind::kIllegal,          // 69 E
  TokenKind::kIllegal,          // 70 F
  TokenKind::kIllegal,          // 71 G
  TokenKind::kIllegal,          // 72 H
  TokenKind::kIllegal,          // 73 I
  TokenKind::kIllegal,          // 74 J
  TokenKind::kIllegal,          // 75 K
  TokenKind::kIllegal,          // 76 L
  TokenKind::kIllegal,          // 77 M
  TokenKind::kIllegal,          // 78 N
  TokenKind::kIllegal,          // 79 O
  TokenKind::kIllegal,          // 80 P
  TokenKind::kIllegal,          // 81 Q
  TokenKind::kIllegal,          // 82 R
  TokenKind::kIllegal,          // 83 S
  TokenKind::kIllegal,          // 84 T
  TokenKind::kIllegal,          // 85 U
  TokenKind::kIllegal,          // 86 V
  TokenKind::kIllegal,          // 87 W
  TokenKind::kIllegal,          // 88 X
  TokenKind::kIllegal,          // 89 Y
  TokenKind::kIllegal,          // 90 Z
  TokenKind::kLeftBracket,      // 91 [
  TokenKind::kIllegal,          // 92 BACK_SLASH
  TokenKind::kRightBracket,     // 93 ]
  TokenKind::kIllegal,          // 94 ^
  TokenKind::kIllegal,          // 95 _
  TokenKind::kIllegal,          // 96 `
  TokenKind::kIllegal,          // 97 a
  TokenKind::kIllegal,          // 98 b
  TokenKind::kIllegal,          // 99 c
  TokenKind::kIllegal,          // 100 d
  TokenKind::kIllegal,          // 101 e
  TokenKind::kIllegal,          // 102 f
  TokenKind::kIllegal,          // 103 g
  TokenKind::kIllegal,          // 104 h
  TokenKind::kIllegal,          // 105 i
  TokenKind::kIllegal,          // 106 j
  TokenKind::kIllegal,          // 107 k
  TokenKind::kIllegal,          // 108 l
  TokenKind::kIllegal,          // 109 m
  TokenKind::kIllegal,          // 110 n
  TokenKind::kIllegal,          // 111 o
  TokenKind::kIllegal,          // 112 p
  TokenKind::kIllegal,          // 113 q
  TokenKind::kIllegal,          // 114 r
  TokenKind::kIllegal,          // 115 s
  TokenKind::kIllegal,          // 116 t
  TokenKind::kIllegal,          // 117 u
  TokenKind::kIllegal,          // 118 v
  TokenKind::kIllegal,          // 119 w
  TokenKind::kIllegal,          // 120 x
  TokenKind::kIllegal,          // 121 y
  TokenKind::kIllegal,          // 122 z
  TokenKind::kLeftBrace,        // 123 {
  TokenKind::kIllegal,          // 124 |
  TokenKind::kRightBrace,       // 125 }
  TokenKind::kIllegal,          // 126 ~
  TokenKind::kIllegal           // 127 DEL
};


// Borrowed from v8 javascript engine.
#define KEYWORDS(KEYWORD_GROUP, KEYWORD)                                \
  KEYWORD_GROUP('b')                                                    \
  KEYWORD("break", TokenKind::kBreak)                                   \
  KEYWORD_GROUP('c')                                                    \
  KEYWORD("case", TokenKind::kCase)                                     \
  KEYWORD("catch", TokenKind::kCatch)                                   \
  KEYWORD("class", TokenKind::kClass)                                   \
  KEYWORD("const", LanguageModeUtil::IsES6(co)? TokenKind::kConst: TokenKind::kFutureReservedWord) \
  KEYWORD("continue", TokenKind::kContinue)                             \
  KEYWORD_GROUP('d')                                                    \
  KEYWORD("debugger", TokenKind::kDebugger)                             \
  KEYWORD("default", TokenKind::kDefault)                               \
  KEYWORD("delete", TokenKind::kDelete)                                 \
  KEYWORD("do", TokenKind::kDo)                                         \
  KEYWORD_GROUP('e')                                                    \
  KEYWORD("else", TokenKind::kElse)                                     \
  KEYWORD("enum", TokenKind::kEnum)                                     \
  KEYWORD("export", TokenKind::kExport)                                 \
  KEYWORD("extends", TokenKind::kExtends)                               \
  KEYWORD_GROUP('f')                                                    \
  KEYWORD("false", TokenKind::kFalse)                                   \
  KEYWORD("finally", TokenKind::kFinally)                               \
  KEYWORD("for", TokenKind::kFor)                                       \
  KEYWORD("function", TokenKind::kFunction)                             \
  KEYWORD_GROUP('i')                                                    \
  KEYWORD("if", TokenKind::kIf)                                         \
  KEYWORD("implements", TokenKind::kImplements)                         \
  KEYWORD("import", TokenKind::kImport)                                 \
  KEYWORD("in", TokenKind::kIn)                                         \
  KEYWORD("instanceof", TokenKind::kInstanceof)                         \
  KEYWORD("interface", TokenKind::kInterface)                           \
  KEYWORD_GROUP('l')                                                    \
  KEYWORD("let", LanguageModeUtil::IsES6(co)? TokenKind::kLet: TokenKind::kIdentifier) \
  KEYWORD_GROUP('m')                                                    \
  KEYWORD("module", ModuleTypeUtil::IsModuleKeywordAllowed(co)? TokenKind::kModule: TokenKind::kIdentifier) \
  KEYWORD_GROUP('n')                                                    \
  KEYWORD("new", TokenKind::kNew)                                       \
  KEYWORD("null", TokenKind::kNull)                                     \
  KEYWORD_GROUP('N')                                                    \
  KEYWORD("NaN", TokenKind::kNan)                                       \
  KEYWORD_GROUP('p')                                                    \
  KEYWORD("package", LanguageModeUtil::IsFutureReservedWord(co)?        \
          TokenKind::kFutureStrictReservedWord: TokenKind::kIdentifier) \
  KEYWORD("private", TokenKind::kPrivate)                               \
  KEYWORD("protected", TokenKind::kProtected)                           \
  KEYWORD("public", TokenKind::kPublic)                                 \
  KEYWORD_GROUP('r')                                                    \
  KEYWORD("return", TokenKind::kReturn)                                 \
  KEYWORD_GROUP('s')                                                    \
  KEYWORD("static", TokenKind::kStatic)                                 \
  KEYWORD("super", TokenKind::kSuper)                                   \
  KEYWORD("switch", TokenKind::kSwitch)                                 \
  KEYWORD_GROUP('t')                                                    \
  KEYWORD("this", TokenKind::kThis)                                     \
  KEYWORD("throw", TokenKind::kThrow)                                   \
  KEYWORD("true", TokenKind::kTrue)                                     \
  KEYWORD("try", TokenKind::kTry)                                       \
  KEYWORD("typeof", TokenKind::kTypeof)                                 \
  KEYWORD_GROUP('u')                                                    \
  KEYWORD("undefined", TokenKind::kUndefined)                           \
  KEYWORD_GROUP('v')                                                    \
  KEYWORD("var", TokenKind::kVar)                                       \
  KEYWORD("void", TokenKind::kVoid)                                     \
  KEYWORD_GROUP('w')                                                    \
  KEYWORD("while", TokenKind::kWhile)                                   \
  KEYWORD("with", TokenKind::kWith)                                     \
  KEYWORD_GROUP('y')                                                    \
  KEYWORD("yield", LanguageModeUtil::IsES6(co)? TokenKind::kYield: TokenKind::kIdentifier)


// Get Identifier type from string.
TokenKind Token::GetIdentifierType(const char* maybe_keyword, const CompilerOption& co) {
  const size_t input_length = Strlen(maybe_keyword);
  const int min_length = 2;
  const int max_length = 10;
  if (input_length < min_length || input_length > max_length) {
    return TokenKind::kIdentifier;
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
  return TokenKind::kIdentifier;
}


// Get puncture type from char.
TokenKind Token::GetPunctureType(const UChar& uchar) {
  return kPunctures[uchar.ToUC8Ascii()];
}


uint8_t GetOperandPriority(TokenKind kind) {
  switch (kind) {
    case TokenKind::kMod:
    case TokenKind::kDiv:
    case TokenKind::kMul:
      return 1;
    case TokenKind::kPlus:
    case TokenKind::kMinus:
      return 2;
    case TokenKind::kShiftRight:
    case TokenKind::kUShiftRight:
    case TokenKind::kShiftLeft:
      return 3;
    case TokenKind::kLess:
    case TokenKind::kGreater:
    case TokenKind::kGreaterEqual:
    case TokenKind::kLessEqual:
    case TokenKind::kIn:
    case TokenKind::kInstanceof:
      return 4;
    case TokenKind::kEq:
    case TokenKind::kEqual:
    case TokenKind::kNotEq:
    case TokenKind::kNotEqual:
      return 5;
    case TokenKind::kBitAnd:
      return 6;
    case TokenKind::kBitXor:
      return 7;
    case TokenKind::kBitOr:
      return 8;
    case TokenKind::kLogicalAnd:
      return 9;
    case TokenKind::kLogicalOr:
      return 10;
    default:
      return 0;
  }
}

bool Token::IsKeyword(TokenKind kind) {
  return kind == TokenKind::kBreak ||
    kind == TokenKind::kCase ||
    kind == TokenKind::kCatch ||
    kind == TokenKind::kClass ||
    kind == TokenKind::kConst ||
    kind == TokenKind::kContinue ||
    kind == TokenKind::kDebugger ||
    kind == TokenKind::kDefault ||
    kind == TokenKind::kDelete ||
    kind == TokenKind::kDo ||
    kind == TokenKind::kElse ||
    kind == TokenKind::kEnum ||
    kind == TokenKind::kExport ||
    kind == TokenKind::kExtends ||
    kind == TokenKind::kFalse ||
    kind == TokenKind::kFinally ||
    kind == TokenKind::kFor ||
    kind == TokenKind::kFunction ||
    kind == TokenKind::kIf ||
    kind == TokenKind::kImplements ||
    kind == TokenKind::kImport ||
    kind == TokenKind::kIn ||
    kind == TokenKind::kInstanceof ||
    kind == TokenKind::kInterface ||
    kind == TokenKind::kLet ||
    kind == TokenKind::kModule ||
    kind == TokenKind::kNew ||
    kind == TokenKind::kNull ||
    kind == TokenKind::kNan ||
    kind == TokenKind::kPackage ||
    kind == TokenKind::kPrivate ||
    kind == TokenKind::kProtected ||
    kind == TokenKind::kPublic ||
    kind == TokenKind::kReturn ||
    kind == TokenKind::kStatic ||
    kind == TokenKind::kSuper ||
    kind == TokenKind::kSwitch ||
    kind == TokenKind::kThis ||
    kind == TokenKind::kThrow ||
    kind == TokenKind::kTrue ||
    kind == TokenKind::kTry ||
    kind == TokenKind::kTypeof ||
    kind == TokenKind::kUndefined ||
    kind == TokenKind::kVar ||
    kind == TokenKind::kVoid ||
    kind == TokenKind::kWhile ||
    kind == TokenKind::kWith ||
    kind == TokenKind::kYield;
}


Token Token::kNullToken;
}
