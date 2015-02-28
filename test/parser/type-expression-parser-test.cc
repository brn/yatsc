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
  PARSER_TEST("anonymous", ParseTypeExpression(), type, code, expected_str, false)

#define TYPE_PARSER_THROW_TEST(type, code)                              \
  PARSER_TEST("anonymous", ParseTypeExpression(), type, code, "", true)

#define TYPE_PARSER_TEST_ALL(code, expected_str)                        \
  [&]{TYPE_PARSER_TEST(yatsc::LanguageMode::ES3, code, expected_str);}(); \
  [&]{TYPE_PARSER_TEST(yatsc::LanguageMode::ES5_STRICT, code, expected_str);}(); \
  [&]{TYPE_PARSER_TEST(yatsc::LanguageMode::ES3, code, expected_str);}()

#define TYPE_PARSER_THROW_TEST_ALL(code)                                \
  [&]{TYPE_PARSER_THROW_TEST(yatsc::LanguageMode::ES3, code);}(); \
  [&]{TYPE_PARSER_THROW_TEST(yatsc::LanguageMode::ES5_STRICT, code);}(); \
  [&]{TYPE_PARSER_THROW_TEST(yatsc::LanguageMode::ES6, code);}()


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

  
  TYPE_PARSER_THROW_TEST_ALL("typeof foo.bar['baz']");
  TYPE_PARSER_THROW_TEST_ALL("typeof foo.['bar'].baz");


  // Union type
  
  TYPE_PARSER_TEST_ALL("T|U|R",
                       "[UnionTypeExprView]\n"
                       "  [SimpleTypeExprView]\n"
                       "    [NameView][T]\n"
                       "  [SimpleTypeExprView]\n"
                       "    [NameView][U]\n"
                       "  [SimpleTypeExprView]\n"
                       "    [NameView][R]");


  TYPE_PARSER_TEST_ALL("T[]|U|R",
                       "[UnionTypeExprView]\n"
                       "  [ArrayTypeExprView]\n"
                       "    [SimpleTypeExprView]\n"
                       "      [NameView][T]\n"
                       "  [SimpleTypeExprView]\n"
                       "    [NameView][U]\n"
                       "  [SimpleTypeExprView]\n"
                       "    [NameView][R]");


  TYPE_PARSER_TEST_ALL("()=>number|{x:number}|R[]",
                       "[FunctionTypeExprView]\n"
                       "  [ParamList]\n"
                       "  [UnionTypeExprView]\n"
                       "    [SimpleTypeExprView]\n"
                       "      [NameView][number]\n"
                       "    [ObjectTypeExprView]\n"
                       "      [PropertySignatureView]\n"
                       "        [NameView][x]\n"
                       "        [SimpleTypeExprView]\n"
                       "          [NameView][number]\n"
                       "    [ArrayTypeExprView]\n"
                       "      [SimpleTypeExprView]\n"
                       "        [NameView][R]");


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


  TYPE_PARSER_TEST_ALL("{foo(): string; bar:number}",
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

  TYPE_PARSER_TEST_ALL("{foo:string; bar(): number}",
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

  TYPE_PARSER_TEST_ALL("{(): string; bar<T>(): number}",
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
