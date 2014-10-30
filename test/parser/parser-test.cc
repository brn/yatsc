// The MIT License (MIT)
// 
// Copyright (c) 2013 Taketoshi Aono(brn)
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.


#include "../gtest-header.h"
#include "../parser-util.h"
#include "../readfile.h"


#define ENTIRE_PARSER_TEST(type, code, expected_str)                      \
  PARSER_TEST(Parse(), type, code, expected_str, false, std::exception)

#define ENTIRE_PARSER_THROW_TEST(type, code, error_type)                  \
  PARSER_TEST(Parse(), type, code, "", true, error_type)

#define ENTIRE_PARSER_TEST_ALL(code, expected_str)                        \
  [&]{ENTIRE_PARSER_TEST(yatsc::LanguageMode::ES3, code, expected_str);}(); \
  [&]{ENTIRE_PARSER_TEST(yatsc::LanguageMode::ES5_STRICT, code, expected_str);}(); \
  [&]{ENTIRE_PARSER_TEST(yatsc::LanguageMode::ES3, code, expected_str);}()

#define ENTIRE_PARSER_THROW_TEST_ALL(code, error_type)                    \
  [&]{ENTIRE_PARSER_THROW_TEST(yatsc::LanguageMode::ES3, code, error_type);}(); \
  [&]{ENTIRE_PARSER_THROW_TEST(yatsc::LanguageMode::ES5_STRICT, code, error_type);}(); \
  [&]{ENTIRE_PARSER_THROW_TEST(yatsc::LanguageMode::ES6, code, error_type);}()


TEST(ParserTest, Parse) {
  print_stack_trace = false;
  // yatsc::String code1 = yatsc::testing::ReadFile("test/microsoft/typescript/src/compiler/core.ts");
  // ENTIRE_PARSER_TEST_ALL(code1.c_str(), "");
  // yatsc::String code2 = yatsc::testing::ReadFile("test/microsoft/typescript/src/compiler/parser.ts");
  // ENTIRE_PARSER_TEST_ALL(code2.c_str(), "");
  // yatsc::String code3 = yatsc::testing::ReadFile("test/microsoft/typescript/src/compiler/scanner.ts");
  // ENTIRE_PARSER_TEST_ALL(code3.c_str(), "");
  //yatsc::String code4 = yatsc::testing::ReadFile("test/microsoft/typescript/src/compiler/diagnosticInformationMap.generated.ts");
  yatsc::String code5 = yatsc::testing::ReadFile("/Users/aono_taketoshi/github/npm-modules/yatsc/test/doppio/src/option_parser.ts");
  ENTIRE_PARSER_TEST_ALL(code5.c_str(), "");
}
