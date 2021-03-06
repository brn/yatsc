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


// Statement[Yield, Return]
//   BlockStatement[?Yield, ?Return]
//   VariableStatement[?Yield]
//   EmptyStatement
//   ExpressionStatement[?Yield]
//   IfStatement[?Yield, ?Return]
//   BreakableStatement[?Yield, ?Return]
//   ContinueStatement[?Yield]
//   BreakStatement[?Yield]
//   [+Return] ReturnStatement[?Yield]
//   WithStatement[?Yield, ?Return]
//   LabelledStatement[?Yield, ?Return]
//   ThrowStatement[?Yield]
//   TryStatement[?Yield, ?Return]
//   DebuggerStatement
//
// Parse statments.
template <typename UCharInputIterator>
ParseResult Parser<UCharInputIterator>::ParseStatement() {
  LOG_PHASE(ParseStatement);
  ParseResult parse_result;
  
  switch (cur_token()->type()) {
    case TokenKind::kLeftBrace:
      parse_result = ParseBlockStatement();
      break;

    case TokenKind::kLineTerminator:
      parse_result = ParseEmptyStatement();
      break;

    case TokenKind::kIf:
      parse_result = ParseIfStatement();
      break;

    case TokenKind::kFor:
      parse_result = ParseForStatement();
      break;

    case TokenKind::kWhile:
      parse_result = ParseWhileStatement();
      break;

    case TokenKind::kDo:
      parse_result = ParseDoWhileStatement();
      break;

    case TokenKind::kContinue:
      // If this stament not in the iteration statement body,
      // record error and continue parsing.
      if (!state_.IsContinuable()) {
        ReportParseError(cur_token(), YATSC_SOURCEINFO_ARGS) << "'continue' only allowed in loops";
      }
      parse_result = ParseContinueStatement();
      break;

    case TokenKind::kBreak: {
      // If this stament not in the iteration statement body,
      // record error and continue parsing.
      if (!state_.IsBreakable()) {
        ReportParseError(cur_token(), YATSC_SOURCEINFO_ARGS) << "'break' not allowed here";
      }
      parse_result = ParseBreakStatement();
      
      if (IsLineTermination()) {
        ConsumeLineTerminator();
      } else if (!cur_token()->OneOf({TokenKind::kRightBrace, TokenKind::kLeftBrace})){
        // After break statement, followed token is not ';' or line break or '}' or '{',
        // that is error.
        ReportParseError(cur_token(), YATSC_SOURCEINFO_ARGS) << "';' expected";
      }
      break;
    }

    case TokenKind::kReturn:
      // If this stament not in the function body,
      // record error and continue parsing.
      if (!state_.IsReturnable()) {
        ReportParseError(cur_token(), YATSC_SOURCEINFO_ARGS) << "'return' statement only allowed in function";
      }
      parse_result = ParseReturnStatement();
      break;

    case TokenKind::kWith:
      parse_result = ParseWithStatement();
      break;

    case TokenKind::kSwitch:
      parse_result = ParseSwitchStatement();
      break;

    case TokenKind::kThrow:
      parse_result = ParseThrowStatement();
      break;

    case TokenKind::kTry:
      parse_result = ParseTryStatement();
      break;

    case TokenKind::kDebugger:
      parse_result = ParseDebuggerStatement();
      break;

    case TokenKind::kVar: {
      auto variable_stmt_result = ParseVariableStatement();
      
      if (IsLineTermination()) {
        ConsumeLineTerminator();
      } else if (!cur_token()->OneOf({TokenKind::kRightBrace, TokenKind::kLeftBrace})) {
        // After variable statement, followed token is not ';' or line break or '}' or '{',
        // that is error.
        ReportParseError(cur_token(), YATSC_SOURCEINFO_ARGS) << "';' expected";
      }
      return variable_stmt_result;
    }

    case TokenKind::kEof: {
      UnexpectedEndOfInput(cur_token(), YATSC_SOURCEINFO_ARGS);
      return Failed();
    }
      
    default: {
      // If keyword encounted, record as error and,
      // treat it as identifier and continue parsing.
      if (Token::IsKeyword(cur_token()->type()) &&
          !cur_token()->IsPrimaryKeyword()) {
        ReportParseError(cur_token(), YATSC_SOURCEINFO_ARGS)
          << "'" << cur_token()->utf8_value() << "' is not allowed here.";
        cur_token()->set_type(TokenKind::kIdentifier);
      }

      auto tokens = token_pack();

      // Labelled statment and expression is ambiguous,
      // so first, try parse as a labelled statement.
      // If labelled statement parsing is failed, parse as an expression.
      if (cur_token()->Is(TokenKind::kIdentifier)) {
        Next();

        // If colon is encounted after identifier,
        // it must be a labelled statement.
        if (cur_token()->Is(TokenKind::kColon)) {
          RestoreTokens(tokens);
          return ParseLabelledStatement();
        }

        RestoreTokens(tokens);
      }

      // Parse an expression.
      Token info = *cur_token();
      parse_result = ParseExpression();
      CHECK_AST(parse_result);
      parse_result = Success(New<ir::StatementView>(parse_result.value()));
      parse_result.value()->SetInformationForNode(&info);
    }
  }
  
  return parse_result;
}


template <typename UCharInputIterator>
ParseResult Parser<UCharInputIterator>::ParseDeclaration(bool error) {
  switch (cur_token()->type()) {
    case TokenKind::kFunction: {
      auto function_overloads_result = ParseFunctionOverloads(true, false);
      CHECK_AST(function_overloads_result);
      current_scope()->Declare(function_overloads_result.value());
      return function_overloads_result;
    }
    case TokenKind::kClass:
      return ParseClassDeclaration();
    case TokenKind::kEnum:
      return ParseEnumDeclaration();
    case TokenKind::kInterface:
      return ParseInterfaceDeclaration();
    case TokenKind::kLet:
    case TokenKind::kConst: {
      auto lexical_decl_result = ParseLexicalDeclaration();
      if (IsLineTermination()) {
        ConsumeLineTerminator();
      } else if (!cur_token()->Is(TokenKind::kRightBrace)) {
        ReportParseError(cur_token(), YATSC_SOURCEINFO_ARGS) << "';' expected.";
      }
      return lexical_decl_result;
    }
    default:
      if (!error) {
        return Failed();
      }
      ReportParseError(cur_token(), YATSC_SOURCEINFO_ARGS) << "unexpected token.";
      return Failed();
  }
}



// { StatementList[?Yield, ?Return](opt) }
//
// Parse block statment.
// e.g. { ... }
template <typename UCharInputIterator>
ParseResult Parser<UCharInputIterator>::ParseBlockStatement() {
  LOG_PHASE(ParseBlockStatement);
  
  Handle<ir::Scope> scope = NewScope();
  set_current_scope(scope);
  
  auto block_view = New<ir::BlockView>(scope);
  block_view->SetInformationForNode(cur_token());

  if (cur_token()->Is(TokenKind::kLeftBrace)) {
    OpenBraceFound();
    Next();
    
    while (!cur_token()->Is(TokenKind::kRightBrace)) {
      CheckEof(YATSC_SOURCEINFO_ARGS);
      auto statement_list_result = ParseStatementListItem();
      // If parse failed,
      // skip to next statement beggining.
      if (!statement_list_result) {
        SkipToNextStatement();
      } else {
        block_view->InsertLast(statement_list_result.value());
      }
    }

    CloseBraceFound();
    BalanceEnclosureIfNotBalanced(cur_token(), TokenKind::kRightBrace, true);
    
    set_current_scope(scope->parent_scope());
    return Success(block_view);
  }
  
  ReportParseError(cur_token(), YATSC_SOURCEINFO_ARGS) << "'{' expected";
  return Failed();
}

// StatementListItem[Yield, Return]
//   Statement[?Yield, ?Return]
//   Declaration[?Yield]
//
// Parse statements and declarations.
template <typename UCharInputIterator>
ParseResult Parser<UCharInputIterator>::ParseStatementListItem() {
  LOG_PHASE(ParseStatementListItem);

  // First parse declarations,
  // If declaration parsing failed,
  // parse statement.
  auto decl_result = ParseDeclaration(false);
  
  if (!decl_result) {
    return ParseStatement();
  }
  
  return decl_result;
}


// LexicalDeclaration[In, Yield]
//   LetOrConst BindingList[?In, ?Yield] ;
//
// Parse lexical declaration.
template <typename UCharInputIterator>
ParseResult Parser<UCharInputIterator>::ParseLexicalDeclaration() {
  LOG_PHASE(ParseLexicalDeclaration);
  bool has_const = cur_token()->Is(TokenKind::kConst);
  
  ir::LexicalDeclView* lexical_decl = New<ir::LexicalDeclView>(cur_token()->type());
  lexical_decl->SetInformationForNode(cur_token());

  // Consume let or const.
  Next();
  
  while (1) {
    auto lexical_decl_result = ParseLexicalBinding(has_const);
    
    if (lexical_decl_result) {
      lexical_decl->InsertLast(lexical_decl_result.value());
    } else {
      // If parse failed, skip to comma or line end.
      SKIP_IF_ERROR_RECOVERY_ENABLED(false, TokenKind::kComma, TokenKind::kLineTerminator);
    }

    // Parse comma.
    // const a = 1, b = 2, c = 3;
    if (cur_token()->Is(TokenKind::kComma)) {
      Next();
    } else {
      break;
    }
  }
  
  return Success(lexical_decl);
}


// BindingList[In, Yield]
//   LexicalBinding[?In, ?Yield]
//   BindingList[?In, ?Yield] , LexicalBinding[?In, ?Yield]
//   LexicalBinding[In, Yield]
//
// BindingIdentifier[?Yield] TypeAnnotation(opt) Initializer[?In, ?Yield](opt)
//   BindingPattern[?Yield] TypeAnnotation(opt) Initializer[?In, ?Yield]
//
// Parse lexical variable declaration identifier bindings.
template <typename UCharInputIterator>
ParseResult Parser<UCharInputIterator>::ParseLexicalBinding(bool const_decl) {
  LOG_PHASE(ParseLexicalBinding);

  ParseResult value_result;
  ParseResult lhs_result;

  // If binding is identifier, parse as simple identifier.
  // If not, parse as destructuring assignment pattern.
  if (cur_token()->Is(TokenKind::kIdentifier)) {
    lhs_result = ParseBindingIdentifier();
  } else {
    lhs_result = ParseBindingPattern();
  }

  // If binding parsing is failed,
  // parsing is not continue.
  CHECK_AST(lhs_result);

  if (!lhs_result.value()->IsValidLhs()) {
    ReportParseError(cur_token(), YATSC_SOURCEINFO_ARGS)
      << "left hand side of lexical binding is invalid";
    return Failed();
  }

  ParseResult type_expr_result;

  // If colon continue after binding,
  // that is type annotation, so parse as type expression.
  if (cur_token()->Is(TokenKind::kColon)) {
    Next();
    type_expr_result = ParseTypeExpression();
    CHECK_AST(type_expr_result);
  }

  // If initializer exists.
  if (cur_token()->Is(TokenKind::kAssign)) {
    Next();
    value_result = ParseAssignmentExpression();
    CHECK_AST(value_result);
    
  } else if (const_decl) {
    // If const declaration is not initialized,
    // treat it as error.
    ReportParseError(cur_token(), YATSC_SOURCEINFO_ARGS)
      << "const declaration must have an initializer.";
  }

  ir::Node* ret = New<ir::VariableView>(lhs_result.value(), value_result.or(Null()), type_expr_result.or(Null()));
  ret->SetInformationForNode(lhs_result.value());
  current_scope()->Declare(ret);
  return Success(ret);
}


