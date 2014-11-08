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


#define DECLARATION_TEST(type, code, expected_str)                      \
  PARSER_TEST("anonymous", ParseDeclaration(true, true, false), type, code, expected_str, false)

#define DECLARATION_THROW_TEST(type, code)                              \
  PARSER_TEST("anonymous", ParseDeclaration(true, true, false), type, code, "", true)

#define DECLARATION_TEST_ALL(code, expected_str)                        \
  [&]{DECLARATION_TEST(yatsc::LanguageMode::ES3, code, expected_str);}(); \
  [&]{DECLARATION_TEST(yatsc::LanguageMode::ES5_STRICT, code, expected_str);}(); \
  [&]{DECLARATION_TEST(yatsc::LanguageMode::ES3, code, expected_str);}()

#define DECLARATION_THROW_TEST_ALL(code)                                \
  [&]{DECLARATION_THROW_TEST(yatsc::LanguageMode::ES3, code);}();       \
  [&]{DECLARATION_THROW_TEST(yatsc::LanguageMode::ES5_STRICT, code);}(); \
  [&]{DECLARATION_THROW_TEST(yatsc::LanguageMode::ES6, code);}()


TEST(DeclarationParseTest, ParseLexicalDeclaration_let) {
  DECLARATION_TEST(yatsc::LanguageMode::ES6, "let x = 100;",
                   "[LexicalDeclView][TS_LET]\n"
                   "  [VariableView]\n"
                   "    [NameView][x]\n"
                   "    [NumberView][100]\n"
                   "    [Empty]");

  DECLARATION_TEST(yatsc::LanguageMode::ES6, "let x = 100, y = 200, z = 300",
                   "[LexicalDeclView][TS_LET]\n"
                   "  [VariableView]\n"
                   "    [NameView][x]\n"
                   "    [NumberView][100]\n"
                   "    [Empty]\n"
                   "  [VariableView]\n"
                   "    [NameView][y]\n"
                   "    [NumberView][200]\n"
                   "    [Empty]\n"
                   "  [VariableView]\n"
                   "    [NameView][z]\n"
                   "    [NumberView][300]\n"
                   "    [Empty]");

  DECLARATION_TEST(yatsc::LanguageMode::ES6, "let x: string = 100;",
                   "[LexicalDeclView][TS_LET]\n"
                   "  [VariableView]\n"
                   "    [NameView][x]\n"
                   "    [NumberView][100]\n"
                   "    [SimpleTypeExprView]\n"
                   "      [NameView][string]");

  DECLARATION_TEST(yatsc::LanguageMode::ES6, "let [a, b, c] = [1,2,3];",
                   "[LexicalDeclView][TS_LET]\n"
                   "  [VariableView]\n"
                   "    [BindingArrayView]\n"
                   "      [BindingElementView]\n"
                   "        [Empty]\n"
                   "        [NameView][a]\n"
                   "        [Empty]\n"
                   "      [BindingElementView]\n"
                   "        [Empty]\n"
                   "        [NameView][b]\n"
                   "        [Empty]\n"
                   "      [BindingElementView]\n"
                   "        [Empty]\n"
                   "        [NameView][c]\n"
                   "        [Empty]\n"
                   "    [ArrayLiteralView]\n"
                   "      [NumberView][1]\n"
                   "      [NumberView][2]\n"
                   "      [NumberView][3]\n"
                   "    [Empty]");

  DECLARATION_TEST(yatsc::LanguageMode::ES6, "let {a,b,c} = {a:100,b:200,c:300};",
                   "[LexicalDeclView][TS_LET]\n"
                   "  [VariableView]\n"
                   "    [BindingPropListView]\n"
                   "      [BindingElementView]\n"
                   "        [NameView][a]\n"
                   "        [Empty]\n"
                   "        [Empty]\n"
                   "      [BindingElementView]\n"
                   "        [NameView][b]\n"
                   "        [Empty]\n"
                   "        [Empty]\n"
                   "      [BindingElementView]\n"
                   "        [NameView][c]\n"
                   "        [Empty]\n"
                   "        [Empty]\n"
                   "    [ObjectLiteralView]\n"
                   "      [ObjectElementView]\n"
                   "        [NameView][a]\n"
                   "        [NumberView][100]\n"
                   "      [ObjectElementView]\n"
                   "        [NameView][b]\n"
                   "        [NumberView][200]\n"
                   "      [ObjectElementView]\n"
                   "        [NameView][c]\n"
                   "        [NumberView][300]\n"
                   "    [Empty]");

  DECLARATION_TEST(yatsc::LanguageMode::ES6, "let {foo:a,b:{c:[x,y,z]},c:[{bar}]} = {a:100,b:{c:[1,2,3]},c:[{bar:100}]};",
                   "[LexicalDeclView][TS_LET]\n"
                   "  [VariableView]\n"
                   "    [BindingPropListView]\n"
                   "      [BindingElementView]\n"
                   "        [NameView][foo]\n"
                   "        [NameView][a]\n"
                   "        [Empty]\n"
                   "      [BindingElementView]\n"
                   "        [NameView][b]\n"
                   "        [BindingPropListView]\n"
                   "          [BindingElementView]\n"
                   "            [NameView][c]\n"
                   "            [BindingArrayView]\n"
                   "              [BindingElementView]\n"
                   "                [Empty]\n"
                   "                [NameView][x]\n"
                   "                [Empty]\n"
                   "              [BindingElementView]\n"
                   "                [Empty]\n"
                   "                [NameView][y]\n"
                   "                [Empty]\n"
                   "              [BindingElementView]\n"
                   "                [Empty]\n"
                   "                [NameView][z]\n"
                   "                [Empty]\n"
                   "            [Empty]\n"
                   "        [Empty]\n"
                   "      [BindingElementView]\n"
                   "        [NameView][c]\n"
                   "        [BindingArrayView]\n"
                   "          [BindingElementView]\n"
                   "            [Empty]\n"
                   "            [BindingPropListView]\n"
                   "              [BindingElementView]\n"
                   "                [NameView][bar]\n"
                   "                [Empty]\n"
                   "                [Empty]\n"
                   "            [Empty]\n"
                   "        [Empty]\n"
                   "    [ObjectLiteralView]\n"
                   "      [ObjectElementView]\n"
                   "        [NameView][a]\n"
                   "        [NumberView][100]\n"
                   "      [ObjectElementView]\n"
                   "        [NameView][b]\n"
                   "        [ObjectLiteralView]\n"
                   "          [ObjectElementView]\n"
                   "            [NameView][c]\n"
                   "            [ArrayLiteralView]\n"
                   "              [NumberView][1]\n"
                   "              [NumberView][2]\n"
                   "              [NumberView][3]\n"
                   "      [ObjectElementView]\n"
                   "        [NameView][c]\n"
                   "        [ArrayLiteralView]\n"
                   "          [ObjectLiteralView]\n"
                   "            [ObjectElementView]\n"
                   "              [NameView][bar]\n"
                   "              [NumberView][100]\n"
                   "    [Empty]");


  DECLARATION_TEST(yatsc::LanguageMode::ES6, "let [a, b, c]: number[] = [1,2,3];",
                   "[LexicalDeclView][TS_LET]\n"
                   "  [VariableView]\n"
                   "    [BindingArrayView]\n"
                   "      [BindingElementView]\n"
                   "        [Empty]\n"
                   "        [NameView][a]\n"
                   "        [Empty]\n"
                   "      [BindingElementView]\n"
                   "        [Empty]\n"
                   "        [NameView][b]\n"
                   "        [Empty]\n"
                   "      [BindingElementView]\n"
                   "        [Empty]\n"
                   "        [NameView][c]\n"
                   "        [Empty]\n"
                   "    [ArrayLiteralView]\n"
                   "      [NumberView][1]\n"
                   "      [NumberView][2]\n"
                   "      [NumberView][3]\n"
                   "    [ArrayTypeExprView]\n"
                   "      [SimpleTypeExprView]\n"
                   "        [NameView][number]");

  DECLARATION_TEST(yatsc::LanguageMode::ES6, "let {a,b,c}: Object = {a:100,b:200,c:300};",
                   "[LexicalDeclView][TS_LET]\n"
                   "  [VariableView]\n"
                   "    [BindingPropListView]\n"
                   "      [BindingElementView]\n"
                   "        [NameView][a]\n"
                   "        [Empty]\n"
                   "        [Empty]\n"
                   "      [BindingElementView]\n"
                   "        [NameView][b]\n"
                   "        [Empty]\n"
                   "        [Empty]\n"
                   "      [BindingElementView]\n"
                   "        [NameView][c]\n"
                   "        [Empty]\n"
                   "        [Empty]\n"
                   "    [ObjectLiteralView]\n"
                   "      [ObjectElementView]\n"
                   "        [NameView][a]\n"
                   "        [NumberView][100]\n"
                   "      [ObjectElementView]\n"
                   "        [NameView][b]\n"
                   "        [NumberView][200]\n"
                   "      [ObjectElementView]\n"
                   "        [NameView][c]\n"
                   "        [NumberView][300]\n"
                   "    [SimpleTypeExprView]\n"
                   "      [NameView][Object]");
}


