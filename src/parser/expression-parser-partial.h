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
ParseResult Parser<UCharInputIterator>::ParseExpression() {
  LOG_PHASE(ParseExpression);
  auto assignment_expr_result = ParseAssignmentExpression();
  CHECK_AST(assignment_expr_result);
  
  auto assignment_expr = assignment_expr_result.value();

  if (!cur_token()->Is(TokenKind::kComma)) {
    return assignment_expr_result;
  }

  // Parse comma expressions.
  Next();
  ir::CommaExprView* comma_expr = New<ir::CommaExprView>({assignment_expr});
  comma_expr->SetInformationForNode(*cur_token());
  
  while (1) {
    assignment_expr_result = ParseAssignmentExpression();
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
ParseResult Parser<UCharInputIterator>::ParseAssignmentPattern() {
  LOG_PHASE(ParseAssignmentPattern);
  
  Token token = *cur_token();
  ParseResult result;
  
  switch(cur_token()->type()) {
    case TokenKind::kLeftBrace: {
      result = ParseObjectAssignmentPattern();
      break;
    }
    case TokenKind::kLeftBracket: {
      result = ParseArrayAssignmentPattern();
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
ParseResult Parser<UCharInputIterator>::ParseObjectAssignmentPattern() {
  LOG_PHASE(ParseObjectAssignmentPattern);

  // This method begin with kLeftBrace, so advance and consume token.
  Next();
  
  auto result = ParseAssignmentPropertyList();
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
ParseResult Parser<UCharInputIterator>::ParseArrayAssignmentPattern() {
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
      auto result = ParseAssignmentRestElement();
    
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
ParseResult Parser<UCharInputIterator>::ParseAssignmentPropertyList() {
  LOG_PHASE(ParseAssignmentPropertyList);
  auto prop_list = New<ir::BindingPropListView>();

  bool success = true;
  
  while (1) {
    auto result = ParseAssignmentProperty();

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
ParseResult Parser<UCharInputIterator>::ParseAssignmentProperty() {
  LOG_PHASE(ParseAssignmentProperty);
  Token info = *cur_token();
  
  ParseResult property_name;

  // Check whether property name is identifier reference or not.
  bool identifier = false;
  
  if (cur_token()->Is(TokenKind::kIdentifier)) {
    if (cur_token()->value()->Equals("yield") && state_.IsInGenerator()) {
      ReportParseError(cur_token(), YATSC_SOURCEINFO_ARGS)
        << "'yield' not allowed here.";
      cur_token()->set_type(TokenKind::kIdentifier);
    }
    identifier = true;
    property_name = ParseIdentifierReference();
  } else if (cur_token()->OneOf({TokenKind::kStringLiteral, TokenKind::kNumericLiteral, TokenKind::kLeftBracket})) {
    property_name = ParsePropertyName();
  }
  CHECK_AST(property_name);
  
  ParseResult init;

  // Initializer is only allowed, if property name is identifier reference.
  if (identifier && cur_token()->Is(TokenKind::kAssign)) {
    Next();
    init = ParseAssignmentExpression();
  } else if (cur_token()->Is(TokenKind::kColon)) {
    Next();
    init = ParseAssignmentElement();
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
ParseResult Parser<UCharInputIterator>::ParseAssignmentElement() {
  LOG_PHASE(ParseAssignmentElement);

  auto target = ParseDestructuringAssignmentTarget();
  CHECK_AST(target);
  
  ParseResult init;
  
  if (cur_token()->Is(TokenKind::kAssign)) {
    Next();
    init = ParseAssignmentExpression();
    CHECK_AST(init);
  }

  // All destructuring assignment element is convert to BindingElementView.
  auto binding_element = New<ir::BindingElementView>(target.value(), init.or(Null()));
  binding_element->SetInformationForNode(target.value());
  return Success(binding_element);
}


// AssignmentRestElement[Yield]
//   ... DestructuringAssignmentTarget[?Yield]
//
template <typename UCharInputIterator>
ParseResult Parser<UCharInputIterator>::ParseAssignmentRestElement() {
  LOG_PHASE(ParseAssignmentRestElement);

  Token info = *cur_token();
  Next();
  
  return ParseDestructuringAssignmentTarget() >>= [&](ir::Node* target) {
    auto rest = New<ir::RestParamView>(target);
    rest->SetInformationForNode(&info);
    return Success(rest);
  };
}


// DestructuringAssignmentTarget[Yield]
//   LeftHandSideExpression[?Yield]
//
template <typename UCharInputIterator>
ParseResult Parser<UCharInputIterator>::ParseDestructuringAssignmentTarget() {
  LOG_PHASE(ParseDestructuringAssignmentTarget);

  DisableErrorRecovery();
  YATSC_SCOPED([&]{EnableErrorRecovery();});
  
  RecordedParserState rps = parser_state();
  
  return TryParse([&]{return ParseLeftHandSideExpression();}) >>= [&](ir::Node* lhs) {  
    // Check whether DestructuringAssignmentTarget is IsValidAssignmentTarget or not.
    if (!lhs->IsValidLhs()) {
      if (lhs->HasObjectLiteralView() || lhs->HasArrayLiteralView()) {
        RestoreParserState(rps);
        EnableErrorRecovery();
        return ParseAssignmentPattern();
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


template <typename UCharInputIterator>
ParseResult Parser<UCharInputIterator>::ParseCoveredTypeExpression() {
  EnableNestedGenericTypeScanMode();
  YATSC_SCOPED([&]{DisableNestedGenericTypeScanMode();});

  auto type_arguments = New<ir::TypeArgumentsView>();
  
  Next();
    
  while (1) {
      
    TokenPack tokens = token_pack();
    ParseResult type;
      
    if (cur_token()->Is(TokenKind::kIdentifier)) {
      Next();
        
      if (cur_token()->Is(TokenKind::kExtends)) {
        RestoreTokens(tokens);
        type = ParseTypeParameter();
      } else {
        RestoreTokens(tokens);
        type = ParseTypeExpression();
      }
        
    } else {
      type = ParseTypeExpression();
    }
      
    CHECK_AST(type);
    type_arguments->InsertLast(type.value());

    if (cur_token()->Is(TokenKind::kComma)) {
      Next();
    } else if (cur_token()->Is(TokenKind::kGreater)) {
      Next();
      break;
    } else {
      ReportParseError(cur_token(), YATSC_SOURCEINFO_ARGS)
        << "unexpected token.";
      return Failed();
    }
  }

  return Success(type_arguments);
}


template <typename UCharInputIterator>
ParseResult Parser<UCharInputIterator>::ParseCoveredExpressionSuffix(bool invalid_arrow_param, bool has_types, ir::Node* type_arguments, const ir::Node::List& covered_expr_node_list) {
  switch (cur_token()->type()) {      
    case TokenKind::kArrowGlyph: {
      Next();
      return ParseAsArrowFunction(type_arguments, covered_expr_node_list, Null());
    }
        
    case TokenKind::kColon: {
      if (invalid_arrow_param) {
        if (has_types) {
          return ParseAsTypeAssertion(type_arguments, covered_expr_node_list);
        }
        return ParseAsExpression(covered_expr_node_list);
      }
        
      auto rps = parser_state();
      Next();
      auto ret_type_ret = ParseTypeExpression();
        
      if (!ret_type_ret) {
        RestoreParserState(rps);
        return ParseAsExpression(covered_expr_node_list);
      }
        
      if (cur_token()->Isnt(TokenKind::kArrowGlyph)) {
        ReportParseError(cur_token(), YATSC_SOURCEINFO_ARGS)
          << "=> expected.";
        return Failed();
      }
        
      Next();
      return ParseAsArrowFunction(type_arguments, covered_expr_node_list, ret_type_ret.value());
    }
        
    default: {
      if (has_types) {
        return ParseAsTypeAssertion(type_arguments, covered_expr_node_list);
      }
      return ParseAsExpression(covered_expr_node_list);
    }
  }
}


template <typename UCharInputIterator>
bool Parser<UCharInputIterator>::IsParsibleAsArrowFunctionFormalParameterList() {
  if (cur_token()->OneOf({TokenKind::kIdentifier, TokenKind::kLeftBrace, TokenKind::kLeftBracket})) {
    TokenPack tokens = token_pack();
    YATSC_SCOPED([&]{RestoreTokens(tokens);});
    Next();
        
    if (cur_token()->Is(TokenKind::kQuestionMark)) {
      Next();
      if (!cur_token()->OneOf({TokenKind::kColon, TokenKind::kComma, TokenKind::kRightParen, TokenKind::kAssign})) {
        return false;
      }
      return true;
    } else if (cur_token()->Is(TokenKind::kColon)) {
      return true;
    }
    return false;
  } else if (cur_token()->Is(TokenKind::kRest)) {
    return true;
  }

  return false;
}


template <typename UCharInputIterator>
ParseResult Parser<UCharInputIterator>::ParseCoveredExpression() {
  LOG_PHASE(ParseCoveredExpression);
  bool has_types = false;

  ParseResult type_arguments_result;
  ir::Node* type_arguments = nullptr;
  
  if (cur_token()->Is(TokenKind::kLess)) {
    type_arguments_result = ParseCoveredTypeExpression();
    CHECK_AST(type_arguments_result);
    type_arguments = type_arguments_result.value();
    has_types = true;
  } else {
    type_arguments = New<ir::TypeArgumentsView>();
  }

  ir::Node::List covered_expr_node_list;
  bool invalid_arrow_param = false;
  
  if (cur_token()->Is(TokenKind::kLeftParen)) {
    TokenPack tokens = token_pack();
    Next();

    if (cur_token()->Is(TokenKind::kRightParen)) {
      Token token = *cur_token();
      Next();
      if (cur_token()->OneOf({TokenKind::kColon, TokenKind::kArrowGlyph})) {
        return ParseCoveredExpressionSuffix(invalid_arrow_param, has_types, type_arguments, covered_expr_node_list);
      } else {
        ReportParseError(&token, YATSC_SOURCEINFO_ARGS)
          << "unexpected token.";
      }
    }

    if (cur_token()->Is(TokenKind::kFor)) {
      RestoreTokens(tokens);
      auto ret = ParseGeneratorComprehension();
      CHECK_AST(ret);
      
      if (has_types) {
        covered_expr_node_list.push_back(ret.value());
        return ParseAsTypeAssertion(type_arguments, covered_expr_node_list);
      }
      return ret;
    }
    
    while (1) {

      ParseResult node_ret;

      if (IsParsibleAsArrowFunctionFormalParameterList()) {
        if (cur_token()->Is(TokenKind::kRest)) {
          node_ret = ParseRestParameter(false);
        } else {
          node_ret = ParseParameter(false, false);
        }
        CHECK_AST(node_ret);
      } else {
        node_ret = ParseAssignmentExpression();
        CHECK_AST(node_ret);
        if (!node_ret.value()->HasNameView() &&
            !node_ret.value()->HasObjectLiteralView() &&
            !node_ret.value()->HasArrayLiteralView()) {
          invalid_arrow_param = true;
        }
      }
      
      covered_expr_node_list.push_back(node_ret.value());

      if (cur_token()->Is(TokenKind::kRightParen)) {
        Next();
        break;
      } else if (cur_token()->Is(TokenKind::kComma)) {
        Next();
      } else {
        ReportParseError(cur_token(), YATSC_SOURCEINFO_ARGS)
          << "unexpected token.";
        return Failed();
      }
    }

    return ParseCoveredExpressionSuffix(invalid_arrow_param, has_types, type_arguments, covered_expr_node_list);
  }

  if (has_types) {
    return ParseAsTypeAssertion(type_arguments, covered_expr_node_list);
  }

  return ParseConditionalExpression();
}


template <typename UCharInputIterator>
ParseResult Parser<UCharInputIterator>::ParseAsArrowFunction(ir::Node* type_list, const ir::Node::List& node_list, ir::Node* ret_type) {
  LOG_PHASE(ParseAsArrowFunction);

  auto params = New<ir::TypeParametersView>();
  for (auto x: type_list->node_list()) {
    if (!x->HasSimpleTypeExprView() &&
        !x->HasTypeConstraintsView() &&
        (x->HasSimpleTypeExprView() &&
         !x->ToSimpleTypeExprView()->type_name()->HasNameView())) {
      ReportParseError(x, YATSC_SOURCEINFO_ARGS)
        << "identifier expected.";
      return Failed();
    } else if (x->HasTypeConstraintsView()) {
      params->InsertLast(x);
    } else {
      params->InsertLast(x->first_child());
    }
  }

  ir::Node* param_list = New<ir::ParamList>();
  if (node_list.size() > 0) {
    if (node_list.size() > 1 && node_list[0]->HasRestParamView()) {
      ReportParseError(node_list[0], YATSC_SOURCEINFO_ARGS)
        << "Rest parameter must be at the end of the parameters";
    }
    param_list->SetInformationForNode(node_list[0]);
    for (auto x: node_list) {
      if (x->HasParameterView() || x->HasRestParamView()) {
        param_list->InsertLast(x);
      } else if (x->HasNameView() ||
          x->HasObjectLiteralView() ||
          x->HasArrayLiteralView()) {
        auto param_view = New<ir::ParameterView>(false, x, Null(), Null(), Null());
        param_view->SetInformationForNode(x);
        param_list->InsertLast(param_view);
      } else if (x->HasAssignmentView() && x->ToAssignmentView()->operand() == TokenKind::kAssign &&
                 (x->ToAssignmentView()->target()->HasNameView() ||
                  x->ToAssignmentView()->target()->HasObjectLiteralView() ||
                  x->ToAssignmentView()->target()->HasArrayLiteralView())) {
        auto param_view = New<ir::ParameterView>(false, x->ToAssignmentView()->target(), x->ToAssignmentView()->expr(), Null(), Null());
        param_view->SetInformationForNode(x);
        param_list->InsertLast(param_view);
      } else {
        ReportParseError(x, YATSC_SOURCEINFO_ARGS)
          << "invalid formal parameter list";
        return Failed();
      }
    }
  }

  auto call_sig = New<ir::CallSignatureView>(param_list, ret_type, params && params->size() > 0? params: nullptr);
  auto info = type_list->size() > 0? type_list: param_list->size() > 0? param_list: ret_type;
  if (info) {
    call_sig->SetInformationForNode(info);
  } else {
    call_sig->SetInformationForNode(cur_token());
  }
  return ParseConciseBody(call_sig);
}


template <typename UCharInputIterator>
ParseResult Parser<UCharInputIterator>::ParseAsTypeAssertion(ir::Node* type_list, const ir::Node::List& node_list) {
  LOG_PHASE(ParseAsTypeAssertion);

  for (auto x: type_list->node_list()) {
    if (x->HasTypeConstraintsView()) {
      ReportParseError(x, YATSC_SOURCEINFO_ARGS)
        << "invalid type assertion.";
      return Failed();
    }
  }

  auto is_valid_expr = [&](ir::Node* node) {
    if (node->HasParameterView()) {
      ir::ParameterView* p = node->ToParameterView();
      if (p->optional()) {
        ReportParseError(p, YATSC_SOURCEINFO_ARGS)
          << "unexpected token.";
        return false;
      } else if (p->value()) {
        ReportParseError(p, YATSC_SOURCEINFO_ARGS)
        << "unexpected token.";
        return false;
      }
    }
    return true;
  };


  auto build_cast_view = [&](ir::Node* node) {
    auto ret = New<ir::CastView>(type_list, node);
    ret->SetInformationForNode(type_list);
    return Success(ret);
  };
  
  
  if (node_list.size() > 1) {
    auto comma_expr = New<ir::CommaExprView>();
    comma_expr->SetInformationForNode(node_list[0]);
    for (auto x: node_list) {
      if (!is_valid_expr(x)) {
        ReportParseError(x, YATSC_SOURCEINFO_ARGS)
          << "unexpected token.";
        return Failed();
      }
      comma_expr->InsertLast(x);
    }
    return build_cast_view(comma_expr);
  } else if (node_list.size() == 1) {
    if (!is_valid_expr(node_list[0])) {
      ReportParseError(node_list[0], YATSC_SOURCEINFO_ARGS)
        << "unexpected token.";
      return Failed();
    }
    return build_cast_view(node_list[0]);
  }
  
  return ParseUnaryExpression() >>= [&](ir::Node* unary_expr) {
    return build_cast_view(unary_expr);
  };
}


template <typename UCharInputIterator>
ParseResult Parser<UCharInputIterator>::ParseAsExpression(const ir::Node::List& node_list) {
  LOG_PHASE(ParseAsExpression);
  if (node_list.size() > 1) {
    auto node = New<ir::CommaExprView>();
    node->SetInformationForNode(node_list[0]);
    for (auto x: node_list) {
      if (x->HasParameterView()) {
        ir::ParameterView* p = x->ToParameterView();
        if (p->optional()) {
          ReportParseError(p, YATSC_SOURCEINFO_ARGS)
            << "unexpected token.";
          return Failed();
        } else if (p->value()) {
          ReportParseError(p, YATSC_SOURCEINFO_ARGS)
            << "unexpected token.";
          return Failed();
        }
      }
      node->InsertLast(x);
    }
    return Success(node);
  } else if (node_list.size() == 1) {
    return Success(node_list[0]);
  }
  
  ReportParseError(cur_token(), YATSC_SOURCEINFO_ARGS)
    << "expression expected.";
  return Failed();
}


// AssignmentExpression[In, Yield]
//   ConditionalExpression[?In, ?Yield]
//   [+Yield] YieldExpression[?In]
//   ArrowFunction[?In, ?Yield]
//   LeftHandSideExpression[?Yield] = AssignmentExpression[?In,?Yield]
//   LeftHandSideExpression[?Yield] AssignmentOperator AssignmentExpression[?In, ?Yield]
//  
template <typename UCharInputIterator>
ParseResult Parser<UCharInputIterator>::ParseAssignmentExpression() {
  LOG_PHASE(ParseAssignmentExpression);
  ParseResult node;

  // Record current buffer position.
  RecordedParserState rps = parser_state();
  
  if (cur_token()->OneOf({TokenKind::kLeftParen, TokenKind::kLess})) {
    // DisableErrorRecovery();
    // // First try parse as arrow function.
    // // parsae an arrow_function_parameters.
    // auto arrow_param_result = TryParse([&]{return ParseArrowFunctionParameters(Null());});
    // EnableErrorRecovery();
    // // if node is exists, arrow function parameter parse is succeeded.
    // if (arrow_param_result) {
    //   return ParseConciseBody(arrow_param_result.value());
    // }

    // RestoreParserState(rps);
    //return ParseCoveredExpression();
  }

  ParseResult expr_result;
  bool parsed_as_assignment_pattern = false;

  if (cur_token()->Is(TokenKind::kIdentifier) && cur_token()->value()->Equals("yield") && state_.IsInGenerator()) {
    expr_result = ParseYieldExpression();
    CHECK_AST(expr_result);
  } else {  
    expr_result = TryParse([&]{return ParseConditionalExpression();});
    if (expr_result) {
      auto expr = expr_result.value();
      
      if (expr->HasNameView() &&
          cur_token()->Is(TokenKind::kArrowGlyph)) {
        return ParseArrowFunction(expr);
      }
    } else {
      if (!LanguageModeUtil::IsES6(compiler_option_)) {
        ReportParseError(cur_token(), YATSC_SOURCEINFO_ARGS)
          << "illegal token.";
        return Failed();
      }

      RestoreParserState(rps);
      expr_result = ParseAssignmentPattern();
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
      
      expr_result = ParseAssignmentPattern();
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
      return ParseAssignmentExpression() >>= [&](ir::Node* rhs_result) {
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
ParseResult Parser<UCharInputIterator>::ParseArrowFunction(ir::Node* identifier) {
  LOG_PHASE(ParseArrowFunction);
  return ParseArrowFunctionParameters(identifier) >>= [&](ir::Node* call_sig) {
    return ParseConciseBody(call_sig);
  };
}


template <typename UCharInputIterator>
ParseResult Parser<UCharInputIterator>::ParseArrowFunctionParameters(ir::Node* identifier) {
  LOG_PHASE(ParseArrowFunctionParameters);

  ParseResult call_sig_result;
  
  if (identifier) {
    call_sig_result = Success(New<ir::CallSignatureView>(identifier, ir::Node::Null(), ir::Node::Null()));
    call_sig_result.value()->SetInformationForNode(identifier);
  } else {
    call_sig_result = ParseCallSignature(false, true, false);
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
ParseResult Parser<UCharInputIterator>::ParseConciseBody(ir::Node* call_sig) {
  LOG_PHASE(ParseConciseBody);
  ParseResult concise_body_result;
  
  if (cur_token()->Is(TokenKind::kLeftBrace)) {
    concise_body_result = ParseFunctionBody(false);
  } else {
    concise_body_result = ParseAssignmentExpression();
  }
  
  return concise_body_result >>= [&](ir::Node* concise_body) {
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
ParseResult Parser<UCharInputIterator>::ParseConditionalExpression() {
  LOG_PHASE(ParseConditionalExpression);
  
  auto logical_or_expr_result = ParseLogicalORExpression();
  CHECK_AST(logical_or_expr_result);
  
  if (cur_token()->Is(TokenKind::kQuestionMark)) {
    Next();
    
    return ParseAssignmentExpression() >>= [&](ir::Node* left) {
      
      if (cur_token()->Is(TokenKind::kColon)) {
        Next();
        return ParseAssignmentExpression() >>= [&](ir::Node* right) {
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
  ParseResult Parser<UCharInputIterator>::name() {                      \
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
  PARSE_BINARY_EXPRESSION_INTERNAL((cur_token()->Is(TokenKind::token)), name, (next()))

#define PARSE_BINARY_EXPRESSION_WITH_COND(cond, name, next)         \
  PARSE_BINARY_EXPRESSION_INTERNAL((cond), name, (next()))

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
  (!state_.IsInNoInExpr() && cur_token()->Is(TokenKind::kIn))

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
PARSE_BINARY_EXPRESSION_WITH_CALL(MULTIPLICATIVE_COND, ParseMultiplicativeExpression, ParseUnaryExpression())
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
ParseResult Parser<UCharInputIterator>::ParseUnaryExpression() {
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
      return ParseUnaryExpression() >>= [&](ir::Node* unary_expr) {
        auto ret = New<ir::UnaryExprView>(type, unary_expr);
        ret->SetInformationForNode(unary_expr);
        return Success(ret);
      };
    }
    default:
      return ParsePostfixExpression();
  }
}


// PostfixExpression[Yield]
//   LeftHandSideExpression[?Yield]
//   LeftHandSideExpression[?Yield] [no LineTerminator here] ++
//   LeftHandSideExpression[?Yield] [no LineTerminator here] --
//
template <typename UCharInputIterator>
ParseResult Parser<UCharInputIterator>::ParsePostfixExpression() {
  LOG_PHASE(ParsePostfixExpression);
  return ParseLeftHandSideExpression() >>= [&](ir::Node* lhs_expr) {
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
ParseResult Parser<UCharInputIterator>::ParseLeftHandSideExpression() {
  LOG_PHASE(ParseLeftHandSideExpression);
  auto tokens = token_pack();
  if (cur_token()->Is(TokenKind::kNew)) {
    Next();
    if (cur_token()->Is(TokenKind::kNew)) {
      RestoreTokens(tokens);
      return ParseNewExpression();
    }
    RestoreTokens(tokens);
    return ParseCallExpression();
  }
  return ParseCallExpression();
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
ParseResult Parser<UCharInputIterator>::ParseCallExpression() {
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
    target = ParseMemberExpression();
    CHECK_AST(target);
  }
  
  
  if (cur_token()->OneOf({TokenKind::kLeftParen, TokenKind::kLess})) {
    
    auto arguments_result = ParseArguments();
    if (!arguments_result) {return target;}
    target.value()->MarkAsInValidLhs();
    ir::Node* call = New<ir::CallView>(target.value(), arguments_result.value());
    call->SetInformationForNode(target.value());
    
    while (1) {
      switch (cur_token()->type()) {
        case TokenKind::kLess:
        case TokenKind::kLeftParen: {
          arguments_result = ParseArguments();
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
          auto tmp = ParseGetPropOrElem(call, false, false);
          CHECK_AST(tmp);
          call = tmp.value();
          break;
        }
        default:
          return Success(call);
      }
    }
  } else if (cur_token()->Is(TokenKind::kTemplateLiteral)) {
    return ParseTemplateLiteral() >>= [&](ir::Node* template_literal) {
      ir::Node* call = New<ir::CallView>(target.value(), template_literal);
      call->SetInformationForNode(target.value());
      call->MarkAsInValidLhs();
      return Success(call);
    };
  }
  return target;
}


template <typename UCharInputIterator>
ParseResult Parser<UCharInputIterator>::BuildArguments(ParseResult type_arguments_result,
                                                       ir::Node* args) {
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
ParseResult Parser<UCharInputIterator>::ParseArguments() {
  LOG_PHASE(ParseArguments);

  ParseResult type_arguments_result;
  bool type_arguments = false;
  
  RecordedParserState rps = parser_state();
  if (cur_token()->Is(TokenKind::kLess)) {
    type_arguments_result = TryParse([&]{return ParseTypeArguments();});
    if (!type_arguments_result) {
      RestoreParserState(rps);
      return Failed();
    }
    type_arguments = true;
  }
  
  if (cur_token()->Is(TokenKind::kLeftParen)) {
    ir::CallArgsView* args = New<ir::CallArgsView>();
    args->SetInformationForNode(cur_token());
    Next();
    if (cur_token()->Is(TokenKind::kRightParen)) {
      Next();
      return BuildArguments(type_arguments_result, args);
    }
    
    bool has_rest = false;
    bool success = true;
    
    while (1) {
      if (cur_token()->Is(TokenKind::kRest)) {
        Token info = *cur_token();
        Next();
        auto assignment_expr_result = ParseAssignmentExpression();

        if (assignment_expr_result) {
          auto rest = New<ir::RestParamView>(assignment_expr_result.value());
          rest->SetInformationForNode(&info);
          args->InsertLast(rest);
          has_rest = true;
        } else {
          SKIP_IF_ERROR_RECOVERY_ENABLED(false, TokenKind::kRightParen, TokenKind::kComma);
        }
      } else {
        
        auto assignment_expr_result = ParseAssignmentExpression();

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
        return BuildArguments(type_arguments_result, args);
      }
      ReportParseError(cur_token(), YATSC_SOURCEINFO_ARGS)
        << "unexpected token in 'arguments'";
      return Failed();
    }
  } else if (type_arguments) {
    RestoreParserState(rps);
    return Failed();
  }
  
  ReportParseError(cur_token(), YATSC_SOURCEINFO_ARGS)
    << "'(' expected.";
  return Failed();
}


template <typename UCharInputIterator>
ParseResult Parser<UCharInputIterator>::ParseNewExpression() {
  LOG_PHASE(ParseNewExpression);
  auto tokens = token_pack();
  Next();
  if (cur_token()->Is(TokenKind::kNew)) {
    return ParseNewExpression() >>= [&](ir::Node* new_expr) {
      auto ret = New<ir::NewCallView>(new_expr, ir::Node::Null());
      ret->SetInformationForNode(tokens.current_token());
      return Success(ret);
    };
  }
  RestoreTokens(tokens);
  return ParseMemberExpression();
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
ParseResult Parser<UCharInputIterator>::ParseMemberExpression() {
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
      member = ParseMemberExpression();
      CHECK_AST(member);
    }

    
    // New expression can omit parens.
    // If paren exists, continue parsing.
    if (cur_token()->OneOf({TokenKind::kLeftParen, TokenKind::kLess})) {

      auto arguments_result = ParseArguments();
      if (!arguments_result) {
        return member;
      }
      
      auto ret = New<ir::NewCallView>(member.value(), arguments_result.value());
      ret->SetInformationForNode(member.value());
      ret->MarkAsInValidLhs();
      return ParseGetPropOrElem(ret, false, false);
      
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
    return ParseGetPropOrElem(super, false, false);
  } else {
    return ParsePrimaryExpression() >>= [&](ir::Node* primary_expr) {
      return ParseGetPropOrElem(primary_expr, false, false);
    };
  }
}


// Parse member expression suffix.
// Like 'new foo.bar.baz()', 'new foo["bar"]', '(function(){return {a:1}}).a'
template <typename UCharInputIterator>
ParseResult Parser<UCharInputIterator>::ParseGetPropOrElem(ir::Node* node, bool dot_only, bool is_throw) {
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
        auto expr_result = ParseExpression();
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
        
        auto primary_expr_result = ParsePrimaryExpression();
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
ParseResult Parser<UCharInputIterator>::ParsePrimaryExpression() {
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
      return parse_result = ParseIdentifierReference();
    }
    case TokenKind::kThis: {
      // parse a this.
      ir::Node* this_view = New<ir::ThisView>();
      this_view->SetInformationForNode(token_info);
      Next();
      return parse_result = Success(this_view);
    }
    case TokenKind::kLeftBrace:
      // parse an object literal.
      return parse_result = ParseObjectLiteral();
    case TokenKind::kLeftBracket:
      // parse an array literal.
      return parse_result = ParseArrayInitializer();
    case TokenKind::kLeftParen: {
      auto tokens = token_pack();
      Next();
      if (cur_token()->Is(TokenKind::kFor)) {
        RestoreTokens(tokens);
        return parse_result = ParseGeneratorComprehension();
      } else {
        RestoreTokens(tokens);
        return parse_result = ParseCoveredExpression();
      }
      ReportParseError(cur_token(), YATSC_SOURCEINFO_ARGS)
        << "')' expected";
      return parse_result = Failed();
    }
    case TokenKind::kLess: {
      return parse_result = ParseCoveredExpression();
    }
    case TokenKind::kRegularExpr: {
      return parse_result = ParseRegularExpression();
    }
    case TokenKind::kTemplateLiteral: {
      return parse_result = ParseTemplateLiteral();
    }
    case TokenKind::kFunction:
      return parse_result = ParseFunctionOverloadOrImplementation(ir::Node::Null(), false);
    case TokenKind::kClass:
      return parse_result = ParseClassDeclaration();
    default:
      // parse a literal.
      return parse_result = ParseLiteral();
  }
}



// Array literal parsing is started from after the left bracket.
// because the left bracket is comsumed by ParseArrayInitializer.
template<typename UCharInputIterator>
ParseResult Parser<UCharInputIterator>::ParseArrayLiteral() {
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
      expr_result = ParseAssignmentExpression();
      spread = true;
    } else {
      expr_result = ParseAssignmentExpression();
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
ParseResult Parser<UCharInputIterator>::ParseArrayComprehension() {
  LOG_PHASE(ParseArrayComprehension);
  
  return ParseComprehension(false) >>= [&](ir::Node* comprehension) {
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
ParseResult Parser<UCharInputIterator>::ParseComprehension(bool generator) {
  LOG_PHASE(ParseComprehension);
  return ParseComprehensionFor() >>= [&](ir::Node* comp_for) {
    return ParseComprehensionTail() >>= [&](ir::Node* comp_tail) {
      auto expr = New<ir::ComprehensionExprView>(generator, comp_for, comp_tail);
      expr->SetInformationForNode(comp_for);
      return Success(expr);
    };
  };
}


template<typename UCharInputIterator>
ParseResult Parser<UCharInputIterator>::ParseComprehensionTail() {
  LOG_PHASE(ParseComprehensionTail);
  if (cur_token()->Is(TokenKind::kFor)) {
    
    return ParseComprehensionFor() >>= [&](ir::Node* comp_for) {
      ir::ForStatementView* stmt = comp_for->ToForStatementView();
      return ParseComprehensionTail() >>= [&](ir::Node* comp_tail) {
        stmt->set_body(comp_tail);
        return Success(stmt);
      };
    };
    
  } else if (cur_token()->Is(TokenKind::kIf)) {    
    return ParseComprehensionIf() >>= [&](ir::Node* comp_if) {
      ir::IfStatementView* stmt = comp_if->ToIfStatementView();
      return ParseComprehensionTail() >>= [&](ir::Node* comp_tail) {
        stmt->set_then_block(comp_tail);
        return Success(stmt);
      };
    };
  }
  
  return ParseAssignmentExpression();
}


template<typename UCharInputIterator>
ParseResult Parser<UCharInputIterator>::ParseComprehensionFor() {
  LOG_PHASE(ParseComprehensionFor);
  
  Token info = *cur_token();
  Next();
  if (cur_token()->Is(TokenKind::kLeftParen)) {
    Next();
    
    return ParseForBinding() >>= [&](ir::Node* for_binding) {
      if (cur_token()->Is(TokenKind::kIdentifier) &&
          cur_token()->value()->Equals("of")) {
        Next();
        
        return ParseAssignmentExpression() >>= [&](ir::Node* assignment_expr) {
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
ParseResult Parser<UCharInputIterator>::ParseComprehensionIf() {
  LOG_PHASE(ParseComprehensionIf);

  Token info = *cur_token();
  Next();
  if (cur_token()->Is(TokenKind::kLeftParen)) {
    Next();

    return ParseAssignmentExpression() >>= [&](ir::Node* assignment_expr) {
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
ParseResult Parser<UCharInputIterator>::ParseGeneratorComprehension() {
  LOG_PHASE(ParseGeneratorComprehension);

  Next();
  bool success = true;
  auto comp_result = ParseComprehension(true);
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
ParseResult Parser<UCharInputIterator>::ParseYieldExpression() {
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

  return ParseAssignmentExpression() >>= [&](ir::Node* assignment_expr) {
    auto yield_expr = New<ir::YieldView>(continuation, assignment_expr);
    yield_expr->SetInformationForNode(cur_token());
    return Success(yield_expr);
  };
}


template<typename UCharInputIterator>
ParseResult Parser<UCharInputIterator>::ParseForBinding() {
  LOG_PHASE(ParseForBinding);
  switch (cur_token()->type()) {
    case TokenKind::kLeftBrace: // FALL THROUGH
    case TokenKind::kLeftBracket:
      return ParseBindingPattern();
    default:
      return ParseBindingIdentifier();
  }
}


template <typename UCharInputIterator>
ParseResult Parser<UCharInputIterator>::ParseObjectLiteral() {
  LOG_PHASE(ParseObjectLiteral);

  ir::ObjectLiteralView* object_literal = New<ir::ObjectLiteralView>();
  object_literal->SetInformationForNode(cur_token());
  Handle<ir::Properties> prop = object_literal->properties();
  Next();

  if (cur_token()->Is(TokenKind::kRightBrace)) {
    Next();
    return Success(object_literal);
  }

  bool success = true;
    
  while (1) {
    auto prop_definition_result = ParsePropertyDefinition();
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
ParseResult Parser<UCharInputIterator>::ParsePropertyDefinition() {
  LOG_PHASE(ParsePropertyDefinition);
  ParseResult value_result;
  ParseResult key_result;
  bool generator = false;
  bool getter = false;
  bool setter = false;
  Token info = *cur_token();

  if (cur_token()->IsKeyword()) {
    cur_token()->set_type(TokenKind::kIdentifier);
  }
   
  if (cur_token()->Is(TokenKind::kIdentifier) &&
      cur_token()->value()->Equals("get")) {
      getter = true;
      Next();
  } else if (cur_token()->Is(TokenKind::kIdentifier) &&
             cur_token()->value()->Equals("set")) {
      setter = true;
      Next();
  }

  if (cur_token()->IsKeyword()) {
    cur_token()->set_type(TokenKind::kIdentifier);
  }

  if (cur_token()->Is(TokenKind::kIdentifier)) {
    key_result = ParseIdentifierReference();
  } else {
    key_result = ParsePropertyName();
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
  
  if (cur_token()->OneOf({TokenKind::kLeftParen, TokenKind::kLess})) {
    auto call_sig_result = ParseCallSignature(false, true, false);
    CHECK_AST(call_sig_result);
    if (cur_token()->Is(TokenKind::kLeftBrace)) {
      auto function_body_result = ParseFunctionBody(generator);
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
    value_result = ParseAssignmentExpression();
    CHECK_AST(value_result);
  } else {
    ReportParseError(cur_token(), YATSC_SOURCEINFO_ARGS)
      << "':' expected.";
    return Failed();
  }
  ir::ObjectElementView* element = New<ir::ObjectElementView>(key_result.value(), value_result.or(Null()));
  element->SetInformationForNode(&info);
  return Success(element);
}


template <typename UCharInputIterator>
ParseResult Parser<UCharInputIterator>::ParsePropertyName() {
  LOG_PHASE(ParsePropertyName);
  if (cur_token()->Is(TokenKind::kLeftBracket)) {
    return ParseComputedPropertyName();
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
ParseResult Parser<UCharInputIterator>::ParseComputedPropertyName() {
  LOG_PHASE(ParseComputedPropertyName);

  Next();
  auto assignment_expr_result = ParseAssignmentExpression();
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
ParseResult Parser<UCharInputIterator>::ParseArrayInitializer() {
  LOG_PHASE(ParseArrayInitializer);

  // Consume '['.
  Next();

  Token info = *cur_token();
  if (cur_token()->Is(TokenKind::kFor)) {
    if (!LanguageModeUtil::IsES6(compiler_option_)) {
      ReportParseError(cur_token(), YATSC_SOURCEINFO_ARGS)
        << "ArrayComprehension is not allowed except es6 mode.";
    }
    return ParseArrayComprehension() >>= [&](ir::Node* comp) {
      comp->SetInformationForNode(&info);
      return Success(comp);
    };    
  }
  return ParseArrayLiteral() >>= [&](ir::Node* array_literal) {
    array_literal->SetInformationForNode(&info);
    return Success(array_literal);
  };
}


template <typename UCharInputIterator>
ParseResult Parser<UCharInputIterator>::ParseIdentifierReference() {
  LOG_PHASE(ParseIdentifierReference);

  if (cur_token()->Is(TokenKind::kIdentifier)) {
    if (cur_token()->value()->Equals("yield") && state_.IsInGenerator()) {
      ReportParseError(cur_token(), YATSC_SOURCEINFO_ARGS)
        << "'yield' not allowed here";
    }
    return ParseIdentifier();
  }
  
  ReportParseError(cur_token(), YATSC_SOURCEINFO_ARGS)
    << "'identifier' expected";
  return Failed();
}


template <typename UCharInputIterator>
ParseResult Parser<UCharInputIterator>::ParseLabelIdentifier() {
  LOG_PHASE(ParseLabelIdentifier);
  if (cur_token()->Is(TokenKind::kIdentifier) && cur_token()->value()->Equals("yield") &&
      state_.IsInGenerator()) {
    ReportParseError(cur_token(), YATSC_SOURCEINFO_ARGS)
      << "yield not allowed here";
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
  auto tokens = token_pack();
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
    RestoreTokens(tokens);
  }

  return AccessorType(setter, getter, info);
}
}