// BindingIdentifier[Default, Yield]
//   [+Default] default
//   [~Yield] yield
//   Identifier
//
// Parse binding identifer.
template <typename UCharInputIterator>
ParseResult Parser<UCharInputIterator>::ParseBindingIdentifier() {
  LOG_PHASE(ParseBindingIdentifier);
  
  ir::Node* ret = nullptr;
  if (cur_token()->Is(TokenKind::kIdentifier)) {
    // If current parsing location is the inside generator block,
    // 'yield' is keyword and not allowed as a variable name, but parsing is continue.
    if (cur_token()->value()->Equals("yield") && !state_.IsInGenerator()) {
      ReportParseError(cur_token(), YATSC_SOURCEINFO_ARGS)
        << "yield not allowed here.";
    }
    ret = New<ir::NameView>(NewSymbol(ir::SymbolType::kVariableName, cur_token()->value()));
  } else {
    ReportParseError(cur_token(), YATSC_SOURCEINFO_ARGS)
      << "'identifier' expected.";
    return Failed();
  }
  
  ret->SetInformationForNode(cur_token());
  Next();

  return Success(ret);
}


// BindingPattern[Yield,GeneratorParameter]
//   ObjectBindingPattern[?Yield,?GeneratorParameter]
//   ArrayBindingPattern[?Yield,?GeneratorParameter]
//
// Parse binding pattern.
template <typename UCharInputIterator>
ParseResult Parser<UCharInputIterator>::ParseBindingPattern() {
  LOG_PHASE(ParseBindingPattern);
  switch (cur_token()->type()) {
    case TokenKind::kLeftBrace:
      return ParseObjectBindingPattern();
    case TokenKind::kLeftBracket:
      return ParseArrayBindingPattern();
    default:
      ReportParseError(cur_token(), YATSC_SOURCEINFO_ARGS) << "'[' or '{' expected.";
      return Failed();
  }
}


// ObjectBindingPattern[Yield,GeneratorParameter]
//   { }
//   { BindingPropertyList[?Yield,?GeneratorParameter] }
//   { BindingPropertyList[?Yield,?GeneratorParameter] , }
//
// BindingPropertyList[Yield,GeneratorParameter]
//   BindingProperty[?Yield, ?GeneratorParameter]
//   BindingPropertyList[?Yield, ?GeneratorParameter] , BindingProperty[?Yield, ?GeneratorParameter]
// Parse destructuring assignment pattern for object.
template <typename UCharInputIterator>
ParseResult Parser<UCharInputIterator>::ParseObjectBindingPattern() {
  LOG_PHASE(ParseObjectBindingPattern);
  
  ir::Node* binding_prop_list = New<ir::BindingPropListView>();
  binding_prop_list->SetInformationForNode(cur_token());
  Next();
    
  while (1) {
    auto binding_prop_result = ParseBindingProperty();

    if (binding_prop_list) {
      binding_prop_list->InsertLast(binding_prop_result.value());
    } else {
      SKIP_IF_ERROR_RECOVERY_ENABLED(false, TokenKind::kRightBrace, TokenKind::kComma);
    }

 DELIMITER:
    switch (cur_token()->type()) {
      case TokenKind::kComma: {
        Next();
        break;
      }
      case TokenKind::kRightBrace: {
        CloseBraceFound();
        BalanceEnclosureIfNotBalanced(cur_token(), TokenKind::kRightBrace, true);
        return Success(binding_prop_list);
      }
      default:
        ReportParseError(cur_token(), YATSC_SOURCEINFO_ARGS) << "unexpected token.";
        SKIP_IF_ERROR_RECOVERY_ENABLED(false, TokenKind::kRightBrace, TokenKind::kComma);
        goto DELIMITER;
    }
  }
}


// ArrayBindingPattern[Yield,GeneratorParameter]
//   [ Elision(opt) BindingRestElement[?Yield, ?GeneratorParameter](opt) ]
//   [ BindingElementList[?Yield, ?GeneratorParameter] ]
//   [ BindingElementList[?Yield, ?GeneratorParameter] , Elision(opt) BindingRestElement[?Yield, ?GeneratorParameter](opt) ]
//
// BindingElementList[Yield,GeneratorParameter]
//   BindingElisionElement[?Yield, ?GeneratorParameter]
//   BindingElementList[?Yield, ?GeneratorParameter] , BindingElisionElement[?Yield, ?GeneratorParameter]
//
// BindingElisionElement[Yield,GeneratorParameter]
//   Elision(opt) BindingElement[?Yield, ?GeneratorParameter]
//
// BindingRestElement[Yield, GeneratorParameter]
//   [+GeneratorParameter] ... BindingIdentifier[Yield]
//   [~GeneratorParameter] ... BindingIdentifier[?Yield]
//
// Parse destructuring assignment pattern for array.
template <typename UCharInputIterator>
ParseResult Parser<UCharInputIterator>::ParseArrayBindingPattern() {
  LOG_PHASE(ParseArrayBindingPattern);
    
  ir::Node* binding_array = New<ir::BindingArrayView>();
  binding_array->SetInformationForNode(cur_token());
  Next();

  bool exit = false;
    
  while (1) {
    // If comma found in array binding pattern,
    // this element is skipped.
    if (cur_token()->Is(TokenKind::kComma)) {
      Next();
      // Insert empty.
      binding_array->InsertLast(ir::Node::Null());
    }

    // Case rest element.
    if (cur_token()->Is(TokenKind::kRest)) {
      ir::RestParamView* rest = New<ir::RestParamView>();
      rest->SetInformationForNode(cur_token());
      Next();

      auto binding_identifier_result = ParseBindingIdentifier();
      
      if (binding_identifier_result) {
        rest->set_parameter(binding_identifier_result.value());
        binding_array->InsertLast(rest);
        // Binding rest element must be end of element,
        // so, any element after rest element cause syntax error.
        exit = true;
      } else {
        SKIP_IF_ERROR_RECOVERY_ENABLED(false, TokenKind::kRightBracket, TokenKind::kComma);
      }
      
    } else {
      auto binding_elem_result = ParseBindingElement();
      
      if (binding_elem_result) {
        ParseResult assignment_expr_result;

        // Case initializer.
        if (cur_token()->Is(TokenKind::kAssign)) {
          assignment_expr_result = ParseAssignmentExpression();
        }
        
        auto ret = New<ir::BindingElementView>(binding_elem_result.value(),
                                               assignment_expr_result.or(Null()));
        ret->SetInformationForNode(binding_elem_result.value());
        binding_array->InsertLast(ret);
      } else {
        SKIP_IF_ERROR_RECOVERY_ENABLED(false, TokenKind::kRightBracket, TokenKind::kComma);
      }
    }

    if (cur_token()->Is(TokenKind::kRightBracket)) {
      Next();
      break;
    } else if (exit) {
      ReportParseError(cur_token(), YATSC_SOURCEINFO_ARGS) << "spread binding must be end of bindings";
      return Failed();
    } else if (cur_token()->Is(TokenKind::kComma)) {
      Next();
    } else {
      ReportParseError(cur_token(), YATSC_SOURCEINFO_ARGS) << "unexpected token.";
      SKIP_IF_ERROR_RECOVERY_ENABLED(false, TokenKind::kRightBracket, TokenKind::kComma);
    }
  }
  
  return Success(binding_array);
}


// BindingProperty[Yield,GeneratorParameter]
//   SingleNameBinding[?Yield, ?GeneratorParameter]
//
// SingleNameBinding[Yield,GeneratorParameter]
//   [+GeneratorParameter] BindingIdentifier[Yield] Initializer[In](opt)
//   [~GeneratorParameter] BindingIdentifier[?Yield] Initializer[In, ?Yield](opt)
//
// Parse destructuring assignment object property.
template <typename UCharInputIterator>
ParseResult Parser<UCharInputIterator>::ParseBindingProperty() {
  LOG_PHASE(ParseBindingProperty);
  ParseResult key_result;
  ParseResult value_result;

  if (cur_token()->Is(TokenKind::kIdentifier)) {
    key_result = ParseIdentifier();
    CHECK_AST(key_result);
  } else {
    ReportParseError(cur_token(), YATSC_SOURCEINFO_ARGS) << "'identifier' expected.";
    return Failed();
  }

  // Case nested element.
  if (cur_token()->Is(TokenKind::kColon)) {
    Next();
    value_result = ParseBindingElement();
    CHECK_AST(value_result);
  } else if (cur_token()->Is(TokenKind::kAssign)) {
    // Case initializer.
    Next();
    value_result = ParseAssignmentExpression();
    CHECK_AST(value_result);
  }
  
  ir::Node* ret = New<ir::BindingElementView>(key_result.value(), value_result.or(Null()));
  ret->SetInformationForNode(key_result.value());
  return Success(ret);
}


// BindingElement[Yield, GeneratorParameter ]
//   SingleNameBinding[?Yield, ?GeneratorParameter]
//   [+GeneratorParameter] BindingPattern[?Yield,GeneratorParameter] Initializer[In](opt)
//   [~GeneratorParameter] BindingPattern[?Yield] Initializer[In, ?Yield](opt)
//
// Parse destructuring array element.
template <typename UCharInputIterator>
ParseResult Parser<UCharInputIterator>::ParseBindingElement() {
  LOG_PHASE(ParseBindingElement);
  switch (cur_token()->type()) {
    case TokenKind::kIdentifier:
    case TokenKind::kDefault:
      return ParseBindingIdentifier();
    default:
      return ParseBindingPattern();
  }
}


// VariableStatement[Yield]
//   var VariableDeclarationList[In, ?Yield] ;
//
// VariableDeclarationList[In, Yield]
//   VariableDeclaration[?In, ?Yield]
//   VariableDeclarationList[?In, ?Yield] , VariableDeclaration[?In, ?Yield]
//
// Parse variable statement.
template <typename UCharInputIterator>
ParseResult Parser<UCharInputIterator>::ParseVariableStatement() {
  Next();
  ir::VariableDeclView* vars = New<ir::VariableDeclView>();
    
  while (1) {
    auto variable_decl_result = ParseVariableDeclaration();
    
    if (variable_decl_result) {
      vars->InsertLast(variable_decl_result.value());
    } else {
      SKIP_IF_ERROR_RECOVERY_ENABLED(false, TokenKind::kComma, TokenKind::kLineTerminator);
    }
    
    if (cur_token()->Is(TokenKind::kComma)) {
      Next();
    } else {
      break;
    }
  }
    
  return Success(vars);

}


