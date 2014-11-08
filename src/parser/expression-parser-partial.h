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
Handle<ir::Node> Parser<UCharInputIterator>::ParseExpression(bool in, bool yield) {
  LOG_PHASE(ParseExpression);
  Handle<ir::Node> assignment_expr = ParseAssignmentExpression(in, yield);
  CHECK_AST(assignment_expr);

  // Parse comma expressions.
  if (Current()->type() == Token::TS_COMMA) {
    Next();
    Handle<ir::CommaExprView> comma_expr = New<ir::CommaExprView>({assignment_expr});
    comma_expr->SetInformationForNode(*Current());
    
    while (1) {
      assignment_expr = ParseAssignmentExpression(in, yield);
      CHECK_AST(assignment_expr);
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
Handle<ir::Node> Parser<UCharInputIterator>::ParseAssignmentPattern(bool yield) {
  LOG_PHASE(ParseAssignmentPattern);
  TokenInfo info = *Current();
  Handle<ir::Node> node;
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
      SYNTAX_ERROR("unexpected token", (&info));
  }

  CHECK_AST(node);

  node->SetInformationForNode(&info);
  return node;
}


// ObjectAssignmentPattern[Yield]
//   { }
//   { AssignmentPropertyList[?Yield] }
//   { AssignmentPropertyList[?Yield] , }
//
template <typename UCharInputIterator>
Handle<ir::Node> Parser<UCharInputIterator>::ParseObjectAssignmentPattern(bool yield) {
  LOG_PHASE(ParseObjectAssignmentPattern);
  if (Current()->type() == Token::TS_LEFT_BRACE) {
    Next();
    Handle<ir::Node> node = ParseAssignmentPropertyList(yield);
    CHECK_AST(node);
    
    if (Current()->type() == Token::TS_COMMA) {
      Next();
    }
    if (Current()->type() == Token::TS_RIGHT_BRACE) {
      Next();
      return node;
    }
    SYNTAX_ERROR("unexpected token", Current());
  }
  SYNTAX_ERROR("'{' expected", Current());
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
Handle<ir::Node> Parser<UCharInputIterator>::ParseArrayAssignmentPattern(bool yield) {
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
      auto node = ParseAssignmentRestElement(yield);
      SKIP_TOKEN_IF_AND(node, Token::TS_RIGHT_BRACKET, return ir::Node::Null());
      array_view->InsertLast(node);
      has_rest = true;
    }

    if (Current()->type() == Token::TS_RIGHT_BRACKET) {
      if (array_view->size() == 0) {
        SYNTAX_ERROR("destructuring assignment left hand side is not allowed empty array", Current());
      }
      Next();
      return array_view;
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
        auto node = ParseAssignmentElement(yield);
        SKIP_TOKEN_OR(node, Token::TS_RIGHT_BRACKET) {
          array_view->InsertLast(node);
        }
      }
      switch (Current()->type()) {
        case Token::TS_COMMA:
          Next();
          break;
        case Token::TS_RIGHT_BRACKET:
          Next();
          return array_view;
        default:
          SYNTAX_ERROR("unexpected token", Current());
      }
    }
  }
  SYNTAX_ERROR("unexpected token", Current());
}


// Parse destructuring assignment object pattern properties.
// 
// AssignmentPropertyList[Yield]
//   AssignmentProperty[?Yield]
//   AssignmentPropertyList[?Yield] , AssignmentProperty[?Yield]
//
template <typename UCharInputIterator>
Handle<ir::Node> Parser<UCharInputIterator>::ParseAssignmentPropertyList(bool yield) {
  LOG_PHASE(ParseAssignmentPropertyList);
  auto prop_list = New<ir::BindingPropListView>();
  while (1) {
    auto node = ParseAssignmentProperty(yield);
    
    SKIP_TOKEN_OR(node, Token::TS_RIGHT_BRACE) {
      prop_list->InsertLast(node);
    }
    
    if (Current()->type() == Token::TS_COMMA) {
      Next();
      continue;
    } else if (Current()->type() == Token::TS_RIGHT_BRACE) {
      return prop_list;
    } else {
      SYNTAX_ERROR("',' or '}' expected.", Current());
    }
  }

  // UNREACHABLE
  return ir::Node::Null();
}


// Parse destructuring assignment object pattern property.
//
// AssignmentProperty[Yield] :
//   IdentifierReference[?Yield] Initializer[In,?Yield](opt)
//   PropertyName : AssignmentElement[?Yield]
//
template <typename UCharInputIterator>
Handle<ir::Node> Parser<UCharInputIterator>::ParseAssignmentProperty(bool yield) {
  LOG_PHASE(ParseAssignmentProperty);
  TokenInfo info = *Current();
  
  Handle<ir::Node> property_name;
  Handle<ir::Node> elem;

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
  
  Handle<ir::Node> init;

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
  auto binding_element = New<ir::BindingElementView>(property_name, elem, init);
  binding_element->SetInformationForNode(&info);
  return binding_element;
}


