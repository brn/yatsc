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


#define TYPE_PARSER_TEST(type, code, expected_str)                      \
  PARSER_TEST(ParseTypeExpression(), type, code, expected_str, false, std::exception)

#define TYPE_PARSER_THROW_TEST(type, code, error_type)                  \
  PARSER_TEST(ParseTypeExpression(), type, code, "", true, error_type)

#define TYPE_PARSER_TEST_ALL(code, expected_str)                        \
  [&]{TYPE_PARSER_TEST(yatsc::LanguageMode::ES3, code, expected_str);}(); \
  [&]{TYPE_PARSER_TEST(yatsc::LanguageMode::ES5_STRICT, code, expected_str);}(); \
  [&]{TYPE_PARSER_TEST(yatsc::LanguageMode::ES3, code, expected_str);}()

#define TYPE_PARSER_THROW_TEST_ALL(code, error_type)                    \
  [&]{TYPE_PARSER_THROW_TEST(yatsc::LanguageMode::ES3, code, error_type);}(); \
  [&]{TYPE_PARSER_THROW_TEST(yatsc::LanguageMode::ES5_STRICT, code, error_type);}(); \
  [&]{TYPE_PARSER_THROW_TEST(yatsc::LanguageMode::ES6, code, error_type);}()


TEST(TypeParser, ParseTypeExpression) {
  // Predefined type test.
  TYPE_PARSER_TEST_ALL("string",
                       "[SimpleTypeExprView]\n"
                       "  [NameView][string]");


  TYPE_PARSER_TEST_ALL("boolean",
                       "[SimpleTypeExprView]\n"
                       "  [NameView][boolean]");


  TYPE_PARSER_TEST_ALL("number",
                       "[SimpleTypeExprView]\n"
                       "  [NameView][number]");

  
  TYPE_PARSER_TEST_ALL("void",
                       "[SimpleTypeExprView]\n"
                       "  [NameView][void]");

  TYPE_PARSER_TEST_ALL("any",
                       "[SimpleTypeExprView]\n"
                       "  [NameView][any]");
  // Predefined type test.


  TYPE_PARSER_TEST_ALL("typeof foo",
                       "[TypeQueryView]\n"
                       "  [NameView][foo]");


  TYPE_PARSER_TEST_ALL("typeof foo.bar.baz",
                       "[TypeQueryView]\n"
                       "  [GetPropView]\n"
                       "    [GetPropView]\n"
                       "      [NameView][foo]\n"
                       "      [NameView][bar]\n"
                       "    [NameView][baz]");

  
  TYPE_PARSER_THROW_TEST_ALL("typeof foo.bar['baz']", yatsc::SyntaxError);
  TYPE_PARSER_THROW_TEST_ALL("typeof foo.['bar'].baz", yatsc::SyntaxError);


  // Type Literal

  TYPE_PARSER_TEST_ALL("{foo:string; bar:number}",
                       "[ObjectTypeExprView]\n"
                       "  [PropertySignatureView]\n"
                       "    [NameView][foo]\n"
                       "    [SimpleTypeExprView]\n"
                       "      [NameView][string]\n"
                       "  [PropertySignatureView]\n"
                       "    [NameView][bar]\n"
                       "    [SimpleTypeExprView]\n"
                       "      [NameView][number]");


  TYPE_PARSER_TEST_ALL("{foo() => string; bar:number}",
                       "[ObjectTypeExprView]\n"
                       "  [MethodSignatureView]\n"
                       "    [NameView][foo]\n"
                       "    [CallSignatureView]\n"
                       "      [ParamList]\n"
                       "      [SimpleTypeExprView]\n"
                       "        [NameView][string]\n"
                       "      [Empty]\n"
                       "  [PropertySignatureView]\n"
                       "    [NameView][bar]\n"
                       "    [SimpleTypeExprView]\n"
                       "      [NameView][number]");

  TYPE_PARSER_TEST_ALL("{foo:string; bar() => number}",
                       "[ObjectTypeExprView]\n"
                       "  [PropertySignatureView]\n"
                       "    [NameView][foo]\n"
                       "    [SimpleTypeExprView]\n"
                       "      [NameView][string]\n"
                       "  [MethodSignatureView]\n"
                       "    [NameView][bar]\n"
                       "    [CallSignatureView]\n"
                       "      [ParamList]\n"
                       "      [SimpleTypeExprView]\n"
                       "        [NameView][number]\n"
                       "      [Empty]");

  TYPE_PARSER_TEST_ALL("{() => string; bar<T>() => number}",
                       "[ObjectTypeExprView]\n"
                       "  [CallSignatureView]\n"
                       "    [ParamList]\n"
                       "    [SimpleTypeExprView]\n"
                       "      [NameView][string]\n"
                       "    [Empty]\n"
                       "  [MethodSignatureView]\n"
                       "    [NameView][bar]\n"
                       "    [CallSignatureView]\n"
                       "      [ParamList]\n"
                       "      [SimpleTypeExprView]\n"
                       "        [NameView][number]\n"
                       "      [TypeParametersView]\n"
                       "        [NameView][T]");
}