// VariableDeclaration[In, Yield]
//   BindingIdentifier[?Yield] TypeAnnotation(opt) Initializer[?In, ?Yield](opt)
//   BindingPattern[Yield] TypeAnnotation(opt) Initializer[?In, ?Yield]
//
// Parse variable declaration.
template <typename UCharInputIterator>
ParseResult Parser<UCharInputIterator>::ParseVariableDeclaration() {
  LOG_PHASE(ParseVariableDeclaration);

  ParseResult value_result;
  ParseResult lhs_result;
  
  if (cur_token()->Is(TokenKind::kIdentifier)) {
    lhs_result = ParseBindingIdentifier();
  } else if (cur_token()->IsKeyword()) {
    // If keyword found in binding identifier section,
    // we recognize it as identifier and continue parsing.
    ReportParseError(cur_token(), YATSC_SOURCEINFO_ARGS)
      << "keyword '" << cur_token()->utf8_value() << "' is not valid identifier.";
    cur_token()->set_type(TokenKind::kIdentifier);
    lhs_result = ParseBindingIdentifier();
  } else {
    lhs_result = ParseBindingPattern();
  }

  CHECK_AST(lhs_result);

  if (!lhs_result.value()->IsValidLhs()) {
    ReportParseError(cur_token(), YATSC_SOURCEINFO_ARGS) << "left hand side of variable declaration is invalid.";
    return Failed();
  }

  ParseResult type_expr_result;
  if (cur_token()->Is(TokenKind::kColon)) {
    Next();
    type_expr_result = ParseTypeExpression();
    CHECK_AST(type_expr_result);
  }
  
  if (cur_token()->Is(TokenKind::kAssign)) {
    Next();
    value_result = ParseAssignmentExpression();
    CHECK_AST(value_result);
  }
  
  ir::Node* ret = New<ir::VariableView>(lhs_result.value(),
                                               value_result.or(Null()),
                                               type_expr_result.or(Null()));
  ret->SetInformationForNode(lhs_result.value());
  current_scope()->Declare(ret);
  
  return Success(ret);
}


// IfStatement[Yield, Return]
//   if ( Expression[In, ?Yield] ) Statement[?Yield, ?Return] else Statement[?Yield, ?Return]
//   if ( Expression[In, ?Yield] ) Statement[?Yield, ?Return]
//
// Parse if statement.
template <typename UCharInputIterator>
ParseResult Parser<UCharInputIterator>::ParseIfStatement() {
  LOG_PHASE(ParseIfStatement);
  Token info = *cur_token();
  Next();

  if (cur_token()->Is(TokenKind::kLeftParen)) {
    OpenParenFound();
    Next();
  } else {
    ReportParseError(cur_token(), YATSC_SOURCEINFO_ARGS) << "'(' expected."; 
  }
  

  auto expr_result = ParseExpression();
    
  if (cur_token()->Is(TokenKind::kRightParen)) {
    CloseParenFound();
  } else {
    ReportParseError(cur_token(), YATSC_SOURCEINFO_ARGS) << "')' expected.";
  }
    
  BalanceEnclosureIfNotBalanced(cur_token(), TokenKind::kRightParen, true);
    
  auto then_stmt_result = ParseStatement();
      
  if (!prev_token()->Is(TokenKind::kRightBrace) && IsLineTermination()) {
    ConsumeLineTerminator();
  }
      
  ParseResult else_stmt_result;
      
  if (cur_token()->Is(TokenKind::kElse)) {
    Next();
    else_stmt_result = ParseStatement();
    if (prev_token()->Is(TokenKind::kRightBrace) && IsLineTermination()) {
      ConsumeLineTerminator();
    }
  }
      
  ir::IfStatementView* if_stmt = New<ir::IfStatementView>(expr_result.or(Null()),
                                                                 then_stmt_result.or(Null()),
                                                                 else_stmt_result.or(Null()));
  if_stmt->SetInformationForNode(&info);
  return Success(if_stmt);
}


// while ( Expression[In, ?Yield] ) Statement[?Yield, ?Return]
//
// Parse while statement.
template <typename UCharInputIterator>
ParseResult Parser<UCharInputIterator>::ParseWhileStatement() {
  LOG_PHASE(ParseWhileStatement);
  Token info = *(cur_token());

  // Consume 'while' keyword.
  Next();
  
  if (cur_token()->Is(TokenKind::kLeftParen)) {
    OpenParenFound();
    Next();
    auto expr_result = ParseExpression();
    if (!expr_result) {
      SKIP_IF_ERROR_RECOVERY_ENABLED(false, TokenKind::kRightParen);
    }
    auto iteration_body_result = ParseIterationBody();
    CHECK_AST(iteration_body_result);
    auto while_stmt = New<ir::WhileStatementView>(expr_result.or(Null()), iteration_body_result.value());
    while_stmt->SetInformationForNode(&info);
    return Success(while_stmt);
  }
  
  ReportParseError(cur_token(), YATSC_SOURCEINFO_ARGS) << "'(' expected.";
  return Failed();
}


// do_while_statement
//   : 'do' statement[?yield, ?return] 'while' '(' expression[in, ?yield] ')' ;__opt
//   ;
template <typename UCharInputIterator>
ParseResult Parser<UCharInputIterator>::ParseDoWhileStatement() {
  LOG_PHASE(ParseDoWhileStatement);
  Token info = *(cur_token());
  Next();

  state_.EnterIteration();
  auto stmt_result = ParseStatement();
  state_.ExitIteration();
  
  CHECK_AST(stmt_result);
  
  if (cur_token()->Is(TokenKind::kWhile)) {
    Next();
    
    if (cur_token()->Is(TokenKind::kLeftParen)) {
      
      OpenParenFound();
      Next();
      auto expr_result = ParseExpression();
      CHECK_AST(expr_result);
      
      if (cur_token()->Is(TokenKind::kRightParen)) {

        CloseParenFound();
        BalanceEnclosureIfNotBalanced(cur_token(), TokenKind::kRightParen, true);
        
        if (IsLineTermination()) {
          ConsumeLineTerminator();
        }
        
        auto do_while = New<ir::DoWhileStatementView>(expr_result.value(), stmt_result.value());
        do_while->SetInformationForNode(&info);
        return Success(do_while);
      }

      BalanceEnclosureIfNotBalanced(cur_token(), TokenKind::kRightParen, true);
      
      ReportParseError(cur_token(), YATSC_SOURCEINFO_ARGS) << "')' expected.";
      return Failed();
    }
    
    ReportParseError(cur_token(), YATSC_SOURCEINFO_ARGS) << "'(' expected.";
    return Failed();
  }

  ReportParseError(cur_token(), YATSC_SOURCEINFO_ARGS) << "'while' expected.";
  return Failed();
}


template <typename UCharInputIterator>
ParseResult Parser<UCharInputIterator>::ParseForStatement() {
  LOG_PHASE(ParseForStatement);

  Token info = *cur_token();
  Next();
  
  if (cur_token()->Is(TokenKind::kLeftParen)) {
    OpenParenFound();
    Next();
    ParseResult reciever_result;
    
    switch (cur_token()->type()) {
      case TokenKind::kVar: {
        reciever_result = ParseVariableStatement();
        CHECK_AST(reciever_result);
        break;
      }
      case TokenKind::kLet: {
        reciever_result = ParseLexicalDeclaration();
        CHECK_AST(reciever_result);
        break;
      }
      case TokenKind::kLineTerminator:
        break;
      default: {

        state_.EnterNoInExpr();
        reciever_result = ParseExpression();
        state_.ExitNoInExpr();
        CHECK_AST(reciever_result);
      }
    }
    
    return ParseForIteration(reciever_result.or(ir::Node::Null()), &info);
  }

  ReportParseError(cur_token(), YATSC_SOURCEINFO_ARGS) << "'(' expected.";
  return Failed();
}


template <typename UCharInputIterator>
ParseResult Parser<UCharInputIterator>::ParseForIteration(ir::Node* reciever, Token* token_info) {
  LOG_PHASE(ParseForIterationStatement);

  ParseResult second_result;
  ParseResult third_result;
  bool for_in = false;
  bool for_of = false;
  
  if (cur_token()->Is(TokenKind::kLineTerminator)) {
    // for (var i = 0; i < 10; i++) ...
    Next();
    if (!cur_token()->Is(TokenKind::kLineTerminator)) {
      second_result = ParseExpression();
      CHECK_AST(second_result);
    }
    if (cur_token()->Is(TokenKind::kLineTerminator)) {
      Next();
      if (!cur_token()->Is(TokenKind::kRightParen)) {
        third_result = ParseExpression();
        CHECK_AST(third_result);
      }
      
    }
  } else if (cur_token()->Is(TokenKind::kIdentifier) &&
             cur_token()->value()->Equals("of")) {
    // for (var i of obj) ...
    Next();
    second_result = ParseAssignmentExpression();
    CHECK_AST(second_result);
    for_of = true;
  } else if (cur_token()->Is(TokenKind::kIn)) {
    // for (var i in obj) ...
    Next();
    
    state_.EnterNoInExpr();
    second_result = ParseAssignmentExpression();
    state_.ExitNoInExpr();
    
    CHECK_AST(second_result);
    for_in = true;
  } else {
    ReportParseError(cur_token(), YATSC_SOURCEINFO_ARGS) << "'in' or 'of' or ';' expected.";
    SKIP_IF_ERROR_RECOVERY_ENABLED(true, TokenKind::kRightParen);
  }

  auto iteration_body_result = ParseIterationBody();
  ir::Node* ret = nullptr;
  
  if (for_in) {
    ret = New<ir::ForInStatementView>(reciever, second_result.or(Null()), iteration_body_result.or(Null()));
  } else if (for_of) {
    ret = New<ir::ForOfStatementView>(reciever, second_result.or(Null()), iteration_body_result.or(Null()));
  } else {
    ret = New<ir::ForStatementView>(reciever, second_result.or(Null()), third_result.or(Null()), iteration_body_result.or(Null()));
  }
    
  ret->SetInformationForNode(token_info);
  return Success(ret);
}


template <typename UCharInputIterator>
ParseResult Parser<UCharInputIterator>::ParseIterationBody() {
  if (cur_token()->Is(TokenKind::kRightParen)) {
    CloseParenFound();
    BalanceEnclosureIfNotBalanced(cur_token(), TokenKind::kRightParen, true);
    state_.EnterIteration();
    YATSC_SCOPED([&]{state_.ExitIteration();});
    return ParseStatement();
  }
  
  ReportParseError(cur_token(), YATSC_SOURCEINFO_ARGS) << "')' expected.";
  return Failed();
}


