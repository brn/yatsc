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
#include "../../src/ir/node.h"
#include "../../src/parser/parser.h"
#include "../compare-string.h"
#include "../unicode-util.h"

#define EXPR_TEST(type, code, expected_str) {                           \
    typedef std::vector<yatsc::UChar>::iterator Iterator;               \
    std::string s = code;                                               \
    std::string n = "anonymous";                                        \
    yatsc::ErrorReporter error_reporter(s, n);                          \
    std::vector<yatsc::UChar> v__ = yatsc::testing::AsciiToUCharVector(code); \
    yatsc::CompilerOption compiler_option;                              \
    compiler_option.set_language_mode(type);                            \
    yatsc::Scanner<Iterator> scanner(v__.begin(), v__.end(), &error_reporter, compiler_option); \
    yatsc::Parser<Iterator> parser(&scanner, &error_reporter);          \
    auto node = parser.ParseExpression(false);                          \
    const char* expected = expected_str;                                \
    ASSERT_STREQ(node->ToStringTree().c_str(), expected);               \
  }


TEST(ParserTest, ParseLiteral_string) {
  EXPR_TEST(yatsc::LanguageMode::ES3, "'aaaaaaa'", "[StringView]['aaaaaaa']");
}


TEST(ParserTest, ParseLiteral_numeric) {
  EXPR_TEST(yatsc::LanguageMode::ES3, "12345", "[NumberView][12345]");
}


TEST(ParserTest, ParseLiteral_boolean) {
  EXPR_TEST(yatsc::LanguageMode::ES3, "true", "[TrueView]");


  EXPR_TEST(yatsc::LanguageMode::ES3, "false", "[FalseView]");
}


TEST(ParserTest, ParseLiteral_undefined) {
  EXPR_TEST(yatsc::LanguageMode::ES3, "undefined", "[UndefinedView]");
}


TEST(ParserTest, ParseLiteral_null) {
  EXPR_TEST(yatsc::LanguageMode::ES3, "null", "[NullView]");
}


TEST(ParserTest, ParseLiteral_NaN) {
  EXPR_TEST(yatsc::LanguageMode::ES3, "NaN", "[NaNView]");
}


TEST(ParserTest, ParsePrimaryExpression_this) {
  EXPR_TEST(yatsc::LanguageMode::ES3, "this", "[ThisView]");
}


TEST(ParserTest, ParsePrimaryExpression_identifier) {
  EXPR_TEST(yatsc::LanguageMode::ES3, "Identifier", "[NameView][Identifier]");
}


TEST(ParserTest, ParseExpression_array) {
  EXPR_TEST(yatsc::LanguageMode::ES3, "[1,2,3,4]",
       "[ArrayLiteralView]\n"
       "  [NumberView][1]\n"
       "  [NumberView][2]\n"
       "  [NumberView][3]\n"
       "  [NumberView][4]");
}


