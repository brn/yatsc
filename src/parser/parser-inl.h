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


namespace yatsc {

#define SYNTAX_ERROR(message, token)            \
  SYNTAX_ERROR_POS(message, token->source_position())


#define SYNTAX_ERROR_POS(message, pos)          \
  (*error_reporter_) << message;                \
  error_reporter_->Throw<SyntaxError>(pos)


// Parse source string and create Node.
template <typename UCharInputIterator>
ir::Node* Parser<UCharInputIterator>::Parse() {
  // auto file_scope_view = New<FileScopeView>();
  // file_scope_view->InsertLast(ParseProgram());
  // return file_scope_view;
  return nullptr;
}


// Parse program.
template <typename UCharInputIterator>
ir::Node* Parser<UCharInputIterator>::ParseProgram() {
  // ir::Node* source_elements = ParseSourceElements();
  // return source_elements;
  return nullptr;
}


// Parse root statements.
template <typename UCharInputIterator>
ir::Node* Parser<UCharInputIterator>::ParseSourceElements() {
  // BlockView* block_view = New<BlockView>();
  
  // while (1) {
  //   const TokenInfo* token_info = Peek();
  //   if (Token::END_OF_INPUT == token_info) {
  //     break;
  //   }
  //   ir::Node* statement = ParseSourceElement();
  //   block_view->InsertLast(statement);
  // }
  
  // return block_view;
  return nullptr;
}


// Parse root statement.
template <typename UCharInputIterator>
ir::Node* Parser<UCharInputIterator>::ParseSourceElement() {
  // ir::Node* result = nullptr;
  // const TokenInfo* token_info = Peek();
  // switch (token_info->type()) {
  //   case Token::TS_CLASS: {
  //     result = ParseClassDeclaration();
  //   }
  //     break;
      
  //   case Token::TS_FUNCTION: {
  //     result = ParseFunction();
  //   }
  //     break;

  //   case Token::TS_VAR: {
  //     result = ParseVariableDeclaration();
  //   }
  //     break;

  //   case Token::END_OF_INPUT: {
  //     SYNTAX_ERROR("Unexpected end of input.", token_info);
  //   }
  //     break;

  //   default:
  //     result = ParseStatement();
  // }
  // return result;
  return nullptr;
}


template <typename UCharInputIterator>
ir::Node* Parser<UCharInputIterator>::ParseStatementList() {
  return nullptr;
}


template <typename UCharInputIterator>
ir::Node* Parser<UCharInputIterator>::ParseStatement() {
  // ir::Node* result = nullptr;
  
  // const TokenInfo* token_info = Peek();
  // switch (token_info->type()) {
  //   case Token::TS_LEFT_BRACE:
  //     result = ParseBlockStatement();
  //     break;

  //   case Token::TS_MODULE:
  //     result = ParseModuleStatement();
  //     break;

  //   case Token::TS_EXPORT:
  //     result = ParseExportStatement();
  //     break;

  //   case Token::TS_VAR:
  //     result = ParseVariableDeclaration();
  //     break;

  //   case Token::TS_IF:
  //     result = ParseIfStatement();
  //     break;

  //   case Token::TS_FOR:
  //     result = ParseForStatement();
  //     break;

  //   case Token::TS_WHILE:
  //     result = ParseWhileStatement();
  //     break;

  //   case Token::TS_DO:
  //     result = ParseDoWhileStatement();
  //     break;

  //   case Token::TS_CONTINUE:
  //     result = ParseContinueStatement();
  //     break;

  //   case Token::TS_BREAK:
  //     result = ParseBreakStatement();
  //     break;

  //   case Token::TS_RETURN:
  //     result = ParseReturnStatement();
  //     break;

  //   case Token::TS_WITH:
  //     result = ParseWithStatement();
  //     break;

  //   case Token::TS_SWITCH:
  //     result = ParseSwitchStatement();
  //     break;

  //   case Token::TS_THROW:
  //     result = ParseThrowStatement();
  //     break;

  //   case Token::TS_TRY:
  //     result = ParseTryStatement();
  //     break;

  //   case Token::TS_IMPORT:
  //     result = ParseImportStatement();
  //     break;

  //   case Token::TS_FUNCTION:
  //     result = ParseFunction();
  //     break;

  //   case Token::TS_DEBUGGER:
  //     result = ParseDebuggerStatement();
  //     break;

  //   case Token::END_OF_INPUT:
  //     SYNTAX_ERROR("Unexpected end of input.", token_info);
      
  //   default: {
  //     SYNTAX_ERROR("Illegal token.", token_info);
  //   }
  // }
  
  // return result;
  return nullptr;
}


template <typename UCharInputIterator>
ir::Node* Parser<UCharInputIterator>::ParseBlockStatement() {
  // const TokenInfo* token_info = Seek();
  // BlockView* block_view = New<BlockView>();

  // if (token_info->type() == Token::TS_LEFT_BRACE) {
  //   while (1) {
  //     ir::Node* statement = ParseSourceElement();
  //     block_view->InsertLast(statement);
  //     token_info = Seek();
  //     if (token_info->type() == Token::TS_RIGHT_BRACE) {
  //       Scan();
  //       break;
  //     } else if (token->type() == Token::END_OF_INPUT) {
  //       SYNTAX_ERROR("Unexpected end of input.", token_info);
  //     }
  //   }
  //   return block_view;
  // }
  // SYNTAX_ERROR("Illegal token " << token_info->value()->ToUtf8Value());
  return nullptr;
}


template <typename UCharInputIterator>
ir::Node* Parser<UCharInputIterator>::ParseModuleStatement() {
  return nullptr;
}


template <typename UCharInputIterator>
ir::Node* Parser<UCharInputIterator>::ParseImportStatement() {
  return nullptr;
}


template <typename UCharInputIterator>
ir::Node* Parser<UCharInputIterator>::ParseExportStatement()  {
  return nullptr;
}


template <typename UCharInputIterator>
ir::Node* Parser<UCharInputIterator>::ParseDebuggerStatement() {
  return nullptr;
}


template <typename UCharInputIterator>
ir::Node* Parser<UCharInputIterator>::ParseVariableDeclaration(bool noin) {
  return nullptr;
}


template <typename UCharInputIterator>
ir::Node* Parser<UCharInputIterator>::ParseIfStatement() {
  return nullptr;
}


template <typename UCharInputIterator>
ir::Node* Parser<UCharInputIterator>::ParseWhileStatement() {
  return nullptr;
}


template <typename UCharInputIterator>
ir::Node* Parser<UCharInputIterator>::ParseDoWhileStatement() {
  return nullptr;
}


template <typename UCharInputIterator>
ir::Node* Parser<UCharInputIterator>::ParseForStatement() {
  return nullptr;
}


template <typename UCharInputIterator>
ir::Node* Parser<UCharInputIterator>::ParseContinueStatement() {
  return nullptr;
}


template <typename UCharInputIterator>
ir::Node* Parser<UCharInputIterator>::ParseBreakStatement() {
  return nullptr;
}


template <typename UCharInputIterator>
ir::Node* Parser<UCharInputIterator>::ParseReturnStatement() {
  return nullptr;
}


template <typename UCharInputIterator>
ir::Node* Parser<UCharInputIterator>::ParseWithStatement() {
  return nullptr;
}


template <typename UCharInputIterator>
ir::Node* Parser<UCharInputIterator>::ParseSwitchStatement() {
  return nullptr;
}


template <typename UCharInputIterator>
ir::Node* Parser<UCharInputIterator>::ParseCaseClauses() {
  return nullptr;
}


template <typename UCharInputIterator>
ir::Node* Parser<UCharInputIterator>::ParseLabelledStatement() {
  return nullptr;
}


template <typename UCharInputIterator>
ir::Node* Parser<UCharInputIterator>::ParseThrowStatement() {
  return nullptr;
}


template <typename UCharInputIterator>
ir::Node* Parser<UCharInputIterator>::ParseTryStatement() {
  return nullptr;
}


template <typename UCharInputIterator>
ir::Node* Parser<UCharInputIterator>::ParseCatchBlock() {
  return nullptr;
}


template <typename UCharInputIterator>
ir::Node* Parser<UCharInputIterator>::ParseFinallyBlock() {
  return nullptr;
}


template <typename UCharInputIterator>
ir::Node* Parser<UCharInputIterator>::ParseClassDeclaration() {
  return nullptr;
}


template <typename UCharInputIterator>
ir::Node* Parser<UCharInputIterator>::ParseClassFields() {
  return nullptr;
}


template <typename UCharInputIterator>
ir::Node* Parser<UCharInputIterator>::ParseExpression(bool noin) {
  ir::Node* assignment_expr = ParseAssignmentExpression(noin);
  if (Peek()->type() == Token::TS_COMMA) {
    Next();
    ir::CommaExprView* comma_expr = New<ir::CommaExprView>({assignment_expr});
    while (1) {
      assignment_expr = ParseAssignmentExpression(noin);
      comma_expr->InsertLast(assignment_expr);
      if (Peek()->type() == Token::TS_COMMA) {
        Next();
      } else {
        return comma_expr;
      }
    }
  }
  return assignment_expr;
}


bool IsAssignmentOp(Token type) {
  return type == Token::TS_ASSIGN || type == Token::TS_MUL_LET ||
    type == Token::TS_DIV_LET || type == Token::TS_MOD_LET ||
    type == Token::TS_ADD_LET || type == Token::TS_SUB_LET ||
    type == Token::TS_SHIFT_LEFT_LET || type == Token::TS_SHIFT_RIGHT_LET ||
    type == Token::TS_U_SHIFT_RIGHT_LET || type == Token::TS_AND_LET ||
    type == Token::TS_NOR_LET || type == Token::TS_OR_LET;
}


template <typename UCharInputIterator>
ir::Node* Parser<UCharInputIterator>::ParseAssignmentExpression(bool noin) {
  ir::Node* expr = ParseConditionalExpression(noin);
  TokenInfo *token_info = Peek();
  Token type = token_info->type();
  if (IsAssignmentOp(type)) {
    Next();
    if (expr->IsValidLhs()) {
      ir::Node* rhs = ParseAssignmentExpression(noin);
      return New<ir::AssignmentView>(type, expr, rhs);
    }
    SYNTAX_ERROR("invalid left hand side expression in 'assignment expression'", current_token_info_);
  }
  return expr;
}


template <typename UCharInputIterator>
ir::Node* Parser<UCharInputIterator>::ParseYieldExpression() {
  return nullptr;
}


// Parse condition expression.
// conditional_expression
// 	: logical_or_expression
// 	| logical_or_expression '?' assignment_expression ':' assignment_expression
// 	;

// conditional_expression_no_in
// 	: logical_or_expression_no_in
// 	| logical_or_expression_no_in '?' assignment_expression_no_in ':' assignment_expression_no_in
// 	;
template <typename UCharInputIterator>
ir::Node* Parser<UCharInputIterator>::ParseConditionalExpression(bool noin) {
  ir::Node* expr = ParseBinaryExpression(noin);
  TokenInfo* ti = Peek();
  if (ti->type() == Token::TS_QUESTION_MARK) {
    Next();
    if (Peek()->type() == Token::TS_COLON) {
      ParseTypeExpressionAfterName(expr);
      return New<ir::OptionalParamView>(expr);
    }
    ir::Node* left = ParseAssignmentExpression(noin);
    ti = Peek();
    if (ti->type() == Token::TS_COLON) {
      Next();
      ir::Node* right = ParseAssignmentExpression(noin);
      ir::TemaryExprView* temary = New<ir::TemaryExprView>(expr, left, right);
      temary->MarkAsInValidLhs();
      return temary;
    }
    SYNTAX_ERROR("SyntaxError unexpected token in 'temary expression'", next_token_info_);
  }
  return expr;
}


// multiplicative_expression
// 	: unary_expression
// 	| multiplicative_expression '*' unary_expression
// 	| multiplicative_expression '/' unary_expression
// 	| multiplicative_expression '%' unary_expression
// 	;

// additive_expression
// 	: multiplicative_expression
// 	| additive_expression '+' multiplicative_expression
// 	| additive_expression '-' multiplicative_expression
// 	;

// shift_expression
// 	: additive_expression
// 	| shift_expression SHIFT_LEFT additive_expression
// 	| shift_expression SHIFT_RIGHT additive_expression
// 	| shift_expression U_SHIFT_RIGHT additive_expression
// 	;

// relational_expression
// 	: shift_expression
// 	| relational_expression '<' shift_expression
// 	| relational_expression '>' shift_expression
// 	| relational_expression LESS_EQUAL shift_expression
// 	| relational_expression GRATER_EQUAL shift_expression
// 	| relational_expression INSTANCEOF shift_expression
// 	| relational_expression IN shift_expression
// 	;

// relational_expression_no_in
// 	: shift_expression
// 	| relational_expression_no_in '<' shift_expression
// 	| relational_expression_no_in '>' shift_expression
// 	| relational_expression_no_in LESS_EQUAL shift_expression
// 	| relational_expression_no_in GRATER_EQUAL shift_expression
// 	| relational_expression_no_in INSTANCEOF shift_expression
// 	;

// equality_expression
// 	: relational_expression
// 	| equality_expression EQUAL relational_expression
// 	| equality_expression NOT_EQUAL relational_expression
// 	| equality_expression EQ relational_expression
// 	| equality_expression NOT_EQ relational_expression
// 	;

// equality_expression_no_in
// 	: relational_expression_no_in
// 	| equality_expression_no_in EQUAL relational_expression_no_in
// 	| equality_expression_no_in NOT_EQUAL relational_expression_no_in
// 	| equality_expression_no_in EQ relational_expression_no_in
// 	| equality_expression_no_in NOT_EQ relational_expression_no_in
// 	;

// bitwise_and_expression
// 	: equality_expression
// 	| bitwise_and_expression '&' equality_expression
// 	;

// bitwise_and_expression_no_in
// 	: equality_expression_no_in
// 	| bitwise_and_expression_no_in '&' equality_expression_no_in
// 	;

// bitwise_xor_expression
// 	: bitwise_and_expression
// 	| bitwise_xor_expression '^' bitwise_and_expression
// 	;

// bitwise_xor_expression_no_in
// 	: bitwise_and_expression_no_in
// 	| bitwise_xor_expression_no_in '^' bitwise_and_expression_no_in
// 	;

// bitwise_or_expression
// 	: bitwise_xor_expression
// 	| bitwise_or_expression '|' bitwise_xor_expression
// 	;

// bitwise_or_expression_no_in
// 	: bitwise_xor_expression_no_in
// 	| bitwise_or_expression_no_in '|' bitwise_xor_expression_no_in
// 	;

// logical_and_expression
// 	: bitwise_or_expression
// 	| logical_and_expression LOGICAL_AND bitwise_or_expression
// 	;

// logical_and_expression_no_in
// 	: bitwise_or_expression_no_in
// 	| logical_and_expression_no_in LOGICAL_AND bitwise_or_expression_no_in
// 	;

// logical_or_expression
// 	: logical_and_expression
// 	| logical_or_expression LOGICAL_OR logical_and_expression
// 	;

// logical_or_expression_no_in
// 	: logical_and_expression_no_in
// 	| logical_or_expression_no_in LOGICAL_OR logical_and_expression_no_in
// 	;
template <typename UCharInputIterator>
ir::Node* Parser<UCharInputIterator>::ParseBinaryExpression(bool noin) {
  ir::Node* last = nullptr;
  ir::Node* first = nullptr;
  ir::Node* lhs = ParseUnaryExpression();
  ir::Node* expr = nullptr;
  while (1) {
    TokenInfo* token = Peek();
    Token type = token->type();
    switch (token->type()) {
      case Token::TS_LOGICAL_AND :
      case Token::TS_LOGICAL_OR :
      case Token::TS_EQUAL :
      case Token::TS_NOT_EQUAL :
      case Token::TS_EQ :
      case Token::TS_NOT_EQ :
      case Token::TS_LESS_EQUAL :
      case Token::TS_GREATER_EQUAL :
      case Token::TS_INSTANCEOF :
      case Token::TS_LESS :
      case Token::TS_GREATER : {
        Next();
        ir::Node* rhs = ParseUnaryExpression();
        if (last == nullptr) {
          expr = New<ir::BinaryExprView>(type, lhs, rhs);
          first = expr;
        } else {
          expr = New<ir::BinaryExprView>(type, last, rhs);
        }
        expr->MarkAsInValidLhs();
        last = expr;
      }
        break;

      case Token::TS_IN : {
        if (!noin) {
          Next();
          ir::Node* rhs = ParseUnaryExpression();
          if (last == nullptr) {
            expr = New<ir::BinaryExprView>(type, lhs, rhs);
            first = expr;
          } else {
            expr = New<ir::BinaryExprView>(type, last, rhs);
          }
          expr->MarkAsInValidLhs();
          last = expr;
        } else {
          return (first == nullptr)? lhs : expr;
        }
      }
        break;
        
      case Token::TS_BIT_OR :
      case Token::TS_BIT_XOR :
      case Token::TS_BIT_AND :
      case Token::TS_PLUS :
      case Token::TS_MINUS :
      case Token::TS_MUL :
      case Token::TS_DIV :
      case Token::TS_MOD :
      case Token::TS_SHIFT_LEFT :
      case Token::TS_SHIFT_RIGHT :
      case Token::TS_U_SHIFT_RIGHT :{
        Next();
        ir::Node* rhs = ParseUnaryExpression();
        if (last == nullptr) {
          expr = New<ir::BinaryExprView>(type, lhs, rhs);
          first = expr;
        } else {
          expr = New<ir::BinaryExprView>(type, last, rhs);
        }
        expr->MarkAsInValidLhs();
        last = expr;
      }
        break;
                                                                                                                                
      default :
        return (expr == nullptr)? lhs : expr;
    }
  }
}


template <typename UCharInputIterator>
ir::Node* Parser<UCharInputIterator>::ParseUnaryExpression() {
  Token type = Peek()->type();
  switch (type) {
    case Token::TS_DELETE:
    case Token::TS_VOID:
    case Token::TS_TYPEOF:
    case Token::TS_INCREMENT:
    case Token::TS_DECREMENT:
    case Token::TS_PLUS:
    case Token::TS_MINUS:
    case Token::TS_BIT_NOR:
    case Token::TS_NOT: {
      Next();
      ir::Node* node = ParseUnaryExpression();
      return New<ir::UnaryExprView>(type, node);
    }
    default:
      return ParsePostfixExpression();
  }
}


template <typename UCharInputIterator>
ir::Node* Parser<UCharInputIterator>::ParsePostfixExpression() {
  ir::Node* node = ParseLeftHandSideExpression();
  if (Peek()->type() == Token::TS_INCREMENT) {
    return New<ir::PostfixView>(node, Token::TS_INCREMENT);
  } else if (Peek()->type() == Token::TS_DECREMENT) {
    return New<ir::PostfixView>(node, Token::TS_DECREMENT);
  }
  return node;
}


// Parse left-hand-side-expression
// left_hand_side_expression
// 	: new_expression(omited) -> member_expression
// 	| call_expression
// 	;
template <typename UCharInputIterator>
ir::Node* Parser<UCharInputIterator>::ParseLeftHandSideExpression() {
  if (Peek()->type() == Token::TS_NEW) {
    return ParseMemberExpression();
  }
  return ParseCallExpression();
}


template <typename UCharInputIterator>
ir::Node* Parser<UCharInputIterator>::ParseTypeExpression() {
  if (Peek()->type() == Token::TS_IDENTIFIER) {
    ir::Node* node = ParseLiteral();
    if (Peek()->type() == Token::TS_LEFT_BRACKET) {
      Next();
      if (Peek()->type() == Token::TS_RIGHT_BRACKET) {
        Next();
        return New<ir::ArrayTypeExprView>(node);
      }
      SYNTAX_ERROR("SyntaxError ']' expected", next_token_info_);
    } else {
      return New<ir::SimpleTypeExprView>(node);
    }
  } else if (Peek()->type() == Token::TS_LEFT_PAREN) {
    ParserState::ArrowFunctionScope scope(&parser_state_);
    ir::Node* types = ParseParameter(false);
    if (Peek()->type() == Token::TS_FUNCTION_GLYPH) {
      Next();
      ir::Node* ret_type = ParseTypeExpression();
      return New<ir::FunctionTypeExprView>(types, ret_type);
    }
    SYNTAX_ERROR("SyntaxError '=>' expected", next_token_info_);
  } else if (Peek()->type() == Token::TS_LEFT_BRACE) {
    return ParseObjectLiteral();
  } else if (Peek()->type() == Token::TS_LEFT_BRACKET) {
    Next();
    if (Peek()->type() == Token::TS_IDENTIFIER) {
      ir::Node* name = ParseLiteral();
      if (Peek()->type() == Token::TS_COLON) {
        Next();
        ir::Node* type = ParseTypeExpression();
        return New<ir::AccessorTypeExprView>(name, type);
      }
      SYNTAX_ERROR("SyntaxError ':' expected", next_token_info_);
    }
    SYNTAX_ERROR("SyntaxError identifier expected", next_token_info_);
  }
}


// Parse call expression.
// call_expression
// 	: member_expression arguments
// 	| call_expression arguments
// 	| call_expression '[' expression ']'
// 	| call_expression '.' IDENTIFIER
template <typename UCharInputIterator>
ir::Node* Parser<UCharInputIterator>::ParseCallExpression() {
  ir::Node* target = ParseMemberExpression();
  if (Peek()->type() == Token::TS_LEFT_PAREN) {
    ir::Node* args = ParseArguments();
    ir::Node* call = New<ir::CallView>(target, args);
    while (1) {
      switch (Peek()->type()) {
        case Token::TS_LEFT_PAREN: {
          ir::Node* args = ParseArguments();
          call = New<ir::CallView>(call, args);
          break;
        }
        case Token::TS_LEFT_BRACE:
        case Token::TS_DOT:
          call = ParseGetPropOrElem(call);
          break;
        default:
          return call;
      }
    }
  }
  return target;
}


// Parse arguments and arguments-list
// arguments
// 	: '(' ')'
// 	| '(' argument_list ')'
// 	;

// argument_list
// 	: assignment_expression
// 	| argument_list ',' assignment_expression
// 	;
template <typename UCharInputIterator>
ir::Node* Parser<UCharInputIterator>::ParseArguments() {
  if (Next()->type() == Token::TS_LEFT_PAREN) {
    ir::CallArgsView* args = New<ir::CallArgsView>();
    while (1) {
      args->InsertLast(ParseAssignmentExpression(false));
      Next();
      if (current_token_info_->type() == Token::TS_COMMA) {
        continue;
      } else if (current_token_info_->type() == Token::TS_RIGHT_BRACE) {
        return args;
      }
      SYNTAX_ERROR("SyntaxError unexpected token in 'arguments'", current_token_info_);
    }
  }
  return nullptr;
}


// Parse member-expression
// bnf: member_expression
// 	    : primary_expression
// 	    | function_expression
// 	    | member_expression '[' expression ']'
// 	    | member_expression '.' IDENTIFIER
// 	    | NEW member_expression arguments
// 	    ;
template <typename UCharInputIterator>
ir::Node* Parser<UCharInputIterator>::ParseMemberExpression() {
  // Not advance scanner.
  TokenInfo* token_info = Peek();
  ir::Node* node;
  if (token_info->type() ==  Token::TS_NEW) {
    // Parse new Foo() expression.
    Next();
    ir::Node* member = ParseMemberExpression();

    // New expression can omit parens.
    // If paren exists, continue parsing.
    if (Peek()->type() == Token::TS_LEFT_PAREN) {
      ir::Node* args = ParseArguments();
      node = New<ir::NewCallView>(member, args);
      return ParseGetPropOrElem(node);
    } else {
      // Parens are not exists.
      // Immediate return.
      return New<ir::NewCallView>(member, nullptr);
    }

  } else {
    if (token_info->type() == Token::TS_FUNCTION) {
      // Parse function.
      node = ParseFunction();
    } else {
      node = ParsePrimaryExpression();
    }
    return ParseGetPropOrElem(node);
  }
}


// Parse member expression suffix.
// Like 'new foo.bar.baz()', 'new foo["bar"]', '(function(){return {a:1}}).a'
template <typename UCharInputIterator>
ir::Node* Parser<UCharInputIterator>::ParseGetPropOrElem(ir::Node* node) {
  while (1) {
    switch (Peek()->type()) {
      case Token::TS_LEFT_BRACKET: {
        // [...] expression.
        Next();
        ir::Node* expr = ParseExpression(false);
        node = New<ir::GetElemView>(node, expr);
        if (Next()->type() != Token::TS_RIGHT_BRACKET) {
          SYNTAX_ERROR("Unexpected token.", current_token_info_);
        }
        break;
      }
      case Token::TS_DOT: {
        // a.b.c expression.
        Next();
        ir::Node* expr = ParseExpression(false);
        node = New<ir::GetPropView>(node, expr);
        break;
      }
      default:
        return node;
    }
  }
  SYNTAX_ERROR("SyntaxError", current_token_info_);
}


// Parse primary-expression.
// bnf: primary_expression
//      : THIS
//      | IDENTIFIER
//      | literal
//      | array_literal
//      | object_literal
//      | '(' expression ')'
//      ;
template <typename UCharInputIterator>
ir::Node* Parser<UCharInputIterator>::ParsePrimaryExpression() {
  // Not advance scanner.
  TokenInfo* token_info = Peek();
  switch (token_info->type()) {
    case Token::TS_IDENTIFIER: {
      // parse an identifier.
      ir::NameView* name = New<ir::NameView>(token_info->value());
      Next();
      ParseTypeExpressionAfterName(name);
      return name;
    }
    case Token::TS_THIS:
      // parse a this.
      Next();
      return New<ir::ThisView>();
    case Token::TS_LEFT_BRACE:
      // parse an object literal.
      return ParseObjectLiteral();
    case Token::TS_LEFT_BRACKET:
      // parse an array literal.
      return ParseArrayLiteral();
    case Token::TS_LEFT_PAREN: {
      ParserState::ArrowFunctionScope scope(&parser_state_);
      // parse an expression that beggining '('
      ir::Node* node = ParseExpression(false);
      if (Next()->type() != Token::TS_RIGHT_PAREN) {
        SYNTAX_ERROR("Unexpected token.", current_token_info_);
      }
      if (Peek()->type() == Token::TS_FUNCTION_GLYPH) {
        Next();
        return ParseArrowFunction(node);
      } else {
        CheckExpression(node);
      }
      return node;
    }
    default:
      // parse a literal.
      return ParseLiteral();
  }
}


template <typename UCharInputIterator>
ir::Node* Parser<UCharInputIterator>::ParseTypeExpressionAfterName(ir::Node* name) {
  if (Peek()->type() == Token::TS_COLON ) {
    if (!name->HasNameView()) {
      SYNTAX_ERROR_POS("SyntaxError invalid expression", name->source_position());
    }
    ir::Node name_view = name->ToNameView();
    if (!parser_state_.IsInArrowFunctionScope()) {
      SYNTAX_ERROR("SyntaxError unexpected token", next_token_info_);
    }
    Next();
    ir::Node* type = ParseTypeExpression();
    name_view->set_type_expr(type);
  }
}


template <typename UCharInputIterator>
ir::Node* Parser<UCharInputIterator>::CheckExpression(ir::Node* node) {
  for (auto c: node->node_list()) {
    ir::Node* p = *c;
    if (p != nullptr) {
      if (p->HasOptionalParamView()) {
        SYNTAX_ERROR_POS("SyntaxError unexpected '?'", p->source_position());
      } else if (p->HasNameView()) {
        ir::NameView* name = p->ToNameView();
        if (nullptr != name->type_expr()) {
          SYNTAX_ERROR_POS("SyntaxError unexpected ':'", p->source_position());
        }
      } else if (p->HasAssignmentView()) {
        ir::AssignmentView* assignment = p->ToAssignmentView();
        if (nullptr != assignment->target() && assignment->target()->HasNameView()) {
          ir::NameView* name = assignment->target()->ToNameView();
          if (nullptr != name->type_expr()) {
            SYNTAX_ERROR_POS("SyntaxError unexpected ':'", p->source_position());
          }
        }
      }
    }
  }
}


template <typename UCharInputIterator>
ir::Node* Parser<UCharInputIterator>::ParseObjectLiteral() {
  Next();
  if (current_token_info_->type() == Token::TS_LEFT_BRACE) {
    ir::ObjectLiteralView* object_literal = New<ir::ObjectLiteralView>();
    while (current_token_info_->type() != Token::TS_RIGHT_BRACE) {
      Next();
      ir::Node* key = nullptr;
      if (current_token_info_->type() == Token::TS_IDENTIFIER) {
        key = New<ir::NameView>(current_token_info_->value());
      } else if (current_token_info_->type() == Token::TS_STRING_LITERAL) {
        key = New<ir::StringView>(current_token_info_->value());
      } else if (current_token_info_->type() == Token::TS_NUMERIC_LITERAL) {
        key = New<ir::NumberView>(current_token_info_->value());
      } else {
        SYNTAX_ERROR("Invalid ObjectLiteral key.", current_token_info_);
      }
      Next();
      if (current_token_info_->type() != Token::TS_COMMA ||
          current_token_info_->type() != Token::TS_RIGHT_BRACE) {
        SYNTAX_ERROR("SyntaxError expected ','", current_token_info_);
      }
      ir::Node* value = ParseAssignmentExpression(false);
      ir::ObjectElementView* element = New<ir::ObjectElementView>(key, value);
      object_literal->InsertLast(element);
    }
    return object_literal;
  }
  SYNTAX_ERROR("Unexpected token.", current_token_info_);
}


template <typename UCharInputIterator>
ir::Node* Parser<UCharInputIterator>::ParseArrayLiteral() {
  Next();
  if (current_token_info_->type() == Token::TS_LEFT_BRACKET) {
    ir::ArrayLiteralView* array_literal = New<ir::ArrayLiteralView>();
    while (current_token_info_->type() != Token::TS_RIGHT_BRACKET) {
      ir::Node* expr = ParseAssignmentExpression(false);
      array_literal->InsertLast(expr);
      Next();
      if (current_token_info_->type() != Token::TS_COMMA &&
          current_token_info_->type() != Token::TS_RIGHT_BRACKET) {
        SYNTAX_ERROR("SyntaxError unexpected token in 'array literal'", current_token_info_);
      }
    }
    return array_literal;
  }
  SYNTAX_ERROR("Unexpected token.", current_token_info_);
}


template <typename UCharInputIterator>
ir::Node* Parser<UCharInputIterator>::ParseLiteral() {
  // Allow regular expression in this context.
  Scanner<UCharInputIterator>::RegularExpressionScope scope(scanner_);
  // Advance scanner.
  Next();
  switch(current_token_info_->type()) {
    case Token::TS_TRUE:
      // true value
      return New<ir::TrueView>();
    case Token::TS_FALSE:
      // false value
      return New<ir::FalseView>();
    case Token::TS_STRING_LITERAL:
      // 'aaaa' value
      return New<ir::StringView>(current_token_info_->value());
    case Token::TS_NUMERIC_LITERAL:
      // 1234.5 value
      return New<ir::NumberView>(current_token_info_->value());
    case Token::TS_REGULAR_EXPR:
      // /aaaaa/ value
      return New<ir::RegularExprView>(current_token_info_->value());
    case Token::TS_UNDEFINED:
      // undefined value
      return New<ir::UndefinedView>();
    case Token::TS_NULL:
      // null value
      return New<ir::NullView>();
    case Token::TS_NAN:
      // NaN value
      return New<ir::NaNView>();
    default:
      // Any other token cause SyntaxError.
      SYNTAX_ERROR("SyntaxError literal expected", current_token_info_);
  }
}


template <typename UCharInputIterator>
ir::Node* Parser<UCharInputIterator>::ParseParameter() {
  if (Next()->type() == Token::TS_LEFT_PAREN) {
    ir::ParamList* param_list = New<ir::ParamList>();
    while (1) {
      ir::Node* expr = ParseAssignmentExpression();
      param_list->InsertLast(expr);
      if (Peek()->type() == Token::TS_COMMA) {
        Next();
      } else if (Peek()->type() == Token::TS_RIGHT_PAREN) {
        Next();
        return param_list;
      } else {
        SYNTAX_ERROR("SyntaxError in formal parameter list", next_token_info_);
      }
    }
  }
}


template <typename UCharInputIterator>
ir::Node* Parser<UCharInputIterator>::ParseFunction() {
  if (Peek()->type() == Token::TS_FUNCTION) {
    Next();
    ir::Node* name;
    if (Peek()->type() == Token::TS_IDENTIFIER) {
      name = ParseLiteral();
    }
    ir::Node* param_list = ParseParameter();
    ir::Node* body = ParseFunctionBody();
    return New<ir::FunctionView>(name, param_list, body);
  }
}


template <typename UCharInputIterator>
ir::Node* Parser<UCharInputIterator>::ParseArrowFunction(ir::Node* args) {
  ir::ParamList* param_list = New<ir::ParamList>();
  for (auto c: args->node_list()) {
    ir::Node* arg = *c;
    if (!arg->HasAssignmentView() &&
        !arg->HasOptionalParamView() &&
        !arg->HasNameView()) {
      SYNTAX_ERROR("")
    }
    param_list->InsertLast(*c);
  }
  ir::Node* body;
  if (Peek()->type() == Token::TS_LEFT_BRACE) {
    body = ParseFunctionBody();
  } else {
    body = ParseAssignmentExpression();
  }
  return New<ir::FunctionView>(nullptr, param_list, body);
}


template <typename UCharInputIterator>
ir::Node* Parser<UCharInputIterator>::ParseFunctionBody() {
  if (Peek()->type() == Token::TS_LEFT_BRACE) {
    Next();
    auto block = New<ir::BlockView>();
    while (1) {
      auto node = ParseSourceElement();
      block->InsertLast(node);
      if (Peek()->type() == Token::TS_RIGHT_BRACE) {
        break;
      }
    }
    return block;
  }
  SYNTAX_ERROR("SyntaxError unexpected token in 'function body'", next_token_info_);
}


template <typename UCharInputIterator>
ir::Node* Parser<UCharInputIterator>::ParseFormalParameterList() {
  return nullptr;
}

}
