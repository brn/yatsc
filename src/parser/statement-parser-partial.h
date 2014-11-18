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

template <typename UCharInputIterator>
ParseResult Parser<UCharInputIterator>::ParseStatement(bool yield, bool has_return, bool breakable, bool continuable) {
  LOG_PHASE(ParseStatement);
  ParseResult parse_result;
  
  switch (cur_token()->type()) {
    case TokenKind::kLeftBrace:
      parse_result = ParseBlockStatement(yield, has_return, breakable, continuable);
      break;

    case TokenKind::kLineTerminator:
      parse_result = ParseEmptyStatement();
      break;

    case TokenKind::kIf:
      parse_result = ParseIfStatement(yield, has_return, breakable, continuable);
      break;

    case TokenKind::kFor:
      parse_result = ParseForStatement(yield, has_return);
      break;

    case TokenKind::kWhile:
      parse_result = ParseWhileStatement(yield, has_return);
      break;

    case TokenKind::kDo:
      parse_result = ParseDoWhileStatement(yield, has_return);
      break;

    case TokenKind::kContinue:
      if (!continuable) {
        SYNTAX_ERROR("'continue' only allowed in loops", cur_token());
      }
      parse_result = ParseContinueStatement(yield);
      break;

    case TokenKind::kBreak: {
      if (!breakable) {
        SYNTAX_ERROR("'break' not allowed here", cur_token());
      }
      parse_result = ParseBreakStatement(yield);
      if (IsLineTermination()) {
        ConsumeLineTerminator();
      } else {
        SYNTAX_ERROR("';' expected", cur_token());
      }
      break;
    }

    case TokenKind::kReturn:
      if (!has_return) {
        SYNTAX_ERROR("'return' statement only allowed in function", cur_token());
      }
      parse_result = ParseReturnStatement(yield);
      break;

    case TokenKind::kWith:
      parse_result = ParseWithStatement(yield, has_return, breakable, continuable);
      break;

    case TokenKind::kSwitch:
      parse_result = ParseSwitchStatement(yield, has_return, continuable);
      break;

    case TokenKind::kThrow:
      parse_result = ParseThrowStatement();
      break;

    case TokenKind::kTry:
      parse_result = ParseTryStatement(yield, has_return, breakable, continuable);
      break;

    case TokenKind::kDebugger:
      parse_result = ParseDebuggerStatement();
      break;

    case TokenKind::kVar: {
      auto variable_stmt_result = ParseVariableStatement(true, yield);
      if (IsLineTermination()) {
        ConsumeLineTerminator();
      } else {
        SYNTAX_ERROR("';' expected", cur_token());
      }
      return variable_stmt_result;
    }

    case TokenKind::kEof:
      SYNTAX_ERROR("Unexpected end of input", cur_token());
      
    default: {
      if (cur_token()->type() == TokenKind::kIdentifier) {
        RecordedParserState rps = parser_state();
        Next();
        if (cur_token()->type() == TokenKind::kColon) {
          RestoreParserState(rps);
          return ParseLabelledStatement(yield, has_return, breakable, continuable);
        }
        RestoreParserState(rps);
      }
      Token info = *cur_token();
      parse_result = ParseExpression(true, yield);
      CHECK_AST(parse_result);
      parse_result = Success(New<ir::StatementView>(parse_result.value()));
      parse_result.value()->SetInformationForNode(&info);
    }
  }
  
  return parse_result;
}


template <typename UCharInputIterator>
ParseResult Parser<UCharInputIterator>::ParseDeclaration(bool error, bool yield, bool has_default) {
  switch (cur_token()->type()) {
    case TokenKind::kFunction: {
      auto function_overloads_result = ParseFunctionOverloads(yield, has_default, true, false);
      CHECK_AST(function_overloads_result);
      current_scope()->Declare(function_overloads_result.value());
      return function_overloads_result;
    }
    case TokenKind::kClass:
      return ParseClassDeclaration(yield, has_default);
    case TokenKind::kEnum:
      return ParseEnumDeclaration(yield, has_default);
    case TokenKind::kInterface:
      return ParseInterfaceDeclaration();
    case TokenKind::kLet:
    case TokenKind::kConst: {
      auto lexical_decl_result = ParseLexicalDeclaration(true, yield);
      if (IsLineTermination()) {
        ConsumeLineTerminator();
      } else {
        SYNTAX_ERROR("';' expected", cur_token());
      }
      return lexical_decl_result;
    }
    default:
      if (!error) {
        return Failed();
      }
      SYNTAX_ERROR("unexpected token", cur_token());
  }
}


template <typename UCharInputIterator>
ParseResult Parser<UCharInputIterator>::ParseBlockStatement(bool yield, bool has_return, bool breakable, bool continuable) {
  LOG_PHASE(ParseBlockStatement);
  Handle<ir::Scope> scope = NewScope();
  set_current_scope(scope);
  auto block_view = New<ir::BlockView>(scope);
  block_view->SetInformationForNode(cur_token());

  if (cur_token()->type() == TokenKind::kLeftBrace) {
    Next();
    bool success = true;
    while (1) {
      if (cur_token()->type() == TokenKind::kRightBrace) {
        Next();
        break;
      } else if (cur_token()->type() == TokenKind::kEof) {
        SYNTAX_ERROR("Unexpected end of input.", cur_token());
      } else {
        auto statement_list_result = ParseStatementListItem(yield, has_return, breakable, continuable);
        SKIP_TOKEN_OR(statement_list_result, success, TokenKind::kLeftBrace) {
          block_view->InsertLast(statement_list_result.value());
        }
      }
    }
    set_current_scope(scope->parent_scope());
    return Success(block_view);
  }
  SYNTAX_ERROR("'{' expected", cur_token());
}


template <typename UCharInputIterator>
ParseResult Parser<UCharInputIterator>::ParseStatementListItem(bool yield, bool has_return, bool breakable, bool continuable) {
  LOG_PHASE(ParseStatementListItem);
  auto decl_result = ParseDeclaration(false, yield, false);
  if (!decl_result) {
    return ParseStatement(yield, has_return, breakable, continuable);
  }
  return decl_result;
}


// lexical_declaration[in, yield]
//   : let_or_const binding_list[?in, ?yield]
//   ;
// let_or_const
//   : 'let'
//   | 'const'
//   ;
template <typename UCharInputIterator>
ParseResult Parser<UCharInputIterator>::ParseLexicalDeclaration(bool in, bool yield) {
  LOG_PHASE(ParseLexicalDeclaration);
  bool has_const = cur_token()->type() == TokenKind::kConst;
  Handle<ir::LexicalDeclView> lexical_decl = New<ir::LexicalDeclView>(cur_token()->type());
  lexical_decl->SetInformationForNode(cur_token());
  Next();
  bool success = false;
  while (1) {
    auto lexical_decl_result = ParseLexicalBinding(has_const, in, yield);
    SKIP_TOKEN_OR(lexical_decl_result, success, TokenKind::kLineTerminator) {
      lexical_decl->InsertLast(lexical_decl_result.value());
    }
    if (cur_token()->type() == TokenKind::kComma) {
      Next();
    } else {
      break;
    }
  }
  return Success(lexical_decl);
}


// binding_list[in, yield]
//   : lexical_binding[?in, ?yield]
//   | binding_list[?in, ?yield] ',' lexical_binding[?in, ?yield]
//   ;
// lexical_binding[in, yield]
//   : binding_identifier[?yield] initializer__opt[?in, ?yield]
//   | binding_pattern[?yield] initializer[?in, ?yield]
//   ;
template <typename UCharInputIterator>
ParseResult Parser<UCharInputIterator>::ParseLexicalBinding(bool const_decl, bool in, bool yield) {
  LOG_PHASE(ParseLexicalBinding);
  ParseResult value_result;
  ParseResult lhs_result;
  if (cur_token()->type() == TokenKind::kIdentifier) {
    lhs_result = ParseBindingIdentifier(false, in, yield);
  } else {
    lhs_result = ParseBindingPattern(yield, false);
  }

  CHECK_AST(lhs_result);

  if (!lhs_result.value()->IsValidLhs()) {
    SYNTAX_ERROR("left hand side of lexical binding is invalid", cur_token());
  }

  ParseResult type_expr_result;
  if (cur_token()->type() == TokenKind::kColon) {
    Next();
    type_expr_result = ParseTypeExpression();
    CHECK_AST(type_expr_result);
  }
  
  if (cur_token()->type() == TokenKind::kAssign) {
    Next();
    value_result = ParseAssignmentExpression(in, yield);
    CHECK_AST(value_result);
  } else if (const_decl) {
    SYNTAX_ERROR("const declaration must have an initializer", cur_token());
  }

  Handle<ir::Node> ret = New<ir::VariableView>(lhs_result.value(), value_result.value(), type_expr_result.value());
  ret->SetInformationForNode(lhs_result.value());
  current_scope()->Declare(ret);
  return Success(ret);
}


