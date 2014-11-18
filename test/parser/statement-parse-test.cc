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

bool breakable = false;
bool returnable = false;
bool continuable = false;
#define STATEMENT_TEST(type, code, expected_str)                        \
  PARSER_TEST("anonymous", ParseStatement(false, returnable, breakable, continuable), type, code, expected_str, false)

#define STATEMENT_TEST_THROW(type, code)                                \
  PARSER_TEST("anonymous",  ParseStatement(false, returnable, breakable, continuable), type, code, "", true)

#define STATEMENT_TEST_ALL(code, expected_str)                          \
  []{STATEMENT_TEST(yatsc::LanguageMode::ES3, code, expected_str);}();  \
  []{STATEMENT_TEST(yatsc::LanguageMode::ES5_STRICT, code, expected_str);}(); \
  []{STATEMENT_TEST(yatsc::LanguageMode::ES6, code, expected_str)}()

#define STATEMENT_THROW_TEST_ALL(code)                                \
  []{STATEMENT_TEST_THROW(yatsc::LanguageMode::ES3, code);}();        \
  []{STATEMENT_TEST_THROW(yatsc::LanguageMode::ES5_STRICT, code);}(); \
  []{STATEMENT_TEST_THROW(yatsc::LanguageMode::ES6, code)}()


