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


#include "../gtest-header.h"
#include "../parser-util.h"


#define EXPR_TEST_ALL(code, expected_str)                             \
  []{PARSER_TEST("anonymous", ParseExpression(), yatsc::LanguageMode::ES3, code, expected_str, false); }(); \
  []{PARSER_TEST("anonymous", ParseExpression(), yatsc::LanguageMode::ES5_STRICT, code, expected_str, false);}(); \
  []{PARSER_TEST("anonymous", ParseExpression(), yatsc::LanguageMode::ES6, code, expected_str, false)}()

#define EXPR_TEST(type, code, expected_str)                             \
  PARSER_TEST("anonymous", ParseExpression(), type, code, expected_str, false)

#define EXPR_THROW_TEST(type, code)                                     \
  PARSER_TEST("anonymous", ParseExpression(), type, code, "", true)


TEST(ExpressionParseTest, ParseLiteral_string) {
  EXPR_TEST_ALL("'aaaaaaa'", "[StringView]['aaaaaaa']");
}


TEST(ExpressionParseTest, ParseLiteral_numeric) {
  EXPR_TEST_ALL("12345", "[NumberView][12345]");
}


TEST(ExpressionParseTest, ParseLiteral_boolean) {
  EXPR_TEST_ALL("true", "[TrueView]");
  EXPR_TEST_ALL("false", "[FalseView]");
}


TEST(ExpressionParseTest, ParseLiteral_undefined) {
  EXPR_TEST_ALL("undefined", "[UndefinedView]");
}


TEST(ExpressionParseTest, ParseLiteral_null) {
  EXPR_TEST_ALL("null", "[NullView]");
}


TEST(ExpressionParseTest, ParseLiteral_NaN) {
  EXPR_TEST_ALL("NaN", "[NaNView]");
}


TEST(ExpressionParseTest, ParsePrimaryExpression_this) {
  EXPR_TEST_ALL("this", "[ThisView]");
}


TEST(ExpressionParseTest, ParsePrimaryExpression_identifier) {
  EXPR_TEST_ALL("Identifier", "[NameView][Identifier]");
}


TEST(ExpressionParseTest, ParseExpression_array) {
  EXPR_TEST(yatsc::LanguageMode::ES3, "[1,2,3,4]",
            "[ArrayLiteralView]\n"
            "  [NumberView][1]\n"
            "  [NumberView][2]\n"
            "  [NumberView][3]\n"
            "  [NumberView][4]");
}


TEST(ExpressionParseTest, ParseExpression_complexArray) {
  EXPR_TEST(yatsc::LanguageMode::ES3, "[new Opcode(1, function(x: foo.bar, y: foo.bar2) { return x + y}), 2]",
            "[ArrayLiteralView]\n"
            "  [NewCallView]\n"
            "    [NameView][Opcode]\n"
            "    [ArgumentsView]\n"
            "      [Empty]\n"
            "      [CallArgsView]\n"
            "        [NumberView][1]\n"
            "        [FunctionView]\n"
            "          [Empty]\n"
            "          [Empty]\n"
            "          [CallSignatureView]\n"
            "            [ParamList]\n"
            "              [ParameterView]\n"
            "                [NameView][x]\n"
            "                [Empty]\n"
            "                [SimpleTypeExprView]\n"
            "                  [GetPropView]\n"
            "                    [NameView][foo]\n"
            "                    [NameView][bar]\n"
            "                [Empty]\n"
            "              [ParameterView]\n"
            "                [NameView][y]\n"
            "                [Empty]\n"
            "                [SimpleTypeExprView]\n"
            "                  [GetPropView]\n"
            "                    [NameView][foo]\n"
            "                    [NameView][bar2]\n"
            "                [Empty]\n"
            "            [Empty]\n"
            "            [Empty]\n"
            "          [BlockView]\n"
            "            [ReturnStatementView]\n"
            "              [BinaryExprView][Plus]\n"
            "                [NameView][x]\n"
            "                [NameView][y]\n"
            "  [NumberView][2]");
}


