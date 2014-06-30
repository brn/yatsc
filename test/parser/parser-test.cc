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
#include "../unicode-util.h"

#define INIT(type, var, str, lambda) {                                  \
    typedef std::vector<yatsc::UChar>::iterator Iterator;               \
    std::string s = str;                                                \
    std::string n = "anonymous";                                        \
    yatsc::ErrorReporter error_reporter(s, n);                          \
    std::vector<yatsc::UChar> v__ = yatsc::testing::AsciiToUCharVector(str); \
    yatsc::CompilerOption compiler_option;                              \
    compiler_option.set_language_mode(type);                            \
    yatsc::Scanner<Iterator> scanner(v__.begin(), v__.end(), &error_reporter, compiler_option); \
    yatsc::Parser<Iterator> var(&scanner, &error_reporter);             \
    lambda();                                                           \
  }


TEST(ParserTest, ParseLiteral_string) {
  INIT(yatsc::LanguageMode::ES3, parser, "'aaaaaaa'", [&]{
    auto node = parser.ParseExpression(false);
    ASSERT_EQ(node->node_type(), yatsc::ir::NodeType::kStringView);
  });
}


TEST(ParserTest, ParseLiteral_numeric) {
  INIT(yatsc::LanguageMode::ES3, parser, "12345", [&]{
    auto node = parser.ParseExpression(false);
    ASSERT_EQ(node->node_type(), yatsc::ir::NodeType::kNumberView);
  });
}


TEST(ParserTest, ParseLiteral_boolean) {
  INIT(yatsc::LanguageMode::ES3, parser, "true", [&]{
    auto node = parser.ParseExpression(false);
    ASSERT_EQ(node->node_type(), yatsc::ir::NodeType::kTrueView);
  });


  INIT(yatsc::LanguageMode::ES3, parser, "false", [&]{
    auto node = parser.ParseExpression(false);
    ASSERT_EQ(node->node_type(), yatsc::ir::NodeType::kFalseView);
  });
}


TEST(ParserTest, ParseLiteral_undefined) {
  INIT(yatsc::LanguageMode::ES3, parser, "undefined", [&]{
    auto node = parser.ParseExpression(false);
    ASSERT_EQ(node->node_type(), yatsc::ir::NodeType::kUndefinedView);
  });
}


TEST(ParserTest, ParseLiteral_null) {
  INIT(yatsc::LanguageMode::ES3, parser, "null", [&]{
    auto node = parser.ParseExpression(false);
    ASSERT_EQ(node->node_type(), yatsc::ir::NodeType::kNullView);
  });
}


TEST(ParserTest, ParseLiteral_NaN) {
  INIT(yatsc::LanguageMode::ES3, parser, "NaN", [&]{
    auto node = parser.ParseExpression(false);
    ASSERT_EQ(node->node_type(), yatsc::ir::NodeType::kNaNView);
  });
}


TEST(ParserTest, ParsePrimaryExpression_this) {
  INIT(yatsc::LanguageMode::ES3, parser, "this", [&]{
    auto node = parser.ParseExpression(false);
    ASSERT_EQ(node->node_type(), yatsc::ir::NodeType::kThisView);
  });
}


TEST(ParserTest, ParsePrimaryExpression_identifier) {
  INIT(yatsc::LanguageMode::ES3, parser, "Identifier", [&]{
    auto node = parser.ParseExpression(false);
    ASSERT_EQ(node->node_type(), yatsc::ir::NodeType::kNameView);
  });
}


TEST(ParserTest, ParseExpression_array) {
  INIT(yatsc::LanguageMode::ES3, parser, "[1,2,3,4]", [&]{
    auto node = parser.ParseExpression(false);
    ASSERT_EQ(node->node_type(), yatsc::ir::NodeType::kArrayLiteralView);
    ASSERT_TRUE(node->HasArrayLiteralView());
    const yatsc::ir::Node::List& list = node->node_list();
    ASSERT_EQ(list.size(), 4);
    for (int i = 0; i < 4; i++) {
      ASSERT_EQ(list[i]->node_type(), yatsc::ir::NodeType::kNumberView);
      ASSERT_EQ(list[i]->double_value(), static_cast<double>(i + 1));
    }
  });
}


TEST(ParserTest, ParseExpression_arrow_function) {
  INIT(yatsc::LanguageMode::ES3, parser, "(a:string,b:number) => a + b", [&]{
    auto node = parser.ParseExpression(false);
    ASSERT_EQ(node->node_type(), yatsc::ir::NodeType::kArrowFunctionView);
    ASSERT_TRUE(node->HasArrowFunctionView());
  });
}


TEST(ParserTest, ParseExpression_parenthesized_expression) {
  INIT(yatsc::LanguageMode::ES3, parser, "(100,200,300,b)", [&]{
    auto node = parser.ParseExpression(false);
    ASSERT_EQ(node->node_type(), yatsc::ir::NodeType::kCommaExprView);
    ASSERT_TRUE(node->HasCommaExprView());
  });
}


TEST(ParserTest, ParseExpression_parenthesized_expression_like_arrow_function) {
  INIT(yatsc::LanguageMode::ES3, parser, "(a, b, c) => a + b", [&]{
    auto node = parser.ParseExpression(false);
    ASSERT_EQ(node->node_type(), yatsc::ir::NodeType::kArrowFunctionView);
    ASSERT_TRUE(node->HasArrowFunctionView());
  });
}