TEST(StatementParseTest, ParseForStatement) {
  breakable = true;
  STATEMENT_TEST_ALL("for (var i = 0; i < 10; i++) {}",
                     "[ForStatementView]\n"
                     "  [VariableDeclView]\n"
                     "    [VariableView]\n"
                     "      [NameView][i]\n"
                     "      [NumberView][0]\n"
                     "      [Empty]\n"
                     "  [BinaryExprView][Less]\n"
                     "    [NameView][i]\n"
                     "    [NumberView][10]\n"
                     "  [PostfixView][Increment]\n"
                     "    [NameView][i]\n"
                     "  [BlockView]");

  STATEMENT_TEST_ALL("for (var i = 0; i < 10; i++) {break;}",
                     "[ForStatementView]\n"
                     "  [VariableDeclView]\n"
                     "    [VariableView]\n"
                     "      [NameView][i]\n"
                     "      [NumberView][0]\n"
                     "      [Empty]\n"
                     "  [BinaryExprView][Less]\n"
                     "    [NameView][i]\n"
                     "    [NumberView][10]\n"
                     "  [PostfixView][Increment]\n"
                     "    [NameView][i]\n"
                     "  [BlockView]\n"
                     "    [BreakStatementView]\n"
                     "      [Empty]");


  STATEMENT_TEST_ALL("for (i = 0; i < 10; i++) {}",
                     "[ForStatementView]\n"
                     "  [AssignmentView][Assign]\n"
                     "    [NameView][i]\n"
                     "    [NumberView][0]\n"
                     "  [BinaryExprView][Less]\n"
                     "    [NameView][i]\n"
                     "    [NumberView][10]\n"
                     "  [PostfixView][Increment]\n"
                     "    [NameView][i]\n"
                     "  [BlockView]");

  STATEMENT_TEST_ALL("for (i = 0; i < 10;) {}",
                     "[ForStatementView]\n"
                     "  [AssignmentView][Assign]\n"
                     "    [NameView][i]\n"
                     "    [NumberView][0]\n"
                     "  [BinaryExprView][Less]\n"
                     "    [NameView][i]\n"
                     "    [NumberView][10]\n"
                     "  [Empty]\n"
                     "  [BlockView]");


  STATEMENT_TEST_ALL("for (i = 0;;i++) {}",
                     "[ForStatementView]\n"
                     "  [AssignmentView][Assign]\n"
                     "    [NameView][i]\n"
                     "    [NumberView][0]\n"
                     "  [Empty]\n"
                     "  [PostfixView][Increment]\n"
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
                     "  [BinaryExprView][Less]\n"
                     "    [NameView][i]\n"
                     "    [NumberView][10]\n"
                     "  [PostfixView][Increment]\n"
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
  breakable = true;
  STATEMENT_TEST_ALL("while (true) {}",
                     "[WhileStatementView]\n"
                     "  [TrueView]\n"
                     "  [BlockView]");

  STATEMENT_TEST_ALL("while (i < 10 && x > 10) {}",
                     "[WhileStatementView]\n"
                     "  [BinaryExprView][LogicalAnd]\n"
                     "    [BinaryExprView][Less]\n"
                     "      [NameView][i]\n"
                     "      [NumberView][10]\n"
                     "    [BinaryExprView][Greater]\n"
                     "      [NameView][x]\n"
                     "      [NumberView][10]\n"
                     "  [BlockView]");
}


TEST(StatementParseTest, ParseDoWhileStatement) {
  breakable = true;
  STATEMENT_TEST_ALL("do {} while (true);",
                     "[DoWhileStatementView]\n"
                     "  [TrueView]\n"
                     "  [BlockView]");

  STATEMENT_TEST_ALL("do {} while (i < 10 && x > 10);",
                     "[DoWhileStatementView]\n"
                     "  [BinaryExprView][LogicalAnd]\n"
                     "    [BinaryExprView][Less]\n"
                     "      [NameView][i]\n"
                     "      [NumberView][10]\n"
                     "    [BinaryExprView][Greater]\n"
                     "      [NameView][x]\n"
                     "      [NumberView][10]\n"
                     "  [BlockView]");
}


TEST(StatementParseTest, ParseIfStatement) {
  breakable = false;
  STATEMENT_TEST_ALL("if (true) {}",
                     "[IfStatementView]\n"
                     "  [TrueView]\n"
                     "  [BlockView]\n"
                     "  [Empty]");

  STATEMENT_TEST_ALL("if (true) {} else {}",
                     "[IfStatementView]\n"
                     "  [TrueView]\n"
                     "  [BlockView]\n"
                     "  [BlockView]");

  STATEMENT_TEST_ALL("if (true) {} else if (true) {}",
                     "[IfStatementView]\n"
                     "  [TrueView]\n"
                     "  [BlockView]\n"
                     "  [IfStatementView]\n"
                     "    [TrueView]\n"
                     "    [BlockView]\n"
                     "    [Empty]");

  STATEMENT_TEST_ALL("if (true) {} else if (true) {} else {}",
                     "[IfStatementView]\n"
                     "  [TrueView]\n"
                     "  [BlockView]\n"
                     "  [IfStatementView]\n"
                     "    [TrueView]\n"
                     "    [BlockView]\n"
                     "    [BlockView]");
}


TEST(StatementParseTest, ParseSwitchStatement) {
  breakable = false;
  STATEMENT_TEST_ALL("switch (a) {case 1:break;case 2:break;default:{}}",
                     "[SwitchStatementView]\n"
                     "  [NameView][a]\n"
                     "  [CaseListView]\n"
                     "    [CaseView]\n"
                     "      [NumberView][1]\n"
                     "      [CaseBody]\n"
                     "        [BreakStatementView]\n"
                     "          [Empty]\n"
                     "    [CaseView]\n"
                     "      [NumberView][2]\n"
                     "      [CaseBody]\n"
                     "        [BreakStatementView]\n"
                     "          [Empty]\n"
                     "    [CaseView]\n"
                     "      [Empty]\n"
                     "      [CaseBody]\n"
                     "        [BlockView]");

  STATEMENT_THROW_TEST_ALL("switch (a) {case 1:break;case 2:break;default:{}default:{}}");
  STATEMENT_THROW_TEST_ALL("switch (a) {case :break;case 2:break;default:{}}");
}


TEST(StatementParseTest, ParseThrowStatement) {
  breakable = false;
  STATEMENT_TEST_ALL("throw a;",
                     "[ThrowStatementView]\n"
                     "  [NameView][a]");
}

TEST(StatementParseTest, ParseDebuggerStatement) {
  breakable = false;
  STATEMENT_TEST_ALL("debugger;",
                     "[DebuggerView]");
}

TEST(StatementParseTest, ParseLabelledStatement) {
  breakable = false;
  STATEMENT_TEST_ALL("LABEL:if(1){}",
                     "[LabelledStatementView]\n"
                     "  [NameView][LABEL]\n"
                     "  [IfStatementView]\n"
                     "    [NumberView][1]\n"
                     "    [BlockView]\n"
                     "    [Empty]");
}

TEST(StatementParseTest, ParseTryCatch) {
  breakable = false;
  STATEMENT_TEST_ALL("try {} catch(e) {}",
                     "[TryStatementView]\n"
                     "  [BlockView]\n"
                     "  [CatchStatementView]\n"
                     "    [NameView][e]\n"
                     "    [BlockView]\n"
                     "  [Empty]");

  STATEMENT_TEST_ALL("try {} catch(e) {} finally {}",
                     "[TryStatementView]\n"
                     "  [BlockView]\n"
                     "  [CatchStatementView]\n"
                     "    [NameView][e]\n"
                     "    [BlockView]\n"
                     "  [FinallyStatementView]\n"
                     "    [BlockView]");
}


TEST(StatementParseTest, ParseReturnStatement) {
  breakable = false;
  returnable = true;
  STATEMENT_TEST_ALL("return 0;",
                     "[ReturnStatementView]\n"
                     "  [NumberView][0]");

  STATEMENT_TEST_ALL("return;",
                     "[ReturnStatementView]\n"
                     "  [Empty]");

  STATEMENT_TEST_ALL("return",
                     "[ReturnStatementView]\n"
                     "  [Empty]");

  returnable = false;
  STATEMENT_THROW_TEST_ALL("return 0");
}


TEST(StatementParseTest, ParseBreakStatement) {
  breakable = true;
  returnable = false;
  STATEMENT_TEST_ALL("break;",
                     "[BreakStatementView]\n"
                     "  [Empty]");

  STATEMENT_TEST_ALL("break LABEL;",
                     "[BreakStatementView]\n"
                     "  [NameView][LABEL]");

  breakable = false;
  STATEMENT_THROW_TEST_ALL("break;");
}


TEST(StatementParseTest, ParseContinueStatement) {
  continuable = true;
  STATEMENT_TEST_ALL("continue;",
                     "[ContinueStatementView]\n"
                     "  [Empty]");

  STATEMENT_TEST_ALL("continue LABEL;",
                     "[ContinueStatementView]\n"
                     "  [NameView][LABEL]");

  continuable = false;
  STATEMENT_THROW_TEST_ALL("continue;");
}


TEST(StatementParseTest, ParseWithStatement) {
  continuable = false;
  breakable = false;
  returnable = false;

  STATEMENT_TEST_ALL("with(x) {}",
                     "[WithStatementView]\n"
                     "  [NameView][x]\n"
                     "  [BlockView]");
}


TEST(StatementParseTest, ParseBlockStatement) {
  continuable = false;
  breakable = false;
  returnable = false;

  STATEMENT_TEST_ALL("{var x = 0;var y = 0;}",
                     "[BlockView]\n"
                     "  [VariableDeclView]\n"
                     "    [VariableView]\n"
                     "      [NameView][x]\n"
                     "      [NumberView][0]\n"
                     "      [Empty]\n"
                     "  [VariableDeclView]\n"
                     "    [VariableView]\n"
                     "      [NameView][y]\n"
                     "      [NumberView][0]\n"
                     "      [Empty]");
}
