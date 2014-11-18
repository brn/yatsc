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
// Parse expression and comma expression.
template <typename UCharInputIterator>
ParseResult Parser<UCharInputIterator>::ParseExpression(bool in, bool yield) {
  LOG_PHASE(ParseExpression);
  auto assignment_expr_result = ParseAssignmentExpression(in, yield);
  CHECK_AST(assignment_expr_result);
  
  auto assignment_expr = assignment_expr_result.value();

  if (!cur_token()->Is(TokenKind::kComma)) {
    return assignment_expr_result;
  }

  // Parse comma expressions.
  Next();
  Handle<ir::CommaExprView> comma_expr = New<ir::CommaExprView>({assignment_expr});
  comma_expr->SetInformationForNode(*cur_token());
  
  while (1) {
    assignment_expr_result = ParseAssignmentExpression(in, yield);
    CHECK_AST(assignment_expr_result);
      
    assignment_expr = assignment_expr_result.value();
    comma_expr->InsertLast(assignment_expr);

    // If ',' find, comma expr is continued.
    if (cur_token()->Is(TokenKind::kComma)) {
      Next();
      continue;
    }
    
    return Success(comma_expr);
  }
}


// AssignmentPattern[Yield]
//   ObjectAssignmentPattern[?Yield]
//   ArrayAssignmentPattern[?Yield]
//
// Parse assignment patterns.
template <typename UCharInputIterator>
ParseResult Parser<UCharInputIterator>::ParseAssignmentPattern(bool yield) {
  LOG_PHASE(ParseAssignmentPattern);
  
  Token token = *cur_token();
  ParseResult result;
  
  switch(cur_token()->type()) {
    case TokenKind::kLeftBrace: {
      result = ParseObjectAssignmentPattern(yield);
      break;
    }
    case TokenKind::kLeftBracket: {
      result = ParseArrayAssignmentPattern(yield);
      break;
    }
    default:
      SYNTAX_ERROR("assignment pattern begin with '{' or '[' expected.", (&token));
  }

  CHECK_AST(result);

  result.value()->SetInformationForNode(&token);
  return result;
}