TEST(ExpressionParseTest, ParseExpression_object) {
  EXPR_TEST(yatsc::LanguageMode::ES3, "({})", "[ObjectLiteralView]");
  EXPR_TEST(yatsc::LanguageMode::ES3, "({a:200,b:300,c:400})",
            "[ObjectLiteralView]\n"
            "  [ObjectElementView]\n"
            "    [NameView][a]\n"
            "    [NumberView][200]\n"
            "  [ObjectElementView]\n"
            "    [NameView][b]\n"
            "    [NumberView][300]\n"
            "  [ObjectElementView]\n"
            "    [NameView][c]\n"
            "    [NumberView][400]");
  

  EXPR_TEST(yatsc::LanguageMode::ES3, "({a:{b:{c:{d:{e:{f:100}}}}}})",
            "[ObjectLiteralView]\n"
            "  [ObjectElementView]\n"
            "    [NameView][a]\n"
            "    [ObjectLiteralView]\n"
            "      [ObjectElementView]\n"
            "        [NameView][b]\n"
            "        [ObjectLiteralView]\n"
            "          [ObjectElementView]\n"
            "            [NameView][c]\n"
            "            [ObjectLiteralView]\n"
            "              [ObjectElementView]\n"
            "                [NameView][d]\n"
            "                [ObjectLiteralView]\n"
            "                  [ObjectElementView]\n"
            "                    [NameView][e]\n"
            "                    [ObjectLiteralView]\n"
            "                      [ObjectElementView]\n"
            "                        [NameView][f]\n"
            "                        [NumberView][100]");
}


TEST(ExpressionParseTest, ParseExpression_array_comprehension) {
  EXPR_TEST(yatsc::LanguageMode::ES6, "[for (x of m) x]",
            "[ArrayLiteralView]\n"
            "  [ComprehensionExprView]\n"
            "    [ForOfStatementView]\n"
            "      [NameView][x]\n"
            "      [NameView][m]\n"
            "      [Empty]\n"
            "    [NameView][x]");
  
  EXPR_TEST(yatsc::LanguageMode::ES6, "[for (x of m) if (x % 2 === 0) x]",
            "[ArrayLiteralView]\n"
            "  [ComprehensionExprView]\n"
            "    [ForOfStatementView]\n"
            "      [NameView][x]\n"
            "      [NameView][m]\n"
            "      [Empty]\n"
            "    [IfStatementView]\n"
            "      [BinaryExprView][Eq]\n"
            "        [BinaryExprView][Mod]\n"
            "          [NameView][x]\n"
            "          [NumberView][2]\n"
            "        [NumberView][0]\n"
            "      [NameView][x]\n"
            "      [Empty]");
}


TEST(ExpressionParseTest, ParseGeneratorComprehension) {
  EXPR_TEST(yatsc::LanguageMode::ES6, "(for (x of m) x)",
            "[ComprehensionExprView]\n"
            "  [ForOfStatementView]\n"
            "    [NameView][x]\n"
            "    [NameView][m]\n"
            "    [Empty]\n"
            "  [NameView][x]");

  EXPR_TEST(yatsc::LanguageMode::ES6, "(for (x of m) if (x % 2 === 0) x)",
            "[ComprehensionExprView]\n"
            "  [ForOfStatementView]\n"
            "    [NameView][x]\n"
            "    [NameView][m]\n"
            "    [Empty]\n"
            "  [IfStatementView]\n"
            "    [BinaryExprView][Eq]\n"
            "      [BinaryExprView][Mod]\n"
            "        [NameView][x]\n"
            "        [NumberView][2]\n"
            "      [NumberView][0]\n"
            "    [NameView][x]\n"
            "    [Empty]");
}


TEST(ExpressionParseTest, ParseExpression_call) {
  EXPR_TEST(yatsc::LanguageMode::ES3, "func()",
            "[CallView]\n"
            "  [NameView][func]\n"
            "  [ArgumentsView]\n"
            "    [Empty]\n"
            "    [CallArgsView]");
}


TEST(ExpressionParseTest, ParseExpression_call_elem) {
  EXPR_TEST(yatsc::LanguageMode::ES3, "func()[index]",
            "[GetElemView]\n"
            "  [CallView]\n"
            "    [NameView][func]\n"
            "    [ArgumentsView]\n"
            "      [Empty]\n"
            "      [CallArgsView]\n"
            "  [NameView][index]");
}


TEST(ExpressionParseTest, ParseExpression_property_call) {
  EXPR_TEST(yatsc::LanguageMode::ES3, "foo.bar.null.func()",
            "[CallView]\n"
            "  [GetPropView]\n"
            "    [GetPropView]\n"
            "      [GetPropView]\n"
            "        [NameView][foo]\n"
            "        [NameView][bar]\n"
            "      [NameView][null]\n"
            "    [NameView][func]\n"
            "  [ArgumentsView]\n"
            "    [Empty]\n"
            "    [CallArgsView]");
}


