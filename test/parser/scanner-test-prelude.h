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

#ifndef TEST_PARSER_SCANNER_TEST_PRELUDE_H_
#define TEST_PARSER_SCANNER_TEST_PRELUDE_H_

#include <gtest/gtest.h>
#include "../../src/compiler-option.h"
#include "../../src/parser/scanner.h"
#include "../../src/parser/uchar.h"
#include "../unicode-util.h"


#define INIT__(var, str, type)                                          \
  typedef std::vector<yatsc::UChar>::iterator Iterator;                  \
  std::vector<yatsc::UChar> v__ = yatsc::testing::AsciiToUCharVector(str); \
  yatsc::CompilerOption compiler_option;                                 \
  compiler_option.set_language_mode(type);                              \
  yatsc::Scanner<Iterator> scanner(v__.begin(), v__.end(), compiler_option); \
  auto var = scanner.Scan();

#define INIT(var, str) INIT__(var, str, yatsc::LanguageMode::ES3)
#define INIT_STRICT(var, str) INIT__(var, str, yatsc::LanguageMode::ES5_STRICT)
#define INIT_HARMONY(var, str) INIT__(var, str, yatsc::LanguageMode::HARMONY)


#define END_SCAN                                  \
  {                                               \
  auto t = scanner.Scan();                        \
  ASSERT_STREQ(t->ToString(), "END_OF_INPUT");     \
  ASSERT_EQ(t->type(), yatsc::Token::END_OF_INPUT); \
  }

#endif