// binding_identifier[default, yield]
//   : [+default] 'default'
//   | [~yield] 'yield' identifier
//   | identifier
//   ;
template <typename UCharInputIterator>
ParseResult Parser<UCharInputIterator>::ParseBindingIdentifier(bool default_allowed, bool in, bool yield) {
  LOG_PHASE(ParseBindingIdentifier);
  Handle<ir::Node> ret;
  if (cur_token()->type() == TokenKind::kDefault) {
    if (!default_allowed) {
      SYNTAX_ERROR("'default' keyword not allowed here", cur_token());
    }
    ret = New<ir::DefaultView>();
  } else if (cur_token()->type() == TokenKind::kYield) {
    ret = New<ir::YieldView>(false, ir::Node::Null());
  } else if (cur_token()->type() == TokenKind::kIdentifier) {
    ret = New<ir::NameView>(NewSymbol(ir::SymbolType::kVariableName, cur_token()->value()));
  } else {
    SYNTAX_ERROR("'default', 'yield' or 'identifier' expected", cur_token());
  }
  
  ret->SetInformationForNode(cur_token());
  Next();

  return Success(ret);
}


// binding_pattern[yield,generator_parameter]
//   : object_binding_pattern[?yield,?generator_parameter]
//   | array_binding_pattern[?yield,?generator_parameter]
//   ;
template <typename UCharInputIterator>
ParseResult Parser<UCharInputIterator>::ParseBindingPattern(bool yield, bool generator_parameter) {
  LOG_PHASE(ParseBindingPattern);
  switch (cur_token()->type()) {
    case TokenKind::kLeftBrace:
      return ParseObjectBindingPattern(yield, generator_parameter);
    case TokenKind::kLeftBracket:
      return ParseArrayBindingPattern(yield, generator_parameter);
    default:
      SYNTAX_ERROR("'[' or '{' expected", cur_token());
  }
}


// object_binding_pattern[yield,generator_parameter]
//   : '{' '}'
//   | '{' binding_property_list[?yield,?generator_parameter] '}'
//   | '{' binding_property_list[?yield,?generator_parameter] ',' '}'
//   ;
// binding_property_list[yield,generator_parameter]
//   : binding_property[?yield, ?generator_parameter]
//   | binding_property_list[?yield, ?generator_parameter] ',' binding_property[?yield, ?generator_parameter]
//   ;
template <typename UCharInputIterator>
ParseResult Parser<UCharInputIterator>::ParseObjectBindingPattern(bool yield, bool generator_parameter) {
  LOG_PHASE(ParseObjectBindingPattern);
  Handle<ir::Node> binding_prop_list = New<ir::BindingPropListView>();
  binding_prop_list->SetInformationForNode(cur_token());
  Next();

  bool success = true;
    
  while (1) {
    auto binding_prop_result = ParseBindingProperty(yield, generator_parameter);
      
    SKIP_TOKEN_OR(binding_prop_result, success, TokenKind::kRightBrace) {
      binding_prop_list->InsertLast(binding_prop_result.value());
    }
      
    switch (cur_token()->type()) {
      case TokenKind::kComma: {
        Next();
        break;
      }
      case TokenKind::kRightBrace: {
        Next();
        return Success(binding_prop_list);
      }
      default:
        SYNTAX_ERROR("unexpected token", cur_token());
    }
  }
}


// array_binding_pattern[yield,generator_parameter]
//   : '[' elision__opt binding_rest_element__opt[?yield, ?generator_parameter] ']'
//   | '[' binding_element_list[?yield, ?generator_parameter] ']'
//   | '[' binding_element_list[?yield, ?generator_parameter] ',' elision__opt binding_rest_element__opt[?yield, ?generator_parameter] ']'
//   ;
// binding_element_list[yield,generator_parameter]
//   : binding_elision_element[?yield, ?generator_parameter]
//   | binding_element_list[?yield, ?generator_parameter] ',' binding_elision_element[?yield, ?generator_parameter]
//   ;
// binding_elision_element[yield,generator_parameter]
//   : elision__opt bindingelement[?yield, ?generator_parameter]
//   ;
// binding_rest_element[yield, generator_parameter]
//   : [+generator_parameter] '...' binding_identifier[yield]
//   | [~generator_parameter] '...' binding_identifier[?yield]
//   ;
template <typename UCharInputIterator>
ParseResult Parser<UCharInputIterator>::ParseArrayBindingPattern(bool yield, bool generator_parameter) {
  LOG_PHASE(ParseArrayBindingPattern);
    
  Handle<ir::Node> binding_array = New<ir::BindingArrayView>();
  binding_array->SetInformationForNode(cur_token());
  Next();

  bool exit = false;
  bool success = true;
    
  while (1) {
    if (cur_token()->type() == TokenKind::kComma) {
      Next();
      binding_array->InsertLast(ir::Node::Null());
    }
    if (cur_token()->type() == TokenKind::kRest) {
      Handle<ir::RestParamView> rest = New<ir::RestParamView>();
      rest->SetInformationForNode(cur_token());
      Next();
      auto binding_identifier_result = ParseBindingIdentifier(false, true, yield);
      SKIP_TOKEN_OR(binding_identifier_result, success, TokenKind::kRightBracket) {
        rest->set_parameter(binding_identifier_result.value());
        binding_array->InsertLast(rest);
        exit = true;
      }
    } else {
      auto binding_elem_result = ParseBindingElement(yield, generator_parameter);
        
      SKIP_TOKEN_OR(binding_elem_result, success, TokenKind::kRightBracket) {
        ParseResult assignment_expr_result;
        if (cur_token()->type() == TokenKind::kAssign) {
          assignment_expr_result = ParseAssignmentExpression(true, yield);
        }
        auto ret = New<ir::BindingElementView>(ir::Node::Null(),
                                               binding_elem_result.value(),
                                               assignment_expr_result.value());
        ret->SetInformationForNode(binding_elem_result.value());
        binding_array->InsertLast(ret);
      }
    }

    if (cur_token()->type() == TokenKind::kRightBracket) {
      Next();
      break;
    } else if (exit) {
      SYNTAX_ERROR("spread binding must be end of bindings", cur_token());
    } else if (cur_token()->type() == TokenKind::kComma) {
      Next();
    } else {
      SYNTAX_ERROR("unexpected token.", cur_token());
    }
  }
  return Success(binding_array);
}


// binding_property[yield,generator_parameter]
//   : single_name_binding [?yield, ?generator_parameter]
//   : property_name[?yield, ?generator_parameter] ':' binding_element[?yield, ?generator_parameter]
//   ;
// single_name_binding[yield,generator_parameter]
//   : [+generator_parameter] binding_identifier[yield] initializer__opt[in]
//   | [~generator_parameter] binding_identifier[?yield] initializer__opt[in, ?yield]
//   ;
template <typename UCharInputIterator>
ParseResult Parser<UCharInputIterator>::ParseBindingProperty(bool yield, bool generator_parameter) {
  LOG_PHASE(ParseBindingProperty);
  ParseResult key_result;
  ParseResult elem_result;
  ParseResult init_result;

  if (cur_token()->type() == TokenKind::kIdentifier) {
    key_result = ParseIdentifier();
    CHECK_AST(key_result);
  } else {
    SYNTAX_ERROR("'identifier' expected", cur_token());
  }
  
  if (cur_token()->type() == TokenKind::kColon) {
    Next();
    elem_result = ParseBindingElement(yield, generator_parameter);
    CHECK_AST(elem_result);
  }

  if (cur_token()->type() == TokenKind::kAssign) {
    Next();
    init_result = ParseAssignmentExpression(true, yield);
    CHECK_AST(init_result);
  }
  Handle<ir::Node> ret = New<ir::BindingElementView>(key_result.value(), elem_result.value(), init_result.value());
  ret->SetInformationForNode(key_result.value());
  return Success(ret);
}


// binding_element[yield, generator_parameter ]
//   : single_name_binding[?yield, ?generator_parameter]
//   | [+generator_parameter] binding_pattern[?yield,generator_parameter] initializer__opt[in]
//   | [~generator_parameter] binding_pattern[?yield] initializer__opt[in, ?yield]
//   ;
template <typename UCharInputIterator>
ParseResult Parser<UCharInputIterator>::ParseBindingElement(bool yield, bool generator_parameter) {
  LOG_PHASE(ParseBindingElement);
  switch (cur_token()->type()) {
    case TokenKind::kIdentifier:
    case TokenKind::kYield:
    case TokenKind::kDefault:
      return ParseBindingIdentifier(false, false, yield);
    default:
      return ParseBindingPattern(yield, generator_parameter);
  }
}


// variable_statement[yield]
//  : 'var' variable_declaration_list[in, ?yield]
//  ;
// variable_declaration_list[in, yield]
//  : variable_declaration[?in, ?yield]
//  | variable_declaration_list[?in, ?yield] ',' variable_declaration[?in, ?yield]
//  ;
template <typename UCharInputIterator>
ParseResult Parser<UCharInputIterator>::ParseVariableStatement(bool in, bool yield) {
  Next();
  Handle<ir::VariableDeclView> vars = New<ir::VariableDeclView>();
  bool success = true;
    
  while (1) {
    auto variable_decl_result = ParseVariableDeclaration(in, yield);
    SKIP_TOKEN_OR(variable_decl_result, success, TokenKind::kLineTerminator) {
      vars->InsertLast(variable_decl_result.value());
    }
    if (cur_token()->type() == TokenKind::kComma) {
      Next();
    } else {
      break;
    }
  }
    
  return Success(vars);

}