TEST(ExpressionParseTest, ParseExpression_property_call_call) {
  EXPR_TEST(yatsc::LanguageMode::ES3, "foo.func().foo.baz()()",
            "[CallView]\n"
            "  [CallView]\n"
            "    [GetPropView]\n"
            "      [GetPropView]\n"
            "        [CallView]\n"
            "          [GetPropView]\n"
            "            [NameView][foo]\n"
            "            [NameView][func]\n"
            "          [ArgumentsView]\n"
            "            [Empty]\n"
            "            [CallArgsView]\n"
            "        [NameView][foo]\n"
            "      [NameView][baz]\n"
            "    [ArgumentsView]\n"
            "      [Empty]\n"
            "      [CallArgsView]\n"
            "  [ArgumentsView]\n"
            "    [Empty]\n"
            "    [CallArgsView]");
}


TEST(ExpressionParseTest, ParseExpression_property_elem_call) {
  EXPR_TEST(yatsc::LanguageMode::ES3, "foo['bar'].baz()",
            "[CallView]\n"
            "  [GetPropView]\n"
            "    [GetElemView]\n"
            "      [NameView][foo]\n"
            "      [StringView]['bar']\n"
            "    [NameView][baz]\n"
            "  [ArgumentsView]\n"
            "    [Empty]\n"
            "    [CallArgsView]");
}


TEST(ExpressionParseTest, ParseExpression_property_call2) {
  EXPR_TEST(yatsc::LanguageMode::ES3, "process.stdout.write(str + '\\n')",
            "[CallView]\n"
            "  [GetPropView]\n"
            "    [GetPropView]\n"
            "      [NameView][process]\n"
            "      [NameView][stdout]\n"
            "    [NameView][write]\n"
            "  [ArgumentsView]\n"
            "    [Empty]\n"
            "    [CallArgsView]\n"
            "      [BinaryExprView][Plus]\n"
            "        [NameView][str]\n"
            "        [StringView]['\\n']");
}


TEST(ExpressionParseTest, ParseExpression_property_call3) {
  EXPR_TEST(yatsc::LanguageMode::ES3, "fs.readFile(path.resolve(dir, 'META-INF/MANIFEST.MF'), (err, data) => {})",
            "[CallView]\n"
            "  [GetPropView]\n"
            "    [NameView][fs]\n"
            "    [NameView][readFile]\n"
            "  [ArgumentsView]\n"
            "    [Empty]\n"
            "    [CallArgsView]\n"
            "      [CallView]\n"
            "        [GetPropView]\n"
            "          [NameView][path]\n"
            "          [NameView][resolve]\n"
            "        [ArgumentsView]\n"
            "          [Empty]\n"
            "          [CallArgsView]\n"
            "            [NameView][dir]\n"
            "            [StringView]['META-INF/MANIFEST.MF']\n"
            "      [ArrowFunctionView]\n"
            "        [CallSignatureView]\n"
            "          [ParamList]\n"
            "            [ParameterView]\n"
            "              [NameView][err]\n"
            "              [Empty]\n"
            "              [Empty]\n"
            "              [Empty]\n"
            "            [ParameterView]\n"
            "              [NameView][data]\n"
            "              [Empty]\n"
            "              [Empty]\n"
            "              [Empty]\n"
            "          [Empty]\n"
            "          [Empty]\n"
            "        [BlockView]");
}


TEST(ExpressionParseTest, ParseExpression_generic_call) {
  EXPR_TEST(yatsc::LanguageMode::ES3, "func<string>()",
            "[CallView]\n"
            "  [NameView][func]\n"
            "  [ArgumentsView]\n"
            "    [TypeArgumentsView]\n"
            "      [SimpleTypeExprView]\n"
            "        [NameView][string]\n"
            "    [CallArgsView]");
}


TEST(ExpressionParseTest, ParseExpression_generic_property_call) {
  EXPR_TEST(yatsc::LanguageMode::ES3, "foo.func().func<string>()",
            "[CallView]\n"
            "  [GetPropView]\n"
            "    [CallView]\n"
            "      [GetPropView]\n"
            "        [NameView][foo]\n"
            "        [NameView][func]\n"
            "      [ArgumentsView]\n"
            "        [Empty]\n"
            "        [CallArgsView]\n"
            "    [NameView][func]\n"
            "  [ArgumentsView]\n"
            "    [TypeArgumentsView]\n"
            "      [SimpleTypeExprView]\n"
            "        [NameView][string]\n"
            "    [CallArgsView]");
}

TEST(ExpressionParseTest, ParseArrowFunctionExprInExpr) {
  EXPR_TEST(yatsc::LanguageMode::ES3, "((a:string,b:number): number => a + b)",
            "[ArrowFunctionView]\n"
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
            "    [SimpleTypeExprView]\n"
            "      [NameView][number]\n"
            "    [Empty]\n"
            "  [BinaryExprView][Plus]\n"
            "    [NameView][a]\n"
            "    [NameView][b]");
}


