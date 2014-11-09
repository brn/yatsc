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
  
  switch (Current()->type()) {
    case Token::TS_LEFT_BRACE:
      parse_result = ParseBlockStatement(yield, has_return, breakable, continuable);
      break;

    case Token::LINE_TERMINATOR:
      parse_result = ParseEmptyStatement();
      break;

    case Token::TS_IF:
      parse_result = ParseIfStatement(yield, has_return, breakable, continuable);
      break;

    case Token::TS_FOR:
      parse_result = ParseForStatement(yield, has_return);
      break;

    case Token::TS_WHILE:
      parse_result = ParseWhileStatement(yield, has_return);
      break;

    case Token::TS_DO:
      parse_result = ParseDoWhileStatement(yield, has_return);
      break;

    case Token::TS_CONTINUE:
      if (!continuable) {
        SYNTAX_ERROR("'continue' only allowed in loops", Current());
      }
      parse_result = ParseContinueStatement(yield);
      break;

    case Token::TS_BREAK: {
      if (!breakable) {
        SYNTAX_ERROR("'break' not allowed here", Current());
      }
      parse_result = ParseBreakStatement(yield);
      if (IsLineTermination()) {
        ConsumeLineTerminator();
      } else {
        SYNTAX_ERROR("';' expected", Current());
      }
      break;
    }

    case Token::TS_RETURN:
      if (!has_return) {
        SYNTAX_ERROR("'return' statement only allowed in function", Current());
      }
      parse_result = ParseReturnStatement(yield);
      break;

    case Token::TS_WITH:
      parse_result = ParseWithStatement(yield, has_return, breakable, continuable);
      break;

    case Token::TS_SWITCH:
      parse_result = ParseSwitchStatement(yield, has_return, continuable);
      break;

    case Token::TS_THROW:
      parse_result = ParseThrowStatement();
      break;

    case Token::TS_TRY:
      parse_result = ParseTryStatement(yield, has_return, breakable, continuable);
      break;

    case Token::TS_DEBUGGER:
      parse_result = ParseDebuggerStatement();
      break;

    case Token::TS_VAR: {
      auto variable_stmt_result = ParseVariableStatement(true, yield);
      if (IsLineTermination()) {
        ConsumeLineTerminator();
      } else {
        SYNTAX_ERROR("';' expected", Current());
      }
      return variable_stmt_result;
    }

    case Token::END_OF_INPUT:
      SYNTAX_ERROR("Unexpected end of input", Current());
      
    default: {
      if (Current()->type() == Token::TS_IDENTIFIER) {
        RecordedParserState rps = parser_state();
        Next();
        if (Current()->type() == Token::TS_COLON) {
          RestoreParserState(rps);
          return ParseLabelledStatement(yield, has_return, breakable, continuable);
        }
        RestoreParserState(rps);
      }
      TokenInfo info = *Current();
      parse_result = ParseExpression(true, yield);
      CHECK_AST(parse_result);
      parse_result = Success(New<ir::StatementView>(parse_result.node()));
      parse_result.node()->SetInformationForNode(&info);
    }
  }
  
  return parse_result;
}


template <typename UCharInputIterator>
ParseResult Parser<UCharInputIterator>::ParseDeclaration(bool error, bool yield, bool has_default) {
  switch (Current()->type()) {
    case Token::TS_FUNCTION: {
      auto function_overloads_result = ParseFunctionOverloads(yield, has_default, true, false);
      CHECK_AST(function_overloads_result);
      current_scope()->Declare(function_overloads_result.node());
      return function_overloads_result;
    }
    case Token::TS_CLASS:
      return ParseClassDeclaration(yield, has_default);
    case Token::TS_ENUM:
      return ParseEnumDeclaration(yield, has_default);
    case Token::TS_INTERFACE:
      return ParseInterfaceDeclaration();
    case Token::TS_LET:
    case Token::TS_CONST: {
      auto lexical_decl_result = ParseLexicalDeclaration(true, yield);
      if (IsLineTermination()) {
        ConsumeLineTerminator();
      } else {
        SYNTAX_ERROR("';' expected", Current());
      }
      return lexical_decl_result;
    }
    default:
      if (!error) {
        return Failed();
      }
      SYNTAX_ERROR("unexpected token", Current());
  }
}