template <typename UCharInputIterator>
ParseResult Parser<UCharInputIterator>::ParseContinueStatement() {
  LOG_PHASE(ParseContinueStatement);

  Token info = *cur_token();
  Next();
  ParseResult parse_result;
  YATSC_SCOPED([&]{
    if (IsLineTermination()) {
      ConsumeLineTerminator();
    }
    if (parse_result) {
      parse_result.value()->SetInformationForNode(&info);
    }
  });
  
  if (cur_token()->Is(TokenKind::kIdentifier)) {
    auto primary_expr_result = ParsePrimaryExpression();
    CHECK_AST(primary_expr_result);
    return parse_result = Success(New<ir::ContinueStatementView>(primary_expr_result.value()));
  }
  return parse_result = Success(New<ir::ContinueStatementView>());
}


template <typename UCharInputIterator>
ParseResult Parser<UCharInputIterator>::ParseBreakStatement() {
  LOG_PHASE(ParseBreakStatement);

  Token info = *cur_token();
  Next();
  ParseResult parse_result;
  YATSC_SCOPED([&]{
    if (parse_result) {
      parse_result.value()->SetInformationForNode(&info);
    }
  });
  
  if (cur_token()->Is(TokenKind::kIdentifier)) {
    auto primary_expr_result = ParsePrimaryExpression();
    return parse_result = Success(New<ir::BreakStatementView>(primary_expr_result.value()));
  }
  
  return parse_result = Success(New<ir::BreakStatementView>());
}


template <typename UCharInputIterator>
ParseResult Parser<UCharInputIterator>::ParseReturnStatement() {
  LOG_PHASE(ParseReturnStatement);

  Token info = *cur_token();
  ParseResult parse_result;
  YATSC_SCOPED([&]{
    if (parse_result) {
      parse_result.value()->SetInformationForNode(&info);
    }
  });
    
  Next();
    
  if (IsLineTermination()) {
    ConsumeLineTerminator();
    return parse_result = Success(New<ir::ReturnStatementView>());
  }
  
  auto expr_result = ParseExpression();
  CHECK_AST(expr_result);
  return parse_result = Success(New<ir::ReturnStatementView>(expr_result.value()));
}


template <typename UCharInputIterator>
ParseResult Parser<UCharInputIterator>::ParseWithStatement() {
  LOG_PHASE(ParseWithStatement);

  Token info = *cur_token();
  ParseResult parse_result;
  YATSC_SCOPED([&]{
    if (parse_result) {
      parse_result.value()->SetInformationForNode(&info);
    }
  });
    
  Next();
    
  if (cur_token()->Is(TokenKind::kLeftParen)) {
    OpenParenFound();
    Next();
    auto expr_result = ParseExpression();
    CHECK_AST(expr_result);
    if (cur_token()->Is(TokenKind::kRightParen)) {
      CloseParenFound();
      BalanceEnclosureIfNotBalanced(cur_token(), TokenKind::kRightParen, true);
      auto stmt_result = ParseStatement();
      CHECK_AST(stmt_result);
      return parse_result = Success(New<ir::WithStatementView>(expr_result.value(), stmt_result.value()));
    }
    
    ReportParseError(cur_token(), YATSC_SOURCEINFO_ARGS) << "')' expected.";
    BalanceEnclosureIfNotBalanced(cur_token(), TokenKind::kRightParen, true);    
    return Failed();
  }

  ReportParseError(cur_token(), YATSC_SOURCEINFO_ARGS) << "'(' expected.";
  return Failed();
}


template <typename UCharInputIterator>
ParseResult Parser<UCharInputIterator>::ParseSwitchStatement() {
  LOG_PHASE(ParseSwitchStatement);

  Token info = *cur_token();
  Next();
  
  if (cur_token()->Is(TokenKind::kLeftParen)) {

    OpenParenFound();

    Next();
    auto expr_result = ParseExpression();
    CHECK_AST(expr_result);
    
    if (cur_token()->Is(TokenKind::kRightParen)) {
      CloseParenFound();
      BalanceEnclosureIfNotBalanced(cur_token(), TokenKind::kRightParen, true);
      
      if (cur_token()->Is(TokenKind::kLeftBrace)) {        
        OpenBraceFound();
        Next();
        auto case_clauses_result = ParseCaseClauses();
        CHECK_AST(case_clauses_result);
        
        if (cur_token()->Is(TokenKind::kRightBrace)) {
          CloseBraceFound();
          BalanceEnclosureIfNotBalanced(cur_token(), TokenKind::kRightBrace, true);
          auto switch_stmt = New<ir::SwitchStatementView>(expr_result.value(), case_clauses_result.value());
          switch_stmt->SetInformationForNode(&info);
          return Success(switch_stmt);
        }

        ReportParseError(cur_token(), YATSC_SOURCEINFO_ARGS) << "'}' expected.";
        BalanceEnclosureIfNotBalanced(cur_token(), TokenKind::kRightBrace, true);
        return Failed();
      }

      ReportParseError(cur_token(), YATSC_SOURCEINFO_ARGS) << "'{' expected.";
      return Failed();
    }

    ReportParseError(cur_token(), YATSC_SOURCEINFO_ARGS) << "')' expected.";
    BalanceEnclosureIfNotBalanced(cur_token(), TokenKind::kRightParen, true);
    return Failed();
  }

  
  ReportParseError(cur_token(), YATSC_SOURCEINFO_ARGS) << "'(' expected.";
  return Failed();
}


template <typename UCharInputIterator>
ParseResult Parser<UCharInputIterator>::ParseCaseClauses() {
  LOG_PHASE(ParseCaseClauses);
  ir::CaseListView* case_list = New<ir::CaseListView>();
  case_list->SetInformationForNode(cur_token());
  bool default_encounted = false;
  
  while (1) {
    bool normal_case = false;
    ParseResult expr_result;
    Token info = *cur_token();
    
    switch (cur_token()->type()) {
      
      case TokenKind::kCase: {
        normal_case = true;
        Next();
        expr_result = ParseExpression();
        if (!expr_result) {
          SKIP_IF_ERROR_RECOVERY_ENABLED(false, TokenKind::kRightBrace, TokenKind::kCase, TokenKind::kDefault);
          break;
        }
      }
        FALLTHROUGH;
      case TokenKind::kDefault: {
        if (!normal_case) {
          default_encounted = true;
          Next();
        }
        
        if (cur_token()->Is(TokenKind::kColon)) {
          Next();
        } else {
          ReportParseError(cur_token(), YATSC_SOURCEINFO_ARGS) << "':' expected.";
        }
        
        ir::Node* body = New<ir::CaseBody>();
        
        while (1) {

          if (default_encounted && cur_token()->Is(TokenKind::kDefault)) {
            ReportParseError(cur_token(), YATSC_SOURCEINFO_ARGS) << "more than one 'default' clause in switch statement.";
          }
          
          if (cur_token()->OneOf({TokenKind::kCase, TokenKind::kDefault, TokenKind::kRightBrace})) {
            break;
          }

          state_.EnterCaseBlock();
          if (cur_token()->Is(TokenKind::kLeftBrace)) {
            auto block_stmt_result = ParseBlockStatement();
            state_.ExitCaseBlock();
            if (block_stmt_result) {
              body->InsertLast(block_stmt_result.value());
            } else {
              SKIP_IF_ERROR_RECOVERY_ENABLED(false, TokenKind::kRightBrace, TokenKind::kCase, TokenKind::kDefault);
            }
          } else {
            auto stmt_list_result = ParseStatementListItem();
            state_.ExitCaseBlock();
            if (stmt_list_result) {
              body->InsertLast(stmt_list_result.value());
            } else {
              SKIP_IF_ERROR_RECOVERY_ENABLED(false, TokenKind::kRightBrace, TokenKind::kCase, TokenKind::kDefault);
            }
          }
        }
        
        ir::CaseView* case_view = New<ir::CaseView>(expr_result.or(ir::Node::Null()), body);
        case_view->SetInformationForNode(&info);
        case_list->InsertLast(case_view);
        break;
      }
        
      case TokenKind::kRightBrace: {
        CloseBraceFound();
        BalanceEnclosureIfNotBalanced(cur_token(), TokenKind::kRightBrace, false);
        return Success(case_list);
      }

      case TokenKind::kEof: {
        UnexpectedEndOfInput(cur_token(), YATSC_SOURCEINFO_ARGS);
        return Failed();
      }
        
      default:
        ReportParseError(cur_token(), YATSC_SOURCEINFO_ARGS) << "unexpected token.";
        SKIP_IF_ERROR_RECOVERY_ENABLED(false, TokenKind::kCase, TokenKind::kDefault, TokenKind::kRightBrace);
    }
  }
}


template <typename UCharInputIterator>
ParseResult Parser<UCharInputIterator>::ParseLabelledStatement() {
  LOG_PHASE(ParseLabelledStatement);
  auto label_identifier_result = ParseLabelIdentifier();
  CHECK_AST(label_identifier_result);
  label_identifier_result.value()->symbol()->set_type(ir::SymbolType::kLabelName);

  current_scope()->Declare(label_identifier_result.value());

  if (!cur_token()->Is(TokenKind::kColon)) {
    ReportParseError(cur_token(), YATSC_SOURCEINFO_ARGS) << "':' expected.";
  } else {
    Next();
  }
  
  return ParseLabelledItem() >>= [&] (ir::Node* labelled_item) {
    auto node = New<ir::LabelledStatementView>(label_identifier_result.value(), labelled_item);
    node->SetInformationForNode(label_identifier_result.value());
    return Success(node);
  };
}


template <typename UCharInputIterator>
ParseResult Parser<UCharInputIterator>::ParseLabelledItem() {
  if (cur_token()->Is(TokenKind::kFunction)) {
    return ParseFunctionOverloads(false, false);
  }
  return ParseStatement();
}


template <typename UCharInputIterator>
ParseResult Parser<UCharInputIterator>::ParseThrowStatement() {
  LOG_PHASE(ParseThrowStatement);
  
  Token info = *cur_token();
  Next();

  if (IsLineTermination()) {
    ReportParseError(&info, YATSC_SOURCEINFO_ARGS) << "after 'throw' keyword expression expected.";
    Next();  
    return Failed();
  }
  
  return ParseExpression() >>= [&](ir::Node* expr) {
    auto throw_stmt = New<ir::ThrowStatementView>(expr);
    throw_stmt->SetInformationForNode(&info);
    return Success(throw_stmt);
  };
}


