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
  []{PARSER_TEST(ParseExpression(true, false), yatsc::LanguageMode::ES3, code, expected_str, false, std::exception); }(); \
  []{PARSER_TEST(ParseExpression(true, false), yatsc::LanguageMode::ES5_STRICT, code, expected_str, false, std::exception);}(); \
  []{PARSER_TEST(ParseExpression(true, false), yatsc::LanguageMode::ES6, code, expected_str, false, std::exception)}()

#define EXPR_TEST(type, code, expected_str)                             \
  PARSER_TEST(ParseExpression(true, false), type, code, expected_str, false, std::exception)

#define EXPR_THROW_TEST(type, code, error_type)                         \
  PARSER_TEST(ParseExpression(true, false), type, code, "", true, error_type)


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
            "      [BinaryExprView][TS_EQ]\n"
            "        [BinaryExprView][TS_MOD]\n"
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
            "    [BinaryExprView][TS_EQ]\n"
            "      [BinaryExprView][TS_MOD]\n"
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
            "  [CallArgsView]\n"
            "  [Empty]");
}


TEST(ExpressionParseTest, ParseExpression_call_elem) {
  EXPR_TEST(yatsc::LanguageMode::ES3, "func()[index]",
            "[GetElemView]\n"
            "  [CallView]\n"
            "    [NameView][func]\n"
            "    [CallArgsView]\n"
            "    [Empty]\n"
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
            "      [NullView]\n"
            "    [NameView][func]\n"
            "  [CallArgsView]\n"
            "  [Empty]");
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
            "          [CallArgsView]\n"
            "          [Empty]\n"
            "        [NameView][foo]\n"
            "      [NameView][baz]\n"
            "    [CallArgsView]\n"
            "    [Empty]\n"
            "  [CallArgsView]\n"
            "  [Empty]");
}


TEST(ExpressionParseTest, ParseExpression_property_elem_call) {
  EXPR_TEST(yatsc::LanguageMode::ES3, "foo['bar'].baz()",
            "[CallView]\n"
            "  [GetPropView]\n"
            "    [GetElemView]\n"
            "      [NameView][foo]\n"
            "      [StringView]['bar']\n"
            "    [NameView][baz]\n"
            "  [CallArgsView]\n"
            "  [Empty]");
}


TEST(ExpressionParseTest, ParseExpression_property_call2) {
  EXPR_TEST(yatsc::LanguageMode::ES3, "process.stdout.write(str + '\\n')",
            "[CallView]\n"
            "  [GetPropView]\n"
            "    [GetPropView]\n"
            "      [NameView][process]\n"
            "      [NameView][stdout]\n"
            "    [NameView][write]\n"
            "  [CallArgsView]\n"
            "    [BinaryExprView][TS_PLUS]\n"
            "      [NameView][str]\n"
            "      [StringView]['\\n']\n"
            "  [Empty]");
}


TEST(ExpressionParseTest, ParseExpression_generic_call) {
  EXPR_TEST(yatsc::LanguageMode::ES3, "func<string>()",
            "[CallView]\n"
            "  [NameView][func]\n"
            "  [CallArgsView]\n"
            "  [TypeArgumentsView]\n"
            "    [SimpleTypeExprView]\n"
            "      [NameView][string]");
}


TEST(ExpressionParseTest, ParseExpression_generic_property_call) {
  EXPR_TEST(yatsc::LanguageMode::ES3, "foo.func().func<string>()",
            "[CallView]\n"
            "  [GetPropView]\n"
            "    [CallView]\n"
            "      [GetPropView]\n"
            "        [NameView][foo]\n"
            "        [NameView][func]\n"
            "      [CallArgsView]\n"
            "      [Empty]\n"
            "    [NameView][func]\n"
            "  [CallArgsView]\n"
            "  [TypeArgumentsView]\n"
            "    [SimpleTypeExprView]\n"
            "      [NameView][string]");
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
            "  [BinaryExprView][TS_PLUS]\n"
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
            "  [BinaryExprView][TS_PLUS]\n"
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
            "          [CallArgsView]\n"
            "          [Empty]\n"
            "        [SimpleTypeExprView]\n"
            "          [NameView][string]\n"
            "        [Empty]\n"
            "    [Empty]\n"
            "    [Empty]\n"
            "  [BinaryExprView][TS_PLUS]\n"
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
            "  [BinaryExprView][TS_PLUS]\n"
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
            "  [BinaryExprView][TS_PLUS]\n"
            "    [NameView][a]\n"
            "    [NameView][b]");
  
  EXPR_THROW_TEST(yatsc::LanguageMode::ES3, "(...b, a) => a + b",
                  yatsc::ArrowParametersError);

  EXPR_THROW_TEST(yatsc::LanguageMode::ES3, "(...b = 1) => a + b",
                  yatsc::ArrowParametersError);

  EXPR_THROW_TEST(yatsc::LanguageMode::ES3, "(...b?: string) => a + b",
                  yatsc::ArrowParametersError);
  

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
            "  [BinaryExprView][TS_PLUS]\n"
            "    [NameView][a]\n"
            "    [NameView][b]");
  

  EXPR_TEST(yatsc::LanguageMode::ES3, "x => x + 10",
            "[ArrowFunctionView]\n"
            "  [CallSignatureView]\n"
            "    [NameView][x]\n"
            "    [Empty]\n"
            "    [Empty]\n"
            "  [BinaryExprView][TS_PLUS]\n"
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
            "  [BinaryExprView][TS_PLUS]\n"
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
            "        [NameView][V]\n"
            "  [BinaryExprView][TS_PLUS]\n"
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
            "  [BinaryExprView][TS_PLUS]\n"
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
            "  [BinaryExprView][TS_PLUS]\n"
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
            "  [BinaryExprView][TS_PLUS]\n"
            "    [NameView][param1]\n"
            "    [NameView][param2]");
}