TEST(ExpressionParseTest, ParseExpression_arrow_function) {
  EXPR_TEST(yatsc::LanguageMode::ES3, "(a:string,b:number) => a + b",
            "[ArrowFunctionView]\n"
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
            "    [Empty]\n"
            "    [Empty]\n"
            "  [BinaryExprView][Plus]\n"
            "    [NameView][a]\n"
            "    [NameView][b]");


  EXPR_TEST(yatsc::LanguageMode::ES3, "(a:string = \"aaa\") => a + b",
            "[ArrowFunctionView]\n"
            "  [CallSignatureView]\n"
            "    [ParamList]\n"
            "      [ParameterView]\n"
            "        [NameView][a]\n"
            "        [StringView]['aaa']\n"
            "        [SimpleTypeExprView]\n"
            "          [NameView][string]\n"
            "        [Empty]\n"
            "    [Empty]\n"
            "    [Empty]\n"
            "  [BinaryExprView][Plus]\n"
            "    [NameView][a]\n"
            "    [NameView][b]");

  EXPR_TEST(yatsc::LanguageMode::ES3, "(a?:string = foo()) => a + b",
            "[ArrowFunctionView]\n"
            "  [CallSignatureView]\n"
            "    [ParamList]\n"
            "      [ParameterView]\n"
            "        [NameView][a]\n"
            "        [CallView]\n"
            "          [NameView][foo]\n"
            "          [ArgumentsView]\n"
            "            [Empty]\n"
            "            [CallArgsView]\n"
            "        [SimpleTypeExprView]\n"
            "          [NameView][string]\n"
            "        [Empty]\n"
            "    [Empty]\n"
            "    [Empty]\n"
            "  [BinaryExprView][Plus]\n"
            "    [NameView][a]\n"
            "    [NameView][b]");


  EXPR_TEST(yatsc::LanguageMode::ES3, "(a:string, ...b) => a + b",
            "[ArrowFunctionView]\n"
            "  [CallSignatureView]\n"
            "    [ParamList]\n"
            "      [ParameterView]\n"
            "        [NameView][a]\n"
            "        [Empty]\n"
            "        [SimpleTypeExprView]\n"
            "          [NameView][string]\n"
            "        [Empty]\n"
            "      [RestParamView]\n"
            "        [ParameterView]\n"
            "          [NameView][b]\n"
            "          [Empty]\n"
            "          [Empty]\n"
            "          [Empty]\n"
            "    [Empty]\n"
            "    [Empty]\n"
            "  [BinaryExprView][Plus]\n"
            "    [NameView][a]\n"
            "    [NameView][b]");

  EXPR_TEST(yatsc::LanguageMode::ES3, "(...b) => a + b",
            "[ArrowFunctionView]\n"
            "  [CallSignatureView]\n"
            "    [ParamList]\n"
            "      [RestParamView]\n"
            "        [ParameterView]\n"
            "          [NameView][b]\n"
            "          [Empty]\n"
            "          [Empty]\n"
            "          [Empty]\n"
            "    [Empty]\n"
            "    [Empty]\n"
            "  [BinaryExprView][Plus]\n"
            "    [NameView][a]\n"
            "    [NameView][b]");
  
  EXPR_THROW_TEST(yatsc::LanguageMode::ES3, "(...b, a) => a + b");

  EXPR_THROW_TEST(yatsc::LanguageMode::ES3, "(...b = 1) => a + b");

  EXPR_THROW_TEST(yatsc::LanguageMode::ES3, "(...b?: string) => a + b");
  

  EXPR_TEST(yatsc::LanguageMode::ES3, "(a:string, ...b:number) => a + b",
            "[ArrowFunctionView]\n"
            "  [CallSignatureView]\n"
            "    [ParamList]\n"
            "      [ParameterView]\n"
            "        [NameView][a]\n"
            "        [Empty]\n"
            "        [SimpleTypeExprView]\n"
            "          [NameView][string]\n"
            "        [Empty]\n"
            "      [RestParamView]\n"
            "        [ParameterView]\n"
            "          [NameView][b]\n"
            "          [Empty]\n"
            "          [SimpleTypeExprView]\n"
            "            [NameView][number]\n"
            "          [Empty]\n"
            "    [Empty]\n"
            "    [Empty]\n"
            "  [BinaryExprView][Plus]\n"
            "    [NameView][a]\n"
            "    [NameView][b]");
  

  EXPR_TEST(yatsc::LanguageMode::ES3, "x => x + 10",
            "[ArrowFunctionView]\n"
            "  [CallSignatureView]\n"
            "    [NameView][x]\n"
            "    [Empty]\n"
            "    [Empty]\n"
            "  [BinaryExprView][Plus]\n"
            "    [NameView][x]\n"
            "    [NumberView][10]");
}