// AssignmentElement[Yield]
//   DestructuringAssignmentTarget[?Yield] Initializer[In,?Yield](opt)
//
template <typename UCharInputIterator>
Handle<ir::Node> Parser<UCharInputIterator>::ParseAssignmentElement(bool yield) {
  LOG_PHASE(ParseAssignmentElement);
  Handle<ir::Node> target = ParseDestructuringAssignmentTarget(yield);
  Handle<ir::Node> init;
  
  CHECK_AST(target);
  
  if (Current()->type() == Token::TS_ASSIGN) {
    Next();
    init = ParseAssignmentExpression(true, yield);
    CHECK_AST(init);
  }

  // All destructuring assignment element is convert to BindingElementView.
  auto binding_element = New<ir::BindingElementView>(ir::Node::Null(), target, init);
  binding_element->SetInformationForNode(target);
  return binding_element;
}


// AssignmentRestElement[Yield]
//   ... DestructuringAssignmentTarget[?Yield]
//
template <typename UCharInputIterator>
Handle<ir::Node> Parser<UCharInputIterator>::ParseAssignmentRestElement(bool yield) {
  LOG_PHASE(ParseAssignmentRestElement);
  if (Current()->type() == Token::TS_REST) {
    TokenInfo info = *Current();
    Next();
    Handle<ir::Node> target = ParseDestructuringAssignmentTarget(yield);
    CHECK_AST(target);
    auto rest = New<ir::RestParamView>(target);
    rest->SetInformationForNode(&info);
  }
  SYNTAX_ERROR("'...' expected", Current());
}