TEST(ExpressionParseTest, ParseExpression_regexp) {
  EXPR_TEST(yatsc::LanguageMode::ES3, "x = /a/",
            "[AssignmentView][TS_ASSIGN]\n"
            "  [NameView][x]\n"
            "  [RegularExprView][/a/]");

  EXPR_TEST(yatsc::LanguageMode::ES3, "x = /**//a/",
            "[AssignmentView][TS_ASSIGN]\n"
            "  [NameView][x]\n"
            "  [RegularExprView][/a/]");

  EXPR_TEST(yatsc::LanguageMode::ES3, "x = //\n/a/",
            "[AssignmentView][TS_ASSIGN]\n"
            "  [NameView][x]\n"
            "  [RegularExprView][/a/]");

  EXPR_TEST(yatsc::LanguageMode::ES3, "x = /\\/abc\\/ddd\\/ee/",
            "[AssignmentView][TS_ASSIGN]\n"
            "  [NameView][x]\n"
            "  [RegularExprView][/\\/abc\\/ddd\\/ee/]");

  
  EXPR_TEST(yatsc::LanguageMode::ES3, "m(/a/)",
            "[CallView]\n"
            "  [NameView][m]\n"
            "  [CallArgsView]\n"
            "    [RegularExprView][/a/]\n"
            "  [Empty]");
}



