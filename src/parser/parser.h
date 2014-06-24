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

#ifndef PARSER_PARSER_H
#define PARSER_PARSER_H

#include "../ir/node.h"
#include "../ir/irfactory.h"

namespace yatsc {
template <typename InputSourceIterator>
class Parser {
 public:
  Parser(Scanner<InputSourceIterator>* scanner, ErrorReporter* error_reporter)
      : scanner_(scanner),
        error_reporter_(error_reporter){}


  Node* Parse();

 VISIBLE_FOR_TEST:
  Node* ParseProgram();

  Node* ParseSourceElements();

  Node* ParseSourceElement();

  Node* ParseStatementList();

  Node* ParseStatement();

  Node* ParseBlockStatement();

  Node* ParseModuleStatement();

  Node* ParseImportStatement();

  Node* ParseExportStatement();

  Node* ParseDebuggerStatement();

  Node* ParseVariableDeclaration(bool noin);

  Node* ParseIfStatement();

  Node* ParseWhileStatement();

  Node* ParseDoWhileStatement();

  Node* ParseForStatement();

  Node* ParseContinueStatement();

  Node* ParseBreakStatement();

  Node* ParseReturnStatement();

  Node* ParseWithStatement();

  Node* ParseSwitchStatement();

  Node* ParseCaseClauses();

  Node* ParseLabelledStatement();

  Node* ParseThrowStatement();

  Node* ParseTryStatement();

  Node* ParseCatchBlock();

  Node* ParseFinallyBlock();

  Node* ParseClassDeclaration();

  Node* ParseClassFields();

  Node* ParseExpression();

  Node* ParseAssignmentExpression();

  Node* ParseYieldExpression();

  Node* ParseConditional();

  Node* ParseBinaryExpression();

  Node* ParseUnaryExpression();

  Node* ParsePostfixExpression();

  Node* ParseLeftHandSideExpression();

  Node* ParseNewExpression();

  Node* ParseCallExpression();

  Node* ParseArguments();

  Node* ParseMemberExpression();

  Node* ParsePrimaryExpression();

  Node* ParseObjectLiteral();

  Node* ParseArrayLiteral();

  Node* ParseLiteral();

  Node* ParseFunction();

  Node* ParseFormalParameterList();
  
 private:

  void Next() {
    current_ = scanner_->Scan();
    next_ = scanner_->Scan();
  }
  

  YATSC_INLINE const TokenInfo* Scan() YATSC_NOEXCEPT {
    return scanner_.Scan();
  }

  
  YATSC_INLINE const TokenInfo* Peek() YATSC_NOEXCEPT {
    return scanner_.Scan();
  }
  
  
  template <typename T, typename ... Args>
  T* New(Args ... args) {
    return irfactory_.New<T>(std::forward<Args>(args)...);
  }

  Scanner<InputSourceIterator>* scanner_;
  ir::IRFactory irfactory_;
  ErrorReporter* error_reporter_;
};


// Syntax error exception.
class SyntaxError: public std::exception, private Unmovable, private Uncopyable {
 public:
  SyntaxError(const std::string& message)
      : std::exception(),
        message_(message) {}

  
  /**
   * Return the reason of the error.
   * @returns The error message.
   */
  const char* what() const throw() {return message_.c_str();}
  
 private:
  std::string message_;
};
}

#endif