TEST(ExpressionParseTest, ParseExpression_generic_arrow_function) {
  EXPR_TEST(yatsc::LanguageMode::ES3, "<T, U>(a:T, b:U) => a + b",
            "[ArrowFunctionView]\n"
            "  [CallSignatureView]\n"
            "    [ParamList]\n"
            "      [ParameterView]\n"
            "        [NameView][a]\n"
            "        [Empty]\n"
            "        [SimpleTypeExprView]\n"
            "          [NameView][T]\n"
            "        [Empty]\n"
            "      [ParameterView]\n"
            "        [NameView][b]\n"
            "        [Empty]\n"
            "        [SimpleTypeExprView]\n"
            "          [NameView][U]\n"
            "        [Empty]\n"
            "    [Empty]\n"
            "    [TypeParametersView]\n"
            "      [NameView][T]\n"
            "      [NameView][U]\n"
            "  [BinaryExprView][Plus]\n"
            "    [NameView][a]\n"
            "    [NameView][b]");
}


TEST(ExpressionParseTest, ParseExpression_constraints_generic_arrow_function) {
  EXPR_TEST(yatsc::LanguageMode::ES3, "<T, U extends V>(a:T, b:U) => a + b",
            "[ArrowFunctionView]\n"
            "  [CallSignatureView]\n"
            "    [ParamList]\n"
            "      [ParameterView]\n"
            "        [NameView][a]\n"
            "        [Empty]\n"
            "        [SimpleTypeExprView]\n"
            "          [NameView][T]\n"
            "        [Empty]\n"
            "      [ParameterView]\n"
            "        [NameView][b]\n"
            "        [Empty]\n"
            "        [SimpleTypeExprView]\n"
            "          [NameView][U]\n"
            "        [Empty]\n"
            "    [Empty]\n"
            "    [TypeParametersView]\n"
            "      [NameView][T]\n"
            "      [TypeConstraintsView]\n"
            "        [NameView][U]\n"
            "        [SimpleTypeExprView]\n"
            "          [NameView][V]\n"
            "  [BinaryExprView][Plus]\n"
            "    [NameView][a]\n"
            "    [NameView][b]");
}


TEST(ExpressionParseTest, ParseExpression_generic_instance_arrow_function) {
  EXPR_TEST(yatsc::LanguageMode::ES3, "(a:string, foo:Foo<string, number>) => a + foo.value",
            "[ArrowFunctionView]\n"
            "  [CallSignatureView]\n"
            "    [ParamList]\n"
            "      [ParameterView]\n"
            "        [NameView][a]\n"
            "        [Empty]\n"
            "        [SimpleTypeExprView]\n"
            "          [NameView][string]\n"
            "        [Empty]\n"
            "      [ParameterView]\n"
            "        [NameView][foo]\n"
            "        [Empty]\n"
            "        [GenericTypeExprView]\n"
            "          [NameView][Foo]\n"
            "          [TypeArgumentsView]\n"
            "            [SimpleTypeExprView]\n"
            "              [NameView][string]\n"
            "            [SimpleTypeExprView]\n"
            "              [NameView][number]\n"
            "        [Empty]\n"
            "    [Empty]\n"
            "    [Empty]\n"
            "  [BinaryExprView][Plus]\n"
            "    [NameView][a]\n"
            "    [GetPropView]\n"
            "      [NameView][foo]\n"
            "      [NameView][value]");
}


TEST(ExpressionParseTest, ParseExpression_parenthesized_expression) {
  EXPR_TEST(yatsc::LanguageMode::ES3, "(100,200,300,b)",
            "[CommaExprView]\n"
            "  [NumberView][100]\n"
            "  [NumberView][200]\n"
            "  [NumberView][300]\n"
            "  [NameView][b]");
}


TEST(ExpressionParseTest, ParseExpression_parenthesized_expression_like_arrow_function) {
  EXPR_TEST(yatsc::LanguageMode::ES3, "(a, b, c) => a + b",
            "[ArrowFunctionView]\n"
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
            "  [BinaryExprView][Plus]\n"
            "    [NameView][a]\n"
            "    [NameView][b]");
}