// variable_declaration[in, yield]
//  : binding_identifier[?yield] initializer[?in, ?yield]__opt
//  | binding_pattern[yield] initializer[?in, ?yield]
//  ;
template <typename UCharInputIterator>
ParseResult Parser<UCharInputIterator>::ParseVariableDeclaration(bool in, bool yield) {
  LOG_PHASE(ParseVariableDeclaration);
  ParseResult value_result;
  ParseResult lhs_result;
  if (cur_token()->type() == TokenKind::kIdentifier) {
    lhs_result = ParseBindingIdentifier(false, in, yield);
  } else {
    lhs_result = ParseBindingPattern(yield, false);
  }

  CHECK_AST(lhs_result);

  if (!lhs_result.value()->IsValidLhs()) {
    SYNTAX_ERROR("left hand side of variable declaration is invalid", cur_token());
  }

  ParseResult type_expr_result;
  if (cur_token()->type() == TokenKind::kColon) {
    Next();
    type_expr_result = ParseTypeExpression();
    CHECK_AST(type_expr_result);
  }
  
  if (cur_token()->type() == TokenKind::kAssign) {
    Next();
    value_result = ParseAssignmentExpression(in, yield);
    CHECK_AST(value_result);
  }

  Handle<ir::Node> ret = New<ir::VariableView>(lhs_result.value(), value_result.value(), type_expr_result.value());
  ret->SetInformationForNode(lhs_result.value());
  current_scope()->Declare(ret);
  
  return Success(ret);
}


// if_statement[yield, return]
//   : 'if' '(' expression[in, ?yield] ')' statement[?yield, ?return] 'else' statement[?yield, ?return]
//   | 'if' '(' expression[in, ?yield] ')' statement[?yield, ?return]
//   ;
template <typename UCharInputIterator>
ParseResult Parser<UCharInputIterator>::ParseIfStatement(bool yield, bool has_return, bool breakable, bool continuable) {
  LOG_PHASE(ParseIfStatement);
  Token info = *cur_token();
  Next();
  if (cur_token()->type() == TokenKind::kLeftParen) {
    Next();
    auto expr_result = ParseExpression(true, yield);
    CHECK_AST(expr_result);
    if (cur_token()->type() == TokenKind::kRightParen) {
      Next();
      auto then_stmt_result = ParseStatement(yield, has_return, breakable, continuable);
      CHECK_AST(then_stmt_result);
      if (prev_token()->type() != TokenKind::kRightBrace && IsLineTermination()) {
        ConsumeLineTerminator();
      }
      ParseResult else_stmt_result;
      if (cur_token()->type() == TokenKind::kElse) {
        Next();
        else_stmt_result = ParseStatement(yield, has_return, breakable, continuable);
        CHECK_AST(else_stmt_result);
        if (prev_token()->type() != TokenKind::kRightBrace && IsLineTermination()) {
          ConsumeLineTerminator();
        }
      }
      Handle<ir::IfStatementView> if_stmt = New<ir::IfStatementView>(expr_result.value(),
                                                                     then_stmt_result.value(),
                                                                     else_stmt_result.value());
      if_stmt->SetInformationForNode(&info);
      return Success(if_stmt);
    }
    SYNTAX_ERROR("')' expected", cur_token());
  }
  SYNTAX_ERROR("'(' expected", cur_token());
}


// while_statment
//   : 'while' '(' expression[in, ?yield] ')' statement[?yield, ?return]
//   ;
template <typename UCharInputIterator>
ParseResult Parser<UCharInputIterator>::ParseWhileStatement(bool yield, bool has_return) {
  LOG_PHASE(ParseWhileStatement);
  Token info = *(cur_token());
  Next();
  if (cur_token()->type() == TokenKind::kLeftParen) {
    Next();
    auto expr_result = ParseExpression(true, yield);
    CHECK_AST(expr_result);
    auto iteration_body_result = ParseIterationBody(yield, has_return);
    CHECK_AST(iteration_body_result);
    auto while_stmt = New<ir::WhileStatementView>(expr_result.value(), iteration_body_result.value());
    while_stmt->SetInformationForNode(&info);
    return Success(while_stmt);
  }
  SYNTAX_ERROR("'(' expected", cur_token());
}


// do_while_statement
//   : 'do' statement[?yield, ?return] 'while' '(' expression[in, ?yield] ')' ;__opt
//   ;
template <typename UCharInputIterator>
ParseResult Parser<UCharInputIterator>::ParseDoWhileStatement(bool yield, bool has_return) {
  LOG_PHASE(ParseDoWhileStatement);
  Token info = *(cur_token());
  Next();
  auto stmt_result = ParseStatement(yield, has_return, true, true);
  CHECK_AST(stmt_result);
  if (cur_token()->type() == TokenKind::kWhile) {
    Next();
    if (cur_token()->type() == TokenKind::kLeftParen) {
      Next();
      auto expr_result = ParseExpression(true, yield);
      CHECK_AST(expr_result);
      if (cur_token()->type() == TokenKind::kRightParen) {
        Next();
        if (IsLineTermination()) {
          ConsumeLineTerminator();
        }
        auto do_while = New<ir::DoWhileStatementView>(expr_result.value(), stmt_result.value());
        do_while->SetInformationForNode(&info);
        return Success(do_while);
      }
      SYNTAX_ERROR("')' expected", cur_token());
    }
    SYNTAX_ERROR("'(' expected", cur_token());
  }
  SYNTAX_ERROR("'while' expected", cur_token());
}


template <typename UCharInputIterator>
ParseResult Parser<UCharInputIterator>::ParseForStatement(bool yield, bool has_return) {
  LOG_PHASE(ParseForStatement);

  Token info = *cur_token();
  Next();
  if (cur_token()->type() == TokenKind::kLeftParen) {
    Next();
    ParseResult reciever_result;
    switch (cur_token()->type()) {
      case TokenKind::kVar: {
        reciever_result = ParseVariableStatement(true, yield);
        CHECK_AST(reciever_result);
        break;
      }
      case TokenKind::kLet: {
        reciever_result = ParseLexicalDeclaration(true, yield);
        CHECK_AST(reciever_result);
        break;
      }
      case TokenKind::kLineTerminator:
        break;
      default: {
        RecordedParserState rps = parser_state();
        reciever_result = ParseExpression(true, yield);
        CHECK_AST(reciever_result);
        if (cur_token()->type() != TokenKind::kLineTerminator) {
          RestoreParserState(rps);
          reciever_result = ParseLeftHandSideExpression(yield);
          CHECK_AST(reciever_result);
        }
      }
    }
    return ParseForIteration(reciever_result.value(), &info, yield, has_return);
  }
  SYNTAX_ERROR("'(' expected", cur_token());
}


template <typename UCharInputIterator>
ParseResult Parser<UCharInputIterator>::ParseForIteration(Handle<ir::Node> reciever, Token* token_info, bool yield, bool has_return) {
  LOG_PHASE(ParseForIterationStatement);

  ParseResult second_result;
  ParseResult third_result;
  bool for_in = false;
  bool for_of = false;
  
  if (cur_token()->type() == TokenKind::kLineTerminator) {
    // for (var i = 0; i < 10; i++) ...
    Next();
    if (cur_token()->type() != TokenKind::kLineTerminator) {
      second_result = ParseExpression(true, yield);
      CHECK_AST(second_result);
    }
    if (cur_token()->type() == TokenKind::kLineTerminator) {
      Next();
      if (cur_token()->type() != TokenKind::kRightParen) {
        third_result = ParseExpression(true, yield);
        CHECK_AST(third_result);
      }
      
    }
  } else if (cur_token()->type() == TokenKind::kIdentifier &&
             cur_token()->value()->Equals("of")) {
    // for (var i in obj) ...
    Next();
    second_result = ParseAssignmentExpression(true, yield);
    CHECK_AST(second_result);
    for_of = true;
  } else if (cur_token()->type() == TokenKind::kIn) {
    // for (var i of obj) ...
    Next();
    second_result = ParseAssignmentExpression(false, yield);
    CHECK_AST(second_result);
    for_in = true;
  } else {
    SYNTAX_ERROR("'in' or 'of' or ';' expected", cur_token());
  }

  auto iteration_body_result = ParseIterationBody(yield, has_return);
  Handle<ir::Node> ret;
  
  if (for_in) {
    ret = New<ir::ForInStatementView>(reciever, second_result.value(), iteration_body_result.value());
  } else if (for_of) {
    ret = New<ir::ForOfStatementView>(reciever, second_result.value(), iteration_body_result.value());
  } else {
    ret = New<ir::ForStatementView>(reciever, second_result.value(), third_result.value(), iteration_body_result.value());
  }
    
  ret->SetInformationForNode(token_info);
  return Success(ret);
}


template <typename UCharInputIterator>
ParseResult Parser<UCharInputIterator>::ParseIterationBody(bool yield, bool has_return) {
  if (cur_token()->type() == TokenKind::kRightParen) {
    Next();
    return ParseStatement(yield, has_return, true, true);
  }
  SYNTAX_ERROR("')' expected", cur_token());
}