TEST(DeclarationParseTest, ParseLexicalDeclaration_const) {
  DECLARATION_TEST(yatsc::LanguageMode::ES6, "const x = 100;",
                   "[LexicalDeclView][TS_CONST]\n"
                   "  [VariableView]\n"
                   "    [NameView][x]\n"
                   "    [NumberView][100]\n"
                   "    [Empty]");
}


TEST(DeclarationParseTest, ParseFunctionOverloads) {
  DECLARATION_TEST_ALL("function a() {}",
                       "[FunctionView]\n"
                       "  [FunctionOverloadsView]\n"
                       "  [NameView][a]\n"
                       "  [CallSignatureView]\n"
                       "    [ParamList]\n"
                       "    [Empty]\n"
                       "    [Empty]\n"
                       "  [BlockView]");

  DECLARATION_TEST_ALL("function a();function a();function a() {}",
                       "[FunctionView]\n"
                       "  [FunctionOverloadsView]\n"
                       "    [FunctionOverloadView]\n"
                       "      [NameView][a]\n"
                       "      [CallSignatureView]\n"
                       "        [ParamList]\n"
                       "        [Empty]\n"
                       "        [Empty]\n"
                       "    [FunctionOverloadView]\n"
                       "      [NameView][a]\n"
                       "      [CallSignatureView]\n"
                       "        [ParamList]\n"
                       "        [Empty]\n"
                       "        [Empty]\n"
                       "  [NameView][a]\n"
                       "  [CallSignatureView]\n"
                       "    [ParamList]\n"
                       "    [Empty]\n"
                       "    [Empty]\n"
                       "  [BlockView]");

  DECLARATION_THROW_TEST_ALL("function *a();function a();function a() {}");
  DECLARATION_THROW_TEST_ALL("function a();function b();function b() {}");

  DECLARATION_TEST_ALL("function a(a,b,c) {}",
                       "[FunctionView]\n"
                       "  [FunctionOverloadsView]\n"
                       "  [NameView][a]\n"
                       "  [CallSignatureView]\n"
                       "    [ParamList]\n"
                       "      [ParameterView]\n"
                       "        [NameView][a]\n"
                       "        [Empty]\n"
                       "        [Empty]\n"
                       "        [Empty]\n"
                       "      [ParameterView]\n"
                       "        [NameView][b]\n"
                       "        [Empty]\n"
                       "        [Empty]\n"
                       "        [Empty]\n"
                       "      [ParameterView]\n"
                       "        [NameView][c]\n"
                       "        [Empty]\n"
                       "        [Empty]\n"
                       "        [Empty]\n"
                       "    [Empty]\n"
                       "    [Empty]\n"
                       "  [BlockView]");

  DECLARATION_TEST_ALL("function a(a:string,b:number,...c:string[]): void {}",
                       "[FunctionView]\n"
                       "  [FunctionOverloadsView]\n"
                       "  [NameView][a]\n"
                       "  [CallSignatureView]\n"
                       "    [ParamList]\n"
                       "      [ParameterView]\n"
                       "        [NameView][a]\n"
                       "        [Empty]\n"
                       "        [SimpleTypeExprView]\n"
                       "          [NameView][string]\n"
                       "        [Empty]\n"
                       "      [ParameterView]\n"
                       "        [NameView][b]\n"
                       "        [Empty]\n"
                       "        [SimpleTypeExprView]\n"
                       "          [NameView][number]\n"
                       "        [Empty]\n"
                       "      [RestParamView]\n"
                       "        [ParameterView]\n"
                       "          [NameView][c]\n"
                       "          [Empty]\n"
                       "          [ArrayTypeExprView]\n"
                       "            [SimpleTypeExprView]\n"
                       "              [NameView][string]\n"
                       "          [Empty]\n"
                       "    [SimpleTypeExprView]\n"
                       "      [NameView][void]\n"
                       "    [Empty]\n"
                       "  [BlockView]");

  DECLARATION_TEST_ALL("function *a(a:string,b:number,...c:string[]): void {}",
                       "[FunctionView]\n"
                       "  [FunctionOverloadsView]\n"
                       "  [NameView][a]\n"
                       "  [CallSignatureView]\n"
                       "    [ParamList]\n"
                       "      [ParameterView]\n"
                       "        [NameView][a]\n"
                       "        [Empty]\n"
                       "        [SimpleTypeExprView]\n"
                       "          [NameView][string]\n"
                       "        [Empty]\n"
                       "      [ParameterView]\n"
                       "        [NameView][b]\n"
                       "        [Empty]\n"
                       "        [SimpleTypeExprView]\n"
                       "          [NameView][number]\n"
                       "        [Empty]\n"
                       "      [RestParamView]\n"
                       "        [ParameterView]\n"
                       "          [NameView][c]\n"
                       "          [Empty]\n"
                       "          [ArrayTypeExprView]\n"
                       "            [SimpleTypeExprView]\n"
                       "              [NameView][string]\n"
                       "          [Empty]\n"
                       "    [SimpleTypeExprView]\n"
                       "      [NameView][void]\n"
                       "    [Empty]\n"
                       "  [BlockView]");


  DECLARATION_THROW_TEST_ALL("function (a:string,b:number,...c:string[]): void {}");
}