TEST(ExpressionParseTest, ParseExpression_assignment_expr) {
  EXPR_TEST(yatsc::LanguageMode::ES3, "x = 100",
            "[AssignmentView][TS_ASSIGN]\n"
            "  [NameView][x]\n"
            "  [NumberView][100]");


  EXPR_TEST(yatsc::LanguageMode::ES3, "x = y = z = 100",
            "[AssignmentView][TS_ASSIGN]\n"
            "  [NameView][x]\n"
            "  [AssignmentView][TS_ASSIGN]\n"
            "    [NameView][y]\n"
            "    [AssignmentView][TS_ASSIGN]\n"
            "      [NameView][z]\n"
            "      [NumberView][100]");


  EXPR_TEST(yatsc::LanguageMode::ES3, "x *= 100",
            "[AssignmentView][TS_MUL_LET]\n"
            "  [NameView][x]\n"
            "  [NumberView][100]");


  EXPR_TEST(yatsc::LanguageMode::ES3, "x /= 100",
            "[AssignmentView][TS_DIV_LET]\n"
            "  [NameView][x]\n"
            "  [NumberView][100]");


  EXPR_TEST(yatsc::LanguageMode::ES3, "x %= 100",
            "[AssignmentView][TS_MOD_LET]\n"
            "  [NameView][x]\n"
            "  [NumberView][100]");


  EXPR_TEST(yatsc::LanguageMode::ES3, "x += 100",
            "[AssignmentView][TS_ADD_LET]\n"
            "  [NameView][x]\n"
            "  [NumberView][100]");


  EXPR_TEST(yatsc::LanguageMode::ES3, "x -= 100",
            "[AssignmentView][TS_SUB_LET]\n"
            "  [NameView][x]\n"
            "  [NumberView][100]");


  EXPR_TEST(yatsc::LanguageMode::ES3, "x <<= 100",
            "[AssignmentView][TS_SHIFT_LEFT_LET]\n"
            "  [NameView][x]\n"
            "  [NumberView][100]");


  EXPR_TEST(yatsc::LanguageMode::ES3, "x >>= 100",
            "[AssignmentView][TS_SHIFT_RIGHT_LET]\n"
            "  [NameView][x]\n"
            "  [NumberView][100]");


  EXPR_TEST(yatsc::LanguageMode::ES3, "x >>>= 100",
            "[AssignmentView][TS_U_SHIFT_RIGHT_LET]\n"
            "  [NameView][x]\n"
            "  [NumberView][100]");


  EXPR_TEST(yatsc::LanguageMode::ES3, "x &= 100",
            "[AssignmentView][TS_AND_LET]\n"
            "  [NameView][x]\n"
            "  [NumberView][100]");


  EXPR_TEST(yatsc::LanguageMode::ES3, "x ^= 100",
            "[AssignmentView][TS_XOR_LET]\n"
            "  [NameView][x]\n"
            "  [NumberView][100]");


  EXPR_TEST(yatsc::LanguageMode::ES3, "x |= 100",
            "[AssignmentView][TS_OR_LET]\n"
            "  [NameView][x]\n"
            "  [NumberView][100]");

  EXPR_TEST_ALL("(x + 100) / 3",
                "[BinaryExprView][TS_DIV]\n"
                "  [BinaryExprView][TS_PLUS]\n"
                "    [NameView][x]\n"
                "    [NumberView][100]\n"
                "  [NumberView][3]");

  EXPR_TEST_ALL("x + 100 / 3",
                "[BinaryExprView][TS_PLUS]\n"
                "  [NameView][x]\n"
                "  [BinaryExprView][TS_DIV]\n"
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
            "  [BinaryExprView][TS_EQ]\n"
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
            "    [BinaryExprView][TS_PLUS]\n"
            "      [BinaryExprView][TS_PLUS]\n"
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
            "    [BinaryExprView][TS_PLUS]\n"
            "      [BinaryExprView][TS_MINUS]\n"
            "        [NameView][a]\n"
            "        [NameView][b]\n"
            "      [BinaryExprView][TS_DIV]\n"
            "        [NameView][x]\n"
            "        [NumberView][2]");
}


TEST(ExpressionParseTest, ParseExpression_binary_expr) {
  EXPR_TEST_ALL("1 + 1",
                "[BinaryExprView][TS_PLUS]\n"
                "  [NumberView][1]\n"
                "  [NumberView][1]");
  
  EXPR_TEST_ALL("2 + (3 - 1)",
                "[BinaryExprView][TS_PLUS]\n"
                "  [NumberView][2]\n"
                "  [BinaryExprView][TS_MINUS]\n"
                "    [NumberView][3]\n"
                "    [NumberView][1]");
}


TEST(ExpressionParseTest, ParseExpression_unary_expr) {
  EXPR_TEST(yatsc::LanguageMode::ES3, "++i",
            "[UnaryExprView][TS_INCREMENT]\n"
            "  [NameView][i]");

  EXPR_TEST(yatsc::LanguageMode::ES3, "--i",
            "[UnaryExprView][TS_DECREMENT]\n"
            "  [NameView][i]");

  EXPR_TEST(yatsc::LanguageMode::ES3, "delete i",
            "[UnaryExprView][TS_DELETE]\n"
            "  [NameView][i]");

  EXPR_TEST(yatsc::LanguageMode::ES3, "void i",
            "[UnaryExprView][TS_VOID]\n"
            "  [NameView][i]");

  EXPR_TEST(yatsc::LanguageMode::ES3, "typeof i",
            "[UnaryExprView][TS_TYPEOF]\n"
            "  [NameView][i]");

  EXPR_TEST(yatsc::LanguageMode::ES3, "+i",
            "[UnaryExprView][TS_PLUS]\n"
            "  [NameView][i]");

  EXPR_TEST(yatsc::LanguageMode::ES3, "-i",
            "[UnaryExprView][TS_MINUS]\n"
            "  [NameView][i]");

  EXPR_TEST(yatsc::LanguageMode::ES3, "~i",
            "[UnaryExprView][TS_BIT_NOR]\n"
            "  [NameView][i]");

  EXPR_TEST(yatsc::LanguageMode::ES3, "!i",
            "[UnaryExprView][TS_NOT]\n"
            "  [NameView][i]");
}


TEST(ExpressionParseTest, ParseExpression_postfix_expr) {
  EXPR_TEST(yatsc::LanguageMode::ES3, "i++",
            "[PostfixView][TS_INCREMENT]\n"
            "  [NameView][i]");

  EXPR_TEST(yatsc::LanguageMode::ES3, "i--",
            "[PostfixView][TS_DECREMENT]\n"
            "  [NameView][i]");
}

