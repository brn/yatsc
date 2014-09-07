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


#include <gtest/gtest.h>
#include "../parser-util.h"


#define DECLARATION_TEST(type, code, expected_str)                     \
  PARSER_TEST(ParseDeclaration(true, true, false), type, code, expected_str, false, std::exception)

#define DECLARATION_THROW_TEST(type, code, error_type)                 \
  PARSER_TEST(ParseDeclaration(true, true, false), type, code, "", true, error_type)

#define DECLARATION_TEST_ALL(code, expected_str)                        \
  [&]{DECLARATION_TEST(yatsc::LanguageMode::ES3, code, expected_str);}(); \
  [&]{DECLARATION_TEST(yatsc::LanguageMode::ES5_STRICT, code, expected_str);}(); \
  [&]{DECLARATION_TEST(yatsc::LanguageMode::ES3, code, expected_str);}()

#define DECLARATION_THROW_TEST_ALL(code, error_type)                    \
  [&]{DECLARATION_THROW_TEST(yatsc::LanguageMode::ES3, code, error_type);}(); \
  [&]{DECLARATION_THROW_TEST(yatsc::LanguageMode::ES5_STRICT, code, error_type);}(); \
  [&]{DECLARATION_THROW_TEST(yatsc::LanguageMode::ES6, code, error_type);}()


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

  DECLARATION_THROW_TEST_ALL("function *a();function a();function a() {}", yatsc::SyntaxError);
  DECLARATION_THROW_TEST_ALL("function a();function b();function b() {}", yatsc::SyntaxError);

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


  DECLARATION_THROW_TEST_ALL("function (a:string,b:number,...c:string[]): void {}", yatsc::SyntaxError);
}


TEST(DeclarationParseTest, ParseClassDeclaration) {
  DECLARATION_TEST(yatsc::LanguageMode::ES6, "class Foo {}",
                   "[ClassDeclView]\n"
                   "  [NameView][Foo]\n"
                   "  [ClassFieldListView]\n"
                   "  [ClassBasesView]\n"
                   "    [Empty]\n"
                   "    [Empty]");

  
  DECLARATION_TEST(yatsc::LanguageMode::ES6, "class Foo {constructor(){}}",
                   "[ClassDeclView]\n"
                   "  [NameView][Foo]\n"
                   "  [ClassFieldListView]\n"
                   "    [MemberFunctionView]\n"
                   "      [ClassFieldModifiersView]\n"
                   "      [MemberFunctionOverloadsView]\n"
                   "      [NameView][constructor]\n"
                   "      [CallSignatureView]\n"
                   "        [ParamList]\n"
                   "        [Empty]\n"
                   "        [Empty]\n"
                   "      [BlockView]\n"
                   "  [ClassBasesView]\n"
                   "    [Empty]\n"
                   "    [Empty]");

  
  DECLARATION_TEST(yatsc::LanguageMode::ES6,
                   "class Foo {"
                   "  constructor();"
                   "  constructor(a);"
                   "  constructor(b){}"
                   "}",
                   "[ClassDeclView]\n"
                   "  [NameView][Foo]\n"
                   "  [ClassFieldListView]\n"
                   "    [MemberFunctionView]\n"
                   "      [MemberFunctionOverloadsView]\n"
                   "        [MemberFunctionOverloadView]\n"
                   "          [ClassFieldModifiersView]\n"
                   "          [NameView][constructor]\n"
                   "          [CallSignatureView]\n"
                   "            [ParamList]\n"
                   "            [Empty]\n"
                   "            [Empty]\n"
                   "        [MemberFunctionOverloadView]\n"
                   "          [ClassFieldModifiersView]\n"
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
                   "      [BlockView]\n"
                   "  [ClassBasesView]\n"
                   "    [Empty]\n"
                   "    [Empty]");

  DECLARATION_TEST(yatsc::LanguageMode::ES6,
                   "class Foo {"
                   "  constructor();"
                   "  constructor(a);"
                   "  constructor(b){}"
                   "  public member(){}"
                   "}",
                   "[ClassDeclView]\n"
                   "  [NameView][Foo]\n"
                   "  [ClassFieldListView]\n"
                   "    [MemberFunctionView]\n"
                   "      [MemberFunctionOverloadsView]\n"
                   "        [MemberFunctionOverloadView]\n"
                   "          [ClassFieldModifiersView]\n"
                   "          [NameView][constructor]\n"
                   "          [CallSignatureView]\n"
                   "            [ParamList]\n"
                   "            [Empty]\n"
                   "            [Empty]\n"
                   "        [MemberFunctionOverloadView]\n"
                   "          [ClassFieldModifiersView]\n"
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
                   "      [BlockView]\n"
                   "    [MemberFunctionView]\n"
                   "      [ClassFieldModifiersView]\n"
                   "        [ClassFieldAccessLevelView][TS_PUBLIC]\n"
                   "      [MemberFunctionOverloadsView]\n"
                   "      [NameView][member]\n"
                   "      [CallSignatureView]\n"
                   "        [ParamList]\n"
                   "        [Empty]\n"
                   "        [Empty]\n"
                   "      [BlockView]\n"
                   "  [ClassBasesView]\n"
                   "    [Empty]\n"
                   "    [Empty]");


  DECLARATION_TEST(yatsc::LanguageMode::ES6,
                   "class Foo {"
                   "  constructor();"
                   "  constructor(a);"
                   "  constructor(b){}"
                   "  public member(a);"
                   "  public member(b);"
                   "  public member() {}"
                   "}",
                   "[ClassDeclView]\n"
                   "  [NameView][Foo]\n"
                   "  [ClassFieldListView]\n"
                   "    [MemberFunctionView]\n"
                   "      [MemberFunctionOverloadsView]\n"
                   "        [MemberFunctionOverloadView]\n"
                   "          [ClassFieldModifiersView]\n"
                   "          [NameView][constructor]\n"
                   "          [CallSignatureView]\n"
                   "            [ParamList]\n"
                   "            [Empty]\n"
                   "            [Empty]\n"
                   "        [MemberFunctionOverloadView]\n"
                   "          [ClassFieldModifiersView]\n"
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
                   "      [BlockView]\n"
                   "    [MemberFunctionView]\n"
                   "      [ClassFieldModifiersView]\n"
                   "        [ClassFieldAccessLevelView][TS_PUBLIC]\n"
                   "      [MemberFunctionOverloadsView]\n"
                   "      [NameView][member]\n"
                   "      [CallSignatureView]\n"
                   "        [ParamList]\n"
                   "        [Empty]\n"
                   "        [Empty]\n"
                   "      [BlockView]\n"
                   "  [ClassBasesView]\n"
                   "    [Empty]\n"
                   "    [Empty]");
}
