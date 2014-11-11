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


#include "../src/ir/node.h"
#include "../src/parser/parser.h"
#include "../src/parser/literalbuffer.h"
#include "../src/parser/semantic-error.h"
#include "../src/parser/error-formatter.h"
#include "../src/parser/error-descriptor.h"
#include "../src/utils/stl.h"
#include "../src/utils/notificator.h"
#include "./compare-node.h"
#include "./unicode-util.h"

bool print_stack_trace = true;

#define PARSER_TEST(name, method_expr, type, code, expected_str, error) { int line_num = __LINE__; \
    YatscParserTest(name, [&](yatsc::Parser<yatsc::UCharBuffer::iterator>* p){return p->method_expr;}, type, code, expected_str, error, line_num); \
  }

#define WRAP_PARSER_TEST(name, method_expr, type, code, expected_str, error) { int line_num = __LINE__; \
    YatscParserTest(name, [&](yatsc::Parser<yatsc::UCharBuffer::iterator>* p){return yatsc::ParseResult(p->method_expr, true);}, type, code, expected_str, error, line_num); \
  }


template <typename T>
void YatscParserTest(const char* name,
                     T fn,
                     yatsc::LanguageMode type,
                     const char* code,
                     const char* expected_str,
                     bool error,
                     int line_num) {
  using namespace yatsc;
  typedef std::vector<yatsc::UChar>::iterator Iterator;
  auto module_info = Heap::NewHandle<ModuleInfo>(String(name), String(code), true);
  UCharBuffer uchar_buffer = yatsc::testing::AsciiToUCharVector(module_info->source_stream()->raw_buffer());
  CompilerOption compiler_option;
  compiler_option.set_language_mode(type);
  LiteralBuffer lb;
  
  Scanner<Iterator> scanner(uchar_buffer.begin(), uchar_buffer.end(), &lb, compiler_option);
  Notificator<void(const yatsc::String&)> notificator;
  Parser<Iterator> parser(compiler_option, &scanner, notificator, module_info);
  ErrorFormatter error_formatter(module_info);
  
  auto result = fn(&parser);
  
  if (!error) {
    if (!module_info->HasError() && result.value()) {
      yatsc::testing::CompareNode(line_num, result.value()->ToStringTree(), yatsc::String(expected_str));
    } else {
      if (print_stack_trace) {
        parser.PrintStackTrace();
      }
      error_formatter.Print(stderr, module_info->semantic_error());
    }
  } else {
    ASSERT_TRUE(module_info->semantic_error()->HasError());
  }
  print_stack_trace = true;
}
