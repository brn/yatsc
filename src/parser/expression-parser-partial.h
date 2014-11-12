//  
// The MIT License (MIT)
//  
// Copyright (c) 2013 Taketoshi Aono(brn)
//  
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//  
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//  
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.


namespace yatsc {

// Expression[In, Yield]
//   AssignmentExpression[?In, ?Yield]
//   Expression[?In, ?Yield] , AssignmentExpression[?In, ?Yield]
//
template <typename UCharInputIterator>
ParseResult Parser<UCharInputIterator>::ParseExpression(bool in, bool yield) {
  LOG_PHASE(ParseExpression);
  auto assignment_expr_result = ParseAssignmentExpression(in, yield);
  CHECK_AST(assignment_expr_result);
  
  auto assignment_expr = assignment_expr_result.value();

  // Parse comma expressions.
  if (Current()->type() == Token::TS_COMMA) {
    Next();
    Handle<ir::CommaExprView> comma_expr = New<ir::CommaExprView>({assignment_expr});
    comma_expr->SetInformationForNode(*Current());
    
    while (1) {
      assignment_expr_result = ParseAssignmentExpression(in, yield);
      CHECK_AST(assignment_expr_result);
      
      assignment_expr = assignment_expr_result.value();
      comma_expr->InsertLast(assignment_expr);
      if (Current()->type() == Token::TS_COMMA) {
        Next();
      } else {
        return Success(comma_expr);
      }
    }
  }
  return assignment_expr_result;
}


// AssignmentPattern[Yield]
//   ObjectAssignmentPattern[?Yield]
//   ArrayAssignmentPattern[?Yield]
//
template <typename UCharInputIterator>
ParseResult Parser<UCharInputIterator>::ParseAssignmentPattern(bool yield) {
  LOG_PHASE(ParseAssignmentPattern);
  TokenInfo info = *Current();
  ParseResult result;
  switch(Current()->type()) {
    case Token::TS_LEFT_BRACE: {
      result = ParseObjectAssignmentPattern(yield);
      break;
    }
    case Token::TS_LEFT_BRACKET: {
      result = ParseArrayAssignmentPattern(yield);
      break;
    }
    default:
      SYNTAX_ERROR("unexpected token", (&info));
  }

  CHECK_AST(result);

  result.value()->SetInformationForNode(&info);
  return result;
}


// ObjectAssignmentPattern[Yield]
//   { }
//   { AssignmentPropertyList[?Yield] }
//   { AssignmentPropertyList[?Yield] , }
//
template <typename UCharInputIterator>
ParseResult Parser<UCharInputIterator>::ParseObjectAssignmentPattern(bool yield) {
  LOG_PHASE(ParseObjectAssignmentPattern);

  Next();
  auto result = ParseAssignmentPropertyList(yield);
  CHECK_AST(result);
    
  if (Current()->type() == Token::TS_COMMA) {
    Next();
  }
  if (Current()->type() == Token::TS_RIGHT_BRACE) {
    Next();
    return result;
  }
  SYNTAX_ERROR("'}' expected.", Current());
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
ParseResult Parser<UCharInputIterator>::ParseArrayAssignmentPattern(bool yield) {
  LOG_PHASE(ParseArrayAssignmentPattern);

  bool has_rest = false;
  bool success = true;
  auto array_view = New<ir::BindingArrayView>();
  array_view->SetInformationForNode(Current());
  Next();


  // '[, ...' pattern
  if (Current()->type() == Token::TS_COMMA) {
    Next();
    array_view->InsertLast(New<ir::UndefinedView>());
    array_view->InsertLast(New<ir::UndefinedView>());
  }

  // Parse spread pattern.
  // '[...x]'
  if (Current()->type() == Token::TS_REST) {
    auto result = ParseAssignmentRestElement(yield);
    SKIP_TOKEN_IF_AND(result, success, Token::TS_RIGHT_BRACKET, return Failed());
    array_view->InsertLast(result.value());
    has_rest = true;
  }

  // Parse closed array initializer.
  if (Current()->type() == Token::TS_RIGHT_BRACKET) {
    // If array pattern element count is zero, it's treated as SyntaxError.
    if (array_view->size() == 0) {
      SYNTAX_ERROR("destructuring assignment left hand side is not allowed empty array", Current());
    }
    Next();
    return Success(array_view);
  }

  // The ParameterRest is not allowed in any position of array pattern except the last element.
  if (has_rest) {
    SYNTAX_ERROR("destructuring assignment rest must be the end of element", Current());
  }
    
  while (1) {
    if (Current()->type() == Token::TS_COMMA) {
      array_view->InsertLast(New<ir::UndefinedView>());
      array_view->InsertLast(New<ir::UndefinedView>());
      Next();
    } else {
      auto parser_assignment_result = ParseAssignmentElement(yield);
      SKIP_TOKEN_OR(parser_assignment_result, success, Token::TS_RIGHT_BRACKET) {
        array_view->InsertLast(parser_assignment_result.value());
      }
    }
    switch (Current()->type()) {
      case Token::TS_COMMA:
        Next();
        break;
      case Token::TS_RIGHT_BRACKET:
        Next();
        return Success(array_view);
      default:
        SYNTAX_ERROR("unexpected token", Current());
    }
  }
}


// Parse destructuring assignment object pattern properties.
// 
// AssignmentPropertyList[Yield]
//   AssignmentProperty[?Yield]
//   AssignmentPropertyList[?Yield] , AssignmentProperty[?Yield]
//
template <typename UCharInputIterator>
ParseResult Parser<UCharInputIterator>::ParseAssignmentPropertyList(bool yield) {
  LOG_PHASE(ParseAssignmentPropertyList);
  auto prop_list = New<ir::BindingPropListView>();

  bool success = true;
  
  while (1) {
    auto result = ParseAssignmentProperty(yield);
    
    SKIP_TOKEN_OR(result, success, Token::TS_RIGHT_BRACE) {
      prop_list->InsertLast(result.value());
    }
    
    if (Current()->type() == Token::TS_COMMA) {
      Next();
      continue;
    } else if (Current()->type() == Token::TS_RIGHT_BRACE) {
      return Success(prop_list);
    } else {
      SYNTAX_ERROR("',' or '}' expected.", Current());
    }
  }
}


// Parse destructuring assignment object pattern property.
//
// AssignmentProperty[Yield] :
//   IdentifierReference[?Yield] Initializer[In,?Yield](opt)
//   PropertyName : AssignmentElement[?Yield]
//
template <typename UCharInputIterator>
ParseResult Parser<UCharInputIterator>::ParseAssignmentProperty(bool yield) {
  LOG_PHASE(ParseAssignmentProperty);
  TokenInfo info = *Current();
  
  ParseResult property_name;
  ParseResult elem;

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
  CHECK_AST(property_name);
  
  ParseResult init;

  // Initializer is only allowed, if property name is identifier reference.
  if (identifier && Current()->type() == Token::TS_ASSIGN) {
    Next();
    init = ParseAssignmentExpression(true, yield);
    CHECK_AST(init);
  } else if (Current()->type() == Token::TS_COLON) {
    Next();
    elem = ParseAssignmentElement(yield);
    CHECK_AST(elem);
  }

  // All destructuring assignment element is convert to BindingElementView.
  auto binding_element = New<ir::BindingElementView>(property_name.value(), elem.value(), init.value());
  binding_element->SetInformationForNode(&info);
  return Success(binding_element);
}


// AssignmentElement[Yield]
//   DestructuringAssignmentTarget[?Yield] Initializer[In,?Yield](opt)
//
template <typename UCharInputIterator>
ParseResult Parser<UCharInputIterator>::ParseAssignmentElement(bool yield) {
  LOG_PHASE(ParseAssignmentElement);
  auto target = ParseDestructuringAssignmentTarget(yield);
  ParseResult init;
  
  CHECK_AST(target);
  
  if (Current()->type() == Token::TS_ASSIGN) {
    Next();
    init = ParseAssignmentExpression(true, yield);
    CHECK_AST(init);
  }

  // All destructuring assignment element is convert to BindingElementView.
  auto binding_element = New<ir::BindingElementView>(ir::Node::Null(), target.value(), init.value());
  binding_element->SetInformationForNode(target.value());
  return Success(binding_element);
}


// AssignmentRestElement[Yield]
//   ... DestructuringAssignmentTarget[?Yield]
//
template <typename UCharInputIterator>
ParseResult Parser<UCharInputIterator>::ParseAssignmentRestElement(bool yield) {
  LOG_PHASE(ParseAssignmentRestElement);

  TokenInfo info = *Current();
  Next();
  auto target = ParseDestructuringAssignmentTarget(yield);
  CHECK_AST(target);
  auto rest = New<ir::RestParamView>(target.value());
  rest->SetInformationForNode(&info);
  return Success(rest);
}


// DestructuringAssignmentTarget[Yield]
//   LeftHandSideExpression[?Yield]
//
template <typename UCharInputIterator>
ParseResult Parser<UCharInputIterator>::ParseDestructuringAssignmentTarget(bool yield) {
  LOG_PHASE(ParseDestructuringAssignmentTarget);
  RecordedParserState rps = parser_state();
  auto lhs_result = ParseLeftHandSideExpression(yield);
  CHECK_AST(lhs_result);
  
  auto node = lhs_result.value();
  
  // Check whether DestructuringAssignmentTarget is IsValidAssignmentTarget or not.
  if (!node->IsValidLhs()) {
    if (node->HasObjectLiteralView() || node->HasArrayLiteralView()) {
      RestoreParserState(rps);
      lhs_result = ParseAssignmentPattern(yield);
      CHECK_AST(lhs_result);
    } else {
      SYNTAX_ERROR("invalid Left-Hand-Side expression", lhs_result.value());
    }
  }
  return lhs_result;
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
ParseResult Parser<UCharInputIterator>::ParseAssignmentExpression(bool in, bool yield) {
  LOG_PHASE(ParseAssignmentExpression);
  ParseResult node;

  // Record current buffer position.
  RecordedParserState rps = parser_state();
  
  if (Current()->type() == Token::TS_LEFT_PAREN ||
      Current()->type() == Token::TS_LESS) {
    // First try parse as arrow function.
    // parsae an arrow_function_parameters.
    auto arrow_param_result = ParseArrowFunctionParameters(yield, ir::Node::Null());
    // if node is exists, arrow function parameter parse is succeeded.
    if (arrow_param_result) {
      return ParseConciseBody(in, arrow_param_result.value());
    }

    RestoreParserState(rps);
  }

  ParseResult expr_result;
  bool parsed_as_assignment_pattern = false;

  if (Current()->type() == Token::TS_YIELD) {
    if (!yield) {
      SYNTAX_ERROR("invalid use of 'yield' keyword", Current());
    }
    expr_result = ParseYieldExpression(in);
    CHECK_AST(expr_result);
  } else {  
    expr_result = ParseConditionalExpression(in, yield);
    if (expr_result) {
      auto expr = expr_result.value();
      if (expr->HasNameView() && Current()->type() == Token::TS_ARROW_GLYPH) {
        return ParseArrowFunction(in, yield, expr);
      }
    } else {
      if (!LanguageModeUtil::IsES6(compiler_option_)) {
        return Failed();
      }
      RestoreParserState(rps);
      expr_result = ParseAssignmentPattern(yield);
      parsed_as_assignment_pattern = true;
    }
  }
  
  // Expression is not an arrow_function.
  const TokenInfo *token_info = Current();
  Token type = token_info->type();

  // Check assignment operators.
  if (IsAssignmentOp(type)) {
    if (!parsed_as_assignment_pattern &&
        (expr_result.value()->HasObjectLiteralView() ||
         expr_result.value()->HasArrayLiteralView())) {
      
      RestoreParserState(rps);

      if (!LanguageModeUtil::IsES6(compiler_option_)) {
        SYNTAX_ERROR("Invalid Left-Hand-Side expression", Current());
      }
      
      expr_result = ParseAssignmentPattern(yield);
      CHECK_AST(expr_result);
    }
    Next();

    // Check left hand side expression is valid as an expression.
    // If left hand side expression is like 'func()',
    // that is invalid expression.
    if (expr_result.value()->IsValidLhs()) {
      auto rhs_result = ParseAssignmentExpression(in, yield);
      CHECK_AST(rhs_result);
      auto result = New<ir::AssignmentView>(type, expr_result.value(), rhs_result.value());
      result->SetInformationForNode(expr_result.value());
      return Success(result);
    }
    SYNTAX_ERROR("invalid left hand side expression in 'assignment expression'", Current());
  } else if (parsed_as_assignment_pattern) {
    SYNTAX_ERROR("destructuring assignment must be initialized", Current());
  }
  return expr_result;
}


template <typename UCharInputIterator>
ParseResult Parser<UCharInputIterator>::ParseArrowFunction(bool in, bool yield, Handle<ir::Node> identifier) {
  LOG_PHASE(ParseArrowFunction);
  auto call_sig = ParseArrowFunctionParameters(yield, identifier);
  CHECK_AST(call_sig);
  return ParseConciseBody(in, call_sig.value());
}


template <typename UCharInputIterator>
ParseResult Parser<UCharInputIterator>::ParseArrowFunctionParameters(bool yield, Handle<ir::Node> identifier) {
  LOG_PHASE(ParseArrowFunctionParameters);

  ParseResult call_sig_result;
  
  if (identifier) {
    call_sig_result = Success(New<ir::CallSignatureView>(identifier, ir::Node::Null(), ir::Node::Null()));
    call_sig_result.value()->SetInformationForNode(identifier);
  } else {
    call_sig_result = ParseCallSignature(false, false);
    CHECK_AST(call_sig_result);
  }
  if (Current()->type() != Token::TS_ARROW_GLYPH) {
    SYNTAX_ERROR("'=>' expected", Current());
  }
  Next();
  return call_sig_result;
}


template <typename UCharInputIterator>
ParseResult Parser<UCharInputIterator>::ParseConciseBody(bool in, Handle<ir::Node> call_sig) {
  LOG_PHASE(ParseConciseBody);
  ParseResult concise_body_result;
  if (Current()->type() == Token::TS_LEFT_BRACE) {
    concise_body_result = ParseFunctionBody(false);
  } else {
    concise_body_result = ParseAssignmentExpression(true, false);
  }
  CHECK_AST(concise_body_result);
  auto ret = New<ir::ArrowFunctionView>(call_sig, concise_body_result.value());
  ret->SetInformationForNode(call_sig);
  return Success(ret);
}


// ConditionalExpression[In, Yield]
//   LogicalORExpression[?In, ?Yield]
//   LogicalORExpression[?In,?Yield] ? AssignmentExpression[In, ?Yield] : AssignmentExpression[?In, ?Yield]
//
template <typename UCharInputIterator>
ParseResult Parser<UCharInputIterator>::ParseConditionalExpression(bool in, bool yield) {
  LOG_PHASE(ParseConditionalExpression);
  
  auto logical_or_expr_result = ParseLogicalORExpression(in, yield);
  CHECK_AST(logical_or_expr_result);
  
  if (Current()->type() == Token::TS_QUESTION_MARK) {
    Next();
    auto left_result = ParseAssignmentExpression(in, yield);
    CHECK_AST(left_result);
    
    if (Current()->type() == Token::TS_COLON) {
      Next();
      auto right_result = ParseAssignmentExpression(in, yield);
      CHECK_AST(right_result);      
      auto temary = New<ir::TemaryExprView>(logical_or_expr_result.value(), left_result.value(), right_result.value());
      temary->SetInformationForNode(logical_or_expr_result.value());
      temary->MarkAsInValidLhs();
      return Success(temary);
    }
    SYNTAX_ERROR("unexpected token in 'temary expression'", Current());
  }
  return logical_or_expr_result;
}


#define PARSE_BINARY_EXPRESSION_INTERNAL(check, name, next)             \
  template <typename UCharInputIterator>                                \
  ParseResult Parser<UCharInputIterator>::name(bool in, bool yield) { \
    LOG_PHASE(name);                                                    \
    auto ret = next;                                                    \
    CHECK_AST(ret);                                                     \
    while (1) {                                                         \
      if (check) {                                                      \
        auto tmp = ret;                                                 \
        Token type = Current()->type();                                 \
        Next();                                                         \
        auto n = next;                                                  \
        CHECK_AST(n);                                                   \
        ret = Success(New<ir::BinaryExprView>(type, ret.value(), n.value())); \
        ret.value()->SetInformationForNode(tmp.value());                  \
      } else {                                                          \
        break;                                                          \
      }                                                                 \
    }                                                                   \
    return ret;                                                         \
  }


#define PARSE_BINARY_EXPRESSION(name, next, token)                      \
  PARSE_BINARY_EXPRESSION_INTERNAL((Current()->type() == Token::token), name, (next(in, yield)))

#define PARSE_BINARY_EXPRESSION_WITH_COND(cond, name, next)         \
  PARSE_BINARY_EXPRESSION_INTERNAL((cond), name, (next(in, yield)))

#define PARSE_BINARY_EXPRESSION_WITH_CALL(cond, name, call) \
  PARSE_BINARY_EXPRESSION_INTERNAL((cond), name, (call))


// LogicalORExpression[In, Yield]
//   LogicalANDExpression[?In, ?Yield]
//   LogicalORExpression[?In, ?Yield] || LogicalANDExpression[?In, ?Yield]
//
PARSE_BINARY_EXPRESSION(ParseLogicalORExpression, ParseLogicalANDExpression, TS_LOGICAL_OR)

// LogicalANDExpression[In, Yield]
// BitwiseORExpression[?In, ?Yield]
// LogicalANDExpression[?In, ?Yield] && BitwiseORExpression[?In, ?Yield]
//
PARSE_BINARY_EXPRESSION(ParseLogicalANDExpression, ParseBitwiseORExpression, TS_LOGICAL_AND)

// BitwiseORExpression[In, Yield]
//   BitwiseXORExpression[?In, ?Yield]
//   BitwiseORExpression[?In, ?Yield] | BitwiseXORExpression[?In, ?Yield]
//
PARSE_BINARY_EXPRESSION(ParseBitwiseORExpression, ParseBitwiseXORExpression, TS_BIT_OR)

// BitwiseXORExpression[In, Yield]
//   BitwiseANDExpression[?In, ?Yield]
//   BitwiseXORExpression[?In, ?Yield] ^ BitwiseANDExpression[?In, ?Yield]
//
PARSE_BINARY_EXPRESSION(ParseBitwiseXORExpression, ParseBitwiseANDExpression, TS_BIT_XOR)


// BitwiseANDExpression[In, Yield]
//   EqualityExpression[?In, ?Yield]
//   BitwiseANDExpression[?In, ?Yield] & EqualityExpression[?In, ?Yield]
//
PARSE_BINARY_EXPRESSION(ParseBitwiseANDExpression, ParseEqualityExpression, TS_BIT_AND)


// EqualityExpression[In, Yield]
//   RelationalExpression[?In, ?Yield]
//   EqualityExpression[?In, ?Yield] == RelationalExpression[?In, ?Yield]
//   EqualityExpression[?In, ?Yield] != RelationalExpression[?In, ?Yield]
//   EqualityExpression[?In, ?Yield] ===RelationalExpression[?In, ?Yield]
//   EqualityExpression[?In, ?Yield] !==RelationalExpression[?In, ?Yield]
//
PARSE_BINARY_EXPRESSION_WITH_COND((Current()->type() == Token::TS_EQ || Current()->type() == Token::TS_NOT_EQ || Current()->type() == Token::TS_EQUAL || Current()->type() == Token::TS_NOT_EQUAL), ParseEqualityExpression, ParseRelationalExpression);

#define RELATIONAL_COND                             \
  Current()->type() == Token::TS_LESS ||            \
    Current()->type() == Token::TS_GREATER ||       \
    Current()->type() == Token::TS_LESS_EQUAL ||    \
    Current()->type() == Token::TS_GREATER_EQUAL || \
    Current()->type() == Token::TS_INSTANCEOF ||    \
    (in && Current()->type() == Token::TS_IN)

// RelationalExpression[In, Yield]
//   ShiftExpression[?Yield]
//   RelationalExpression[?In, ?Yield] < ShiftExpression[?Yield]
//   RelationalExpression[?In, ?Yield] > ShiftExpression[?Yield]
//   RelationalExpression[?In, ?Yield] <= ShiftExpression[?Yield]
//   RelationalExpression[?In, ?Yield] >= ShiftExpression[?Yield]
//   RelationalExpression[?In, ?Yield] instanceof ShiftExpression[?Yield]
//   [+In] RelationalExpression[In, ?Yield] in ShiftExpression[?Yield]
//
PARSE_BINARY_EXPRESSION_WITH_COND((RELATIONAL_COND), ParseRelationalExpression, ParseShiftExpression)
#undef RELATIONAL_COND


#define SHIFT_COND                                \
  Current()->type() == Token::TS_SHIFT_LEFT ||    \
    Current()->type() == Token::TS_SHIFT_RIGHT || \
    Current()->type() == Token::TS_U_SHIFT_RIGHT

// ShiftExpression[Yield]
//   AdditiveExpression[?Yield]
//   ShiftExpression[?Yield] << AdditiveExpression[?Yield]
//   ShiftExpression[?Yield] >> AdditiveExpression[?Yield]
//   ShiftExpression[?Yield] >>> AdditiveExpression[?Yield]
//
PARSE_BINARY_EXPRESSION_WITH_COND((SHIFT_COND), ParseShiftExpression, ParseAdditiveExpression)
#undef SHIFT_COND


#define ADDITIVE_COND                           \
  Current()->type() == Token::TS_PLUS ||        \
    Current()->type() == Token::TS_MINUS

// AdditiveExpression[Yield]
//   MultiplicativeExpression[?Yield]
//   AdditiveExpression[?Yield] + MultiplicativeExpression[?Yield]
//   AdditiveExpression[?Yield] - MultiplicativeExpression[?Yield]
//
PARSE_BINARY_EXPRESSION_WITH_COND(ADDITIVE_COND, ParseAdditiveExpression, ParseMultiplicativeExpression)
#undef ADDITIVE_COND


#define MULTIPLICATIVE_COND                     \
  Current()->type() == Token::TS_MUL ||         \
    Current()->type() == Token::TS_MOD ||       \
    Current()->type() == Token::TS_DIV

// AdditiveExpression[Yield]
//   MultiplicativeExpression[?Yield]
//   AdditiveExpression[?Yield] + MultiplicativeExpression[?Yield]
//   AdditiveExpression[?Yield] - MultiplicativeExpression[?Yield]
//
PARSE_BINARY_EXPRESSION_WITH_CALL(MULTIPLICATIVE_COND, ParseMultiplicativeExpression, ParseUnaryExpression(yield))
#undef MULTIPLICATIVE_COND

#undef PARSE_BINARY_EXPRESSION_INTERNAL
#undef PARSE_BINARY_EXPRESSION
#undef PARSE_BINARY_EXPRESSION_WITH_COND
#undef PARSE_BINARY_EXPRESSION_WITH_CALL


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
ParseResult Parser<UCharInputIterator>::ParseUnaryExpression(bool yield) {
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
      auto unary_expr_result = ParseUnaryExpression(yield);
      CHECK_AST(unary_expr_result);
      auto ret = New<ir::UnaryExprView>(type, unary_expr_result.value());
      ret->SetInformationForNode(unary_expr_result.value());
      return Success(ret);
    }
    case Token::TS_LESS: {
      auto type_arguments_result = ParseTypeArguments();
      auto unary_expr_result = ParseUnaryExpression(yield);
      
      CHECK_AST(unary_expr_result);
      CHECK_AST(type_arguments_result);
      
      Handle<ir::Node> ret = New<ir::CastView>(type_arguments_result.value(), unary_expr_result.value());
      ret->SetInformationForNode(type_arguments_result.value());
      return Success(ret);
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
ParseResult Parser<UCharInputIterator>::ParsePostfixExpression(bool yield) {
  LOG_PHASE(ParsePostfixExpression);
  auto lhs_expr_result = ParseLeftHandSideExpression(yield);
  CHECK_AST(lhs_expr_result);
  if (Current()->type() == Token::TS_INCREMENT ||
      Current()->type() == Token::TS_DECREMENT) {
    auto ret = New<ir::PostfixView>(lhs_expr_result.value(), Current()->type());
    ret->SetInformationForNode(lhs_expr_result.value());
    Next();
    return Success(ret);
  }
  return lhs_expr_result;
}


// Parse left-hand-side-expression
// left_hand_side_expression
// 	: new_expression(omited) -> member_expression
// 	| call_expression
// 	;
template <typename UCharInputIterator>
ParseResult Parser<UCharInputIterator>::ParseLeftHandSideExpression(bool yield) {
  LOG_PHASE(ParseLeftHandSideExpression);
  RecordedParserState rps = parser_state();
  if (Current()->type() == Token::TS_NEW) {
    Next();
    if (Current()->type() == Token::TS_NEW) {
      RestoreParserState(rps);
      return ParseNewExpression(yield);
    }
    RestoreParserState(rps);
    return ParseCallExpression(yield);
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
ParseResult Parser<UCharInputIterator>::ParseCallExpression(bool yield) {
  LOG_PHASE(ParseCallExpression);
  ParseResult target;
  if (Current()->type() == Token::TS_SUPER) {
    target = Success(New<ir::SuperView>());
    target.value()->SetInformationForNode(Current());
    Next();
    if (Current()->type() == Token::TS_DOT) {
      Next();
      auto identifier_result = ParseIdentifier();
      CHECK_AST(identifier_result);
      target = Success(New<ir::GetPropView>(target.value(), identifier_result.value()));
      target.value()->SetInformationForNode(identifier_result.value());
    }
  } else {
    target = ParseMemberExpression(yield);
    CHECK_AST(target);
  }
  
  
  if (Current()->type() == Token::TS_LEFT_PAREN ||
      Current()->type() == Token::TS_LESS) {
    auto arguments_result = ParseArguments(yield);
    if (!arguments_result) {return target;}
    target.value()->MarkAsInValidLhs();
    auto call = New<ir::CallView>(target.value(), arguments_result.value());
    call->SetInformationForNode(target.value());
    
    while (1) {
      switch (Current()->type()) {
        case Token::TS_LESS:
        case Token::TS_LEFT_PAREN: {
          arguments_result = ParseArguments(yield);
          if (!arguments_result) {
            return Success(call);
          }
          call = New<ir::CallView>(call, arguments_result.value());
          call->MarkAsInValidLhs();
          call->SetInformationForNode(arguments_result.value());
          break;
        }
        case Token::TS_LEFT_BRACKET:
        case Token::TS_DOT: {
          auto tmp = ParseGetPropOrElem(call, yield, false, false);
          CHECK_AST(tmp);
          call = tmp.value();
          break;
        }
        default:
          return Success(call);
      }
    }
  } else if (Current()->type() == Token::TS_TEMPLATE_LITERAL) {
    auto template_literal_result = ParseTemplateLiteral();
    CHECK_AST(template_literal_result);
    Handle<ir::Node> call = New<ir::CallView>(target.value(), template_literal_result.value());
    call->SetInformationForNode(target.value());
    call->MarkAsInValidLhs();
    return Success(call);
  }
  return target;
}


template <typename UCharInputIterator>
ParseResult Parser<UCharInputIterator>::BuildArguments(ParseResult type_arguments_result,
                                                       Handle<ir::Node> args,
                                                       bool) {
  auto arguments = New<ir::ArgumentsView>(type_arguments_result.value(), args);
  if (type_arguments_result) {
    arguments->SetInformationForNode(type_arguments_result.value());
  } else {
    arguments->SetInformationForNode(args);
  }
  return Success(arguments);
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
ParseResult Parser<UCharInputIterator>::ParseArguments(bool yield) {
  LOG_PHASE(ParseArguments);

  ParseResult type_arguments_result;
  RecordedParserState rps = parser_state();
  if (Current()->type() == Token::TS_LESS) {
    type_arguments_result = ParseTypeArguments();
    if (!type_arguments_result) {
      RestoreParserState(rps);
      return Failed();
    }
  }
  
  if (Current()->type() == Token::TS_LEFT_PAREN) {
    Handle<ir::CallArgsView> args = New<ir::CallArgsView>();
    args->SetInformationForNode(Current());
    Next();
    if (Current()->type() == Token::TS_RIGHT_PAREN) {
      Next();
      return BuildArguments(type_arguments_result, args, true);
    }
    
    bool has_rest = false;
    bool success = true;
    
    while (1) {

      if (Current()->type() == Token::TS_REST) {
        TokenInfo info = *Current();
        Next();
        auto assignment_expr_result = ParseAssignmentExpression(true, yield);
        
        SKIP_TOKEN_OR(assignment_expr_result, success, Token::TS_RIGHT_PAREN) {
          auto rest = New<ir::RestParamView>(assignment_expr_result.value());
          rest->SetInformationForNode(&info);
          args->InsertLast(rest);
          has_rest = true;
        }
      } else {
        auto assignment_expr_result = ParseAssignmentExpression(true, yield);
        
        SKIP_TOKEN_OR(assignment_expr_result, success, Token::TS_RIGHT_PAREN) {
          args->InsertLast(assignment_expr_result.value());
        }
      }
      
      if (Current()->type() == Token::TS_COMMA) {
        if (has_rest) {
          SYNTAX_ERROR("the spread argument must be the end of arguments", Current());
        }
        Next();
        continue;
      } else if (Current()->type() == Token::TS_RIGHT_PAREN) {
        Next();
        return BuildArguments(type_arguments_result, args, success);
      }
      SYNTAX_ERROR("unexpected token in 'arguments'", Current());
    }
  }
  SYNTAX_ERROR("'(' expected.", Current());
}


template <typename UCharInputIterator>
ParseResult Parser<UCharInputIterator>::ParseNewExpression(bool yield) {
  LOG_PHASE(ParseNewExpression);
  RecordedParserState rps = parser_state();
  Next();
  if (Current()->type() == Token::TS_NEW) {
    auto new_expr_result = ParseNewExpression(yield);
    CHECK_AST(new_expr_result);
    auto ret = New<ir::NewCallView>(new_expr_result.value(), ir::Node::Null());
    ret->SetInformationForNode(&(rps.current()));
    return Success(ret);
  }
  RestoreParserState(rps);
  return ParseMemberExpression(yield);
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
ParseResult Parser<UCharInputIterator>::ParseMemberExpression(bool yield) {
  LOG_PHASE(ParseMemberExpression);
  // Not advance scanner.
  TokenInfo* token_info = Current();
  if (token_info->type() ==  Token::TS_NEW) {
    TokenInfo info = *Current();
    // Parse new Foo() expression.
    Next();
    ParseResult member;
    if (Current()->type() == Token::TS_SUPER) {
      member = Success(New<ir::SuperView>());
      member.value()->SetInformationForNode(&info);
    } else {
      member = ParseMemberExpression(yield);
      CHECK_AST(member);
    }

    
    // New expression can omit parens.
    // If paren exists, continue parsing.
    if (Current()->type() == Token::TS_LEFT_PAREN ||
        Current()->type() == Token::TS_LESS) {
      auto arguments_result = ParseArguments(yield);
      if (!arguments_result) {
        return member;
      }
      auto ret = New<ir::NewCallView>(member.value(), arguments_result.value());
      ret->SetInformationForNode(member.value());
      ret->MarkAsInValidLhs();
      return ParseGetPropOrElem(ret, yield, false, false);
    } else {
      // Parens are not exists.
      // Immediate return.
      auto ret = New<ir::NewCallView>(member.value(), ir::Node::Null());
      ret->SetInformationForNode(member.value());
      return Success(ret);
    }
  } else if (token_info->type() == Token::TS_SUPER) {
    auto super = New<ir::SuperView>();
    super->SetInformationForNode(token_info);
    return ParseGetPropOrElem(super, yield, false, false);
  } else {
    auto primary_expr_result = ParsePrimaryExpression(yield);
    CHECK_AST(primary_expr_result);
    return ParseGetPropOrElem(primary_expr_result.value(), yield, false, false);
  }
}


// Parse member expression suffix.
// Like 'new foo.bar.baz()', 'new foo["bar"]', '(function(){return {a:1}}).a'
template <typename UCharInputIterator>
ParseResult Parser<UCharInputIterator>::ParseGetPropOrElem(Handle<ir::Node> node, bool yield, bool dot_only, bool is_throw) {
  LOG_PHASE(ParseGetPropOrElem);
  while (1) {
    switch (Current()->type()) {
      case Token::TS_LEFT_BRACKET: {
        if (dot_only) {
          if (is_throw) {
            SYNTAX_ERROR("'.' expected.", Current());
          }
          return Success(node);
        }
        // [...] expression.
        Next();
        auto expr_result = ParseExpression(true, false);
        CHECK_AST(expr_result);
        node = New<ir::GetElemView>(node, expr_result.value());
        node->SetInformationForNode(node);
        if (Current()->type() != Token::TS_RIGHT_BRACKET) {
          SYNTAX_ERROR("unexpected token", Current());
        }
        Next();
        break;
      }
      case Token::TS_DOT: {
        // a.b.c expression.
        Next();
        if (Current()->type() != Token::TS_IDENTIFIER &&
            !TokenInfo::IsKeyword(Current()->type())) {
          SYNTAX_ERROR("'identifier' expected.", Current());
        }
        if (TokenInfo::IsKeyword(Current()->type())) {
          Current()->set_type(Token::TS_IDENTIFIER);
        }
        auto primary_expr_result = ParsePrimaryExpression(yield);
        CHECK_AST(primary_expr_result);
        node = New<ir::GetPropView>(node, primary_expr_result.value());
        node->SetInformationForNode(node);
        break;
      }
      default:
        return Success(node);
    }
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
ParseResult Parser<UCharInputIterator>::ParsePrimaryExpression(bool yield) {
  LOG_PHASE(ParsePrimaryExpression);

  TokenInfo* token_info;
  
  // Allow regular expression in this context.
  TokenInfo* maybe_regexp = scanner_->CheckRegularExpression(Current());
  if (maybe_regexp != nullptr) {
    token_info = maybe_regexp;
  } else {
    token_info = Current();
  }
  TokenInfo info = *token_info;
  
  ParseResult parse_result;
  
  Parsed* parsed = GetMemoizedRecord(token_info->source_position());
  if (parsed != nullptr) {
    RestoreParserState(parsed->parser_state());
    return parsed->parse_result();
  }


  YATSC_SCOPED([&] {
    Memoize(info.source_position(), parse_result);
  });
  
  
  switch (token_info->type()) {
    case Token::TS_IDENTIFIER: {
      return parse_result = ParseIdentifierReference(yield);
    }
    case Token::TS_THIS: {
      // parse a this.
      Handle<ir::Node> this_view = New<ir::ThisView>();
      this_view->SetInformationForNode(token_info);
      Next();
      return parse_result = Success(this_view);
    }
    case Token::TS_LEFT_BRACE:
      // parse an object literal.
      return parse_result = ParseObjectLiteral(yield);
    case Token::TS_LEFT_BRACKET:
      // parse an array literal.
      return parse_result = ParseArrayInitializer(yield);
    case Token::TS_LEFT_PAREN: {
      RecordedParserState rps = parser_state();
      Next();
      if (Current()->type() == Token::TS_FOR) {
        RestoreParserState(rps);
        return parse_result = ParseGeneratorComprehension(yield);
      } else {
        auto node = parse_result = ParseExpression(true, false);
        CHECK_AST(node);
        if (Current()->type() == Token::TS_RIGHT_PAREN) {
          Next();
          return parse_result = node;
        }
      }
      SYNTAX_ERROR_AND("')' expected", Current(), return parse_result = ParseResult());
    }
    case Token::TS_REGULAR_EXPR: {
      return parse_result = ParseRegularExpression();
    }
    case Token::TS_TEMPLATE_LITERAL: {
      return parse_result = ParseTemplateLiteral();
    }
    case Token::TS_FUNCTION:
      return parse_result = ParseFunctionOverloadOrImplementation(ir::Node::Null(), yield, false, false);
    case Token::TS_CLASS:
      return parse_result = ParseClassDeclaration(yield, false);
    default:
      // parse a literal.
      return parse_result = ParseLiteral();
  }
}


template<typename UCharInputIterator>
ParseResult Parser<UCharInputIterator>::ParseArrayLiteral(bool yield) {
  LOG_PHASE(ParseArrayLiteral);

  auto array_literal = New<ir::ArrayLiteralView>();
  array_literal->SetInformationForNode(Current());
  Next();

  if (Current()->type() == Token::TS_RIGHT_BRACKET) {
    Next();
    return Success(array_literal);
  }

  bool success = true;
    
  while (1) {
    ParseResult expr_result;
    bool spread = false;
    if (Current()->type() == Token::TS_COMMA) {
      expr_result = Success(New<ir::UndefinedView>());
      expr_result.value()->SetInformationForNode(Current());
      Next();
    } else if (Current()->type() == Token::TS_REST) {
      Next();
      expr_result = ParseAssignmentExpression(true, yield);
      spread = true;
    } else {
      expr_result = ParseAssignmentExpression(true, yield);
    }
      
    CHECK_AST(expr_result);
    array_literal->InsertLast(expr_result.value());
      
    if (Current()->type() == Token::TS_COMMA) {
      if (spread) {
        SYNTAX_ERROR("array spread element must be the end of the array element list", Current());
      }
      Next();
      if (Current()->type() == Token::TS_RIGHT_BRACKET) {
        Next();
        break;
      }
    } else if (Current()->type() == Token::TS_RIGHT_BRACKET) {
      Next();
      break;
    } else {
      SYNTAX_ERROR("unexpected token in 'array literal'", Current());
    }
  }
  return Success(array_literal);
}


template<typename UCharInputIterator>
ParseResult Parser<UCharInputIterator>::ParseArrayComprehension(bool yield) {
  LOG_PHASE(ParseArrayComprehension);
  TokenInfo info = *Current();
  Next();
  auto comprehension_result = ParseComprehension(false, yield);
  CHECK_AST(comprehension_result);
  if (Current()->type() == Token::TS_RIGHT_BRACKET) {
    Next();
    auto arr = New<ir::ArrayLiteralView>();
    arr->SetInformationForNode(&info);
    arr->InsertLast(comprehension_result.value());
    return Success(arr);
  }
  SYNTAX_ERROR("']' expected", Current());
}


template<typename UCharInputIterator>
ParseResult Parser<UCharInputIterator>::ParseComprehension(bool generator, bool yield) {
  LOG_PHASE(ParseComprehension);
  auto comp_for_result = ParseComprehensionFor(yield);
  auto comp_tail_result = ParseComprehensionTail(yield);
  CHECK_AST(comp_for_result);
  CHECK_AST(comp_tail_result);
  auto expr = New<ir::ComprehensionExprView>(generator, comp_for_result.value(), comp_tail_result.value());
  expr->SetInformationForNode(comp_for_result.value());
  return Success(expr);
};


template<typename UCharInputIterator>
ParseResult Parser<UCharInputIterator>::ParseComprehensionTail(bool yield) {
  LOG_PHASE(ParseComprehensionTail);
  if (Current()->type() == Token::TS_FOR) {
    
    auto comp_for_result = ParseComprehensionFor(yield);
    CHECK_AST(comp_for_result);
    Handle<ir::ForStatementView> stmt(comp_for_result.value());
    auto comp_tail_result = ParseComprehensionTail(yield);
    CHECK_AST(comp_tail_result);
    stmt->set_body(comp_tail_result.value());
    return Success(stmt);
    
  } else if (Current()->type() == Token::TS_IF) {
    
    auto comp_if_result = ParseComprehensionIf(yield);
    CHECK_AST(comp_if_result);
    Handle<ir::IfStatementView> stmt(comp_if_result.value());
    auto comp_tail_result = ParseComprehensionTail(yield);
    CHECK_AST(comp_tail_result);
    stmt->set_then_block(comp_tail_result.value());
    return Success(stmt);
  }
  return ParseAssignmentExpression(true, yield);
}


template<typename UCharInputIterator>
ParseResult Parser<UCharInputIterator>::ParseComprehensionFor(bool yield) {
  LOG_PHASE(ParseComprehensionFor);
  
  TokenInfo info = *Current();
  Next();
  if (Current()->type() == Token::TS_LEFT_PAREN) {
    Next();
    auto for_bindig_result = ParseForBinding(yield);
    CHECK_AST(for_bindig_result);
    if (Current()->type() == Token::TS_IDENTIFIER &&
        Current()->value()->Equals("of")) {
      Next();
      auto assignment_expr_result = ParseAssignmentExpression(true, yield);
      CHECK_AST(assignment_expr_result);
      if (Current()->type() == Token::TS_RIGHT_PAREN) {
        Next();
        auto for_expr = New<ir::ForOfStatementView>();
        for_expr->set_property_name(for_bindig_result.value());
        for_expr->set_expr(assignment_expr_result.value());
        for_expr->SetInformationForNode(&info);
        return Success(for_expr);
      }
      SYNTAX_ERROR("')' expected", Current());
    }
    SYNTAX_ERROR("'of' expected", Current());
  }
  SYNTAX_ERROR("'(' expected", Current());
}


template<typename UCharInputIterator>
ParseResult Parser<UCharInputIterator>::ParseComprehensionIf(bool yield) {
  LOG_PHASE(ParseComprehensionIf);

  TokenInfo info = *Current();
  Next();
  if (Current()->type() == Token::TS_LEFT_PAREN) {
    Next();
    auto assignment_expr_result = ParseAssignmentExpression(true, yield);
    CHECK_AST(assignment_expr_result);
    if (Current()->type() == Token::TS_RIGHT_PAREN) {
      Next();
      auto if_expr = New<ir::IfStatementView>();
      if_expr->set_expr(assignment_expr_result.value());
      if_expr->SetInformationForNode(&info);
      return Success(if_expr);
    }
    SYNTAX_ERROR("')' expected", Current());
  }
  SYNTAX_ERROR("'(' expected", Current());
}


template<typename UCharInputIterator>
ParseResult Parser<UCharInputIterator>::ParseGeneratorComprehension(bool yield) {
  LOG_PHASE(ParseGeneratorComprehension);

  Next();
  bool success = true;
  auto comp_result = ParseComprehension(true, yield);
  SKIP_TOKEN_IF(comp_result, success, Token::TS_RIGHT_PAREN);
  if (Current()->type() == Token::TS_RIGHT_PAREN) {
    return Success(comp_result.value());
  }
  SYNTAX_ERROR("')' expected", Current());
}


template<typename UCharInputIterator>
ParseResult Parser<UCharInputIterator>::ParseYieldExpression(bool in) {
  LOG_PHASE(ParseYieldExpression);

  TokenInfo info = *Current();
  Next();

  bool end = Current()->type() == Token::LINE_TERMINATOR;
    
  if (info.has_line_break_before_next() ||
      info.has_line_terminator_before_next() ||
      end) {

    auto yield_expr = New<ir::YieldView>(false, ir::Node::Null());
    yield_expr->SetInformationForNode(Current());
      
    if (end) {
      Next();
    }
      
    return Success(yield_expr);
  }
    
  bool continuation = false;
  Next();
    
  if (Current()->type() == Token::TS_MUL) {
    Next();
    continuation = true;
  }

  auto assignment_expr_result = ParseAssignmentExpression(in, true);
  CHECK_AST(assignment_expr_result);
  auto yield_expr = New<ir::YieldView>(continuation, assignment_expr_result.value());
  yield_expr->SetInformationForNode(Current());
  return Success(yield_expr);
}


template<typename UCharInputIterator>
ParseResult Parser<UCharInputIterator>::ParseForBinding(bool yield) {
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
ParseResult Parser<UCharInputIterator>::ParseObjectLiteral(bool yield) {
  LOG_PHASE(ParseObjectLiteral);

  Handle<ir::ObjectLiteralView> object_literal = New<ir::ObjectLiteralView>();
  object_literal->SetInformationForNode(Current());
  Handle<ir::Properties> prop = object_literal->properties();
  Next();

  if (Current()->type() == Token::TS_RIGHT_BRACE) {
    Next();
    return Success(object_literal);
  }

  bool success = true;
    
  while (1) {
    auto prop_definition_result = ParsePropertyDefinition(yield);
    SKIP_TOKEN_OR(prop_definition_result, success, Token::TS_RIGHT_BRACE) {
      object_literal->InsertLast(prop_definition_result.value());
      if (prop_definition_result.value()->first_child()->HasSymbol()) {
        prop->Declare(prop_definition_result.value()->first_child()->symbol(), prop_definition_result.value());
      }
    }

    if (Current()->type() == Token::TS_COMMA) {
      Next();
      if (Current()->type() == Token::TS_RIGHT_BRACE) {
        Next();
        break;
      }
    } else if (Current()->type() == Token::TS_RIGHT_BRACE) {
      Next();
      break;
    } else {
      SYNTAX_ERROR("expected ',' or '}'", Current());
    }
  }
  return Success(object_literal);
}


template <typename UCharInputIterator>
ParseResult Parser<UCharInputIterator>::ParsePropertyDefinition(bool yield) {
  LOG_PHASE(ParsePropertyDefinition);
  ParseResult value_result;
  ParseResult key_result;
  bool generator = false;
  bool getter = false;
  bool setter = false;
  TokenInfo info = *Current();
   
  if (Current()->type() == Token::TS_IDENTIFIER &&
      Current()->value()->Equals("get")) {
      getter = true;
      Next();
  } else if (Current()->type() == Token::TS_IDENTIFIER &&
             Current()->value()->Equals("set")) {
      setter = true;
      Next();
  }

    if (Current()->type() == Token::TS_IDENTIFIER) {
      key_result = ParseIdentifierReference(yield);
    } else {
      key_result = ParsePropertyName(yield, false);
    }

    if (key_result) {
      if (key_result.value()->HasSymbol()) {
        key_result.value()->symbol()->set_type(ir::SymbolType::kPropertyName);
      }
    } else {
      if (getter || setter) {
        key_result = Success(New<ir::NameView>(NewSymbol(ir::SymbolType::kPropertyName, info.value())));
        key_result.value()->SetInformationForNode(&info);
      } else {
        return Failed();
      }
    }

  if (Current()->type() == Token::TS_MUL) {
    generator = true;
    Next();
  }
  
  if (Current()->type() == Token::TS_LEFT_PAREN) {
    auto call_sig_result = ParseCallSignature(yield, false);
    CHECK_AST(call_sig_result);
    if (Current()->type() == Token::TS_LEFT_BRACE) {
      auto function_body_result = ParseFunctionBody(yield || generator);
      CHECK_AST(function_body_result);
      value_result = Success(New<ir::FunctionView>(getter, setter, generator, New<ir::FunctionOverloadsView>(),
                                            ir::Node::Null(), call_sig_result.value(), function_body_result.value()));
    }
  } else if (generator) {
    SYNTAX_ERROR("'(' expected", Current());
  } else if (Current()->type() == Token::TS_COLON) {
    Next();
    value_result = ParseAssignmentExpression(true, false);
    CHECK_AST(value_result);
  }
  Handle<ir::ObjectElementView> element = New<ir::ObjectElementView>(key_result.value(), value_result.value());
  element->SetInformationForNode(&info);
  return Success(element);
}


template <typename UCharInputIterator>
ParseResult Parser<UCharInputIterator>::ParsePropertyName(bool yield, bool generator_parameter) {
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
ParseResult Parser<UCharInputIterator>::ParseLiteralPropertyName() {
  LOG_PHASE(ParseLiteralPropertyName);
  if (TokenInfo::IsKeyword(Current()->type())) {
    Current()->set_type(Token::TS_IDENTIFIER);
  }
  switch (Current()->type()) {
    case Token::TS_IDENTIFIER:
      return ParseIdentifier();
    case Token::TS_STRING_LITERAL:
      return ParseStringLiteral();
    case Token::TS_NUMERIC_LITERAL:
      return ParseNumericLiteral();
    default:
      if (TokenInfo::IsKeyword(Current()->type())) {
        Current()->set_type(Token::TS_IDENTIFIER);
        return ParseIdentifier();
      }
      SYNTAX_ERROR("identifier or string literal or numeric literal expected", Current());
  }
}


template <typename UCharInputIterator>
ParseResult Parser<UCharInputIterator>::ParseComputedPropertyName(bool yield) {
  LOG_PHASE(ParseComputedPropertyName);

  Next();
  auto assignment_expr_result = ParseAssignmentExpression(true, yield);
  CHECK_AST(assignment_expr_result);
  if (Current()->type() == Token::TS_RIGHT_BRACKET) {
    Next();
    return assignment_expr_result;
  }
  SYNTAX_ERROR("']' expected", Current());
}


template <typename UCharInputIterator>
ParseResult Parser<UCharInputIterator>::ParseLiteral() {
  LOG_PHASE(ParseLiteral);
  switch (Current()->type()) {
    case Token::TS_NULL: {
      auto node = New<ir::NullView>();
      node->SetInformationForNode(Current());
      Next();
      return Success(node);
    }
    default: {
      return ParseValueLiteral();
    }
  }
}


template <typename UCharInputIterator>
ParseResult Parser<UCharInputIterator>::ParseValueLiteral() {
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
      SYNTAX_ERROR("boolean or numeric literal or string literal expected", Current());
  }
}


template <typename UCharInputIterator>
ParseResult Parser<UCharInputIterator>::ParseArrayInitializer(bool yield) {
  LOG_PHASE(ParseArrayInitializer);

  RecordedParserState rps = parser_state();
  auto array_literal_result = ParseArrayLiteral(yield);
  if (!array_literal_result) {      
    RestoreParserState(rps);
    if (!LanguageModeUtil::IsES6(compiler_option_)) {
      SYNTAX_ERROR("ArrayComprehension is not allowed except es6 mode.", Current());
    }
    array_literal_result = ParseArrayComprehension(yield);
    CHECK_AST(array_literal_result);
  }
  return array_literal_result;
}


template <typename UCharInputIterator>
ParseResult Parser<UCharInputIterator>::ParseIdentifierReference(bool yield) {
  LOG_PHASE(ParseIdentifierReference);
  if (Current()->type() == Token::TS_IDENTIFIER) {
    return ParseIdentifier();
  } else if (Current()->type() == Token::TS_YIELD) {
    if (!yield) {
      SYNTAX_ERROR("'yield' not allowed here", Current());
    }
    auto node = New<ir::YieldView>(false, ir::Node::Null());
    node->SetInformationForNode(Current());
    return Success(node);
  }
  SYNTAX_ERROR("'identifier' or 'yield' expected", Current());
}


template <typename UCharInputIterator>
ParseResult Parser<UCharInputIterator>::ParseBindingIdentifier(bool default_allowed, bool yield) {
  LOG_PHASE(ParseBindingIdentifier);
  ParseResult parse_result;
  if (Current()->type() == Token::TS_DEFAULT) {
    if (!default_allowed) {
      SYNTAX_ERROR("'default' keyword not allowed here", Current());
    }
    parse_result = Success(New<ir::DefaultView>());
    Next();
  } else if (Current()->type() == Token::TS_YIELD) {
    Next();
    auto identifier_result = ParseIdentifier();
    CHECK_AST(identifier_result);
    parse_result = Success(New<ir::YieldView>(false, identifier_result.value()));
    Next();
  } else if (Current()->type() == Token::TS_IDENTIFIER) {
    parse_result = ParseIdentifier();
    CHECK_AST(parse_result);
  } else {
    SYNTAX_ERROR("'default', 'yield' or 'identifier' expected", Current());
  }

  parse_result.value()->SetInformationForNode(Current());

  return parse_result;
}

template <typename UCharInputIterator>
ParseResult Parser<UCharInputIterator>::ParseLabelIdentifier(bool yield) {
  LOG_PHASE(ParseLabelIdentifier);
  if (Current()->type() == Token::TS_YIELD && yield) {
    SYNTAX_ERROR("yield not allowed here", Current());
  }
  if (Current()->type() == Token::TS_YIELD) {
    auto node = New<ir::YieldView>(false, ir::Node::Null());
    node->SetInformationForNode(Current());
    return Success(node);
  }
  return ParseIdentifier();
}



template <typename UCharInputIterator>
ParseResult Parser<UCharInputIterator>::ParseIdentifier() {
  LOG_PHASE(ParseIdentifier);
  if (Current()->type() == Token::TS_IDENTIFIER) {
    auto node = New<ir::NameView>(NewSymbol(ir::SymbolType::kVariableName, Current()->value()));
    node->SetInformationForNode(Current());
    Next();
    return Success(node);
  }
  SYNTAX_ERROR("'identifier' expected", Current());
}


template <typename UCharInputIterator>
ParseResult Parser<UCharInputIterator>::ParseStringLiteral() {
  LOG_PHASE(ParseStringLiteral);
  if (Current()->type() == Token::TS_STRING_LITERAL) {
    auto string_literal = New<ir::StringView>(Current()->value());
    string_literal->SetInformationForNode(Current());
    Next();
    return Success(string_literal);
  }
  SYNTAX_ERROR("string literal expected", Current());
}


template <typename UCharInputIterator>
ParseResult Parser<UCharInputIterator>::ParseNumericLiteral() {
  LOG_PHASE(ParseNumericLiteral);
  if (Current()->type() == Token::TS_NUMERIC_LITERAL) {
    auto number = New<ir::NumberView>(Current()->value());
    number->SetInformationForNode(Current());
    Next();
    return Success(number);
  }
  SYNTAX_ERROR("numeric literal expected", Current());
}


template <typename UCharInputIterator>
ParseResult Parser<UCharInputIterator>::ParseBooleanLiteral() {
  LOG_PHASE(ParseBooleanLiteral);
  ParseResult parse_result;
  if (Current()->type() == Token::TS_TRUE) {
    parse_result = Success(New<ir::TrueView>());
  } else if (Current()->type() == Token::TS_FALSE) {
    parse_result = Success(New<ir::FalseView>());
  }

  if (parse_result) {
    parse_result.value()->SetInformationForNode(Current());
    Next();
    return parse_result;
  }
  SYNTAX_ERROR("boolean literal expected", Current());
}


template <typename UCharInputIterator>
ParseResult Parser<UCharInputIterator>::ParseUndefinedLiteral() {
  LOG_PHASE(ParseUndefinedLiteral);
  if (Current()->type() == Token::TS_UNDEFINED) {
    auto node = New<ir::UndefinedView>();
    node->SetInformationForNode(Current());
    Next();
    return Success(node);
  }
  SYNTAX_ERROR("'undefined' expected", Current());
}


template <typename UCharInputIterator>
ParseResult Parser<UCharInputIterator>::ParseNaNLiteral() {
  LOG_PHASE(ParseNaNLiteral);
  if (Current()->type() == Token::TS_NAN) {
    auto node = New<ir::NaNView>();
    node->SetInformationForNode(Current());
    Next();
    return Success(node);
  }
  SYNTAX_ERROR("'NaN' expected", Current());
}


template <typename UCharInputIterator>
ParseResult Parser<UCharInputIterator>::ParseRegularExpression() {
  LOG_PHASE(ParseRegularExpression);
  if (Current()->type() == Token::TS_REGULAR_EXPR) {
    auto reg_expr = New<ir::RegularExprView>(Current()->value());
    reg_expr->SetInformationForNode(Current());
    Next();
    return Success(reg_expr);
  }
  SYNTAX_ERROR("regular expression expected", Current());
}


template <typename UCharInputIterator>
ParseResult Parser<UCharInputIterator>::ParseTemplateLiteral() {
  LOG_PHASE(ParseTemplateLiteral);
  if (Current()->type() == Token::TS_TEMPLATE_LITERAL) {
    auto template_literal = New<ir::TemplateLiteralView>(Current()->value());
    Next();
    template_literal->SetInformationForNode(Current());
    return Success(template_literal);
  }
  SYNTAX_ERROR("template literal expected", Current());
}


template <typename UCharInputIterator>
typename Parser<UCharInputIterator>::AccessorType Parser<UCharInputIterator>::ParseAccessor() {
  LOG_PHASE(ParseAccessor);
  
  bool getter = false;
  bool setter = false;

  TokenInfo info = *Current();
  RecordedParserState rps = parser_state();
  // Parse the getter or setter if inditifer is the get or set.
  if (Current()->type() == Token::TS_IDENTIFIER &&
      Current()->value()->Equals("get")) {
    getter = true;
    Next();
  } else if (Current()->type() == Token::TS_IDENTIFIER &&
             Current()->value()->Equals("set")) {
    setter = true;
    Next();
  }

  // If next token is left paren and getter or setter is true,
  // get or set keyword is not treated as the keyword,
  // so change current curosr position before the get or set.
  if ((getter || setter) &&
      (Current()->type() == Token::TS_LEFT_PAREN ||
       Current()->type() == Token::TS_LESS)) {
    getter = setter = false;
    RestoreParserState(rps);
  }

  return AccessorType(setter, getter, info);
}
}