TEST(ExpressionParseTest, ParseExpression_function_type) {
  EXPR_TEST(yatsc::LanguageMode::ES3, "(param1:(string, number, Object) => string, param2):string => param1 + param2",
            "[ArrowFunctionView]\n"
            "  [CallSignatureView]\n"
            "    [ParamList]\n"
            "      [ParameterView]\n"
            "        [NameView][param1]\n"
            "        [Empty]\n"
            "        [FunctionTypeExprView]\n"
            "          [ParamList]\n"
            "            [ParameterView]\n"
            "              [NameView][string]\n"
            "              [Empty]\n"
            "              [Empty]\n"
            "              [Empty]\n"
            "            [ParameterView]\n"
            "              [NameView][number]\n"
            "              [Empty]\n"
            "              [Empty]\n"
            "              [Empty]\n"
            "            [ParameterView]\n"
            "              [NameView][Object]\n"
            "              [Empty]\n"
            "              [Empty]\n"
            "              [Empty]\n"
            "          [SimpleTypeExprView]\n"
            "            [NameView][string]\n"
            "        [Empty]\n"
            "      [ParameterView]\n"
            "        [NameView][param2]\n"
            "        [Empty]\n"
            "        [Empty]\n"
            "        [Empty]\n"
            "    [SimpleTypeExprView]\n"
            "      [NameView][string]\n"
            "    [Empty]\n"
            "  [BinaryExprView][Plus]\n"
            "    [NameView][param1]\n"
            "    [NameView][param2]");
}


TEST(ExpressionParseTest, ParseExpression_regexp) {
  EXPR_TEST(yatsc::LanguageMode::ES3, "x = /a/",
            "[AssignmentView][Assign]\n"
            "  [NameView][x]\n"
            "  [RegularExprView][/a/]");

  EXPR_TEST(yatsc::LanguageMode::ES3, "x = /**//a/",
            "[AssignmentView][Assign]\n"
            "  [NameView][x]\n"
            "  [RegularExprView][/a/]");

  EXPR_TEST(yatsc::LanguageMode::ES3, "x = //\n/a/",
            "[AssignmentView][Assign]\n"
            "  [NameView][x]\n"
            "  [RegularExprView][/a/]");

  EXPR_TEST(yatsc::LanguageMode::ES3, "x = /\\/abc\\/ddd\\/ee/",
            "[AssignmentView][Assign]\n"
            "  [NameView][x]\n"
            "  [RegularExprView][/\\/abc\\/ddd\\/ee/]");

  EXPR_TEST(yatsc::LanguageMode::ES3, "v = /^(\\/\\/\\/\\s*<reference\\s+path\\s*=\\s*)('|\")(.+?)\\2.*?\\/>/",
            "[AssignmentView][Assign]\n"
            "  [NameView][v]\n"
            "  [RegularExprView][/^(\\/\\/\\/\\s*<reference\\s+path\\s*=\\s*)('|\")(.+?)\\2.*?\\/>/]");
  
  
  EXPR_TEST(yatsc::LanguageMode::ES3, "m(/a/)",
            "[CallView]\n"
            "  [NameView][m]\n"
            "  [ArgumentsView]\n"
            "    [Empty]\n"
            "    [CallArgsView]\n"
            "      [RegularExprView][/a/]");
}



TEST(ExpressionParseTest, ParseExpression_assignment_expr) {
  EXPR_TEST(yatsc::LanguageMode::ES3, "x = 100",
            "[AssignmentView][Assign]\n"
            "  [NameView][x]\n"
            "  [NumberView][100]");


  EXPR_TEST(yatsc::LanguageMode::ES3, "x = y = z = 100",
            "[AssignmentView][Assign]\n"
            "  [NameView][x]\n"
            "  [AssignmentView][Assign]\n"
            "    [NameView][y]\n"
            "    [AssignmentView][Assign]\n"
            "      [NameView][z]\n"
            "      [NumberView][100]");


  EXPR_TEST(yatsc::LanguageMode::ES3, "x *= 100",
            "[AssignmentView][MulLet]\n"
            "  [NameView][x]\n"
            "  [NumberView][100]");


  EXPR_TEST(yatsc::LanguageMode::ES3, "x /= 100",
            "[AssignmentView][DivLet]\n"
            "  [NameView][x]\n"
            "  [NumberView][100]");


  EXPR_TEST(yatsc::LanguageMode::ES3, "x %= 100",
            "[AssignmentView][ModLet]\n"
            "  [NameView][x]\n"
            "  [NumberView][100]");


  EXPR_TEST(yatsc::LanguageMode::ES3, "x += 100",
            "[AssignmentView][AddLet]\n"
            "  [NameView][x]\n"
            "  [NumberView][100]");


  EXPR_TEST(yatsc::LanguageMode::ES3, "x -= 100",
            "[AssignmentView][SubLet]\n"
            "  [NameView][x]\n"
            "  [NumberView][100]");


  EXPR_TEST(yatsc::LanguageMode::ES3, "x <<= 100",
            "[AssignmentView][ShiftLeftLet]\n"
            "  [NameView][x]\n"
            "  [NumberView][100]");


  EXPR_TEST(yatsc::LanguageMode::ES3, "x >>= 100",
            "[AssignmentView][ShiftRightLet]\n"
            "  [NameView][x]\n"
            "  [NumberView][100]");


  EXPR_TEST(yatsc::LanguageMode::ES3, "x >>>= 100",
            "[AssignmentView][UShiftRightLet]\n"
            "  [NameView][x]\n"
            "  [NumberView][100]");


  EXPR_TEST(yatsc::LanguageMode::ES3, "x &= 100",
            "[AssignmentView][AndLet]\n"
            "  [NameView][x]\n"
            "  [NumberView][100]");


  EXPR_TEST(yatsc::LanguageMode::ES3, "x ^= 100",
            "[AssignmentView][XorLet]\n"
            "  [NameView][x]\n"
            "  [NumberView][100]");


  EXPR_TEST(yatsc::LanguageMode::ES3, "x |= 100",
            "[AssignmentView][OrLet]\n"
            "  [NameView][x]\n"
            "  [NumberView][100]");

  EXPR_TEST_ALL("(x + 100) / 3",
                "[BinaryExprView][Div]\n"
                "  [BinaryExprView][Plus]\n"
                "    [NameView][x]\n"
                "    [NumberView][100]\n"
                "  [NumberView][3]");

  EXPR_TEST_ALL("x + 100 / 3",
                "[BinaryExprView][Plus]\n"
                "  [NameView][x]\n"
                "  [BinaryExprView][Div]\n"
                "    [NumberView][100]\n"
                "    [NumberView][3]");
}