TEST(ParserTest, ParseExpression_object) {
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


TEST(ParserTest, ParseExpression_call) {
  EXPR_TEST(yatsc::LanguageMode::ES3, "func()",
       "[CallView]\n"
       "  [NameView][func]\n"
       "  [CallArgsView]");
}


TEST(ParserTest, ParseExpression_property_call) {
  EXPR_TEST(yatsc::LanguageMode::ES3, "foo.bar.null.func()",
       "[CallView]\n"
       "  [GetPropView]\n"
       "    [NameView][foo]\n"
       "    [GetPropView]\n"
       "      [NameView][bar]\n"
       "      [GetPropView]\n"
       "        [NullView]\n"
       "        [NameView][func]\n"
       "  [CallArgsView]");
}


TEST(ParserTest, ParseExpression_property_call_call) {
  EXPR_TEST(yatsc::LanguageMode::ES3, "foo.func().foo.baz()()",
       "[CallView]\n"
       "  [CallView]\n"
       "    [GetPropView]\n"
       "      [CallView]\n"
       "        [GetPropView]\n"
       "          [NameView][foo]\n"
       "          [NameView][func]\n"
       "        [CallArgsView]\n"
       "      [GetPropView]\n"
       "        [NameView][foo]\n"
       "        [NameView][baz]\n"
       "    [CallArgsView]\n"
       "  [CallArgsView]");
}


TEST(ParserTest, ParseExpression_arrow_function) {
  EXPR_TEST(yatsc::LanguageMode::ES3, "(a:string,b:number) => a + b",
       "[ArrowFunctionView]\n"
       "  [CallSignatureView]\n"
       "    [ParamList]\n"
       "      [ParameterView]\n"
       "        [NameView][a]\n"
       "        [nullptr]\n"
       "        [SimpleTypeExprView]\n"
       "          [NameView][string]\n"
       "        [nullptr]\n"
       "      [ParameterView]\n"
       "        [NameView][b]\n"
       "        [nullptr]\n"
       "        [SimpleTypeExprView]\n"
       "          [NameView][number]\n"
       "        [nullptr]\n"
       "    [nullptr]\n"
       "  [BinaryExprView][TS_PLUS]\n"
       "    [NameView][a]\n"
       "    [NameView][b]");
}


TEST(ParserTest, ParseExpression_parenthesized_expression) {
  EXPR_TEST(yatsc::LanguageMode::ES3, "(100,200,300,b)",
       "[CommaExprView]\n"
       "  [NumberView][100]\n"
       "  [NumberView][200]\n"
       "  [NumberView][300]\n"
       "  [NameView][b]");
}


TEST(ParserTest, ParseExpression_parenthesized_expression_like_arrow_function) {
  EXPR_TEST(yatsc::LanguageMode::ES3, "(a, b, c) => a + b",
       "[ArrowFunctionView]\n"
       "  [CallSignatureView]\n"
       "    [ParamList]\n"
       "      [ParameterView]\n"
       "        [NameView][a]\n"
       "        [nullptr]\n"
       "        [nullptr]\n"
       "        [nullptr]\n"
       "      [ParameterView]\n"
       "        [NameView][b]\n"
       "        [nullptr]\n"
       "        [nullptr]\n"
       "        [nullptr]\n"
       "      [ParameterView]\n"
       "        [NameView][c]\n"
       "        [nullptr]\n"
       "        [nullptr]\n"
       "        [nullptr]\n"
       "    [nullptr]\n"
       "  [BinaryExprView][TS_PLUS]\n"
       "    [NameView][a]\n"
       "    [NameView][b]");
}


TEST(ParserTest, ParseExpression_function_type) {
  EXPR_TEST(yatsc::LanguageMode::ES3, "(param1:(string, number, Object) => string, param2):string => param1 + param2",
       "[ArrowFunctionView]\n"
       "  [CallSignatureView]\n"
       "    [ParamList]\n"
       "      [ParameterView]\n"
       "        [NameView][param1]\n"
       "        [nullptr]\n"
       "        [FunctionTypeExprView]\n"
       "          [ParamList]\n"
       "            [ParameterView]\n"
       "              [NameView][string]\n"
       "              [nullptr]\n"
       "              [nullptr]\n"
       "              [nullptr]\n"
       "            [ParameterView]\n"
       "              [NameView][number]\n"
       "              [nullptr]\n"
       "              [nullptr]\n"
       "              [nullptr]\n"
       "            [ParameterView]\n"
       "              [NameView][Object]\n"
       "              [nullptr]\n"
       "              [nullptr]\n"
       "              [nullptr]\n"
       "          [SimpleTypeExprView]\n"
       "            [NameView][string]\n"
       "        [nullptr]\n"
       "      [ParameterView]\n"
       "        [NameView][param2]\n"
       "        [nullptr]\n"
       "        [nullptr]\n"
       "        [nullptr]\n"
       "    [SimpleTypeExprView]\n"
       "      [NameView][string]\n"
       "  [BinaryExprView][TS_PLUS]\n"
       "    [NameView][param1]\n"
       "    [NameView][param2]");
}


TEST(ParserTest, ParseExpression_assignment_expr) {
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
}


TEST(ParserTest, ParseExpression_condition_expr) {
  EXPR_TEST(yatsc::LanguageMode::ES3, "x? y: z",
       "[TemaryExprView]\n"
       "  [NameView][x]\n"
       "  [NameView][y]\n"
       "  [NameView][z]");


  EXPR_TEST(yatsc::LanguageMode::ES3, "x === 100? (a,b) => a + b + x: (a,b) => a - b + x",
       "[TemaryExprView]\n"
       "  [BinaryExprView][TS_EQ]\n"
       "    [NameView][x]\n"
       "    [NumberView][100]\n"
       "  [ArrowFunctionView]\n"
       "    [CallSignatureView]\n"
       "      [ParamList]\n"
       "        [ParameterView]\n"
       "          [NameView][a]\n"
       "          [nullptr]\n"
       "          [nullptr]\n"
       "          [nullptr]\n"
       "        [ParameterView]\n"
       "          [NameView][b]\n"
       "          [nullptr]\n"
       "          [nullptr]\n"
       "          [nullptr]\n"
       "      [nullptr]\n"
       "    [BinaryExprView][TS_PLUS]\n"
       "      [NameView][a]\n"
       "      [BinaryExprView][TS_PLUS]\n"
       "        [NameView][b]\n"
       "        [NameView][x]\n"
       "  [ArrowFunctionView]\n"
       "    [CallSignatureView]\n"
       "      [ParamList]\n"
       "        [ParameterView]\n"
       "          [NameView][a]\n"
       "          [nullptr]\n"
       "          [nullptr]\n"
       "          [nullptr]\n"
       "        [ParameterView]\n"
       "          [NameView][b]\n"
       "          [nullptr]\n"
       "          [nullptr]\n"
       "          [nullptr]\n"
       "      [nullptr]\n"
       "    [BinaryExprView][TS_MINUS]\n"
       "      [NameView][a]\n"
       "      [BinaryExprView][TS_PLUS]\n"
       "        [NameView][b]\n"
       "        [NameView][x]");
}
