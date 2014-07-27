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

#include "./parser-util.h"

namespace yatsc {

// Expression[In, Yield]
//   AssignmentExpression[?In, ?Yield]
//   Expression[?In, ?Yield] , AssignmentExpression[?In, ?Yield]
//
template <typename UCharInputIterator>
ir::Node* ExpressionParser<UCharInputIterator>::ParseExpression(bool in, bool yield) {
  PARSER_LOG_PHASE(ParseExpression);
  ir::Node* assignment_expr = ParseAssignmentExpression(in, yield);

  // Parse comma expressions.
  if (parser()->Current()->type() == Token::TS_COMMA) {
    Next();
    ir::CommaExprView* comma_expr = New<ir::CommaExprView>({assignment_expr});
    comma_expr->SetInformationForNode(*Current());
    
    while (1) {
      assignment_expr = ParseAssignmentExpression(in, yield);
      comma_expr->InsertLast(assignment_expr);
      if (parser()->Current()->type() == Token::TS_COMMA) {
        Next();
      } else {
        return comma_expr;
      }
    }
  }
  
  return assignment_expr;
}


// AssignmentPattern[Yield]
//   ObjectAssignmentPattern[?Yield]
//   ArrayAssignmentPattern[?Yield]
//
template <typename UCharInputIterator>
ir::Node* ExpressionParser<UCharInputIterator>::ParseAssignmentPattern(bool yield) {
  PARSER_LOG_PHASE(ParseAssignmentPattern);
  TokenCursor cursor = parser()->GetBufferCursorPosition();
  ir::Node* node = nullptr;
  switch(parser()->Current()->type()) {
    case Token::TS_LEFT_BRACE: {
      node = ParseObjectPattern(yield);
      break;
    }
    case Token::TS_LEFT_BRACKET: {
      node = ParseArrayPattern(yield);
      break;
    }
    default:
      PARSER_SYNTAX_ERROR("SyntaxError unexpected token", parser()->PeekBuffer(cursor));
  }

  node->SetInformationForNode(parser()->PeekBuffer(cursor));
  return node;
}


// ObjectAssignmentPattern[Yield]
//   { }
//   { AssignmentPropertyList[?Yield] }
//   { AssignmentPropertyList[?Yield] , }
//
template <typename UCharInputIterator>
ir::Node* ExpressionParser<UCharInputIterator>::ParseObjectAssignmentPattern(bool yield) {
  PARSER_LOG_PHASE(ParseObjectAssignmentPattern);
  if (parser()->Current()->type() == Token::TS_LEFT_BRACE) {
    parser()->Next();
    ir::Node* node = ParserAssignmentPropertyList();
    if (parser()->Current()->type() == Token::TS_COMMA) {
      parser()->Next();
    }
    if (parser()->Current()->type() == Token::TS_RIGHT_BRACE) {
      parser()->Next();
      return node;
    }
    PARSER_SYNTAX_ERROR("SyntaxError unexpected token", parser()->Current());
  }
}


// ArrayAssignmentPattern[Yield]
//   [ Elision(opt) AssignmentRestElement[?Yield](opt) ]
//   [ AssignmentElementList[?Yield] ]
//   [ AssignmentElementList[?Yield] , Elision(opt) AssignmentRestElement[?Yield](opt) ]
//
// AssignmentElementList[Yield]
//   AssignmentElisionElement[?Yield]
//   AssignmentElementList[?Yield] , AssignmentElisionElement[?Yield]
//
template <typename UCharInputIterator>
ir::Node* ExpressionParser<UCharInputIterator>::ParseArrayAssignmentPattern(bool yield) {
  PARSER_LOG_PHASE(ParseArrayAssignmentPattern);
  if (parser()->Current()->type() == Token::TS_LEFT_BRACKET) {
    bool has_rest = false;
    auto array_view = parser()->New<ir::BindingArrayView>();
    array_view->SetInformationForNode(parser()->Current());
    parser()->Next();


    // '[, ...' pattern
    if (parser()->Current()->type() == Token::TS_COMMA) {
      parser()->Next();
      array_view->InsertLast(parser()->New<ir::UndefinedView>());
      array_view->InsertLast(parser()->New<ir::UndefinedView>());
    }
    
    if (parser()->Current()->type() == Token::TS_REST) {
      array_view->InsertLast(ParseAssignmentRestElement(yield));
      has_rest = true;
    }

    if (parser()->Current()->type() == Token::TS_RIGHT_BRACKET) {
      if (array_view->size() == 0) {
        PARSER_SYNTAX_ERROR("SyntaxError destructuring assignment left hand side is not allowed empty array", parser()->Current());
      }
      parser()->Next();
      return array_view;
    }

    // The ParameterRest is not allowed in any position of array pattern except the last element.
    if (has_rest) {
      PARSER_SYNTAX_ERROR("SyntaxError destructuring assignment rest must be the end of element", parser()->Current());
    }

    while (1) {
      if (parser()->Current()->type() == Token::TS_COMMA) {
        array_view->InsertLast(parser()->New<ir::UndefinedView>());
        array_view->InsertLast(parser()->New<ir::UndefinedView>());
        parser()->Next();
      } else {
        array_view->InsertLast(ParseAssignmentElement());
      }
      switch (parser()->Current()->type()) {
        case Token::TS_COMMA:
          parser()->Next();
          break;
        case Token::TS_RIGHT_BRACKET:
          parser()->Next();
          return array_view;
        default:
          PARSER_SYNTAX_ERROR("SyntaxError unexpected token", parser()->Current());
      }
    }
  }
  SYNTAX_ERROR("SyntaxError unexpected token", parser()->Current());
}


// Parse destructuring assignment object pattern properties.
// 
// AssignmentPropertyList[Yield]
//   AssignmentProperty[?Yield]
//   AssignmentPropertyList[?Yield] , AssignmentProperty[?Yield]
//
template <typename UCharInputIterator>
ir::Node* ExpressionParser<UCharInputIterator>::ParseAssignmentPropertyList(bool yield) {
  PARSER_LOG_PHASE(ParseAssignmentPropertyList);
  auto prop_list = parser()->New<ir::BindingPropListView>();
  while (1) {
    prop_list->InsertLast(ParseAssignmentProperty(yield));
    if (parser()->Current()->type() == Token::TS_COMMA) {
      parser()->Next();
      continue;
    }
    if (parser()->Current()->type() == Token::TS_RIGHT_BRACE) {
      return prop_list;
    }
  }

  // UNREACHABLE
  return nullptr;
}


// Parse destructuring assignment object pattern property.
//
// AssignmentProperty[Yield] :
//   IdentifierReference[?Yield] Initializer[In,?Yield](opt)
//   PropertyName : AssignmentElement[?Yield]
//
template <typename UCharInputIterator>
ir::Node* ExpressionParser<UCharInputIterator>::ParseAssignmentProperty(bool yield) {
  PARSER_LOG_PHASE(ParseAssignmentProperty);
  TokenCursor cursor = parser()->GetBufferCursorPosition();
  
  ir::Node* property_name = nullptr;
  ir::Node* elem = nullptr;

  // Check whether property name is identifier reference or not.
  bool identifier = false;
  
  if ((yield && parser()->Current()->type() == Token::TS_YIELD) ||
      parser()->Current()->type() == Token::TS_IDENTIFIER) {
    identifier = true;
    property_name = ParseIdentifierReference();
  } else if (parser()->Current()->type() == Token::TS_STRING_LITERAL ||
             parser()->Current()->type() == Token::TS_NUMERIC_LITERAL ||
             parser()->Current()->type() == Token::TS_LEFT_BRACKET) {
    property_name = ParsePropertyName(yield, false);
  }
  
  ir::Node* init = nullptr;

  // Initializer is only allowed, if property name is identifier reference.
  if (identifier && parser()->Current()->type() == Token::TS_ASSIGN) {
    init = ParseInitializer(true, yield);
  } else if (parser()->Current()->type() == Token::TS_COLON) {
    parser()->Next();
    elem = ParseAssignmentElement(yield);
  }

  // All destructuring assignment element is convert to BindingElementView.
  auto binding_element = parser()->New<ir::BindingElementView>(property_name, elem, init);
  binding_element->SetInformationForNode(parser()->PeekBuffer(cursor));
  return binding_element;
}


// AssignmentElement[Yield]
//   DestructuringAssignmentTarget[?Yield] Initializer[In,?Yield](opt)
//
template <typename UCharInputIterator>
ir::Node* ExpressionParser<UCharInputIterator>::ParseAssignmentElement(bool yield) {
  PARSER_LOG_PHASE(ParseAssignmentElement);
  ir::Node* target = ParseDestructuringAssignmentTarget(yield);
  ir::Node* init = nullptr;
  if (parser()->Current()->type() == Token::TS_ASSIGN) {
    init = ParseInitializer(true, yield);
  }

  // All destructuring assignment element is convert to BindingElementView.
  auto binding_element = parser()->New<ir::BindingElementView>(nullptr, target, init);
  binding_element->SetInformationForNode(parser()->PeekBuffer(cursor));
  return binding_element;
}


// AssignmentRestElement[Yield]
//   ... DestructuringAssignmentTarget[?Yield]
//
template <typename UCharInputIterator>
ir::Node* ExpressionParser<UCharInputIterator>::ParseAssignmentRestElement(bool yield) {
  PARSER_LOG_PHASE(ParseAssignmentRestElement);
  if (parser()->Current()->type() == Token::TS_REST) {
    TokenCursor cursor = parser()->GetBufferCursorPosition();
    parser()->Next();
    ir::Node* target = ParseDestructuringAssignmentTarget(yield);
    auto rest = parser()->New<ir::RestParamView>(target);
    rest->SetInformationForNode(parser()->PeekBuffer(cursor));
  }
  PARSER_SYNTAX_ERROR("SyntaxError '...' expected", parser()->Current());
}


// DestructuringAssignmentTarget[Yield]
//   LeftHandSideExpression[?Yield]
//
template <typename UCharInputIterator>
ir::Node* ExpressionParser<UCharInputIterator>::ParseDestructuringAssignmentTarget(bool yield) {
  PARSER_LOG_PHASE(ParseDestructuringAssignmentTarget);
  return ParseLeftHandSideExpression(yield);
}


// AssignmentExpression[In, Yield]
//   ConditionalExpression[?In, ?Yield]
//   [+Yield] YieldExpression[?In]
//   ArrowFunction[?In, ?Yield]
//   LeftHandSideExpression[?Yield] = AssignmentExpression[?In,?Yield]
//   LeftHandSideExpression[?Yield] AssignmentOperator AssignmentExpression[?In, ?Yield]
//  
template <typename UCharInputIterator>
ir::Node* ExpressionParser<UCharInputIterator>::ParseAssignmentExpression(bool in, bool yield) {
  PARSER_LOG_PHASE(ParseAssignmentExpression);
  ir::Node* node = nullptr;

  // Record current buffer position.
  TokenCursor cursor = parser()->GetBufferCursorPosition();
  
  if (parser()->Current()->type() == Token::TS_LEFT_PAREN ||
      parser()->Current()->type() == Token::TS_LESS) {
    // First try parse as arrow function.
    bool failed = false;
    try {
      // parsae an arrow_function_parameters.
      node = parser()->ParseArrowFunctionParameters();
    } catch (const SyntaxError& e) {
      // If parse failed, try parse as an parenthesized_expression in primary expression,
      // Do nothing.
      failed = true;
    } catch (const ArrowParametersError& a) {
      // If ArrowParameterError thrown, rethrow error because
      // that is a ParseError for the arrow_function_parameters.
      throw a;   
    } catch (const std::exception& e) {
      // Any errors except the SyntaxError and ArrowParametersError are goto here.
      throw e;
    }

    if (!failed) {
      return parser()->ParseArrowFunctionBody(node);
    }
    
    parser()->SetBufferCursorPosition(cursor);
  }

  ir::Node* expr = nullptr;
  bool parsed_as_assignment_pattern = false;

  if (parser()->Current()->type() == Token::TS_YIELD) {
    if (!yield) {
      PARSER_SYNTAX_ERROR("SyntaxError invalid use of 'yield' keyword", parser()->Current());
    }
    expr = ParseYieldExpression();
  } else {  
    try {
      expr = ParseConditionalExpression(in, yield);
    } catch (const SyntaxError& e) {
      parser()->SetBufferCursorPosition(cursor);
      expr = ParseAssignmentPattern(yield);
      parsed_as_assignment_pattern = true;
    }
  }

  if (expr->HasNameView() && parser()->Current()->type() == Token::TS_ARROW_GLYPH) {
    return parser()->ParseArrowFunction(expr);
  }
  
  // Expression is not an arrow_function.
  TokenInfo *token_info = parser()->Current();
  Token type = token_info->type();

  // Check assignment operators.
  if (IsAssignmentOp(type)) {
    if (!parsed_as_assignment_pattern &&
        expr->HasObjectLiteralView() ||
        expr->HasArrayLiteralView()) {
      parser()->SetBufferCursorPosition(cursor);
      expr = ParseAssignmentPattern(yield);
    }
    parser()->Next();

    // Check left hand side expression is valid as an expression.
    // If left hand side expression is like 'func()',
    // that is invalid expression.
    if (expr->IsValidLhs()) {
      ir::Node* rhs = ParseAssignmentExpression(has_in, has_yield);
      ir::Node* result = New<ir::AssignmentView>(type, expr, rhs);
      result->SetInformationForNode(expr);
      return result;
    }
    PARSER_SYNTAX_ERROR("SyntaxError invalid left hand side expression in 'assignment expression'", parser()->Current());
  } else if (parsed_as_assignment_pattern) {
    PARSER_SYNTAX_ERROR("SyntaxError destructuring assignment must be initialized", parser()->Current());
  }
  return expr;
}


// ConditionalExpression[In, Yield]
//   LogicalORExpression[?In, ?Yield]
//   LogicalORExpression[?In,?Yield] ? AssignmentExpression[In, ?Yield] : AssignmentExpression[?In, ?Yield]
//
template <typename UCharInputIterator>
ir::Node* ExpressionParser<UCharInputIterator>::ParseConditionalExpression(bool in, bool yield) {
  PARSER_LOG_PHASE(ParseConditionalExpression);
  ir::Node* expr = ParseBinaryExpression(in, yield);
  TokenInfo* ti = parser()->Current();
  if (ti->type() == Token::TS_QUESTION_MARK) {
    parser()->Next();
    ir::Node* left = ParseAssignmentExpression(in, yield);
    ti = parser()->Current();
    if (ti->type() == Token::TS_COLON) {
      parser()->Next();
      ir::Node* right = ParseAssignmentExpression(has_in, has_yield);
      ir::TemaryExprView* temary = New<ir::TemaryExprView>(expr, left, right);
      temary->SetInformationForNode(expr);
      temary->MarkAsInValidLhs();
      return temary;
    }
    PARSER_SYNTAX_ERROR("SyntaxError unexpected token in 'temary expression'", Current());
  }
  return expr;
}


// ConditionalExpression[In, Yield]
//   LogicalORExpression[?In, ?Yield]
//   LogicalORExpression[?In,?Yield] ? AssignmentExpression[In, ?Yield] : AssignmentExpression[?In, ?Yield]
//   BitwiseORExpression[In, Yield]
//
// BitwiseXORExpression[?In, ?Yield]
//   BitwiseORExpression[?In, ?Yield] | BitwiseXORExpression[?In, ?Yield]
//   BitwiseXORExpression[In, Yield]
//
// BitwiseANDExpression[?In, ?Yield]
//   BitwiseXORExpression[?In, ?Yield] ^ BitwiseANDExpression[?In, ?Yield]
//   BitwiseANDExpression[In, Yield]
//
// EqualityExpression[?In, ?Yield]
//   BitwiseANDExpression[?In, ?Yield] & EqualityExpression[?In, ?Yield]
//   EqualityExpression[In, Yield]
//
// RelationalExpression[?In, ?Yield]
//   EqualityExpression[?In, ?Yield] == RelationalExpression[?In, ?Yield]
//   EqualityExpression[?In, ?Yield] != RelationalExpression[?In, ?Yield]
//   EqualityExpression[?In, ?Yield] ===RelationalExpression[?In, ?Yield]
//   EqualityExpression[?In, ?Yield] !==RelationalExpression[?In, ?Yield]
//   RelationalExpression[In, Yield]
//
// ShiftExpression[?Yield]
//   RelationalExpression[?In, ?Yield] < ShiftExpression[?Yield]
//   RelationalExpression[?In, ?Yield] > ShiftExpression[?Yield]
//   RelationalExpression[?In, ?Yield] <= ShiftExpression[?Yield]
//   RelationalExpression[?In, ?Yield] >= ShiftExpression[?Yield]
//   RelationalExpression[?In, ?Yield] instanceof ShiftExpression[?Yield]
//   [+In] RelationalExpression[In, ?Yield] in ShiftExpression[?Yield]
//   ShiftExpression[Yield]
//
// AdditiveExpression[?Yield]
//   ShiftExpression[?Yield] << AdditiveExpression[?Yield]
//   ShiftExpression[?Yield] >> AdditiveExpression[?Yield]
//   ShiftExpression[?Yield] >>> AdditiveExpression[?Yield]
//   AdditiveExpression[Yield]
//
// MultiplicativeExpression[?Yield]
//   AdditiveExpression[?Yield] + MultiplicativeExpression[?Yield]
//   AdditiveExpression[?Yield] - MultiplicativeExpression[?Yield]
//   MultiplicativeExpression[Yield]
//
// UnaryExpression[?Yield]
//   MultiplicativeExpression[?Yield] * UnaryExpression[?Yield]
//   MultiplicativeExpression[?Yield] / UnaryExpression[?Yield]
//   MultiplicativeExpression[?Yield] % UnaryExpression[?Yield]
//
template <typename UCharInputIterator>
ir::Node* Parser<UCharInputIterator>::ParseBinaryExpression(bool in, bool yield) {
  PARSER_LOG_PHASE(ParseBinaryExpression);
  ir::Node* last = nullptr;
  ir::Node* first = nullptr;
  ir::Node* lhs = ParseUnaryExpression();
  ir::Node* expr = nullptr;
  while (1) {
    TokenInfo* token = parser()->Current();
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
        parser()->Next();
        ir::Node* rhs = ParseBinaryExpression(has_in, has_yield);
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
        if (in) {
          parser()->Next();
          ir::Node* rhs = ParseBinaryExpression(in, yield);
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
        parser()->Next();
        ir::Node* rhs = ParseBinaryExpression(in, yield);
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


// UnaryExpression[Yield]
//   PostfixExpression[?Yield]
//   delete UnaryExpression[?Yield]
//   void UnaryExpression[?Yield]
//   typeof UnaryExpression[?Yield]
//   ++ UnaryExpression[?Yield]
//   -- UnaryExpression[?Yield]
//   + UnaryExpression[?Yield]
//   - UnaryExpression[?Yield]
//   ~ UnaryExpression[?Yield]
//   ! UnaryExpression[?Yield]
//
template <typename UCharInputIterator>
ir::Node* ExpressionParser<UCharInputIterator>::ParseUnaryExpression(bool yield) {
  PARSER_LOG_PHASE(ParseUnaryExpression);
  Token type = parser()->Current()->type();
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
      ir::Node* node = ParseUnaryExpression(yield);
      ir::Node* ret = New<ir::UnaryExprView>(type, node);
      ret->SetInformationForNode(node);
      return ret;
    }
    case Token::TS_LESS: {
      ir::Node* type_arguments = ParseTypeArguments();
      ir::Node* expr = ParseUnaryExpression(yield);
      ir::Node* ret = New<ir::CastView>(type_arguments, expr);
      ret->SetInformationForNode(type_arguments);
      return ret;
    }
    default:
      return ParsePostfixExpression(yield);
  }
}


// PostfixExpression[Yield]
//   LeftHandSideExpression[?Yield]
//   LeftHandSideExpression[?Yield] [no LineTerminator here] ++
//   LeftHandSideExpression[?Yield] [no LineTerminator here] --
//
template <typename UCharInputIterator>
ir::Node* ExpressionParser<UCharInputIterator>::ParsePostfixExpression(bool yield) {
  PARSER_LOG_PHASE(ParsePostfixExpression);
  ir::Node* node = ParseLeftHandSideExpression(yield);
  if (parser()->Current()->type() == Token::TS_INCREMENT ||
      parser()->Current()->type() == Token::TS_DECREMENT) {
    ir::Node* ret = New<ir::PostfixView>(node, Current()->type());
    ret->SetInformationForNode(node);
    Next();
    return ret;
  }
  return node;
}


// CallExpression[Yield]
//   MemberExpression[?Yield] TypeArguments(opt) Arguments[?Yield]
//   super TypeArguments(opt) Arguments[?Yield]
//   super . IdentifierName
//   CallExpression[?Yield] TypeArguments(opt) Arguments[?Yield]
//   CallExpression[?Yield] [ Expression[In, ?Yield] ]
//   CallExpression[?Yield] . IdentifierName
//   CallExpression[?Yield] TemplateLiteral[?Yield]
//
template <typename UCharInputIterator>
ir::Node* ExpressionParser<UCharInputIterator>::ParseCallExpression() {
  PARSER_ENTER(ParseCallExpression);
  ir::Node* target;
  if (parser()->Current()->type() == Token::TS_SUPER) {
    target = parser()->New<ir::SuperView>();
    target->SetInformationForNode(parser()->Current());
    parser()->Next();
    if (parser()->Current()->type() == Token::TS_DOT) {
      parser()->Next();
      ir::Node* literal = ParseLiteral();
      if (literal->HasNameView()) {
        target = parser()->New<ir::GetPropView>(target, literal);
        target->SetInformationForNode(literal);
      } else {
        PARSER_SYNTAX_ERROR_POS("SyntaxError unexpected token", literal->source_position());
      }
    }
  } else {
    target = ParseMemberExpression();
  }

  ir::Node* type_arguments = nullptr;

  if (parser()->Current()->type() == Token::TS_LESS) {
    type_arguments = parser()->ParseTypeArguments();
  }
  
  if (parser()->Current()->type() == Token::TS_LEFT_PAREN) {
    ir::Node* args = ParseArguments();
    ir::Node* call = parser()->New<ir::CallView>(target, args, type_arguments);
    call->SetInformationForNode(target);
    type_arguments = nullptr;
    while (1) {
      switch (parser()->Current()->type()) {
        case Token::TS_LESS: {
          type_arguments = ParseTypeArguments();
        }
        case Token::TS_LEFT_PAREN: {
          ir::Node* args = ParseArguments();
          call = parser()->New<ir::CallView>(call, args, type_arguments);
          call->SetInformationForNode(args);
          type_arguments = nullptr;
          break;
        }
        case Token::TS_LEFT_BRACE:
        case Token::TS_DOT:
          if (type_arguments != nullptr) {
            PARSER_SYNTAX_ERROR_POS("SyntaxError unexpected token", type_arguments->source_position());
          }
          call = ParseGetPropOrElem(call);
          break;
        default:
          return call;
      }
    }
  } else if (parser()->Current()->type() == Token::TS_BACKQUOTE) {
    ir::Node* template_literal = ParseTemplateLiteral(yield);
    ir::Node* call = parser()->New<ir::CallView>(target, template_literal, type_arguments);
    call->SetInformationForNode(target);
    return call;
  }
  return target;
}


// Arguments[Yield]
//   ( )
//   ( ArgumentList[?Yield] )
//
// ArgumentList[Yield]
//   AssignmentExpression[In, ?Yield]
//   ... AssignmentExpression[In, ?Yield]
//   ArgumentList[?Yield] , AssignmentExpression[In, ?Yield]
//   ArgumentList[?Yield] , ... AssignmentExpression[In, ?Yield]
//
template <typename UCharInputIterator>
ir::Node* ExpressionParser<UCharInputIterator>::ParseArguments(bool yield) {
  PARSER_ENTER(ParseArguments);
  if (parser()->Current()->type() == Token::TS_LEFT_PAREN) {
    ir::CallArgsView* args = parser()->New<ir::CallArgsView>();
    args->SetInformationForNode(parser()->Current());
    parser()->Next();
    if (parser()->Current()->type() == Token::TS_RIGHT_PAREN) {
      parser()->Next();
      return args;
    }
    bool has_rest = false;
    while (1) {
      if (parser()->Current()->type() == Token::TS_REST) {
        TokenCursor cursor = parser()->GetBufferCursorPosition();
        parser()->Next();
        ir::Node* expr = ParseAssignmentExpression(true, yield);
        auto rest = parser()->New<ir::RestParamView>(expr);
        rest->SetInformationForNode(parser()->PeekBuffer(cursor));
        args->InsertLast(rest);
        has_rest = true;
      } else {
        args->InsertLast(ParseAssignmentExpression(true, yield));
      }
      if (parser()->Current()->type() == Token::TS_COMMA) {
        if (has_rest) {
          PARSER_SYNTAX_ERROR("SyntaxError the spread argument must be the end of arguments", parser()->Current());
        }
        continue;
      } else if (parser()->Current()->type() == Token::TS_RIGHT_PAREN) {
        parser()->Next();
        return args;
      }
      PARSER_SYNTAX_ERROR("SyntaxError unexpected token in 'arguments'", parser()->Current());
    }
  }
  return nullptr;
}


// MemberExpression[Yield]
//   [Lexical goal InputElementRegExp] PrimaryExpression[?Yield]
//   MemberExpression[?Yield] [ Expression[In, ?Yield] ]
//   MemberExpression[?Yield] . IdentifierName
//   MemberExpression[?Yield] TemplateLiteral[?Yield]
//   super [ Expression[In, ?Yield] ]
//   super . IdentifierName
//   new super TypeArguments(opt) Arguments[?Yield](opt)
//   new [ lookahead  { super } ] MemberExpression[?Yield]
//
template <typename UCharInputIterator>
ir::Node* ExpressionParser<UCharInputIterator>::ParseMemberExpression(bool yield) {
  PARSER_LOG_PHASE(ParseMemberExpression);
  // Not advance scanner.
  TokenInfo* token_info = parser()->Current();
  ir::Node* node;
  if (token_info->type() ==  Token::TS_NEW) {
    TokenCursor cursor = parser()->GetBufferCursorPosition();
    // Parse new Foo() expression.
    parser()->Next();
    ir::Node* member = nullptr;
    if (parser()->Current()->type() == Token::TS_SUPER) {
      member = parser()->New<ir::SuperView>();
      member->SetInformationForNode(parser()->PeekBuffer(cursor));
    } else {
      member = ParseMemberExpression(yield);
    }

    ir::Node* type_arguments = nullptr;
    if (parser()->Current()->type() == Token::TS_LESS) {
      type_arguments = ParseTypeArguments();
    }

    // New expression can omit parens.
    // If paren exists, continue parsing.
    if (parser()->Current()->type() == Token::TS_LEFT_PAREN) {
      ir::Node* args = ParseArguments(yield);
      node = parser()->New<ir::NewCallView>(member, args, type_arguments);
      node->SetInformationForNode(member);
      return ParseGetPropOrElem(node);
    } else {
      // Parens are not exists.
      // Immediate return.
      ir::Node* ret = parser()->New<ir::NewCallView>(member, nullptr, type_arguments);
      ret->SetInformationForNode(member);
      return ret;
    }
  } else if (token_info->type() == Token::TS_SUPER) {
    auto super = parser()->New<ir::SuperView>();
    super->SetInformationForNode(token_info);
    return ParseGetPropOrElem(super);
  } else {
    return ParseGetPropOrElem(ParsePrimaryExpression(yield));
  }
}


// Parse member expression suffix.
// Like 'new foo.bar.baz()', 'new foo["bar"]', '(function(){return {a:1}}).a'
template <typename UCharInputIterator>
ir::Node* ExpressionParser<UCharInputIterator>::ParseGetPropOrElem(ir::Node* node, bool yield) {
  PARSER_ENTER(ParseGetPropOrElem);
  
  switch (parser()->Current()->type()) {
    case Token::TS_LEFT_BRACKET: {
      // [...] expression.
      parser()->Next();
      ir::Node* expr = ParseExpression(true, false);
      ir::Node* result = parser()->New<ir::GetElemView>(node, expr);
      result->SetInformationForNode(node);
      if (parser()->Current()->type() != Token::TS_RIGHT_BRACKET) {
        PARSER_SYNTAX_ERROR("SyntaxError unexpected token", parser()->Current());
      }
      return result;
    }
    case Token::TS_DOT: {
      // a.b.c expression.
      parser()->Next();
      ir::Node* expr = ParseMemberExpression();
      if (!expr->HasNameView() && !expr->HasKeywordLiteralView() && !expr->HasGetPropView() && !expr->HasGetElemView()) {
        PARSER_SYNTAX_ERROR_POS("SyntaxError identifier expected", expr->source_position());
      }
      ir::Node* ret = parser()->New<ir::GetPropView>(node, expr);
      ret->SetInformationForNode(node);
      return ret;
    }
    default:
      return node;
  }
}


// PrimaryExpression[Yield]
//   this
//   IdentifierReference[?Yield]
//   Literal
//   ArrayInitializer[?Yield]
//   ObjectLiteral[?Yield]
//   FunctionExpression
//   ClassExpression
//   GeneratorExpression
//   GeneratorComprehension[?Yield]
//   RegularExpressionLiteral
//   TemplateLiteral[?Yield]
//   ( Expression[In, ?Yield] )
//
template <typename UCharInputIterator>
ir::Node* ExpressionParser<UCharInputIterator>::ParsePrimaryExpression(bool yield) {
  PARSER_LOG_PHASE(ParsePrimaryExpression);

  TokenInfo* token_info = nullptr;
  
  // Allow regular expression in this context.
  TokenInfo* maybe_regexp = parser()->CheckRegularExpression();
  if (maybe_regexp) {
    token_info = maybe_regexp;
  } else {
    token_info = parser()->Current();
  }
  
  switch (token_info->type()) {
    case Token::TS_IDENTIFIER: {
      return ParseIdentifierReference(yield);
    }
    case Token::TS_THIS: {
      // parse a this.
      ir::Node* this_view = New<ir::ThisView>();
      this_view->SetInformationForNode(token_info);
      parser()->Next();
      return this_view;
    }
    case Token::TS_LEFT_BRACE:
      // parse an object literal.
      return ParseObjectLiteral(yield);
    case Token::TS_LEFT_BRACKET:
      // parse an array literal.
      return ParseArrayInitializer(yield);
    case Token::TS_LEFT_PAREN: {
      parser()->Next();
      try {
        // First, try parse as parenthesized expression.
        ir::Node* node = ParseExpression(true, false);
        if (parser()->Current()->type() == Token::TS_RIGHT_PAREN) {
          parser()->Next();
          return node;
        }
      } catch (SyntaxError& e) {
        // If SyntaxError thrown, try parse as generator comprehensions.
        try {
          return ParseGeneratorComprehension(yield);
        } catch (GeneratorSyntaxError& e) {
          // If GeneratorSyntaxError thrown, throw error.
          throw e;
        } catch (SyntaxError& _) {
          // If Generic SyntaxError thrown, throw previous error.
          throw e;
        } catch (std::exception& e) {
          throw e;
        }
      } catch (std::exception& e) {
        throw e;
      }
      SYNTAX_ERROR("SyntaxError ')' expected", parser()->Current());
    }
    case Token::TS_REGULAR_EXPR: {
      auto reg_expr = parser()->New<ir::RegularExprView>(token_info->value());
      reg_expr->SetInformationForNode(token_info);
      return reg_expr;
    }
    case Token::TS_TEMPLATE_LITERAL: {
      auto template_literal = parser()->New<ir::TemplateLiteral>(token_info->value());
      template_literal->SetInformationForNode(token_info);
    }
    case Token::TS_FUNCTION:
      return ParseFunction();
    case Token::TS_CLASS:
      return ParseClassExpression(yield, false);
    default:
      // parse a literal.
      return ParseLiteral();
  }
}


template<typename UCharInputIterator>
ir::Node* ExpressionParser<UCharInputIterator>::ParseArrayLiteral(bool yield) {
  PARSER_LOG_PHASE(ParseArrayLiteral);
  if (parser()->Current()->type() == Token::TS_LEFT_BRACKET) {
    ir::ArrayLiteralView* array_literal = parser()->New<ir::ArrayLiteralView>();
    array_literal->SetInformationForNode(parser()->Current());
    parser()->Next();
    while (1) {
      ir::Node* expr = nullptr;
      bool spread = false;
      if (parser()->Current()->type() == Token::TS_COMMA) {
        expr = parser()->New<ir::UndefinedView>();
        expr->SetInformationForNode(parser()->Current());
        parser()->Next();
      } else if (parser()->Current()->type() == Token::TS_REST) {
        expr = ParseSpreadElement(yield);
        spread = true;
      } else {
        expr = ParseAssignmentExpression(true, yield);
      }
      array_literal->InsertLast(expr);
      if (parser()->Current()->type() == Token::TS_COMMA) {
        if (spread) {
          PARSER_SYNTAX_ERROR("SyntaxError array spread element must be the end of the array element list",
                              parser()->Current());
        }
        parser()->Next();
      } else if (parser()->Current()->type() == Token::TS_RIGHT_BRACKET) {
        parser()->Next();
        break;
      } else {
        SYNTAX_ERROR("SyntaxError unexpected token in 'array literal'", parser()->Current());
      }
    }
    return array_literal;
  }
  SYNTAX_ERROR("Unexpected token.", Current());
}


template<typename UCharInputIterator>
ir::Node* ExpressionParser<UCharInputIterator>::ParseSpreadElement(bool yield) {
  if (parser()->Current()->type() == Token::TS_REST) {
    parser()->Next();
    return ParseAssignmentExpression(true, yield);
  }
  SYNTAX_ERROR("SyntaxError '...' expected", parser()->Current());
}


template<typename UCharInputIterator>
ir::Node* ExpressionParser<UCharInputIterator>::ParseArrayComprehension(bool yield) {
  if (parser()->Current()->type() == Token::TS_LEFT_BRACKET) {
    parser()->Next();
    ir::Node* expr = ParseComprehension(yield);
    if (parser()->Current()->type() == Token::TS_RIGHT_BRACKET) {
      parser()->Next();
      return expr;
    }
    SYNTAX_ERROR("SyntaxError ']' expected", parser()->Current());
  }
  SYNTAX_ERROR("SyntaxError '[' expected", parser()->Current());
}


template<typename UCharInputIterator>
ir::Node* ExpressionParser<UCharInputIterator>::ParseComprehension(bool yield) {
  ir::Node* comp_for = ParseComprehensionFor(yield);
  ir::Node* comp_tail = ParseComprehensionTail(yield);
  auto expr = parser()->New<ir::ComprehensionExprView>(comp_for, comp_tail);
  expr->SetInformationForNode(comp_for);
  return expr;
};


template<typename UCharInputIterator>
ir::Node* ExpressionParser<UCharInputIterator>::ParseComprehensionTail(bool yield) {
  if (parser()->Current()->type() == Token::TS_FOR) {
    ir::Node* for_expr = ParseComprehensionFor(yield);
    ir::ForStatementView* stmt = for_expr->ToForStatementView();
    stmt->set_body(ParseComprehensionTail(yield));
    return stmt;
  } else if (parser()->Current()->type() == Token::TS_IF) {
    ir::Node* if_expr = ParseComprehensionIf(yield);
    ir::IfStatementView* stmt = if_expr->ToIfStatementView();
    stmt->set_body(ParseComprehensionTail(yield));
    return stmt;
  }
  return ParseAssignmentExpression(true, yield);
}


template<typename UCharInputIterator>
ir::Node* ExpressionParser<UCharInputIterator>::ParseComprehensionFor(bool yield) {
  if (parser()->Current()->type() == Token::TS_FOR) {
    TokenCursor cursor = parser()->GetBufferCursorPosition();
    parser()->Next();
    if (parser()->Current()->type() == Token::TS_LEFT_PAREN) {
      parser()->Next();
      ir::Node* for_bindig = ParseForBinding();
      if (parser()->Current()->type() == Token::TS_IDENTIFIER &&
          parser()->Current()->value() == "of") {
        parser()->Next();
        ir::Node* expr = ParseAssignmentExpression(true, yield);
        if (parser()->Current()->type() == Token::TS_RIGHT_PAREN) {
          parser()->Next();
          auto for_expr = parser()->New<ir::ForOfStatementView>();
          for_expr->set_property_name(for_bindig);
          for_expr->set_expr(expr);
          for_expr->SetInformationForNode(parser()->PeekBuffer(cursor));
          return for_expr;
        }
        PARSER_SYNTAX_ERROR("SyntaxError ')' expected", parser()->Current());
      }
      PARSER_SYNTAX_ERROR("SyntaxError 'of' expected", parser()->Current());
    }
    PARSER_SYNTAX_ERROR("SyntaxError '(' expected", parser()->Current());
  }
  PARSER_SYNTAX_ERROR("SyntaxError 'for' expected", parser()->Currn());
}


template<typename UCharInputIterator>
ir::Node* ExpressionParser<UCharInputIterator>::ParseComprehensionIf(bool yield) {
  if (parser()->Current()->type() == Token::TS_IF) {
    TokenCursor cursor = parser()->GetBufferCursorPosition();
    parser()->Next();
    if (parser()->Current()->type() == Token::TS_LEFT_PAREN) {
      parser()->Next();
      ir::Node* expr = ParseAssignmentExpression(true, yield);
      if (parser()->Current()->type() == Token::TS_RIGHT_PAREN) {
        parser()->Next();
        auto if_expr = parser()->New<ir::IfStatementView>();
        if_expr->set_expr(expr);
        if_expr->SetInformationForNode(parser()->PeekBuffer(cursor));
        return if_expr;
      }
      PARSER_SYNTAX_ERROR("SyntaxError ')' expected", parser()->Current());
    }
    PARSER_SYNTAX_ERROR("SyntaxError '(' expected", parser()->Current());
  }
  PARSER_SYNTAX_ERROR("SyntaxError 'if' expected", parser()->Current());
}


template<typename UCharInputIterator>
ir::Node* ExpressionParser<UCharInputIterator>::ParseGeneratorComprehension(bool yield) {
  if (parser()->Current()->type() == Token::TS_LEFT_PAREN) {
    parser()->Next();
    ir::Node* comp = ParseComprehension(yield);
    if (parser()->Current()->type() == Token::TS_RIGHT_PAREN) {
      return comp;
    }
    PARSER_SYNTAX_ERROR("SyntaxError ')' expected", parser()->Current());
  }
  PARSER_SYNTAX_ERROR("SyntaxError '(' expected", parser()->Current());
}


template<typename UCharInputIterator>
ir::Node* ExpressionParser<UCharInputIterator>::ParseForBinding(bool yield) {
  switch (parser()->Current()->type()) {
    case Token::TS_LEFT_BRACE: // FALL THROUGH
    case Token::TS_LEFT_BRACKET:
      return ParseBindingPattern(yield);
    default:
      return ParseBindingIdentifier(yield);
  }
}


template <typename UCharInputIterator>
ir::Node* ExpressionParser<UCharInputIterator>::ParseObjectLiteral(bool yield) {
  PARSER_LOG_PHASE(ParseObjectLiteral);
  if (parser()->Current()->type() == Token::TS_LEFT_BRACE) {
    ir::ObjectLiteralView* object_literal = parser()->New<ir::ObjectLiteralView>();
    object_literal->SetInformationForNode(parser()->Current());
    parser()->Next();
    while (1) {
      ir::Node* element = ParsePropertyDefinition(yield);
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
}

template <typename UCharInputIterator>
ir::Node* ExpressionParser<UCharInputIterator>::ParsePropertyDefinition(bool yield) {
  ir::Node* value = nullptr;
  ir::Node* key = nullptr;
  bool generator = false;
  bool getter = false;
  bool setter = false;
  TokenCursor cursor = parser()->GetBufferCursorPosition();
   
  if (parser()->Current()->type() == Token::TS_IDENTIFIER &&
      parser()->Current()->value() == "get") {
      getter = true;
      parser()->Next();
  } else if (parser()->Current()->type() == Token::TS_IDENTIFIER &&
             parser()->Current()->value() == "set") {
      setter = true;
      parser()->Next();
  }

  try {
    if (parser()->Current()->type() == Token::TS_IDENTIFIER) {
      key = ParseIdentifierReference(yield);
    } else {
      key = ParsePropertyName(yield);
    }
  } catch (const SyntaxError& e) {
    if (getter || setter) {
      key = parser()->PeekBuffer(cursor);
    } else {
      throw e;
    }
  }

  if (parser()->Current()->type() == Token::TS_MUL) {
    generator = true;
    parser()->Next();
  }
  
  if (parser()->Current()->type() == Token::TS_LEFT_PAREN) {
    ir::Node* call_sig = ParseCallSignature(yield);
    if (parser()->Current()->type() == Token::TS_LEFT_BRACE) {
      parser()->Next();
      ir::Node* body = ParseFunctionBody(yield || generator);
      value = parser()->New<ir::FunctionView>(getter, setter, generator, nullptr, call_sig, body);
    }
  } else if (generator) {
    PARSER_SYNTAX_ERROR("SyntaxError '(' expected", parser()->Current());
  } else if (parser()->Current()->type() == Token::TS_COLON) {
    parser()->Next();
    value = ParseAssignmentExpression(true, false);
  }
  ir::ObjectElementView* element = New<ir::ObjectElementView>(key, value);
  element->SetInformationForNode(parser()->PeekBuffer(cursor));
  return element;
}
}
