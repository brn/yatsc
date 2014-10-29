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

#include "../gtest-header.h"
#include "../../src/parser/error-reporter.h"
#include "../../src/parser/literalbuffer.h"
#include "../../src/compiler-option.h"
#include "../../src/parser/scanner.h"
#include "../../src/parser/uchar.h"
#include "../../src/utils/stl.h"
#include "../unicode-util.h"


#define SCAN__(var, error_reporter, ex_type)                            \
  yatsc::LiteralBuffer lb;                                              \
  yatsc::Scanner<Iterator> scanner(v__.begin(), v__.end(), &error_reporter, &lb, compiler_option, module_info); \
  auto var = scanner.Scan();


#define SCAN_THROW__(var, error_reporter, ex_type)                      \
  yatsc::LiteralBuffer lb;                                              \
  yatsc::Scanner<Iterator> scanner(v__.begin(), v__.end(), &error_reporter, &lb, compiler_option, module_info); \
  ASSERT_THROW(scanner.Scan(), ex_type);


#define INIT__(SCAN, var, str, type, ex_type)                           \
  typedef yatsc::Vector<yatsc::UChar>::iterator Iterator;               \
  yatsc::String s = str;                                                \
  yatsc::String n = "anonymous";                                        \
  yatsc::Handle<yatsc::ModuleInfo> module_info = yatsc::Heap::NewHandle<yatsc::ModuleInfo>(n, true); \
  yatsc::ErrorReporter error_reporter(s, module_info);                  \
  yatsc::Vector<yatsc::UChar> v__ = yatsc::testing::AsciiToUCharVector(str); \
  yatsc::CompilerOption compiler_option;                                \
  compiler_option.set_language_mode(type);                              \
  SCAN(var, error_reporter, ex_type)
  

#define INIT(var, str) INIT__(SCAN__, var, str, yatsc::LanguageMode::ES3, nullptr)
#define INIT_THROW(var, str, ex_type) INIT__(SCAN_THROW__, var, str, yatsc::LanguageMode::ES3, ex_type)
#define INIT_STRICT(var, str) INIT__(SCAN__, var, str, yatsc::LanguageMode::ES5_STRICT, nullptr)
#define INIT_STRICT_THROW(var, str, ex_type) INIT__(SCAN_THROW__, var, str, yatsc::LanguageMode::ES5_STRICT, ex_type)
#define INIT_ES6(var, str) INIT__(SCAN__, var, str, yatsc::LanguageMode::ES6, nullptr)
#define INIT_ES6_THROW(var, str, ex_type) INIT__(SCAN_THROW__, var, str, yatsc::LanguageMode::ES6, ex_type)


#define END_SCAN                                          \
  {                                                       \
    auto t = scanner.Scan();                              \
    ASSERT_STREQ(t->ToString().c_str(), "END_OF_INPUT");  \
    ASSERT_EQ(t->type(), yatsc::Token::END_OF_INPUT);     \
  }

#endif
