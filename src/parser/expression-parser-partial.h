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
      ReportParseError(&token, YATSC_SOURCEINFO_ARGS)
        << "assignment pattern begin with '{' or '[' expected.";
      return Failed();
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

  ReportParseError(cur_token(), YATSC_SOURCEINFO_ARGS)
    << "'}' expected.";
  return Failed();
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
      if (!result) {
        SKIP_IF_ERROR_RECOVERY_ENABLED(false, TokenKind::kComma, TokenKind::kRightBracket, TokenKind::kRest);
        return Failed();
      }
    
      array_view->InsertLast(result.value());
      rest_token = Just(*cur_token());
      
    } else if (cur_token()->Is(TokenKind::kRightBracket)) {
      // Parse closed array initializer.
      // If array pattern element count is zero, it's treated as SyntaxError.
      if (array_view->size() == 0) {
        ReportParseError(cur_token(), YATSC_SOURCEINFO_ARGS)
          << "destructuring assignment left hand side is not allowed empty array";
        return Failed();
      }
    
      Next();
      return Success(array_view);
    }

    // The ParameterRest is not allowed in any position of array pattern except the last element.
    if (rest_token) {
      ReportParseError(rest_token.value(), YATSC_SOURCEINFO_ARGS)
        << "destructuring assignment rest expression must be the end of the elements.";
      return Failed();
    } else if (cur_token()->Is(TokenKind::kComma)) {
      Next();
      continue;
    }

    // If reached to this line, that mean token not machted with '[' or ',',
    // so produce error.
    ReportParseError(cur_token(), YATSC_SOURCEINFO_ARGS)
      << "unexpected token. ']' or ',' expected.";
    return Failed();
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

    if (result) {
      prop_list->InsertLast(result.value());
    } else {
      SKIP_IF_ERROR_RECOVERY_ENABLED(false, TokenKind::kComma, TokenKind::kRightBrace);
    }
    
    if (cur_token()->Is(TokenKind::kComma)) {
      Next();
      continue;
    } else if (cur_token()->Is(TokenKind::kRightBrace)) {
      return Success(prop_list);
    }

    ReportParseError(cur_token(), YATSC_SOURCEINFO_ARGS)
      << "',' or '}' expected.";
    return Failed();
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

  // Check whether property name is identifier reference or not.
  bool identifier = false;
  
  if (cur_token()->Is(TokenKind::kIdentifier)) {
    identifier = true;
    property_name = ParseIdentifierReference(yield);
  } else if (cur_token()->OneOf({TokenKind::kStringLiteral, TokenKind::kNumericLiteral, TokenKind::kLeftBracket})) {
    property_name = ParsePropertyName(yield, false);
  }
  CHECK_AST(property_name);
  
  ParseResult init;

  // Initializer is only allowed, if property name is identifier reference.
  if (identifier && cur_token()->Is(TokenKind::kAssign)) {
    Next();
    init = ParseAssignmentExpression(true, yield);
  } else if (cur_token()->Is(TokenKind::kColon)) {
    Next();
    init = ParseAssignmentElement(yield);
  }

  CHECK_AST(init);

  // All destructuring assignment element is convert to BindingElementView.
  auto binding_element = New<ir::BindingElementView>(property_name.value(), init.value());
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
  
  if (cur_token()->Is(TokenKind::kAssign)) {
    Next();
    init = ParseAssignmentExpression(true, yield);
    CHECK_AST(init);
  }

  // All destructuring assignment element is convert to BindingElementView.
  auto binding_element = New<ir::BindingElementView>(target.value(), init.value());
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
  
  return ParseDestructuringAssignmentTarget(yield) >>= [&](Handle<ir::Node> target) {
    auto rest = New<ir::RestParamView>(target);
    rest->SetInformationForNode(&info);
    return Success(rest);
  };
}