TEST(ExpressionParseTest, ParseExpression_condition_expr) {
  EXPR_TEST(yatsc::LanguageMode::ES3, "x? y: z",
            "[TemaryExprView]\n"
            "  [NameView][x]\n"
            "  [NameView][y]\n"
            "  [NameView][z]");


  EXPR_TEST(yatsc::LanguageMode::ES3, "x === 100? (a,b) => a + b + x: (a,b) => a - b + x / 2",
            "[TemaryExprView]\n"
            "  [BinaryExprView][Eq]\n"
            "    [NameView][x]\n"
            "    [NumberView][100]\n"
            "  [ArrowFunctionView]\n"
            "    [CallSignatureView]\n"
            "      [ParamList]\n"
            "        [ParameterView]\n"
            "          [NameView][a]\n"
            "          [Empty]\n"
            "          [Empty]\n"
            "          [Empty]\n"
            "        [ParameterView]\n"
            "          [NameView][b]\n"
            "          [Empty]\n"
            "          [Empty]\n"
            "          [Empty]\n"
            "      [Empty]\n"
            "      [Empty]\n"
            "    [BinaryExprView][Plus]\n"
            "      [BinaryExprView][Plus]\n"
            "        [NameView][a]\n"
            "        [NameView][b]\n"
            "      [NameView][x]\n"
            "  [ArrowFunctionView]\n"
            "    [CallSignatureView]\n"
            "      [ParamList]\n"
            "        [ParameterView]\n"
            "          [NameView][a]\n"
            "          [Empty]\n"
            "          [Empty]\n"
            "          [Empty]\n"
            "        [ParameterView]\n"
            "          [NameView][b]\n"
            "          [Empty]\n"
            "          [Empty]\n"
            "          [Empty]\n"
            "      [Empty]\n"
            "      [Empty]\n"
            "    [BinaryExprView][Plus]\n"
            "      [BinaryExprView][Minus]\n"
            "        [NameView][a]\n"
            "        [NameView][b]\n"
            "      [BinaryExprView][Div]\n"
            "        [NameView][x]\n"
            "        [NumberView][2]");
}


TEST(ExpressionParseTest, ParseExpressionConditional2) {
  EXPR_TEST_ALL("components.length ? (quote ? quoteStr(components[components.length - 1]) : components[components.length - 1]) : modPath;",
                "[TemaryExprView]\n"
                "  [GetPropView]\n"
                "    [NameView][components]\n"
                "    [NameView][length]\n"
                "  [TemaryExprView]\n"
                "    [NameView][quote]\n"
                "    [CallView]\n"
                "      [NameView][quoteStr]\n"
                "      [ArgumentsView]\n"
                "        [Empty]\n"
                "        [CallArgsView]\n"
                "          [GetElemView]\n"
                "            [NameView][components]\n"
                "            [BinaryExprView][Minus]\n"
                "              [GetPropView]\n"
                "                [NameView][components]\n"
                "                [NameView][length]\n"
                "              [NumberView][1]\n"
                "    [GetElemView]\n"
                "      [NameView][components]\n"
                "      [BinaryExprView][Minus]\n"
                "        [GetPropView]\n"
                "          [NameView][components]\n"
                "          [NameView][length]\n"
                "        [NumberView][1]\n"
                "  [NameView][modPath]");
}