TEST(DeclarationParseTest, ParseClassDeclaration) {
  DECLARATION_TEST_ALL("class Foo {}",
                       "[ClassDeclView]\n"
                       "  [NameView][Foo]\n"
                       "  [Empty]\n"
                       "  [ClassBasesView]\n"
                       "    [Empty]\n"
                       "    [Empty]\n"
                       "  [ClassFieldListView]");


  DECLARATION_TEST_ALL("class Foo {constructor(){}}",
                       "[ClassDeclView]\n"
                       "  [NameView][Foo]\n"
                       "  [Empty]\n"
                       "  [ClassBasesView]\n"
                       "    [Empty]\n"
                       "    [Empty]\n"
                       "  [ClassFieldListView]\n"
                       "    [MemberFunctionView]\n"
                       "      [ClassFieldModifiersView]\n"
                       "        [ClassFieldAccessLevelView][TS_PUBLIC]\n"
                       "      [NameView][constructor]\n"
                       "      [CallSignatureView]\n"
                       "        [ParamList]\n"
                       "        [Empty]\n"
                       "        [Empty]\n"
                       "      [MemberFunctionOverloadsView]\n"
                       "      [BlockView]");


  DECLARATION_TEST_ALL("class Foo {"
                       "  constructor();"
                       "  constructor(a);"
                       "  constructor(b){}"
                       "}",
                       "[ClassDeclView]\n"
                       "  [NameView][Foo]\n"
                       "  [Empty]\n"
                       "  [ClassBasesView]\n"
                       "    [Empty]\n"
                       "    [Empty]\n"
                       "  [ClassFieldListView]\n"
                       "    [MemberFunctionView]\n"
                       "      [ClassFieldModifiersView]\n"
                       "        [ClassFieldAccessLevelView][TS_PUBLIC]\n"
                       "      [NameView][constructor]\n"
                       "      [CallSignatureView]\n"
                       "        [ParamList]\n"
                       "          [ParameterView]\n"
                       "            [NameView][b]\n"
                       "            [Empty]\n"
                       "            [Empty]\n"
                       "            [Empty]\n"
                       "        [Empty]\n"
                       "        [Empty]\n"
                       "      [MemberFunctionOverloadsView]\n"
                       "        [MemberFunctionOverloadView]\n"
                       "          [ClassFieldModifiersView]\n"
                       "            [ClassFieldAccessLevelView][TS_PUBLIC]\n"
                       "          [NameView][constructor]\n"
                       "          [CallSignatureView]\n"
                       "            [ParamList]\n"
                       "            [Empty]\n"
                       "            [Empty]\n"
                       "        [MemberFunctionOverloadView]\n"
                       "          [ClassFieldModifiersView]\n"
                       "            [ClassFieldAccessLevelView][TS_PUBLIC]\n"
                       "          [NameView][constructor]\n"
                       "          [CallSignatureView]\n"
                       "            [ParamList]\n"
                       "              [ParameterView]\n"
                       "                [NameView][a]\n"
                       "                [Empty]\n"
                       "                [Empty]\n"
                       "                [Empty]\n"
                       "            [Empty]\n"
                       "            [Empty]\n"
                       "      [BlockView]");

  DECLARATION_TEST_ALL("class Foo {"
                       "  constructor();"
                       "  constructor(a);"
                       "  constructor(b){}"
                       "  public member(){}"
                       "}",
                       "[ClassDeclView]\n"
                       "  [NameView][Foo]\n"
                       "  [Empty]\n"
                       "  [ClassBasesView]\n"
                       "    [Empty]\n"
                       "    [Empty]\n"
                       "  [ClassFieldListView]\n"
                       "    [MemberFunctionView]\n"
                       "      [ClassFieldModifiersView]\n"
                       "        [ClassFieldAccessLevelView][TS_PUBLIC]\n"
                       "      [NameView][constructor]\n"
                       "      [CallSignatureView]\n"
                       "        [ParamList]\n"
                       "          [ParameterView]\n"
                       "            [NameView][b]\n"
                       "            [Empty]\n"
                       "            [Empty]\n"
                       "            [Empty]\n"
                       "        [Empty]\n"
                       "        [Empty]\n"
                       "      [MemberFunctionOverloadsView]\n"
                       "        [MemberFunctionOverloadView]\n"
                       "          [ClassFieldModifiersView]\n"
                       "            [ClassFieldAccessLevelView][TS_PUBLIC]\n"
                       "          [NameView][constructor]\n"
                       "          [CallSignatureView]\n"
                       "            [ParamList]\n"
                       "            [Empty]\n"
                       "            [Empty]\n"
                       "        [MemberFunctionOverloadView]\n"
                       "          [ClassFieldModifiersView]\n"
                       "            [ClassFieldAccessLevelView][TS_PUBLIC]\n"
                       "          [NameView][constructor]\n"
                       "          [CallSignatureView]\n"
                       "            [ParamList]\n"
                       "              [ParameterView]\n"
                       "                [NameView][a]\n"
                       "                [Empty]\n"
                       "                [Empty]\n"
                       "                [Empty]\n"
                       "            [Empty]\n"
                       "            [Empty]\n"
                       "      [BlockView]\n"
                       "    [MemberFunctionView]\n"
                       "      [ClassFieldModifiersView]\n"
                       "        [ClassFieldAccessLevelView][TS_PUBLIC]\n"
                       "      [NameView][member]\n"
                       "      [CallSignatureView]\n"
                       "        [ParamList]\n"
                       "        [Empty]\n"
                       "        [Empty]\n"
                       "      [MemberFunctionOverloadsView]\n"
                       "      [BlockView]");


  DECLARATION_TEST_ALL("class Foo {"
                       "  constructor();"
                       "  constructor(a);"
                       "  constructor(b){}"
                       "  public static member(a);"
                       "  static public member(b);"
                       "  public static member() {}"
                       "  public static *gmember(a);"
                       "  public static *gmember(b, c);"
                       "  public static *gmember() {};"
                       "}",
                       "[ClassDeclView]\n"
                       "  [NameView][Foo]\n"
                       "  [Empty]\n"
                       "  [ClassBasesView]\n"
                       "    [Empty]\n"
                       "    [Empty]\n"
                       "  [ClassFieldListView]\n"
                       "    [MemberFunctionView]\n"
                       "      [ClassFieldModifiersView]\n"
                       "        [ClassFieldAccessLevelView][TS_PUBLIC]\n"
                       "      [NameView][constructor]\n"
                       "      [CallSignatureView]\n"
                       "        [ParamList]\n"
                       "          [ParameterView]\n"
                       "            [NameView][b]\n"
                       "            [Empty]\n"
                       "            [Empty]\n"
                       "            [Empty]\n"
                       "        [Empty]\n"
                       "        [Empty]\n"
                       "      [MemberFunctionOverloadsView]\n"
                       "        [MemberFunctionOverloadView]\n"
                       "          [ClassFieldModifiersView]\n"
                       "            [ClassFieldAccessLevelView][TS_PUBLIC]\n"
                       "          [NameView][constructor]\n"
                       "          [CallSignatureView]\n"
                       "            [ParamList]\n"
                       "            [Empty]\n"
                       "            [Empty]\n"
                       "        [MemberFunctionOverloadView]\n"
                       "          [ClassFieldModifiersView]\n"
                       "            [ClassFieldAccessLevelView][TS_PUBLIC]\n"
                       "          [NameView][constructor]\n"
                       "          [CallSignatureView]\n"
                       "            [ParamList]\n"
                       "              [ParameterView]\n"
                       "                [NameView][a]\n"
                       "                [Empty]\n"
                       "                [Empty]\n"
                       "                [Empty]\n"
                       "            [Empty]\n"
                       "            [Empty]\n"
                       "      [BlockView]\n"
                       "    [MemberFunctionView]\n"
                       "      [ClassFieldModifiersView]\n"
                       "        [ClassFieldAccessLevelView][TS_STATIC]\n"
                       "        [ClassFieldAccessLevelView][TS_PUBLIC]\n"
                       "      [NameView][member]\n"
                       "      [CallSignatureView]\n"
                       "        [ParamList]\n"
                       "        [Empty]\n"
                       "        [Empty]\n"
                       "      [MemberFunctionOverloadsView]\n"
                       "        [MemberFunctionOverloadView]\n"
                       "          [ClassFieldModifiersView]\n"
                       "            [ClassFieldAccessLevelView][TS_STATIC]\n"
                       "            [ClassFieldAccessLevelView][TS_PUBLIC]\n"
                       "          [NameView][member]\n"
                       "          [CallSignatureView]\n"
                       "            [ParamList]\n"
                       "              [ParameterView]\n"
                       "                [NameView][a]\n"
                       "                [Empty]\n"
                       "                [Empty]\n"
                       "                [Empty]\n"
                       "            [Empty]\n"
                       "            [Empty]\n"
                       "        [MemberFunctionOverloadView]\n"
                       "          [ClassFieldModifiersView]\n"
                       "            [ClassFieldAccessLevelView][TS_STATIC]\n"
                       "            [ClassFieldAccessLevelView][TS_PUBLIC]\n"
                       "          [NameView][member]\n"
                       "          [CallSignatureView]\n"
                       "            [ParamList]\n"
                       "              [ParameterView]\n"
                       "                [NameView][b]\n"
                       "                [Empty]\n"
                       "                [Empty]\n"
                       "                [Empty]\n"
                       "            [Empty]\n"
                       "            [Empty]\n"
                       "      [BlockView]\n"
                       "    [MemberFunctionView]\n"
                       "      [ClassFieldModifiersView]\n"
                       "        [ClassFieldAccessLevelView][TS_STATIC]\n"
                       "        [ClassFieldAccessLevelView][TS_PUBLIC]\n"
                       "      [NameView][gmember]\n"
                       "      [CallSignatureView]\n"
                       "        [ParamList]\n"
                       "        [Empty]\n"
                       "        [Empty]\n"
                       "      [MemberFunctionOverloadsView]\n"
                       "        [MemberFunctionOverloadView]\n"
                       "          [ClassFieldModifiersView]\n"
                       "            [ClassFieldAccessLevelView][TS_STATIC]\n"
                       "            [ClassFieldAccessLevelView][TS_PUBLIC]\n"
                       "          [NameView][gmember]\n"
                       "          [CallSignatureView]\n"
                       "            [ParamList]\n"
                       "              [ParameterView]\n"
                       "                [NameView][a]\n"
                       "                [Empty]\n"
                       "                [Empty]\n"
                       "                [Empty]\n"
                       "            [Empty]\n"
                       "            [Empty]\n"
                       "        [MemberFunctionOverloadView]\n"
                       "          [ClassFieldModifiersView]\n"
                       "            [ClassFieldAccessLevelView][TS_STATIC]\n"
                       "            [ClassFieldAccessLevelView][TS_PUBLIC]\n"
                       "          [NameView][gmember]\n"
                       "          [CallSignatureView]\n"
                       "            [ParamList]\n"
                       "              [ParameterView]\n"
                       "                [NameView][b]\n"
                       "                [Empty]\n"
                       "                [Empty]\n"
                       "                [Empty]\n"
                       "              [ParameterView]\n"
                       "                [NameView][c]\n"
                       "                [Empty]\n"
                       "                [Empty]\n"
                       "                [Empty]\n"
                       "            [Empty]\n"
                       "            [Empty]\n"
                       "      [BlockView]");


  DECLARATION_TEST_ALL("class Foo {public member = 1;}",
                       "[ClassDeclView]\n"
                       "  [NameView][Foo]\n"
                       "  [Empty]\n"
                       "  [ClassBasesView]\n"
                       "    [Empty]\n"
                       "    [Empty]\n"
                       "  [ClassFieldListView]\n"
                       "    [MemberVariableView]\n"
                       "      [ClassFieldModifiersView]\n"
                       "        [ClassFieldAccessLevelView][TS_PUBLIC]\n"
                       "      [NameView][member]\n"
                       "      [Empty]\n"
                       "      [NumberView][1]");


  DECLARATION_TEST_ALL("class Foo {private static member = 1;}",
                       "[ClassDeclView]\n"
                       "  [NameView][Foo]\n"
                       "  [Empty]\n"
                       "  [ClassBasesView]\n"
                       "    [Empty]\n"
                       "    [Empty]\n"
                       "  [ClassFieldListView]\n"
                       "    [MemberVariableView]\n"
                       "      [ClassFieldModifiersView]\n"
                       "        [ClassFieldAccessLevelView][TS_STATIC]\n"
                       "        [ClassFieldAccessLevelView][TS_PRIVATE]\n"
                       "      [NameView][member]\n"
                       "      [Empty]\n"
                       "      [NumberView][1]");


  DECLARATION_TEST_ALL("class Foo extends Bar {}",
                       "[ClassDeclView]\n"
                       "  [NameView][Foo]\n"
                       "  [Empty]\n"
                       "  [ClassBasesView]\n"
                       "    [ClassHeritageView]\n"
                       "      [SimpleTypeExprView]\n"
                       "        [NameView][Bar]\n"
                       "    [Empty]\n"
                       "  [ClassFieldListView]");


  DECLARATION_TEST_ALL("class Foo implements Bar {}",
                       "[ClassDeclView]\n"
                       "  [NameView][Foo]\n"
                       "  [Empty]\n"
                       "  [ClassBasesView]\n"
                       "    [Empty]\n"
                       "    [ClassImplsView]\n"
                       "      [SimpleTypeExprView]\n"
                       "        [NameView][Bar]\n"
                       "  [ClassFieldListView]");


  DECLARATION_TEST_ALL("class Foo implements Bar implements Buz {}",
                       "[ClassDeclView]\n"
                       "  [NameView][Foo]\n"
                       "  [Empty]\n"
                       "  [ClassBasesView]\n"
                       "    [Empty]\n"
                       "    [ClassImplsView]\n"
                       "      [SimpleTypeExprView]\n"
                       "        [NameView][Bar]\n"
                       "      [SimpleTypeExprView]\n"
                       "        [NameView][Buz]\n"
                       "  [ClassFieldListView]");


  DECLARATION_TEST_ALL("class Foo extends Bar implements Buz implements Qux {}",
                       "[ClassDeclView]\n"
                       "  [NameView][Foo]\n"
                       "  [Empty]\n"
                       "  [ClassBasesView]\n"
                       "    [ClassHeritageView]\n"
                       "      [SimpleTypeExprView]\n"
                       "        [NameView][Bar]\n"
                       "    [ClassImplsView]\n"
                       "      [SimpleTypeExprView]\n"
                       "        [NameView][Buz]\n"
                       "      [SimpleTypeExprView]\n"
                       "        [NameView][Qux]\n"
                       "  [ClassFieldListView]");


  DECLARATION_TEST_ALL("class Foo {"
                       "  public get bar(){}"
                       "}",
                       "[ClassDeclView]\n"
                       "  [NameView][Foo]\n"
                       "  [Empty]\n"
                       "  [ClassBasesView]\n"
                       "    [Empty]\n"
                       "    [Empty]\n"
                       "  [ClassFieldListView]\n"
                       "    [MemberFunctionView]\n"
                       "      [ClassFieldModifiersView]\n"
                       "        [ClassFieldAccessLevelView][TS_PUBLIC]\n"
                       "      [NameView][bar]\n"
                       "      [CallSignatureView]\n"
                       "        [ParamList]\n"
                       "        [Empty]\n"
                       "        [Empty]\n"
                       "      [MemberFunctionOverloadsView]\n"
                       "      [BlockView]");

  DECLARATION_TEST_ALL("class Foo {"
                       "  public set bar(x): void{}"
                       "}",
                       "[ClassDeclView]\n"
                       "  [NameView][Foo]\n"
                       "  [Empty]\n"
                       "  [ClassBasesView]\n"
                       "    [Empty]\n"
                       "    [Empty]\n"
                       "  [ClassFieldListView]\n"
                       "    [MemberFunctionView]\n"
                       "      [ClassFieldModifiersView]\n"
                       "        [ClassFieldAccessLevelView][TS_PUBLIC]\n"
                       "      [NameView][bar]\n"
                       "      [CallSignatureView]\n"
                       "        [ParamList]\n"
                       "          [ParameterView]\n"
                       "            [NameView][x]\n"
                       "            [Empty]\n"
                       "            [Empty]\n"
                       "            [Empty]\n"
                       "        [SimpleTypeExprView]\n"
                       "          [NameView][void]\n"
                       "        [Empty]\n"
                       "      [MemberFunctionOverloadsView]\n"
                       "      [BlockView]");


  DECLARATION_THROW_TEST_ALL("class Foo {"
                             "  public get bar(x): void{}"
                             "}");

  DECLARATION_THROW_TEST_ALL("class Foo {"
                             "  public get bar(): void{}"
                             "}");

  DECLARATION_THROW_TEST_ALL("class Foo {"
                             "  public get bar(): null{}"
                             "}");

  DECLARATION_THROW_TEST_ALL("class Foo {"
                             "  public set bar() {}"
                             "}");

  DECLARATION_THROW_TEST_ALL("class Foo {"
                             "  public set bar(): string {}"
                             "}");


  DECLARATION_TEST_ALL("class Foo<T> {}",
                       "[ClassDeclView]\n"
                       "  [NameView][Foo]\n"
                       "  [TypeParametersView]\n"
                       "    [NameView][T]\n"
                       "  [ClassBasesView]\n"
                       "    [Empty]\n"
                       "    [Empty]\n"
                       "  [ClassFieldListView]");


  DECLARATION_TEST_ALL("class Foo<T, U, V> {}",
                       "[ClassDeclView]\n"
                       "  [NameView][Foo]\n"
                       "  [TypeParametersView]\n"
                       "    [NameView][T]\n"
                       "    [NameView][U]\n"
                       "    [NameView][V]\n"
                       "  [ClassBasesView]\n"
                       "    [Empty]\n"
                       "    [Empty]\n"
                       "  [ClassFieldListView]");


  DECLARATION_TEST_ALL("class Foo<T extends Bar, U extends Baz> {}",
                       "[ClassDeclView]\n"
                       "  [NameView][Foo]\n"
                       "  [TypeParametersView]\n"
                       "    [TypeConstraintsView]\n"
                       "      [NameView][T]\n"
                       "      [NameView][Bar]\n"
                       "    [TypeConstraintsView]\n"
                       "      [NameView][U]\n"
                       "      [NameView][Baz]\n"
                       "  [ClassBasesView]\n"
                       "    [Empty]\n"
                       "    [Empty]\n"
                       "  [ClassFieldListView]");


  DECLARATION_TEST_ALL("class Foo<T, U> extends Bar<BarT> implements Buz<BuzT> implements Qux<QuxT> {}",
                       "[ClassDeclView]\n"
                       "  [NameView][Foo]\n"
                       "  [TypeParametersView]\n"
                       "    [NameView][T]\n"
                       "    [NameView][U]\n"
                       "  [ClassBasesView]\n"
                       "    [ClassHeritageView]\n"
                       "      [GenericTypeExprView]\n"
                       "        [NameView][Bar]\n"
                       "        [TypeArgumentsView]\n"
                       "          [SimpleTypeExprView]\n"
                       "            [NameView][BarT]\n"
                       "    [ClassImplsView]\n"
                       "      [GenericTypeExprView]\n"
                       "        [NameView][Buz]\n"
                       "        [TypeArgumentsView]\n"
                       "          [SimpleTypeExprView]\n"
                       "            [NameView][BuzT]\n"
                       "      [GenericTypeExprView]\n"
                       "        [NameView][Qux]\n"
                       "        [TypeArgumentsView]\n"
                       "          [SimpleTypeExprView]\n"
                       "            [NameView][QuxT]\n"
                       "  [ClassFieldListView]");


  DECLARATION_TEST_ALL("class Foo {[x:string]:number}",
                       "[ClassDeclView]\n"
                       "  [NameView][Foo]\n"
                       "  [Empty]\n"
                       "  [ClassBasesView]\n"
                       "    [Empty]\n"
                       "    [Empty]\n"
                       "  [ClassFieldListView]\n"
                       "    [IndexSignatureView]\n"
                       "      [NameView][x]\n"
                       "      [SimpleTypeExprView]\n"
                       "        [NameView][number]");
}