// ObjectAssignmentPattern[Yield]
//   { }
//   { AssignmentPropertyList[?Yield] }
//   { AssignmentPropertyList[?Yield] , }
//
// Parse object type destructuring assignment.
template <typename UCharInputIterator>
ParseResult Parser<UCharInputIterator>::ParseObjectAssignmentPattern(bool yield) {
  LOG_PHASE(ParseObjectAssignmentPattern);

  // This method begin with kLeftBrace, so advance and consume token.
  Next();
  
  auto result = ParseAssignmentPropertyList(yield);
  CHECK_AST(result);

  // typescript allow trailing comma in the object like expressions.
  if (cur_token()->Is(TokenKind::kComma)) {
    Next();
  }
  
  if (cur_token()->Is(TokenKind::kRightBrace)) {
    Next();
    return result;
  }
  
  SYNTAX_ERROR("'}' expected.", cur_token());
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
// Parse array type destructuring assignment pattern.
template <typename UCharInputIterator>
ParseResult Parser<UCharInputIterator>::ParseArrayAssignmentPattern(bool yield) {
  LOG_PHASE(ParseArrayAssignmentPattern);

  bool success = true;
  
  auto array_view = New<ir::BindingArrayView>();
  array_view->SetInformationForNode(cur_token());

  // This method begin with kLeftBracket, so advance and consume token.
  Next();

  while (1) {
    
    Maybe<Token> rest_token;
      
    // '[, ...' pattern
    if (cur_token()->Is(TokenKind::kComma)) {
      Next();
      array_view->InsertLast(New<ir::UndefinedView>());
      array_view->InsertLast(New<ir::UndefinedView>());
    }

    // Parse spread pattern.
    // '[...x]'
    if (cur_token()->Is(TokenKind::kRest)) {
      auto result = ParseAssignmentRestElement(yield);
    
      // If failed, skip all tokens until RightBracket encounted.
      SKIP_TOKEN_IF_AND(result, success, TokenKind::kRightBracket, return Failed());
    
      array_view->InsertLast(result.value());
      rest_token = Just(*cur_token());
      
    } else if (cur_token()->Is(TokenKind::kRightBracket)) {
      // Parse closed array initializer.
      // If array pattern element count is zero, it's treated as SyntaxError.
      if (array_view->size() == 0) {
        SYNTAX_ERROR("destructuring assignment left hand side is not allowed empty array", cur_token());
      }
    
      Next();
      return Success(array_view);
    }

    // The ParameterRest is not allowed in any position of array pattern except the last element.
    if (rest_token) {
      SYNTAX_ERROR("destructuring assignment rest expression must be the end of the elements.", (rest_token.value()));
    } else if (cur_token()->Is(TokenKind::kComma)) {
      Next();
      continue;
    }

    // If reached to this line, that mean token not machted with '[' or ',',
    // so produce error.
    SYNTAX_ERROR("unexpected token. ']' or ',' expected.", cur_token());
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

    SKIP_TOKEN_OR(result, success, TokenKind::kComma) {
      prop_list->InsertLast(result.value());
    }
    
    if (cur_token()->type() == TokenKind::kComma) {
      Next();
      continue;
    } else if (cur_token()->type() == TokenKind::kRightBrace) {
      return Success(prop_list);
    } else {
      SYNTAX_ERROR("',' or '}' expected.", cur_token());
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
  Token info = *cur_token();
  
  ParseResult property_name;
  ParseResult elem;

  // Check whether property name is identifier reference or not.
  bool identifier = false;
  
  if (cur_token()->Is(TokenKind::kIdentifier) {
    identifier = true;
    property_name = ParseIdentifierReference(yield);
  } else if (cur_token()->Is(TokenKind::kStringLiteral) ||
             cur_token()->Is(TokenKind::kNumericLiteral) ||
             cur_token()->Is(TokenKind::kLeftBracket)) {
    property_name = ParsePropertyName(yield, false);
  }
  CHECK_AST(property_name);
  
  ParseResult init;

  // Initializer is only allowed, if property name is identifier reference.
  if (identifier && cur_token()->Is(TokenKind::kAssign)) {
    Next();
    init = ParseAssignmentExpression(true, yield);
    CHECK_AST(init);
  } else if (cur_token()->Is(TokenKind::kColon)) {
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
  CHECK_AST(target);
  
  ParseResult init;
  
  if (cur_token()->type() == TokenKind::kAssign) {
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

  Token info = *cur_token();
  Next();
  
  return ParseDestructuringAssignmentTarget(yield).fmap([&](target) {
    auto rest = New<ir::RestParamView>(target.value());
    rest->SetInformationForNode(&info);
    return rest;
  });
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
bool IsAssignmentOp(TokenKind type) {
  return type == TokenKind::kAssign || type == TokenKind::kMulLet ||
    type == TokenKind::kDivLet || type == TokenKind::kModLet ||
    type == TokenKind::kAddLet || type == TokenKind::kSubLet ||
    type == TokenKind::kShiftLeftLet || type == TokenKind::kShiftRightLet ||
    type == TokenKind::kUShiftRightLet || type == TokenKind::kAndLet ||
    type == TokenKind::kNorLet || type == TokenKind::kOrLet ||
    type == TokenKind::kXorLet;
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
  
  if (cur_token()->Is(TokenKind::kLeftParen) ||
      cur_token()->Is(TokenKind::kLess)) {
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

  if (cur_token()->Is(TokenKind::kYield)) {
    if (!yield) {
      SYNTAX_ERROR("invalid use of 'yield' keyword", cur_token());
    }
    expr_result = ParseYieldExpression(in);
    CHECK_AST(expr_result);
  } else {  
    expr_result = ParseConditionalExpression(in, yield);
    if (expr_result) {
      auto expr = expr_result.value();
      if (expr->HasNameView() &&
          cur_token()->Is(TokenKind::kArrowGlyph)) {
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
  const Token *token_info = cur_token();
  TokenKind type = token_info->type();

  // Check assignment operators.
  if (IsAssignmentOp(type)) {
    if (!parsed_as_assignment_pattern &&
        (expr_result.value()->HasObjectLiteralView() ||
         expr_result.value()->HasArrayLiteralView())) {
      
      RestoreParserState(rps);

      if (!LanguageModeUtil::IsES6(compiler_option_)) {
        SYNTAX_ERROR("Invalid Left-Hand-Side expression", cur_token());
      }
      
      expr_result = ParseAssignmentPattern(yield);
      CHECK_AST(expr_result);
    }
    
    Next();

    // Check left hand side expression is valid as an expression.
    // If left hand side expression is like 'func()',
    // that is invalid expression.
    if (expr_result.value()->IsValidLhs()) {
      return ParseAssignmentExpression(in, yield).fmap([expr_result, &](rhs_result) {
        auto result = New<ir::AssignmentView>(type, expr_result.value(), rhs_result.value());
        result->SetInformationForNode(expr_result.value());
        return Success(result);
      });
    }
    SYNTAX_ERROR("invalid left hand side expression in 'assignment expression'", cur_token());
  } else if (parsed_as_assignment_pattern) {
    SYNTAX_ERROR("destructuring assignment must be initialized", cur_token());
  }
  
  return expr_result;
}


template <typename UCharInputIterator>
ParseResult Parser<UCharInputIterator>::ParseArrowFunction(bool in, bool yield, Handle<ir::Node> identifier) {
  LOG_PHASE(ParseArrowFunction);
  return ParseArrowFunctionParameters(yield, identifier).fmap([&](call_sig) {
    return ParseConciseBody(in, call_sig.value());
  })
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
  if (cur_token()->type() != TokenKind::kArrowGlyph) {
    SYNTAX_ERROR("'=>' expected", cur_token());
  }
  Next();
  return call_sig_result;
}


template <typename UCharInputIterator>
ParseResult Parser<UCharInputIterator>::ParseConciseBody(bool in, Handle<ir::Node> call_sig) {
  LOG_PHASE(ParseConciseBody);
  ParseResult concise_body_result;
  if (cur_token()->type() == TokenKind::kLeftBrace) {
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
  
  if (cur_token()->type() == TokenKind::kQuestionMark) {
    Next();
    auto left_result = ParseAssignmentExpression(in, yield);
    CHECK_AST(left_result);
    
    if (cur_token()->type() == TokenKind::kColon) {
      Next();
      auto right_result = ParseAssignmentExpression(in, yield);
      CHECK_AST(right_result);      
      auto temary = New<ir::TemaryExprView>(logical_or_expr_result.value(), left_result.value(), right_result.value());
      temary->SetInformationForNode(logical_or_expr_result.value());
      temary->MarkAsInValidLhs();
      return Success(temary);
    }
    SYNTAX_ERROR("unexpected token in 'temary expression'", cur_token());
  }
  return logical_or_expr_result;
}


#define PARSE_BINARY_EXPRESSION_INTERNAL(check, name, next)             \
  template <typename UCharInputIterator>                                \
  ParseResult Parser<UCharInputIterator>::name(bool in, bool yield) {   \
    LOG_PHASE(name);                                                    \
    auto ret = next;                                                    \
    CHECK_AST(ret);                                                     \
    while (1) {                                                         \
      if (check) {                                                      \
        auto tmp = ret;                                                 \
        TokenKind type = cur_token()->type();                             \
        Next();                                                         \
        auto n = next;                                                  \
        CHECK_AST(n);                                                   \
        ret = Success(New<ir::BinaryExprView>(type, ret.value(), n.value())); \
        ret.value()->SetInformationForNode(tmp.value());                \
      } else {                                                          \
        break;                                                          \
      }                                                                 \
    }                                                                   \
    return ret;                                                         \
  }


#define PARSE_BINARY_EXPRESSION(name, next, token)                      \
  PARSE_BINARY_EXPRESSION_INTERNAL((cur_token()->type() == TokenKind::token), name, (next(in, yield)))

#define PARSE_BINARY_EXPRESSION_WITH_COND(cond, name, next)         \
  PARSE_BINARY_EXPRESSION_INTERNAL((cond), name, (next(in, yield)))

#define PARSE_BINARY_EXPRESSION_WITH_CALL(cond, name, call) \
  PARSE_BINARY_EXPRESSION_INTERNAL((cond), name, (call))


// LogicalORExpression[In, Yield]
//   LogicalANDExpression[?In, ?Yield]
//   LogicalORExpression[?In, ?Yield] || LogicalANDExpression[?In, ?Yield]
//
PARSE_BINARY_EXPRESSION(ParseLogicalORExpression, ParseLogicalANDExpression, kLogicalOr)

// LogicalANDExpression[In, Yield]
// BitwiseORExpression[?In, ?Yield]
// LogicalANDExpression[?In, ?Yield] && BitwiseORExpression[?In, ?Yield]
//
PARSE_BINARY_EXPRESSION(ParseLogicalANDExpression, ParseBitwiseORExpression, kLogicalAnd)

// BitwiseORExpression[In, Yield]
//   BitwiseXORExpression[?In, ?Yield]
//   BitwiseORExpression[?In, ?Yield] | BitwiseXORExpression[?In, ?Yield]
//
PARSE_BINARY_EXPRESSION(ParseBitwiseORExpression, ParseBitwiseXORExpression, kBitOr)

// BitwiseXORExpression[In, Yield]
//   BitwiseANDExpression[?In, ?Yield]
//   BitwiseXORExpression[?In, ?Yield] ^ BitwiseANDExpression[?In, ?Yield]
//
PARSE_BINARY_EXPRESSION(ParseBitwiseXORExpression, ParseBitwiseANDExpression, kBitXor)


// BitwiseANDExpression[In, Yield]
//   EqualityExpression[?In, ?Yield]
//   BitwiseANDExpression[?In, ?Yield] & EqualityExpression[?In, ?Yield]
//
PARSE_BINARY_EXPRESSION(ParseBitwiseANDExpression, ParseEqualityExpression, kBitAnd)


// EqualityExpression[In, Yield]
//   RelationalExpression[?In, ?Yield]
//   EqualityExpression[?In, ?Yield] == RelationalExpression[?In, ?Yield]
//   EqualityExpression[?In, ?Yield] != RelationalExpression[?In, ?Yield]
//   EqualityExpression[?In, ?Yield] ===RelationalExpression[?In, ?Yield]
//   EqualityExpression[?In, ?Yield] !==RelationalExpression[?In, ?Yield]
//
PARSE_BINARY_EXPRESSION_WITH_COND((cur_token()->type() == TokenKind::kEq || cur_token()->type() == TokenKind::kNotEq || cur_token()->type() == TokenKind::kEqual || cur_token()->type() == TokenKind::kNotEqual), ParseEqualityExpression, ParseRelationalExpression)

#define RELATIONAL_COND                             \
  cur_token()->type() == TokenKind::kLess ||            \
    cur_token()->type() == TokenKind::kGreater ||       \
    cur_token()->type() == TokenKind::kLessEqual ||    \
    cur_token()->type() == TokenKind::kGreaterEqual || \
    cur_token()->type() == TokenKind::kInstanceof ||    \
    (in && cur_token()->type() == TokenKind::kIn)

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
  cur_token()->type() == TokenKind::kShiftLeft ||    \
    cur_token()->type() == TokenKind::kShiftRight || \
    cur_token()->type() == TokenKind::kUShiftRight

// ShiftExpression[Yield]
//   AdditiveExpression[?Yield]
//   ShiftExpression[?Yield] << AdditiveExpression[?Yield]
//   ShiftExpression[?Yield] >> AdditiveExpression[?Yield]
//   ShiftExpression[?Yield] >>> AdditiveExpression[?Yield]
//
PARSE_BINARY_EXPRESSION_WITH_COND((SHIFT_COND), ParseShiftExpression, ParseAdditiveExpression)
#undef SHIFT_COND


#define ADDITIVE_COND                           \
  cur_token()->type() == TokenKind::kPlus ||        \
    cur_token()->type() == TokenKind::kMinus

// AdditiveExpression[Yield]
//   MultiplicativeExpression[?Yield]
//   AdditiveExpression[?Yield] + MultiplicativeExpression[?Yield]
//   AdditiveExpression[?Yield] - MultiplicativeExpression[?Yield]
//
PARSE_BINARY_EXPRESSION_WITH_COND(ADDITIVE_COND, ParseAdditiveExpression, ParseMultiplicativeExpression)
#undef ADDITIVE_COND


#define MULTIPLICATIVE_COND                     \
  cur_token()->type() == TokenKind::kMul ||         \
    cur_token()->type() == TokenKind::kMod ||       \
    cur_token()->type() == TokenKind::kDiv

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
  TokenKind type = cur_token()->type();
  switch (type) {
    case TokenKind::kDelete:
    case TokenKind::kVoid:
    case TokenKind::kTypeof:
    case TokenKind::kIncrement:
    case TokenKind::kDecrement:
    case TokenKind::kPlus:
    case TokenKind::kMinus:
    case TokenKind::kBitNor:
    case TokenKind::kNot: {
      Next();
      auto unary_expr_result = ParseUnaryExpression(yield);
      CHECK_AST(unary_expr_result);
      auto ret = New<ir::UnaryExprView>(type, unary_expr_result.value());
      ret->SetInformationForNode(unary_expr_result.value());
      return Success(ret);
    }
    case TokenKind::kLess: {
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
  if (cur_token()->type() == TokenKind::kIncrement ||
      cur_token()->type() == TokenKind::kDecrement) {
    auto ret = New<ir::PostfixView>(lhs_expr_result.value(), cur_token()->type());
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
  if (cur_token()->type() == TokenKind::kNew) {
    Next();
    if (cur_token()->type() == TokenKind::kNew) {
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
  if (cur_token()->type() == TokenKind::kSuper) {
    target = Success(New<ir::SuperView>());
    target.value()->SetInformationForNode(cur_token());
    Next();
    if (cur_token()->type() == TokenKind::kDot) {
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
  
  
  if (cur_token()->type() == TokenKind::kLeftParen ||
      cur_token()->type() == TokenKind::kLess) {
    auto arguments_result = ParseArguments(yield);
    if (!arguments_result) {return target;}
    target.value()->MarkAsInValidLhs();
    auto call = New<ir::CallView>(target.value(), arguments_result.value());
    call->SetInformationForNode(target.value());
    
    while (1) {
      switch (cur_token()->type()) {
        case TokenKind::kLess:
        case TokenKind::kLeftParen: {
          arguments_result = ParseArguments(yield);
          if (!arguments_result) {
            return Success(call);
          }
          call = New<ir::CallView>(call, arguments_result.value());
          call->MarkAsInValidLhs();
          call->SetInformationForNode(arguments_result.value());
          break;
        }
        case TokenKind::kLeftBracket:
        case TokenKind::kDot: {
          auto tmp = ParseGetPropOrElem(call, yield, false, false);
          CHECK_AST(tmp);
          call = tmp.value();
          break;
        }
        default:
          return Success(call);
      }
    }
  } else if (cur_token()->type() == TokenKind::kTemplateLiteral) {
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
  if (cur_token()->type() == TokenKind::kLess) {
    type_arguments_result = ParseTypeArguments();
    if (!type_arguments_result) {
      RestoreParserState(rps);
      return Failed();
    }
  }
  
  if (cur_token()->type() == TokenKind::kLeftParen) {
    Handle<ir::CallArgsView> args = New<ir::CallArgsView>();
    args->SetInformationForNode(cur_token());
    Next();
    if (cur_token()->type() == TokenKind::kRightParen) {
      Next();
      return BuildArguments(type_arguments_result, args, true);
    }
    
    bool has_rest = false;
    bool success = true;
    
    while (1) {

      if (cur_token()->type() == TokenKind::kRest) {
        Token info = *cur_token();
        Next();
        auto assignment_expr_result = ParseAssignmentExpression(true, yield);
        
        SKIP_TOKEN_OR(assignment_expr_result, success, TokenKind::kRightParen) {
          auto rest = New<ir::RestParamView>(assignment_expr_result.value());
          rest->SetInformationForNode(&info);
          args->InsertLast(rest);
          has_rest = true;
        }
      } else {
        auto assignment_expr_result = ParseAssignmentExpression(true, yield);
        
        SKIP_TOKEN_OR(assignment_expr_result, success, TokenKind::kRightParen) {
          args->InsertLast(assignment_expr_result.value());
        }
      }
      
      if (cur_token()->type() == TokenKind::kComma) {
        if (has_rest) {
          SYNTAX_ERROR("the spread argument must be the end of arguments", cur_token());
        }
        Next();
        continue;
      } else if (cur_token()->type() == TokenKind::kRightParen) {
        Next();
        return BuildArguments(type_arguments_result, args, success);
      }
      SYNTAX_ERROR("unexpected token in 'arguments'", cur_token());
    }
  }
  SYNTAX_ERROR("'(' expected.", cur_token());
}


template <typename UCharInputIterator>
ParseResult Parser<UCharInputIterator>::ParseNewExpression(bool yield) {
  LOG_PHASE(ParseNewExpression);
  RecordedParserState rps = parser_state();
  Next();
  if (cur_token()->type() == TokenKind::kNew) {
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
  Token* token_info = cur_token();
  if (token_info->type() ==  TokenKind::kNew) {
    Token info = *cur_token();
    // Parse new Foo() expression.
    Next();
    ParseResult member;
    if (cur_token()->type() == TokenKind::kSuper) {
      member = Success(New<ir::SuperView>());
      member.value()->SetInformationForNode(&info);
    } else {
      member = ParseMemberExpression(yield);
      CHECK_AST(member);
    }

    
    // New expression can omit parens.
    // If paren exists, continue parsing.
    if (cur_token()->type() == TokenKind::kLeftParen ||
        cur_token()->type() == TokenKind::kLess) {
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
  } else if (token_info->type() == TokenKind::kSuper) {
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
    switch (cur_token()->type()) {
      case TokenKind::kLeftBracket: {
        if (dot_only) {
          if (is_throw) {
            SYNTAX_ERROR("'.' expected.", cur_token());
          }
          return Success(node);
        }
        // [...] expression.
        Next();
        auto expr_result = ParseExpression(true, false);
        CHECK_AST(expr_result);
        node = New<ir::GetElemView>(node, expr_result.value());
        node->SetInformationForNode(node);
        if (cur_token()->type() != TokenKind::kRightBracket) {
          SYNTAX_ERROR("unexpected token", cur_token());
        }
        Next();
        break;
      }
      case TokenKind::kDot: {
        // a.b.c expression.
        Next();
        if (cur_token()->type() != TokenKind::kIdentifier &&
            !Token::IsKeyword(cur_token()->type())) {
          SYNTAX_ERROR("'identifier' expected.", cur_token());
        }
        if (Token::IsKeyword(cur_token()->type())) {
          cur_token()->set_type(TokenKind::kIdentifier);
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

  Token* token_info;
  
  // Allow regular expression in this context.
  Token* maybe_regexp = scanner_->CheckRegularExpression(cur_token());
  if (maybe_regexp != nullptr) {
    token_info = maybe_regexp;
  } else {
    token_info = cur_token();
  }
  Token info = *token_info;
  
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
    case TokenKind::kIdentifier: {
      return parse_result = ParseIdentifierReference(yield);
    }
    case TokenKind::kThis: {
      // parse a this.
      Handle<ir::Node> this_view = New<ir::ThisView>();
      this_view->SetInformationForNode(token_info);
      Next();
      return parse_result = Success(this_view);
    }
    case TokenKind::kLeftBrace:
      // parse an object literal.
      return parse_result = ParseObjectLiteral(yield);
    case TokenKind::kLeftBracket:
      // parse an array literal.
      return parse_result = ParseArrayInitializer(yield);
    case TokenKind::kLeftParen: {
      RecordedParserState rps = parser_state();
      Next();
      if (cur_token()->type() == TokenKind::kFor) {
        RestoreParserState(rps);
        return parse_result = ParseGeneratorComprehension(yield);
      } else {
        auto node = parse_result = ParseExpression(true, false);
        CHECK_AST(node);
        if (cur_token()->type() == TokenKind::kRightParen) {
          Next();
          return parse_result = node;
        }
      }
      SYNTAX_ERROR_AND("')' expected", cur_token(), return parse_result = ParseResult());
    }
    case TokenKind::kRegularExpr: {
      return parse_result = ParseRegularExpression();
    }
    case TokenKind::kTemplateLiteral: {
      return parse_result = ParseTemplateLiteral();
    }
    case TokenKind::kFunction:
      return parse_result = ParseFunctionOverloadOrImplementation(ir::Node::Null(), yield, false, false);
    case TokenKind::kClass:
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
  array_literal->SetInformationForNode(cur_token());
  Next();

  if (cur_token()->type() == TokenKind::kRightBracket) {
    Next();
    return Success(array_literal);
  }

  bool success = true;
    
  while (1) {
    ParseResult expr_result;
    bool spread = false;
    if (cur_token()->type() == TokenKind::kComma) {
      expr_result = Success(New<ir::UndefinedView>());
      expr_result.value()->SetInformationForNode(cur_token());
      Next();
    } else if (cur_token()->type() == TokenKind::kRest) {
      Next();
      expr_result = ParseAssignmentExpression(true, yield);
      spread = true;
    } else {
      expr_result = ParseAssignmentExpression(true, yield);
    }
      
    CHECK_AST(expr_result);
    array_literal->InsertLast(expr_result.value());
      
    if (cur_token()->type() == TokenKind::kComma) {
      if (spread) {
        SYNTAX_ERROR("array spread element must be the end of the array element list", cur_token());
      }
      Next();
      if (cur_token()->type() == TokenKind::kRightBracket) {
        Next();
        break;
      }
    } else if (cur_token()->type() == TokenKind::kRightBracket) {
      Next();
      break;
    } else {
      SYNTAX_ERROR("unexpected token in 'array literal'", cur_token());
    }
  }
  return Success(array_literal);
}


template<typename UCharInputIterator>
ParseResult Parser<UCharInputIterator>::ParseArrayComprehension(bool yield) {
  LOG_PHASE(ParseArrayComprehension);
  Token info = *cur_token();
  Next();
  auto comprehension_result = ParseComprehension(false, yield);
  CHECK_AST(comprehension_result);
  if (cur_token()->type() == TokenKind::kRightBracket) {
    Next();
    auto arr = New<ir::ArrayLiteralView>();
    arr->SetInformationForNode(&info);
    arr->InsertLast(comprehension_result.value());
    return Success(arr);
  }
  SYNTAX_ERROR("']' expected", cur_token());
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
}


template<typename UCharInputIterator>
ParseResult Parser<UCharInputIterator>::ParseComprehensionTail(bool yield) {
  LOG_PHASE(ParseComprehensionTail);
  if (cur_token()->type() == TokenKind::kFor) {
    
    auto comp_for_result = ParseComprehensionFor(yield);
    CHECK_AST(comp_for_result);
    Handle<ir::ForStatementView> stmt(comp_for_result.value());
    auto comp_tail_result = ParseComprehensionTail(yield);
    CHECK_AST(comp_tail_result);
    stmt->set_body(comp_tail_result.value());
    return Success(stmt);
    
  } else if (cur_token()->type() == TokenKind::kIf) {
    
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
  
  Token info = *cur_token();
  Next();
  if (cur_token()->type() == TokenKind::kLeftParen) {
    Next();
    auto for_bindig_result = ParseForBinding(yield);
    CHECK_AST(for_bindig_result);
    if (cur_token()->type() == TokenKind::kIdentifier &&
        cur_token()->value()->Equals("of")) {
      Next();
      auto assignment_expr_result = ParseAssignmentExpression(true, yield);
      CHECK_AST(assignment_expr_result);
      if (cur_token()->type() == TokenKind::kRightParen) {
        Next();
        auto for_expr = New<ir::ForOfStatementView>();
        for_expr->set_property_name(for_bindig_result.value());
        for_expr->set_expr(assignment_expr_result.value());
        for_expr->SetInformationForNode(&info);
        return Success(for_expr);
      }
      SYNTAX_ERROR("')' expected", cur_token());
    }
    SYNTAX_ERROR("'of' expected", cur_token());
  }
  SYNTAX_ERROR("'(' expected", cur_token());
}


template<typename UCharInputIterator>
ParseResult Parser<UCharInputIterator>::ParseComprehensionIf(bool yield) {
  LOG_PHASE(ParseComprehensionIf);

  Token info = *cur_token();
  Next();
  if (cur_token()->type() == TokenKind::kLeftParen) {
    Next();
    auto assignment_expr_result = ParseAssignmentExpression(true, yield);
    CHECK_AST(assignment_expr_result);
    if (cur_token()->type() == TokenKind::kRightParen) {
      Next();
      auto if_expr = New<ir::IfStatementView>();
      if_expr->set_expr(assignment_expr_result.value());
      if_expr->SetInformationForNode(&info);
      return Success(if_expr);
    }
    SYNTAX_ERROR("')' expected", cur_token());
  }
  SYNTAX_ERROR("'(' expected", cur_token());
}


template<typename UCharInputIterator>
ParseResult Parser<UCharInputIterator>::ParseGeneratorComprehension(bool yield) {
  LOG_PHASE(ParseGeneratorComprehension);

  Next();
  bool success = true;
  auto comp_result = ParseComprehension(true, yield);
  SKIP_TOKEN_IF(comp_result, success, TokenKind::kRightParen);
  if (cur_token()->type() == TokenKind::kRightParen) {
    return Success(comp_result.value());
  }
  SYNTAX_ERROR("')' expected", cur_token());
}


template<typename UCharInputIterator>
ParseResult Parser<UCharInputIterator>::ParseYieldExpression(bool in) {
  LOG_PHASE(ParseYieldExpression);

  Token info = *cur_token();
  Next();

  bool end = cur_token()->type() == TokenKind::kLineTerminator;
    
  if (info.has_line_break_before_next() ||
      info.has_line_terminator_before_next() ||
      end) {

    auto yield_expr = New<ir::YieldView>(false, ir::Node::Null());
    yield_expr->SetInformationForNode(cur_token());
      
    if (end) {
      Next();
    }
      
    return Success(yield_expr);
  }
    
  bool continuation = false;
  Next();
    
  if (cur_token()->type() == TokenKind::kMul) {
    Next();
    continuation = true;
  }

  auto assignment_expr_result = ParseAssignmentExpression(in, true);
  CHECK_AST(assignment_expr_result);
  auto yield_expr = New<ir::YieldView>(continuation, assignment_expr_result.value());
  yield_expr->SetInformationForNode(cur_token());
  return Success(yield_expr);
}


template<typename UCharInputIterator>
ParseResult Parser<UCharInputIterator>::ParseForBinding(bool yield) {
  LOG_PHASE(ParseForBinding);
  switch (cur_token()->type()) {
    case TokenKind::kLeftBrace: // FALL THROUGH
    case TokenKind::kLeftBracket:
      return ParseBindingPattern(yield, false);
    default:
      return ParseBindingIdentifier(false, yield);
  }
}


template <typename UCharInputIterator>
ParseResult Parser<UCharInputIterator>::ParseObjectLiteral(bool yield) {
  LOG_PHASE(ParseObjectLiteral);

  Handle<ir::ObjectLiteralView> object_literal = New<ir::ObjectLiteralView>();
  object_literal->SetInformationForNode(cur_token());
  Handle<ir::Properties> prop = object_literal->properties();
  Next();

  if (cur_token()->type() == TokenKind::kRightBrace) {
    Next();
    return Success(object_literal);
  }

  bool success = true;
    
  while (1) {
    auto prop_definition_result = ParsePropertyDefinition(yield);
    SKIP_TOKEN_OR(prop_definition_result, success, TokenKind::kRightBrace) {
      object_literal->InsertLast(prop_definition_result.value());
      if (prop_definition_result.value()->first_child()->HasSymbol()) {
        prop->Declare(prop_definition_result.value()->first_child()->symbol(), prop_definition_result.value());
      }
    }

    if (cur_token()->type() == TokenKind::kComma) {
      Next();
      if (cur_token()->type() == TokenKind::kRightBrace) {
        Next();
        break;
      }
    } else if (cur_token()->type() == TokenKind::kRightBrace) {
      Next();
      break;
    } else {
      SYNTAX_ERROR("expected ',' or '}'", cur_token());
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
  Token info = *cur_token();
   
  if (cur_token()->type() == TokenKind::kIdentifier &&
      cur_token()->value()->Equals("get")) {
      getter = true;
      Next();
  } else if (cur_token()->type() == TokenKind::kIdentifier &&
             cur_token()->value()->Equals("set")) {
      setter = true;
      Next();
  }

    if (cur_token()->type() == TokenKind::kIdentifier) {
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

  if (cur_token()->type() == TokenKind::kMul) {
    generator = true;
    Next();
  }
  
  if (cur_token()->type() == TokenKind::kLeftParen) {
    auto call_sig_result = ParseCallSignature(yield, false);
    CHECK_AST(call_sig_result);
    if (cur_token()->type() == TokenKind::kLeftBrace) {
      auto function_body_result = ParseFunctionBody(yield || generator);
      CHECK_AST(function_body_result);
      value_result = Success(New<ir::FunctionView>(getter, setter, generator, New<ir::FunctionOverloadsView>(),
                                            ir::Node::Null(), call_sig_result.value(), function_body_result.value()));
    }
  } else if (generator) {
    SYNTAX_ERROR("'(' expected", cur_token());
  } else if (cur_token()->type() == TokenKind::kColon) {
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
  if (cur_token()->type() == TokenKind::kLeftBracket) {
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
  if (Token::IsKeyword(cur_token()->type())) {
    cur_token()->set_type(TokenKind::kIdentifier);
  }
  switch (cur_token()->type()) {
    case TokenKind::kIdentifier:
      return ParseIdentifier();
    case TokenKind::kStringLiteral:
      return ParseStringLiteral();
    case TokenKind::kNumericLiteral:
      return ParseNumericLiteral();
    default:
      if (Token::IsKeyword(cur_token()->type())) {
        cur_token()->set_type(TokenKind::kIdentifier);
        return ParseIdentifier();
      }
      SYNTAX_ERROR("identifier or string literal or numeric literal expected", cur_token());
  }
}


template <typename UCharInputIterator>
ParseResult Parser<UCharInputIterator>::ParseComputedPropertyName(bool yield) {
  LOG_PHASE(ParseComputedPropertyName);

  Next();
  auto assignment_expr_result = ParseAssignmentExpression(true, yield);
  CHECK_AST(assignment_expr_result);
  if (cur_token()->type() == TokenKind::kRightBracket) {
    Next();
    return assignment_expr_result;
  }
  SYNTAX_ERROR("']' expected", cur_token());
}


template <typename UCharInputIterator>
ParseResult Parser<UCharInputIterator>::ParseLiteral() {
  LOG_PHASE(ParseLiteral);
  switch (cur_token()->type()) {
    case TokenKind::kNull: {
      auto node = New<ir::NullView>();
      node->SetInformationForNode(cur_token());
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
  switch (cur_token()->type()) {
    case TokenKind::kTrue: // FALL THROUGH
    case TokenKind::kFalse:
      return ParseBooleanLiteral();
    case TokenKind::kNumericLiteral:
      return ParseNumericLiteral();
    case TokenKind::kStringLiteral:
      return ParseStringLiteral();
    case TokenKind::kUndefined:
      return ParseUndefinedLiteral();
    case TokenKind::kNan:
      return ParseNaNLiteral();
    default:
      SYNTAX_ERROR("boolean or numeric literal or string literal expected", cur_token());
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
      SYNTAX_ERROR("ArrayComprehension is not allowed except es6 mode.", cur_token());
    }
    array_literal_result = ParseArrayComprehension(yield);
    CHECK_AST(array_literal_result);
  }
  return array_literal_result;
}


template <typename UCharInputIterator>
ParseResult Parser<UCharInputIterator>::ParseIdentifierReference(bool yield) {
  LOG_PHASE(ParseIdentifierReference);
  if (cur_token()->type() == TokenKind::kIdentifier) {
    return ParseIdentifier();
  } else if (cur_token()->type() == TokenKind::kYield) {
    if (!yield) {
      SYNTAX_ERROR("'yield' not allowed here", cur_token());
    }
    auto node = New<ir::YieldView>(false, ir::Node::Null());
    node->SetInformationForNode(cur_token());
    return Success(node);
  }
  SYNTAX_ERROR("'identifier' or 'yield' expected", cur_token());
}


template <typename UCharInputIterator>
ParseResult Parser<UCharInputIterator>::ParseBindingIdentifier(bool default_allowed, bool yield) {
  LOG_PHASE(ParseBindingIdentifier);
  ParseResult parse_result;
  if (cur_token()->type() == TokenKind::kDefault) {
    if (!default_allowed) {
      SYNTAX_ERROR("'default' keyword not allowed here", cur_token());
    }
    parse_result = Success(New<ir::DefaultView>());
    Next();
  } else if (cur_token()->type() == TokenKind::kYield) {
    Next();
    auto identifier_result = ParseIdentifier();
    CHECK_AST(identifier_result);
    parse_result = Success(New<ir::YieldView>(false, identifier_result.value()));
    Next();
  } else if (cur_token()->type() == TokenKind::kIdentifier) {
    parse_result = ParseIdentifier();
    CHECK_AST(parse_result);
  } else {
    SYNTAX_ERROR("'default', 'yield' or 'identifier' expected", cur_token());
  }

  parse_result.value()->SetInformationForNode(cur_token());

  return parse_result;
}

template <typename UCharInputIterator>
ParseResult Parser<UCharInputIterator>::ParseLabelIdentifier(bool yield) {
  LOG_PHASE(ParseLabelIdentifier);
  if (cur_token()->type() == TokenKind::kYield && yield) {
    SYNTAX_ERROR("yield not allowed here", cur_token());
  }
  if (cur_token()->type() == TokenKind::kYield) {
    auto node = New<ir::YieldView>(false, ir::Node::Null());
    node->SetInformationForNode(cur_token());
    return Success(node);
  }
  return ParseIdentifier();
}



template <typename UCharInputIterator>
ParseResult Parser<UCharInputIterator>::ParseIdentifier() {
  LOG_PHASE(ParseIdentifier);
  if (cur_token()->type() == TokenKind::kIdentifier) {
    auto node = New<ir::NameView>(NewSymbol(ir::SymbolType::kVariableName, cur_token()->value()));
    node->SetInformationForNode(cur_token());
    Next();
    return Success(node);
  }
  SYNTAX_ERROR("'identifier' expected", cur_token());
}


template <typename UCharInputIterator>
ParseResult Parser<UCharInputIterator>::ParseStringLiteral() {
  LOG_PHASE(ParseStringLiteral);
  if (cur_token()->type() == TokenKind::kStringLiteral) {
    auto string_literal = New<ir::StringView>(cur_token()->value());
    string_literal->SetInformationForNode(cur_token());
    Next();
    return Success(string_literal);
  }
  SYNTAX_ERROR("string literal expected", cur_token());
}


template <typename UCharInputIterator>
ParseResult Parser<UCharInputIterator>::ParseNumericLiteral() {
  LOG_PHASE(ParseNumericLiteral);
  if (cur_token()->type() == TokenKind::kNumericLiteral) {
    auto number = New<ir::NumberView>(cur_token()->value());
    number->SetInformationForNode(cur_token());
    Next();
    return Success(number);
  }
  SYNTAX_ERROR("numeric literal expected", cur_token());
}


template <typename UCharInputIterator>
ParseResult Parser<UCharInputIterator>::ParseBooleanLiteral() {
  LOG_PHASE(ParseBooleanLiteral);
  ParseResult parse_result;
  if (cur_token()->type() == TokenKind::kTrue) {
    parse_result = Success(New<ir::TrueView>());
  } else if (cur_token()->type() == TokenKind::kFalse) {
    parse_result = Success(New<ir::FalseView>());
  }

  if (parse_result) {
    parse_result.value()->SetInformationForNode(cur_token());
    Next();
    return parse_result;
  }
  SYNTAX_ERROR("boolean literal expected", cur_token());
}


template <typename UCharInputIterator>
ParseResult Parser<UCharInputIterator>::ParseUndefinedLiteral() {
  LOG_PHASE(ParseUndefinedLiteral);
  if (cur_token()->type() == TokenKind::kUndefined) {
    auto node = New<ir::UndefinedView>();
    node->SetInformationForNode(cur_token());
    Next();
    return Success(node);
  }
  SYNTAX_ERROR("'undefined' expected", cur_token());
}


template <typename UCharInputIterator>
ParseResult Parser<UCharInputIterator>::ParseNaNLiteral() {
  LOG_PHASE(ParseNaNLiteral);
  if (cur_token()->type() == TokenKind::kNan) {
    auto node = New<ir::NaNView>();
    node->SetInformationForNode(cur_token());
    Next();
    return Success(node);
  }
  SYNTAX_ERROR("'NaN' expected", cur_token());
}


template <typename UCharInputIterator>
ParseResult Parser<UCharInputIterator>::ParseRegularExpression() {
  LOG_PHASE(ParseRegularExpression);
  if (cur_token()->type() == TokenKind::kRegularExpr) {
    auto reg_expr = New<ir::RegularExprView>(cur_token()->value());
    reg_expr->SetInformationForNode(cur_token());
    Next();
    return Success(reg_expr);
  }
  SYNTAX_ERROR("regular expression expected", cur_token());
}


template <typename UCharInputIterator>
ParseResult Parser<UCharInputIterator>::ParseTemplateLiteral() {
  LOG_PHASE(ParseTemplateLiteral);
  if (cur_token()->type() == TokenKind::kTemplateLiteral) {
    auto template_literal = New<ir::TemplateLiteralView>(cur_token()->value());
    Next();
    template_literal->SetInformationForNode(cur_token());
    return Success(template_literal);
  }
  SYNTAX_ERROR("template literal expected", cur_token());
}


template <typename UCharInputIterator>
typename Parser<UCharInputIterator>::AccessorType Parser<UCharInputIterator>::ParseAccessor() {
  LOG_PHASE(ParseAccessor);
  
  bool getter = false;
  bool setter = false;

  Token info = *cur_token();
  RecordedParserState rps = parser_state();
  // Parse the getter or setter if inditifer is the get or set.
  if (cur_token()->type() == TokenKind::kIdentifier &&
      cur_token()->value()->Equals("get")) {
    getter = true;
    Next();
  } else if (cur_token()->type() == TokenKind::kIdentifier &&
             cur_token()->value()->Equals("set")) {
    setter = true;
    Next();
  }

  // If next token is left paren and getter or setter is true,
  // get or set keyword is not treated as the keyword,
  // so change current curosr position before the get or set.
  if ((getter || setter) &&
      (cur_token()->type() == TokenKind::kLeftParen ||
       cur_token()->type() == TokenKind::kLess)) {
    getter = setter = false;
    RestoreParserState(rps);
  }

  return AccessorType(setter, getter, info);
}
}
