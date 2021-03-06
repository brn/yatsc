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
#include "../src/parser/error-reporter.h"
#include "../src/parser/error-formatter.h"
#include "../src/parser/error-descriptor.h"
#include "../src/utils/stl.h"
#include "../src/utils/notificator.h"
#include "./compare-node.h"
#include "./unicode-util.h"

bool print_stack_trace = true;
bool compare_node = true;

#define PARSER_TEST(name, method_expr, type, code, expected_str, error) { int line_num = __LINE__; \
    YatscParserTest(name, [&](yatsc::Parser<yatsc::SourceStream::iterator>* p){return p->method_expr;}, type, code, expected_str, error, line_num); \
  }

#define WRAP_PARSER_TEST(name, method_expr, type, code, expected_str, error) { int line_num = __LINE__; \
    YatscParserTest(name, [&](yatsc::Parser<yatsc::SourceStream::iterator>* p){return p->method_expr;}, type, code, expected_str, error, line_num); \
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
  typedef yatsc::SourceStream::iterator Iterator;
  auto module_info = Heap::NewHandle<ModuleInfo>(String(name), String(code), true);
  CompilerOption compiler_option;
  compiler_option.set_language_mode(type);
  auto lb = Heap::NewHandle<yatsc::LiteralBuffer>();
  auto global_scope = Heap::NewHandle<yatsc::ir::GlobalScope>(lb);
  auto irfactory = Heap::NewHandle<ir::IRFactory>();
  
  Scanner<Iterator> scanner(module_info->source_stream()->begin(), module_info->source_stream()->end(), lb.Get(), compiler_option);
  Notificator<void(const yatsc::String&)> notificator;
  Parser<Iterator> parser(compiler_option, &scanner, notificator, irfactory, module_info, global_scope);
  ErrorFormatter error_formatter(module_info);

  ParseResult result;
  try {
    result = fn(&parser);
  } catch(const FatalParseError& fpe) {}
  
  if (!error) {
    if (!module_info->HasError() && result && result.value() && compare_node) {
      yatsc::testing::CompareNode(line_num, result.value()->ToStringTree(), yatsc::String(expected_str));
    } else {
      if (print_stack_trace) {
        parser.PrintStackTrace();
      }
      error_formatter.Print(stderr, module_info->error_reporter());
    }
  } else {
    ASSERT_TRUE(module_info->error_reporter()->HasError());
  }
  print_stack_trace = true;
  compare_node = true;
}