template <typename UCharInputIterator>
ParseResult Parser<UCharInputIterator>::ParseContinueStatement(bool yield) {
  LOG_PHASE(ParseContinueStatement);

  Token info = *cur_token();
  Next();
  ParseResult parse_result;
  YATSC_SCOPED([&]{
    if (parse_result) {
      parse_result.value()->SetInformationForNode(&info);
    }
  });
  if (cur_token()->type() == TokenKind::kIdentifier) {
    auto primary_expr_result = ParsePrimaryExpression(yield);
    CHECK_AST(primary_expr_result);
    return parse_result = Success(New<ir::ContinueStatementView>(primary_expr_result.value()));
  }
  return parse_result = Success(New<ir::ContinueStatementView>());
}


template <typename UCharInputIterator>
ParseResult Parser<UCharInputIterator>::ParseBreakStatement(bool yield) {
  LOG_PHASE(ParseBreakStatement);

  Token info = *cur_token();
  Next();
  ParseResult parse_result;
  YATSC_SCOPED([&]{
    if (parse_result) {
      parse_result.value()->SetInformationForNode(&info);
    }
  });
  if (cur_token()->type() == TokenKind::kIdentifier) {
    auto primary_expr_result = ParsePrimaryExpression(yield);
    return parse_result = Success(New<ir::BreakStatementView>(primary_expr_result.value()));
  }
  return parse_result = Success(New<ir::BreakStatementView>());

}


template <typename UCharInputIterator>
ParseResult Parser<UCharInputIterator>::ParseReturnStatement(bool yield) {
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
  auto expr_result = ParseExpression(true, yield);
  CHECK_AST(expr_result);
  return parse_result = Success(New<ir::ReturnStatementView>(expr_result.value()));
}


template <typename UCharInputIterator>
ParseResult Parser<UCharInputIterator>::ParseWithStatement(bool yield, bool has_return, bool breakable, bool continuable) {
  LOG_PHASE(ParseWithStatement);

  Token info = *cur_token();
  ParseResult parse_result;
  YATSC_SCOPED([&]{
    if (parse_result) {
      parse_result.value()->SetInformationForNode(&info);
    }
  });
    
  Next();
    
  if (cur_token()->type() == TokenKind::kLeftParen) {
    Next();
    auto expr_result = ParseExpression(true, yield);
    CHECK_AST(expr_result);
    if (cur_token()->type() == TokenKind::kRightParen) {
      Next();
      auto stmt_result = ParseStatement(yield, has_return, breakable, continuable);
      CHECK_AST(stmt_result);
      return parse_result = Success(New<ir::WithStatementView>(expr_result.value(), stmt_result.value()));
    }
    SYNTAX_ERROR("')' expected", cur_token());
  }
  SYNTAX_ERROR("'(' expected", cur_token());
}


template <typename UCharInputIterator>
ParseResult Parser<UCharInputIterator>::ParseSwitchStatement(bool yield, bool has_return, bool continuable) {
  LOG_PHASE(ParseSwitchStatement);

  Token info = *cur_token();
  Next();
  if (cur_token()->type() == TokenKind::kLeftParen) {
    Next();
    auto expr_result = ParseExpression(true, yield);
    CHECK_AST(expr_result);
    if (cur_token()->type() == TokenKind::kRightParen) {
      Next();
      if (cur_token()->type() == TokenKind::kLeftBrace) {
        Next();
        auto case_clauses_result = ParseCaseClauses(yield, has_return, continuable);
        CHECK_AST(case_clauses_result);
        if (cur_token()->type() == TokenKind::kRightBrace) {
          Next();
          auto switch_stmt = New<ir::SwitchStatementView>(expr_result.value(), case_clauses_result.value());
          switch_stmt->SetInformationForNode(&info);
          return Success(switch_stmt);
        }
        SYNTAX_ERROR("'}' expected", cur_token());
      }
      SYNTAX_ERROR("'{' expected", cur_token());
    }
    SYNTAX_ERROR("')' expected", cur_token());
  }
  SYNTAX_ERROR("'(' expected", cur_token());
}


template <typename UCharInputIterator>
ParseResult Parser<UCharInputIterator>::ParseCaseClauses(bool yield, bool has_return, bool continuable) {
  LOG_PHASE(ParseCaseClauses);
  Handle<ir::CaseListView> case_list = New<ir::CaseListView>();
  case_list->SetInformationForNode(cur_token());
  bool default_encounted = false;
  bool success = true;
  
  while (1) {
    bool normal_case = false;
    ParseResult expr_result;
    Token info = *cur_token();
    
    switch (cur_token()->type()) {
      case TokenKind::kCase: {
        normal_case = true;
        Next();
        expr_result = ParseExpression(true, yield);
        SKIP_TOKEN_IF_AND(expr_result, success, TokenKind::kRightBrace, break);
      }
      case TokenKind::kDefault: {
        if (!normal_case) {
          default_encounted = true;
          Next();
        }
        if (cur_token()->type() == TokenKind::kColon) {
          Next();
        } else {
          SYNTAX_ERROR("':' expected", cur_token());
        }
        Handle<ir::Node> body = New<ir::CaseBody>();
        while (1) {
          if (cur_token()->type() == TokenKind::kCase ||
              cur_token()->type() == TokenKind::kDefault) {
            if (default_encounted && cur_token()->type() == TokenKind::kDefault) {
              SYNTAX_ERROR("More than one 'default' clause in switch statement", cur_token());
            }
            break;
          } else if (cur_token()->type() == TokenKind::kRightBrace) {
            break;
          }
          if (cur_token()->type() == TokenKind::kLeftBrace) {
            auto block_stmt_result = ParseBlockStatement(yield, has_return, true, continuable);
            SKIP_TOKEN_OR(block_stmt_result, success, TokenKind::kRightBrace) {
              body->InsertLast(block_stmt_result.value());
            }
          } else {
            auto stmt_list_result = ParseStatementListItem(yield, has_return, true, continuable);
            SKIP_TOKEN_OR(stmt_list_result, success, TokenKind::kRightBrace) {
              body->InsertLast(stmt_list_result.value());
            }
          }
        }
        
        Handle<ir::CaseView> case_view = New<ir::CaseView>(expr_result.value(), body);
        case_view->SetInformationForNode(&info);
        case_list->InsertLast(case_view);
        break;
      }
      case TokenKind::kRightBrace: {
        return Success(case_list);
      }
      default:
        SYNTAX_ERROR("unexpected token", cur_token());
    }
  }
}


template <typename UCharInputIterator>
ParseResult Parser<UCharInputIterator>::ParseLabelledStatement(bool yield, bool has_return, bool breakable, bool continuable) {
  LOG_PHASE(ParseLabelledStatement);
  auto label_identifier_result = ParseLabelIdentifier(yield);
  CHECK_AST(label_identifier_result);
  label_identifier_result.value()->symbol()->set_type(ir::SymbolType::kLabelName);

  current_scope()->Declare(label_identifier_result.value());
    
  if (cur_token()->type() == TokenKind::kColon) {
    Next();
    auto lebelled_item_result = ParseLabelledItem(yield, has_return, breakable, continuable);
    CHECK_AST(lebelled_item_result);
    auto node = New<ir::LabelledStatementView>(label_identifier_result.value(), lebelled_item_result.value());
    node->SetInformationForNode(label_identifier_result.value());
    return Success(node);
  }
  SYNTAX_ERROR("':' expected", cur_token());
}


template <typename UCharInputIterator>
ParseResult Parser<UCharInputIterator>::ParseLabelledItem(bool yield, bool has_return, bool breakable, bool continuable) {
  if (cur_token()->type() == TokenKind::kFunction) {
    return ParseFunctionOverloads(yield, false, true, false);
  }
  return ParseStatement(yield, has_return, breakable, continuable);
}


template <typename UCharInputIterator>
ParseResult Parser<UCharInputIterator>::ParseThrowStatement() {
  LOG_PHASE(ParseThrowStatement);
  Token info = *cur_token();
  Next();
  if (!IsLineTermination()) {
    auto expr_result = ParseExpression(false, false);
    CHECK_AST(expr_result);
    Handle<ir::ThrowStatementView> throw_stmt = New<ir::ThrowStatementView>(expr_result.value());
    throw_stmt->SetInformationForNode(&info);
    return Success(throw_stmt);
  }
  SYNTAX_ERROR("throw statement expected expression", (&info));
}


template <typename UCharInputIterator>
ParseResult Parser<UCharInputIterator>::ParseTryStatement(bool yield, bool has_return, bool breakable, bool continuable) {
  LOG_PHASE(ParseTryStatement);

  Token info = *cur_token();
  Next();
  auto block_stmt_result = ParseBlockStatement(yield, has_return, breakable, continuable);
  CHECK_AST(block_stmt_result);
  ParseResult catch_block_result;
  ParseResult finally_block_result;
  bool has_catch_or_finally = false;

  if (cur_token()->type() == TokenKind::kCatch) {
    has_catch_or_finally = true;
    catch_block_result = ParseCatchBlock(yield, has_return, breakable, continuable);
    CHECK_AST(catch_block_result);
  }
    
  if (cur_token()->type() == TokenKind::kFinally) {
    has_catch_or_finally = true;
    finally_block_result = ParseFinallyBlock(yield, has_return, breakable, continuable);
    CHECK_AST(finally_block_result);
  }

  if (!has_catch_or_finally) {
    SYNTAX_ERROR("try statement need catch block or finally block", cur_token());
  }
    
  Handle<ir::TryStatementView> try_stmt = New<ir::TryStatementView>(block_stmt_result.value(),
                                                                    catch_block_result.value(),
                                                                    finally_block_result.value());
  try_stmt->SetInformationForNode(&info);
  return Success(try_stmt);
}


