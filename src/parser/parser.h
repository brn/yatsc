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
#include "../parser/scanner.h"

namespace yatsc {
template <typename UCharInputSourceIterator>
class Parser {
 public:
  Parser(Scanner<UCharInputSourceIterator>* scanner, ErrorReporter* error_reporter)
      : print_parser_phase_(false),
        scanner_(scanner),
        error_reporter_(error_reporter){}


  ir::Node* Parse();

 VISIBLE_FOR_TEST:
  ir::Node* ParseProgram();

  ir::Node* ParseSourceElements();

  ir::Node* ParseSourceElement();

  ir::Node* ParseStatementList();

  ir::Node* ParseStatement();

  ir::Node* ParseBlockStatement();

  ir::Node* ParseModuleStatement();

  ir::Node* ParseImportStatement();

  ir::Node* ParseExportStatement();

  ir::Node* ParseDebuggerStatement();

  ir::Node* ParseVariableDeclaration(bool noin);

  ir::Node* ParseIfStatement();

  ir::Node* ParseWhileStatement();

  ir::Node* ParseDoWhileStatement();

  ir::Node* ParseForStatement();

  ir::Node* ParseContinueStatement();

  ir::Node* ParseBreakStatement();

  ir::Node* ParseReturnStatement();

  ir::Node* ParseWithStatement();

  ir::Node* ParseSwitchStatement();

  ir::Node* ParseCaseClauses();

  ir::Node* ParseLabelledStatement();

  ir::Node* ParseThrowStatement();

  ir::Node* ParseTryStatement();

  ir::Node* ParseCatchBlock();

  ir::Node* ParseFinallyBlock();

  ir::Node* ParseClassDeclaration();

  ir::Node* ParseClassFields();

  ir::Node* ParseExpression(bool noin);

  ir::Node* ParseAssignmentExpression(bool noin);

  ir::Node* ParseYieldExpression();

  ir::Node* ParseConditionalExpression(bool noin);

  ir::Node* ParseBinaryExpression(bool noin);

  ir::Node* ParseUnaryExpression();

  ir::Node* ParsePostfixExpression();

  ir::Node* ParseLeftHandSideExpression();

  ir::Node* ParseTypeExpression();

  ir::Node* ParseObjectTypeExpression();

  ir::Node* ParseObjectTypeElement();

  ir::Node* ParseCallExpression();

  ir::Node* ParseArguments();

  ir::Node* ParseParameter();

  ir::Node* ParseMemberExpression();

  ir::Node* ParseGetPropOrElem(ir::Node* node);

  ir::Node* ParsePrimaryExpression();

  ir::Node* ParseTypedParameterOrNameExpression();

  void CheckExpression(ir::Node* expr);

  ir::Node* ParseObjectLiteral();

  ir::Node* ParseArrayLiteral();

  ir::Node* ParseLiteral();

  ir::Node* ParseFunction();

  ir::Node* ParseArrowFunction(ir::Node* args);

  ir::Node* ParseFunctionBody();

  ir::Node* ParseFormalParameterList();

  ir::Node* ParseCallSignature();

  ir::Node* ParseObjectKey();

  void ParseTypeExpressionAfterName(ir::Node* expr);

  void ConvertExpressionToParameter(ir::Node* args, ir::Node* param_list);

  void EnablePrintParsePhase() {print_parser_phase_ = true;};

  void DisablePrintParsePhase() {print_parser_phase_ = false;};
  
 private:

  class ParserState: private Unmovable, private Uncopyable {
   public:
    ParserState() {}

#define SCOPE(name, bit)                                              \
    class name: private Unmovable, private Uncopyable {               \
     public:                                                          \
     name(ParserState* parser_state)      \
         : parser_state_(parser_state) {                              \
       parser_state_->bits_.set(bit);                                 \
     }                                                                \
                                                                      \
     ~name() {                                                        \
       parser_state_->bits_.reset(bit);                               \
     }                                                                \
     private:                                                         \
     ParserState* parser_state_;                                      \
    };                                                                \
    YATSC_INLINE bool IsIn##name() {                                  \
      return 1 == bits_.test(bit);                                    \
    }                                                                 \
    

    SCOPE(ArrowFunctionScope, 0);
    SCOPE(MethodScope, 1);

#undef SCOPE
    
   private:
    std::bitset<8> bits_;
  };

  YATSC_INLINE TokenInfo* Next() {
    current_token_info_ = scanner_->Scan();
    return current_token_info_;
  }


  YATSC_INLINE TokenInfo* Peek() {
    next_token_info_ = scanner_->Peek();
    return next_token_info_;
  }
  
  
  template <typename T, typename ... Args>
  T* New(Args ... args) {
    return irfactory_.New<T>(std::forward<Args>(args)...);
  }


  template <typename T>
  T* New(std::initializer_list<ir::Node*> list) {
    return irfactory_.New<T>(list);
  }

  bool print_parser_phase_;
  Scanner<UCharInputSourceIterator>* scanner_;
  TokenInfo* current_token_info_;
  TokenInfo* next_token_info_;
  ir::IRFactory irfactory_;
  ErrorReporter* error_reporter_;
  ParserState parser_state_;
};


// Syntax error exception.
class SyntaxError: public std::exception {
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

#include "./parser-inl.h"

#endif
