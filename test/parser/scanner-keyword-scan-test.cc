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

#include "./scanner-test-prelude.h"
#ifdef interface
#undef interface
#endif

#define KEYWORD_TEST_ALL(keyword, token_type)   \
  KEYWORD_TEST(keyword, token_type);            \
  KEYWORD_TEST_STRICT(keyword, token_type);     \
  KEYWORD_TEST_ES6(keyword, token_type);


#define KEYWORD_TEST(keyword, token_type)             \
  TEST(ScannerTest, ScanIdentifier_keyword_##keyword) \
  {                                                   \
    INIT(token, #keyword);                            \
    KEYWORD_TEST__(keyword, token_type);              \
  }


#define KEYWORD_TEST_STRICT(keyword, token_type)              \
  TEST(ScannerTest, ScanIdentifier_keyword_strict_##keyword)  \
  {                                                           \
    INIT_STRICT(token, #keyword);                             \
    KEYWORD_TEST__(keyword, token_type);                      \
  }


#define KEYWORD_TEST_ES6(keyword, token_type)             \
  TEST(ScannerTest, ScanIdentifier_keyword_es6_##keyword) \
  {                                                       \
    INIT_ES6(token, #keyword);                            \
    KEYWORD_TEST__(keyword, token_type);                  \
  }


#define KEYWORD_TEST__(keyword, token_type)                   \
  yatsc::String str(#token_type);                             \
  str += "[";                                                 \
  str += #keyword;                                            \
  str += "]";                                                 \
  ASSERT_STREQ(str.c_str(), token->ToStringWithValue().c_str());  \
  ASSERT_EQ(yatsc::TokenKind::token_type, token->type());         \
  ASSERT_STREQ(token->utf8_value(), #keyword);                \
  ASSERT_EQ(token->value()->utf8_length(), strlen(#keyword));  \
  END_SCAN;                                                   \


#define RESERVED_KW_TEST(keyword)               \
  KEYWORD_TEST(keyword, kFutureReservedWord)


#define STRICT_RESERVED_KW_TEST(keyword)                    \
  KEYWORD_TEST_STRICT(keyword, kFutureStrictReservedWord)

#define ES6_RESERVED_KW_TEST(keyword)                       \
  KEYWORD_TEST_ES6(keyword, kFutureStrictReservedWord)


KEYWORD_TEST_ALL(break, kBreak);
KEYWORD_TEST_ALL(case, kCase);
KEYWORD_TEST_ALL(catch, kCatch);
KEYWORD_TEST_ALL(class, kClass);
KEYWORD_TEST(const, kFutureReservedWord);
KEYWORD_TEST_STRICT(const, kFutureReservedWord);
KEYWORD_TEST_ES6(const, kConst);
KEYWORD_TEST_ALL(continue, kContinue);
KEYWORD_TEST_ALL(debugger, kDebugger);
KEYWORD_TEST_ALL(default, kDefault);
KEYWORD_TEST_ALL(delete, kDelete);
KEYWORD_TEST_ALL(do, kDo);
KEYWORD_TEST_ALL(else, kElse);
KEYWORD_TEST_ALL(enum, kEnum);
KEYWORD_TEST_ALL(export, kExport);
KEYWORD_TEST_ALL(extends, kExtends);
KEYWORD_TEST_ALL(false, kFalse);
KEYWORD_TEST_ALL(finally, kFinally);
KEYWORD_TEST_ALL(for, kFor);
KEYWORD_TEST_ALL(function, kFunction);
KEYWORD_TEST_ALL(if, kIf);
KEYWORD_TEST_ALL(implements, kImplements);
KEYWORD_TEST_ALL(import, kImport);
KEYWORD_TEST_ALL(in, kIn);
KEYWORD_TEST_ALL(instanceof, kInstanceof);
KEYWORD_TEST_ALL(interface, kInterface);
KEYWORD_TEST(let, kIdentifier);
KEYWORD_TEST_STRICT(let, kIdentifier);
KEYWORD_TEST_ES6(let, kLet);
KEYWORD_TEST_ALL(NaN, kNan);
KEYWORD_TEST_ALL(new, kNew);
KEYWORD_TEST_ALL(null, kNull);
STRICT_RESERVED_KW_TEST(package);
ES6_RESERVED_KW_TEST(package);
KEYWORD_TEST_ALL(private, kPrivate);
KEYWORD_TEST_ALL(protected, kProtected);
KEYWORD_TEST_ALL(public, kPublic);
KEYWORD_TEST_ALL(return, kReturn);
KEYWORD_TEST_ALL(static, kStatic);
KEYWORD_TEST_ALL(super, kSuper);
KEYWORD_TEST_ALL(switch, kSwitch);
KEYWORD_TEST_ALL(this, kThis);
KEYWORD_TEST_ALL(throw, kThrow);
KEYWORD_TEST_ALL(true, kTrue);
KEYWORD_TEST_ALL(try, kTry);
KEYWORD_TEST_ALL(typeof, kTypeof);
KEYWORD_TEST_ALL(undefined, kUndefined);
KEYWORD_TEST_ALL(var, kVar);
KEYWORD_TEST_ALL(void, kVoid);
KEYWORD_TEST_ALL(while, kWhile);
KEYWORD_TEST_ALL(with, kWith);
KEYWORD_TEST_ES6(yield, kYield);

