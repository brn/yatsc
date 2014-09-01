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


#define STATEMENT_TEST(type, code, expected_str)                     \
  PARSER_TEST(ParseStatement(false, false), type, code, expected_str, false, std::exception)

#define STATEMENT_TEST_ALL(code, expected_str)                          \
  []{STATEMENT_TEST(yatsc::LanguageMode::ES3, code, expected_str);}();  \
  []{STATEMENT_TEST(yatsc::LanguageMode::ES5_STRICT, code, expected_str);}(); \
  []{STATEMENT_TEST(yatsc::LanguageMode::ES6, code, expected_str)}()


TEST(StatementParseTest, ParseForStatement) {
  STATEMENT_TEST_ALL("for (var i = 0; i < 10; i++) {}",
                     "[ForStatementView]\n"
                     "  [VariableDeclView]\n"
                     "    [VariableView]\n"
                     "      [NameView][i]\n"
                     "      [NumberView][0]\n"
                     "      [Empty]\n"
                     "  [BinaryExprView][TS_LESS]\n"
                     "    [NameView][i]\n"
                     "    [NumberView][10]\n"
                     "  [PostfixView][TS_INCREMENT]\n"
                     "    [NameView][i]\n"
                     "  [BlockView]");


  STATEMENT_TEST_ALL("for (i = 0; i < 10; i++) {}",
                     "[ForStatementView]\n"
                     "  [AssignmentView][TS_ASSIGN]\n"
                     "    [NameView][i]\n"
                     "    [NumberView][0]\n"
                     "  [BinaryExprView][TS_LESS]\n"
                     "    [NameView][i]\n"
                     "    [NumberView][10]\n"
                     "  [PostfixView][TS_INCREMENT]\n"
                     "    [NameView][i]\n"
                     "  [BlockView]");

  STATEMENT_TEST_ALL("for (i = 0; i < 10;) {}",
                     "[ForStatementView]\n"
                     "  [AssignmentView][TS_ASSIGN]\n"
                     "    [NameView][i]\n"
                     "    [NumberView][0]\n"
                     "  [BinaryExprView][TS_LESS]\n"
                     "    [NameView][i]\n"
                     "    [NumberView][10]\n"
                     "  [Empty]\n"
                     "  [BlockView]");


  STATEMENT_TEST_ALL("for (i = 0;;i++) {}",
                     "[ForStatementView]\n"
                     "  [AssignmentView][TS_ASSIGN]\n"
                     "    [NameView][i]\n"
                     "    [NumberView][0]\n"
                     "  [Empty]\n"
                     "  [PostfixView][TS_INCREMENT]\n"
                     "    [NameView][i]\n"
                     "  [BlockView]");


  STATEMENT_TEST_ALL("for (var i = 0, x = 20; i < 10; i++) {}",
                     "[ForStatementView]\n"
                     "  [VariableDeclView]\n"
                     "    [VariableView]\n"
                     "      [NameView][i]\n"
                     "      [NumberView][0]\n"
                     "      [Empty]\n"
                     "    [VariableView]\n"
                     "      [NameView][x]\n"
                     "      [NumberView][20]\n"
                     "      [Empty]\n"
                     "  [BinaryExprView][TS_LESS]\n"
                     "    [NameView][i]\n"
                     "    [NumberView][10]\n"
                     "  [PostfixView][TS_INCREMENT]\n"
                     "    [NameView][i]\n"
                     "  [BlockView]");

  
  STATEMENT_TEST_ALL("for (;;) {}",
                     "[ForStatementView]\n"
                     "  [Empty]\n"
                     "  [Empty]\n"
                     "  [Empty]\n"
                     "  [BlockView]");


  STATEMENT_TEST_ALL("for (var i in obj) {}",
                     "[ForInStatementView]\n"
                     "  [VariableDeclView]\n"
                     "    [VariableView]\n"
                     "      [NameView][i]\n"
                     "      [Empty]\n"
                     "      [Empty]\n"
                     "  [NameView][obj]\n"
                     "  [BlockView]");

  STATEMENT_TEST_ALL("for (i in obj) {}",
                     "[ForInStatementView]\n"
                     "  [NameView][i]\n"
                     "  [NameView][obj]\n"
                     "  [BlockView]");


  STATEMENT_TEST(yatsc::LanguageMode::ES6, "for (var i of x) {}",
                 "[ForOfStatementView]\n"
                 "  [VariableDeclView]\n"
                 "    [VariableView]\n"
                 "      [NameView][i]\n"
                 "      [Empty]\n"
                 "      [Empty]\n"
                 "  [NameView][x]\n"
                 "  [BlockView]");

  
  STATEMENT_TEST(yatsc::LanguageMode::ES6, "for (i of x) {}",
                 "[ForOfStatementView]\n"
                 "  [NameView][i]\n"
                 "  [NameView][x]\n"
                 "  [BlockView]");
}


TEST(StatementParseTest, ParseWhileStatement) {
  STATEMENT_TEST_ALL("while (true) {}",
                     "[WhileStatementView]\n"
                     "  [TrueView]\n"
                     "  [BlockView]");

  STATEMENT_TEST_ALL("while (i < 10 && x > 10) {}",
                     "[WhileStatementView]\n"
                     "  [BinaryExprView][TS_LOGICAL_AND]\n"
                     "    [BinaryExprView][TS_LESS]\n"
                     "      [NameView][i]\n"
                     "      [NumberView][10]\n"
                     "    [BinaryExprView][TS_GREATER]\n"
                     "      [NameView][x]\n"
                     "      [NumberView][10]\n"
                     "  [BlockView]");
}
