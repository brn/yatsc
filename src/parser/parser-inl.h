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

#define SYNTAX_ERROR(message, token)                  \
  SYNTAX_ERROR_POS(message, token->source_position())


#ifndef DEBUG
#define SYNTAX_ERROR_POS(message, pos)          \
  (*error_reporter_) << message;                \
  error_reporter_->Throw<SyntaxError>(pos)
#else
#define SYNTAX_ERROR_POS(message, pos)                          \
  (*error_reporter_) << message << '\n' << __FILE__ << ":" << __LINE__; \
  error_reporter_->Throw<SyntaxError>(pos)
#endif


#ifdef DEBUG
#define ENTER(name)                                                     \
  if (print_parser_phase_) {                                            \
    if (Current() != nullptr) {                               \
      Printf("%sEnter %s: CurrentToken = %s NextToken = %s\n", indent_.c_str(), #name, Current()->ToString(), Peek()->ToString()); \
    } else {                                                            \
      Printf("%sEnter %s: CurrentToken = null NextToken = %s\n", indent_.c_str(), #name, Peek()->ToString()); \
    }                                                                   \
  }                                                                     \
  indent_ += "  ";                                                      \
  YATSC_SCOPED([&]{                                                     \
      indent_ = indent_.substr(0, indent_.size() - 2);                  \
      if (this->print_parser_phase_) {                                  \
        if (this->Current() != nullptr) {                     \
          Printf("%sExit %s: CurrentToken = %s NextToken = %s\n", indent_.c_str(), #name, Current()->ToString(), this->Peek()->ToString()); \
        } else {                                                        \
          Printf("%sExit %s: CurrentToken = null NextToken = %s\n", indent_.c_str(), #name, this->Peek()->ToString()); \
        }                                                               \
      }                                                                 \
    })
#else
#define ENTER(name)
#endif



#define NO_RETURN return nullptr;


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


// Parse expression.
// like '(' expression ')' or assignment expression.
template <typename UCharInputIterator>
ir::Node* Parser<UCharInputIterator>::ParseExpression(bool noin) {
  ENTER(ParseExpression);
  ir::Node* assignment_expr = ParseAssignmentExpression(noin);
  if (Current()->type() == Token::TS_COMMA) {
    Next();
    ir::CommaExprView* comma_expr = New<ir::CommaExprView>({assignment_expr});
    comma_expr->SetInformationForNode(*Current());
    
    while (1) {
      assignment_expr = ParseAssignmentExpression(noin);
      comma_expr->InsertLast(assignment_expr);
      if (Current()->type() == Token::TS_COMMA) {
        Next();
      } else {
        return comma_expr;
      }
    }
  }
  
  return assignment_expr;
}


/**
 * Return true if token type is assignment operator.
 * @param type A token type.
 * @returns True if token type is assignment operator.
 */
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
  ENTER(ParseAssignmentExpression);
  ir::Node* expr = ParseConditionalExpression(noin);
  TokenInfo *token_info = Current();
  Token type = token_info->type();
  if (IsAssignmentOp(type)) {
    Next();
    if (expr->IsValidLhs()) {
      ir::Node* rhs = ParseAssignmentExpression(noin);
      ir::Node* result = New<ir::AssignmentView>(type, expr, rhs);
      result->SetInformationForNode(expr);
      return result;
    }
    SYNTAX_ERROR("invalid left hand side expression in 'assignment expression'", Current());
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
  ENTER(ParseConditionalExpression);
  ir::Node* expr = ParseBinaryExpression(noin);
  TokenInfo* ti = Current();
  if (ti->type() == Token::TS_QUESTION_MARK) {
    Next();
    ir::Node* left = ParseAssignmentExpression(noin);
    ti = Current();
    if (ti->type() == Token::TS_COLON) {
      Next();
      ir::Node* right = ParseAssignmentExpression(noin);
      ir::TemaryExprView* temary = New<ir::TemaryExprView>(expr, left, right);
      temary->SetInformationForNode(expr);
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
  ENTER(ParseBinaryExpression);
  ir::Node* last = nullptr;
  ir::Node* first = nullptr;
  ir::Node* lhs = ParseUnaryExpression();
  ir::Node* expr = nullptr;
  while (1) {
    TokenInfo* token = Current();
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
          expr->SetInformationForNode(lhs);
          first = expr;
        } else {
          expr = New<ir::BinaryExprView>(type, last, rhs);
          expr->SetInformationForNode(last);
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
            expr->SetInformationForNode(lhs);
            first = expr;
          } else {
            expr = New<ir::BinaryExprView>(type, last, rhs);
            expr->SetInformationForNode(last);
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
          expr->SetInformationForNode(lhs);
          first = expr;
        } else {
          expr = New<ir::BinaryExprView>(type, last, rhs);
          expr->SetInformationForNode(last);
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
  ENTER(ParseUnaryExpression);
  Token type = Current()->type();
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
  ENTER(ParsePostfixExpression);
  ir::Node* node = ParseLeftHandSideExpression();
  if (Current()->type() == Token::TS_INCREMENT) {
    return New<ir::PostfixView>(node, Token::TS_INCREMENT);
  } else if (Current()->type() == Token::TS_DECREMENT) {
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
  ENTER(ParseLeftHandSideExpression);
  if (Current()->type() == Token::TS_NEW) {
    return ParseMemberExpression();
  }
  return ParseCallExpression();
}


// type_parameters
//   : '<' type_parameter_list '>'
//   ;
// type_parameter_list
//   : type_parameter 
//   | type_parameter_list , type_parameter
//   ;
// type_parameter
//   : identifier constraint__opt
//   ;
// constraint
//   : 'extends' type
//   ;

template <typename UCharInputIterator>
ir::Node* Parser<UCharInputIterator>::ParseTypeParameters() {
  if (Current()->type() == Token::TS_LESS) {
    Next();
    ir::TypeParametersView* type_params = New<ir::TypeParametersView>();
    bool found = false;
    while (1) {
      if (Current()->type() == Token::TS_IDENTIFIER) {
        found = true;
        ir::Node* name = ParsePrimaryExpression();
        if (Current()->type() == Token::TS_EXTENDS) {
          type_params->InsertLast(New<ir::TypeConstraintsView>(name));
        } else {
          type_params->InsertLast(name);
        }
      } else if (Current()->type() == Token::TS_GREATER) {
        if (!found) {
          SYNTAX_ERROR("SyntaxError need type parameter", Current());
        }
        Next();
        return type_params;
      } else if (Current()->type() == Token::TS_COMMA) {
        Next();
      } else {
        SYNTAX_ERROR("SyntaxError unexpected token", next_token_info_);
        NO_RETURN;
      }
    }
  }
  SYNTAX_ERROR("SyntaxError '<' expected", next_token_info_);
  NO_RETURN;
}


// type
//   : predefined_type 
//   | type_reference 
//   | type_query 
//   | type_literal
//   ;
//
// pre_defined_type 
//   : 'any'
//   | 'number'
//   | 'boolean'
//   | 'string'
//   | 'void'
//   ;
// type_literal 
//   : object_type 
//   | array_type 
//   | function_type 
//   | constructor_type
//   ;
template <typename UCharInputIterator>
ir::Node* Parser<UCharInputIterator>::ParseTypeExpression() {
  ENTER(ParseTypeExpression);
  if (Current()->type() == Token::TS_IDENTIFIER) {
    return ParseArrayType(ParseReferencedType());
  } else if (Current()->type() == Token::TS_LEFT_PAREN) {
    ir::Node* types = ParseParameterList(false);
    if (Current()->type() == Token::TS_ARROW_GLYPH) {
      Next();
      ir::Node* ret_type = ParseTypeExpression();
      return ParseArrayType(New<ir::FunctionTypeExprView>(types, ret_type));
    }
    SYNTAX_ERROR("SyntaxError '=>' expected", next_token_info_);
  } else if (Current()->type() == Token::TS_LEFT_BRACE) {
    return ParseArrayType(ParseObjectTypeExpression());
  } else if (Current()->type() == Token::TS_TYPEOF) {
    return ParseArrayType(ParseTypeQueryExpression());
  }
  NO_RETURN;
}


// type_reference 
//   : type_name [no line_terminator here] type_arguments__opt
//   ;
// type_name
//   : identifier 
//   | module_name '.' identifier
//   ;
// module_name
//   : identifier
//   | module_name '.' identifier
//   ;
template <typename UCharInputIterator>
ir::Node* Parser<UCharInputIterator>::ParseReferencedType() {
  ENTER(ParseReferencedType);
  if (Current()->type() == Token::TS_IDENTIFIER) {
    ir::Node* node = ParsePrimaryExpression();
    if (Current()->type() == Token::TS_DOT) {
      ir::GetPropView* get_prop_view = New<ir::GetPropView>();
      while (1) {
        if (Current()->type() == Token::TS_DOT) {
          Next();
          get_prop_view->InsertLast(ParsePrimaryExpression());
        } else {
          break;
        }
      }
      node = get_prop_view;
    }
    if (!Current()->has_line_break_before_next() && Current()->type() == Token::TS_LESS) {
      ir::Node* type_parameter = ParseTypeArguments();
      return New<ir::GenericTypeExprView>(node, type_parameter);
    }

    return New<ir::SimpleTypeExprView>(node);
  }

  SYNTAX_ERROR("SyntaxError identifier expected", next_token_info_);
  NO_RETURN;
}


// type_query
//   : 'typeof' type_query_expression
//   ;
// type_query_expression: 
//   : identifier
//   | type_query_expression '.' identifier_name
//   ;
template <typename UCharInputIterator>
ir::Node* Parser<UCharInputIterator>::ParseTypeQueryExpression() {
  if (Current()->type() == Token::TS_TYPEOF) {
    Next();
    if (Current()->type() == Token::TS_IDENTIFIER) {
      ir::Node* name = ParsePrimaryExpression();
      if (Current()->type() == Token::TS_DOT) {
        Next();
        ir::GetPropView* getprop = New<ir::GetPropView>();
        ir::GetPropView* root = getprop;
        getprop->set_target(name);
        while (1) {
          if (Current()->type() == Token::TS_IDENTIFIER) {
            getprop->set_prop(ParsePrimaryExpression());
            if (Current()->type() == Token::TS_DOT) {
              ir::GetPropView* getprop_tmp = New<ir::GetPropView>();
              getprop->set_target(getprop_tmp);
              getprop_tmp = getprop;
            } else {
              break;
            }
          } else {
            break;
          }
        }
        return New<ir::TypeQueryView>(root);
      }
      return New<ir::TypeQueryView>(name);
    }
    SYNTAX_ERROR("SyntaxError identifier expected", next_token_info_);
  }
  SYNTAX_ERROR("SyntaxError 'typeof' expected", next_token_info_);
  NO_RETURN;
}


// type_arguments 
//   : '<' type_argument_list '>'
//   ;
// type_argument_list
//   : type_argument 
//   | type_argument_list ',' type_argument
//   ;
// type_argument
//   : type
//   ;
template <typename UCharInputIterator>
ir::Node* Parser<UCharInputIterator>::ParseTypeArguments() {
  if (Current()->type() == Token::TS_LESS) {
    Next();
    ir::TypeArgumentsView* type_arguments = New<ir::TypeArgumentsView>();
    while (1) {
      type_arguments->InsertLast(ParseTypeExpression());
      if (Current()->type() == Token::TS_COMMA) {
        Next();
      } else {
        return type_arguments;
      }
    }
  }
  SYNTAX_ERROR("SyntaxError '<' expected", next_token_info_);
}


// array_type
//   : element_type [no LineTerminator here] '[' ']'
//   ;
// element_type
//   : pre_defined_type
//   | type_reference
//   | type_query
//   | object_type
//   | array_type
//   ;
template <typename UCharInputIterator>
ir::Node* Parser<UCharInputIterator>::ParseArrayType(ir::Node* type_expr) {
  if (Current()->type() == Token::TS_LEFT_BRACKET) {
    Next();
    if (Current()->type() == Token::TS_RIGHT_BRACKET) {
      Next();
      return ParseArrayType(New<ir::ArrayTypeExprView>(type_expr));
    }
    SYNTAX_ERROR("SyntaxError ']' expected", next_token_info_);
    NO_RETURN;
  }
  return type_expr;
}


// object_type
//   : '{' type_body__opt '}'
//   ;
// type_body
//   : type_member_list ';'__opt
//   ;
// type_member_list
//   : type_member
//   | type_member_list ';' type_member
//   ;
template <typename UCharInputIterator>
ir::Node* Parser<UCharInputIterator>::ParseObjectTypeExpression() {
  ENTER(ParseObjectTypeExpression);
  if (Current()->type() == Token::TS_LEFT_BRACE) {
    Next();
    ir::ObjectTypeExprView* object_type = New<ir::ObjectTypeExprView>();
    object_type->SetInformationForNode(Current());
    while (Current()->type() != Token::TS_RIGHT_BRACE) {
      ir::Node* property = ParseObjectTypeElement();
      object_type->InsertLast(property);
    }
    return object_type;
  }
  SYNTAX_ERROR("SyntaxError '{' expected", next_token_info_);
  NO_RETURN;
}


// type_member
//   : property_signature
//   | call_signature
//   | construct_signature
//   | index_signature
//   | method_signature
//   ;
template <typename UCharInputIterator>
ir::Node* Parser<UCharInputIterator>::ParseObjectTypeElement() {
  ENTER(ParseObjectTypeElement);
  if (Current()->type() == Token::TS_NEW) {
    Next();
    ir::Node* call_sig = ParseCallSignature(false);
    return New<ir::ConstructSignatureView>(call_sig);
  } else if (Current()->type() == Token::TS_LEFT_PAREN) {
    return ParseCallSignature(false);
  } else if (Current()->type() == Token::TS_LEFT_BRACKET) {
    Next();
    if (Current()->type() == Token::TS_IDENTIFIER) {
      ir::Node* name = ParseLiteral();
      if (Current()->type() == Token::TS_COLON) {
        Next();
        if (Current()->type() == Token::TS_IDENTIFIER &&
            (Current()->value() == "string" || Current()->value() == "number")) {
          Next();
          return New<ir::AccessorTypeExprView>(name, ParsePrimaryExpression());
        } else {
          SYNTAX_ERROR("SyntaxError type name in indexSignature only allowed one of 'string' or 'number'", next_token_info_);
        }
      }
      SYNTAX_ERROR("SyntaxError ':' expected", next_token_info_);
    }
    SYNTAX_ERROR("SyntaxError identifier expected", next_token_info_);
  } else {
    bool optional = false;
    ir::Node* key = ParseObjectKey();
    if (Current()->type() == Token::TS_QUESTION_MARK) {
      optional = true;
      Next();
    }
    if (Current()->type() == Token::TS_LEFT_PAREN) {
      if (key->HasNameView()) {
        SYNTAX_ERROR_POS("SyntaxError invalid method name", key->source_position());
      }
      ir::Node* call_sig = ParseCallSignature(false);
      return New<ir::MethodSignatureView>(optional, key, call_sig);
    } else if (Current()->type() == Token::TS_COLON) {
      Next();
      ir::Node* type_expr = ParseTypeExpression();
      return New<ir::PropertySignatureView>(optional, key, type_expr);
    } else if (Current()->type() == Token::TS_LEFT_BRACKET) {
      if (optional) {
        SYNTAX_ERROR("SyntaxError unexpected '?'", Current());
      }
      return ParseTypeExpression();
    }
    return New<ir::PropertySignatureView>(optional, key, nullptr);
  }
}


// Parse call expression.
// call_expression
// 	: member_expression arguments
//  | super arguments
//  | super '.' identifier arguments
// 	| call_expression arguments
// 	| call_expression '[' expression ']'
// 	| call_expression '.' IDENTIFIER
template <typename UCharInputIterator>
ir::Node* Parser<UCharInputIterator>::ParseCallExpression() {
  ENTER(ParseCallExpression);
  ir::Node* target;
  if (Current()->type() == Token::TS_SUPER) {
    Next();
    target = New<ir::SuperView>();
    if (Current()->type() == Token::TS_DOT) {
      Next();
      ir::Node* literal = ParseLiteral();
      if (literal->HasNameView()) {
        target = New<ir::GetPropView>(target, literal);
      } else {
        SYNTAX_ERROR_POS("SyntaxError unexpected token", literal->source_position());
      }
    }
  } else {
    target = ParseMemberExpression();
  }
  if (Current()->type() == Token::TS_LEFT_PAREN) {
    ir::Node* args = ParseArguments();
    ir::Node* call = New<ir::CallView>(target, args);
    while (1) {
      switch (Current()->type()) {
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
  ENTER(ParseArguments);
  if (Next()->type() == Token::TS_LEFT_PAREN) {
    ir::CallArgsView* args = New<ir::CallArgsView>();
    while (1) {
      args->InsertLast(ParseAssignmentExpression(false));
      Next();
      if (Current()->type() == Token::TS_COMMA) {
        continue;
      } else if (Current()->type() == Token::TS_RIGHT_BRACE) {
        return args;
      }
      SYNTAX_ERROR("SyntaxError unexpected token in 'arguments'", Current());
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
  ENTER(ParseMemberExpression);
  // Not advance scanner.
  TokenInfo* token_info = Current();
  ir::Node* node;
  if (token_info->type() ==  Token::TS_NEW) {
    // Parse new Foo() expression.
    Next();
    ir::Node* member = ParseMemberExpression();

    // New expression can omit parens.
    // If paren exists, continue parsing.
    if (Current()->type() == Token::TS_LEFT_PAREN) {
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
  ENTER(ParseGetPropOrElem);
  while (1) {
    switch (Current()->type()) {
      case Token::TS_LEFT_BRACKET: {
        // [...] expression.
        Next();
        ir::Node* expr = ParseExpression(false);
        node = New<ir::GetElemView>(node, expr);
        if (Next()->type() != Token::TS_RIGHT_BRACKET) {
          SYNTAX_ERROR("Unexpected token.", Current());
        }
        break;
      }
      case Token::TS_DOT: {
        // a.b.c expression.
        Next();
        ir::Node* expr = ParsePrimaryExpression();
        if (!expr->HasNameView() && !expr->HasKeywordLiteralView()) {
          SYNTAX_ERROR_POS("SyntaxError identifier expected", expr->source_position());
        }
        node = New<ir::GetPropView>(node, expr);
        break;
      }
      default:
        return node;
    }
  }
  SYNTAX_ERROR("SyntaxError", Current());
  NO_RETURN;
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
  ENTER(ParsePrimaryExpression);
  // Not advance scanner.
  TokenInfo* token_info = Current();
  switch (token_info->type()) {
    case Token::TS_IDENTIFIER: {
      // parse an identifier.
      ir::NameView* name = New<ir::NameView>(token_info->value());
      name->SetInformationForNode(Current());
      Next();
      return name;
    }
    case Token::TS_THIS: {
      // parse a this.
      ir::Node* this_view = New<ir::ThisView>();
      this_view->SetInformationForNode(Current());
      Next();
      return this_view;
    }
    case Token::TS_LEFT_BRACE:
      // parse an object literal.
      return ParseObjectLiteral();
    case Token::TS_LEFT_BRACKET:
      // parse an array literal.
      return ParseArrayLiteral();
    case Token::TS_LEFT_PAREN: {
      typename ParserState::ArrowFunctionScope scope(&parser_state_);
      ir::Node* node = nullptr;

      // First try parse as expression.
      try {
        bool reparse_as_arrow_parameter = false;
        {
          // Enable token recording mode.
          typename ParserState::RecordTokenScope token_scope(&parser_state_);
          token_buffer_.PushBack(Current());
          Next();
          // parse an expression that beggining '('
          node = ParseExpression(false);

          // If close brace is not exists, it's treat as an arrow function.
          if (Current()->type() != Token::TS_RIGHT_PAREN || Peek()->type() == Token::TS_ARROW_GLYPH) {
            reparse_as_arrow_parameter = true;
          }
        }

        // If parse failed, try parse as an arrow parameter,
        // and if parse success but arrow glyph(=>) exists,
        // treat as an arrow parameter too.
        if (reparse_as_arrow_parameter) {
          return ParseArrowFunction();
        }

        token_buffer_.Clear();
        
        return node;
      } catch (const SyntaxError& e) {
        // If failure, try parse as arrow function.
        return ParseArrowFunction();
      } catch (const std::exception& e) {
        // Any errors except the SyntaxError are goto here.
        throw e;
      }
      break;
    }
    default:
      // parse a literal.
      return ParseLiteral();
  }
}


template <typename UCharInputIterator>
ir::Node* Parser<UCharInputIterator>::ParseObjectLiteral() {
  ENTER(ParseObjectLiteral);
  if (Current()->type() == Token::TS_LEFT_BRACE) {
    Next();
    ir::ObjectLiteralView* object_literal = New<ir::ObjectLiteralView>();
    while (1) {
      ir::Node* key = ParseObjectKey();
      if (Current()->type() != Token::TS_COLON) {
        SYNTAX_ERROR("SyntaxError ':' expected", Current());
      }
      Next();
      ir::Node* value = ParseAssignmentExpression(false);
      ir::ObjectElementView* element = New<ir::ObjectElementView>(key, value);
      object_literal->InsertLast(element);
      if (Current()->type() == Token::TS_COMMA) {
        Next();
      } else if (Current()->type() == Token::TS_RIGHT_BRACE) {
        Next();
        break;
      } else {
        SYNTAX_ERROR("SyntaxError expected ',' or '}'", Current());
      }
    }
    return object_literal;
  }
  SYNTAX_ERROR("Unexpected token.", Current());
  NO_RETURN;
}


template <typename UCharInputIterator>
ir::Node* Parser<UCharInputIterator>::ParseObjectKey() {
  ENTER(ParseObjectKey);
  YATSC_SCOPED([&]{this->Next();});
  if (Current()->type() == Token::TS_IDENTIFIER) {
    return New<ir::NameView>(Current()->value());
  } else if (Current()->type() == Token::TS_STRING_LITERAL) {
    return New<ir::StringView>(Current()->value());
  } else if (Current()->type() == Token::TS_NUMERIC_LITERAL) {
    return New<ir::NumberView>(Current()->value());
  } else {
    SYNTAX_ERROR("Invalid ObjectLiteral key.", Current());
  }
  return nullptr;
}


template <typename UCharInputIterator>
ir::Node* Parser<UCharInputIterator>::ParseArrayLiteral() {
  ENTER(ParseArrayLiteral);
  if (Current()->type() == Token::TS_LEFT_BRACKET) {
    Next();
    ir::ArrayLiteralView* array_literal = New<ir::ArrayLiteralView>();
    while (1) {
      ir::Node* expr = ParseAssignmentExpression(false);
      array_literal->InsertLast(expr);
      if (Current()->type() == Token::TS_COMMA) {
        Next();
      } else if (Current()->type() == Token::TS_RIGHT_BRACKET) {
        Next();
        break;
      } else {
        SYNTAX_ERROR("SyntaxError unexpected token in 'array literal'", Current());
      }
    }
    return array_literal;
  }
  SYNTAX_ERROR("Unexpected token.", Current());
  NO_RETURN;
}


template <typename UCharInputIterator>
ir::Node* Parser<UCharInputIterator>::ParseLiteral() {
  ENTER(ParseLiteral);
  // Allow regular expression in this context.
  typename Scanner<UCharInputIterator>::RegularExpressionScope scope(scanner_);
  ir::Node* ret = nullptr;
  YATSC_SCOPED([&]{
    if (ret != nullptr) {
      ret->SetInformationForNode(this->Current());
    }
    this->Next();
  })
  switch(Current()->type()) {
    case Token::TS_TRUE:
      // true value
      return ret = New<ir::TrueView>();
    case Token::TS_FALSE:
      // false value
      return ret = New<ir::FalseView>();
    case Token::TS_STRING_LITERAL:
      // 'aaaa' value
      return ret = New<ir::StringView>(Current()->value());
    case Token::TS_NUMERIC_LITERAL:
      // 1234.5 value
      return ret = New<ir::NumberView>(Current()->value());
    case Token::TS_REGULAR_EXPR:
      // /aaaaa/ value
      return ret = New<ir::RegularExprView>(Current()->value());
    case Token::TS_UNDEFINED:
      // undefined value
      return ret = New<ir::UndefinedView>();
    case Token::TS_NULL:
      // null value
      return ret = New<ir::NullView>();
    case Token::TS_NAN:
      // NaN value
      return ret = New<ir::NaNView>();
    default:
      // Any other token cause SyntaxError.
      SYNTAX_ERROR("SyntaxError literal expected", Current());
      NO_RETURN;
  }
}


template <typename UCharInputIterator>
ir::Node* Parser<UCharInputIterator>::ParseParameterList(bool accesslevel_allowed) {
  ENTER(ParseParameterList);
  if (Current()->type() == Token::TS_LEFT_PAREN) {
    Next();
    ir::ParamList* param_list = New<ir::ParamList>();
    while (1) {
      switch (Current()->type()) {
        case Token::TS_IDENTIFIER: {
          param_list->InsertLast(ParseParameter(false, accesslevel_allowed));
          break;
        }

        case Token::TS_REST: {
          Next();
          ir::Node* node = New<ir::RestParamView>(ParseParameter(true, accesslevel_allowed));
          param_list->InsertLast(node);
          break;
        }

        case Token::TS_COMMA: {
          Next();
          break;
        }

        default:
          if (Current()->type() == Token::TS_RIGHT_PAREN) {
            Next();
            return param_list;
          }
          SYNTAX_ERROR("SyntaxError ')' expected in parameter list", next_token_info_);
      }
    }
  }
  SYNTAX_ERROR("SyntaxError '(' is expected in parameter list", next_token_info_);
  NO_RETURN;
}


template <typename UCharInputIterator>
ir::Node* Parser<UCharInputIterator>::ParseParameter(bool rest, bool accesslevel_allowed) {
  ENTER(ParseParameter);
  ir::Node* access_level = nullptr;
  if (Current()->type() == Token::TS_PUBLIC || Current()->type() == Token::TS_PRIVATE) {
    Next();
    if (accesslevel_allowed) {
      access_level = New<ir::ClassFieldAccessLevelView>(Current()->type());
    } else {
      SYNTAX_ERROR("SyntaxError 'private' or 'public' not allowed here", Current());
      NO_RETURN;
    }
  }
  if (Current()->type() == Token::TS_IDENTIFIER) {
    ir::ParameterView* pv = New<ir::ParameterView>();
    ir::NameView* nv = New<ir::NameView>(Current()->value());
    pv->set_access_level(access_level);
    pv->set_name(nv);
    Next();
    if (Current()->type() == Token::TS_QUESTION_MARK) {
      if (rest) {
        SYNTAX_ERROR("SyntaxError optional parameter not allowed in rest parameter", Current());
        NO_RETURN;
      }
      Next();
      pv->set_optional(true);
    }
    if (Current()->type() == Token::TS_COLON) {
      Next();
      pv->set_type_expr(ParseTypeExpression());
    }
    if (Current()->type() == Token::TS_ASSIGN) {
      if (rest) {
        SYNTAX_ERROR("SyntaxError default parameter not allowed in rest parameter", Current());
        NO_RETURN;
      }
      Next();
      pv->set_value(ParseAssignmentExpression(false));
    }
    return pv;
  }
  SYNTAX_ERROR("SyntaxError identifier expected", Current());
  NO_RETURN;
}


template <typename UCharInputIterator>
ir::Node* Parser<UCharInputIterator>::ParseFunction() {
  ENTER(ParseFunction);
  if (Current()->type() == Token::TS_FUNCTION) {
    Next();
    ir::Node* name = nullptr;
    if (Current()->type() == Token::TS_IDENTIFIER) {
      name = ParseLiteral();
    }
    ir::Node* call_signature = ParseCallSignature(false);
    ir::Node* body = ParseFunctionBody();
    return New<ir::FunctionView>(name, call_signature, body);
  }
  SYNTAX_ERROR("SyntaxError 'function' expected", next_token_info_);
  NO_RETURN;
}


template <typename UCharInputIterator>
ir::Node* Parser<UCharInputIterator>::ParseCallSignature(bool accesslevel_allowed) {
  ENTER(ParseCallSignature);
  if (Current()->type() == Token::TS_LEFT_PAREN) {
    ir::Node* parameter_list = ParseParameterList(accesslevel_allowed);
    ir::Node* return_type = nullptr;
    if (Current()->type() == Token::TS_COLON) {
      Next();
      return_type = ParseTypeExpression();
    }
    return New<ir::CallSinatureView>(parameter_list, return_type);
  }
  SYNTAX_ERROR("SyntaxError expected '('", next_token_info_);
  NO_RETURN;
}


template <typename UCharInputIterator>
ir::Node* Parser<UCharInputIterator>::ParseArrowFunction() {
  ENTER(ParseArrowFunction);
  
  ir::Node* param_list = ParseParameterList(false);
  if (Current()->type() != Token::TS_ARROW_GLYPH) {
    SYNTAX_ERROR("SyntaxError '=>' expected", Current());
    NO_RETURN;
  }
  Next();
  ir::Node* body;
  if (Current()->type() == Token::TS_LEFT_BRACE) {
    Next();
    body = ParseFunctionBody();
  } else {
    body = ParseAssignmentExpression(false);
  }
  return New<ir::ArrowFunctionView>(nullptr, param_list, body);
}


template <typename UCharInputIterator>
ir::Node* Parser<UCharInputIterator>::ParseFunctionBody() {
  ENTER(ParseFunctionBody);
  if (Current()->type() == Token::TS_LEFT_BRACE) {
    Next();
    auto block = New<ir::BlockView>();
    while (1) {
      auto node = ParseSourceElement();
      block->InsertLast(node);
      if (Current()->type() == Token::TS_RIGHT_BRACE) {
        break;
      }
    }
    return block;
  }
  SYNTAX_ERROR("SyntaxError unexpected token in 'function body'", next_token_info_);
  NO_RETURN;
}


template <typename UCharInputIterator>
ir::Node* Parser<UCharInputIterator>::ParseFormalParameterList() {
  return nullptr;
}
}

#undef ENTER
#undef SYNTAX_ERROR