template <typename UCharInputIterator>
ParseResult Parser<UCharInputIterator>::ParseCatchBlock(bool yield, bool has_return, bool breakable, bool continuable) {
  LOG_PHASE(ParseCatchBlock);

  Token info = *cur_token();
  Next();
  if (cur_token()->type() == TokenKind::kLeftParen) {
    Next();
    ParseResult catch_parameter_result;
    if (cur_token()->type() == TokenKind::kIdentifier) {
      catch_parameter_result = ParseBindingIdentifier(false, false, yield);
    } else {
      catch_parameter_result = ParseBindingPattern(yield, false);
    }
    CHECK_AST(catch_parameter_result);

    if (cur_token()->type() == TokenKind::kRightParen) {
      Next();
      auto block_stmt_result = ParseBlockStatement(yield, has_return, breakable, continuable);
      CHECK_AST(block_stmt_result);
      Handle<ir::CatchStatementView> catch_stmt = New<ir::CatchStatementView>(catch_parameter_result.value(),
                                                                              block_stmt_result.value());
      catch_stmt->SetInformationForNode(&info);
      return Success(catch_stmt);
    }
    SYNTAX_ERROR("')' expected", cur_token());
  }
  SYNTAX_ERROR("'(' expected", cur_token());
}


template <typename UCharInputIterator>
ParseResult Parser<UCharInputIterator>::ParseFinallyBlock(bool yield, bool has_return, bool breakable, bool continuable) {
  LOG_PHASE(ParseFinallyBlock);
  Token info = *cur_token();
  Next();
  auto block_stmt_result = ParseBlockStatement(yield, has_return, breakable, continuable);
  CHECK_AST(block_stmt_result);
  Handle<ir::FinallyStatementView> finally_stmt = New<ir::FinallyStatementView>(block_stmt_result.value());
  finally_stmt->SetInformationForNode(&info);
  return Success(finally_stmt);
}


template <typename UCharInputIterator>
ParseResult Parser<UCharInputIterator>::ParseDebuggerStatement() {
  LOG_PHASE(ParseDebuggerStatement);

  Handle<ir::Node> ret = New<ir::DebuggerView>();
  ret->SetInformationForNode(cur_token());
  Next();
  return Success(ret);
}


template <typename UCharInputIterator>
ParseResult Parser<UCharInputIterator>::ParseInterfaceDeclaration() {
  LOG_PHASE(ParseInterfaceDeclaration);
  bool success = true;

  Next();
  auto identifier_result = ParseIdentifier();
  SKIP_TOKEN_IF_AND(identifier_result, success, TokenKind::kRightBrace, return Failed());
  ParseResult type_parameters_result;

  identifier_result.value()->symbol()->set_type(ir::SymbolType::kInterfaceName);
    
  if (cur_token()->type() == TokenKind::kLess) {
    type_parameters_result = ParseTypeParameters();
    SKIP_TOKEN_IF_AND(type_parameters_result, success, TokenKind::kRightBrace, return Failed());
  }
    
  auto extends = New<ir::InterfaceExtendsView>();

  if (cur_token()->type() == TokenKind::kExtends) {
    Next();
    while (1) {
      auto ref_type_result = ParseReferencedType();
      SKIP_TOKEN_OR(ref_type_result, success, TokenKind::kLeftBrace) {
        extends->InsertLast(ref_type_result.value());
      }
      if (cur_token()->type() != TokenKind::kLeftBrace) {
        Next();
      } else if (cur_token()->type() == TokenKind::kEof) {
        SYNTAX_ERROR("unexpected end of input.", cur_token());
      } else {
        break;
      }
    }
  }

  if (cur_token()->type() == TokenKind::kLeftBrace) {
    auto interface_body_result = ParseObjectTypeExpression();
    return Success(New<ir::InterfaceView>(identifier_result.value(),
                                          type_parameters_result.value(),
                                          extends,
                                          interface_body_result.value()));
  }
  SYNTAX_ERROR("'{' expected.", cur_token());
}


template <typename UCharInputIterator>
ParseResult Parser<UCharInputIterator>::ParseEnumDeclaration(bool yield, bool has_default) {
  LOG_PHASE(ParseEnumDeclaration);
  bool success = true;

  Token info = *cur_token();
  Next();
  auto identifier_result = ParseIdentifier();
  SKIP_TOKEN_OR(identifier_result, success, TokenKind::kLeftBrace) {
    identifier_result.value()->symbol()->set_type(ir::SymbolType::kEnumName);
  }
    
  if (cur_token()->type() == TokenKind::kLeftBrace) {
    auto enum_body_result = ParseEnumBody(yield, has_default);
    CHECK_AST(enum_body_result);
    auto ret = New<ir::EnumDeclView>(identifier_result.value(), enum_body_result.value());
    ret->SetInformationForNode(&info);
    return Success(ret);
  }
  SYNTAX_ERROR("'{' expected.", cur_token());
}


template <typename UCharInputIterator>
ParseResult Parser<UCharInputIterator>::ParseEnumBody(bool yield, bool has_default) {
  LOG_PHASE(ParseEnumBody);

  auto ret = New<ir::EnumBodyView>();
  ret->SetInformationForNode(cur_token());
  Next();
    
  if (cur_token()->type() == TokenKind::kRightBrace) {
    Next();
    return Success(ret);
  }

  bool success = true;
    
  while (1) {
    auto enum_property_result = ParseEnumProperty(yield, has_default);
    SKIP_TOKEN_OR(enum_property_result, success, TokenKind::kRightBrace) {
      ret->InsertLast(enum_property_result.value());
    }
    if (cur_token()->type() == TokenKind::kComma) {
      Next();
      if (cur_token()->type() == TokenKind::kRightBrace) {
        Next();
        return Success(ret);
      }
    } else if (cur_token()->type() == TokenKind::kRightBrace) {
      Next();
      return Success(ret);
    } else {
      SYNTAX_ERROR("',' or '}' expected.", cur_token());
    }
  }
}


template <typename UCharInputIterator>
ParseResult Parser<UCharInputIterator>::ParseEnumProperty(bool yield, bool has_default) {
  LOG_PHASE(ParseEnumProperty);
  auto prop_name_result = ParsePropertyName(yield, false);
  CHECK_AST(prop_name_result);
  if (cur_token()->type() == TokenKind::kAssign) {
    Next();
    auto assignment_expr_result = ParseAssignmentExpression(true, yield);
    CHECK_AST(assignment_expr_result);
    return Success(CreateEnumFieldView(prop_name_result.value(), assignment_expr_result.value()));
  }
  return Success(CreateEnumFieldView(prop_name_result.value(), ir::Node::Null()));
}


template <typename UCharInputIterator>
Handle<ir::Node> Parser<UCharInputIterator>::CreateEnumFieldView(
    Handle<ir::Node> name,
    Handle<ir::Node> value) {
  LOG_PHASE(CreateEnumFieldView);
  auto ret = New<ir::EnumFieldView>(name, value);
  ret->SetInformationForNode(name);
  return ret;
}


template <typename UCharInputIterator>
ParseResult Parser<UCharInputIterator>::ParseClassDeclaration(bool yield, bool has_default) {
  LOG_PHASE(ParseClassDeclaration);

  Token info = *cur_token();
  Next();
  ParseResult type_parameters_result;
  auto identifier_result = ParseIdentifier();
  bool success = true;
    
  SKIP_TOKEN_IF_AND(identifier_result, success, TokenKind::kLeftBrace, return Failed());
    
  identifier_result.value()->symbol()->set_type(ir::SymbolType::kClassName);
    
  if (cur_token()->type() == TokenKind::kLess) {
    type_parameters_result = ParseTypeParameters();
    SKIP_TOKEN_IF(type_parameters_result, success, TokenKind::kLeftBrace);
  }
    
  auto class_bases_result = ParseClassBases();
  SKIP_TOKEN_IF(class_bases_result, success, TokenKind::kLeftBrace);
    
  if (cur_token()->type() == TokenKind::kLeftBrace) {
    Next();
    auto class_body_result = ParseClassBody();
    SKIP_TOKEN_IF(class_body_result, success, TokenKind::kRightBrace);
    auto class_decl = New<ir::ClassDeclView>(identifier_result.value(),
                                             type_parameters_result.value(),
                                             class_bases_result.value(),
                                             class_body_result.value());
    class_decl->SetInformationForNode(&info);
    return Success(class_decl);
  }
  SYNTAX_ERROR("'{' expected", cur_token());
}


