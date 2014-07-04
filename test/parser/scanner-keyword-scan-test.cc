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

#define KEYWORD_TEST(keyword, token_type)                       \
  TEST(ScannerTest, ScanIdentifier_keyword_##keyword)           \
  {                                                             \
    INIT(token, #keyword);                                      \
    ASSERT_STREQ(#token_type, token->ToString());                \
    ASSERT_EQ(yatsc::Token::token_type, token->type());           \
    yatsc::Utf8Value utf8 = token->value().ToUtf8Value();         \
    ASSERT_STREQ(utf8.value(), #keyword);                       \
    ASSERT_EQ(utf8.size(), strlen(#keyword));                   \
    END_SCAN;                                                   \
  }


#define RESERVED_KW_TEST(keyword)               \
  KEYWORD_TEST(keyword, FUTURE_RESERVED_WORD)


#define STRICT_RESERVED_KW_TEST(keyword)        \
  KEYWORD_TEST(keyword, FUTURE_STRICT_RESERVED_WORD)


KEYWORD_TEST(declare, TS_DECLARE);
KEYWORD_TEST(break, TS_BREAK);
KEYWORD_TEST(case, TS_CASE);
KEYWORD_TEST(catch, TS_CATCH);
KEYWORD_TEST(class, TS_CLASS);
KEYWORD_TEST(const, TS_CONST);
KEYWORD_TEST(continue, TS_CONTINUE);
KEYWORD_TEST(debugger, TS_DEBUGGER);
KEYWORD_TEST(default, TS_DEFAULT);
KEYWORD_TEST(delete, TS_DELETE);
KEYWORD_TEST(do, TS_DO);
KEYWORD_TEST(else, TS_ELSE);
KEYWORD_TEST(enum, TS_ENUM);
KEYWORD_TEST(export, TS_EXPORT);
KEYWORD_TEST(extends, TS_EXTENDS);
KEYWORD_TEST(false, TS_FALSE);
KEYWORD_TEST(finally, TS_FINALLY);
KEYWORD_TEST(for, TS_FOR);
KEYWORD_TEST(function, TS_FUNCTION);
KEYWORD_TEST(if, TS_IF);
KEYWORD_TEST(implements, TS_IMPLEMENTS);
KEYWORD_TEST(import, TS_IMPORT);
KEYWORD_TEST(in, TS_IN);
KEYWORD_TEST(instanceof, TS_INSTANCEOF);
KEYWORD_TEST(interface, TS_INTERFACE);
KEYWORD_TEST(let, TS_LET);
KEYWORD_TEST(NaN, TS_NAN);
KEYWORD_TEST(new, TS_NEW);
KEYWORD_TEST(null, TS_NULL);
STRICT_RESERVED_KW_TEST(package);
KEYWORD_TEST(private, TS_PRIVATE);
STRICT_RESERVED_KW_TEST(protected);
KEYWORD_TEST(public, TS_PUBLIC);
KEYWORD_TEST(return, TS_RETURN);
KEYWORD_TEST(static, TS_STATIC);
KEYWORD_TEST(super, TS_SUPER);
KEYWORD_TEST(switch, TS_SWITCH);
KEYWORD_TEST(this, TS_THIS);
KEYWORD_TEST(throw, TS_THROW);
KEYWORD_TEST(true, TS_TRUE);
KEYWORD_TEST(try, TS_TRY);
KEYWORD_TEST(typeof, TS_TYPEOF);
KEYWORD_TEST(undefined, TS_UNDEFINED);
KEYWORD_TEST(var, TS_VAR);
KEYWORD_TEST(void, TS_VOID);
KEYWORD_TEST(while, TS_WHILE);
KEYWORD_TEST(with, TS_WITH);
KEYWORD_TEST(yield, TS_YIELD);


