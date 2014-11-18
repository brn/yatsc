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
#include "../../src/parser/error-formatter.h"
#include "../../src/parser/error-descriptor.h"
#include "../../src/parser/error-reporter.h"
#include "../../src/parser/literalbuffer.h"
#include "../../src/compiler-option.h"
#include "../../src/parser/scanner.h"
#include "../../src/parser/uchar.h"
#include "../../src/utils/stl.h"
#include "../unicode-util.h"


#define SCAN__(var)                                                     \
  yatsc::LiteralBuffer lb;                                              \
  yatsc::ErrorReporter er;                                              \
  yatsc::Scanner<Iterator> scanner(v__.begin(), v__.end(), &lb, compiler_option); \
  scanner.SetErrorCallback([&] (const char* message, const yatsc::SourcePosition& source_position){ \
    er.SyntaxError(source_position) << message;                         \
  });                                                                   \
  auto var = scanner.Scan();                                            \
  if (er.HasError()) {                                                  \
    yatsc::ErrorFormatter error_formatter(module_info);                 \
    error_formatter.Print(stderr, er);                                  \
    ASSERT_FALSE(er.HasError());                                        \
  }                                                                     \


#define SCAN_ERROR__(var)                                               \
  yatsc::LiteralBuffer lb;                                              \
  yatsc::ErrorReporter er;                                              \
  yatsc::Scanner<Iterator> scanner(v__.begin(), v__.end(), &lb, compiler_option); \
  scanner.SetErrorCallback([&] (const char* message, const yatsc::SourcePosition& source_position){ \
    er.SyntaxError(source_position) << message;                         \
  });                                                                   \
  scanner.Scan();                                                       \
  ASSERT_TRUE(er.HasError());


#define INIT__(SCAN, var, str, type)                                    \
  typedef yatsc::Vector<yatsc::UChar>::iterator Iterator;               \
  yatsc::String n = "anonymous";                                        \
  yatsc::Handle<yatsc::ModuleInfo> module_info = yatsc::Heap::NewHandle<yatsc::ModuleInfo>(n, yatsc::String(str), true); \
  yatsc::Vector<yatsc::UChar> v__ = yatsc::testing::AsciiToUCharVector(yatsc::String(module_info->raw_source_code())); \
  yatsc::CompilerOption compiler_option;                                \
  compiler_option.set_language_mode(type);                              \
  SCAN(var)
  

#define INIT(var, str) INIT__(SCAN__, var, str, yatsc::LanguageMode::ES3)
#define INIT_ERROR(var, str) INIT__(SCAN_ERROR__, var, str, yatsc::LanguageMode::ES3)
#define INIT_STRICT(var, str) INIT__(SCAN__, var, str, yatsc::LanguageMode::ES5_STRICT)
#define INIT_STRICT_ERROR(var, str) INIT__(SCAN_ERROR__, var, str, yatsc::LanguageMode::ES5_STRICT)
#define INIT_ES6(var, str) INIT__(SCAN__, var, str, yatsc::LanguageMode::ES6)
#define INIT_ES6_ERROR(var, str) INIT__(SCAN_ERROR__, var, str, yatsc::LanguageMode::ES6)


#define END_SCAN                                          \
  {                                                       \
    auto t = scanner.Scan();                              \
    ASSERT_STREQ(t->ToString().c_str(), "Eof");  \
    ASSERT_EQ(t->type(), yatsc::TokenKind::kEof);     \
  }

#endif