template <typename UCharInputIterator>
ParseResult Parser<UCharInputIterator>::ParseClassBases() {
  LOG_PHASE(ParseClassBases);
  bool extends_keyword = false;
  bool success = true;
  
  auto bases = New<ir::ClassBasesView>();
  auto impls = New<ir::ClassImplsView>();
  bases->SetInformationForNode(cur_token());
  
  while (1) {
    if (cur_token()->type() == TokenKind::kExtends) {
      if (extends_keyword) {
        SYNTAX_ERROR("class extendable only one class", cur_token());
      }
      Token info = *cur_token();
      Next();
      extends_keyword = true;
      auto ref_type_result = ParseReferencedType();
      
      SKIP_TOKEN_OR(ref_type_result, success, TokenKind::kLeftBrace) {
        auto heritage = New<ir::ClassHeritageView>(ref_type_result.value());
        heritage->SetInformationForNode(&info);
        bases->set_base(heritage);
      }
    } else if (cur_token()->type() == TokenKind::kImplements) {
      Next();
      while (1) {
        auto ref_type_result = ParseReferencedType();
        SKIP_TOKEN_OR(ref_type_result, success, TokenKind::kLeftBrace) {
          impls->InsertLast(ref_type_result.value());
        }
        if (cur_token()->type() != TokenKind::kComma) {
          break;
        }
        Next();
      }
    } else if (cur_token()->type() == TokenKind::kLeftBrace) {
      if (impls->size() > 0) {
        bases->set_impls(impls);
      }
      return Success(bases);
    } else {
      SYNTAX_ERROR("unexpected token", cur_token());
    }
  }
}


template <typename UCharInputIterator>
ParseResult Parser<UCharInputIterator>::ParseClassBody() {
  LOG_PHASE(ParseClassBody);
  auto fields = New<ir::ClassFieldListView>();
  bool success = true;
  while (1) {
    if (cur_token()->type() != TokenKind::kRightBrace) {
      auto class_element_result = ParseClassElement();
      SKIP_TOKEN_OR(class_element_result, success, TokenKind::kLineTerminator) {
        fields->InsertLast(class_element_result.value());
      }
      if (IsLineTermination()) {
        ConsumeLineTerminator();
      } else if (cur_token()->type() != TokenKind::kRightBrace &&
                 prev_token()->type() != TokenKind::kRightBrace) {
        SYNTAX_ERROR("';' expected", prev_token());
      }
    } else {
      Next();
      return Success(fields);
    }
  }
}


template <typename UCharInputIterator>
ParseResult Parser<UCharInputIterator>::ParseClassElement() {
  LOG_PHASE(ParseClassElement);
  
  if (cur_token()->type() == TokenKind::kLeftBracket) {
    return ParseIndexSignature();
  }
  
  auto field_modifiers_result = ParseFieldModifiers();
  CHECK_AST(field_modifiers_result);
  AccessorType at = ParseAccessor();

  if (Token::IsKeyword(cur_token()->type())) {
    cur_token()->set_type(TokenKind::kIdentifier);
  }
  
  if (cur_token()->type() == TokenKind::kIdentifier) {
    if (cur_token()->value()->Equals("constructor")) {
      return ParseConstructorOverloads(field_modifiers_result.value());
    } else {
      RecordedParserState rps = parser_state();
      Next();
      if (cur_token()->type() == TokenKind::kLeftParen ||
          cur_token()->type() == TokenKind::kLess) {
        RestoreParserState(rps);
        return ParseMemberFunctionOverloads(field_modifiers_result.value(), &at);
      } else {
        RestoreParserState(rps);
        return ParseMemberVariable(field_modifiers_result.value());
      }
    }
  } else if (cur_token()->type() == TokenKind::kMul) {
    Next();
    return ParseGeneratorMethodOverloads(field_modifiers_result.value());
  }
  SYNTAX_ERROR("unexpected token", cur_token());
}