template <typename UCharInputIterator>
ParseResult Parser<UCharInputIterator>::ParseTryStatement() {
  LOG_PHASE(ParseTryStatement);

  Token info = *cur_token();
  Next();
  
  auto block_stmt_result = ParseBlockStatement();
  
  ParseResult catch_block_result;
  ParseResult finally_block_result;
  bool has_catch_or_finally = false;

  if (cur_token()->Is(TokenKind::kCatch)) {
    has_catch_or_finally = true;
    catch_block_result = ParseCatchBlock();
  }
    
  if (cur_token()->Is(TokenKind::kFinally)) {
    has_catch_or_finally = true;
    finally_block_result = ParseFinallyBlock();
  }

  if (!has_catch_or_finally) {
    ReportParseError(&info, YATSC_SOURCEINFO_ARGS) << "after try block, catch block or finally block expected.";
    return Failed();
  }
    
  ir::TryStatementView* try_stmt = New<ir::TryStatementView>(block_stmt_result.or(Null()),
                                                                    catch_block_result.or(Null()),
                                                                    finally_block_result.or(Null()));
  try_stmt->SetInformationForNode(&info);
  return Success(try_stmt);
}


template <typename UCharInputIterator>
ParseResult Parser<UCharInputIterator>::ParseCatchBlock() {
  LOG_PHASE(ParseCatchBlock);

  Token info = *cur_token();
  Next();
  
  if (cur_token()->Is(TokenKind::kLeftParen)) {
    OpenParenFound();
    Next();
    ParseResult catch_parameter_result;
    
    if (cur_token()->Is(TokenKind::kIdentifier)) {
      catch_parameter_result = ParseBindingIdentifier();
    } else {
      catch_parameter_result = ParseBindingPattern();
    }

    if (cur_token()->Is(TokenKind::kRightParen)) {

      CloseParenFound();
      BalanceEnclosureIfNotBalanced(cur_token(), TokenKind::kRightParen, true);
      
      return ParseBlockStatement() >>= [&](ir::Node* catch_block) {
        ir::CatchStatementView* catch_stmt = New<ir::CatchStatementView>(catch_parameter_result.or(Null()),
                                                                                catch_block);
        catch_stmt->SetInformationForNode(&info);
        return Success(catch_stmt);
      };
    }
    
    ReportParseError(cur_token(), YATSC_SOURCEINFO_ARGS) << "')' expected.";
    BalanceEnclosureIfNotBalanced(cur_token(), TokenKind::kRightParen, true);
    return Failed();
  }
  
  ReportParseError(cur_token(), YATSC_SOURCEINFO_ARGS) << "'(' expected.";
  return Failed();
}


template <typename UCharInputIterator>
ParseResult Parser<UCharInputIterator>::ParseFinallyBlock() {
  LOG_PHASE(ParseFinallyBlock);
  Token info = *cur_token();
  Next();
  
  return ParseBlockStatement() >>= [&](ir::Node* block_stmt) {
    ir::FinallyStatementView* finally_stmt = New<ir::FinallyStatementView>(block_stmt);
    finally_stmt->SetInformationForNode(&info);
    return Success(finally_stmt);
  };
}


template <typename UCharInputIterator>
ParseResult Parser<UCharInputIterator>::ParseDebuggerStatement() {
  LOG_PHASE(ParseDebuggerStatement);

  ir::Node* ret = New<ir::DebuggerView>();
  ret->SetInformationForNode(cur_token());
  Next();
  return Success(ret);
}


template <typename UCharInputIterator>
ParseResult Parser<UCharInputIterator>::ParseInterfaceDeclaration() {
  LOG_PHASE(ParseInterfaceDeclaration);
  Token info = *cur_token();

  Next();

  auto identifier_result = ParseIdentifier();
  if (!identifier_result) {
    SKIP_IF_ERROR_RECOVERY_ENABLED(false, TokenKind::kExtends, TokenKind::kLeftBrace, TokenKind::kRightBrace);
  } else {
    identifier_result.value()->symbol()->set_type(ir::SymbolType::kInterfaceName);
  }
  
  ParseResult type_parameters_result;
    
  if (cur_token()->Is(TokenKind::kLess)) {
    type_parameters_result = ParseTypeParameters();
    
    if (!type_parameters_result) {
      SKIP_IF_ERROR_RECOVERY_ENABLED(false, TokenKind::kGreater, TokenKind::kExtends, TokenKind::kLeftBrace, TokenKind::kRightBrace);
    }
  }
    
  auto extends = New<ir::InterfaceExtendsView>();

  if (cur_token()->Is(TokenKind::kExtends)) {
    Next();
    while (1) {
      auto ref_type_result = ParseReferencedType();
      if (!ref_type_result) {
        SKIP_IF_ERROR_RECOVERY_ENABLED(false, TokenKind::kComma, TokenKind::kLeftBrace);
      } else {
        extends->InsertLast(ref_type_result.value());
      }

      if (cur_token()->Is(TokenKind::kComma)) {
        Next();
      } else if (cur_token()->Is(TokenKind::kEof)) {
        UnexpectedEndOfInput(cur_token(), YATSC_SOURCEINFO_ARGS);
        return Failed();
      } else {
        break;
      }
    }
  }

  if (cur_token()->Is(TokenKind::kLeftBrace)) {
    OpenBraceFound();
    return ParseObjectTypeExpression() >>= [&](ir::Node* body) {
      auto ret = New<ir::InterfaceView>(identifier_result.or(Null()),
                                        type_parameters_result.or(Null()),
                                        extends,
                                        body);
      ret->SetInformationForNode(info);
      current_scope()->Declare(ret);
      return Success(ret);
    };
  }

  ReportParseError(cur_token(), YATSC_SOURCEINFO_ARGS) << "'{' expected.";
  return Failed();
}


template <typename UCharInputIterator>
ParseResult Parser<UCharInputIterator>::ParseEnumDeclaration() {
  LOG_PHASE(ParseEnumDeclaration);
  Token info = *cur_token();
  Next();
  auto identifier_result = ParseIdentifier();

  if (!identifier_result) {
    SKIP_IF_ERROR_RECOVERY_ENABLED(false, TokenKind::kLeftBrace, TokenKind::kRightBrace);
  }
    
  if (cur_token()->Is(TokenKind::kLeftBrace)) {
    OpenBraceFound();
    return ParseEnumBody() >>= [&](ir::Node* enum_body) {
      auto ret = New<ir::EnumDeclView>(identifier_result.or(Null()), enum_body);
      ret->SetInformationForNode(&info);
      current_scope()->Declare(ret);
      return Success(ret);
    };
  }

  ReportParseError(cur_token(), YATSC_SOURCEINFO_ARGS) << "'{' expected.";
  return Failed();
}


template <typename UCharInputIterator>
ParseResult Parser<UCharInputIterator>::ParseEnumBody() {
  LOG_PHASE(ParseEnumBody);

  auto ret = New<ir::EnumBodyView>();
  ret->SetInformationForNode(cur_token());
  Next();
    
  if (cur_token()->Is(TokenKind::kRightBrace)) {
    CloseBraceFound();
    BalanceEnclosureIfNotBalanced(cur_token(), TokenKind::kRightBrace, true);
    return Success(ret);
  }
    
  while (1) {
    auto enum_property_result = ParseEnumProperty();
    if (!enum_property_result) {
      SKIP_IF_ERROR_RECOVERY_ENABLED(false, TokenKind::kComma, TokenKind::kLineTerminator, TokenKind::kLineTerminator);
    } else {
      ret->InsertLast(enum_property_result.value());
    }
    
    if (cur_token()->Is(TokenKind::kComma)) {
      Next();
      if (cur_token()->Is(TokenKind::kRightBrace)) {
        CloseBraceFound();
        BalanceEnclosureIfNotBalanced(cur_token(), TokenKind::kRightBrace, true);
        return Success(ret);
      }
    } else if (cur_token()->Is(TokenKind::kRightBrace)) {
      CloseBraceFound();
      BalanceEnclosureIfNotBalanced(cur_token(), TokenKind::kRightParen, true);
      return Success(ret);
    } else {
      ReportParseError(cur_token(), YATSC_SOURCEINFO_ARGS) << "',' or '}' expected.";
      return Failed();
    }
  }
}


template <typename UCharInputIterator>
ParseResult Parser<UCharInputIterator>::ParseEnumProperty() {
  LOG_PHASE(ParseEnumProperty);
  
  return ParsePropertyName() >>= [&](ir::Node* prop_name) {
    if (cur_token()->Is(TokenKind::kAssign)) {
      Next();
      return ParseAssignmentExpression() >>= [&](ir::Node* assignment_expr) {
        return Success(CreateEnumFieldView(prop_name, assignment_expr));
      };
    }
    return Success(CreateEnumFieldView(prop_name, Null()));
  };
}


template <typename UCharInputIterator>
ir::Node* Parser<UCharInputIterator>::CreateEnumFieldView(
    ir::Node* name,
    ir::Node* value) {
  LOG_PHASE(CreateEnumFieldView);
  auto ret = New<ir::EnumFieldView>(name, value);
  ret->SetInformationForNode(name);
  return ret;
}


template <typename UCharInputIterator>
ParseResult Parser<UCharInputIterator>::ParseClassDeclaration() {
  LOG_PHASE(ParseClassDeclaration);

  Token info = *cur_token();
  Next();
  ParseResult type_parameters_result;
  auto identifier_result = ParseIdentifier();

  if (!identifier_result) {
    SKIP_IF_ERROR_RECOVERY_ENABLED(false, TokenKind::kLess, TokenKind::kImplements, TokenKind::kExtends, TokenKind::kLeftBrace);
  } else {
    identifier_result.value()->symbol()->set_type(ir::SymbolType::kClassName); 
  }
    
  if (cur_token()->Is(TokenKind::kLess)) {
    type_parameters_result = ParseTypeParameters();
    if (!type_parameters_result) {
      SKIP_IF_ERROR_RECOVERY_ENABLED(false, TokenKind::kGreater, TokenKind::kImplements, TokenKind::kExtends, TokenKind::kLeftBrace);
    }
    TryConsume(TokenKind::kGreater);
  }

  ParseResult class_bases_result;
  if (cur_token()->OneOf({TokenKind::kImplements, TokenKind::kExtends})) {
    class_bases_result = ParseClassBases();
  }
    
  if (cur_token()->Is(TokenKind::kLeftBrace)) {
    OpenBraceFound();
  } else {
    ReportParseError(cur_token(), YATSC_SOURCEINFO_ARGS) << "'{' expected";
  }
  Next();
  
  auto class_body_result = ParseClassBody();
  if (!class_body_result) {
    SKIP_IF_ERROR_RECOVERY_ENABLED(true, TokenKind::kRightBrace);
  }
    
  auto class_decl = New<ir::ClassDeclView>(identifier_result.or(Null()),
                                           type_parameters_result.or(Null()),
                                           class_bases_result.or(Null()),
                                           class_body_result.or(Null()));
  class_decl->SetInformationForNode(&info);
  current_scope()->Declare(class_decl);
  return Success(class_decl);
}


