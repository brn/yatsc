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


#define ENTIRE_PARSER_TEST(name, type, expected_str)                    \
  yatsc::String code = yatsc::testing::ReadFile(name);                  \
  compare_node = false;                                                 \
  print_stack_trace = false;                                            \
  WRAP_PARSER_TEST(name, Parse(), type, code.c_str(), expected_str, false)

#define ENTIRE_PARSER_THROW_TEST(name, type)                      \
  yatsc::String code = yatsc::testing::ReadFile(name);            \
  WRAP_PARSER_TEST(name, Parse(), type, code.c_str(), "", true)

#define ENTIRE_PARSER_TEST_ALL(name, expected_str)                      \
  [&]{ENTIRE_PARSER_TEST(name, yatsc::LanguageMode::ES3, expected_str);}(); \
  [&]{ENTIRE_PARSER_TEST(name, yatsc::LanguageMode::ES5_STRICT, expected_str);}(); \
  [&]{ENTIRE_PARSER_TEST(name, yatsc::LanguageMode::ES3, expected_str);}()

#define ENTIRE_PARSER_THROW_TEST_ALL(name)                              \
  [&]{ENTIRE_PARSER_THROW_TEST(name, yatsc::LanguageMode::ES3);}(); \
  [&]{ENTIRE_PARSER_THROW_TEST(name, yatsc::LanguageMode::ES5_STRICT);}(); \
  [&]{ENTIRE_PARSER_THROW_TEST(name, yatsc::LanguageMode::ES6);}()


yatsc::Vector<yatsc::String> Split(const yatsc::String &s, char delim) {
  yatsc::Vector<yatsc::String> elems;
  yatsc::StringStream ss(s);
  yatsc::String item;
  while (getline(ss, item, delim)) {
    if (!item.empty()) {
      elems.push_back(item);
    }
  }
  return std::move(elems);
}


void ParseTestCases() {
  yatsc::String filelist = yatsc::testing::ReadFile("test/parser/parser-test-case/filelist");
  auto ret = Split(filelist, '\n');
  for (auto i: ret) {
    printf("Parse => %s\n", i.c_str());
    ENTIRE_PARSER_TEST_ALL(i.c_str(), "");
  }
}


TEST(ParserTest, Parse) {
  // yatsc::String code1 = yatsc::testing::ReadFile("test/microsoft/typescript/src/compiler/core.ts");
  // ENTIRE_PARSER_TEST_ALL(code1.c_str(), "");
  // yatsc::String code2 = yatsc::testing::ReadFile("test/microsoft/typescript/src/compiler/parser.ts");
  // ENTIRE_PARSER_TEST_ALL(code2.c_str(), "");
  // yatsc::String code3 = yatsc::testing::ReadFile("test/microsoft/typescript/src/compiler/scanner.ts");
  // ENTIRE_PARSER_TEST_ALL(code3.c_str(), "");
  //yatsc::String code4 = yatsc::testing::ReadFile("test/microsoft/typescript/src/compiler/diagnosticInformationMap.generated.ts");
  //ENTIRE_PARSER_TEST_ALL("test/microsoft/typescript/tests/cases/conformance/ambient/ambientInsideNonAmbient.ts", "");
  // ENTIRE_PARSER_TEST_ALL("test/microsoft/typescript/tests/cases/conformance/ambient/ambientExternalModuleInsideNonAmbient.ts", "");
  //ENTIRE_PARSER_TEST_ALL("test/microsoft/typescript/src/compiler/checker.ts", "");
  //ENTIRE_PARSER_TEST_ALL("test/microsoft/typescript/tests/cases/compiler/sourceMap-LineBreaks.ts.", "");
  //ENTIRE_PARSER_TEST_ALL("test/microsoft/typescript/tests/cases/conformance/parser/ecmascript5/ErrorRecovery/ParameterLists/parserErrorRecovery_ParameterList2.ts", "");
  ParseTestCases();
}