template <typename UCharInputIterator>
ParseResult Parser<UCharInputIterator>::ParseFieldModifiers() {
  LOG_PHASE(ParseFieldModifiers);
  
  auto mods = New<ir::ClassFieldModifiersView>();
  mods->SetInformationForNode(cur_token());
  
  if (cur_token()->type() == TokenKind::kStatic) {
    auto field_modifier_result = ParseFieldModifier();
    CHECK_AST(field_modifier_result);
    mods->InsertLast(field_modifier_result.value());
    if (cur_token()->type() == TokenKind::kPublic ||
        cur_token()->type() == TokenKind::kProtected ||
        cur_token()->type() == TokenKind::kPrivate) {
      field_modifier_result = ParseFieldModifier();
      CHECK_AST(field_modifier_result);
      mods->InsertLast(field_modifier_result.value());
    }
  } else if (cur_token()->type() == TokenKind::kPublic ||
             cur_token()->type() == TokenKind::kProtected ||
             cur_token()->type() == TokenKind::kPrivate) {
    auto field_modifier_result1 = ParseFieldModifier();
    CHECK_AST(field_modifier_result1);
    if (cur_token()->type() == TokenKind::kStatic) {
      auto field_modifier_result2 = ParseFieldModifier();
      CHECK_AST(field_modifier_result2);
      mods->InsertLast(field_modifier_result2.value());
      mods->InsertLast(field_modifier_result1.value());
    } else {
      mods->InsertLast(field_modifier_result1.value());
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
      return Failed();
  }
}


template <typename UCharInputIterator>
void Parser<UCharInputIterator>::ValidateOverload(Handle<ir::MemberFunctionDefinitionView> node, Handle<ir::Node> overloads) {
  LOG_PHASE(ValidateOverload);
  
  if (overloads->size() > 0) {
    Handle<ir::MemberFunctionOverloadView> last(overloads->last_child());
    
    if (!node->name()->SymbolEquals(last->at(1))) {
      SYNTAX_ERROR_NO_RETURN("member function overload must have a same name", node->at(1));
    }
    
    if (!node->modifiers()->Equals(last->modifiers())) {
      Handle<ir::Node> target;
      if (node->modifiers()->size() > last->modifiers()->size()) {
        target = node->modifiers()->first_child();
      } else {
        target = last->modifiers()->first_child();
      }
      SYNTAX_ERROR_NO_RETURN("member function overload must have same modifiers", target); 
    }
  } else {
    Handle<ir::MemberFunctionOverloadView> fn(node);
    if (fn->getter()) {
      Handle<ir::CallSignatureView> call_sig(fn->call_signature());
      if (call_sig->param_list()->size() > 0) {
        SYNTAX_ERROR_NO_RETURN("the formal parameter of getter function must be empty.", call_sig->param_list());
      }
      
      if (call_sig->return_type()) {
        Handle<ir::Node> ret = call_sig->return_type();
        if (ret->HasSimpleTypeExprView()) {
          Handle<ir::Node> ret_type(ret->ToSimpleTypeExprView()->type_name());
          auto name = ret_type->symbol();
          if (name->Equals("void") || name->Equals("null")) {
            SYNTAX_ERROR_NO_RETURN("getter function must return value.", ret_type);
          }
        }
      }
    } else if (fn->setter()) {
      Handle<ir::CallSignatureView> call_sig(fn->call_signature());
      if (call_sig->param_list()->size() != 1) {
        SYNTAX_ERROR_NO_RETURN("the setter function allowed only one parameter.", call_sig->param_list());
      }
      if (call_sig->return_type()) {
        Handle<ir::Node> ret = call_sig->return_type();
        if (ret->HasSimpleTypeExprView()) {
          Handle<ir::Node> ret_type(ret->ToSimpleTypeExprView()->type_name());
          auto name = ret_type->symbol();
          if (!name->Equals("void") && !name->Equals("null")) {
            SYNTAX_ERROR_NO_RETURN("setter function must not return value.", ret_type);
          }
        }
      }
    }
  }
}


template <typename UCharInputIterator>
ParseResult Parser<UCharInputIterator>::ParseConstructorOverloads(Handle<ir::Node> mods) {
  LOG_PHASE(ParseConstructorOverloads);
  auto overloads = New<ir::MemberFunctionOverloadsView>();
  bool first = true;
  while (1) {
    if ((cur_token()->type() == TokenKind::kIdentifier &&
         cur_token()->value()->Equals("constructor")) ||
        cur_token()->type() == TokenKind::kPublic ||
        cur_token()->type() == TokenKind::kPrivate ||
        cur_token()->type() == TokenKind::kProtected) {
      auto constructor_overload_result = ParseConstructorOverloadOrImplementation(first, mods, overloads);
      CHECK_AST(constructor_overload_result);
      if (constructor_overload_result.value()->HasMemberFunctionOverloadView()) {
        overloads->InsertLast(constructor_overload_result.value());
        ValidateOverload(Handle<ir::MemberFunctionOverloadView>(constructor_overload_result.value()), overloads);
      } else {
        ValidateOverload(Handle<ir::MemberFunctionView>(constructor_overload_result.value()), overloads);
        return constructor_overload_result;
      }
    } else {
      SYNTAX_ERROR("incomplete constructor definition", cur_token());
    }
  }
}


template <typename UCharInputIterator>
ParseResult Parser<UCharInputIterator>::ParseConstructorOverloadOrImplementation(
    bool first,
    Handle<ir::Node> mods,
    Handle<ir::Node> overloads) {
  
  LOG_PHASE(ParseConstructorOverloadOrImplementation);
  
  if (!first) {
    auto field_modifiers_result = ParseFieldModifiers();
    CHECK_AST(field_modifiers_result);
    mods = field_modifiers_result.value();
  }
  
  if (cur_token()->type() == TokenKind::kIdentifier &&
      cur_token()->value()->Equals("constructor")) {
    Token info = *cur_token();
    auto identifier_result = ParseIdentifier();
    CHECK_AST(identifier_result);
    auto call_sig_result = ParseCallSignature(true, false);
    CHECK_AST(call_sig_result);
    Handle<ir::Node> ret;
    if (cur_token()->type() == TokenKind::kLeftBrace) {
      auto function_body_result = ParseFunctionBody(false);
      CHECK_AST(function_body_result);
      ret = New<ir::MemberFunctionView>(mods,
                                        identifier_result.value(),
                                        call_sig_result.value(),
                                        overloads,
                                        function_body_result.value());
    } else if (overloads) {
      ret = New<ir::MemberFunctionOverloadView>(mods, identifier_result.value(), call_sig_result.value());
      if (IsLineTermination()) {
        ConsumeLineTerminator();
      } else {
        SYNTAX_ERROR("';' expected", cur_token());
      }
    } else {
      SYNTAX_ERROR("invalid constructor definition", (&info));
    }
    ret->SetInformationForNode(mods);
    return Success(ret);
  }
  SYNTAX_ERROR("'constructor' expected", cur_token());
}


// Check member function begging token.
template <typename UCharInputIterator>
bool Parser<UCharInputIterator>::IsMemberFunctionOverloadsBegin(Token* info) {
  return info->type() == TokenKind::kIdentifier ||
    info->type() == TokenKind::kPublic ||
    info->type() == TokenKind::kPrivate ||
    info->type() == TokenKind::kStatic ||
    info->type() == TokenKind::kProtected ||
    Token::IsKeyword(info->type());
}


// Parse member function overloads.
// 
// MemberFunctionOverloads
//   MemberFunctionOverload
//   MemberFunctionOverloads MemberFunctionOverload
//
template <typename UCharInputIterator>
ParseResult Parser<UCharInputIterator>::ParseMemberFunctionOverloads(Handle<ir::Node> mods, AccessorType* at) {
  LOG_PHASE(ParseMemberFunctionOverloads);
  
  auto overloads = New<ir::MemberFunctionOverloadsView>();
  bool first = true;
  
  while (1) {
    Token info = *cur_token();
    RecordedParserState rps = parser_state();
    Next();
    if (IsMemberFunctionOverloadsBegin(&info)) {
      // Reserve token position.
      RestoreParserState(rps);

      // Parse an overload or an implementation.
      auto member_function_result = ParseMemberFunctionOverloadOrImplementation(first, mods, at, overloads);
      CHECK_AST(member_function_result);

      // If function is overload decl,
      // add node to the overloads list.
      if (member_function_result.value()->HasMemberFunctionOverloadView()) {
        ValidateOverload(Handle<ir::MemberFunctionOverloadView>(member_function_result.value()), overloads);
        overloads->InsertLast(member_function_result.value());
      } else {
        // Else, return node.
        ValidateOverload(Handle<ir::MemberFunctionView>(member_function_result.value()), overloads);
        return member_function_result;
      }
      first = false;
    } else {
      SYNTAX_ERROR("incomplete member function definition", (&info));
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
    Handle<ir::Node> mods,
    AccessorType* acessor_type,
    Handle<ir::Node> overloads) {
  LOG_PHASE(ParseMemberFunctionOverloadOrImplementation);
  
  AccessorType at(false, false, Token());

  // If this method is not a first function that is parsed from ParseMemberFunctionOverloads,
  // parse modifiers.
  if (!first) {
    auto field_modifiers_result = ParseFieldModifiers();
    CHECK_AST(field_modifiers_result);
    mods = field_modifiers_result.value();
    at = ParseAccessor();
  } else {
    at = *acessor_type;
  }


  if (Token::IsKeyword(cur_token()->type())) {
    cur_token()->set_type(TokenKind::kIdentifier);
  }
  

  // Method must be began an js identifier.
  if (cur_token()->type() == TokenKind::kIdentifier) {
    // Save position.
    Token info = *cur_token();

    auto identifier_result = ParseIdentifier();
    CHECK_AST(identifier_result);
    auto call_sig_result = ParseCallSignature(false, false);
    CHECK_AST(call_sig_result);
    Handle<ir::Node> ret;

    // public something(): void {
    // -------------------------^ here
    if (cur_token()->type() == TokenKind::kLeftBrace) {
      auto function_body_result = ParseFunctionBody(false);
      CHECK_AST(function_body_result);
      ret = New<ir::MemberFunctionView>(at.getter, at.setter, false,
                                        mods,
                                        identifier_result.value(),
                                        call_sig_result.value(),
                                        overloads,
                                        function_body_result.value());
    } else if (overloads) {
      
      // Getter and setter is not allowed to overload function declaration.
      if (at.getter || at.setter) {
        SYNTAX_ERROR("overload is not allowed to getter and setter.", (&info));
      }
      ret = New<ir::MemberFunctionOverloadView>(mods, identifier_result.value(), call_sig_result.value());
      if (IsLineTermination()) {
        ConsumeLineTerminator();
      } else {
        SYNTAX_ERROR("';' expected", cur_token());
      }
    } else {
      SYNTAX_ERROR("invalid member function definition", (&info));
    }
    ret->SetInformationForNode(mods);
    return Success(ret);
  }
  SYNTAX_ERROR("identifier expected", cur_token());
}


template <typename UCharInputIterator>
ParseResult Parser<UCharInputIterator>::ParseGeneratorMethodOverloads(Handle<ir::Node> mods) {
  LOG_PHASE(ParseGeneratorMethodOverloads);
  auto overloads = New<ir::MemberFunctionOverloadsView>();
  bool first = true;
  
  while (1) {
    Token info = *cur_token();
    RecordedParserState rps = parser_state();
    Next();
    if (IsMemberFunctionOverloadsBegin(&info)) {
      RestoreParserState(rps);
      auto generator_result = ParseGeneratorMethodOverloadOrImplementation(first, mods, overloads);
      CHECK_AST(generator_result);
      if (generator_result.value()->HasMemberFunctionOverloadView()) {
        ValidateOverload(Handle<ir::MemberFunctionOverloadView>(generator_result.value()), overloads);
        overloads->InsertLast(generator_result.value());
      } else {
        ValidateOverload(Handle<ir::MemberFunctionView>(generator_result.value()), overloads);
        return generator_result;
      }
      first = false;
    } else {
      SYNTAX_ERROR("incomplete member function definition", cur_token());
    }
  }
}


template <typename UCharInputIterator>
ParseResult Parser<UCharInputIterator>::ParseGeneratorMethodOverloadOrImplementation(
    bool first,
    Handle<ir::Node> mods,
    Handle<ir::Node> overloads) {
  
  LOG_PHASE(ParseGeneratorMethodOverloadOrImplementation);
  
  if (!first) {
    auto field_modifiers_result = ParseFieldModifiers();
    CHECK_AST(field_modifiers_result);
    mods = field_modifiers_result.value();
  }

  Token info = *cur_token();
  if (cur_token()->type() == TokenKind::kMul) {
    Next();
  } else if (!first) {
    SYNTAX_ERROR("'*' expected", cur_token());
  }

  
  if (Token::IsKeyword(cur_token()->type())) {
    cur_token()->set_type(TokenKind::kIdentifier);
  }
  
    
  if (cur_token()->type() == TokenKind::kIdentifier) {
    auto identifier_result = ParseIdentifier();
    CHECK_AST(identifier_result);
    auto call_sig_result = ParseCallSignature(false, false);
    CHECK_AST(call_sig_result);
    Handle<ir::Node> ret;
      
    if (cur_token()->type() == TokenKind::kLeftBrace) {
      auto function_body_result = ParseFunctionBody(false);
      CHECK_AST(function_body_result);
      ret = New<ir::MemberFunctionView>(mods,
                                        identifier_result.value(),
                                        call_sig_result.value(),
                                        overloads,
                                        function_body_result.value());
    } else if (overloads) {
      ret = New<ir::MemberFunctionOverloadView>(mods, identifier_result.value(), call_sig_result.value());
      if (IsLineTermination()) {
        ConsumeLineTerminator();
      } else {
        SYNTAX_ERROR("';' expected", cur_token());
      }
    } else {
      SYNTAX_ERROR("invalid member function definition", (&info));
    }
      
    ret->SetInformationForNode(mods);
    return Success(ret);
  }
  SYNTAX_ERROR("identifier expected", cur_token());
}


template <typename UCharInputIterator>
ParseResult Parser<UCharInputIterator>::ParseMemberVariable(Handle<ir::Node> mods) {
  LOG_PHASE(ParseMemberVariable);

  if (Token::IsKeyword(cur_token()->type())) {
    cur_token()->set_type(TokenKind::kIdentifier);
  }
  
  if (cur_token()->type() == TokenKind::kIdentifier) {
    auto identifier_result = ParseIdentifier();
    CHECK_AST(identifier_result);
    ParseResult value_result;
    ParseResult type_result;
    if (cur_token()->type() == TokenKind::kColon) {
      Next();
      type_result = ParseTypeExpression();
      CHECK_AST(type_result);
    }
    if (cur_token()->type() == TokenKind::kAssign) {
      Next();
      value_result = ParseExpression(true, false);
      CHECK_AST(value_result);
    }
    auto member_variable = New<ir::MemberVariableView>(mods,
                                                       identifier_result.value(),
                                                       type_result.value(),
                                                       value_result.value());
    member_variable->SetInformationForNode(mods);
    return Success(member_variable);
  }
  SYNTAX_ERROR("'identifier' expected", cur_token());
}


template <typename UCharInputIterator>
ParseResult Parser<UCharInputIterator>::ParseFunctionOverloads(bool yield, bool has_default, bool declaration, bool is_export) {
  auto overloads = New<ir::FunctionOverloadsView>();
  bool first = true;
  
  while (1) {
    if (is_export && !first && cur_token()->type() == TokenKind::kExport) {
      Next();
    } else if (is_export && !first) {
      SYNTAX_ERROR("export expected.", cur_token());
    }
    first = false;
    if (cur_token()->type() == TokenKind::kFunction) {
      auto function_overloads_result = ParseFunctionOverloadOrImplementation(overloads, yield, has_default, declaration);
      CHECK_AST(function_overloads_result);
      if (function_overloads_result.value()->HasFunctionOverloadView()) {
        Handle<ir::FunctionOverloadView> overload(function_overloads_result.value());
        if (overloads->size() > 0) {
          Handle<ir::FunctionOverloadView> last(overloads->last_child());
          if (!last->name()) {
            SYNTAX_ERROR("function overload must have a name", overload);
          } else if (!last->name()->SymbolEquals(overload->name())) {
            SYNTAX_ERROR("function overload must have a same name", overload->name());
          }

          if (last->generator() != overload->generator()) {
            SYNTAX_ERROR("generator function can only overloaded by generator function", overload->name());
          }
        }
        overloads->InsertLast(function_overloads_result.value());
      } else {
        return function_overloads_result;
      }
    } else {
      SYNTAX_ERROR("incomplete function definition", cur_token());
    }
  }
}


template <typename UCharInputIterator>
ParseResult Parser<UCharInputIterator>::ParseFunctionOverloadOrImplementation(Handle<ir::Node> overloads, bool yield, bool has_default, bool declaration) {
  LOG_PHASE(ParseFunctionOverloadOrImplementation);
  if (cur_token()->type() == TokenKind::kFunction) {
    bool generator = false;
    bool success = true;
    
    Token info = *cur_token();
    Next();
    if (cur_token()->type() == TokenKind::kMul) {
      generator = true;
      Next();
    }
    ParseResult identifier_result;
    if (cur_token()->type() == TokenKind::kIdentifier) {
      identifier_result = ParseIdentifier();
      CHECK_AST(identifier_result);
    }

    if (declaration && !identifier_result) {
      SYNTAX_ERROR("function name required", cur_token());
    }
    
    auto call_sig_result = ParseCallSignature(false, false);
    CHECK_AST(call_sig_result);
    Handle<ir::Node> ret;
    if (cur_token()->type() == TokenKind::kLeftBrace) {
      auto function_body_result = ParseFunctionBody(yield? yield: generator);
      SKIP_TOKEN_IF_AND(function_body_result, success, TokenKind::kRightBrace, return Failed());
      ret = New<ir::FunctionView>(overloads,
                                  identifier_result.value(),
                                  call_sig_result.value(),
                                  function_body_result.value());
    } else if (overloads) {
      ret = New<ir::FunctionOverloadView>(generator,
                                          identifier_result.value(),
                                          call_sig_result.value());
      if (IsLineTermination()) {
        ConsumeLineTerminator();
      } else {
        SYNTAX_ERROR("';' expected", cur_token());
      }
    } else {
      SYNTAX_ERROR("invalid function definition", (&info));
    }
    ret->SetInformationForNode(&info);
    return Success(ret);
  }
  SYNTAX_ERROR("'function' expected", cur_token());
}


template <typename UCharInputIterator>
ParseResult Parser<UCharInputIterator>::ParseParameterList(bool accesslevel_allowed) {
  LOG_PHASE(ParseParameterList);

  Handle<ir::ParamList> param_list = New<ir::ParamList>();
  param_list->SetInformationForNode(cur_token());
  Next();

  if (cur_token()->type() == TokenKind::kRightParen) {
    Next();
    return Success(param_list);
  }
    
  bool has_rest = false;
  bool success = true;
    
  while (1) {
    if (has_rest) {
      SYNTAX_ERROR("Rest parameter must be at the end of the parameters", param_list->last_child());
    }
    if (cur_token()->type() == TokenKind::kIdentifier ||
        cur_token()->type() == TokenKind::kPrivate ||
        cur_token()->type() == TokenKind::kPublic ||
        cur_token()->type() == TokenKind::kProtected) {
      auto parameter_result = ParseParameter(false, accesslevel_allowed);
      SKIP_TOKEN_OR(parameter_result, success, TokenKind::kRightParen) {
        param_list->InsertLast(parameter_result.value());
      }
    } else if (cur_token()->type() == TokenKind::kRest) {
      has_rest = true;
      Token token = (*cur_token());
      Next();
      auto parameter_result = ParseParameter(true, accesslevel_allowed);
      SKIP_TOKEN_OR(parameter_result, success, TokenKind::kRightParen) {
        Handle<ir::Node> node = New<ir::RestParamView>(parameter_result.value());
        node->SetInformationForNode(&token);
        param_list->InsertLast(node);
      }
    } else {
      SYNTAX_ERROR("unexpected token in formal parameter list", cur_token());
    }

    if (cur_token()->type() == TokenKind::kComma) {
      Next();
    } else if (cur_token()->type() == TokenKind::kRightParen) {
      Next();
      return Success(param_list);
    } else {
      SYNTAX_ERROR("')' or ',' expected in parameter list", cur_token());
    }
  }
}


template <typename UCharInputIterator>
ParseResult Parser<UCharInputIterator>::ParseParameter(bool rest, bool accesslevel_allowed) {
  LOG_PHASE(ParseParameter);
  Handle<ir::Node> access_level;
  
  if (cur_token()->type() == TokenKind::kPublic || cur_token()->type() == TokenKind::kPrivate) {
    if (accesslevel_allowed) {
      access_level = New<ir::ClassFieldAccessLevelView>(cur_token()->type());
      access_level->SetInformationForNode(cur_token());
    } else {
      SYNTAX_ERROR("'private' or 'public' not allowed here", cur_token());
    }
    Next();
  }
  
  if (cur_token()->type() == TokenKind::kIdentifier) {
    Handle<ir::ParameterView> pv = New<ir::ParameterView>();
    pv->SetInformationForNode(cur_token());
    Handle<ir::NameView> nv = New<ir::NameView>(NewSymbol(ir::SymbolType::kVariableName, cur_token()->value()));
    nv->SetInformationForNode(cur_token());
    pv->set_access_level(access_level);
    pv->set_name(nv);
    Next();
    if (cur_token()->type() == TokenKind::kQuestionMark) {
      if (rest) {
        SYNTAX_ERROR("optional parameter not allowed in rest parameter", cur_token());
      }
      Next();
      pv->set_optional(true);
    }
    if (cur_token()->type() == TokenKind::kColon) {
      Next();
      if (cur_token()->type() == TokenKind::kStringLiteral) {
        auto string_literal_result = ParseStringLiteral();
        CHECK_AST(string_literal_result);
        pv->set_type_expr(string_literal_result.value());
      } else {
        auto type_expr_result = ParseTypeExpression();
        CHECK_AST(type_expr_result);
        pv->set_type_expr(type_expr_result.value());
      }
    }
    if (cur_token()->type() == TokenKind::kAssign) {
      if (rest) {
        SYNTAX_ERROR("default parameter not allowed in rest parameter", cur_token());
      }
      Next();
      auto assignment_expr_result = ParseAssignmentExpression(true, false);
      CHECK_AST(assignment_expr_result);
      pv->set_value(assignment_expr_result.value());
    }
    return Success(pv);
  }
  SYNTAX_ERROR("identifier expected", cur_token());
}


template <typename UCharInputIterator>
ParseResult Parser<UCharInputIterator>::ParseFunctionBody(bool yield) {
  LOG_PHASE(ParseFunctionBody);

  Handle<ir::Scope> scope = NewScope();
  set_current_scope(scope);
  auto block = New<ir::BlockView>(scope);
  block->SetInformationForNode(cur_token());
  Next();

  bool success = true;
    
  while (1) {
    if (cur_token()->type() == TokenKind::kRightBrace) {
      Next();
      break;
    } else if (cur_token()->type() == TokenKind::kEof) {
      SYNTAX_ERROR("unexpected end of input.", cur_token());
    }
    auto stmt_list_result = ParseStatementListItem(yield, true, false, false);
    SKIP_TOKEN_OR(stmt_list_result, success, TokenKind::kRightBrace) {
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