template <typename UCharInputIterator>
ParseResult Parser<UCharInputIterator>::ParseClassBases() {
  LOG_PHASE(ParseClassBases);
  bool extends_keyword = false;
  
  auto bases = New<ir::ClassBasesView>();
  auto impls = New<ir::ClassImplsView>();
  bases->SetInformationForNode(cur_token());
  
  while (1) {
    if (cur_token()->Is(TokenKind::kExtends)) {
      if (extends_keyword) {
        ReportParseError(cur_token(), YATSC_SOURCEINFO_ARGS) << "class extendable only one class";
      }
      Token info = *cur_token();
      Next();
      extends_keyword = true;
      auto ref_type_result = ParseReferencedType();

      if (!ref_type_result) {
        SKIP_IF_ERROR_RECOVERY_ENABLED(false, TokenKind::kImplements, TokenKind::kLeftBrace);
      } else {
        auto heritage = New<ir::ClassHeritageView>(ref_type_result.value());
        heritage->SetInformationForNode(&info);
        bases->set_base(heritage); 
      }
    } else if (cur_token()->Is(TokenKind::kImplements)) {
      Next();
      while (1) {
        auto ref_type_result = ParseReferencedType();
        if (!ref_type_result) {
          SKIP_IF_ERROR_RECOVERY_ENABLED(false, TokenKind::kComma, TokenKind::kExtends, TokenKind::kLeftBrace);
        } else {
          impls->InsertLast(ref_type_result.value()); 
        }
        
        if (!cur_token()->Is(TokenKind::kComma)) {
          break;
        }
        
        Next();
      }
    } else if (cur_token()->Is(TokenKind::kLeftBrace)) {
      if (impls->size() > 0) {
        bases->set_impls(impls);
      }
      return Success(bases);
    } else {
      ReportParseError(cur_token(), YATSC_SOURCEINFO_ARGS) << "unexpected token";
      return Failed();
    }
  }
}


template <typename UCharInputIterator>
ParseResult Parser<UCharInputIterator>::ParseClassBody() {
  LOG_PHASE(ParseClassBody);
  auto fields = New<ir::ClassFieldListView>();
  
  while (!cur_token()->Is(TokenKind::kRightBrace)) {
    auto class_element_result = ParseClassElement();

    if (!class_element_result) {
      SKIP_IF_ERROR_RECOVERY_ENABLED(false, TokenKind::kPublic, TokenKind::kPrivate, TokenKind::kProtected,
            TokenKind::kStatic, TokenKind::kIdentifier, TokenKind::kRightBrace);
      
      if (cur_token()->Is(TokenKind::kRightBrace)) {
        continue;
      }
      
    } else {
      fields->InsertLast(class_element_result.value());
    }

    if (cur_token()->Is(TokenKind::kEof)) {
      ReportParseError(cur_token(), YATSC_SOURCEINFO_ARGS) << "unexpected end of input.";
      return Failed();
    } else if (IsLineTermination()) {
      ConsumeLineTerminator();
    } else if (!cur_token()->Is(TokenKind::kRightBrace) &&
               !prev_token()->Is(TokenKind::kRightBrace)) {
      ReportParseError(cur_token(), YATSC_SOURCEINFO_ARGS) << "';' expected.";
    }
  }
  
  CloseBraceFound();
  BalanceEnclosureIfNotBalanced(cur_token(), TokenKind::kRightBrace, true);
  return Success(fields);
}


template <typename UCharInputIterator>
ParseResult Parser<UCharInputIterator>::ParseClassElement() {
  LOG_PHASE(ParseClassElement);
  
  if (cur_token()->Is(TokenKind::kLeftBracket)) {
    return ParseIndexSignature();
  }
  
  auto field_modifiers_result = ParseFieldModifiers();
  AccessorType at = ParseAccessor();

  if (Token::IsKeyword(cur_token()->type())) {
    cur_token()->set_type(TokenKind::kIdentifier);
  }
  
  if (cur_token()->Is(TokenKind::kIdentifier) && cur_token()->value()->Equals("constructor")) {
    return ParseConstructorOverloads(field_modifiers_result.value());
  } else if (cur_token()->Is(TokenKind::kMul)) {
    Next();
    return ParseGeneratorMethodOverloads(field_modifiers_result.value());
  } else {
    auto tokens = token_pack();
    Next();
    if (cur_token()->OneOf({TokenKind::kLeftParen, TokenKind::kLess})) {
      if (cur_token()->Is(TokenKind::kLeftParen)) {
        OpenParenFound();
      }
      RestoreTokens(tokens);
      return ParseMemberFunctionOverloads(field_modifiers_result.or(Null()), &at);
    } 
    RestoreTokens(tokens);
    return ParseMemberVariable(field_modifiers_result.or(Null()));
  }

  ReportParseError(cur_token(), YATSC_SOURCEINFO_ARGS) << "unexpected token.";
  return Failed();
}


template <typename UCharInputIterator>
ParseResult Parser<UCharInputIterator>::ParseFieldModifiers() {
  LOG_PHASE(ParseFieldModifiers);
  
  auto mods = New<ir::ClassFieldModifiersView>();
  mods->SetInformationForNode(cur_token());
  
  if (cur_token()->Is(TokenKind::kStatic)) {

    auto field_modifier_result = ParseFieldModifier();
    mods->InsertLast(field_modifier_result.or(Null()));
    
    if (IsAccessLevelModifier(cur_token())) {
      field_modifier_result = ParseFieldModifier();
      mods->InsertLast(field_modifier_result.or(Null()));
    }
    
  } else if (IsAccessLevelModifier(cur_token())) {

    auto field_modifier_result1 = ParseFieldModifier();
    
    if (cur_token()->Is(TokenKind::kStatic)) {
      auto field_modifier_result2 = ParseFieldModifier();
      mods->InsertLast(field_modifier_result2.or(Null()));
      mods->InsertLast(field_modifier_result1.or(Null()));
    } else {
      mods->InsertLast(field_modifier_result1.or(Null()));
    }
  } else {
    auto pub = New<ir::ClassFieldAccessLevelView>(TokenKind::kPublic);
    pub->SetInformationForNode(cur_token());
    mods->InsertLast(pub);
  }
  
  return Success(mods);
}


template <typename UCharInputIterator>
ParseResult Parser<UCharInputIterator>::ParseFieldModifier() {
  LOG_PHASE(ParseFieldModifier);
  
  switch (cur_token()->type()) {
    case TokenKind::kStatic:
    case TokenKind::kPublic:
    case TokenKind::kProtected:
    case TokenKind::kPrivate: {
      auto node = New<ir::ClassFieldAccessLevelView>(cur_token()->type());
      node->SetInformationForNode(cur_token());
      Next();
      return Success(node);
    }
    default:
      ReportParseError(cur_token(), YATSC_SOURCEINFO_ARGS)
        << "class field modifiers is allowed one of 'public', 'private', 'protected', 'static'";
      return Failed();
  }
}


template <typename UCharInputIterator>
void Parser<UCharInputIterator>::ValidateOverload(ir::MemberFunctionDefinitionView* node, ir::Node* overloads) {
  LOG_PHASE(ValidateOverload);
  
  if (overloads->size() > 0) {
    ir::MemberFunctionOverloadView* last = overloads->last_child()->ToMemberFunctionOverloadView();

    if (node->name()->HasNameView()) {
      if (!node->name()->SymbolEquals(last->at(1))) {
        ReportParseError(node->at(1), YATSC_SOURCEINFO_ARGS)
          << "member function overload must have a same name.";
      }
    }
    
    if (!node->modifiers()->Equals(last->modifiers())) {
      ir::Node* target = nullptr;
      if (node->modifiers()) {
        if (node->modifiers()->size() > last->modifiers()->size()) {
          target = node->modifiers()->first_child();
        } else {
          target = last->modifiers()->first_child();
        }
        ReportParseError(target, YATSC_SOURCEINFO_ARGS)
          << "member function overload must have same modifiers.";
      }
    }
  } else {
    if (node->getter()) {
      ir::CallSignatureView* call_sig = node->call_signature()->ToCallSignatureView();
      if (call_sig->param_list()->size() > 0) {
        ReportParseError(call_sig->param_list(), YATSC_SOURCEINFO_ARGS)
          << "the formal parameter of getter function must be empty.";
      }
      
      if (call_sig->return_type()) {
        ir::Node* ret = call_sig->return_type();
        if (ret->HasSimpleTypeExprView()) {
          ir::Node* ret_type = ret->ToSimpleTypeExprView()->type_name();
          if (ret_type->HasNameView()) {
            auto name = ret_type->symbol();
            if (name->Equals("void") || name->Equals("null")) {
              ReportParseError(ret_type, YATSC_SOURCEINFO_ARGS)
                << "getter function must return value.";
            }
          }
        }
      }
    } else if (node->setter()) {
      ir::CallSignatureView* call_sig = node->call_signature()->ToCallSignatureView();
      if (call_sig->param_list()->size() != 1) {
        ReportParseError(call_sig->param_list(), YATSC_SOURCEINFO_ARGS)
          << "the setter function allowed only one parameter.";
      }
      if (call_sig->return_type()) {
        ir::Node* ret = call_sig->return_type();
        if (ret->HasSimpleTypeExprView()) {
          ir::Node* ret_type = ret->ToSimpleTypeExprView()->type_name();
          if (ret_type->HasNameView()) {
            auto name = ret_type->symbol();
            if (!name->Equals("void") && !name->Equals("null")) {
              ReportParseError(ret_type, YATSC_SOURCEINFO_ARGS)
                << "setter function must not return value.";
            }
          }
        }
      }
    }
  }
}


template <typename UCharInputIterator>
ParseResult Parser<UCharInputIterator>::ParseConstructorOverloads(ir::Node* mods) {
  LOG_PHASE(ParseConstructorOverloads);
  auto overloads = New<ir::MemberFunctionOverloadsView>();
  bool first = true;
  
  while (1) {
    if ((cur_token()->Is(TokenKind::kIdentifier) &&
         cur_token()->value()->Equals("constructor")) ||
        IsAccessLevelModifier(cur_token())) {
      
      auto constructor_overload_result = ParseConstructorOverloadOrImplementation(first, mods, overloads);
      CHECK_AST(constructor_overload_result);
      
      if (constructor_overload_result.value()->HasMemberFunctionOverloadView()) {
        overloads->InsertLast(constructor_overload_result.value());
        ValidateOverload(constructor_overload_result.value()->ToMemberFunctionOverloadView(), overloads);
      } else {
        ValidateOverload(constructor_overload_result.value()->ToMemberFunctionView(), overloads);
        return constructor_overload_result;
      }
    } else {
      SourcePosition pos;
      if (overloads->size() > 0) {
        pos = overloads->last_child()->source_position();
      } else {
        pos = prev_token()->source_position();
      }
      ReportParseError(pos, YATSC_SOURCEINFO_ARGS) << "incomplete constructor definition.";
      return Failed();
    }
  }
}


