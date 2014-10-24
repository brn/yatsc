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
#include "../src/utils/stl.h"
#include "./compare-node.h"
#include "./unicode-util.h"


#define PARSER_METHOD_CALL__(var, method_expr)  \
  var.method_expr

#define PARSER_TEST(method_expr, type, code, expected_str, dothrow, error_type) { int line_num__ = __LINE__; \
    typedef std::vector<yatsc::UChar>::iterator Iterator;               \
    yatsc::String s = code;                                             \
    yatsc::String n = "anonymous";                                      \
    yatsc::Handle<yatsc::ModuleInfo> module_info = yatsc::Heap::NewHandle<yatsc::ModuleInfo>(n, true); \
    yatsc::ErrorReporter error_reporter(s, module_info);                \
    yatsc::UCharBuffer v__ = yatsc::testing::AsciiToUCharVector(code);  \
    yatsc::CompilerOption compiler_option;                              \
    compiler_option.set_language_mode(type);                            \
    yatsc::LiteralBuffer lb;                                            \
    yatsc::Scanner<Iterator> scanner(v__.begin(), v__.end(), &error_reporter, &lb, compiler_option, module_info); \
    yatsc::Parser<Iterator> parser(compiler_option, &scanner, &error_reporter, module_info); \
    if (!dothrow) {                                                     \
      try {                                                             \
        auto node = PARSER_METHOD_CALL__(parser, method_expr);          \
        yatsc::testing::CompareNode(line_num__, node->ToStringTree(), yatsc::String(expected_str)); \
      } catch(const std::exception& e) {                                \
        parser.PrintStackTrace();                                       \
        yatsc::FPrintf(stderr, "%s\n", e.what());                       \
        throw e;                                                        \
      }                                                                 \
    } else {                                                            \
      ASSERT_THROW(PARSER_METHOD_CALL__(parser, method_expr), error_type); \
    }                                                                   \
  }