// DestructuringAssignmentTarget[Yield]
//   LeftHandSideExpression[?Yield]
//
template <typename UCharInputIterator>
Handle<ir::Node> Parser<UCharInputIterator>::ParseDestructuringAssignmentTarget(bool yield) {
  LOG_PHASE(ParseDestructuringAssignmentTarget);
  RecordedParserState rps = parser_state();
  Handle<ir::Node> ret = ParseLeftHandSideExpression(yield);
  CHECK_AST(ret);
  // Check whether DestructuringAssignmentTarget is IsValidAssignmentTarget or not.
  if (!ret->IsValidLhs()) {
    if (ret->HasObjectLiteralView() || ret->HasArrayLiteralView()) {
      RestoreParserState(rps);
      ret = ParseAssignmentPattern(yield);
      CHECK_AST(ret);
    } else {
      SYNTAX_ERROR("invalid Left-Hand-Side expression", ret);
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
Handle<ir::Node> Parser<UCharInputIterator>::ParseAssignmentExpression(bool in, bool yield) {
  LOG_PHASE(ParseAssignmentExpression);
  Handle<ir::Node> node;

  // Record current buffer position.
  RecordedParserState rps = parser_state();
  
  if (Current()->type() == Token::TS_LEFT_PAREN ||
      Current()->type() == Token::TS_LESS) {
    // First try parse as arrow function.
    bool failed = false;
    // parsae an arrow_function_parameters.
    Handle<ir::Node> node = ParseArrowFunctionParameters(yield, ir::Node::Null());
    // if node is exists, arrow function parameter parse is succeeded.
    if (node) {
      return ParseConciseBody(in, node);
    }

    RestoreParserState(rps);
  }

  Handle<ir::Node> expr;
  bool parsed_as_assignment_pattern = false;

  if (Current()->type() == Token::TS_YIELD) {
    if (!yield) {
      SYNTAX_ERROR("invalid use of 'yield' keyword", Current());
    }
    expr = ParseYieldExpression(in);
    CHECK_AST(expr);
  } else {  
    expr = ParseConditionalExpression(in, yield);
    if (expr) {
      if (expr->HasNameView() && Current()->type() == Token::TS_ARROW_GLYPH) {
        return ParseArrowFunction(in, yield, expr);
      }
    } else {
      if (!LanguageModeUtil::IsES6(compiler_option_)) {
        return ir::Node::Null();
      }
      RestoreParserState(rps);
      expr = ParseAssignmentPattern(yield);
      parsed_as_assignment_pattern = true;
    }
  }
  
  // Expression is not an arrow_function.
  const TokenInfo *token_info = Current();
  Token type = token_info->type();

  // Check assignment operators.
  if (IsAssignmentOp(type)) {
    if (!parsed_as_assignment_pattern &&
        (expr->HasObjectLiteralView() ||
         expr->HasArrayLiteralView())) {

      if (!LanguageModeUtil::IsES6(compiler_option_)) {
        SYNTAX_ERROR("Invalid Left-Hand-Side expression", expr);
      }
      
      RestoreParserState(rps);
      expr = ParseAssignmentPattern(yield);
      CHECK_AST(expr);
    }
    Next();

    // Check left hand side expression is valid as an expression.
    // If left hand side expression is like 'func()',
    // that is invalid expression.
    if (expr->IsValidLhs()) {
      Handle<ir::Node> rhs = ParseAssignmentExpression(in, yield);
      CHECK_AST(rhs);
      Handle<ir::Node> result = New<ir::AssignmentView>(type, expr, rhs);
      result->SetInformationForNode(expr);
      return result;
    }
    SYNTAX_ERROR("invalid left hand side expression in 'assignment expression'", Current());
  } else if (parsed_as_assignment_pattern) {
    SYNTAX_ERROR("destructuring assignment must be initialized", Current());
  }
  return expr;
}


template <typename UCharInputIterator>
Handle<ir::Node> Parser<UCharInputIterator>::ParseArrowFunction(bool in, bool yield, Handle<ir::Node> identifier) {
  LOG_PHASE(ParseArrowFunction);
  Handle<ir::Node> call_sig = ParseArrowFunctionParameters(yield, identifier);
  CHECK_AST(call_sig);
  return ParseConciseBody(in, call_sig);
}


template <typename UCharInputIterator>
Handle<ir::Node> Parser<UCharInputIterator>::ParseArrowFunctionParameters(bool yield, Handle<ir::Node> identifier) {
  LOG_PHASE(ParseArrowFunctionParameters);

  Handle<ir::Node> call_sig;
  
  if (identifier) {
    call_sig = New<ir::CallSignatureView>(identifier, ir::Node::Null(), ir::Node::Null());
    call_sig->SetInformationForNode(identifier);
  } else {  
    call_sig = ParseCallSignature(false, true);
    CHECK_AST(call_sig);
  }
  if (Current()->type() != Token::TS_ARROW_GLYPH) {
    SYNTAX_ERROR("'=>' expected", Current());
  }
  Next();
  return call_sig;
}


template <typename UCharInputIterator>
Handle<ir::Node> Parser<UCharInputIterator>::ParseConciseBody(bool in, Handle<ir::Node> call_sig) {
  LOG_PHASE(ParseConciseBody);
  Handle<ir::Node> body;
  if (Current()->type() == Token::TS_LEFT_BRACE) {
    body = ParseFunctionBody(false);
  } else {
    body = ParseAssignmentExpression(true, false);
  }
  CHECK_AST(body);
  Handle<ir::Node> ret = New<ir::ArrowFunctionView>(call_sig, body);
  ret->SetInformationForNode(call_sig);
  return ret;
}


// ConditionalExpression[In, Yield]
//   LogicalORExpression[?In, ?Yield]
//   LogicalORExpression[?In,?Yield] ? AssignmentExpression[In, ?Yield] : AssignmentExpression[?In, ?Yield]
//
template <typename UCharInputIterator>
Handle<ir::Node> Parser<UCharInputIterator>::ParseConditionalExpression(bool in, bool yield) {
  LOG_PHASE(ParseConditionalExpression);
  Handle<ir::Node> expr = ParseLogicalORExpression(in, yield);
  CHECK_AST(expr);
  
  if (Current()->type() == Token::TS_QUESTION_MARK) {
    Next();
    Handle<ir::Node> left = ParseAssignmentExpression(in, yield);
    CHECK_AST(left);
    
    if (Current()->type() == Token::TS_COLON) {
      Next();
      Handle<ir::Node> right = ParseAssignmentExpression(in, yield);
      CHECK_AST(right);      
      Handle<ir::TemaryExprView> temary = New<ir::TemaryExprView>(expr, left, right);
      temary->SetInformationForNode(expr);
      temary->MarkAsInValidLhs();
      return temary;
    }
    SYNTAX_ERROR("unexpected token in 'temary expression'", Current());
  }
  return expr;
}


#define PARSE_BINARY_EXPRESSION_INTERNAL(check, name, next)             \
  template <typename UCharInputIterator>                                \
  Handle<ir::Node> Parser<UCharInputIterator>::name(bool in, bool yield) { \
    LOG_PHASE(name);                                                    \
    Handle<ir::Node> ret = next;                                        \
    CHECK_AST(ret);                                                     \
    while (1) {                                                         \
      if (check) {                                                      \
        Handle<ir::Node> tmp = ret;                                     \
        Token type = Current()->type();                                 \
        Next();                                                         \
        ret = New<ir::BinaryExprView>(type, ret, next);                 \
        ret->SetInformationForNode(tmp);                                \
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
Handle<ir::Node> Parser<UCharInputIterator>::ParseUnaryExpression(bool yield) {
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
      Handle<ir::Node> node = ParseUnaryExpression(yield);
      CHECK_AST(node);
      Handle<ir::Node> ret = New<ir::UnaryExprView>(type, node);
      ret->SetInformationForNode(node);
      return ret;
    }
    case Token::TS_LESS: {
      Handle<ir::Node> type_arguments = ParseTypeArguments();
      Handle<ir::Node> expr = ParseUnaryExpression(yield);
      
      CHECK_AST(expr);
      CHECK_AST(type_arguments);
      
      Handle<ir::Node> ret = New<ir::CastView>(type_arguments, expr);
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
Handle<ir::Node> Parser<UCharInputIterator>::ParsePostfixExpression(bool yield) {
  LOG_PHASE(ParsePostfixExpression);
  Handle<ir::Node> node = ParseLeftHandSideExpression(yield);
  CHECK_AST(node);
  if (Current()->type() == Token::TS_INCREMENT ||
      Current()->type() == Token::TS_DECREMENT) {
    Handle<ir::Node> ret = New<ir::PostfixView>(node, Current()->type());
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
Handle<ir::Node> Parser<UCharInputIterator>::ParseLeftHandSideExpression(bool yield) {
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
Handle<ir::Node> Parser<UCharInputIterator>::ParseCallExpression(bool yield) {
  LOG_PHASE(ParseCallExpression);
  Handle<ir::Node> target;
  if (Current()->type() == Token::TS_SUPER) {
    target = New<ir::SuperView>();
    target->SetInformationForNode(Current());
    Next();
    if (Current()->type() == Token::TS_DOT) {
      Next();
      Handle<ir::Node> identifier = ParseIdentifier();
      CHECK_AST(identifier);
      target = New<ir::GetPropView>(target, identifier);
      target->SetInformationForNode(identifier);
    }
  } else {
    target = ParseMemberExpression(yield);
    CHECK_AST(target);
  }
  
  
  if (Current()->type() == Token::TS_LEFT_PAREN ||
      Current()->type() == Token::TS_LESS) {
    NodePair pair = ParseArguments(yield);
    if (!pair.first) {return target;}
    target->MarkAsInValidLhs();
    Handle<ir::Node> call = New<ir::CallView>(target, pair.first, pair.second);
    call->SetInformationForNode(target);
    if (!pair.first && !pair.second) {
      return call;
    }
    while (1) {
      switch (Current()->type()) {
        case Token::TS_LESS:
        case Token::TS_LEFT_PAREN: {
          NodePair pair = ParseArguments(yield);
          if (!pair.first) {
            return call;
          }
          call = New<ir::CallView>(call, pair.first, pair.second);
          call->MarkAsInValidLhs();
          call->SetInformationForNode(pair.first);
          break;
        }
        case Token::TS_LEFT_BRACKET:
        case Token::TS_DOT:
          call = ParseGetPropOrElem(call, yield, false, false);
          CHECK_AST(call);
          break;
        default:
          return call;
      }
    }
  } else if (Current()->type() == Token::TS_TEMPLATE_LITERAL) {
    Handle<ir::Node> template_literal = ParseTemplateLiteral();
    CHECK_AST(template_literal);
    Handle<ir::Node> call = New<ir::CallView>(target, template_literal, ir::Node::Null());
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
typename Parser<UCharInputIterator>::NodePair Parser<UCharInputIterator>::ParseArguments(bool yield) {
  LOG_PHASE(ParseArguments);

  Handle<ir::Node> type_arguments;
  RecordedParserState rps = parser_state();
  if (Current()->type() == Token::TS_LESS) {
    type_arguments = ParseTypeArguments();
    if (!type_arguments) {
      RestoreParserState(rps);
      return InvalidPair();
    }
  }
  
  if (Current()->type() == Token::TS_LEFT_PAREN) {
    Handle<ir::CallArgsView> args = New<ir::CallArgsView>();
    args->SetInformationForNode(Current());
    Next();
    if (Current()->type() == Token::TS_RIGHT_PAREN) {
      Next();
      return NodePair(args, type_arguments);
    }
    bool has_rest = false;
    while (1) {
      if (Current()->type() == Token::TS_REST) {
        TokenInfo info = *Current();
        Next();
        Handle<ir::Node> expr = ParseAssignmentExpression(true, yield);
        SKIP_TOKEN_OR(expr, Token::TS_RIGHT_PAREN) {
          auto rest = New<ir::RestParamView>(expr);
          rest->SetInformationForNode(&info);
          args->InsertLast(rest);
          has_rest = true;
        }
      } else {
        args->InsertLast(ParseAssignmentExpression(true, yield));
      }
      if (Current()->type() == Token::TS_COMMA) {
        if (has_rest) {
          SYNTAX_ERROR_AND("the spread argument must be the end of arguments",
                           Current(),
                           return InvalidPair());
        }
        Next();
        continue;
      } else if (Current()->type() == Token::TS_RIGHT_PAREN) {
        Next();
        return NodePair(args, type_arguments);
      }
      SYNTAX_ERROR_AND("unexpected token in 'arguments'", Current(), return InvalidPair());
    }
  }
  SYNTAX_ERROR_AND("'(' expected.", Current(), return InvalidPair());
}


template <typename UCharInputIterator>
Handle<ir::Node> Parser<UCharInputIterator>::ParseNewExpression(bool yield) {
  if (Current()->type() == Token::TS_NEW) {
    RecordedParserState rps = parser_state();
    Next();
    if (Current()->type() == Token::TS_NEW) {
      auto node = ParseNewExpression(yield);
      CHECK_AST(node);
      auto ret = New<ir::NewCallView>(node, ir::Node::Null(), ir::Node::Null());
      ret->SetInformationForNode(&(rps.current()));
      return ret;
    }
    RestoreParserState(rps);
    return ParseMemberExpression(yield); 
  }
  SYNTAX_ERROR("'new' expected.", Current());
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
Handle<ir::Node> Parser<UCharInputIterator>::ParseMemberExpression(bool yield) {
  LOG_PHASE(ParseMemberExpression);
  // Not advance scanner.
  TokenInfo* token_info = Current();
  Handle<ir::Node> node;
  if (token_info->type() ==  Token::TS_NEW) {
    TokenInfo info = *Current();
    // Parse new Foo() expression.
    Next();
    Handle<ir::Node> member;
    if (Current()->type() == Token::TS_SUPER) {
      member = New<ir::SuperView>();
      member->SetInformationForNode(&info);
    } else {
      member = ParseMemberExpression(yield);
      CHECK_AST(member);
    }

    
    // New expression can omit parens.
    // If paren exists, continue parsing.
    if (Current()->type() == Token::TS_LEFT_PAREN ||
        Current()->type() == Token::TS_LESS) {
      NodePair pair = ParseArguments(yield);
      if (!pair.first && !pair.second) {
        return ir::Node::Null();
      }
      node = New<ir::NewCallView>(member, pair.first, pair.second);
      node->SetInformationForNode(member);
      node->MarkAsInValidLhs();
      return ParseGetPropOrElem(node, yield, false, false);
    } else {
      // Parens are not exists.
      // Immediate return.
      Handle<ir::Node> ret = New<ir::NewCallView>(member, ir::Node::Null(), ir::Node::Null());
      ret->SetInformationForNode(member);
      return ret;
    }
  } else if (token_info->type() == Token::TS_SUPER) {
    auto super = New<ir::SuperView>();
    super->SetInformationForNode(token_info);
    return ParseGetPropOrElem(super, yield, false, false);
  } else {
    auto node = ParsePrimaryExpression(yield);
    CHECK_AST(node);
    return ParseGetPropOrElem(node, yield, false, false);
  }
}


// Parse member expression suffix.
// Like 'new foo.bar.baz()', 'new foo["bar"]', '(function(){return {a:1}}).a'
template <typename UCharInputIterator>
Handle<ir::Node> Parser<UCharInputIterator>::ParseGetPropOrElem(Handle<ir::Node> node, bool yield, bool dot_only, bool is_throw) {
  LOG_PHASE(ParseGetPropOrElem);
  while (1) {
    switch (Current()->type()) {
      case Token::TS_LEFT_BRACKET: {
        if (dot_only) {
          if (is_throw) {
            SYNTAX_ERROR("'.' expected.", Current());
          }
          return node;
        }
        // [...] expression.
        Next();
        Handle<ir::Node> expr = ParseExpression(true, false);
        CHECK_AST(expr);
        node = New<ir::GetElemView>(node, expr);
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
        Handle<ir::Node> expr = ParsePrimaryExpression(yield);
        CHECK_AST(expr);
        node = New<ir::GetPropView>(node, expr);
        node->SetInformationForNode(node);
        break;
      }
      default:
        return node;
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
Handle<ir::Node> Parser<UCharInputIterator>::ParsePrimaryExpression(bool yield) {
  LOG_PHASE(ParsePrimaryExpression);

  TokenInfo* token_info;
  
  // Allow regular expression in this context.
  TokenInfo* maybe_regexp = scanner_->CheckRegularExpression(Current());
  if (maybe_regexp != nullptr) {
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
      Handle<ir::Node> this_view = New<ir::ThisView>();
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
      RecordedParserState rps = parser_state();
      Next();
      if (Current()->type() == Token::TS_FOR) {
        RestoreParserState(rps);
        return ParseGeneratorComprehension(yield);
      } else {
        Handle<ir::Node> node = ParseExpression(true, false);
        CHECK_AST(node);
        if (Current()->type() == Token::TS_RIGHT_PAREN) {
          Next();
          return node;
        }
      }
      SYNTAX_ERROR("')' expected", Current());
    }
    case Token::TS_REGULAR_EXPR: {
      return ParseRegularExpression();
    }
    case Token::TS_TEMPLATE_LITERAL: {
      return ParseTemplateLiteral();
    }
    case Token::TS_FUNCTION:
      return ParseFunctionOverloadOrImplementation(ir::Node::Null(), yield, false, false);
    case Token::TS_CLASS:
      return ParseClassDeclaration(yield, false);
    default:
      // parse a literal.
      return ParseLiteral();
  }
}


template<typename UCharInputIterator>
Handle<ir::Node> Parser<UCharInputIterator>::ParseArrayLiteral(bool yield) {
  LOG_PHASE(ParseArrayLiteral);
  if (Current()->type() == Token::TS_LEFT_BRACKET) {
    auto array_literal = New<ir::ArrayLiteralView>();
    array_literal->SetInformationForNode(Current());
    Next();

    if (Current()->type() == Token::TS_RIGHT_BRACKET) {
      Next();
      return array_literal;
    }
    
    while (1) {
      Handle<ir::Node> expr;
      bool spread = false;
      if (Current()->type() == Token::TS_COMMA) {
        expr = New<ir::UndefinedView>();
        expr->SetInformationForNode(Current());
        Next();
      } else if (Current()->type() == Token::TS_REST) {
        expr = ParseSpreadElement(yield);
        SKIP_TOKEN_OR(expr, Token::TS_RIGHT_BRACKET) {
          spread = true;
        }
      } else {
        expr = ParseAssignmentExpression(true, yield);
        SKIP_TOKEN_IF(expr, Token::TS_RIGHT_BRACKET);
      }
      array_literal->InsertLast(expr);
      if (Current()->type() == Token::TS_COMMA) {
        if (spread) {
          SYNTAX_ERROR("array spread element must be the end of the array element list",
                              Current());
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
    return array_literal;
  }
  SYNTAX_ERROR("Unexpected token.", Current());
}


template<typename UCharInputIterator>
Handle<ir::Node> Parser<UCharInputIterator>::ParseSpreadElement(bool yield) {
  LOG_PHASE(ParseSpreadElement);
  if (Current()->type() == Token::TS_REST) {
    Next();
    return ParseAssignmentExpression(true, yield);
  }
  SYNTAX_ERROR("'...' expected", Current());
}


template<typename UCharInputIterator>
Handle<ir::Node> Parser<UCharInputIterator>::ParseArrayComprehension(bool yield) {
  LOG_PHASE(ParseArrayComprehension);
  if (Current()->type() == Token::TS_LEFT_BRACKET) {
    TokenInfo info = *Current();
    Next();
    Handle<ir::Node> expr = ParseComprehension(false, yield);
    CHECK_AST(expr);
    if (Current()->type() == Token::TS_RIGHT_BRACKET) {
      Next();
      auto arr = New<ir::ArrayLiteralView>();
      arr->SetInformationForNode(&info);
      arr->InsertLast(expr);
      return arr;
    }
    SYNTAX_ERROR("']' expected", Current());
  }
  SYNTAX_ERROR("'[' expected", Current());
}


template<typename UCharInputIterator>
Handle<ir::Node> Parser<UCharInputIterator>::ParseComprehension(bool generator, bool yield) {
  LOG_PHASE(ParseComprehension);
  Handle<ir::Node> comp_for = ParseComprehensionFor(yield);
  Handle<ir::Node> comp_tail = ParseComprehensionTail(yield);
  CHECK_AST(comp_for);
  CHECK_AST(comp_tail);
  auto expr = New<ir::ComprehensionExprView>(generator, comp_for, comp_tail);
  expr->SetInformationForNode(comp_for);
  return expr;
};


template<typename UCharInputIterator>
Handle<ir::Node> Parser<UCharInputIterator>::ParseComprehensionTail(bool yield) {
  LOG_PHASE(ParseComprehensionTail);
  if (Current()->type() == Token::TS_FOR) {
    auto node = ParseComprehensionFor(yield);
    CHECK_AST(node);
    Handle<ir::ForStatementView> stmt(node);
    auto tail = ParseComprehensionTail(yield);
    CHECK_AST(tail);
    stmt->set_body(tail);
    return stmt;
  } else if (Current()->type() == Token::TS_IF) {
    auto comp_if = ParseComprehensionIf(yield);
    CHECK_AST(comp_if);
    Handle<ir::IfStatementView> stmt(comp_if);
    auto comp_tail = ParseComprehensionTail(yield);
    CHECK_AST(comp_tail);
    stmt->set_then_block(comp_tail);
    return stmt;
  }
  return ParseAssignmentExpression(true, yield);
}


template<typename UCharInputIterator>
Handle<ir::Node> Parser<UCharInputIterator>::ParseComprehensionFor(bool yield) {
  LOG_PHASE(ParseComprehensionFor);
  if (Current()->type() == Token::TS_FOR) {
    TokenInfo info = *Current();
    Next();
    if (Current()->type() == Token::TS_LEFT_PAREN) {
      Next();
      Handle<ir::Node> for_bindig = ParseForBinding(yield);
      CHECK_AST(for_bindig);
      if (Current()->type() == Token::TS_IDENTIFIER &&
          Current()->value()->Equals("of")) {
        Next();
        Handle<ir::Node> expr = ParseAssignmentExpression(true, yield);
        CHECK_AST(expr);
        if (Current()->type() == Token::TS_RIGHT_PAREN) {
          Next();
          auto for_expr = New<ir::ForOfStatementView>();
          for_expr->set_property_name(for_bindig);
          for_expr->set_expr(expr);
          for_expr->SetInformationForNode(&info);
          return for_expr;
        }
        SYNTAX_ERROR("')' expected", Current());
      }
      SYNTAX_ERROR("'of' expected", Current());
    }
    SYNTAX_ERROR("'(' expected", Current());
  }
  SYNTAX_ERROR("'for' expected", Current());
}


template<typename UCharInputIterator>
Handle<ir::Node> Parser<UCharInputIterator>::ParseComprehensionIf(bool yield) {
  LOG_PHASE(ParseComprehensionIf);
  if (Current()->type() == Token::TS_IF) {
    TokenInfo info = *Current();
    Next();
    if (Current()->type() == Token::TS_LEFT_PAREN) {
      Next();
      Handle<ir::Node> expr = ParseAssignmentExpression(true, yield);
      CHECK_AST(expr);
      if (Current()->type() == Token::TS_RIGHT_PAREN) {
        Next();
        auto if_expr = New<ir::IfStatementView>();
        if_expr->set_expr(expr);
        if_expr->SetInformationForNode(&info);
        return if_expr;
      }
      SYNTAX_ERROR("')' expected", Current());
    }
    SYNTAX_ERROR("'(' expected", Current());
  }
  SYNTAX_ERROR("'if' expected", Current());
}


template<typename UCharInputIterator>
Handle<ir::Node> Parser<UCharInputIterator>::ParseGeneratorComprehension(bool yield) {
  LOG_PHASE(ParseGeneratorComprehension);
  if (Current()->type() == Token::TS_LEFT_PAREN) {
    Next();
    Handle<ir::Node> comp = ParseComprehension(true, yield);
    CHECK_AST(comp);
    if (Current()->type() == Token::TS_RIGHT_PAREN) {
      return comp;
    }
    SYNTAX_ERROR("')' expected", Current());
  }
  SYNTAX_ERROR("'(' expected", Current());
}


template<typename UCharInputIterator>
Handle<ir::Node> Parser<UCharInputIterator>::ParseYieldExpression(bool in) {
  LOG_PHASE(ParseYieldExpression);
  if (Current()->type() == Token::TS_YIELD) {
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
      
      return yield_expr;
    }
    
    bool continuation = false;
    Next();
    
    if (Current()->type() == Token::TS_MUL) {
      Next();
      continuation = true;
    }

    Handle<ir::Node> expr = ParseAssignmentExpression(in, true);
    CHECK_AST(expr);
    auto yield_expr = New<ir::YieldView>(continuation, expr);
    yield_expr->SetInformationForNode(Current());
    return yield_expr;
  }
  SYNTAX_ERROR("'yield' expected", Current());
}


template<typename UCharInputIterator>
Handle<ir::Node> Parser<UCharInputIterator>::ParseForBinding(bool yield) {
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
Handle<ir::Node> Parser<UCharInputIterator>::ParseObjectLiteral(bool yield) {
  LOG_PHASE(ParseObjectLiteral);
  if (Current()->type() == Token::TS_LEFT_BRACE) {
    Handle<ir::ObjectLiteralView> object_literal = New<ir::ObjectLiteralView>();
    object_literal->SetInformationForNode(Current());
    Handle<ir::Properties> prop = object_literal->properties();
    Next();

    if (Current()->type() == Token::TS_RIGHT_BRACE) {
      Next();
      return object_literal;
    }
    
    while (1) {
      Handle<ir::Node> element = ParsePropertyDefinition(yield);
      SKIP_TOKEN_OR(element, Token::TS_RIGHT_BRACE) {
        object_literal->InsertLast(element);
        if (element->first_child()->HasSymbol()) {
          prop->Declare(element->first_child()->symbol(), element);
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
    return object_literal;
  }
  SYNTAX_ERROR("Unexpected token.", Current());
}

template <typename UCharInputIterator>
Handle<ir::Node> Parser<UCharInputIterator>::ParsePropertyDefinition(bool yield) {
  LOG_PHASE(ParsePropertyDefinition);
  Handle<ir::Node> value;
  Handle<ir::Node> key;
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
      key = ParseIdentifierReference(yield);
    } else {
      key = ParsePropertyName(yield, false);
    }

    if (key) {
      if (key->HasSymbol()) {
        key->symbol()->set_type(ir::SymbolType::kPropertyName);
      }
    } else {
      if (getter || setter) {
        key = New<ir::NameView>(NewSymbol(ir::SymbolType::kPropertyName, info.value()));
        key->SetInformationForNode(&info);
      } else {
        return ir::Node::Null();
      }
    }

  if (Current()->type() == Token::TS_MUL) {
    generator = true;
    Next();
  }
  
  if (Current()->type() == Token::TS_LEFT_PAREN) {
    Handle<ir::Node> call_sig = ParseCallSignature(yield, false);
    CHECK_AST(call_sig);
    if (Current()->type() == Token::TS_LEFT_BRACE) {
      Handle<ir::Node> body = ParseFunctionBody(yield || generator);
      CHECK_AST(body);
      value = New<ir::FunctionView>(getter, setter, generator, New<ir::FunctionOverloadsView>(), ir::Node::Null(), call_sig, body);
    }
  } else if (generator) {
    SYNTAX_ERROR("'(' expected", Current());
  } else if (Current()->type() == Token::TS_COLON) {
    Next();
    value = ParseAssignmentExpression(true, false);
    CHECK_AST(value);
  }
  Handle<ir::ObjectElementView> element = New<ir::ObjectElementView>(key, value);
  element->SetInformationForNode(&info);
  return element;
}


template <typename UCharInputIterator>
Handle<ir::Node> Parser<UCharInputIterator>::ParsePropertyName(bool yield, bool generator_parameter) {
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
Handle<ir::Node> Parser<UCharInputIterator>::ParseLiteralPropertyName() {
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
Handle<ir::Node> Parser<UCharInputIterator>::ParseComputedPropertyName(bool yield) {
  LOG_PHASE(ParseComputedPropertyName);
  if (Current()->type() == Token::TS_LEFT_BRACKET) {
    Next();
    Handle<ir::Node> node = ParseAssignmentExpression(true, yield);
    CHECK_AST(node);
    if (Current()->type() == Token::TS_RIGHT_BRACKET) {
      Next();
      return node;
    }
    SYNTAX_ERROR("']' expected", Current());
  }
  SYNTAX_ERROR("'[' expected", Current());
}


template <typename UCharInputIterator>
Handle<ir::Node> Parser<UCharInputIterator>::ParseLiteral() {
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
Handle<ir::Node> Parser<UCharInputIterator>::ParseValueLiteral() {
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
Handle<ir::Node> Parser<UCharInputIterator>::ParseArrayInitializer(bool yield) {
  LOG_PHASE(ParseArrayInitializer);
  if (Current()->type() == Token::TS_LEFT_BRACKET) {
    RecordedParserState rps = parser_state();
    Handle<ir::Node> ret = ParseArrayLiteral(yield);
    if (!ret) {      
      if (!LanguageModeUtil::IsES6(compiler_option_)) {
        return ir::Node::Null();
      }
      RestoreParserState(rps);
      ret = ParseArrayComprehension(yield);
      CHECK_AST(ret);
    }
    return ret;
  }
  SYNTAX_ERROR("'[' expected", Current());
}


template <typename UCharInputIterator>
Handle<ir::Node> Parser<UCharInputIterator>::ParseIdentifierReference(bool yield) {
  LOG_PHASE(ParseIdentifierReference);
  if (Current()->type() == Token::TS_IDENTIFIER) {
    if (Current()->type() == Token::TS_YIELD) {
      if (!yield) {
        SYNTAX_ERROR("'yield' not allowed here", Current());
      }
      auto node = New<ir::YieldView>(false, ir::Node::Null());
      node->SetInformationForNode(Current());
      return node;
    }
    return ParseIdentifier();
  }
  SYNTAX_ERROR("'identifier' expected", Current());
}


template <typename UCharInputIterator>
Handle<ir::Node> Parser<UCharInputIterator>::ParseBindingIdentifier(bool default_allowed, bool yield) {
  LOG_PHASE(ParseBindingIdentifier);
  Handle<ir::Node> ret;
  if (Current()->type() == Token::TS_DEFAULT) {
    if (!default_allowed) {
      SYNTAX_ERROR("'default' keyword not allowed here", Current());
    }
    ret = New<ir::DefaultView>();
    Next();
  } else if (Current()->type() == Token::TS_YIELD) {
    Next();
    Handle<ir::Node> expr = ParseIdentifier();
    CHECK_AST(expr);
    ret = New<ir::YieldView>(false, expr);
    Next();
  } else if (Current()->type() == Token::TS_IDENTIFIER) {
    ret = ParseIdentifier();
    CHECK_AST(ret);
  } else {
    SYNTAX_ERROR("'default', 'yield' or 'identifier' expected", Current());
  }

  ret->SetInformationForNode(Current());

  return ret;
}

template <typename UCharInputIterator>
Handle<ir::Node> Parser<UCharInputIterator>::ParseLabelIdentifier(bool yield) {
  LOG_PHASE(ParseLabelIdentifier);
  if (Current()->type() == Token::TS_YIELD && yield) {
    SYNTAX_ERROR("yield not allowed here", Current());
  }
  if (Current()->type() == Token::TS_YIELD) {
    auto node = New<ir::YieldView>(false, ir::Node::Null());
    node->SetInformationForNode(Current());
    return node;
  }
  return ParseIdentifier();
}



template <typename UCharInputIterator>
Handle<ir::Node> Parser<UCharInputIterator>::ParseIdentifier() {
  LOG_PHASE(ParseIdentifier);
  if (Current()->type() == Token::TS_IDENTIFIER) {
    auto node = New<ir::NameView>(NewSymbol(ir::SymbolType::kVariableName, Current()->value()));
    node->SetInformationForNode(Current());
    Next();
    return node;
  }
  SYNTAX_ERROR("'identifier' expected", Current());
}


template <typename UCharInputIterator>
Handle<ir::Node> Parser<UCharInputIterator>::ParseStringLiteral() {
  LOG_PHASE(ParseStringLiteral);
  if (Current()->type() == Token::TS_STRING_LITERAL) {
    auto string_literal = New<ir::StringView>(Current()->value());
    string_literal->SetInformationForNode(Current());
    Next();
    return string_literal;
  }
  SYNTAX_ERROR("string literal expected", Current());
}


template <typename UCharInputIterator>
Handle<ir::Node> Parser<UCharInputIterator>::ParseNumericLiteral() {
  LOG_PHASE(ParseNumericLiteral);
  if (Current()->type() == Token::TS_NUMERIC_LITERAL) {
    auto number = New<ir::NumberView>(Current()->value());
    number->SetInformationForNode(Current());
    Next();
    return number;
  }
  SYNTAX_ERROR("numeric literal expected", Current());
}


template <typename UCharInputIterator>
Handle<ir::Node> Parser<UCharInputIterator>::ParseBooleanLiteral() {
  LOG_PHASE(ParseBooleanLiteral);
  Handle<ir::Node> ret;
  if (Current()->type() == Token::TS_TRUE) {
    ret = New<ir::TrueView>();
  } else if (Current()->type() == Token::TS_FALSE) {
    ret = New<ir::FalseView>();
  }

  if (ret) {
    ret->SetInformationForNode(Current());
    Next();
    return ret;
  }
  SYNTAX_ERROR("boolean literal expected", Current());
}


template <typename UCharInputIterator>
Handle<ir::Node> Parser<UCharInputIterator>::ParseUndefinedLiteral() {
  LOG_PHASE(ParseUndefinedLiteral);
  if (Current()->type() == Token::TS_UNDEFINED) {
    auto node = New<ir::UndefinedView>();
    node->SetInformationForNode(Current());
    Next();
    return node;
  }
  SYNTAX_ERROR("'undefined' expected", Current());
}


template <typename UCharInputIterator>
Handle<ir::Node> Parser<UCharInputIterator>::ParseNaNLiteral() {
  LOG_PHASE(ParseNaNLiteral);
  if (Current()->type() == Token::TS_NAN) {
    auto node = New<ir::NaNView>();
    node->SetInformationForNode(Current());
    Next();
    return node;
  }
  SYNTAX_ERROR("'NaN' expected", Current());
}


template <typename UCharInputIterator>
Handle<ir::Node> Parser<UCharInputIterator>::ParseRegularExpression() {
  LOG_PHASE(ParseRegularExpression);
  if (Current()->type() == Token::TS_REGULAR_EXPR) {
    auto reg_expr = New<ir::RegularExprView>(Current()->value());
    reg_expr->SetInformationForNode(Current());
    Next();
    return reg_expr;
  }
  SYNTAX_ERROR("regular expression expected", Current());
}


template <typename UCharInputIterator>
Handle<ir::Node> Parser<UCharInputIterator>::ParseTemplateLiteral() {
  LOG_PHASE(ParseTemplateLiteral);
  if (Current()->type() == Token::TS_TEMPLATE_LITERAL) {
    auto template_literal = New<ir::TemplateLiteralView>(Current()->value());
    Next();
    template_literal->SetInformationForNode(Current());
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