template <typename UCharInputIterator>
ParseResult Parser<UCharInputIterator>::ParseConstructorOverloadOrImplementation(
    bool first,
    ir::Node* mods,
    ir::Node* overloads) {
  
  LOG_PHASE(ParseConstructorOverloadOrImplementation);
  
  if (!first) {
    auto field_modifiers_result = ParseFieldModifiers();
    mods = field_modifiers_result.or(Null());
  }
  
  if (cur_token()->Is(TokenKind::kIdentifier) &&
      cur_token()->value()->Equals("constructor")) {
    
    Token info = *cur_token();
    auto identifier_result = ParseIdentifier();
    auto call_sig_result = ParseCallSignature(true, false, false);

    if (!call_sig_result) {
      SKIP_IF_ERROR_RECOVERY_ENABLED(false, TokenKind::kLeftBrace, TokenKind::kLineTerminator);
    }
    
    ir::Node* ret = nullptr;
    if (cur_token()->Is(TokenKind::kLeftBrace)) {
      OpenBraceFound();
      auto function_body_result = ParseFunctionBody(false);
      CHECK_AST(function_body_result);
      
      ret = New<ir::MemberFunctionView>(mods,
                                        identifier_result.or(Null()),
                                        call_sig_result.or(Null()),
                                        overloads,
                                        function_body_result.or(Null()));
    } else if (overloads) {
      ret = New<ir::MemberFunctionOverloadView>(mods, identifier_result.or(Null()), call_sig_result.or(Null()));
      if (IsLineTermination()) {
        ConsumeLineTerminator();
      } else {
        ReportParseError(cur_token(), YATSC_SOURCEINFO_ARGS) << "';' expected.";
        return Failed();
      }
    } else {
      ReportParseError(&info, YATSC_SOURCEINFO_ARGS) << "invalid constructor definition.";
      return Failed();
    }
    ret->SetInformationForNode(mods);
    return Success(ret);
  }

  ReportParseError(cur_token(), YATSC_SOURCEINFO_ARGS) << "'constructor' expected.";
  return Failed();
}


// Check member function begging token.
template <typename UCharInputIterator>
bool Parser<UCharInputIterator>::IsMemberFunctionOverloadsBegin(Token* info) {
  return info->OneOf({TokenKind::kIdentifier, TokenKind::kStringLiteral, TokenKind::kNumericLiteral, TokenKind::kStatic}) ||
    IsAccessLevelModifier(info) ||
    Token::IsKeyword(info->type());
}


// Parse member function overloads.
// 
// MemberFunctionOverloads
//   MemberFunctionOverload
//   MemberFunctionOverloads MemberFunctionOverload
//
template <typename UCharInputIterator>
ParseResult Parser<UCharInputIterator>::ParseMemberFunctionOverloads(ir::Node* mods, AccessorType* at) {
  LOG_PHASE(ParseMemberFunctionOverloads);
  
  auto overloads = New<ir::MemberFunctionOverloadsView>();
  bool first = true;
  
  while (1) {
    if (IsMemberFunctionOverloadsBegin(cur_token())) {
      // Parse an overload or an implementation.
      auto member_function_result = ParseMemberFunctionOverloadOrImplementation(first, mods, at, overloads);
      CHECK_AST(member_function_result);

      // If function is overload decl,
      // add node to the overloads list.
      if (member_function_result.value()->HasMemberFunctionOverloadView()) {
        ValidateOverload(member_function_result.value()->ToMemberFunctionOverloadView(), overloads);
        overloads->InsertLast(member_function_result.value());
      } else {
        // Else, return node.
        ValidateOverload(member_function_result.value()->ToMemberFunctionView(), overloads);
        return member_function_result;
      }
      first = false;
    } else {
      ReportParseError(cur_token(), YATSC_SOURCEINFO_ARGS) << "incomplete member function definition.";
      return Failed();
    }
  }
}


// Parse a member function overload or implementation.
// The member function overload is like follows.
//
// class Foo {
//   public something(a: string, b: number): void
//   public something(a: number): void
//   public something(a: number, b: string): void {
//     ...
//   }
// }
template <typename UCharInputIterator>
ParseResult Parser<UCharInputIterator>::ParseMemberFunctionOverloadOrImplementation(
    bool first,
    ir::Node* mods,
    AccessorType* acessor_type,
    ir::Node* overloads) {
  LOG_PHASE(ParseMemberFunctionOverloadOrImplementation);
  
  AccessorType at(false, false, Token());

  // If this method is not a first function that is parsed from ParseMemberFunctionOverloads,
  // parse modifiers.
  if (!first) {
    auto field_modifiers_result = ParseFieldModifiers();
    mods = field_modifiers_result.or(Null());
    at = ParseAccessor();
  } else {
    at = *acessor_type;
  }


  if (Token::IsKeyword(cur_token()->type())) {
    cur_token()->set_type(TokenKind::kIdentifier);
  }
  

  // Method must be began an js identifier.

  // Save position.
  Token info = *cur_token();

  auto identifier_result = ParseLiteralPropertyName();
  CHECK_AST(identifier_result);
  auto call_sig_result = ParseCallSignature(false, true, false);
  ir::Node* ret = nullptr;

  // public something(): void {
  // -------------------------^ here
  if (cur_token()->Is(TokenKind::kLeftBrace)) {
    OpenBraceFound();
    auto function_body_result = ParseFunctionBody(false);
    CHECK_AST(function_body_result);
    ret = New<ir::MemberFunctionView>(at.getter, at.setter, false,
                                      mods,
                                      identifier_result.or(Null()),
                                      call_sig_result.or(Null()),
                                      overloads,
                                      function_body_result.or(Null()));
  } else if (overloads) {
      
    // Getter and setter is not allowed to overload function declaration.
    if (at.getter || at.setter) {
      ReportParseError(&info, YATSC_SOURCEINFO_ARGS)
        << "overload is not allowed to getter and setter.";
    }
    ret = New<ir::MemberFunctionOverloadView>(mods, identifier_result.or(Null()), call_sig_result.or(Null()));
      
    if (IsLineTermination()) {
      ConsumeLineTerminator();
    } else {
      ReportParseError(cur_token(), YATSC_SOURCEINFO_ARGS)
        << "';' expected";
      return Failed();
    }
  } else {
    ReportParseError(&info, YATSC_SOURCEINFO_ARGS)
      << "invalid member function definition";
    return Failed();
  }
  ret->SetInformationForNode(mods);
  return Success(ret);
}


template <typename UCharInputIterator>
ParseResult Parser<UCharInputIterator>::ParseGeneratorMethodOverloads(ir::Node* mods) {
  LOG_PHASE(ParseGeneratorMethodOverloads);
  auto overloads = New<ir::MemberFunctionOverloadsView>();
  bool first = true;
  
  while (1) {
    if (IsMemberFunctionOverloadsBegin(cur_token())) {      
      auto generator_result = ParseGeneratorMethodOverloadOrImplementation(first, mods, overloads);
      CHECK_AST(generator_result);
      
      if (generator_result.value()->HasMemberFunctionOverloadView()) {
        ValidateOverload(generator_result.value()->ToMemberFunctionOverloadView(), overloads);
        overloads->InsertLast(generator_result.value());
      } else {
        ValidateOverload(generator_result.value()->ToMemberFunctionView(), overloads);
        return generator_result;
      }
      first = false;
    } else {
      ReportParseError(cur_token(), YATSC_SOURCEINFO_ARGS)
        << "incomplete member function definition";
      return Failed();
    }
  }
}


template <typename UCharInputIterator>
ParseResult Parser<UCharInputIterator>::ParseGeneratorMethodOverloadOrImplementation(
    bool first,
    ir::Node* mods,
    ir::Node* overloads) {
  
  LOG_PHASE(ParseGeneratorMethodOverloadOrImplementation);
  
  if (!first) {
    auto field_modifiers_result = ParseFieldModifiers();
    mods = field_modifiers_result.or(Null());
  }

  Token info = *cur_token();
  if (cur_token()->Is(TokenKind::kMul)) {
    Next();
  } else if (!first) {
    ReportParseError(cur_token(), YATSC_SOURCEINFO_ARGS)
      << "'*' expected.";
  }

  
  if (Token::IsKeyword(cur_token()->type())) {
    cur_token()->set_type(TokenKind::kIdentifier);
  }
  
    
  auto identifier_result = ParseLiteralPropertyName();
  CHECK_AST(identifier_result);
  auto call_sig_result = ParseCallSignature(false, true, false);
  ir::Node* ret = nullptr;
      
  if (cur_token()->Is(TokenKind::kLeftBrace)) {
    OpenBraceFound();
    auto function_body_result = ParseFunctionBody(true);
    CHECK_AST(function_body_result);
    ret = New<ir::MemberFunctionView>(mods,
                                      identifier_result.or(Null()),
                                      call_sig_result.or(Null()),
                                      overloads,
                                      function_body_result.or(Null()));
  } else if (overloads) {
    ret = New<ir::MemberFunctionOverloadView>(mods, identifier_result.or(Null()), call_sig_result.or(Null()));
    if (IsLineTermination()) {
      ConsumeLineTerminator();
    } else {
      ReportParseError(cur_token(), YATSC_SOURCEINFO_ARGS)
        << "';' expected.";
      return Failed();
    }
  } else {
    ReportParseError(&info, YATSC_SOURCEINFO_ARGS)
      << "invalid member function definition";
    return Failed();
  }
      
  ret->SetInformationForNode(mods);
  return Success(ret);
}


template <typename UCharInputIterator>
ParseResult Parser<UCharInputIterator>::ParseMemberVariable(ir::Node* mods) {
  LOG_PHASE(ParseMemberVariable);

  if (Token::IsKeyword(cur_token()->type())) {
    cur_token()->set_type(TokenKind::kIdentifier);
  }
  

  auto identifier_result = ParseLiteralPropertyName();
  CHECK_AST(identifier_result);
  ParseResult value_result;
  ParseResult type_result;
    
  if (cur_token()->Is(TokenKind::kColon)) {
    Next();
    type_result = ParseTypeExpression();
  }
    
  if (cur_token()->Is(TokenKind::kAssign)) {
    Next();
    value_result = ParseExpression();
  }
    
  auto member_variable = New<ir::MemberVariableView>(mods,
                                                     identifier_result.or(Null()),
                                                     type_result.or(Null()),
                                                     value_result.or(Null()));
  member_variable->SetInformationForNode(mods);
  return Success(member_variable);
}


