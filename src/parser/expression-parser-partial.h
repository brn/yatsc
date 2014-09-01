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

// Expression[In, Yield]
//   AssignmentExpression[?In, ?Yield]
//   Expression[?In, ?Yield] , AssignmentExpression[?In, ?Yield]
//
template <typename UCharInputIterator>
ir::Node* Parser<UCharInputIterator>::ParseExpression(bool in, bool yield) {
  LOG_PHASE(ParseExpression);
  ir::Node* assignment_expr = ParseAssignmentExpression(in, yield);

  // Parse comma expressions.
  if (Current()->type() == Token::TS_COMMA) {
    Next();
    ir::CommaExprView* comma_expr = New<ir::CommaExprView>({assignment_expr});
    comma_expr->SetInformationForNode(*Current());
    
    while (1) {
      assignment_expr = ParseAssignmentExpression(in, yield);
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


// AssignmentPattern[Yield]
//   ObjectAssignmentPattern[?Yield]
//   ArrayAssignmentPattern[?Yield]
//
template <typename UCharInputIterator>
ir::Node* Parser<UCharInputIterator>::ParseAssignmentPattern(bool yield) {
  LOG_PHASE(ParseAssignmentPattern);
  TokenCursor cursor = GetBufferCursorPosition();
  ir::Node* node = nullptr;
  switch(Current()->type()) {
    case Token::TS_LEFT_BRACE: {
      node = ParseObjectAssignmentPattern(yield);
      break;
    }
    case Token::TS_LEFT_BRACKET: {
      node = ParseArrayAssignmentPattern(yield);
      break;
    }
    default:
      SYNTAX_ERROR("SyntaxError unexpected token", PeekBuffer(cursor));
  }

  node->SetInformationForNode(PeekBuffer(cursor));
  return node;
}


// ObjectAssignmentPattern[Yield]
//   { }
//   { AssignmentPropertyList[?Yield] }
//   { AssignmentPropertyList[?Yield] , }
//
template <typename UCharInputIterator>
ir::Node* Parser<UCharInputIterator>::ParseObjectAssignmentPattern(bool yield) {
  LOG_PHASE(ParseObjectAssignmentPattern);
  if (Current()->type() == Token::TS_LEFT_BRACE) {
    Next();
    ir::Node* node = ParseAssignmentPropertyList(yield);
    if (Current()->type() == Token::TS_COMMA) {
      Next();
    }
    if (Current()->type() == Token::TS_RIGHT_BRACE) {
      Next();
      return node;
    }
    SYNTAX_ERROR("SyntaxError unexpected token", Current());
  }
  SYNTAX_ERROR("SyntaxError '{' expected", Current());
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
ir::Node* Parser<UCharInputIterator>::ParseArrayAssignmentPattern(bool yield) {
  LOG_PHASE(ParseArrayAssignmentPattern);
  if (Current()->type() == Token::TS_LEFT_BRACKET) {
    bool has_rest = false;
    auto array_view = New<ir::BindingArrayView>();
    array_view->SetInformationForNode(Current());
    Next();


    // '[, ...' pattern
    if (Current()->type() == Token::TS_COMMA) {
      Next();
      array_view->InsertLast(New<ir::UndefinedView>());
      array_view->InsertLast(New<ir::UndefinedView>());
    }
    
    if (Current()->type() == Token::TS_REST) {
      array_view->InsertLast(ParseAssignmentRestElement(yield));
      has_rest = true;
    }

    if (Current()->type() == Token::TS_RIGHT_BRACKET) {
      if (array_view->size() == 0) {
        SYNTAX_ERROR("SyntaxError destructuring assignment left hand side is not allowed empty array", Current());
      }
      Next();
      return array_view;
    }

    // The ParameterRest is not allowed in any position of array pattern except the last element.
    if (has_rest) {
      SYNTAX_ERROR("SyntaxError destructuring assignment rest must be the end of element", Current());
    }

    while (1) {
      if (Current()->type() == Token::TS_COMMA) {
        array_view->InsertLast(New<ir::UndefinedView>());
        array_view->InsertLast(New<ir::UndefinedView>());
        Next();
      } else {
        array_view->InsertLast(ParseAssignmentElement(yield));
      }
      switch (Current()->type()) {
        case Token::TS_COMMA:
          Next();
          break;
        case Token::TS_RIGHT_BRACKET:
          Next();
          return array_view;
        default:
          SYNTAX_ERROR("SyntaxError unexpected token", Current());
      }
    }
  }
  SYNTAX_ERROR("SyntaxError unexpected token", Current());
}


// Parse destructuring assignment object pattern properties.
// 
// AssignmentPropertyList[Yield]
//   AssignmentProperty[?Yield]
//   AssignmentPropertyList[?Yield] , AssignmentProperty[?Yield]
//
template <typename UCharInputIterator>
ir::Node* Parser<UCharInputIterator>::ParseAssignmentPropertyList(bool yield) {
  LOG_PHASE(ParseAssignmentPropertyList);
  auto prop_list = New<ir::BindingPropListView>();
  while (1) {
    prop_list->InsertLast(ParseAssignmentProperty(yield));
    if (Current()->type() == Token::TS_COMMA) {
      Next();
      continue;
    }
    if (Current()->type() == Token::TS_RIGHT_BRACE) {
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
ir::Node* Parser<UCharInputIterator>::ParseAssignmentProperty(bool yield) {
  LOG_PHASE(ParseAssignmentProperty);
  TokenCursor cursor = GetBufferCursorPosition();
  
  ir::Node* property_name = nullptr;
  ir::Node* elem = nullptr;

  // Check whether property name is identifier reference or not.
  bool identifier = false;
  
  if ((yield && Current()->type() == Token::TS_YIELD) ||
      Current()->type() == Token::TS_IDENTIFIER) {
    identifier = true;
    property_name = ParseIdentifierReference(yield);
  } else if (Current()->type() == Token::TS_STRING_LITERAL ||
             Current()->type() == Token::TS_NUMERIC_LITERAL ||
             Current()->type() == Token::TS_LEFT_BRACKET) {
    property_name = ParsePropertyName(yield, false);
  }
  
  ir::Node* init = nullptr;

  // Initializer is only allowed, if property name is identifier reference.
  if (identifier && Current()->type() == Token::TS_ASSIGN) {
    Next();
    init = ParseAssignmentExpression(true, yield);
  } else if (Current()->type() == Token::TS_COLON) {
    Next();
    elem = ParseAssignmentElement(yield);
  }

  // All destructuring assignment element is convert to BindingElementView.
  auto binding_element = New<ir::BindingElementView>(property_name, elem, init);
  binding_element->SetInformationForNode(PeekBuffer(cursor));
  return binding_element;
}


// AssignmentElement[Yield]
//   DestructuringAssignmentTarget[?Yield] Initializer[In,?Yield](opt)
//
template <typename UCharInputIterator>
ir::Node* Parser<UCharInputIterator>::ParseAssignmentElement(bool yield) {
  LOG_PHASE(ParseAssignmentElement);
  ir::Node* target = ParseDestructuringAssignmentTarget(yield);
  ir::Node* init = nullptr;
  if (Current()->type() == Token::TS_ASSIGN) {
    Next();
    init = ParseAssignmentExpression(true, yield);
  }

  // All destructuring assignment element is convert to BindingElementView.
  auto binding_element = New<ir::BindingElementView>(nullptr, target, init);
  binding_element->SetInformationForNode(target);
  return binding_element;
}


// AssignmentRestElement[Yield]
//   ... DestructuringAssignmentTarget[?Yield]
//
template <typename UCharInputIterator>
ir::Node* Parser<UCharInputIterator>::ParseAssignmentRestElement(bool yield) {
  LOG_PHASE(ParseAssignmentRestElement);
  if (Current()->type() == Token::TS_REST) {
    TokenCursor cursor = GetBufferCursorPosition();
    Next();
    ir::Node* target = ParseDestructuringAssignmentTarget(yield);
    auto rest = New<ir::RestParamView>(target);
    rest->SetInformationForNode(PeekBuffer(cursor));
  }
  SYNTAX_ERROR("SyntaxError '...' expected", Current());
}


// DestructuringAssignmentTarget[Yield]
//   LeftHandSideExpression[?Yield]
//
template <typename UCharInputIterator>
ir::Node* Parser<UCharInputIterator>::ParseDestructuringAssignmentTarget(bool yield) {
  LOG_PHASE(ParseDestructuringAssignmentTarget);
  TokenCursor cursor = GetBufferCursorPosition();
  ir::Node* ret = ParseLeftHandSideExpression(yield);
  // Check whether DestructuringAssignmentTarget is IsValidAssignmentTarget or not.
  if (!ret->IsValidLhs()) {
    if (ret->HasObjectLiteralView() || ret->HasArrayLiteralView()) {
      SetBufferCursorPosition(cursor);
      ret = ParseAssignmentPattern(yield);
    } else {
      SYNTAX_ERROR_POS("SyntaxError invalid Left-Hand-Side expression", ret->source_position());
    }
  }
  return ret;
}



// Return true if token type is assignment operator.
bool IsAssignmentOp(Token type) {
  return type == Token::TS_ASSIGN || type == Token::TS_MUL_LET ||
    type == Token::TS_DIV_LET || type == Token::TS_MOD_LET ||
    type == Token::TS_ADD_LET || type == Token::TS_SUB_LET ||
    type == Token::TS_SHIFT_LEFT_LET || type == Token::TS_SHIFT_RIGHT_LET ||
    type == Token::TS_U_SHIFT_RIGHT_LET || type == Token::TS_AND_LET ||
    type == Token::TS_NOR_LET || type == Token::TS_OR_LET ||
    type == Token::TS_XOR_LET;
}


// AssignmentExpression[In, Yield]
//   ConditionalExpression[?In, ?Yield]
//   [+Yield] YieldExpression[?In]
//   ArrowFunction[?In, ?Yield]
//   LeftHandSideExpression[?Yield] = AssignmentExpression[?In,?Yield]
//   LeftHandSideExpression[?Yield] AssignmentOperator AssignmentExpression[?In, ?Yield]
//  
template <typename UCharInputIterator>
ir::Node* Parser<UCharInputIterator>::ParseAssignmentExpression(bool in, bool yield) {
  LOG_PHASE(ParseAssignmentExpression);
  ir::Node* node = nullptr;

  // Record current buffer position.
  TokenCursor cursor = GetBufferCursorPosition();
  
  if (Current()->type() == Token::TS_LEFT_PAREN ||
      Current()->type() == Token::TS_LESS) {
    // First try parse as arrow function.
    bool failed = false;
    try {
      // parsae an arrow_function_parameters.
      node = ParseArrowFunctionParameters(yield);
    } catch (const SyntaxError&) {
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
      return ParseConciseBody(in, node);
    }
    
    SetBufferCursorPosition(cursor);
  }

  ir::Node* expr = nullptr;
  bool parsed_as_assignment_pattern = false;

  if (Current()->type() == Token::TS_YIELD) {
    if (!yield) {
      SYNTAX_ERROR("SyntaxError invalid use of 'yield' keyword", Current());
    }
    expr = ParseYieldExpression(in);
  } else {  
    try {
      expr = ParseConditionalExpression(in, yield);
    } catch (const SyntaxError& e) {

      if (!LanguageModeUtil::IsES6(compiler_option_)) {
        throw e;
      }
      
      SetBufferCursorPosition(cursor);
      expr = ParseAssignmentPattern(yield);
      parsed_as_assignment_pattern = true;
    }
  }

  if (expr->HasNameView() && Current()->type() == Token::TS_ARROW_GLYPH) {
    return ParseArrowFunction(in, yield, expr);
  }
  
  // Expression is not an arrow_function.
  TokenInfo *token_info = Current();
  Token type = token_info->type();

  // Check assignment operators.
  if (IsAssignmentOp(type)) {
    if (!parsed_as_assignment_pattern &&
        (expr->HasObjectLiteralView() ||
         expr->HasArrayLiteralView())) {

      if (!LanguageModeUtil::IsES6(compiler_option_)) {
        SYNTAX_ERROR_POS("Invalid Left-Hand-Side expression", expr->source_position());
      }
      
      SetBufferCursorPosition(cursor);
      expr = ParseAssignmentPattern(yield);
    }
    Next();

    // Check left hand side expression is valid as an expression.
    // If left hand side expression is like 'func()',
    // that is invalid expression.
    if (expr->IsValidLhs()) {
      ir::Node* rhs = ParseAssignmentExpression(in, yield);
      ir::Node* result = New<ir::AssignmentView>(type, expr, rhs);
      result->SetInformationForNode(expr);
      return result;
    }
    SYNTAX_ERROR("SyntaxError invalid left hand side expression in 'assignment expression'", Current());
  } else if (parsed_as_assignment_pattern) {
    SYNTAX_ERROR("SyntaxError destructuring assignment must be initialized", Current());
  }
  return expr;
}


template <typename UCharInputIterator>
ir::Node* Parser<UCharInputIterator>::ParseArrowFunction(bool in, bool yield, ir::Node* identifier) {
  LOG_PHASE(ParseArrowFunction);
  ir::Node* call_sig = ParseArrowFunctionParameters(yield, identifier);
  return ParseConciseBody(in, call_sig);
}


template <typename UCharInputIterator>
ir::Node* Parser<UCharInputIterator>::ParseArrowFunctionParameters(bool yield, ir::Node* identifier) {
  LOG_PHASE(ParseArrowFunction);

  ir::Node* call_sig = nullptr;
  
  if (identifier != nullptr) {
    call_sig = New<ir::CallSignatureView>(identifier, nullptr, nullptr);
    call_sig->SetInformationForNode(identifier);
  } else {  
    call_sig = ParseCallSignature(false);
  }
  if (Current()->type() != Token::TS_ARROW_GLYPH) {
    SYNTAX_ERROR("SyntaxError '=>' expected", Current());
  }
  Next();
  return call_sig;
}


template <typename UCharInputIterator>
ir::Node* Parser<UCharInputIterator>::ParseConciseBody(bool in, ir::Node* call_sig) {
  ir::Node* body;
  if (Current()->type() == Token::TS_LEFT_BRACE) {
    Next();
    body = ParseFunctionBody(false);
  } else {
    body = ParseAssignmentExpression(true, false);
  }
  ir::Node* ret = New<ir::ArrowFunctionView>(call_sig, body);
  ret->SetInformationForNode(call_sig);
  return ret;
}


// ConditionalExpression[In, Yield]
//   LogicalORExpression[?In, ?Yield]
//   LogicalORExpression[?In,?Yield] ? AssignmentExpression[In, ?Yield] : AssignmentExpression[?In, ?Yield]
//
template <typename UCharInputIterator>
ir::Node* Parser<UCharInputIterator>::ParseConditionalExpression(bool in, bool yield) {
  LOG_PHASE(ParseConditionalExpression);
  ir::Node* expr = ParseBinaryExpression(in, yield);
  if (Current()->type() == Token::TS_QUESTION_MARK) {
    Next();
    ir::Node* left = ParseAssignmentExpression(in, yield);
    if (Current()->type() == Token::TS_COLON) {
      Next();
      ir::Node* right = ParseAssignmentExpression(in, yield);
      ir::TemaryExprView* temary = New<ir::TemaryExprView>(expr, left, right);
      temary->SetInformationForNode(expr);
      temary->MarkAsInValidLhs();
      return temary;
    }
    SYNTAX_ERROR("SyntaxError unexpected token in 'temary expression'", Current());
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
  LOG_PHASE(ParseBinaryExpression);
  ir::Node* last = nullptr;
  ir::Node* first = nullptr;
  ir::Node* lhs = ParseUnaryExpression(yield);
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
        ir::Node* rhs = ParseUnaryExpression(yield);
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
          Next();
          ir::Node* rhs = ParseUnaryExpression(yield);
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
        ir::Node* rhs = ParseUnaryExpression(yield);
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
ir::Node* Parser<UCharInputIterator>::ParseUnaryExpression(bool yield) {
  LOG_PHASE(ParseUnaryExpression);
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
ir::Node* Parser<UCharInputIterator>::ParsePostfixExpression(bool yield) {
  LOG_PHASE(ParsePostfixExpression);
  ir::Node* node = ParseLeftHandSideExpression(yield);
  if (Current()->type() == Token::TS_INCREMENT ||
      Current()->type() == Token::TS_DECREMENT) {
    ir::Node* ret = New<ir::PostfixView>(node, Current()->type());
    ret->SetInformationForNode(node);
    Next();
    return ret;
  }
  return node;
}


// Parse left-hand-side-expression
// left_hand_side_expression
// 	: new_expression(omited) -> member_expression
// 	| call_expression
// 	;
template <typename UCharInputIterator>
ir::Node* Parser<UCharInputIterator>::ParseLeftHandSideExpression(bool yield) {
  LOG_PHASE(ParseLeftHandSideExpression); 
  
  if (Current()->type() == Token::TS_NEW) {
    return ParseMemberExpression(yield);
  }
  return ParseCallExpression(yield);
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
ir::Node* Parser<UCharInputIterator>::ParseCallExpression(bool yield) {
  LOG_PHASE(ParseCallExpression);
  ir::Node* target;
  if (Current()->type() == Token::TS_SUPER) {
    target = New<ir::SuperView>();
    target->SetInformationForNode(Current());
    Next();
    if (Current()->type() == Token::TS_DOT) {
      Next();
      ir::Node* literal = ParseLiteral();
      if (literal->HasNameView()) {
        target = New<ir::GetPropView>(target, literal);
        target->SetInformationForNode(literal);
      } else {
        SYNTAX_ERROR_POS("SyntaxError unexpected token", literal->source_position());
      }
    }
  } else {
    target = ParseMemberExpression(yield);
  }

  ir::Node* type_arguments = nullptr;

  if (Current()->type() == Token::TS_LESS) {
    TokenCursor cursor = GetBufferCursorPosition();
    try {
      type_arguments = ParseTypeArguments();
      target->MarkAsInValidLhs();
    } catch (const SyntaxError&) {
      SetBufferCursorPosition(cursor);
    }
  }
  
  if (Current()->type() == Token::TS_LEFT_PAREN) {
    target->MarkAsInValidLhs();
    ir::Node* args = ParseArguments(yield);
    ir::Node* call = New<ir::CallView>(target, args, type_arguments);
    call->SetInformationForNode(target);
    type_arguments = nullptr;
    while (1) {
      switch (Current()->type()) {
        case Token::TS_LESS: {
          type_arguments = ParseTypeArguments();
        }
        case Token::TS_LEFT_PAREN: {
          ir::Node* args = ParseArguments(yield);
          call = New<ir::CallView>(call, args, type_arguments);
          call->MarkAsInValidLhs();
          call->SetInformationForNode(args);
          type_arguments = nullptr;
          break;
        }
        case Token::TS_LEFT_BRACE:
        case Token::TS_DOT:
          if (type_arguments != nullptr) {
            SYNTAX_ERROR_POS("SyntaxError unexpected token", type_arguments->source_position());
          }
          call = ParseGetPropOrElem(call, yield);
          break;
        default:
          return call;
      }
    }
  } else if (Current()->type() == Token::TS_TEMPLATE_LITERAL) {
    ir::Node* template_literal = ParseTemplateLiteral();
    ir::Node* call = New<ir::CallView>(target, template_literal, type_arguments);
    call->SetInformationForNode(target);
    call->MarkAsInValidLhs();
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
ir::Node* Parser<UCharInputIterator>::ParseArguments(bool yield) {
  LOG_PHASE(ParseArguments);
  if (Current()->type() == Token::TS_LEFT_PAREN) {
    ir::CallArgsView* args = New<ir::CallArgsView>();
    args->SetInformationForNode(Current());
    Next();
    if (Current()->type() == Token::TS_RIGHT_PAREN) {
      Next();
      return args;
    }
    bool has_rest = false;
    while (1) {
      if (Current()->type() == Token::TS_REST) {
        TokenCursor cursor = GetBufferCursorPosition();
        Next();
        ir::Node* expr = ParseAssignmentExpression(true, yield);
        auto rest = New<ir::RestParamView>(expr);
        rest->SetInformationForNode(PeekBuffer(cursor));
        args->InsertLast(rest);
        has_rest = true;
      } else {
        args->InsertLast(ParseAssignmentExpression(true, yield));
      }
      if (Current()->type() == Token::TS_COMMA) {
        if (has_rest) {
          SYNTAX_ERROR("SyntaxError the spread argument must be the end of arguments", Current());
        }
        continue;
      } else if (Current()->type() == Token::TS_RIGHT_PAREN) {
        Next();
        return args;
      }
      SYNTAX_ERROR("SyntaxError unexpected token in 'arguments'", Current());
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
//   new [ lookahead != { super } ] MemberExpression[?Yield]
//
template <typename UCharInputIterator>
ir::Node* Parser<UCharInputIterator>::ParseMemberExpression(bool yield) {
  LOG_PHASE(ParseMemberExpression);
  // Not advance scanner.
  TokenInfo* token_info = Current();
  ir::Node* node;
  if (token_info->type() ==  Token::TS_NEW) {
    TokenCursor cursor = GetBufferCursorPosition();
    // Parse new Foo() expression.
    Next();
    ir::Node* member = nullptr;
    if (Current()->type() == Token::TS_SUPER) {
      member = New<ir::SuperView>();
      member->SetInformationForNode(PeekBuffer(cursor));
    } else {
      member = ParseMemberExpression(yield);
    }

    ir::Node* type_arguments = nullptr;
    if (Current()->type() == Token::TS_LESS) {
      type_arguments = ParseTypeArguments();
      member->MarkAsInValidLhs();
    }

    // New expression can omit parens.
    // If paren exists, continue parsing.
    if (Current()->type() == Token::TS_LEFT_PAREN) {
      ir::Node* args = ParseArguments(yield);
      node = New<ir::NewCallView>(member, args, type_arguments);
      node->SetInformationForNode(member);
      node->MarkAsInValidLhs();
      return ParseGetPropOrElem(node, yield);
    } else {
      // Parens are not exists.
      // Immediate return.
      ir::Node* ret = New<ir::NewCallView>(member, nullptr, type_arguments);
      ret->SetInformationForNode(member);
      return ret;
    }
  } else if (token_info->type() == Token::TS_SUPER) {
    auto super = New<ir::SuperView>();
    super->SetInformationForNode(token_info);
    return ParseGetPropOrElem(super, yield);
  } else {
    return ParseGetPropOrElem(ParsePrimaryExpression(yield), yield);
  }
}


// Parse member expression suffix.
// Like 'new foo.bar.baz()', 'new foo["bar"]', '(function(){return {a:1}}).a'
template <typename UCharInputIterator>
ir::Node* Parser<UCharInputIterator>::ParseGetPropOrElem(ir::Node* node, bool yield) {
  LOG_PHASE(ParseGetPropOrElem);
  
  switch (Current()->type()) {
    case Token::TS_LEFT_BRACKET: {
      // [...] expression.
      Next();
      ir::Node* expr = ParseExpression(true, false);
      ir::Node* result = New<ir::GetElemView>(node, expr);
      result->SetInformationForNode(node);
      if (Current()->type() != Token::TS_RIGHT_BRACKET) {
        SYNTAX_ERROR("SyntaxError unexpected token", Current());
      }
      return result;
    }
    case Token::TS_DOT: {
      // a.b.c expression.
      Next();
      ir::Node* expr = ParseMemberExpression(yield);
      if (!expr->HasNameView() && !expr->HasKeywordLiteralView() && !expr->HasGetPropView() && !expr->HasGetElemView()) {
        SYNTAX_ERROR_POS("SyntaxError identifier expected", expr->source_position());
      }
      ir::Node* ret = New<ir::GetPropView>(node, expr);
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
ir::Node* Parser<UCharInputIterator>::ParsePrimaryExpression(bool yield) {
  LOG_PHASE(ParsePrimaryExpression);

  TokenInfo* token_info = nullptr;
  
  // Allow regular expression in this context.
  TokenInfo* maybe_regexp = scanner_->CheckRegularExpression();
  if (maybe_regexp) {
    token_info = maybe_regexp;
  } else {
    token_info = Current();
  }
  
  switch (token_info->type()) {
    case Token::TS_IDENTIFIER: {
      return ParseIdentifierReference(yield);
    }
    case Token::TS_THIS: {
      // parse a this.
      ir::Node* this_view = New<ir::ThisView>();
      this_view->SetInformationForNode(token_info);
      Next();
      return this_view;
    }
    case Token::TS_LEFT_BRACE:
      // parse an object literal.
      return ParseObjectLiteral(yield);
    case Token::TS_LEFT_BRACKET:
      // parse an array literal.
      return ParseArrayInitializer(yield);
    case Token::TS_LEFT_PAREN: {
      Next();
      try {
        // First, try parse as parenthesized expression.
        ir::Node* node = ParseExpression(true, false);
        if (Current()->type() == Token::TS_RIGHT_PAREN) {
          Next();
          return node;
        }
      } catch (SyntaxError& e) {
        if (!LanguageModeUtil::IsES6(compiler_option_)) {
          throw e;
        }
        // If SyntaxError thrown, try parse as generator comprehensions.
        try {
          return ParseGeneratorComprehension(yield);
        // } catch (GeneratorSyntaxError& e) {
        //   // If GeneratorSyntaxError thrown, throw error.
        //   throw e;
        } catch (SyntaxError&) {
          // If Generic SyntaxError thrown, throw previous error.
          throw e;
        } catch (std::exception& e) {
          throw e;
        }
      } catch (std::exception& e) {
        throw e;
      }
      SYNTAX_ERROR("SyntaxError ')' expected", Current());
    }
    case Token::TS_REGULAR_EXPR: {
      return ParseRegularExpression();
    }
    case Token::TS_TEMPLATE_LITERAL: {
      return ParseTemplateLiteral();
    }
    case Token::TS_FUNCTION:
      return ParseFunction(yield, false);
    case Token::TS_CLASS:
      return ParseClassDeclaration(yield, false);
    default:
      // parse a literal.
      return ParseLiteral();
  }
}


template<typename UCharInputIterator>
ir::Node* Parser<UCharInputIterator>::ParseArrayLiteral(bool yield) {
  LOG_PHASE(ParseArrayLiteral);
  if (Current()->type() == Token::TS_LEFT_BRACKET) {
    ir::ArrayLiteralView* array_literal = New<ir::ArrayLiteralView>();
    array_literal->SetInformationForNode(Current());
    Next();
    while (1) {
      ir::Node* expr = nullptr;
      bool spread = false;
      if (Current()->type() == Token::TS_COMMA) {
        expr = New<ir::UndefinedView>();
        expr->SetInformationForNode(Current());
        Next();
      } else if (Current()->type() == Token::TS_REST) {
        expr = ParseSpreadElement(yield);
        spread = true;
      } else {
        expr = ParseAssignmentExpression(true, yield);
      }
      array_literal->InsertLast(expr);
      if (Current()->type() == Token::TS_COMMA) {
        if (spread) {
          SYNTAX_ERROR("SyntaxError array spread element must be the end of the array element list",
                              Current());
        }
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
}


template<typename UCharInputIterator>
ir::Node* Parser<UCharInputIterator>::ParseSpreadElement(bool yield) {
  LOG_PHASE(ParseSpreadElement);
  if (Current()->type() == Token::TS_REST) {
    Next();
    return ParseAssignmentExpression(true, yield);
  }
  SYNTAX_ERROR("SyntaxError '...' expected", Current());
}


template<typename UCharInputIterator>
ir::Node* Parser<UCharInputIterator>::ParseArrayComprehension(bool yield) {
  LOG_PHASE(ParseArrayComprehension);
  if (Current()->type() == Token::TS_LEFT_BRACKET) {
    TokenCursor cursor = GetBufferCursorPosition();
    Next();
    ir::Node* expr = ParseComprehension(yield);
    if (Current()->type() == Token::TS_RIGHT_BRACKET) {
      Next();
      auto arr = New<ir::ArrayLiteralView>();
      arr->SetInformationForNode(PeekBuffer(cursor));
      arr->InsertLast(expr);
      return arr;
    }
    SYNTAX_ERROR("SyntaxError ']' expected", Current());
  }
  SYNTAX_ERROR("SyntaxError '[' expected", Current());
}


template<typename UCharInputIterator>
ir::Node* Parser<UCharInputIterator>::ParseComprehension(bool yield) {
  LOG_PHASE(ParseComprehension);
  ir::Node* comp_for = ParseComprehensionFor(yield);
  ir::Node* comp_tail = ParseComprehensionTail(yield);
  auto expr = New<ir::ComprehensionExprView>(comp_for, comp_tail);
  expr->SetInformationForNode(comp_for);
  return expr;
};


template<typename UCharInputIterator>
ir::Node* Parser<UCharInputIterator>::ParseComprehensionTail(bool yield) {
  LOG_PHASE(ParseComprehensionTail);
  if (Current()->type() == Token::TS_FOR) {
    ir::Node* for_expr = ParseComprehensionFor(yield);
    ir::ForStatementView* stmt = for_expr->ToForStatementView();
    stmt->set_body(ParseComprehensionTail(yield));
    return stmt;
  } else if (Current()->type() == Token::TS_IF) {
    ir::Node* if_expr = ParseComprehensionIf(yield);
    ir::IfStatementView* stmt = if_expr->ToIfStatementView();
    stmt->set_then_block(ParseComprehensionTail(yield));
    return stmt;
  }
  return ParseAssignmentExpression(true, yield);
}


template<typename UCharInputIterator>
ir::Node* Parser<UCharInputIterator>::ParseComprehensionFor(bool yield) {
  LOG_PHASE(ParseComprehensionFor);
  if (Current()->type() == Token::TS_FOR) {
    TokenCursor cursor = GetBufferCursorPosition();
    Next();
    if (Current()->type() == Token::TS_LEFT_PAREN) {
      Next();
      ir::Node* for_bindig = ParseForBinding(yield);
      if (Current()->type() == Token::TS_IDENTIFIER &&
          Current()->value() == "of") {
        Next();
        ir::Node* expr = ParseAssignmentExpression(true, yield);
        if (Current()->type() == Token::TS_RIGHT_PAREN) {
          Next();
          auto for_expr = New<ir::ForOfStatementView>();
          for_expr->set_property_name(for_bindig);
          for_expr->set_expr(expr);
          for_expr->SetInformationForNode(PeekBuffer(cursor));
          return for_expr;
        }
        SYNTAX_ERROR("SyntaxError ')' expected", Current());
      }
      SYNTAX_ERROR("SyntaxError 'of' expected", Current());
    }
    SYNTAX_ERROR("SyntaxError '(' expected", Current());
  }
  SYNTAX_ERROR("SyntaxError 'for' expected", Current());
}


template<typename UCharInputIterator>
ir::Node* Parser<UCharInputIterator>::ParseComprehensionIf(bool yield) {
  LOG_PHASE(ParseComprehensionIf);
  if (Current()->type() == Token::TS_IF) {
    TokenCursor cursor = GetBufferCursorPosition();
    Next();
    if (Current()->type() == Token::TS_LEFT_PAREN) {
      Next();
      ir::Node* expr = ParseAssignmentExpression(true, yield);
      if (Current()->type() == Token::TS_RIGHT_PAREN) {
        Next();
        auto if_expr = New<ir::IfStatementView>();
        if_expr->set_expr(expr);
        if_expr->SetInformationForNode(PeekBuffer(cursor));
        return if_expr;
      }
      SYNTAX_ERROR("SyntaxError ')' expected", Current());
    }
    SYNTAX_ERROR("SyntaxError '(' expected", Current());
  }
  SYNTAX_ERROR("SyntaxError 'if' expected", Current());
}


template<typename UCharInputIterator>
ir::Node* Parser<UCharInputIterator>::ParseGeneratorComprehension(bool yield) {
  LOG_PHASE(ParseGeneratorComprehension);
  if (Current()->type() == Token::TS_LEFT_PAREN) {
    Next();
    ir::Node* comp = ParseComprehension(yield);
    if (Current()->type() == Token::TS_RIGHT_PAREN) {
      return comp;
    }
    SYNTAX_ERROR("SyntaxError ')' expected", Current());
  }
  SYNTAX_ERROR("SyntaxError '(' expected", Current());
}


template<typename UCharInputIterator>
ir::Node* Parser<UCharInputIterator>::ParseYieldExpression(bool in) {
  LOG_PHASE(ParseYieldExpression);
  if (Current()->type() == Token::TS_YIELD) {
    Next();

    TokenCursor cursor = GetBufferCursorPosition();
    bool end = Current()->type() == Token::LINE_TERMINATOR;
    
    if (PeekBuffer(cursor)->has_line_break_before_next() ||
        PeekBuffer(cursor)->has_line_terminator_before_next() ||
        end) {

      auto yield_expr = New<ir::YieldView>(false, nullptr);
      yield_expr->SetInformationForNode(Current());
      
      if (end) {
        Next();
      }
      
      return yield_expr;
    }
    
    bool continuation = false;
    Next();
    
    if (Current()->type() == Token::TS_MUL) {
      Next();
      continuation = true;
    }

    ir::Node* expr = ParseAssignmentExpression(in, true);
    auto yield_expr = New<ir::YieldView>(continuation, expr);
    yield_expr->SetInformationForNode(Current());
    return yield_expr;
  }
  SYNTAX_ERROR("SyntaxError 'yield' expected", Current());
}


template<typename UCharInputIterator>
ir::Node* Parser<UCharInputIterator>::ParseForBinding(bool yield) {
  LOG_PHASE(ParseForBinding);
  switch (Current()->type()) {
    case Token::TS_LEFT_BRACE: // FALL THROUGH
    case Token::TS_LEFT_BRACKET:
      return ParseBindingPattern(yield, false);
    default:
      return ParseBindingIdentifier(false, yield);
  }
}


template <typename UCharInputIterator>
ir::Node* Parser<UCharInputIterator>::ParseObjectLiteral(bool yield) {
  LOG_PHASE(ParseObjectLiteral);
  if (Current()->type() == Token::TS_LEFT_BRACE) {
    ir::ObjectLiteralView* object_literal = New<ir::ObjectLiteralView>();
    object_literal->SetInformationForNode(Current());
    Next();
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
ir::Node* Parser<UCharInputIterator>::ParsePropertyDefinition(bool yield) {
  LOG_PHASE(ParsePropertyDefinition);
  ir::Node* value = nullptr;
  ir::Node* key = nullptr;
  bool generator = false;
  bool getter = false;
  bool setter = false;
  TokenCursor cursor = GetBufferCursorPosition();
   
  if (Current()->type() == Token::TS_IDENTIFIER &&
      Current()->value() == "get") {
      getter = true;
      Next();
  } else if (Current()->type() == Token::TS_IDENTIFIER &&
             Current()->value() == "set") {
      setter = true;
      Next();
  }

  try {
    if (Current()->type() == Token::TS_IDENTIFIER) {
      key = ParseIdentifierReference(yield);
    } else {
      key = ParsePropertyName(yield, false);
    }
  } catch (const SyntaxError& e) {
    if (getter || setter) {
      key = New<ir::NameView>(PeekBuffer(cursor)->value());
      key->SetInformationForNode(PeekBuffer(cursor));
    } else {
      throw e;
    }
  }

  if (Current()->type() == Token::TS_MUL) {
    generator = true;
    Next();
  }
  
  if (Current()->type() == Token::TS_LEFT_PAREN) {
    ir::Node* call_sig = ParseCallSignature(yield);
    if (Current()->type() == Token::TS_LEFT_BRACE) {
      Next();
      ir::Node* body = ParseFunctionBody(yield || generator);
      value = New<ir::FunctionView>(getter, setter, generator, nullptr, call_sig, body);
    }
  } else if (generator) {
    SYNTAX_ERROR("SyntaxError '(' expected", Current());
  } else if (Current()->type() == Token::TS_COLON) {
    Next();
    value = ParseAssignmentExpression(true, false);
  }
  ir::ObjectElementView* element = New<ir::ObjectElementView>(key, value);
  element->SetInformationForNode(PeekBuffer(cursor));
  return element;
}


template <typename UCharInputIterator>
ir::Node* Parser<UCharInputIterator>::ParsePropertyName(bool yield, bool generator_parameter) {
  LOG_PHASE(ParsePropertyName);
  if (Current()->type() == Token::TS_LEFT_BRACKET) {
    if (generator_parameter) {
      return ParseComputedPropertyName(yield);
    }
    return ParseComputedPropertyName(false);
  }
  return ParseLiteralPropertyName();
}


template <typename UCharInputIterator>
ir::Node* Parser<UCharInputIterator>::ParseLiteralPropertyName() {
  LOG_PHASE(ParseLiteralPropertyName);
  switch (Current()->type()) {
    case Token::TS_IDENTIFIER:
      return ParseIdentifier();
    case Token::TS_STRING_LITERAL:
      return ParseStringLiteral();
    case Token::TS_NUMERIC_LITERAL:
      return ParseNumericLiteral();
    default:
      SYNTAX_ERROR("SyntaxError identifier or string literal or numeric literal expected", Current());
  }
}


template <typename UCharInputIterator>
ir::Node* Parser<UCharInputIterator>::ParseComputedPropertyName(bool yield) {
  LOG_PHASE(ParseComputedPropertyName);
  if (Current()->type() == Token::TS_LEFT_BRACKET) {
    Next();
    ir::Node* node = ParseAssignmentExpression(true, yield);
    if (Current()->type() == Token::TS_RIGHT_BRACKET) {
      Next();
      return node;
    }
    SYNTAX_ERROR("SyntaxError ']' expected", Current());
  }
  SYNTAX_ERROR("SyntaxError '[' expected", Current());
}


template <typename UCharInputIterator>
ir::Node* Parser<UCharInputIterator>::ParseLiteral() {
  LOG_PHASE(ParseLiteral);
  switch (Current()->type()) {
    case Token::TS_NULL: {
      auto node = New<ir::NullView>();
      node->SetInformationForNode(Current());
      Next();
      return node;
    }
    default: {
      return ParseValueLiteral();
    }
  }
}


template <typename UCharInputIterator>
ir::Node* Parser<UCharInputIterator>::ParseValueLiteral() {
  LOG_PHASE(ParseValueLiteral);
  switch (Current()->type()) {
    case Token::TS_TRUE: // FALL THROUGH
    case Token::TS_FALSE:
      return ParseBooleanLiteral();
    case Token::TS_NUMERIC_LITERAL:
      return ParseNumericLiteral();
    case Token::TS_STRING_LITERAL:
      return ParseStringLiteral();
    case Token::TS_UNDEFINED:
      return ParseUndefinedLiteral();
    case Token::TS_NAN:
      return ParseNaNLiteral();
    default:
      SYNTAX_ERROR("SyntaxError boolean or numeric literal or string literal expected", Current());
  }
}


template <typename UCharInputIterator>
ir::Node* Parser<UCharInputIterator>::ParseArrayInitializer(bool yield) {
  LOG_PHASE(ParseArrayInitializer);
  if (Current()->type() == Token::TS_LEFT_BRACKET) {
    TokenCursor cursor = GetBufferCursorPosition();
    ir::Node* ret = nullptr;
    try {
      ret = ParseArrayLiteral(yield);
    } catch (const SyntaxError& e) {
      
      if (!LanguageModeUtil::IsES6(compiler_option_)) {
        throw e;
      }
      SetBufferCursorPosition(cursor);
      ret = ParseArrayComprehension(yield);
    }
    return ret;
  }
  SYNTAX_ERROR("SyntaxError '[' expected", Current());
}


template <typename UCharInputIterator>
ir::Node* Parser<UCharInputIterator>::ParseIdentifierReference(bool yield) {
  LOG_PHASE(ParseIdentifierReference);
  if (Current()->type() == Token::TS_IDENTIFIER) {
    if (Current()->type() == Token::TS_YIELD) {
      if (!yield) {
        SYNTAX_ERROR("SyntaxError 'yield' not allowed here", Current());
      }
      auto node = New<ir::YieldView>(false, nullptr);
      node->SetInformationForNode(Current());
      return node;
    }
    return ParseIdentifier();
  }
  SYNTAX_ERROR("SyntaxError 'identifier' expected", Current());
}


template <typename UCharInputIterator>
ir::Node* Parser<UCharInputIterator>::ParseBindingIdentifier(bool default_allowed, bool yield) {
  LOG_PHASE(ParseBindingIdentifier);
  ir::Node* ret;
  if (Current()->type() == Token::TS_DEFAULT) {
    if (!default_allowed) {
      SYNTAX_ERROR("SyntaxError 'default' keyword not allowed here", Current());
    }
    ret = New<ir::DefaultView>();
    Next();
  } else if (Current()->type() == Token::TS_YIELD) {
    TokenCursor cursor = GetBufferCursorPosition();
    Next();
    ir::Node* expr = ParseIdentifier();
    ret = New<ir::YieldView>(false, expr);
    Next();
  } else if (Current()->type() == Token::TS_IDENTIFIER) {
    ret = ParseIdentifier();
  } else {
    SYNTAX_ERROR("SyntaxError 'default', 'yield' or 'identifier' expected", Current());
  }

  ret->SetInformationForNode(Current());

  return ret;
}

template <typename UCharInputIterator>
ir::Node* Parser<UCharInputIterator>::ParseLabelIdentifier(bool yield) {
  LOG_PHASE(ParseLabelIdentifier);
  if (Current()->type() == Token::TS_YIELD && yield) {
    SYNTAX_ERROR("SyntaxError yield not allowed here", Current());
  }
  if (Current()->type() == Token::TS_YIELD) {
    auto node = New<ir::YieldView>(false, nullptr);
    node->SetInformationForNode(Current());
    return node;
  }
  return ParseIdentifier();
}



template <typename UCharInputIterator>
ir::Node* Parser<UCharInputIterator>::ParseIdentifier() {
  LOG_PHASE(ParseIdentifier);
  if (Current()->type() == Token::TS_IDENTIFIER) {
    auto node = New<ir::NameView>(Current()->value());
    node->SetInformationForNode(Current());
    Next();
    return node;
  }
  SYNTAX_ERROR("SyntaxError 'identifier' expected", Current());
}


template <typename UCharInputIterator>
ir::Node* Parser<UCharInputIterator>::ParseStringLiteral() {
  LOG_PHASE(ParseStringLiteral);
  if (Current()->type() == Token::TS_STRING_LITERAL) {
    auto string_literal = New<ir::StringView>(Current()->value());
    string_literal->SetInformationForNode(Current());
    Next();
    return string_literal;
  }
  SYNTAX_ERROR("SyntaxError string literal expected", Current());
}


template <typename UCharInputIterator>
ir::Node* Parser<UCharInputIterator>::ParseNumericLiteral() {
  LOG_PHASE(ParseNumericLiteral);
  if (Current()->type() == Token::TS_NUMERIC_LITERAL) {
    auto number = New<ir::NumberView>(Current()->value());
    number->SetInformationForNode(Current());
    Next();
    return number;
  }
  SYNTAX_ERROR("SyntaxError numeric literal expected", Current());
}


template <typename UCharInputIterator>
ir::Node* Parser<UCharInputIterator>::ParseBooleanLiteral() {
  LOG_PHASE(ParseBooleanLiteral);
  ir::Node* ret = nullptr;
  if (Current()->type() == Token::TS_TRUE) {
    ret = New<ir::TrueView>();
  } else if (Current()->type() == Token::TS_FALSE) {
    ret = New<ir::FalseView>();
  }

  if (ret != nullptr) {
    ret->SetInformationForNode(Current());
    Next();
    return ret;
  }
  SYNTAX_ERROR("SyntaxError boolean literal expected", Current());
}


template <typename UCharInputIterator>
ir::Node* Parser<UCharInputIterator>::ParseUndefinedLiteral() {
  LOG_PHASE(ParseUndefinedLiteral);
  if (Current()->type() == Token::TS_UNDEFINED) {
    auto node = New<ir::UndefinedView>();
    node->SetInformationForNode(Current());
    Next();
    return node;
  }
  SYNTAX_ERROR("SyntaxError 'undefined' expected", Current());
}


template <typename UCharInputIterator>
ir::Node* Parser<UCharInputIterator>::ParseNaNLiteral() {
  LOG_PHASE(ParseNaNLiteral);
  if (Current()->type() == Token::TS_NAN) {
    auto node = New<ir::NaNView>();
    node->SetInformationForNode(Current());
    Next();
    return node;
  }
  SYNTAX_ERROR("SyntaxError 'NaN' expected", Current());
}


template <typename UCharInputIterator>
ir::Node* Parser<UCharInputIterator>::ParseRegularExpression() {
  LOG_PHASE(ParseRegularExpression);
  if (Current()->type() == Token::TS_REGULAR_EXPR) {
    auto reg_expr = New<ir::RegularExprView>(Current()->value());
    reg_expr->SetInformationForNode(Current());
    Next();
    return reg_expr;
  }
  SYNTAX_ERROR("SyntaxError regular expression expected", Current());
}


template <typename UCharInputIterator>
ir::Node* Parser<UCharInputIterator>::ParseTemplateLiteral() {
  LOG_PHASE(ParseTemplateLiteral);
  if (Current()->type() == Token::TS_TEMPLATE_LITERAL) {
    auto template_literal = New<ir::TemplateLiteralView>(Current()->value());
    Next();
    template_literal->SetInformationForNode(Current());
  }
  SYNTAX_ERROR("SyntaxError template literal expected", Current());
}
}