TEST(ExpressionParseTest, ParseExpression_binary_expr) {
  EXPR_TEST_ALL("1 + 1",
                "[BinaryExprView][Plus]\n"
                "  [NumberView][1]\n"
                "  [NumberView][1]");
  
  EXPR_TEST_ALL("2 + (3 - 1)",
                "[BinaryExprView][Plus]\n"
                "  [NumberView][2]\n"
                "  [BinaryExprView][Minus]\n"
                "    [NumberView][3]\n"
                "    [NumberView][1]");


  EXPR_TEST_ALL("1 > 1",
                "[BinaryExprView][Greater]\n"
                "  [NumberView][1]\n"
                "  [NumberView][1]");


  EXPR_TEST_ALL("1 < 1",
                "[BinaryExprView][Less]\n"
                "  [NumberView][1]\n"
                "  [NumberView][1]");
}


TEST(ExpressionParseTest, ParseExpression_unary_expr) {
  EXPR_TEST(yatsc::LanguageMode::ES3, "++i",
            "[UnaryExprView][Increment]\n"
            "  [NameView][i]");

  EXPR_TEST(yatsc::LanguageMode::ES3, "--i",
            "[UnaryExprView][Decrement]\n"
            "  [NameView][i]");

  EXPR_TEST(yatsc::LanguageMode::ES3, "delete i",
            "[UnaryExprView][Delete]\n"
            "  [NameView][i]");

  EXPR_TEST(yatsc::LanguageMode::ES3, "void i",
            "[UnaryExprView][Void]\n"
            "  [NameView][i]");

  EXPR_TEST(yatsc::LanguageMode::ES3, "typeof i",
            "[UnaryExprView][Typeof]\n"
            "  [NameView][i]");

  EXPR_TEST(yatsc::LanguageMode::ES3, "+i",
            "[UnaryExprView][Plus]\n"
            "  [NameView][i]");

  EXPR_TEST(yatsc::LanguageMode::ES3, "-i",
            "[UnaryExprView][Minus]\n"
            "  [NameView][i]");

  EXPR_TEST(yatsc::LanguageMode::ES3, "~i",
            "[UnaryExprView][BitNor]\n"
            "  [NameView][i]");

  EXPR_TEST(yatsc::LanguageMode::ES3, "!i",
            "[UnaryExprView][Not]\n"
            "  [NameView][i]");
}


TEST(ExpressionParseTest, ParseExpression_postfix_expr) {
  EXPR_TEST(yatsc::LanguageMode::ES3, "i++",
            "[PostfixView][Increment]\n"
            "  [NameView][i]");

  EXPR_TEST(yatsc::LanguageMode::ES3, "i--",
            "[PostfixView][Decrement]\n"
            "  [NameView][i]");
}


TEST(ExpressionParseTest, ParseTypeAssertions) {
  EXPR_TEST(yatsc::LanguageMode::ES3, "((<InterfaceType>type).typeParameters ? (<InterfaceType>type).typeParameters.length : 0) !== arity",
            "[BinaryExprView][NotEq]\n"
            "  [TemaryExprView]\n"
            "    [GetPropView]\n"
            "      [CastView]\n"
            "        [TypeArgumentsView]\n"
            "          [SimpleTypeExprView]\n"
            "            [NameView][InterfaceType]\n"
            "        [NameView][type]\n"
            "      [NameView][typeParameters]\n"
            "    [GetPropView]\n"
            "      [GetPropView]\n"
            "        [CastView]\n"
            "          [TypeArgumentsView]\n"
            "            [SimpleTypeExprView]\n"
            "              [NameView][InterfaceType]\n"
            "          [NameView][type]\n"
            "        [NameView][typeParameters]\n"
            "      [NameView][length]\n"
            "    [NumberView][0]\n"
            "  [NameView][arity]");
}


TEST(ExpressionParseTest, ParseTypeAssertions2) {
  EXPR_TEST(yatsc::LanguageMode::ES3, "(<Identifier>(<ExpressionStatement>node).expression).text",
            "[GetPropView]\n"
            "  [GetPropView]\n"
            "    [CastView]\n"
            "      [TypeArgumentsView]\n"
            "        [SimpleTypeExprView]\n"
            "          [NameView][Identifier]\n"
            "      [CastView]\n"
            "        [TypeArgumentsView]\n"
            "          [SimpleTypeExprView]\n"
            "            [NameView][ExpressionStatement]\n"
            "        [NameView][node]\n"
            "    [NameView][expression]\n"
            "  [NameView][text]");
}