// DestructuringAssignmentTarget[Yield]
//   LeftHandSideExpression[?Yield]
//
template <typename UCharInputIterator>
ParseResult Parser<UCharInputIterator>::ParseDestructuringAssignmentTarget(bool yield) {
  LOG_PHASE(ParseDestructuringAssignmentTarget);

  DisableErrorRecovery();
  YATSC_SCOPED([&]{EnableErrorRecovery();});
  
  RecordedParserState rps = parser_state();
  
  return ParseLeftHandSideExpression(yield) >>= [&](Handle<ir::Node> lhs) {  
    // Check whether DestructuringAssignmentTarget is IsValidAssignmentTarget or not.
    if (!lhs->IsValidLhs()) {
      if (lhs->HasObjectLiteralView() || lhs->HasArrayLiteralView()) {
        RestoreParserState(rps);
        EnableErrorRecovery();
        return ParseAssignmentPattern(yield);
      }
      ReportParseError(cur_token(), YATSC_SOURCEINFO_ARGS)
        << "invalid Left-Hand-Side expression";
      return Failed();
    }
    return Success(lhs);
  };
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
  
  if (cur_token()->OneOf({TokenKind::kLeftParen, TokenKind::kLess})) {
    DisableErrorRecovery();
    // First try parse as arrow function.
    // parsae an arrow_function_parameters.
    auto arrow_param_result = ParseArrowFunctionParameters(yield, ir::Node::Null());
    EnableErrorRecovery();
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
      ReportParseError(cur_token(), YATSC_SOURCEINFO_ARGS)
        << "invalid use of 'yield' keyword";
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

  CHECK_AST(expr_result);
  
  // Expression is not an arrow_function.
  const Token *token = cur_token();
  auto type = token->type();

  // Check assignment operators.
  if (IsAssignmentOp(type)) {    
    if (!parsed_as_assignment_pattern &&
        (expr_result.value()->HasObjectLiteralView() ||
         expr_result.value()->HasArrayLiteralView())) {
      
      RestoreParserState(rps);

      if (!LanguageModeUtil::IsES6(compiler_option_)) {
        ReportParseError(cur_token(), YATSC_SOURCEINFO_ARGS)
          << "Invalid Left-Hand-Side expression";
      }
      
      expr_result = ParseAssignmentPattern(yield);
      CHECK_AST(expr_result);
    }

    if ((Token::IsLetOperator(type) &&
         !expr_result.value()->HasNameView() &&
         !expr_result.value()->HasGetPropView() &&
         !expr_result.value()->HasGetElemView())) {
      ReportParseError(cur_token(), YATSC_SOURCEINFO_ARGS) << "Invalid Left-Hand-Side expression";
      return Failed();
    }
    
    Next();

    // Check left hand side expression is valid as an expression.
    // If left hand side expression is like 'func()',
    // that is invalid expression.
    if (expr_result.value()->IsValidLhs()) {
      return ParseAssignmentExpression(in, yield) >>= [&](Handle<ir::Node> rhs_result) {
        auto result = New<ir::AssignmentView>(type, expr_result.value(), rhs_result);
        result->SetInformationForNode(expr_result.value());
        return Success(result);
      };
    }
    ReportParseError(cur_token(), YATSC_SOURCEINFO_ARGS)
      << "invalid left hand side expression in 'assignment expression'";
    return Failed();
  } else if (parsed_as_assignment_pattern) {
    ReportParseError(cur_token(), YATSC_SOURCEINFO_ARGS)
      << "destructuring assignment must be initialized";
    return Failed();
  }
  
  return expr_result;
}


template <typename UCharInputIterator>
ParseResult Parser<UCharInputIterator>::ParseArrowFunction(bool in, bool yield, Handle<ir::Node> identifier) {
  LOG_PHASE(ParseArrowFunction);
  return ParseArrowFunctionParameters(yield, identifier) >>= [&](Handle<ir::Node> call_sig) {
    return ParseConciseBody(in, call_sig);
  };
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
  
  if (!cur_token()->Is(TokenKind::kArrowGlyph)) {
    ReportParseError(cur_token(), YATSC_SOURCEINFO_ARGS)
      << "'=>' expected";
    return Failed();
  }
  
  Next();
  return call_sig_result;
}


template <typename UCharInputIterator>
ParseResult Parser<UCharInputIterator>::ParseConciseBody(bool in, Handle<ir::Node> call_sig) {
  LOG_PHASE(ParseConciseBody);
  ParseResult concise_body_result;
  
  if (cur_token()->Is(TokenKind::kLeftBrace)) {
    concise_body_result = ParseFunctionBody(false);
  } else {
    concise_body_result = ParseAssignmentExpression(true, false);
  }
  
  return concise_body_result >>= [&](Handle<ir::Node> concise_body) {
    auto ret = New<ir::ArrowFunctionView>(call_sig, concise_body);
    ret->SetInformationForNode(call_sig);
    return Success(ret);
  };
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
  
  if (cur_token()->Is(TokenKind::kQuestionMark)) {
    Next();
    
    return ParseAssignmentExpression(in, yield) >>= [&](Handle<ir::Node> left) {
      
      if (cur_token()->Is(TokenKind::kColon)) {
        Next();
        return ParseAssignmentExpression(in, yield) >>= [&](Handle<ir::Node> right) {
          auto temary = New<ir::TemaryExprView>(logical_or_expr_result.value(), left, right);
          temary->SetInformationForNode(logical_or_expr_result.value());
          temary->MarkAsInValidLhs();
          return Success(temary);
        };
      }
      ReportParseError(cur_token(), YATSC_SOURCEINFO_ARGS)
        << "unexpected token in 'temary expression'";
      return Failed();
    };
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
        if ((!ret.value()->IsValidLhs() && Token::IsLetOperator(cur_token()->type())) || \
            (Token::IsLetOperator(cur_token()->type()) &&               \
             !ret.value()->HasNameView() &&                             \
             !ret.value()->HasGetPropView() &&                          \
             !ret.value()->HasGetElemView())) {                         \
          ReportParseError(cur_token(), YATSC_SOURCEINFO_ARGS) << "invalid left handle side expression."; \
          return Failed();                                              \
        }                                                               \
        auto tmp = ret;                                                 \
        TokenKind type = cur_token()->type();                           \
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
  PARSE_BINARY_EXPRESSION_INTERNAL((cur_token()->Is(TokenKind::token)), name, (next(in, yield)))

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
PARSE_BINARY_EXPRESSION_WITH_COND((cur_token()->Is(TokenKind::kEq) || cur_token()->Is(TokenKind::kNotEq) || cur_token()->Is(TokenKind::kEqual) || cur_token()->Is(TokenKind::kNotEqual)), ParseEqualityExpression, ParseRelationalExpression)

#define RELATIONAL_COND                             \
  cur_token()->OneOf({TokenKind::kLess, TokenKind::kGreater, TokenKind::kLessEqual, TokenKind::kGreaterEqual, TokenKind::kInstanceof}) || \
  (in && cur_token()->Is(TokenKind::kIn))

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
  cur_token()->OneOf({TokenKind::kShiftLeft, TokenKind::kShiftRight, TokenKind::kUShiftRight})

// ShiftExpression[Yield]
//   AdditiveExpression[?Yield]
//   ShiftExpression[?Yield] << AdditiveExpression[?Yield]
//   ShiftExpression[?Yield] >> AdditiveExpression[?Yield]
//   ShiftExpression[?Yield] >>> AdditiveExpression[?Yield]
//
PARSE_BINARY_EXPRESSION_WITH_COND((SHIFT_COND), ParseShiftExpression, ParseAdditiveExpression)
#undef SHIFT_COND


#define ADDITIVE_COND                           \
  cur_token()->OneOf({TokenKind::kPlus, TokenKind::kMinus})

// AdditiveExpression[Yield]
//   MultiplicativeExpression[?Yield]
//   AdditiveExpression[?Yield] + MultiplicativeExpression[?Yield]
//   AdditiveExpression[?Yield] - MultiplicativeExpression[?Yield]
//
PARSE_BINARY_EXPRESSION_WITH_COND(ADDITIVE_COND, ParseAdditiveExpression, ParseMultiplicativeExpression)
#undef ADDITIVE_COND


#define MULTIPLICATIVE_COND                     \
    cur_token()->OneOf({TokenKind::kMul, TokenKind::kMod, TokenKind::kDiv})

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
      return ParseUnaryExpression(yield) >>= [&](Handle<ir::Node> unary_expr) {
        auto ret = New<ir::UnaryExprView>(type, unary_expr);
        ret->SetInformationForNode(unary_expr);
        return Success(ret);
      };
    }
    case TokenKind::kLess: {
      return ParseTypeArguments() >>= [&](Handle<ir::Node> type_arguments) {
        return ParseUnaryExpression(yield) >>= [&](Handle<ir::Node> unary_expr) {
          auto ret = New<ir::CastView>(type_arguments, unary_expr);
          ret->SetInformationForNode(type_arguments);
          return Success(ret);
        };
      };
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
  return ParseLeftHandSideExpression(yield) >>= [&](Handle<ir::Node> lhs_expr) {
    if (cur_token()->OneOf({TokenKind::kIncrement, TokenKind::kDecrement})) {
      auto ret = New<ir::PostfixView>(lhs_expr, cur_token()->type());
      ret->SetInformationForNode(lhs_expr);
      Next();
      return Success(ret);
    }
    return Success(lhs_expr);
  };
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
  if (cur_token()->Is(TokenKind::kNew)) {
    Next();
    if (cur_token()->Is(TokenKind::kNew)) {
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
  
  if (cur_token()->Is(TokenKind::kSuper)) {
    target = Success(New<ir::SuperView>());
    target.value()->SetInformationForNode(cur_token());
    Next();
    if (cur_token()->Is(TokenKind::kDot)) {
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
  
  
  if (cur_token()->OneOf({TokenKind::kLeftParen, TokenKind::kLess})) {
    
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
  } else if (cur_token()->Is(TokenKind::kTemplateLiteral)) {
    return ParseTemplateLiteral() >>= [&](Handle<ir::Node> template_literal) {
      Handle<ir::Node> call = New<ir::CallView>(target.value(), template_literal);
      call->SetInformationForNode(target.value());
      call->MarkAsInValidLhs();
      return Success(call);
    };
  }
  return target;
}


template <typename UCharInputIterator>
ParseResult Parser<UCharInputIterator>::BuildArguments(ParseResult type_arguments_result,
                                                       Handle<ir::Node> args,
                                                       bool) {
  auto arguments = New<ir::ArgumentsView>(type_arguments_result.or(ir::Node::Null()), args);
  if (type_arguments_result) {
    arguments->SetInformationForNode(type_arguments_result.or(ir::Node::Null()));
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
  if (cur_token()->Is(TokenKind::kLess)) {
    type_arguments_result = ParseTypeArguments();
    if (!type_arguments_result) {
      RestoreParserState(rps);
      return Failed();
    }
  }
  
  if (cur_token()->Is(TokenKind::kLeftParen)) {
    Handle<ir::CallArgsView> args = New<ir::CallArgsView>();
    args->SetInformationForNode(cur_token());
    Next();
    if (cur_token()->Is(TokenKind::kRightParen)) {
      Next();
      return BuildArguments(type_arguments_result, args, true);
    }
    
    bool has_rest = false;
    bool success = true;
    
    while (1) {

      if (cur_token()->Is(TokenKind::kRest)) {
        Token info = *cur_token();
        Next();
        auto assignment_expr_result = ParseAssignmentExpression(true, yield);

        if (assignment_expr_result) {
          auto rest = New<ir::RestParamView>(assignment_expr_result.value());
          rest->SetInformationForNode(&info);
          args->InsertLast(rest);
          has_rest = true;
        } else {
          SKIP_IF_ERROR_RECOVERY_ENABLED(false, TokenKind::kRightParen, TokenKind::kComma);
        }
      } else {
        
        auto assignment_expr_result = ParseAssignmentExpression(true, yield);

        if (assignment_expr_result) {
          args->InsertLast(assignment_expr_result.value()); 
        } else {
          SKIP_IF_ERROR_RECOVERY_ENABLED(false, TokenKind::kRightParen, TokenKind::kComma);
        }
      }
      
      if (cur_token()->Is(TokenKind::kComma)) {
        if (has_rest) {
          ReportParseError(cur_token(), YATSC_SOURCEINFO_ARGS)
            << "the spread argument must be the end of arguments";
          return Failed();
        }
        Next();
        continue;
      } else if (cur_token()->Is(TokenKind::kRightParen)) {
        Next();
        return BuildArguments(type_arguments_result, args, success);
      }
      ReportParseError(cur_token(), YATSC_SOURCEINFO_ARGS)
        << "unexpected token in 'arguments'";
      return Failed();
    }
  }
  ReportParseError(cur_token(), YATSC_SOURCEINFO_ARGS)
    << "'(' expected.";
  return Failed();
}


template <typename UCharInputIterator>
ParseResult Parser<UCharInputIterator>::ParseNewExpression(bool yield) {
  LOG_PHASE(ParseNewExpression);
  RecordedParserState rps = parser_state();
  Next();
  if (cur_token()->Is(TokenKind::kNew)) {
    return ParseNewExpression(yield) >>= [&](Handle<ir::Node> new_expr) {
      auto ret = New<ir::NewCallView>(new_expr, ir::Node::Null());
      ret->SetInformationForNode(&(rps.current()));
      return Success(ret);
    };
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
  if (token_info->Is(TokenKind::kNew)) {
    Token info = *cur_token();
    // Parse new Foo() expression.
    Next();
    ParseResult member;
    if (cur_token()->Is(TokenKind::kSuper)) {
      member = Success(New<ir::SuperView>());
      member.value()->SetInformationForNode(&info);
    } else {
      member = ParseMemberExpression(yield);
      CHECK_AST(member);
    }

    
    // New expression can omit parens.
    // If paren exists, continue parsing.
    if (cur_token()->OneOf({TokenKind::kLeftParen, TokenKind::kLess})) {

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
  } else if (token_info->Is(TokenKind::kSuper)) {
    auto super = New<ir::SuperView>();
    super->SetInformationForNode(token_info);
    return ParseGetPropOrElem(super, yield, false, false);
  } else {
    return ParsePrimaryExpression(yield) >>= [&](Handle<ir::Node> primary_expr) {
      return ParseGetPropOrElem(primary_expr, yield, false, false);
    };
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
            ReportParseError(cur_token(), YATSC_SOURCEINFO_ARGS)
              << "'.' expected.";
            return Failed();
          }
          return Success(node);
        }
        // [...] expression.
        Next();
        auto expr_result = ParseExpression(true, false);
        CHECK_AST(expr_result);
        node = New<ir::GetElemView>(node, expr_result.value());
        node->SetInformationForNode(node);
        
        if (cur_token()->Isnt(TokenKind::kRightBracket)) {
          ReportParseError(cur_token(), YATSC_SOURCEINFO_ARGS)
            << "']' expected";
        }
        Next();
        break;
      }
      case TokenKind::kDot: {
        // a.b.c expression.
        Next();

        if (Token::IsKeyword(cur_token()->type())) {
          cur_token()->set_type(TokenKind::kIdentifier);
        }
        
        if (cur_token()->Isnt(TokenKind::kIdentifier)) {
          ReportParseError(cur_token(), YATSC_SOURCEINFO_ARGS)
            << "'identifier' expected.";
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
  
  // Parsed* parsed = GetMemoizedRecord(token_info->source_position());
  // if (parsed != nullptr) {
  //   RestoreParserState(parsed->parser_state());
  //   return parsed->parse_result();
  // }


  // if (IsInRecordMode()) {
  //   YATSC_SCOPED([&] {
  //     Memoize(info.source_position(), parse_result);
  //   });
  // }
  
  
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
      if (cur_token()->Is(TokenKind::kFor)) {
        RestoreParserState(rps);
        return parse_result = ParseGeneratorComprehension(yield);
      } else {
        auto node = parse_result = ParseExpression(true, false);
        CHECK_AST(node);
        if (cur_token()->Is(TokenKind::kRightParen)) {
          Next();
          return parse_result = node;
        }
      }
      ReportParseError(cur_token(), YATSC_SOURCEINFO_ARGS)
        << "')' expected";
      return parse_result = Failed();
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



// Array literal parsing is started from after the left bracket.
// because the left bracket is comsumed by ParseArrayInitializer.
template<typename UCharInputIterator>
ParseResult Parser<UCharInputIterator>::ParseArrayLiteral(bool yield) {
  LOG_PHASE(ParseArrayLiteral);

  auto array_literal = New<ir::ArrayLiteralView>();

  if (cur_token()->Is(TokenKind::kRightBracket)) {
    Next();
    return Success(array_literal);
  }

  bool success = true;
    
  while (1) {
    ParseResult expr_result;
    bool spread = false;
    if (cur_token()->Is(TokenKind::kComma)) {
      expr_result = Success(New<ir::UndefinedView>());
      expr_result.value()->SetInformationForNode(cur_token());
      Next();
    } else if (cur_token()->Is(TokenKind::kRest)) {
      Next();
      expr_result = ParseAssignmentExpression(true, yield);
      spread = true;
    } else {
      expr_result = ParseAssignmentExpression(true, yield);
    }
      
    CHECK_AST(expr_result);
    array_literal->InsertLast(expr_result.value());
      
    if (cur_token()->Is(TokenKind::kComma)) {
      if (spread) {
        ReportParseError(cur_token(), YATSC_SOURCEINFO_ARGS)
          << "array spread element must be the end of the array element list";
        return Failed();
      }
      Next();
      if (cur_token()->Is(TokenKind::kRightBracket)) {
        Next();
        break;
      }
    } else if (cur_token()->Is(TokenKind::kRightBracket)) {
      Next();
      break;
    } else {
      ReportParseError(cur_token(), YATSC_SOURCEINFO_ARGS)
        << "unexpected token in 'array literal'";
      return Failed();
    }
  }
  return Success(array_literal);
}


// Array comprehension parsing is started from after the left bracket.
// because the left bracket is comsumed by ParseArrayInitializer.
template<typename UCharInputIterator>
ParseResult Parser<UCharInputIterator>::ParseArrayComprehension(bool yield) {
  LOG_PHASE(ParseArrayComprehension);
  
  return ParseComprehension(false, yield) >>= [&](Handle<ir::Node> comprehension) {
    if (cur_token()->Is(TokenKind::kRightBracket)) {
      Next();
      auto arr = New<ir::ArrayLiteralView>();
      arr->InsertLast(comprehension);
      return Success(arr);
    }
    ReportParseError(cur_token(), YATSC_SOURCEINFO_ARGS)
      << "']' expected";
    return Failed();
  };
}


template<typename UCharInputIterator>
ParseResult Parser<UCharInputIterator>::ParseComprehension(bool generator, bool yield) {
  LOG_PHASE(ParseComprehension);
  return ParseComprehensionFor(yield) >>= [&](Handle<ir::Node> comp_for) {
    return ParseComprehensionTail(yield) >>= [&](Handle<ir::Node> comp_tail) {
      auto expr = New<ir::ComprehensionExprView>(generator, comp_for, comp_tail);
      expr->SetInformationForNode(comp_for);
      return Success(expr);
    };
  };
}


template<typename UCharInputIterator>
ParseResult Parser<UCharInputIterator>::ParseComprehensionTail(bool yield) {
  LOG_PHASE(ParseComprehensionTail);
  if (cur_token()->Is(TokenKind::kFor)) {
    
    return ParseComprehensionFor(yield) >>= [&](Handle<ir::Node> comp_for) {
      Handle<ir::ForStatementView> stmt(comp_for);
      return ParseComprehensionTail(yield) >>= [&](Handle<ir::Node> comp_tail) {
        stmt->set_body(comp_tail);
        return Success(stmt);
      };
    };
    
  } else if (cur_token()->Is(TokenKind::kIf)) {
    
    return ParseComprehensionIf(yield) >>= [&](Handle<ir::Node> comp_if) {
      Handle<ir::IfStatementView> stmt(comp_if);
      return ParseComprehensionTail(yield) >>= [&](Handle<ir::Node> comp_tail) {
        stmt->set_then_block(comp_tail);
        return Success(stmt);
      };
    };
  }
  
  return ParseAssignmentExpression(true, yield);
}


template<typename UCharInputIterator>
ParseResult Parser<UCharInputIterator>::ParseComprehensionFor(bool yield) {
  LOG_PHASE(ParseComprehensionFor);
  
  Token info = *cur_token();
  Next();
  if (cur_token()->Is(TokenKind::kLeftParen)) {
    Next();
    
    return ParseForBinding(yield) >>= [&](Handle<ir::Node> for_binding) {
      if (cur_token()->Is(TokenKind::kIdentifier) &&
          cur_token()->value()->Equals("of")) {
        Next();
        
        return ParseAssignmentExpression(true, yield) >>= [&](Handle<ir::Node> assignment_expr) {
          if (cur_token()->Is(TokenKind::kRightParen)) {
            Next();
            auto for_expr = New<ir::ForOfStatementView>();
            for_expr->set_property_name(for_binding);
            for_expr->set_expr(assignment_expr);
            for_expr->SetInformationForNode(&info);
            return Success(for_expr);
          }
          ReportParseError(cur_token(), YATSC_SOURCEINFO_ARGS)
            << "')' expected";
          return Failed();
        };
        
      }
      ReportParseError(cur_token(), YATSC_SOURCEINFO_ARGS)
        << "'of' expected";
      return Failed();
    };
    
  }
  ReportParseError(cur_token(), YATSC_SOURCEINFO_ARGS)
    << "'(' expected";
  return Failed();
}


template<typename UCharInputIterator>
ParseResult Parser<UCharInputIterator>::ParseComprehensionIf(bool yield) {
  LOG_PHASE(ParseComprehensionIf);

  Token info = *cur_token();
  Next();
  if (cur_token()->Is(TokenKind::kLeftParen)) {
    Next();

    return ParseAssignmentExpression(true, yield) >>= [&](Handle<ir::Node> assignment_expr) {
      if (cur_token()->Is(TokenKind::kRightParen)) {
        Next();
        auto if_expr = New<ir::IfStatementView>();
        if_expr->set_expr(assignment_expr);
        if_expr->SetInformationForNode(&info);
        return Success(if_expr);
      }
      ReportParseError(cur_token(), YATSC_SOURCEINFO_ARGS)
        << "')' expected";
      return Failed();
    };
    
  }
  ReportParseError(cur_token(), YATSC_SOURCEINFO_ARGS)
    << "'(' expected";
  return Failed();
}


template<typename UCharInputIterator>
ParseResult Parser<UCharInputIterator>::ParseGeneratorComprehension(bool yield) {
  LOG_PHASE(ParseGeneratorComprehension);

  Next();
  bool success = true;
  auto comp_result = ParseComprehension(true, yield);
  if (comp_result) {
    if (cur_token()->Is(TokenKind::kRightParen)) {
      return Success(comp_result.value());
    }
  } else {
    SKIP_IF_ERROR_RECOVERY_ENABLED(false, TokenKind::kRightParen, TokenKind::kRightParen);
  }
  
  ReportParseError(cur_token(), YATSC_SOURCEINFO_ARGS)
    << "')' expected";
  return Failed();
}


template<typename UCharInputIterator>
ParseResult Parser<UCharInputIterator>::ParseYieldExpression(bool in) {
  LOG_PHASE(ParseYieldExpression);

  Token info = *cur_token();
  Next();

  bool end = cur_token()->Is(TokenKind::kLineTerminator);
    
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
    
  if (cur_token()->Is(TokenKind::kMul)) {
    Next();
    continuation = true;
  }

  return ParseAssignmentExpression(in, true) >>= [&](Handle<ir::Node> assignment_expr) {
    auto yield_expr = New<ir::YieldView>(continuation, assignment_expr);
    yield_expr->SetInformationForNode(cur_token());
    return Success(yield_expr);
  };
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

  if (cur_token()->Is(TokenKind::kRightBrace)) {
    Next();
    return Success(object_literal);
  }

  bool success = true;
    
  while (1) {
    auto prop_definition_result = ParsePropertyDefinition(yield);
    if (prop_definition_result) {
      object_literal->InsertLast(prop_definition_result.value());
      if (prop_definition_result.value()->first_child()->HasSymbol()) {
        prop->Declare(prop_definition_result.value()->first_child()->symbol(), prop_definition_result.value());
      }
    } else {
      SkipTokensUntil({TokenKind::kComma, TokenKind::kRightBrace}, false);
    }

    if (cur_token()->Is(TokenKind::kComma)) {
      Next();
      if (cur_token()->Is(TokenKind::kRightBrace)) {
        Next();
        break;
      }
    } else if (cur_token()->Is(TokenKind::kRightBrace)) {
      Next();
      break;
    } else {
      ReportParseError(cur_token(), YATSC_SOURCEINFO_ARGS)
        << "expected ',' or '}'";
      return Failed();
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
   
  if (cur_token()->Is(TokenKind::kIdentifier) &&
      cur_token()->value()->Equals("get")) {
      getter = true;
      Next();
  } else if (cur_token()->Is(TokenKind::kIdentifier) &&
             cur_token()->value()->Equals("set")) {
      setter = true;
      Next();
  }

  if (cur_token()->Is(TokenKind::kIdentifier)) {
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

  if (cur_token()->Is(TokenKind::kMul)) {
    generator = true;
    Next();
  }
  
  if (cur_token()->Is(TokenKind::kLeftParen)) {
    auto call_sig_result = ParseCallSignature(yield, false);
    CHECK_AST(call_sig_result);
    if (cur_token()->Is(TokenKind::kLeftBrace)) {
      auto function_body_result = ParseFunctionBody(yield || generator);
      CHECK_AST(function_body_result);
      value_result = Success(New<ir::FunctionView>(getter, setter, generator, New<ir::FunctionOverloadsView>(),
                                            ir::Node::Null(), call_sig_result.value(), function_body_result.value()));
    }
  } else if (generator) {
    ReportParseError(cur_token(), YATSC_SOURCEINFO_ARGS)
      << "'(' expected";
    return Failed();
  } else if (cur_token()->Is(TokenKind::kColon)) {
    Next();
    value_result = ParseAssignmentExpression(true, false);
    CHECK_AST(value_result);
  } else {
    ReportParseError(cur_token(), YATSC_SOURCEINFO_ARGS)
      << "':' expected.";
    return Failed();
  }
  Handle<ir::ObjectElementView> element = New<ir::ObjectElementView>(key_result.value(), value_result.or(Null()));
  element->SetInformationForNode(&info);
  return Success(element);
}


template <typename UCharInputIterator>
ParseResult Parser<UCharInputIterator>::ParsePropertyName(bool yield, bool generator_parameter) {
  LOG_PHASE(ParsePropertyName);
  if (cur_token()->Is(TokenKind::kLeftBracket)) {
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
      ReportParseError(cur_token(), YATSC_SOURCEINFO_ARGS)
        << "identifier or string literal or numeric literal expected";
      return Failed();
  }
}


template <typename UCharInputIterator>
ParseResult Parser<UCharInputIterator>::ParseComputedPropertyName(bool yield) {
  LOG_PHASE(ParseComputedPropertyName);

  Next();
  auto assignment_expr_result = ParseAssignmentExpression(true, yield);
  CHECK_AST(assignment_expr_result);
  if (cur_token()->Is(TokenKind::kRightBracket)) {
    Next();
    return assignment_expr_result;
  }
  
  ReportParseError(cur_token(), YATSC_SOURCEINFO_ARGS)
    << "']' expected";
  return Failed();
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
      ReportParseError(cur_token(), YATSC_SOURCEINFO_ARGS)
        << "boolean or numeric literal or string literal expected";
      return Failed();
  }
}


template <typename UCharInputIterator>
ParseResult Parser<UCharInputIterator>::ParseArrayInitializer(bool yield) {
  LOG_PHASE(ParseArrayInitializer);

  // Consume '['.
  Next();

  Token info = *cur_token();
  if (cur_token()->Is(TokenKind::kFor)) {
    if (!LanguageModeUtil::IsES6(compiler_option_)) {
      ReportParseError(cur_token(), YATSC_SOURCEINFO_ARGS)
        << "ArrayComprehension is not allowed except es6 mode.";
      return Failed();
    }
    return ParseArrayComprehension(yield) >>= [&](Handle<ir::Node> comp) {
      comp->SetInformationForNode(&info);
      return Success(comp);
    };    
  }
  return ParseArrayLiteral(yield) >>= [&](Handle<ir::Node> array_literal) {
    array_literal->SetInformationForNode(&info);
    return Success(array_literal);
  };
}


template <typename UCharInputIterator>
ParseResult Parser<UCharInputIterator>::ParseIdentifierReference(bool yield) {
  LOG_PHASE(ParseIdentifierReference);

  if (cur_token()->Is(TokenKind::kIdentifier)) {
    return ParseIdentifier();
  } else if (cur_token()->Is(TokenKind::kYield)) {
    if (!yield) {
      ReportParseError(cur_token(), YATSC_SOURCEINFO_ARGS)
        << "'yield' not allowed here";
      return Failed();
    }
    auto node = New<ir::YieldView>(false, ir::Node::Null());
    node->SetInformationForNode(cur_token());
    return Success(node);
  }
  
  ReportParseError(cur_token(), YATSC_SOURCEINFO_ARGS)
    << "'identifier' or 'yield' expected";
  return Failed();
}


template <typename UCharInputIterator>
ParseResult Parser<UCharInputIterator>::ParseBindingIdentifier(bool default_allowed, bool yield) {
  LOG_PHASE(ParseBindingIdentifier);
  ParseResult parse_result;
  if (cur_token()->Is(TokenKind::kDefault)) {
    if (!default_allowed) {
      ReportParseError(cur_token(), YATSC_SOURCEINFO_ARGS)
        << "'default' keyword not allowed here";
      return Failed();
    }
    parse_result = Success(New<ir::DefaultView>());
    Next();
  } else if (cur_token()->Is(TokenKind::kYield)) {
    Next();
    auto identifier_result = ParseIdentifier();
    CHECK_AST(identifier_result);
    parse_result = Success(New<ir::YieldView>(false, identifier_result.value()));
    Next();
  } else if (cur_token()->Is(TokenKind::kIdentifier)) {
    parse_result = ParseIdentifier();
    CHECK_AST(parse_result);
  } else {
    ReportParseError(cur_token(), YATSC_SOURCEINFO_ARGS)
      << "'default', 'yield' or 'identifier' expected";
    return Failed();
  }

  parse_result.value()->SetInformationForNode(cur_token());

  return parse_result;
}

template <typename UCharInputIterator>
ParseResult Parser<UCharInputIterator>::ParseLabelIdentifier(bool yield) {
  LOG_PHASE(ParseLabelIdentifier);
  if (cur_token()->Is(TokenKind::kYield) && yield) {
    ReportParseError(cur_token(), YATSC_SOURCEINFO_ARGS)
      << "yield not allowed here";
    return Failed();
  }
  if (cur_token()->Is(TokenKind::kYield)) {
    auto node = New<ir::YieldView>(false, ir::Node::Null());
    node->SetInformationForNode(cur_token());
    return Success(node);
  }
  return ParseIdentifier();
}



template <typename UCharInputIterator>
ParseResult Parser<UCharInputIterator>::ParseIdentifier() {
  LOG_PHASE(ParseIdentifier);
  if (cur_token()->Is(TokenKind::kIdentifier)) {
    auto node = New<ir::NameView>(NewSymbol(ir::SymbolType::kVariableName, cur_token()->value()));
    node->SetInformationForNode(cur_token());
    Next();
    return Success(node);
  }
  ReportParseError(cur_token(), YATSC_SOURCEINFO_ARGS)
    << "'identifier' expected";
  return Failed();
}


template <typename UCharInputIterator>
ParseResult Parser<UCharInputIterator>::ParseStringLiteral() {
  LOG_PHASE(ParseStringLiteral);
  if (cur_token()->Is(TokenKind::kStringLiteral)) {
    auto string_literal = New<ir::StringView>(cur_token()->value());
    string_literal->SetInformationForNode(cur_token());
    Next();
    return Success(string_literal);
  }
  ReportParseError(cur_token(), YATSC_SOURCEINFO_ARGS)
    << "string literal expected";
  return Failed();
}


template <typename UCharInputIterator>
ParseResult Parser<UCharInputIterator>::ParseNumericLiteral() {
  LOG_PHASE(ParseNumericLiteral);
  if (cur_token()->Is(TokenKind::kNumericLiteral)) {
    auto number = New<ir::NumberView>(cur_token()->value());
    number->SetInformationForNode(cur_token());
    Next();
    return Success(number);
  }
  ReportParseError(cur_token(), YATSC_SOURCEINFO_ARGS)
    << "numeric literal expected";
  return Failed();
}


template <typename UCharInputIterator>
ParseResult Parser<UCharInputIterator>::ParseBooleanLiteral() {
  LOG_PHASE(ParseBooleanLiteral);
  ParseResult parse_result;
  if (cur_token()->Is(TokenKind::kTrue)) {
    parse_result = Success(New<ir::TrueView>());
  } else if (cur_token()->Is(TokenKind::kFalse)) {
    parse_result = Success(New<ir::FalseView>());
  }

  if (parse_result) {
    parse_result.value()->SetInformationForNode(cur_token());
    Next();
    return parse_result;
  }
  ReportParseError(cur_token(), YATSC_SOURCEINFO_ARGS)
    << "boolean literal expected";
  return Failed();
}


template <typename UCharInputIterator>
ParseResult Parser<UCharInputIterator>::ParseUndefinedLiteral() {
  LOG_PHASE(ParseUndefinedLiteral);
  if (cur_token()->Is(TokenKind::kUndefined)) {
    auto node = New<ir::UndefinedView>();
    node->SetInformationForNode(cur_token());
    Next();
    return Success(node);
  }
  ReportParseError(cur_token(), YATSC_SOURCEINFO_ARGS)
    << "'undefined' expected";
  return Failed();
}


template <typename UCharInputIterator>
ParseResult Parser<UCharInputIterator>::ParseNaNLiteral() {
  LOG_PHASE(ParseNaNLiteral);
  if (cur_token()->Is(TokenKind::kNan)) {
    auto node = New<ir::NaNView>();
    node->SetInformationForNode(cur_token());
    Next();
    return Success(node);
  }
  ReportParseError(cur_token(), YATSC_SOURCEINFO_ARGS)
    << "'NaN' expected";
  return Failed();
}


template <typename UCharInputIterator>
ParseResult Parser<UCharInputIterator>::ParseRegularExpression() {
  LOG_PHASE(ParseRegularExpression);
  if (cur_token()->Is(TokenKind::kRegularExpr)) {
    auto reg_expr = New<ir::RegularExprView>(cur_token()->value());
    reg_expr->SetInformationForNode(cur_token());
    Next();
    return Success(reg_expr);
  }
  ReportParseError(cur_token(), YATSC_SOURCEINFO_ARGS)
    << "regular expression expected";
  return Failed();
}


template <typename UCharInputIterator>
ParseResult Parser<UCharInputIterator>::ParseTemplateLiteral() {
  LOG_PHASE(ParseTemplateLiteral);
  if (cur_token()->Is(TokenKind::kTemplateLiteral)) {
    auto template_literal = New<ir::TemplateLiteralView>(cur_token()->value());
    Next();
    template_literal->SetInformationForNode(cur_token());
    return Success(template_literal);
  }
  ReportParseError(cur_token(), YATSC_SOURCEINFO_ARGS)
    << "template literal expected";
  return Failed();
}


template <typename UCharInputIterator>
typename Parser<UCharInputIterator>::AccessorType Parser<UCharInputIterator>::ParseAccessor() {
  LOG_PHASE(ParseAccessor);
  
  bool getter = false;
  bool setter = false;

  Token info = *cur_token();
  RecordedParserState rps = parser_state();
  // Parse the getter or setter if inditifer is the get or set.
  if (cur_token()->Is(TokenKind::kIdentifier) &&
      cur_token()->value()->Equals("get")) {
    getter = true;
    Next();
  } else if (cur_token()->Is(TokenKind::kIdentifier) &&
             cur_token()->value()->Equals("set")) {
    setter = true;
    Next();
  }

  // If next token is left paren and getter or setter is true,
  // get or set keyword is not treated as the keyword,
  // so change current curosr position before the get or set.
  if ((getter || setter) &&
      (cur_token()->OneOf({TokenKind::kLeftParen, TokenKind::kLess}))) {
    getter = setter = false;
    RestoreParserState(rps);
  }

  return AccessorType(setter, getter, info);
}
}