template <typename UCharInputIterator>
ParseResult Parser<UCharInputIterator>::ParseFunctionOverloads(bool declaration, bool is_export) {
  auto overloads = New<ir::FunctionOverloadsView>();
  bool first = true;
  
  while (1) {
    if (is_export && !first && cur_token()->Is(TokenKind::kExport)) {
      Next();
    } else if (is_export && !first) {
      ReportParseError(cur_token(), YATSC_SOURCEINFO_ARGS)
        << "export expected.";
    }
    
    first = false;
    
    if (cur_token()->Is(TokenKind::kFunction)) {
      auto function_overloads_result = ParseFunctionOverloadOrImplementation(overloads, declaration);
      CHECK_AST(function_overloads_result);
      
      if (function_overloads_result.value()->HasFunctionOverloadView()) {
        ir::FunctionOverloadView* overload = function_overloads_result.value()->ToFunctionOverloadView();
        if (overloads->size() > 0) {
          ir::FunctionOverloadView* last = overloads->last_child()->ToFunctionOverloadView();
          if (!last->name()) {
            ReportParseError(overload, YATSC_SOURCEINFO_ARGS)
              << "function overload must have a name.";
          } else if (!last->name()->SymbolEquals(overload->name())) {
            ReportParseError(overload->name(), YATSC_SOURCEINFO_ARGS)
              << "function overload must have a same name";
          }

          if (last->generator() != overload->generator()) {
            ReportParseError(overload->name(), YATSC_SOURCEINFO_ARGS)
              << "generator function can only overloaded by generator function.";
          }
        }
        overloads->InsertLast(function_overloads_result.value());
      } else {
        return function_overloads_result;
      }
    } else {
      ReportParseError(cur_token(), YATSC_SOURCEINFO_ARGS)
        << "incomplete function definition";
      return Failed();
    }
  }
}


template <typename UCharInputIterator>
ParseResult Parser<UCharInputIterator>::ParseFunctionOverloadOrImplementation(ir::Node* overloads, bool declaration) {
  LOG_PHASE(ParseFunctionOverloadOrImplementation);
  if (cur_token()->Is(TokenKind::kFunction)) {
    bool generator = false;
    
    Token info = *cur_token();
    Next();
    if (cur_token()->Is(TokenKind::kMul)) {
      generator = true;
      Next();
    }
    
    ParseResult identifier_result;
    
    if (cur_token()->Is(TokenKind::kIdentifier)) {
      identifier_result = ParseIdentifier();
    }

    if (declaration && !identifier_result) {
      ReportParseError(cur_token(), YATSC_SOURCEINFO_ARGS)
        << "function name required.";
    }
    
    auto call_sig_result = ParseCallSignature(false, true, false);
    ir::Node* ret = nullptr;
    if (cur_token()->Is(TokenKind::kLeftBrace)) {
      OpenBraceFound();
      auto function_body_result = ParseFunctionBody(generator);
      ret = New<ir::FunctionView>(overloads,
                                  identifier_result.or(Null()),
                                  call_sig_result.or(Null()),
                                  function_body_result.or(Null()));
    } else if (overloads) {
      ret = New<ir::FunctionOverloadView>(generator,
                                          identifier_result.or(Null()),
                                          call_sig_result.or(Null()));
      if (IsLineTermination()) {
        ConsumeLineTerminator();
      } else {
        ReportParseError(cur_token(), YATSC_SOURCEINFO_ARGS)
          << "';' expected";
        return Failed();
      }
    } else {
      ReportParseError(cur_token(), YATSC_SOURCEINFO_ARGS)
        << "invalid function definition.";
      if (IsLineTermination()) {
        ConsumeLineTerminator();
      }
      return Failed();
    }
    ret->SetInformationForNode(&info);
    return Success(ret);
  }

  ReportParseError(cur_token(), YATSC_SOURCEINFO_ARGS)
    << "'function' expected.";
  return Failed();
}


template <typename UCharInputIterator>
ParseResult Parser<UCharInputIterator>::ParseParameterList(bool accesslevel_allowed) {
  LOG_PHASE(ParseParameterList);

  ir::ParamList* param_list = New<ir::ParamList>();
  param_list->SetInformationForNode(cur_token());
  Next();

  if (cur_token()->Is(TokenKind::kRightParen)) {
    CloseParenFound();
    BalanceEnclosureIfNotBalanced(cur_token(), TokenKind::kRightParen, true);
    return Success(param_list);
  }
    
  bool has_rest = false;
    
  while (1) {
    if (has_rest) {
      ReportParseError(param_list->last_child(), YATSC_SOURCEINFO_ARGS)
        << "Rest parameter must be at the end of the parameters";
    }
    
    if (cur_token()->Is(TokenKind::kIdentifier) ||
        IsAccessLevelModifier(cur_token())) {
      
      auto parameter_result = ParseParameter(false, accesslevel_allowed);
      if (!parameter_result) {
        SKIP_IF_ERROR_RECOVERY_ENABLED(false, TokenKind::kComma, TokenKind::kRightParen);
      } else {
        param_list->InsertLast(parameter_result.value());
      }
      
    } else if (cur_token()->Is(TokenKind::kRest)) {
      has_rest = true;
      auto ret = ParseRestParameter(accesslevel_allowed);
      if (!ret) {
        SKIP_IF_ERROR_RECOVERY_ENABLED(false, TokenKind::kComma, TokenKind::kRightParen);
      } else {
        param_list->InsertLast(ret.value());
      }
      
    } else {
      ReportParseError(cur_token(), YATSC_SOURCEINFO_ARGS)
        << "unexpected token in formal parameter list.";
      SKIP_IF_ERROR_RECOVERY_ENABLED(false, TokenKind::kComma, TokenKind::kRightParen);
    }

    if (cur_token()->Is(TokenKind::kComma)) {
      Next();
    } else if (cur_token()->Is(TokenKind::kRightParen)) {
      CloseParenFound();
      BalanceEnclosureIfNotBalanced(cur_token(), TokenKind::kRightParen, true);
      return Success(param_list);
    } else {
      ReportParseError(cur_token(), YATSC_SOURCEINFO_ARGS)
        << "')' or ',' expected in parameter list.";
      return Failed();
    }
  }
}


template <typename UCharInputIterator>
ParseResult Parser<UCharInputIterator>::ParseRestParameter(bool accesslevel_allowed) {
  Token token = (*cur_token());
  Next();
  return ParseParameter(true, accesslevel_allowed) >>= [&](ir::Node* parameter) {
    ir::Node* node = New<ir::RestParamView>(parameter);
    node->SetInformationForNode(&token);
    return Success(node);
  };
}


template <typename UCharInputIterator>
ParseResult Parser<UCharInputIterator>::ParseParameter(bool rest, bool accesslevel_allowed) {
  LOG_PHASE(ParseParameter);
  ir::Node* access_level = nullptr;
  
  if (IsAccessLevelModifier(cur_token())) {
    if (accesslevel_allowed) {
      access_level = New<ir::ClassFieldAccessLevelView>(cur_token()->type());
      access_level->SetInformationForNode(cur_token());
    } else {
      ReportParseError(cur_token(), YATSC_SOURCEINFO_ARGS)
        << "'private' or 'public' not allowed here";
    }
    Next();
  }
  
  if (cur_token()->Is(TokenKind::kIdentifier)) {
    ir::ParameterView* pv = New<ir::ParameterView>();
    pv->SetInformationForNode(cur_token());
    ir::NameView* nv = New<ir::NameView>(NewSymbol(ir::SymbolType::kVariableName, cur_token()->value()));
    nv->SetInformationForNode(cur_token());
    pv->set_access_level(access_level);
    pv->set_name(nv);
    Next();
    
    if (cur_token()->Is(TokenKind::kQuestionMark)) {
      if (rest) {
        ReportParseError(cur_token(), YATSC_SOURCEINFO_ARGS)
          << "optional parameter not allowed in rest parameter.";
      }
      Next();
      pv->set_optional(true);
    }
    
    if (cur_token()->Is(TokenKind::kColon)) {
      Next();
      
      if (cur_token()->Is(TokenKind::kStringLiteral)) {
        auto string_literal_result = ParseStringLiteral();
        pv->set_type_expr(string_literal_result.or(Null()));
      } else {
        auto type_expr_result = ParseTypeExpression();
        pv->set_type_expr(type_expr_result.or(Null()));
      }
    }
    
    if (cur_token()->Is(TokenKind::kAssign)) {
      if (rest) {
        ReportParseError(cur_token(), YATSC_SOURCEINFO_ARGS)
          << "default parameter not allowed in rest parameter.";
      }
      Next();

      auto assignment_expr_result = ParseAssignmentExpression();
      
      CHECK_AST(assignment_expr_result);
      pv->set_value(assignment_expr_result.value());
    }
    return Success(pv);
  } else if (cur_token()->IsKeyword()) {
    ReportParseError(cur_token(), YATSC_SOURCEINFO_ARGS)
      << "keyword '" << cur_token()->ToString() << "' is not allowed in formal parameter list.";
    return Failed();
  }

  ReportParseError(cur_token(), YATSC_SOURCEINFO_ARGS)
    << "identifier expected.";
  return Failed();
}


template <typename UCharInputIterator>
ParseResult Parser<UCharInputIterator>::ParseFunctionBody(bool generator) {
  LOG_PHASE(ParseFunctionBody);

  Handle<ir::Scope> scope = NewScope();
  set_current_scope(scope);
  auto block = New<ir::BlockView>(scope);
  block->SetInformationForNode(cur_token());
  Next();
    
  while (1) {
    if (cur_token()->Is(TokenKind::kRightBrace)) {
      CloseBraceFound();
      BalanceEnclosureIfNotBalanced(cur_token(), TokenKind::kRightBrace, true);
      break;
    } else if (cur_token()->Is(TokenKind::kIllegal)) {
      ReportParseError(cur_token(), YATSC_SOURCEINFO_ARGS)
        << "unexpected token.";
      SkipIllegalTokens();
    } else if (cur_token()->Is(TokenKind::kEof)) {
      UnexpectedEndOfInput(cur_token(), YATSC_SOURCEINFO_ARGS);
      return Failed();
    }

    if (generator) {
      state_.EnterGenerator();
    } else {
      state_.EnterFunction();
    }

    auto stmt_list_result = ParseStatementListItem();
    
    if (generator) {
      state_.ExitGenerator();
    } else {
      state_.ExitFunction();
    }
    
    if (!stmt_list_result) {
      SKIP_IF_ERROR_RECOVERY_ENABLED(false, TokenKind::kRightBrace);
    } else {
      block->InsertLast(stmt_list_result.value());
    }
    
    if (IsLineTermination()) {
      ConsumeLineTerminator();
    }
  }
  set_current_scope(scope->parent_scope());
  return Success(block);
}


template <typename UCharInputIterator>
ParseResult Parser<UCharInputIterator>::ParseEmptyStatement() {
  Next();
  auto node = New<ir::Empty>();
  node->SetInformationForNode(cur_token());
  return Success(node);
}
}