template <typename UCharInputIterator>
ParseResult Parser<UCharInputIterator>::ParseBlockStatement(bool yield, bool has_return, bool breakable, bool continuable) {
  LOG_PHASE(ParseBlockStatement);
  Handle<ir::Scope> scope = NewScope();
  set_current_scope(scope);
  auto block_view = New<ir::BlockView>(scope);
  block_view->SetInformationForNode(Current());

  if (Current()->type() == Token::TS_LEFT_BRACE) {
    Next();
    bool success = true;
    while (1) {
      if (Current()->type() == Token::TS_RIGHT_BRACE) {
        Next();
        break;
      } else if (Current()->type() == Token::END_OF_INPUT) {
        SYNTAX_ERROR("Unexpected end of input.", Current());
      } else {
        auto statement_list_result = ParseStatementListItem(yield, has_return, breakable, continuable);
        SKIP_TOKEN_OR(statement_list_result, success, Token::TS_LEFT_BRACE) {
          block_view->InsertLast(statement_list_result.node());
        }
      }
    }
    set_current_scope(scope->parent_scope());
    return Success(block_view);
  }
  SYNTAX_ERROR("'{' expected", Current());
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
  if (Current()->type() == Token::TS_LET ||
      Current()->type() == Token::TS_CONST) {
    bool has_const = Current()->type() == Token::TS_CONST;
    Handle<ir::LexicalDeclView> lexical_decl = New<ir::LexicalDeclView>(Current()->type());
    lexical_decl->SetInformationForNode(Current());
    Next();
    bool success = false;
    while (1) {
      auto lexical_decl_result = ParseLexicalBinding(has_const, in, yield);
      SKIP_TOKEN_OR(lexical_decl_result, success, Token::LINE_TERMINATOR) {
        lexical_decl->InsertLast(lexical_decl_result.node());
      }
      if (Current()->type() == Token::TS_COMMA) {
        Next();
      } else {
        break;
      }
    }
    return Success(lexical_decl);
  }
  SYNTAX_ERROR("'let' or 'const' expected", Current());
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
  if (Current()->type() == Token::TS_IDENTIFIER) {
    lhs_result = ParseBindingIdentifier(false, in, yield);
  } else {
    lhs_result = ParseBindingPattern(yield, false);
  }

  CHECK_AST(lhs_result);

  if (!lhs_result.node()->IsValidLhs()) {
    SYNTAX_ERROR("left hand side of lexical binding is invalid", Current());
  }

  ParseResult type_expr_result;
  if (Current()->type() == Token::TS_COLON) {
    Next();
    type_expr_result = ParseTypeExpression();
    CHECK_AST(type_expr_result);
  }
  
  if (Current()->type() == Token::TS_ASSIGN) {
    Next();
    value_result = ParseAssignmentExpression(in, yield);
    CHECK_AST(value_result);
  } else if (const_decl) {
    SYNTAX_ERROR("const declaration must have an initializer", Current());
  }

  Handle<ir::Node> ret = New<ir::VariableView>(lhs_result.node(), value_result.node(), type_expr_result.node());
  ret->SetInformationForNode(lhs_result.node());
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
  if (Current()->type() == Token::TS_DEFAULT) {
    if (!default_allowed) {
      SYNTAX_ERROR("'default' keyword not allowed here", Current());
    }
    ret = New<ir::DefaultView>();
  } else if (Current()->type() == Token::TS_YIELD) {
    ret = New<ir::YieldView>(false, ir::Node::Null());
  } else if (Current()->type() == Token::TS_IDENTIFIER) {
    ret = New<ir::NameView>(NewSymbol(ir::SymbolType::kVariableName, Current()->value()));
  } else {
    SYNTAX_ERROR("'default', 'yield' or 'identifier' expected", Current());
  }
  
  ret->SetInformationForNode(Current());
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
  switch (Current()->type()) {
    case Token::TS_LEFT_BRACE:
      return ParseObjectBindingPattern(yield, generator_parameter);
    case Token::TS_LEFT_BRACKET:
      return ParseArrayBindingPattern(yield, generator_parameter);
    default:
      SYNTAX_ERROR("'[' or '{' expected", Current());
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
  if (Current()->type() == Token::TS_LEFT_BRACE) {
    Handle<ir::Node> binding_prop_list = New<ir::BindingPropListView>();
    binding_prop_list->SetInformationForNode(Current());
    Next();

    bool success = true;
    
    while (1) {
      auto binding_prop_result = ParseBindingProperty(yield, generator_parameter);
      
      SKIP_TOKEN_OR(binding_prop_result, success, Token::TS_RIGHT_BRACE) {
        binding_prop_list->InsertLast(binding_prop_result.node());
      }
      
      switch (Current()->type()) {
        case Token::TS_COMMA: {
          Next();
          break;
        }
        case Token::TS_RIGHT_BRACE: {
          Next();
          return Success(binding_prop_list);
        }
        default:
          SYNTAX_ERROR("unexpected token", Current());
      }
    }
  }
  SYNTAX_ERROR("'{' expected", Current());
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
  
  if (Current()->type() == Token::TS_LEFT_BRACKET) {
    
    Handle<ir::Node> binding_array = New<ir::BindingArrayView>();
    binding_array->SetInformationForNode(Current());
    Next();

    bool exit = false;
    bool success = true;
    
    while (1) {
      if (Current()->type() == Token::TS_COMMA) {
        Next();
        binding_array->InsertLast(ir::Node::Null());
      }
      if (Current()->type() == Token::TS_REST) {
        Handle<ir::RestParamView> rest = New<ir::RestParamView>();
        rest->SetInformationForNode(Current());
        Next();
        auto binding_identifier_result = ParseBindingIdentifier(false, true, yield);
        SKIP_TOKEN_OR(binding_identifier_result, success, Token::TS_RIGHT_BRACKET) {
          rest->set_parameter(binding_identifier_result.node());
          binding_array->InsertLast(rest);
          exit = true;
        }
      } else {
        auto binding_elem_result = ParseBindingElement(yield, generator_parameter);
        
        SKIP_TOKEN_OR(binding_elem_result, success, Token::TS_RIGHT_BRACKET) {
          ParseResult assignment_expr_result;
          if (Current()->type() == Token::TS_ASSIGN) {
            assignment_expr_result = ParseAssignmentExpression(true, yield);
          }
          auto ret = New<ir::BindingElementView>(ir::Node::Null(),
                                                 binding_elem_result.node(),
                                                 assignment_expr_result.node());
          ret->SetInformationForNode(binding_elem_result.node());
          binding_array->InsertLast(ret);
        }
      }

      if (Current()->type() == Token::TS_RIGHT_BRACKET) {
        Next();
        break;
      } else if (exit) {
        SYNTAX_ERROR("spread binding must be end of bindings", Current());
      } else if (Current()->type() == Token::TS_COMMA) {
        Next();
      } else {
        SYNTAX_ERROR("unexpected token.", Current());
      }
    }
    return Success(binding_array);
  }
  SYNTAX_ERROR("'[' expected", Current());
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

  if (Current()->type() == Token::TS_IDENTIFIER) {
    key_result = ParseIdentifier();
    CHECK_AST(key_result);
  } else {
    SYNTAX_ERROR("'identifier' expected", Current());
  }
  
  if (Current()->type() == Token::TS_COLON) {
    Next();
    elem_result = ParseBindingElement(yield, generator_parameter);
    CHECK_AST(elem_result);
  }

  if (Current()->type() == Token::TS_ASSIGN) {
    Next();
    init_result = ParseAssignmentExpression(true, yield);
    CHECK_AST(init_result);
  }
  Handle<ir::Node> ret = New<ir::BindingElementView>(key_result.node(), elem_result.node(), init_result.node());
  ret->SetInformationForNode(key_result.node());
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
  switch (Current()->type()) {
    case Token::TS_IDENTIFIER:
    case Token::TS_YIELD:
    case Token::TS_DEFAULT:
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
  if (Current()->type() == Token::TS_VAR) {
    Next();
    Handle<ir::VariableDeclView> vars = New<ir::VariableDeclView>();
    bool success = true;
    
    while (1) {
      auto variable_decl_result = ParseVariableDeclaration(in, yield);
      SKIP_TOKEN_OR(variable_decl_result, success, Token::LINE_TERMINATOR) {
        vars->InsertLast(variable_decl_result.node());
      }
      if (Current()->type() == Token::TS_COMMA) {
        Next();
      } else {
        break;
      }
    }
    
    return Success(vars);
  }
  SYNTAX_ERROR("'var' expected", Current());
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
  if (Current()->type() == Token::TS_IDENTIFIER) {
    lhs_result = ParseBindingIdentifier(false, in, yield);
  } else {
    lhs_result = ParseBindingPattern(yield, false);
  }

  CHECK_AST(lhs_result);

  if (!lhs_result.node()->IsValidLhs()) {
    SYNTAX_ERROR("left hand side of variable declaration is invalid", Current());
  }

  ParseResult type_expr_result;
  if (Current()->type() == Token::TS_COLON) {
    Next();
    type_expr_result = ParseTypeExpression();
    CHECK_AST(type_expr_result);
  }
  
  if (Current()->type() == Token::TS_ASSIGN) {
    Next();
    value_result = ParseAssignmentExpression(in, yield);
    CHECK_AST(value_result);
  }

  Handle<ir::Node> ret = New<ir::VariableView>(lhs_result.node(), value_result.node(), type_expr_result.node());
  ret->SetInformationForNode(lhs_result.node());
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
  if (Current()->type() == Token::TS_IF) {
    TokenInfo info = *Current();
    Next();
    if (Current()->type() == Token::TS_LEFT_PAREN) {
      Next();
      auto expr_result = ParseExpression(true, yield);
      CHECK_AST(expr_result);
      if (Current()->type() == Token::TS_RIGHT_PAREN) {
        Next();
        auto then_stmt_result = ParseStatement(yield, has_return, breakable, continuable);
        CHECK_AST(then_stmt_result);
        if (Prev()->type() != Token::TS_RIGHT_BRACE && IsLineTermination()) {
          ConsumeLineTerminator();
        }
        ParseResult else_stmt_result;
        if (Current()->type() == Token::TS_ELSE) {
          Next();
          else_stmt_result = ParseStatement(yield, has_return, breakable, continuable);
          CHECK_AST(else_stmt_result);
          if (Prev()->type() != Token::TS_RIGHT_BRACE && IsLineTermination()) {
            ConsumeLineTerminator();
          }
        }
        Handle<ir::IfStatementView> if_stmt = New<ir::IfStatementView>(expr_result.node(),
                                                                       then_stmt_result.node(),
                                                                       else_stmt_result.node());
        if_stmt->SetInformationForNode(&info);
        return Success(if_stmt);
      }
      SYNTAX_ERROR("')' expected", Current());
    }
    SYNTAX_ERROR("'(' expected", Current());
  }
  SYNTAX_ERROR("'if' expected", Current());
}


// while_statment
//   : 'while' '(' expression[in, ?yield] ')' statement[?yield, ?return]
//   ;
template <typename UCharInputIterator>
ParseResult Parser<UCharInputIterator>::ParseWhileStatement(bool yield, bool has_return) {
  LOG_PHASE(ParseWhileStatement);
  if (Current()->type() == Token::TS_WHILE) {
    TokenInfo info = *(Current());
    Next();
    if (Current()->type() == Token::TS_LEFT_PAREN) {
      Next();
      auto expr_result = ParseExpression(true, yield);
      CHECK_AST(expr_result);
      auto iteration_body_result = ParseIterationBody(yield, has_return);
      CHECK_AST(iteration_body_result);
      auto while_stmt = New<ir::WhileStatementView>(expr_result.node(), iteration_body_result.node());
      while_stmt->SetInformationForNode(&info);
      return Success(while_stmt);
    }
    SYNTAX_ERROR("'(' expected", Current());
  }
  SYNTAX_ERROR("'while' expected", Current());
}


// do_while_statement
//   : 'do' statement[?yield, ?return] 'while' '(' expression[in, ?yield] ')' ;__opt
//   ;
template <typename UCharInputIterator>
ParseResult Parser<UCharInputIterator>::ParseDoWhileStatement(bool yield, bool has_return) {
  LOG_PHASE(ParseDoWhileStatement);
  if (Current()->type() == Token::TS_DO) {
    TokenInfo info = *(Current());
    Next();
    auto stmt_result = ParseStatement(yield, has_return, true, true);
    CHECK_AST(stmt_result);
    if (Current()->type() == Token::TS_WHILE) {
      Next();
      if (Current()->type() == Token::TS_LEFT_PAREN) {
        Next();
        auto expr_result = ParseExpression(true, yield);
        CHECK_AST(expr_result);
        if (Current()->type() == Token::TS_RIGHT_PAREN) {
          Next();
          if (IsLineTermination()) {
            ConsumeLineTerminator();
          }
          auto do_while = New<ir::DoWhileStatementView>(expr_result.node(), stmt_result.node());
          do_while->SetInformationForNode(&info);
          return Success(do_while);
        }
        SYNTAX_ERROR("')' expected", Current());
      }
      SYNTAX_ERROR("'(' expected", Current());
    }
    SYNTAX_ERROR("'while' expected", Current());
  }
  SYNTAX_ERROR("'do' expected", Current());
}


template <typename UCharInputIterator>
ParseResult Parser<UCharInputIterator>::ParseForStatement(bool yield, bool has_return) {
  LOG_PHASE(ParseForStatement);
  if (Current()->type() == Token::TS_FOR) {
    TokenInfo info = *Current();
    Next();
    if (Current()->type() == Token::TS_LEFT_PAREN) {
      Next();
      ParseResult reciever_result;
      switch (Current()->type()) {
        case Token::TS_VAR: {
          reciever_result = ParseVariableStatement(true, yield);
          CHECK_AST(reciever_result);
          break;
        }
        case Token::TS_LET: {
          reciever_result = ParseLexicalDeclaration(true, yield);
          CHECK_AST(reciever_result);
          break;
        }
        case Token::LINE_TERMINATOR:
          break;
        default: {
          RecordedParserState rps = parser_state();
          reciever_result = ParseExpression(true, yield);
          CHECK_AST(reciever_result);
          if (Current()->type() != Token::LINE_TERMINATOR) {
            RestoreParserState(rps);
            reciever_result = ParseLeftHandSideExpression(yield);
            CHECK_AST(reciever_result);
          }
        }
      }
      return ParseForIteration(reciever_result.node(), &info, yield, has_return);
    }
    SYNTAX_ERROR("'(' expected", Current());
  }
  SYNTAX_ERROR("'for' expected", Current());
}


template <typename UCharInputIterator>
ParseResult Parser<UCharInputIterator>::ParseForIteration(Handle<ir::Node> reciever, TokenInfo* token_info, bool yield, bool has_return) {
  LOG_PHASE(ParseForIterationStatement);

  ParseResult second_result;
  ParseResult third_result;
  bool for_in = false;
  bool for_of = false;
  
  if (Current()->type() == Token::LINE_TERMINATOR) {
    // for (var i = 0; i < 10; i++) ...
    Next();
    if (Current()->type() != Token::LINE_TERMINATOR) {
      second_result = ParseExpression(true, yield);
      CHECK_AST(second_result);
    }
    if (Current()->type() == Token::LINE_TERMINATOR) {
      Next();
      if (Current()->type() != Token::TS_RIGHT_PAREN) {
        third_result = ParseExpression(true, yield);
        CHECK_AST(third_result);
      }
      
    }
  } else if (Current()->type() == Token::TS_IDENTIFIER &&
             Current()->value()->Equals("of")) {
    // for (var i in obj) ...
    Next();
    second_result = ParseAssignmentExpression(true, yield);
    CHECK_AST(second_result);
    for_of = true;
  } else if (Current()->type() == Token::TS_IN) {
    // for (var i of obj) ...
    Next();
    second_result = ParseAssignmentExpression(false, yield);
    CHECK_AST(second_result);
    for_in = true;
  } else {
    SYNTAX_ERROR("'in' or 'of' or ';' expected", Current());
  }

  auto iteration_body_result = ParseIterationBody(yield, has_return);
  Handle<ir::Node> ret;
  
  if (for_in) {
    ret = New<ir::ForInStatementView>(reciever, second_result.node(), iteration_body_result.node());
  } else if (for_of) {
    ret = New<ir::ForOfStatementView>(reciever, second_result.node(), iteration_body_result.node());
  } else {
    ret = New<ir::ForStatementView>(reciever, second_result.node(), third_result.node(), iteration_body_result.node());
  }
    
  ret->SetInformationForNode(token_info);
  return Success(ret);
}


template <typename UCharInputIterator>
ParseResult Parser<UCharInputIterator>::ParseIterationBody(bool yield, bool has_return) {
  if (Current()->type() == Token::TS_RIGHT_PAREN) {
    Next();
    return ParseStatement(yield, has_return, true, true);
  }
  SYNTAX_ERROR("')' expected", Current());
}


template <typename UCharInputIterator>
ParseResult Parser<UCharInputIterator>::ParseContinueStatement(bool yield) {
  LOG_PHASE(ParseContinueStatement);
  if (Current()->type() == Token::TS_CONTINUE) {
    TokenInfo info = *Current();
    Next();
    ParseResult parse_result;
    YATSC_SCOPED([&]{
      if (parse_result) {
        parse_result.node()->SetInformationForNode(&info);
      }
    });
    if (Current()->type() == Token::TS_IDENTIFIER) {
      auto primary_expr_result = ParsePrimaryExpression(yield);
      CHECK_AST(primary_expr_result);
      return parse_result = Success(New<ir::ContinueStatementView>(primary_expr_result.node()));
    }
    return parse_result = Success(New<ir::ContinueStatementView>());
  }
  SYNTAX_ERROR("'continue' expected", Current());
}


template <typename UCharInputIterator>
ParseResult Parser<UCharInputIterator>::ParseBreakStatement(bool yield) {
  LOG_PHASE(ParseBreakStatement);
  if (Current()->type() == Token::TS_BREAK) {
    TokenInfo info = *Current();
    Next();
    ParseResult parse_result;
    YATSC_SCOPED([&]{
      if (parse_result) {
        parse_result.node()->SetInformationForNode(&info);
      }
    });
    if (Current()->type() == Token::TS_IDENTIFIER) {
      auto primary_expr_result = ParsePrimaryExpression(yield);
      return parse_result = Success(New<ir::BreakStatementView>(primary_expr_result.node()));
    }
    return parse_result = Success(New<ir::BreakStatementView>());
  }
  SYNTAX_ERROR("'break' expected", Current());
}


template <typename UCharInputIterator>
ParseResult Parser<UCharInputIterator>::ParseReturnStatement(bool yield) {
  LOG_PHASE(ParseReturnStatement);
  if (Current()->type() == Token::TS_RETURN) {
    TokenInfo info = *Current();
    ParseResult parse_result;
    YATSC_SCOPED([&]{
      if (parse_result) {
        parse_result.node()->SetInformationForNode(&info);
      }
    });
    
    Next();
    
    if (IsLineTermination()) {
      ConsumeLineTerminator();
      return parse_result = Success(New<ir::ReturnStatementView>());
    }
    auto expr_result = ParseExpression(true, yield);
    CHECK_AST(expr_result);
    return parse_result = Success(New<ir::ReturnStatementView>(expr_result.node()));
  }
  SYNTAX_ERROR("'return' expected", Current());
}


template <typename UCharInputIterator>
ParseResult Parser<UCharInputIterator>::ParseWithStatement(bool yield, bool has_return, bool breakable, bool continuable) {
  LOG_PHASE(ParseWithStatement);
  if (Current()->type() == Token::TS_WITH) {
    TokenInfo info = *Current();
    ParseResult parse_result;
    YATSC_SCOPED([&]{
      if (parse_result) {
        parse_result.node()->SetInformationForNode(&info);
      }
    });
    
    Next();
    
    if (Current()->type() == Token::TS_LEFT_PAREN) {
      Next();
      auto expr_result = ParseExpression(true, yield);
      CHECK_AST(expr_result);
      if (Current()->type() == Token::TS_RIGHT_PAREN) {
        Next();
        auto stmt_result = ParseStatement(yield, has_return, breakable, continuable);
        CHECK_AST(stmt_result);
        return parse_result = Success(New<ir::WithStatementView>(expr_result.node(), stmt_result.node()));
      }
      SYNTAX_ERROR("')' expected", Current());
    }
    SYNTAX_ERROR("'(' expected", Current());
  }
  SYNTAX_ERROR("'with' expected", Current());
}


template <typename UCharInputIterator>
ParseResult Parser<UCharInputIterator>::ParseSwitchStatement(bool yield, bool has_return, bool continuable) {
  LOG_PHASE(ParseSwitchStatement);
  if (Current()->type() == Token::TS_SWITCH) {
    TokenInfo info = *Current();
    Next();
    if (Current()->type() == Token::TS_LEFT_PAREN) {
      Next();
      auto expr_result = ParseExpression(true, yield);
      CHECK_AST(expr_result);
      if (Current()->type() == Token::TS_RIGHT_PAREN) {
        Next();
        if (Current()->type() == Token::TS_LEFT_BRACE) {
          Next();
          auto case_clauses_result = ParseCaseClauses(yield, has_return, continuable);
          CHECK_AST(case_clauses_result);
          if (Current()->type() == Token::TS_RIGHT_BRACE) {
            Next();
            auto switch_stmt = New<ir::SwitchStatementView>(expr_result.node(), case_clauses_result.node());
            switch_stmt->SetInformationForNode(&info);
            return Success(switch_stmt);
          }
          SYNTAX_ERROR("'}' expected", Current());
        }
        SYNTAX_ERROR("'{' expected", Current());
      }
      SYNTAX_ERROR("')' expected", Current());
    }
    SYNTAX_ERROR("'(' expected", Current());
  }
  SYNTAX_ERROR("'switch' expected", Current());
}


template <typename UCharInputIterator>
ParseResult Parser<UCharInputIterator>::ParseCaseClauses(bool yield, bool has_return, bool continuable) {
  LOG_PHASE(ParseCaseClauses);
  Handle<ir::CaseListView> case_list = New<ir::CaseListView>();
  case_list->SetInformationForNode(Current());
  bool default_encounted = false;
  bool success = true;
  
  while (1) {
    bool normal_case = false;
    ParseResult expr_result;
    TokenInfo info = *Current();
    
    switch (Current()->type()) {
      case Token::TS_CASE: {
        normal_case = true;
        Next();
        expr_result = ParseExpression(true, yield);
        SKIP_TOKEN_IF_AND(expr_result, success, Token::TS_RIGHT_BRACE, break);
      }
      case Token::TS_DEFAULT: {
        if (!normal_case) {
          default_encounted = true;
          Next();
        }
        if (Current()->type() == Token::TS_COLON) {
          Next();
        } else {
          SYNTAX_ERROR("':' expected", Current());
        }
        Handle<ir::Node> body = New<ir::CaseBody>();
        while (1) {
          if (Current()->type() == Token::TS_CASE ||
              Current()->type() == Token::TS_DEFAULT) {
            if (default_encounted && Current()->type() == Token::TS_DEFAULT) {
              SYNTAX_ERROR("More than one 'default' clause in switch statement", Current());
            }
            break;
          } else if (Current()->type() == Token::TS_RIGHT_BRACE) {
            break;
          }
          if (Current()->type() == Token::TS_LEFT_BRACE) {
            auto block_stmt_result = ParseBlockStatement(yield, has_return, true, continuable);
            SKIP_TOKEN_OR(block_stmt_result, success, Token::TS_RIGHT_BRACE) {
              body->InsertLast(block_stmt_result.node());
            }
          } else {
            auto stmt_list_result = ParseStatementListItem(yield, has_return, true, continuable);
            SKIP_TOKEN_OR(stmt_list_result, success, Token::TS_RIGHT_BRACE) {
              body->InsertLast(stmt_list_result.node());
            }
          }
        }
        
        Handle<ir::CaseView> case_view = New<ir::CaseView>(expr_result.node(), body);
        case_view->SetInformationForNode(&info);
        case_list->InsertLast(case_view);
        break;
      }
      case Token::TS_RIGHT_BRACE: {
        return Success(case_list);
      }
      default:
        SYNTAX_ERROR("unexpected token", Current());
    }
  }
}


template <typename UCharInputIterator>
ParseResult Parser<UCharInputIterator>::ParseLabelledStatement(bool yield, bool has_return, bool breakable, bool continuable) {
  if (Current()->type() == Token::TS_IDENTIFIER) {
    auto label_identifier_result = ParseLabelIdentifier(yield);
    CHECK_AST(label_identifier_result);
    label_identifier_result.node()->symbol()->set_type(ir::SymbolType::kLabelName);

    current_scope()->Declare(label_identifier_result.node());
    
    if (Current()->type() == Token::TS_COLON) {
      Next();
      auto lebelled_item_result = ParseLabelledItem(yield, has_return, breakable, continuable);
      CHECK_AST(lebelled_item_result);
      auto node = New<ir::LabelledStatementView>(label_identifier_result.node(), lebelled_item_result.node());
      node->SetInformationForNode(label_identifier_result.node());
      return Success(node);
    }
    SYNTAX_ERROR("':' expected", Current());
  }
  SYNTAX_ERROR("'identifier' expected", Current());
}


template <typename UCharInputIterator>
ParseResult Parser<UCharInputIterator>::ParseLabelledItem(bool yield, bool has_return, bool breakable, bool continuable) {
  if (Current()->type() == Token::TS_FUNCTION) {
    return ParseFunctionOverloads(yield, false, true, false);
  }
  return ParseStatement(yield, has_return, breakable, continuable);
}


template <typename UCharInputIterator>
ParseResult Parser<UCharInputIterator>::ParseThrowStatement() {
  LOG_PHASE(ParseThrowStatement);
  if (Current()->type() == Token::TS_THROW) {
    TokenInfo info = *Current();
    Next();
    if (!IsLineTermination()) {
      auto expr_result = ParseExpression(false, false);
      CHECK_AST(expr_result);
      Handle<ir::ThrowStatementView> throw_stmt = New<ir::ThrowStatementView>(expr_result.node());
      throw_stmt->SetInformationForNode(&info);
      return Success(throw_stmt);
    }
    SYNTAX_ERROR("throw statement expected expression", (&info));
  }
  SYNTAX_ERROR("throw expected", Current());
}


template <typename UCharInputIterator>
ParseResult Parser<UCharInputIterator>::ParseTryStatement(bool yield, bool has_return, bool breakable, bool continuable) {
  LOG_PHASE(ParseTryStatement);
  if (Current()->type() == Token::TS_TRY) {
    TokenInfo info = *Current();
    Next();
    auto block_stmt_result = ParseBlockStatement(yield, has_return, breakable, continuable);
    CHECK_AST(block_stmt_result);
    ParseResult catch_block_result;
    ParseResult finally_block_result;
    bool has_catch_or_finally = false;

    if (Current()->type() == Token::TS_CATCH) {
      has_catch_or_finally = true;
      catch_block_result = ParseCatchBlock(yield, has_return, breakable, continuable);
      CHECK_AST(catch_block_result);
    }
    
    if (Current()->type() == Token::TS_FINALLY) {
      has_catch_or_finally = true;
      finally_block_result = ParseFinallyBlock(yield, has_return, breakable, continuable);
      CHECK_AST(finally_block_result);
    }

    if (!has_catch_or_finally) {
      SYNTAX_ERROR("try statement need catch block or finally block", Current());
    }
    
    Handle<ir::TryStatementView> try_stmt = New<ir::TryStatementView>(block_stmt_result.node(),
                                                                      catch_block_result.node(),
                                                                      finally_block_result.node());
    try_stmt->SetInformationForNode(&info);
    return Success(try_stmt);
  }
  SYNTAX_ERROR("'try' expected", Current());
}


template <typename UCharInputIterator>
ParseResult Parser<UCharInputIterator>::ParseCatchBlock(bool yield, bool has_return, bool breakable, bool continuable) {
  LOG_PHASE(ParseCatchBlock);
  if (Current()->type() == Token::TS_CATCH) {
    TokenInfo info = *Current();
    Next();
    if (Current()->type() == Token::TS_LEFT_PAREN) {
      Next();
      ParseResult catch_parameter_result;
      if (Current()->type() == Token::TS_IDENTIFIER) {
        catch_parameter_result = ParseBindingIdentifier(false, false, yield);
      } else {
        catch_parameter_result = ParseBindingPattern(yield, false);
      }
      CHECK_AST(catch_parameter_result);

      if (Current()->type() == Token::TS_RIGHT_PAREN) {
        Next();
        auto block_stmt_result = ParseBlockStatement(yield, has_return, breakable, continuable);
        CHECK_AST(block_stmt_result);
        Handle<ir::CatchStatementView> catch_stmt = New<ir::CatchStatementView>(catch_parameter_result.node(),
                                                                                block_stmt_result.node());
        catch_stmt->SetInformationForNode(&info);
        return Success(catch_stmt);
      }
      SYNTAX_ERROR("')' expected", Current());
    }
    SYNTAX_ERROR("'(' expected", Current());
  }
  SYNTAX_ERROR("'catch' expected", Current());
}


template <typename UCharInputIterator>
ParseResult Parser<UCharInputIterator>::ParseFinallyBlock(bool yield, bool has_return, bool breakable, bool continuable) {
  LOG_PHASE(ParseFinallyBlock);
  if (Current()->type() == Token::TS_FINALLY) {
    TokenInfo info = *Current();
    Next();
    auto block_stmt_result = ParseBlockStatement(yield, has_return, breakable, continuable);
    CHECK_AST(block_stmt_result);
    Handle<ir::FinallyStatementView> finally_stmt = New<ir::FinallyStatementView>(block_stmt_result.node());
    finally_stmt->SetInformationForNode(&info);
    return Success(finally_stmt);
  }
  SYNTAX_ERROR("'finally' expected", Current());
}

template <typename UCharInputIterator>
ParseResult Parser<UCharInputIterator>::ParseDebuggerStatement() {
  LOG_PHASE(ParseDebuggerStatement);
  if (Current()->type() == Token::TS_DEBUGGER) {
    Handle<ir::Node> ret = New<ir::DebuggerView>();
    ret->SetInformationForNode(Current());
    Next();
    return Success(ret);
  }
  SYNTAX_ERROR("'debugger' expected", Current());
}


template <typename UCharInputIterator>
ParseResult Parser<UCharInputIterator>::ParseInterfaceDeclaration() {
  LOG_PHASE(ParseInterfaceDeclaration);
  bool success = true;
  if (Current()->type() == Token::TS_INTERFACE) {
    Next();
    auto identifier_result = ParseIdentifier();
    SKIP_TOKEN_IF_AND(identifier_result, success, Token::TS_RIGHT_BRACE, return Failed());
    ParseResult type_parameters_result;

    identifier_result.node()->symbol()->set_type(ir::SymbolType::kInterfaceName);
    
    if (Current()->type() == Token::TS_LESS) {
      type_parameters_result = ParseTypeParameters();
      SKIP_TOKEN_IF_AND(type_parameters_result, success, Token::TS_RIGHT_BRACE, return Failed());
    }
    
    auto extends = New<ir::InterfaceExtendsView>();

    if (Current()->type() == Token::TS_EXTENDS) {
      Next();
      while (1) {
        auto ref_type_result = ParseReferencedType();
        SKIP_TOKEN_OR(ref_type_result, success, Token::TS_LEFT_BRACE) {
          extends->InsertLast(ref_type_result.node());
        }
        if (Current()->type() != Token::TS_LEFT_BRACE) {
          Next();
        } else if (Current()->type() == Token::END_OF_INPUT) {
          SYNTAX_ERROR("unexpected end of input.", Current());
        } else {
          break;
        }
      }
    }

    if (Current()->type() == Token::TS_LEFT_BRACE) {
      auto interface_body_result = ParseObjectTypeExpression();
      return Success(New<ir::InterfaceView>(identifier_result.node(),
                                            type_parameters_result.node(),
                                            extends,
                                            interface_body_result.node()));
    }
    SYNTAX_ERROR("'{' expected.", Current());
  }
  SYNTAX_ERROR("'interface' expected.", Current());
}


template <typename UCharInputIterator>
ParseResult Parser<UCharInputIterator>::ParseEnumDeclaration(bool yield, bool has_default) {
  LOG_PHASE(ParseEnumDeclaration);
  bool success = true;
  
  if (Current()->type() == Token::TS_ENUM) {
    TokenInfo info = *Current();
    Next();
    auto identifier_result = ParseIdentifier();
    SKIP_TOKEN_OR(identifier_result, success, Token::TS_LEFT_BRACE) {
      identifier_result.node()->symbol()->set_type(ir::SymbolType::kEnumName);
    }
    
    if (Current()->type() == Token::TS_LEFT_BRACE) {
      auto enum_body_result = ParseEnumBody(yield, has_default);
      CHECK_AST(enum_body_result);
      auto ret = New<ir::EnumDeclView>(identifier_result.node(), enum_body_result.node());
      ret->SetInformationForNode(&info);
      return Success(ret);
    }
    SYNTAX_ERROR("'{' expected.", Current());
  }
  SYNTAX_ERROR("'enum' expected.", Current());
}


template <typename UCharInputIterator>
ParseResult Parser<UCharInputIterator>::ParseEnumBody(bool yield, bool has_default) {
  LOG_PHASE(ParseEnumBody);
  if (Current()->type() == Token::TS_LEFT_BRACE) {
    auto ret = New<ir::EnumBodyView>();
    ret->SetInformationForNode(Current());
    Next();
    
    if (Current()->type() == Token::TS_RIGHT_BRACE) {
      Next();
      return Success(ret);
    }

    bool success = true;
    
    while (1) {
      auto enum_property_result = ParseEnumProperty(yield, has_default);
      SKIP_TOKEN_OR(enum_property_result, success, Token::TS_RIGHT_BRACE) {
        ret->InsertLast(enum_property_result.node());
      }
      if (Current()->type() == Token::TS_COMMA) {
        Next();
        if (Current()->type() == Token::TS_RIGHT_BRACE) {
          Next();
          return Success(ret);
        }
      } else if (Current()->type() == Token::TS_RIGHT_BRACE) {
        Next();
        return Success(ret);
      } else {
        SYNTAX_ERROR("',' or '}' expected.", Current());
      }
    }
  }
  SYNTAX_ERROR("'{' expected.", Current());
}


template <typename UCharInputIterator>
ParseResult Parser<UCharInputIterator>::ParseEnumProperty(bool yield, bool has_default) {
  LOG_PHASE(ParseEnumProperty);
  auto prop_name_result = ParsePropertyName(yield, false);
  CHECK_AST(prop_name_result);
  if (Current()->type() == Token::TS_ASSIGN) {
    Next();
    auto assignment_expr_result = ParseAssignmentExpression(true, yield);
    CHECK_AST(assignment_expr_result);
    return Success(CreateEnumFieldView(prop_name_result.node(), assignment_expr_result.node()));
  }
  return Success(CreateEnumFieldView(prop_name_result.node(), ir::Node::Null()));
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
  if (Current()->type() == Token::TS_CLASS) {
    TokenInfo info = *Current();
    Next();
    ParseResult type_parameters_result;
    auto identifier_result = ParseIdentifier();
    bool success = true;
    
    SKIP_TOKEN_IF_AND(identifier_result, success, Token::TS_LEFT_BRACE, return Failed());
    
    identifier_result.node()->symbol()->set_type(ir::SymbolType::kClassName);
    
    if (Current()->type() == Token::TS_LESS) {
      type_parameters_result = ParseTypeParameters();
      SKIP_TOKEN_IF(type_parameters_result, success, Token::TS_LEFT_BRACE);
    }
    
    auto class_bases_result = ParseClassBases();
    SKIP_TOKEN_IF(class_bases_result, success, Token::TS_LEFT_BRACE);
    
    if (Current()->type() == Token::TS_LEFT_BRACE) {
      Next();
      auto class_body_result = ParseClassBody();
      SKIP_TOKEN_IF(class_body_result, success, Token::TS_RIGHT_BRACE);
      auto class_decl = New<ir::ClassDeclView>(identifier_result.node(),
                                               type_parameters_result.node(),
                                               class_bases_result.node(),
                                               class_body_result.node());
      class_decl->SetInformationForNode(&info);
      return Success(class_decl);
    }
    SYNTAX_ERROR("'{' expected", Current());
  }
  SYNTAX_ERROR("'class' expected", Current());
}


template <typename UCharInputIterator>
ParseResult Parser<UCharInputIterator>::ParseClassBases() {
  LOG_PHASE(ParseClassBases);
  bool extends_keyword = false;
  bool success = true;
  
  auto bases = New<ir::ClassBasesView>();
  auto impls = New<ir::ClassImplsView>();
  bases->SetInformationForNode(Current());
  
  while (1) {
    if (Current()->type() == Token::TS_EXTENDS) {
      if (extends_keyword) {
        SYNTAX_ERROR("class extendable only one class", Current());
      }
      TokenInfo info = *Current();
      Next();
      extends_keyword = true;
      auto ref_type_result = ParseReferencedType();
      
      SKIP_TOKEN_OR(ref_type_result, success, Token::TS_LEFT_BRACE) {
        auto heritage = New<ir::ClassHeritageView>(ref_type_result.node());
        heritage->SetInformationForNode(&info);
        bases->set_base(heritage);
      }
    } else if (Current()->type() == Token::TS_IMPLEMENTS) {
      Next();
      while (1) {
        auto ref_type_result = ParseReferencedType();
        SKIP_TOKEN_OR(ref_type_result, success, Token::TS_LEFT_BRACE) {
          impls->InsertLast(ref_type_result.node());
        }
        if (Current()->type() != Token::TS_COMMA) {
          break;
        }
        Next();
      }
    } else if (Current()->type() == Token::TS_LEFT_BRACE) {
      if (impls->size() > 0) {
        bases->set_impls(impls);
      }
      return Success(bases);
    } else {
      SYNTAX_ERROR("unexpected token", Current());
    }
  }
}


template <typename UCharInputIterator>
ParseResult Parser<UCharInputIterator>::ParseClassBody() {
  LOG_PHASE(ParseClassBody);
  auto fields = New<ir::ClassFieldListView>();
  bool success = true;
  while (1) {
    if (Current()->type() != Token::TS_RIGHT_BRACE) {
      auto class_element_result = ParseClassElement();
      SKIP_TOKEN_OR(class_element_result, success, Token::LINE_TERMINATOR) {
        fields->InsertLast(class_element_result.node());
      }
      if (IsLineTermination()) {
        ConsumeLineTerminator();
      } else if (Current()->type() != Token::TS_RIGHT_BRACE &&
                 Prev()->type() != Token::TS_RIGHT_BRACE) {
        SYNTAX_ERROR("';' expected", Prev());
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
  
  if (Current()->type() == Token::TS_LEFT_BRACKET) {
    return ParseIndexSignature();
  }
  
  auto field_modifiers_result = ParseFieldModifiers();
  CHECK_AST(field_modifiers_result);
  AccessorType at = ParseAccessor();

  if (TokenInfo::IsKeyword(Current()->type())) {
    Current()->set_type(Token::TS_IDENTIFIER);
  }
  
  if (Current()->type() == Token::TS_IDENTIFIER) {
    if (Current()->value()->Equals("constructor")) {
      return ParseConstructorOverloads(field_modifiers_result.node());
    } else {
      RecordedParserState rps = parser_state();
      Next();
      if (Current()->type() == Token::TS_LEFT_PAREN ||
          Current()->type() == Token::TS_LESS) {
        RestoreParserState(rps);
        return ParseMemberFunctionOverloads(field_modifiers_result.node(), &at);
      } else {
        RestoreParserState(rps);
        return ParseMemberVariable(field_modifiers_result.node());
      }
    }
  } else if (Current()->type() == Token::TS_MUL) {
    Next();
    return ParseGeneratorMethodOverloads(field_modifiers_result.node());
  }
  SYNTAX_ERROR("unexpected token", Current());
}


template <typename UCharInputIterator>
ParseResult Parser<UCharInputIterator>::ParseFieldModifiers() {
  LOG_PHASE(ParseFieldModifiers);
  
  auto mods = New<ir::ClassFieldModifiersView>();
  mods->SetInformationForNode(Current());
  
  if (Current()->type() == Token::TS_STATIC) {
    auto field_modifier_result = ParseFieldModifier();
    CHECK_AST(field_modifier_result);
    mods->InsertLast(field_modifier_result.node());
    if (Current()->type() == Token::TS_PUBLIC ||
        Current()->type() == Token::TS_PROTECTED ||
        Current()->type() == Token::TS_PRIVATE) {
      field_modifier_result = ParseFieldModifier();
      CHECK_AST(field_modifier_result);
      mods->InsertLast(field_modifier_result.node());
    }
  } else if (Current()->type() == Token::TS_PUBLIC ||
             Current()->type() == Token::TS_PROTECTED ||
             Current()->type() == Token::TS_PRIVATE) {
    auto field_modifier_result1 = ParseFieldModifier();
    CHECK_AST(field_modifier_result1);
    if (Current()->type() == Token::TS_STATIC) {
      auto field_modifier_result2 = ParseFieldModifier();
      CHECK_AST(field_modifier_result2);
      mods->InsertLast(field_modifier_result2.node());
      mods->InsertLast(field_modifier_result1.node());
    } else {
      mods->InsertLast(field_modifier_result1.node());
    }
  } else {
    auto pub = New<ir::ClassFieldAccessLevelView>(Token::TS_PUBLIC);
    pub->SetInformationForNode(Current());
    mods->InsertLast(pub);
  }
  
  return Success(mods);
}


template <typename UCharInputIterator>
ParseResult Parser<UCharInputIterator>::ParseFieldModifier() {
  LOG_PHASE(ParseFieldModifier);
  switch (Current()->type()) {
    case Token::TS_STATIC:
    case Token::TS_PUBLIC:
    case Token::TS_PROTECTED:
    case Token::TS_PRIVATE: {
      auto node = New<ir::ClassFieldAccessLevelView>(Current()->type());
      node->SetInformationForNode(Current());
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
    if ((Current()->type() == Token::TS_IDENTIFIER &&
         Current()->value()->Equals("constructor")) ||
        Current()->type() == Token::TS_PUBLIC ||
        Current()->type() == Token::TS_PRIVATE ||
        Current()->type() == Token::TS_PROTECTED) {
      auto constructor_overload_result = ParseConstructorOverloadOrImplementation(first, mods, overloads);
      CHECK_AST(constructor_overload_result);
      if (constructor_overload_result.node()->HasMemberFunctionOverloadView()) {
        overloads->InsertLast(constructor_overload_result.node());
        ValidateOverload(Handle<ir::MemberFunctionOverloadView>(constructor_overload_result.node()), overloads);
      } else {
        ValidateOverload(Handle<ir::MemberFunctionView>(constructor_overload_result.node()), overloads);
        return constructor_overload_result;
      }
    } else {
      SYNTAX_ERROR("incomplete constructor definition", Current());
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
    mods = field_modifiers_result.node();
  }
  
  if (Current()->type() == Token::TS_IDENTIFIER &&
      Current()->value()->Equals("constructor")) {
    TokenInfo info = *Current();
    auto identifier_result = ParseIdentifier();
    CHECK_AST(identifier_result);
    auto call_sig_result = ParseCallSignature(true, false);
    CHECK_AST(call_sig_result);
    Handle<ir::Node> ret;
    if (Current()->type() == Token::TS_LEFT_BRACE) {
      auto function_body_result = ParseFunctionBody(false);
      CHECK_AST(function_body_result);
      ret = New<ir::MemberFunctionView>(mods,
                                        identifier_result.node(),
                                        call_sig_result.node(),
                                        overloads,
                                        function_body_result.node());
    } else if (overloads) {
      ret = New<ir::MemberFunctionOverloadView>(mods, identifier_result.node(), call_sig_result.node());
      if (IsLineTermination()) {
        ConsumeLineTerminator();
      } else {
        SYNTAX_ERROR("';' expected", Current());
      }
    } else {
      SYNTAX_ERROR("invalid constructor definition", (&info));
    }
    ret->SetInformationForNode(mods);
    return Success(ret);
  }
  SYNTAX_ERROR("'constructor' expected", Current());
}


// Check member function begging token.
template <typename UCharInputIterator>
bool Parser<UCharInputIterator>::IsMemberFunctionOverloadsBegin(TokenInfo* info) {
  return info->type() == Token::TS_IDENTIFIER ||
    info->type() == Token::TS_PUBLIC ||
    info->type() == Token::TS_PRIVATE ||
    info->type() == Token::TS_STATIC ||
    info->type() == Token::TS_PROTECTED ||
    TokenInfo::IsKeyword(info->type());
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
    TokenInfo info = *Current();
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
      if (member_function_result.node()->HasMemberFunctionOverloadView()) {
        ValidateOverload(Handle<ir::MemberFunctionOverloadView>(member_function_result.node()), overloads);
        overloads->InsertLast(member_function_result.node());
      } else {
        // Else, return node.
        ValidateOverload(Handle<ir::MemberFunctionView>(member_function_result.node()), overloads);
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
  
  AccessorType at(false, false, TokenInfo());

  // If this method is not a first function that is parsed from ParseMemberFunctionOverloads,
  // parse modifiers.
  if (!first) {
    auto field_modifiers_result = ParseFieldModifiers();
    CHECK_AST(field_modifiers_result);
    mods = field_modifiers_result.node();
    at = ParseAccessor();
  } else {
    at = *acessor_type;
  }


  if (TokenInfo::IsKeyword(Current()->type())) {
    Current()->set_type(Token::TS_IDENTIFIER);
  }
  

  // Method must be began an js identifier.
  if (Current()->type() == Token::TS_IDENTIFIER) {
    // Save position.
    TokenInfo info = *Current();

    auto identifier_result = ParseIdentifier();
    CHECK_AST(identifier_result);
    auto call_sig_result = ParseCallSignature(false, false);
    CHECK_AST(call_sig_result);
    Handle<ir::Node> ret;

    // public something(): void {
    // -------------------------^ here
    if (Current()->type() == Token::TS_LEFT_BRACE) {
      auto function_body_result = ParseFunctionBody(false);
      CHECK_AST(function_body_result);
      ret = New<ir::MemberFunctionView>(at.getter, at.setter, false,
                                        mods,
                                        identifier_result.node(),
                                        call_sig_result.node(),
                                        overloads,
                                        function_body_result.node());
    } else if (overloads) {
      
      // Getter and setter is not allowed to overload function declaration.
      if (at.getter || at.setter) {
        SYNTAX_ERROR("overload is not allowed to getter and setter.", (&info));
      }
      ret = New<ir::MemberFunctionOverloadView>(mods, identifier_result.node(), call_sig_result.node());
      if (IsLineTermination()) {
        ConsumeLineTerminator();
      } else {
        SYNTAX_ERROR("';' expected", Current());
      }
    } else {
      SYNTAX_ERROR("invalid member function definition", (&info));
    }
    ret->SetInformationForNode(mods);
    return Success(ret);
  }
  SYNTAX_ERROR("identifier expected", Current());
}


template <typename UCharInputIterator>
ParseResult Parser<UCharInputIterator>::ParseGeneratorMethodOverloads(Handle<ir::Node> mods) {
  LOG_PHASE(ParseGeneratorMethodOverloads);
  auto overloads = New<ir::MemberFunctionOverloadsView>();
  bool first = true;
  
  while (1) {
    TokenInfo info = *Current();
    RecordedParserState rps = parser_state();
    Next();
    if (IsMemberFunctionOverloadsBegin(&info)) {
      RestoreParserState(rps);
      auto generator_result = ParseGeneratorMethodOverloadOrImplementation(first, mods, overloads);
      CHECK_AST(generator_result);
      if (generator_result.node()->HasMemberFunctionOverloadView()) {
        ValidateOverload(Handle<ir::MemberFunctionOverloadView>(generator_result.node()), overloads);
        overloads->InsertLast(generator_result.node());
      } else {
        ValidateOverload(Handle<ir::MemberFunctionView>(generator_result.node()), overloads);
        return generator_result;
      }
      first = false;
    } else {
      SYNTAX_ERROR("incomplete member function definition", Current());
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
    mods = field_modifiers_result.node();
  }

  TokenInfo info = *Current();
  if (Current()->type() == Token::TS_MUL) {
    Next();
  } else if (!first) {
    SYNTAX_ERROR("'*' expected", Current());
  }

  
  if (TokenInfo::IsKeyword(Current()->type())) {
    Current()->set_type(Token::TS_IDENTIFIER);
  }
  
    
  if (Current()->type() == Token::TS_IDENTIFIER) {
    auto identifier_result = ParseIdentifier();
    CHECK_AST(identifier_result);
    auto call_sig_result = ParseCallSignature(false, false);
    CHECK_AST(call_sig_result);
    Handle<ir::Node> ret;
      
    if (Current()->type() == Token::TS_LEFT_BRACE) {
      auto function_body_result = ParseFunctionBody(false);
      CHECK_AST(function_body_result);
      ret = New<ir::MemberFunctionView>(mods,
                                        identifier_result.node(),
                                        call_sig_result.node(),
                                        overloads,
                                        function_body_result.node());
    } else if (overloads) {
      ret = New<ir::MemberFunctionOverloadView>(mods, identifier_result.node(), call_sig_result.node());
      if (IsLineTermination()) {
        ConsumeLineTerminator();
      } else {
        SYNTAX_ERROR("';' expected", Current());
      }
    } else {
      SYNTAX_ERROR("invalid member function definition", (&info));
    }
      
    ret->SetInformationForNode(mods);
    return Success(ret);
  }
  SYNTAX_ERROR("identifier expected", Current());
}


template <typename UCharInputIterator>
ParseResult Parser<UCharInputIterator>::ParseMemberVariable(Handle<ir::Node> mods) {
  LOG_PHASE(ParseMemberVariable);

  if (TokenInfo::IsKeyword(Current()->type())) {
    Current()->set_type(Token::TS_IDENTIFIER);
  }
  
  if (Current()->type() == Token::TS_IDENTIFIER) {
    auto identifier_result = ParseIdentifier();
    CHECK_AST(identifier_result);
    ParseResult value_result;
    ParseResult type_result;
    if (Current()->type() == Token::TS_COLON) {
      Next();
      type_result = ParseTypeExpression();
      CHECK_AST(type_result);
    }
    if (Current()->type() == Token::TS_ASSIGN) {
      Next();
      value_result = ParseExpression(true, false);
      CHECK_AST(value_result);
    }
    auto member_variable = New<ir::MemberVariableView>(mods,
                                                       identifier_result.node(),
                                                       type_result.node(),
                                                       value_result.node());
    member_variable->SetInformationForNode(mods);
    return Success(member_variable);
  }
  SYNTAX_ERROR("'identifier' expected", Current());
}


template <typename UCharInputIterator>
ParseResult Parser<UCharInputIterator>::ParseFunctionOverloads(bool yield, bool has_default, bool declaration, bool is_export) {
  auto overloads = New<ir::FunctionOverloadsView>();
  bool first = true;
  
  while (1) {
    if (is_export && !first && Current()->type() == Token::TS_EXPORT) {
      Next();
    } else if (is_export && !first) {
      SYNTAX_ERROR("export expected.", Current());
    }
    first = false;
    if (Current()->type() == Token::TS_FUNCTION) {
      auto function_overloads_result = ParseFunctionOverloadOrImplementation(overloads, yield, has_default, declaration);
      CHECK_AST(function_overloads_result);
      if (function_overloads_result.node()->HasFunctionOverloadView()) {
        Handle<ir::FunctionOverloadView> overload(function_overloads_result.node());
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
        overloads->InsertLast(function_overloads_result.node());
      } else {
        return function_overloads_result;
      }
    } else {
      SYNTAX_ERROR("incomplete function definition", Current());
    }
  }
}


template <typename UCharInputIterator>
ParseResult Parser<UCharInputIterator>::ParseFunctionOverloadOrImplementation(Handle<ir::Node> overloads, bool yield, bool has_default, bool declaration) {
  LOG_PHASE(ParseFunctionOverloadOrImplementation);
  if (Current()->type() == Token::TS_FUNCTION) {
    bool generator = false;
    bool success = true;
    
    TokenInfo info = *Current();
    Next();
    if (Current()->type() == Token::TS_MUL) {
      generator = true;
      Next();
    }
    ParseResult identifier_result;
    if (Current()->type() == Token::TS_IDENTIFIER) {
      identifier_result = ParseIdentifier();
      CHECK_AST(identifier_result);
    }

    if (declaration && !identifier_result) {
      SYNTAX_ERROR("function name required", Current());
    }
    
    auto call_sig_result = ParseCallSignature(false, false);
    CHECK_AST(call_sig_result);
    Handle<ir::Node> ret;
    if (Current()->type() == Token::TS_LEFT_BRACE) {
      auto function_body_result = ParseFunctionBody(yield? yield: generator);
      SKIP_TOKEN_IF_AND(function_body_result, success, Token::TS_RIGHT_BRACE, return Failed());
      ret = New<ir::FunctionView>(overloads,
                                  identifier_result.node(),
                                  call_sig_result.node(),
                                  function_body_result.node());
    } else if (overloads) {
      ret = New<ir::FunctionOverloadView>(generator,
                                          identifier_result.node(),
                                          call_sig_result.node());
      if (IsLineTermination()) {
        ConsumeLineTerminator();
      } else {
        SYNTAX_ERROR("';' expected", Current());
      }
    } else {
      SYNTAX_ERROR("invalid function definition", (&info));
    }
    ret->SetInformationForNode(&info);
    return Success(ret);
  }
  SYNTAX_ERROR("'function' expected", Current());
}


template <typename UCharInputIterator>
ParseResult Parser<UCharInputIterator>::ParseParameterList(bool accesslevel_allowed) {
  LOG_PHASE(ParseParameterList);
  if (Current()->type() == Token::TS_LEFT_PAREN) {
    Handle<ir::ParamList> param_list = New<ir::ParamList>();
    param_list->SetInformationForNode(Current());
    Next();

    if (Current()->type() == Token::TS_RIGHT_PAREN) {
      Next();
      return Success(param_list);
    }
    
    bool has_rest = false;
    bool success = true;
    
    while (1) {
      if (has_rest) {
        SYNTAX_ERROR("Rest parameter must be at the end of the parameters", param_list->last_child());
      }
      if (Current()->type() == Token::TS_IDENTIFIER ||
          Current()->type() == Token::TS_PRIVATE ||
          Current()->type() == Token::TS_PUBLIC ||
          Current()->type() == Token::TS_PROTECTED) {
        auto parameter_result = ParseParameter(false, accesslevel_allowed);
        SKIP_TOKEN_OR(parameter_result, success, Token::TS_RIGHT_PAREN) {
          param_list->InsertLast(parameter_result.node());
        }
      } else if (Current()->type() == Token::TS_REST) {
        has_rest = true;
        TokenInfo token = (*Current());
        Next();
        auto parameter_result = ParseParameter(true, accesslevel_allowed);
        SKIP_TOKEN_OR(parameter_result, success, Token::TS_RIGHT_PAREN) {
          Handle<ir::Node> node = New<ir::RestParamView>(parameter_result.node());
          node->SetInformationForNode(&token);
          param_list->InsertLast(node);
        }
      } else {
        SYNTAX_ERROR("unexpected token in formal parameter list", Current());
      }

      if (Current()->type() == Token::TS_COMMA) {
        Next();
      } else if (Current()->type() == Token::TS_RIGHT_PAREN) {
        Next();
        return Success(param_list);
      } else {
        SYNTAX_ERROR("')' or ',' expected in parameter list", Current());
      }
    }
  }
  SYNTAX_ERROR("'(' is expected in parameter list", Current());
}


template <typename UCharInputIterator>
ParseResult Parser<UCharInputIterator>::ParseParameter(bool rest, bool accesslevel_allowed) {
  LOG_PHASE(ParseParameter);
  Handle<ir::Node> access_level;
  
  if (Current()->type() == Token::TS_PUBLIC || Current()->type() == Token::TS_PRIVATE) {
    if (accesslevel_allowed) {
      access_level = New<ir::ClassFieldAccessLevelView>(Current()->type());
      access_level->SetInformationForNode(Current());
    } else {
      SYNTAX_ERROR("'private' or 'public' not allowed here", Current());
    }
    Next();
  }
  
  if (Current()->type() == Token::TS_IDENTIFIER) {
    Handle<ir::ParameterView> pv = New<ir::ParameterView>();
    pv->SetInformationForNode(Current());
    Handle<ir::NameView> nv = New<ir::NameView>(NewSymbol(ir::SymbolType::kVariableName, Current()->value()));
    nv->SetInformationForNode(Current());
    pv->set_access_level(access_level);
    pv->set_name(nv);
    Next();
    if (Current()->type() == Token::TS_QUESTION_MARK) {
      if (rest) {
        SYNTAX_ERROR("optional parameter not allowed in rest parameter", Current());
      }
      Next();
      pv->set_optional(true);
    }
    if (Current()->type() == Token::TS_COLON) {
      Next();
      if (Current()->type() == Token::TS_STRING_LITERAL) {
        auto string_literal_result = ParseStringLiteral();
        CHECK_AST(string_literal_result);
        pv->set_type_expr(string_literal_result.node());
      } else {
        auto type_expr_result = ParseTypeExpression();
        CHECK_AST(type_expr_result);
        pv->set_type_expr(type_expr_result.node());
      }
    }
    if (Current()->type() == Token::TS_ASSIGN) {
      if (rest) {
        SYNTAX_ERROR("default parameter not allowed in rest parameter", Current());
      }
      Next();
      auto assignment_expr_result = ParseAssignmentExpression(true, false);
      CHECK_AST(assignment_expr_result);
      pv->set_value(assignment_expr_result.node());
    }
    return Success(pv);
  }
  SYNTAX_ERROR("identifier expected", Current());
}


template <typename UCharInputIterator>
ParseResult Parser<UCharInputIterator>::ParseFunctionBody(bool yield) {
  LOG_PHASE(ParseFunctionBody);
  if (Current()->type() == Token::TS_LEFT_BRACE) {
    Handle<ir::Scope> scope = NewScope();
    set_current_scope(scope);
    auto block = New<ir::BlockView>(scope);
    block->SetInformationForNode(Current());
    Next();

    bool success = true;
    
    while (1) {
      if (Current()->type() == Token::TS_RIGHT_BRACE) {
        Next();
        break;
      } else if (Current()->type() == Token::END_OF_INPUT) {
        SYNTAX_ERROR("unexpected end of input.", Current());
      }
      auto stmt_list_result = ParseStatementListItem(yield, true, false, false);
      SKIP_TOKEN_OR(stmt_list_result, success, Token::TS_RIGHT_BRACE) {
        block->InsertLast(stmt_list_result.node());
      }
      if (IsLineTermination()) {
        ConsumeLineTerminator();
      }
    }
    set_current_scope(scope->parent_scope());
    return Success(block);
  }
  SYNTAX_ERROR("unexpected token in 'function body'", Current());
}


template <typename UCharInputIterator>
ParseResult Parser<UCharInputIterator>::ParseEmptyStatement() {
  if (Current()->type() == Token::LINE_TERMINATOR) {
    Next();
    auto node = New<ir::Empty>();
    node->SetInformationForNode(Current());
    return Success(node);
  }
  SYNTAX_ERROR("';' expected.", Current());
}
}