TEST(DeclarationParseTest, ParseInterfaceDeclaration) {
  DECLARATION_TEST_ALL("interface Foo {x:string;y:number}",
                       "[InterfaceView]\n"
                       "  [NameView][Foo]\n"
                       "  [Empty]\n"
                       "  [InterfaceExtendsView]\n"
                       "  [ObjectTypeExprView]\n"
                       "    [PropertySignatureView]\n"
                       "      [NameView][x]\n"
                       "      [SimpleTypeExprView]\n"
                       "        [NameView][string]\n"
                       "    [PropertySignatureView]\n"
                       "      [NameView][y]\n"
                       "      [SimpleTypeExprView]\n"
                       "        [NameView][number]");


  DECLARATION_TEST_ALL("interface Foo {x(a,b,c):void}",
                       "[InterfaceView]\n"
                       "  [NameView][Foo]\n"
                       "  [Empty]\n"
                       "  [InterfaceExtendsView]\n"
                       "  [ObjectTypeExprView]\n"
                       "    [MethodSignatureView]\n"
                       "      [NameView][x]\n"
                       "      [CallSignatureView]\n"
                       "        [ParamList]\n"
                       "          [ParameterView]\n"
                       "            [NameView][a]\n"
                       "            [Empty]\n"
                       "            [Empty]\n"
                       "            [Empty]\n"
                       "          [ParameterView]\n"
                       "            [NameView][b]\n"
                       "            [Empty]\n"
                       "            [Empty]\n"
                       "            [Empty]\n"
                       "          [ParameterView]\n"
                       "            [NameView][c]\n"
                       "            [Empty]\n"
                       "            [Empty]\n"
                       "            [Empty]\n"
                       "        [SimpleTypeExprView]\n"
                       "          [NameView][void]\n"
                       "        [Empty]");


  DECLARATION_TEST_ALL("interface Foo {x<T>(a,b,c):void}",
                       "[InterfaceView]\n"
                       "  [NameView][Foo]\n"
                       "  [Empty]\n"
                       "  [InterfaceExtendsView]\n"
                       "  [ObjectTypeExprView]\n"
                       "    [MethodSignatureView]\n"
                       "      [NameView][x]\n"
                       "      [CallSignatureView]\n"
                       "        [ParamList]\n"
                       "          [ParameterView]\n"
                       "            [NameView][a]\n"
                       "            [Empty]\n"
                       "            [Empty]\n"
                       "            [Empty]\n"
                       "          [ParameterView]\n"
                       "            [NameView][b]\n"
                       "            [Empty]\n"
                       "            [Empty]\n"
                       "            [Empty]\n"
                       "          [ParameterView]\n"
                       "            [NameView][c]\n"
                       "            [Empty]\n"
                       "            [Empty]\n"
                       "            [Empty]\n"
                       "        [SimpleTypeExprView]\n"
                       "          [NameView][void]\n"
                       "        [TypeParametersView]\n"
                       "          [NameView][T]");


  DECLARATION_TEST_ALL("interface Foo {[a:string]:number}",
                       "[InterfaceView]\n"
                       "  [NameView][Foo]\n"
                       "  [Empty]\n"
                       "  [InterfaceExtendsView]\n"
                       "  [ObjectTypeExprView]\n"
                       "    [IndexSignatureView]\n"
                       "      [NameView][a]\n"
                       "      [SimpleTypeExprView]\n"
                       "        [NameView][number]");


  DECLARATION_TEST_ALL("interface Foo extends Bar {}",
                       "[InterfaceView]\n"
                       "  [NameView][Foo]\n"
                       "  [Empty]\n"
                       "  [InterfaceExtendsView]\n"
                       "    [SimpleTypeExprView]\n"
                       "      [NameView][Bar]\n"
                       "  [ObjectTypeExprView]");


  DECLARATION_TEST_ALL("interface Foo extends Bar extends Baz {}",
                       "[InterfaceView]\n"
                       "  [NameView][Foo]\n"
                       "  [Empty]\n"
                       "  [InterfaceExtendsView]\n"
                       "    [SimpleTypeExprView]\n"
                       "      [NameView][Bar]\n"
                       "    [SimpleTypeExprView]\n"
                       "      [NameView][Baz]\n"
                       "  [ObjectTypeExprView]");


  DECLARATION_TEST_ALL("interface Foo<X, Y, Z> extends Bar extends Baz {}",
                       "[InterfaceView]\n"
                       "  [NameView][Foo]\n"
                       "  [TypeParametersView]\n"
                       "    [NameView][X]\n"
                       "    [NameView][Y]\n"
                       "    [NameView][Z]\n"
                       "  [InterfaceExtendsView]\n"
                       "    [SimpleTypeExprView]\n"
                       "      [NameView][Bar]\n"
                       "    [SimpleTypeExprView]\n"
                       "      [NameView][Baz]\n"
                       "  [ObjectTypeExprView]");


  DECLARATION_TEST_ALL("interface Foo<X extends Bar, Y extends Baz> extends Bar extends Baz {}",
                       "[InterfaceView]\n"
                       "  [NameView][Foo]\n"
                       "  [TypeParametersView]\n"
                       "    [TypeConstraintsView]\n"
                       "      [NameView][X]\n"
                       "      [NameView][Bar]\n"
                       "    [TypeConstraintsView]\n"
                       "      [NameView][Y]\n"
                       "      [NameView][Baz]\n"
                       "  [InterfaceExtendsView]\n"
                       "    [SimpleTypeExprView]\n"
                       "      [NameView][Bar]\n"
                       "    [SimpleTypeExprView]\n"
                       "      [NameView][Baz]\n"
                       "  [ObjectTypeExprView]");
};


TEST(DeclarationParseTest, ParseEnumDeclaration) {
  DECLARATION_TEST_ALL("enum Foo{BAR=1}",
                       "[EnumDeclView]\n"
                       "  [NameView][Foo]\n"
                       "  [EnumBodyView]\n"
                       "    [EnumFieldView]\n"
                       "      [NameView][BAR]\n"
                       "      [NumberView][1]");

  
  DECLARATION_TEST_ALL("enum Foo{BAR=1,BAZ,QUX=1 + 2}",
                       "[EnumDeclView]\n"
                       "  [NameView][Foo]\n"
                       "  [EnumBodyView]\n"
                       "    [EnumFieldView]\n"
                       "      [NameView][BAR]\n"
                       "      [NumberView][1]\n"
                       "    [EnumFieldView]\n"
                       "      [NameView][BAZ]\n"
                       "      [Empty]\n"
                       "    [EnumFieldView]\n"
                       "      [NameView][QUX]\n"
                       "      [BinaryExprView][TS_PLUS]\n"
                       "        [NumberView][1]\n"
                       "        [NumberView][2]");

  
  DECLARATION_THROW_TEST_ALL("enum Foo {BAR;BAZ}");
  DECLARATION_THROW_TEST_ALL("enum {}");
}
