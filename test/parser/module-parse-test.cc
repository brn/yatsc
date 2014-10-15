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


#define MODULE_TEST(type, code, expected_str)                           \
  PARSER_TEST(ParseModule(), type, code, expected_str, false, std::exception)

#define MODULE_THROW_TEST(type, code, error_type)                       \
  PARSER_TEST(ParseModule(), type, code, "", true, error_type)

#define MODULE_TEST_ALL(code, expected_str)                             \
  [&]{MODULE_TEST(yatsc::LanguageMode::ES3, code, expected_str);}();    \
  [&]{MODULE_TEST(yatsc::LanguageMode::ES5_STRICT, code, expected_str);}(); \
  [&]{MODULE_TEST(yatsc::LanguageMode::ES3, code, expected_str);}()

#define MODULE_THROW_TEST_ALL(code, error_type)                         \
  [&]{MODULE_THROW_TEST(yatsc::LanguageMode::ES3, code, error_type);}(); \
  [&]{MODULE_THROW_TEST(yatsc::LanguageMode::ES5_STRICT, code, error_type);}(); \
  [&]{MODULE_THROW_TEST(yatsc::LanguageMode::ES6, code, error_type);}()



TEST(ModuleParser, ParseModule) {
  MODULE_TEST_ALL("import 'foo/bar/baz.js'",
                  "[FileScopeView]\n"
                  "  [ImportView]\n"
                  "    [Empty]\n"
                  "    [StringView]['foo/bar/baz.js']");


  MODULE_TEST_ALL("import a from 'foo/bar/baz.js'",
                  "[FileScopeView]\n"
                  "  [ImportView]\n"
                  "    [ImportListView]\n"
                  "      [NameView][a]\n"
                  "    [StringView]['foo/bar/baz.js']");


  MODULE_TEST_ALL("import a, b from 'foo/bar/baz.js'",
                  "[FileScopeView]\n"
                  "  [ImportView]\n"
                  "    [ImportListView]\n"
                  "      [NameView][a]\n"
                  "      [NameView][b]\n"
                  "    [StringView]['foo/bar/baz.js']");


  MODULE_TEST_ALL("import a, {b as c} from 'foo/bar/baz.js'",
                  "[FileScopeView]\n"
                  "  [ImportView]\n"
                  "    [ImportListView]\n"
                  "      [NameView][a]\n"
                  "      [NamedImportListView]\n"
                  "        [NamedImportView]\n"
                  "          [NameView][b]\n"
                  "          [NameView][c]\n"
                  "    [StringView]['foo/bar/baz.js']");
}
