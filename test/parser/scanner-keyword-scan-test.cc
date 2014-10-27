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
  ASSERT_STREQ(str.c_str(), token->ToString().c_str());       \
  ASSERT_EQ(yatsc::Token::token_type, token->type());         \
  ASSERT_STREQ(token->utf8_value(), #keyword);                \
  ASSERT_EQ(token->value().utf8_length(), strlen(#keyword));  \
  END_SCAN;                                                   \


#define RESERVED_KW_TEST(keyword)               \
  KEYWORD_TEST(keyword, FUTURE_RESERVED_WORD)


#define STRICT_RESERVED_KW_TEST(keyword)                    \
  KEYWORD_TEST_STRICT(keyword, FUTURE_STRICT_RESERVED_WORD)

#define ES6_RESERVED_KW_TEST(keyword)                       \
  KEYWORD_TEST_ES6(keyword, FUTURE_STRICT_RESERVED_WORD)


KEYWORD_TEST_ALL(break, TS_BREAK);
KEYWORD_TEST_ALL(case, TS_CASE);
KEYWORD_TEST_ALL(catch, TS_CATCH);
KEYWORD_TEST_ALL(class, TS_CLASS);
KEYWORD_TEST(const, FUTURE_RESERVED_WORD);
KEYWORD_TEST_STRICT(const, FUTURE_RESERVED_WORD);
KEYWORD_TEST_ES6(const, TS_CONST);
KEYWORD_TEST_ALL(continue, TS_CONTINUE);
KEYWORD_TEST_ALL(debugger, TS_DEBUGGER);
KEYWORD_TEST_ALL(default, TS_DEFAULT);
KEYWORD_TEST_ALL(delete, TS_DELETE);
KEYWORD_TEST_ALL(do, TS_DO);
KEYWORD_TEST_ALL(else, TS_ELSE);
KEYWORD_TEST_ALL(enum, TS_ENUM);
KEYWORD_TEST_ALL(export, TS_EXPORT);
KEYWORD_TEST_ALL(extends, TS_EXTENDS);
KEYWORD_TEST_ALL(false, TS_FALSE);
KEYWORD_TEST_ALL(finally, TS_FINALLY);
KEYWORD_TEST_ALL(for, TS_FOR);
KEYWORD_TEST_ALL(function, TS_FUNCTION);
KEYWORD_TEST_ALL(if, TS_IF);
KEYWORD_TEST_ALL(implements, TS_IMPLEMENTS);
KEYWORD_TEST_ALL(import, TS_IMPORT);
KEYWORD_TEST_ALL(in, TS_IN);
KEYWORD_TEST_ALL(instanceof, TS_INSTANCEOF);
KEYWORD_TEST_ALL(interface, TS_INTERFACE);
KEYWORD_TEST(let, TS_IDENTIFIER);
KEYWORD_TEST_STRICT(let, TS_IDENTIFIER);
KEYWORD_TEST_ES6(let, TS_LET);
KEYWORD_TEST_ALL(NaN, TS_NAN);
KEYWORD_TEST_ALL(new, TS_NEW);
KEYWORD_TEST_ALL(null, TS_NULL);
STRICT_RESERVED_KW_TEST(package);
ES6_RESERVED_KW_TEST(package);
KEYWORD_TEST_ALL(private, TS_PRIVATE);
KEYWORD_TEST_ALL(protected, TS_PROTECTED);
KEYWORD_TEST_ALL(public, TS_PUBLIC);
KEYWORD_TEST_ALL(return, TS_RETURN);
KEYWORD_TEST_ALL(static, TS_STATIC);
KEYWORD_TEST_ALL(super, TS_SUPER);
KEYWORD_TEST_ALL(switch, TS_SWITCH);
KEYWORD_TEST_ALL(this, TS_THIS);
KEYWORD_TEST_ALL(throw, TS_THROW);
KEYWORD_TEST_ALL(true, TS_TRUE);
KEYWORD_TEST_ALL(try, TS_TRY);
KEYWORD_TEST_ALL(typeof, TS_TYPEOF);
KEYWORD_TEST_ALL(undefined, TS_UNDEFINED);
KEYWORD_TEST_ALL(var, TS_VAR);
KEYWORD_TEST_ALL(void, TS_VOID);
KEYWORD_TEST_ALL(while, TS_WHILE);
KEYWORD_TEST_ALL(with, TS_WITH);
KEYWORD_TEST_ES6(yield, TS_YIELD);

