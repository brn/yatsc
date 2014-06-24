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

#include "./parser.h"

namespace yatsc {

#define SYNTAX_ERROR(message, token)            \
  (*error_reporter_) << message;                \
  error_reporter_->Throw(token->source_position())


// Parse source string and create Node.
Node* Parser::Parse() {
  auto file_scope_view = New<FileScopeView>();
  file_scope_view->InsertLast(ParseProgram());
  return file_scope_view;
}


// Parse program.
Node* Parser::ParseProgram() {
  Node* source_elements = ParseSourceElements();
  return source_elements;
}


// Parse root statements.
Node* Parser::ParseSourceElements() {
  BlockView* block_view = New<BlockView>();
  
  while (1) {
    const TokenInfo* token_info = Peek();
    if (Token::END_OF_INPUT == token_info) {
      break;
    }
    Node* statement = ParseSourceElement();
    block_view->InsertLast(statement);
  }
  
  return block_view;
}


// Parse root statement.
Node* Parser::ParseSourceElement() {
  Node* result = nullptr;
  const TokenInfo* token_info = Peek();
  switch (token_info->type()) {
    case Token::TS_CLASS: {
      result = ParseClassDeclaration();
    }
      break;
      
    case Token::TS_FUNCTION: {
      result = ParseFunction();
    }
      break;

    case Token::TS_VAR: {
      result = ParseVariableDeclaration();
    }
      break;

    case Token::END_OF_INPUT: {
      SYNTAX_ERROR("Unexpected end of input.", token_info);
    }
      break;

    default:
      result = ParseStatement();
  }
  return result;
}


Node* Parser::ParseStatementList() {
  
}

Node* Parser::ParseStatement() {
  Node* result = nullptr;
  
  const TokenInfo* token_info = Peek();
  switch (token_info->type()) {
    case Token::TS_LEFT_BRACE:
      result = ParseBlockStatement();
      break;

    case Token::TS_MODULE:
      result = ParseModuleStatement();
      break;

    case Token::TS_EXPORT:
      result = ParseExportStatement();
      break;

    case Token::TS_VAR:
      result = ParseVariableDeclaration();
      break;

    case Token::TS_IF:
      result = ParseIfStatement();
      break;

    case Token::TS_FOR:
      result = ParseForStatement();
      break;

    case Token::TS_WHILE:
      result = ParseWhileStatement();
      break;

    case Token::TS_DO:
      result = ParseDoWhileStatement();
      break;

    case Token::TS_CONTINUE:
      result = ParseContinueStatement();
      break;

    case Token::TS_BREAK:
      result = ParseBreakStatement();
      break;

    case Token::TS_RETURN:
      result = ParseReturnStatement();
      break;

    case Token::TS_WITH:
      result = ParseWithStatement();
      break;

    case Token::TS_SWITCH:
      result = ParseSwitchStatement();
      break;

    case Token::TS_THROW:
      result = ParseThrowStatement();
      break;

    case Token::TS_TRY:
      result = ParseTryStatement();
      break;

    case Token::TS_IMPORT:
      result = ParseImportStatement();
      break;

    case Token::TS_FUNCTION:
      result = ParseFunction();
      break;

    case Token::TS_DEBUGGER:
      result = ParseDebuggerStatement();
      break;

    case Token::END_OF_INPUT:
      SYNTAX_ERROR("Unexpected end of input.", token_info);
      
    default: {
      SYNTAX_ERROR("Illegal token.", token_info);
    }
  }
  
  return result;
}

Node* Parser::ParseBlockStatement() {
  const TokenInfo* token_info = Seek();
  BlockView* block_view = New<BlockView>();

  if (token_info->type() == Token::TS_LEFT_BRACE) {
    while (1) {
      Node* statement = ParseSourceElement();
      block_view->InsertLast(statement);
      token_info = Seek();
      if (token_info->type() == Token::TS_RIGHT_BRACE) {
        Scan();
        break;
      } else if (token->type() == Token::END_OF_INPUT) {
        SYNTAX_ERROR("Unexpected end of input.", token_info);
      }
    }
    return block_view;
  }
  SYNTAX_ERROR("Illegal token " << token_info->value()->ToUtf8Value());
}

Node* Parser::ParseModuleStatement() {
  return nullptr;
}

Node* Parser::ParseImportStatement() {
  return nullptr;
}

Node* Parser::ParseExportStatement()  {
  return nullptr;
}

Node* Parser::ParseDebuggerStatement() {
  return nullptr;
}

Node* Parser::ParseVariableDeclaration(bool noin) {
  return nullptr;
}

Node* Parser::ParseIfStatement() {
  return nullptr;
}

Node* Parser::ParseWhileStatement() {
  return nullptr;
}

Node* Parser::ParseDoWhileStatement() {
  return nullptr;
}

Node* Parser::ParseForStatement() {
  return nullptr;
}

Node* Parser::ParseContinueStatement() {
  return nullptr;
}

Node* Parser::ParseBreakStatement() {
  return nullptr;
}

Node* Parser::ParseReturnStatement() {
  return nullptr;
}

Node* Parser::ParseWithStatement() {
  return nullptr;
}

Node* Parser::ParseSwitchStatement() {
  return nullptr;
}

Node* Parser::ParseCaseClauses() {
  return nullptr;
}

Node* Parser::ParseLabelledStatement() {
  return nullptr;
}

Node* Parser::ParseThrowStatement() {
  return nullptr;
}

Node* Parser::ParseTryStatement() {
  return nullptr;
}

Node* Parser::ParseCatchBlock() {
  return nullptr;
}

Node* Parser::ParseFinallyBlock() {
  return nullptr;
}

Node* Parser::ParseClassDeclaration() {
  return nullptr;
}

Node* Parser::ParseClassFields() {
  return nullptr;
}

Node* Parser::ParseExpression() {
  return nullptr;
}

Node* Parser::ParseAssignmentExpression() {
  return nullptr;
}

Node* Parser::ParseYieldExpression() {
  return nullptr;
}

Node* Parser::ParseConditional() {
  return nullptr;
}

Node* Parser::ParseBinaryExpression() {
  return nullptr;
}

Node* Parser::ParseUnaryExpression() {
  return nullptr;
}

Node* Parser::ParsePostfixExpression() {
  return nullptr;
}

Node* Parser::ParseLeftHandSideExpression() {
  return nullptr;
}

Node* Parser::ParseNewExpression() {
  return nullptr;
}

Node* Parser::ParseCallExpression() {
  return nullptr;
}

Node* Parser::ParseArguments() {
  return nullptr;
}

Node* Parser::ParseMemberExpression() {
  return nullptr;
}

Node* Parser::ParsePrimaryExpression() {
  return nullptr;
}

Node* Parser::ParseObjectLiteral() {
  return nullptr;
}

Node* Parser::ParseArrayLiteral() {
  return nullptr;
}

Node* Parser::ParseLiteral() {
  return nullptr;
}

Node* Parser::ParseFunction() {
  return nullptr;
}

Node* Parser::ParseFormalParameterList() {
  return nullptr;
}

}
