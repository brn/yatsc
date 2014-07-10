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
#include "../compare-node.h"
#include "../unicode-util.h"

static bool show_parse_phase = false;

#define PARSER_METHOD_CALL__(var, method_expr)  \
  var.method_expr

#define PARSER_TEST__(method_expr, type, code, expected_str, dothrow, error_type) { \
    typedef std::vector<yatsc::UChar>::iterator Iterator;               \
    std::string s = code;                                               \
    std::string n = "anonymous";                                        \
    yatsc::ErrorReporter error_reporter(s, n);                          \
    std::vector<yatsc::UChar> v__ = yatsc::testing::AsciiToUCharVector(code); \
    yatsc::CompilerOption compiler_option;                              \
    compiler_option.set_language_mode(type);                            \
    yatsc::Scanner<Iterator> scanner(v__.begin(), v__.end(), &error_reporter, compiler_option); \
    yatsc::Parser<Iterator> parser(&scanner, &error_reporter);          \
    if (show_parse_phase) parser.EnablePrintParsePhase();               \
    show_parse_phase = false;                                           \
    if (!dothrow) {                                                     \
      auto node = PARSER_METHOD_CALL__(parser, method_expr);            \
      yatsc::testing::CompareNode(node->ToStringTree(), std::string(expected_str)); \
    } else {                                                            \
      ASSERT_THROW(PARSER_METHOD_CALL__(parser, method_expr), error_type); \
    }                                                                   \
  }

#define EXPR_TEST(type, code, expected_str)                             \
  PARSER_TEST__(ParseExpression(true, false), type, code, expected_str, false, std::exception)

#define EXPR_THROW_TEST(type, code, error_type)                         \
  PARSER_TEST__(ParseExpression(true, false), type, code, "", true, error_type)

#define LEXICAL_DECL_TEST(type, code, expected_str)                     \
  PARSER_TEST__(ParseLexicalDeclaration(true, false), type, code, expected_str, false, std::exception)

#define LEXICAL_DECL_THROW_TEST(type, code, error_type)                 \
  PARSER_TEST__(ParseLexicalDeclaration(true, false), type, code, "", true, error_type)


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
            "  [CallArgsView]\n"
            "  [Empty]");
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
            "  [CallArgsView]\n"
            "  [Empty]");
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
            "        [Empty]\n"
            "      [GetPropView]\n"
            "        [NameView][foo]\n"
            "        [NameView][baz]\n"
            "    [CallArgsView]\n"
            "    [Empty]\n"
            "  [CallArgsView]\n"
            "  [Empty]");
}


TEST(ParserTest, ParseExpression_generic_call) {
  EXPR_TEST(yatsc::LanguageMode::ES3, "func<string>()",
            "[CallView]\n"
            "  [NameView][func]\n"
            "  [CallArgsView]\n"
            "  [TypeArgumentsView]\n"
            "    [SimpleTypeExprView]\n"
            "      [NameView][string]");
}


TEST(ParserTest, ParseExpression_generic_property_call) {
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


TEST(ParserTest, ParseExpression_arrow_function) {
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


TEST(ParserTest, ParseExpression_generic_arrow_function) {
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


TEST(ParserTest, ParseExpression_constraints_generic_arrow_function) {
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


TEST(ParserTest, ParseExpression_generic_instance_arrow_function) {
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


TEST(ParserTest, ParseExpression_function_type) {
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


TEST(ParserTest, ParseExpression_regexp) {
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
            "      [NameView][a]\n"
            "      [BinaryExprView][TS_PLUS]\n"
            "        [NameView][b]\n"
            "        [NameView][x]\n"
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
            "    [BinaryExprView][TS_MINUS]\n"
            "      [NameView][a]\n"
            "      [BinaryExprView][TS_PLUS]\n"
            "        [NameView][b]\n"
            "        [NameView][x]");
}


TEST(ParserTest, ParseExpression_unary_expr) {
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


TEST(ParserTest, ParseExpression_postfix_expr) {
  EXPR_TEST(yatsc::LanguageMode::ES3, "i++",
            "[PostfixView][TS_INCREMENT]\n"
            "  [NameView][i]");

  EXPR_TEST(yatsc::LanguageMode::ES3, "i--",
            "[PostfixView][TS_DECREMENT]\n"
            "  [NameView][i]");
}


TEST(ParserTest, ParseLexicalDeclaration_let) {
  LEXICAL_DECL_TEST(yatsc::LanguageMode::ES3, "let x = 100;",
                    "[LexicalDeclView][TS_LET]\n"
                    "  [VariableView]\n"
                    "    [NameView][x]\n"
                    "    [NumberView][100]\n"
                    "    [Empty]");

  LEXICAL_DECL_TEST(yatsc::LanguageMode::ES3, "let x = 100, y = 200, z = 300",
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

  LEXICAL_DECL_TEST(yatsc::LanguageMode::ES3, "let x: string = 100;",
                    "[LexicalDeclView][TS_LET]\n"
                    "  [VariableView]\n"
                    "    [NameView][x]\n"
                    "    [NumberView][100]\n"
                    "    [SimpleTypeExprView]\n"
                    "      [NameView][string]");
  
  LEXICAL_DECL_TEST(yatsc::LanguageMode::ES3, "let [a, b, c] = [1,2,3];",
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
  
  LEXICAL_DECL_TEST(yatsc::LanguageMode::ES3, "let {a,b,c} = {a:100,b:200,c:300};",
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

  LEXICAL_DECL_TEST(yatsc::LanguageMode::ES3, "let {foo:a,b:{c:[x,y,z]},c:[{bar}]} = {a:100,b:{c:[1,2,3]},c:[{bar:100}]};",
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


  LEXICAL_DECL_TEST(yatsc::LanguageMode::ES3, "let [a, b, c]: number[] = [1,2,3];",
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

  LEXICAL_DECL_TEST(yatsc::LanguageMode::ES3, "let {a,b,c}: Object = {a:100,b:200,c:300};",
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


TEST(ParserTest, ParseLexicalDeclaration_const) {
  LEXICAL_DECL_TEST(yatsc::LanguageMode::ES3, "const x = 100;",
                    "[LexicalDeclView][TS_CONST]\n"
                    "  [VariableView]\n"
                    "    [NameView][x]\n"
                    "    [NumberView][100]\n"
                    "    [Empty]");
}
