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


#define KEYWORD_TEST__(keyword, token_type)                       \
  yatsc::String str(#token_type);                                 \
  str += "[";                                                     \
  str += #keyword;                                                \
  str += "]";                                                     \
  ASSERT_STREQ(str.c_str(), token->ToStringWithValue().c_str());  \
  ASSERT_EQ(yatsc::TokenKind::k##token_type, token->type());      \
  ASSERT_STREQ(token->utf8_value(), #keyword);                    \
  ASSERT_EQ(token->value()->utf8_length(), strlen(#keyword));     \
  END_SCAN;                                                       \


#define RESERVED_KW_TEST(keyword)               \
  KEYWORD_TEST(keyword, kFutureReservedWord)


#define STRICT_RESERVED_KW_TEST(keyword)                    \
  KEYWORD_TEST_STRICT(keyword, FutureStrictReservedWord)

#define ES6_RESERVED_KW_TEST(keyword)                       \
  KEYWORD_TEST_ES6(keyword, FutureStrictReservedWord)


KEYWORD_TEST_ALL(break, Break);
KEYWORD_TEST_ALL(case, Case);
KEYWORD_TEST_ALL(catch, Catch);
KEYWORD_TEST_ALL(class, Class);
KEYWORD_TEST(const, FutureReservedWord);
KEYWORD_TEST_STRICT(const, FutureReservedWord);
KEYWORD_TEST_ES6(const, Const);
KEYWORD_TEST_ALL(continue, Continue);
KEYWORD_TEST_ALL(debugger, Debugger);
KEYWORD_TEST_ALL(default, Default);
KEYWORD_TEST_ALL(delete, Delete);
KEYWORD_TEST_ALL(do, Do);
KEYWORD_TEST_ALL(else, Else);
KEYWORD_TEST_ALL(enum, Enum);
KEYWORD_TEST_ALL(export, Export);
KEYWORD_TEST_ALL(extends, Extends);
KEYWORD_TEST_ALL(false, False);
KEYWORD_TEST_ALL(finally, Finally);
KEYWORD_TEST_ALL(for, For);
KEYWORD_TEST_ALL(function, Function);
KEYWORD_TEST_ALL(if, If);
KEYWORD_TEST_ALL(implements, Implements);
KEYWORD_TEST_ALL(import, Import);
KEYWORD_TEST_ALL(in, In);
KEYWORD_TEST_ALL(instanceof, Instanceof);
KEYWORD_TEST_ALL(interface, Interface);
KEYWORD_TEST(let, Identifier);
KEYWORD_TEST_STRICT(let, Identifier);
KEYWORD_TEST_ES6(let, Let);
KEYWORD_TEST_ALL(NaN, Nan);
KEYWORD_TEST_ALL(new, New);
KEYWORD_TEST_ALL(null, Null);
STRICT_RESERVED_KW_TEST(package);
ES6_RESERVED_KW_TEST(package);
KEYWORD_TEST_ALL(private, Private);
KEYWORD_TEST_ALL(protected, Protected);
KEYWORD_TEST_ALL(public, Public);
KEYWORD_TEST_ALL(return, Return);
KEYWORD_TEST_ALL(static, Static);
KEYWORD_TEST_ALL(super, Super);
KEYWORD_TEST_ALL(switch, Switch);
KEYWORD_TEST_ALL(this, This);
KEYWORD_TEST_ALL(throw, Throw);
KEYWORD_TEST_ALL(true, True);
KEYWORD_TEST_ALL(try, Try);
KEYWORD_TEST_ALL(typeof, Typeof);
KEYWORD_TEST_ALL(undefined, Undefined);
KEYWORD_TEST_ALL(var, Var);
KEYWORD_TEST_ALL(void, Void);
KEYWORD_TEST_ALL(while, While);
KEYWORD_TEST_ALL(with, With);

