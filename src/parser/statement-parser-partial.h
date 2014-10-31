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
Handle<ir::Node> Parser<UCharInputIterator>::ParseStatement(bool yield, bool has_return, bool breakable, bool continuable) {
  LOG_PHASE(ParseStatement);
  Handle<ir::Node> result;
  
  switch (Current()->type()) {
    case Token::TS_LEFT_BRACE:
      result = ParseBlockStatement(yield, has_return, breakable, continuable);
      break;

    case Token::LINE_TERMINATOR:
      result = ParseEmptyStatement();
      break;

    case Token::TS_IF:
      result = ParseIfStatement(yield, has_return, breakable, continuable);
      break;

    case Token::TS_FOR:
      result = ParseForStatement(yield, has_return);
      break;

    case Token::TS_WHILE:
      result = ParseWhileStatement(yield, has_return);
      break;

    case Token::TS_DO:
      result = ParseDoWhileStatement(yield, has_return);
      break;

    case Token::TS_CONTINUE:
      if (!continuable) {
        SYNTAX_ERROR("SyntaxError 'continue' only allowed in loops", Current());
      }
      result = ParseContinueStatement(yield);
      break;

    case Token::TS_BREAK: {
      if (!breakable) {
        SYNTAX_ERROR("SyntaxError 'break' not allowed here", Current());
      }
      result = ParseBreakStatement(yield);
      if (IsLineTermination()) {
        ConsumeLineTerminator();
      } else {
        SYNTAX_ERROR("SyntaxError ';' expected", Current());
      }
      break;
    }

    case Token::TS_RETURN:
      if (!has_return) {
        SYNTAX_ERROR("SyntaxError 'return' statement only allowed in function", Current());
      }
      result = ParseReturnStatement(yield);
      break;

    case Token::TS_WITH:
      result = ParseWithStatement(yield, has_return, breakable, continuable);
      break;

    case Token::TS_SWITCH:
      result = ParseSwitchStatement(yield, has_return, continuable);
      break;

    case Token::TS_THROW:
      result = ParseThrowStatement();
      break;

    case Token::TS_TRY:
      result = ParseTryStatement(yield, has_return, breakable, continuable);
      break;

    case Token::TS_DEBUGGER:
      result = ParseDebuggerStatement();
      break;

    case Token::TS_VAR: {
      Handle<ir::Node> node = ParseVariableStatement(true, yield);
      if (IsLineTermination()) {
        ConsumeLineTerminator();
      } else {
        SYNTAX_ERROR("SyntaxError ';' expected", Current());
      }
      return node;
    }

    case Token::END_OF_INPUT:
      SYNTAX_ERROR("SyntaxError Unexpected end of input", Current());
      
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
      result = ParseExpression(true, yield);
      result = New<ir::StatementView>(result);
      result->SetInformationForNode(&info);
    }
  }
  
  return result;
}


template <typename UCharInputIterator>
Handle<ir::Node> Parser<UCharInputIterator>::ParseDeclaration(bool error, bool yield, bool has_default) {
  switch (Current()->type()) {
    case Token::TS_FUNCTION:
      return ParseFunctionOverloads(yield, has_default, true);
    case Token::TS_CLASS:
      return ParseClassDeclaration(yield, has_default);
    case Token::TS_ENUM:
      return ParseEnumDeclaration(yield, has_default);
    case Token::TS_INTERFACE:
      return ParseInterfaceDeclaration();
    case Token::TS_LET:
    case Token::TS_CONST: {
      Handle<ir::Node> node = ParseLexicalDeclaration(true, yield);
      if (IsLineTermination()) {
        ConsumeLineTerminator();
      } else {
        SYNTAX_ERROR("SyntaxError ';' expected", Current());
      }
      return node;
    }
    default:
      if (!error) {
        return ir::Node::Null();
      }
      SYNTAX_ERROR("SyntaxError unexpected token", Current());
  }
}


template <typename UCharInputIterator>
Handle<ir::Node> Parser<UCharInputIterator>::ParseBlockStatement(bool yield, bool has_return, bool breakable, bool continuable) {
  LOG_PHASE(ParseBlockStatement);
  Handle<ir::Scope> scope = NewScope();
  set_current_scope(scope);
  auto block_view = New<ir::BlockView>(scope);
  block_view->SetInformationForNode(Current());

  if (Current()->type() == Token::TS_LEFT_BRACE) {
    Next();
    while (1) {
      if (Current()->type() == Token::TS_RIGHT_BRACE) {
        Next();
        break;
      } else if (Current()->type() == Token::END_OF_INPUT) {
        SYNTAX_ERROR("Unexpected end of input.", Current());
      } else {
        Handle<ir::Node> statement = ParseStatementListItem(yield, has_return, breakable, continuable);
        block_view->InsertLast(statement);
      }
    }
    set_current_scope(scope->parent_scope());
    return block_view;
  }
  SYNTAX_ERROR("SyntaxError '{' expected", Current());
}


template <typename UCharInputIterator>
Handle<ir::Node> Parser<UCharInputIterator>::ParseStatementListItem(bool yield, bool has_return, bool breakable, bool continuable) {
  LOG_PHASE(ParseStatementListItem);
  Handle<ir::Node> node = ParseDeclaration(false, yield, false);
  if (node == ir::Node::Null()) {
    return ParseStatement(yield, has_return, breakable, continuable);
  }
  return node;
}


// lexical_declaration[in, yield]
//   : let_or_const binding_list[?in, ?yield]
//   ;
// let_or_const
//   : 'let'
//   | 'const'
//   ;
template <typename UCharInputIterator>
Handle<ir::Node> Parser<UCharInputIterator>::ParseLexicalDeclaration(bool in, bool yield) {
  LOG_PHASE(ParseLexicalDeclaration);
  if (Current()->type() == Token::TS_LET ||
      Current()->type() == Token::TS_CONST) {
    bool has_const = Current()->type() == Token::TS_CONST;
    Handle<ir::LexicalDeclView> lexical_decl = New<ir::LexicalDeclView>(Current()->type());
    lexical_decl->SetInformationForNode(Current());
    Next();
    while (1) {
      lexical_decl->InsertLast(ParseLexicalBinding(has_const, in, yield));
      if (Current()->type() == Token::TS_COMMA) {
        Next();
      } else {
        break;
      }
    }
    return lexical_decl;
  }
  SYNTAX_ERROR("SyntaxError 'let' or 'const' expected", Current());
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
Handle<ir::Node> Parser<UCharInputIterator>::ParseLexicalBinding(bool const_decl, bool in, bool yield) {
  LOG_PHASE(ParseLexicalBinding);
  Handle<ir::Node> value;
  Handle<ir::Node> lhs;
  if (Current()->type() == Token::TS_IDENTIFIER) {
    lhs = ParseBindingIdentifier(false, in, yield);
  } else {
    lhs = ParseBindingPattern(yield, false);
  }

  if (!lhs->IsValidLhs()) {
    SYNTAX_ERROR("SyntaxError left hand side of lexical binding is invalid", Current());
  }

  Handle<ir::Node> type_expr;
  if (Current()->type() == Token::TS_COLON) {
    Next();
    type_expr = ParseTypeExpression();
  }
  
  if (Current()->type() == Token::TS_ASSIGN) {
    Next();
    value = ParseAssignmentExpression(in, yield);
  } else if (const_decl) {
    SYNTAX_ERROR("SyntaxError const declaration must have an initializer", Current());
  }

  Handle<ir::Node> ret = New<ir::VariableView>(lhs, value, type_expr);
  ret->SetInformationForNode(lhs);
  return ret;
}


// binding_identifier[default, yield]
//   : [+default] 'default'
//   | [~yield] 'yield' identifier
//   | identifier
//   ;
template <typename UCharInputIterator>
Handle<ir::Node> Parser<UCharInputIterator>::ParseBindingIdentifier(bool default_allowed, bool in, bool yield) {
  LOG_PHASE(ParseBindingIdentifier);
  Handle<ir::Node> ret;
  if (Current()->type() == Token::TS_DEFAULT) {
    if (!default_allowed) {
      SYNTAX_ERROR("SyntaxError 'default' keyword not allowed here", Current());
    }
    ret = New<ir::DefaultView>();
  } else if (Current()->type() == Token::TS_YIELD) {
    ret = New<ir::YieldView>(false, ir::Node::Null());
  } else if (Current()->type() == Token::TS_IDENTIFIER) {
    ret = New<ir::NameView>(Current()->value());
  } else {
    SYNTAX_ERROR("SyntaxError 'default', 'yield' or 'identifier' expected", Current());
  }

  ret->SetInformationForNode(Current());
  Next();

  return ret;
}


// binding_pattern[yield,generator_parameter]
//   : object_binding_pattern[?yield,?generator_parameter]
//   | array_binding_pattern[?yield,?generator_parameter]
//   ;
template <typename UCharInputIterator>
Handle<ir::Node> Parser<UCharInputIterator>::ParseBindingPattern(bool yield, bool generator_parameter) {
  LOG_PHASE(ParseBindingPattern);
  switch (Current()->type()) {
    case Token::TS_LEFT_BRACE:
      return ParseObjectBindingPattern(yield, generator_parameter);
    case Token::TS_LEFT_BRACKET:
      return ParseArrayBindingPattern(yield, generator_parameter);
    default:
      SYNTAX_ERROR("SyntaxError '[' or '{' expected", Current());
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
Handle<ir::Node> Parser<UCharInputIterator>::ParseObjectBindingPattern(bool yield, bool generator_parameter) {
  LOG_PHASE(ParseObjectBindingPattern);
  if (Current()->type() == Token::TS_LEFT_BRACE) {
    Handle<ir::Node> binding_prop_list = New<ir::BindingPropListView>();
    binding_prop_list->SetInformationForNode(Current());
    Next();
    while (1) {
      binding_prop_list->InsertLast(ParseBindingProperty(yield, generator_parameter));
      switch (Current()->type()) {
        case Token::TS_COMMA: {
          Next();
          break;
        }
        case Token::TS_RIGHT_BRACE: {
          Next();
          return binding_prop_list;
        }
        default:
          SYNTAX_ERROR("SyntaxError unexpected token", Current());
      }
    }
  }
  SYNTAX_ERROR("SyntaxError '{' expected", Current());
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
Handle<ir::Node> Parser<UCharInputIterator>::ParseArrayBindingPattern(bool yield, bool generator_parameter) {
  LOG_PHASE(ParseArrayBindingPattern);
  if (Current()->type() == Token::TS_LEFT_BRACKET) {
    Handle<ir::Node> binding_array = New<ir::BindingArrayView>();
    binding_array->SetInformationForNode(Current());
    Next();
    bool exit = false;
    while (1) {
      if (Current()->type() == Token::TS_COMMA) {
        Next();
        binding_array->InsertLast(ir::Node::Null());
      }
      if (Current()->type() == Token::TS_REST) {
        Handle<ir::RestParamView> rest = New<ir::RestParamView>();
        rest->SetInformationForNode(Current());
        Next();
        rest->set_parameter(ParseBindingIdentifier(false, true, yield));
        binding_array->InsertLast(rest);
        exit = true;
      } else {
        Handle<ir::Node> elem = ParseBindingElement(yield, generator_parameter);
        Handle<ir::Node> init;
        if (Current()->type() == Token::TS_ASSIGN) {
          init = ParseAssignmentExpression(true, yield);
        }
        Handle<ir::Node> ret = New<ir::BindingElementView>(ir::Node::Null(), elem, init);
        ret->SetInformationForNode(elem);
        binding_array->InsertLast(ret);
      }

      if (Current()->type() == Token::TS_RIGHT_BRACKET) {
        Next();
        break;

      } else if (exit) {
        SYNTAX_ERROR("SyntaxError spread binding must be end of bindings", Current());
      } else if (Current()->type() == Token::TS_COMMA) {Next();}
    }
    return binding_array;
  }
  SYNTAX_ERROR("SyntaxError '[' expected", Current());
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
Handle<ir::Node> Parser<UCharInputIterator>::ParseBindingProperty(bool yield, bool generator_parameter) {
  LOG_PHASE(ParseBindingProperty);
  Handle<ir::Node> key;
  Handle<ir::Node> elem;
  Handle<ir::Node> init;

  if (Current()->type() == Token::TS_IDENTIFIER) {
    key = ParseIdentifier();
  } else {
    SYNTAX_ERROR("SyntaxError 'identifier' expected", Current());
  }
  
  if (Current()->type() == Token::TS_COLON) {
    Next();
    elem = ParseBindingElement(yield, generator_parameter);
  }

  if (Current()->type() == Token::TS_ASSIGN) {
    Next();
    init = ParseAssignmentExpression(true, yield);
  }
  Handle<ir::Node> ret = New<ir::BindingElementView>(key, elem, init);
  ret->SetInformationForNode(key);
  return ret;
}


// binding_element[yield, generator_parameter ]
//   : single_name_binding[?yield, ?generator_parameter]
//   | [+generator_parameter] binding_pattern[?yield,generator_parameter] initializer__opt[in]
//   | [~generator_parameter] binding_pattern[?yield] initializer__opt[in, ?yield]
//   ;
template <typename UCharInputIterator>
Handle<ir::Node> Parser<UCharInputIterator>::ParseBindingElement(bool yield, bool generator_parameter) {
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
Handle<ir::Node> Parser<UCharInputIterator>::ParseVariableStatement(bool in, bool yield) {
  if (Current()->type() == Token::TS_VAR) {
    Next();
    Handle<ir::VariableDeclView> vars = New<ir::VariableDeclView>();
    while (1) {
      Handle<ir::Node> node = ParseVariableDeclaration(in, yield);
      vars->InsertLast(node);
      if (Current()->type() == Token::TS_COMMA) {
        Next();
      } else {
        break;
      }
    }
    return vars;
  }
  SYNTAX_ERROR("SyntaxError 'var' expected", Current());
}


// variable_declaration[in, yield]
//  : binding_identifier[?yield] initializer[?in, ?yield]__opt
//  | binding_pattern[yield] initializer[?in, ?yield]
//  ;
template <typename UCharInputIterator>
Handle<ir::Node> Parser<UCharInputIterator>::ParseVariableDeclaration(bool in, bool yield) {
  LOG_PHASE(ParseVariableDeclaration);
  Handle<ir::Node> value;
  Handle<ir::Node> lhs;
  if (Current()->type() == Token::TS_IDENTIFIER) {
    lhs = ParseBindingIdentifier(false, in, yield);
  } else {
    lhs = ParseBindingPattern(yield, false);
  }

  if (!lhs->IsValidLhs()) {
    SYNTAX_ERROR("SyntaxError left hand side of variable declaration is invalid", Current());
  }

  Handle<ir::Node> type_expr;
  if (Current()->type() == Token::TS_COLON) {
    Next();
    type_expr = ParseTypeExpression();
  }
  
  if (Current()->type() == Token::TS_ASSIGN) {
    Next();
    value = ParseAssignmentExpression(in, yield);
  }

  Handle<ir::Node> ret = New<ir::VariableView>(lhs, value, type_expr);
  ret->SetInformationForNode(lhs);
  current_scope()->Declare(ret);
  
  return ret;
}


// if_statement[yield, return]
//   : 'if' '(' expression[in, ?yield] ')' statement[?yield, ?return] 'else' statement[?yield, ?return]
//   | 'if' '(' expression[in, ?yield] ')' statement[?yield, ?return]
//   ;
template <typename UCharInputIterator>
Handle<ir::Node> Parser<UCharInputIterator>::ParseIfStatement(bool yield, bool has_return, bool breakable, bool continuable) {
  LOG_PHASE(ParseIfStatement);
  if (Current()->type() == Token::TS_IF) {
    TokenInfo info = *Current();
    Next();
    if (Current()->type() == Token::TS_LEFT_PAREN) {
      Next();
      Handle<ir::Node> expr = ParseExpression(true, yield);
      if (Current()->type() == Token::TS_RIGHT_PAREN) {
        Next();
        Handle<ir::Node> then_stmt = ParseStatement(yield, has_return, breakable, continuable);
        if (Prev()->type() != Token::TS_RIGHT_BRACE && IsLineTermination()) {
          ConsumeLineTerminator();
        }
        Handle<ir::Node> else_stmt;
        if (Current()->type() == Token::TS_ELSE) {
          Next();
          else_stmt = ParseStatement(yield, has_return, breakable, continuable);
          if (Prev()->type() != Token::TS_RIGHT_BRACE && IsLineTermination()) {
            ConsumeLineTerminator();
          }
        }
        Handle<ir::IfStatementView> if_stmt = New<ir::IfStatementView>(expr, then_stmt, else_stmt);
        if_stmt->SetInformationForNode(&info);
        return if_stmt;
      }
      SYNTAX_ERROR("SyntaxError ')' expected", Current());
    }
    SYNTAX_ERROR("SyntaxError '(' expected", Current());
  }
  SYNTAX_ERROR("SyntaxError 'if' expected", Current());
}


// while_statment
//   : 'while' '(' expression[in, ?yield] ')' statement[?yield, ?return]
//   ;
template <typename UCharInputIterator>
Handle<ir::Node> Parser<UCharInputIterator>::ParseWhileStatement(bool yield, bool has_return) {
  LOG_PHASE(ParseWhileStatement);
  if (Current()->type() == Token::TS_WHILE) {
    TokenInfo info = *(Current());
    Next();
    if (Current()->type() == Token::TS_LEFT_PAREN) {
      Next();
      Handle<ir::Node> expr = ParseExpression(true, yield);
      Handle<ir::WhileStatementView> while_stmt = New<ir::WhileStatementView>(expr, ParseIterationBody(yield, has_return));
      while_stmt->SetInformationForNode(&info);
      return while_stmt;
    }
    SYNTAX_ERROR("SyntaxError '(' expected", Current());
  }
  SYNTAX_ERROR("SyntaxError 'while' expected", Current());
}


// do_while_statement
//   : 'do' statement[?yield, ?return] 'while' '(' expression[in, ?yield] ')' ;__opt
//   ;
template <typename UCharInputIterator>
Handle<ir::Node> Parser<UCharInputIterator>::ParseDoWhileStatement(bool yield, bool has_return) {
  LOG_PHASE(ParseDoWhileStatement);
  if (Current()->type() == Token::TS_DO) {
    TokenInfo info = *(Current());
    Next();
    Handle<ir::Node> stmt = ParseStatement(yield, has_return, true, true);
    if (Current()->type() == Token::TS_WHILE) {
      Next();
      if (Current()->type() == Token::TS_LEFT_PAREN) {
        Next();
        Handle<ir::Node> expr = ParseExpression(true, yield);
        if (Current()->type() == Token::TS_RIGHT_PAREN) {
          Next();
          if (IsLineTermination()) {
            ConsumeLineTerminator();
          }
          auto do_while = New<ir::DoWhileStatementView>(expr, stmt);
          do_while->SetInformationForNode(&info);
          return do_while;
        }
        SYNTAX_ERROR("SyntaxError ')' expected", Current());
      }
      SYNTAX_ERROR("SyntaxError '(' expected", Current());
    }
    SYNTAX_ERROR("SyntaxError 'while' expected", Current());
  }
  SYNTAX_ERROR("SyntaxError 'do' expected", Current());
}


template <typename UCharInputIterator>
Handle<ir::Node> Parser<UCharInputIterator>::ParseForStatement(bool yield, bool has_return) {
  LOG_PHASE(ParseForStatement);
  if (Current()->type() == Token::TS_FOR) {
    TokenInfo info = *Current();
    Next();
    if (Current()->type() == Token::TS_LEFT_PAREN) {
      Next();
      Handle<ir::Node> reciever;
      switch (Current()->type()) {
        case Token::TS_VAR:
          reciever = ParseVariableStatement(true, yield);
          break;
        case Token::TS_LET:
          reciever = ParseLexicalDeclaration(true, yield);
          break;
        case Token::LINE_TERMINATOR:
          break;
        default: {
          RecordedParserState rps = parser_state();
          reciever = ParseExpression(true, yield);
          if (Current()->type() != Token::LINE_TERMINATOR) {
            RestoreParserState(rps);
            reciever = ParseLeftHandSideExpression(yield);
          }
        }
      }
      return ParseForIteration(reciever, &info, yield, has_return);
    }
    SYNTAX_ERROR("SyntaxError '(' expected", Current());
  }
  SYNTAX_ERROR("SyntaxError 'for' expected", Current());
}


template <typename UCharInputIterator>
Handle<ir::Node> Parser<UCharInputIterator>::ParseForIteration(Handle<ir::Node> reciever, TokenInfo* token_info, bool yield, bool has_return) {
  LOG_PHASE(ParseForIterationStatement);

  Handle<ir::Node> second;
  Handle<ir::Node> third;
  bool for_in = false;
  bool for_of = false;
  
  if (Current()->type() == Token::LINE_TERMINATOR) {
    // for (var i = 0; i < 10; i++) ...
    Next();
    if (Current()->type() != Token::LINE_TERMINATOR) {
      second = ParseExpression(true, yield);
    }
    if (Current()->type() == Token::LINE_TERMINATOR) {
      Next();
      if (Current()->type() != Token::TS_RIGHT_PAREN) {
        third = ParseExpression(true, yield);
      }
      
    }
  } else if (Current()->type() == Token::TS_IDENTIFIER &&
             Current()->value() == "of") {
    // for (var i in obj) ...
    Next();
    second = ParseAssignmentExpression(true, yield);
    for_of = true;
  } else if (Current()->type() == Token::TS_IN) {
    // for (var i of obj) ...
    Next();
    second = ParseAssignmentExpression(false, yield);
    for_in = true;
  } else {
    SYNTAX_ERROR("SyntaxError 'in' or 'of' or ';' expected", Current());
  }

  Handle<ir::Node> body = ParseIterationBody(yield, has_return);
  Handle<ir::Node> ret;
  
  if (for_in) {
    ret = New<ir::ForInStatementView>(reciever, second, body);
  } else if (for_of) {
    ret = New<ir::ForOfStatementView>(reciever, second, body);
  } else {
    ret = New<ir::ForStatementView>(reciever, second, third, body);
  }
    
  ret->SetInformationForNode(token_info);
  return ret;
}


template <typename UCharInputIterator>
Handle<ir::Node> Parser<UCharInputIterator>::ParseIterationBody(bool yield, bool has_return) {
  if (Current()->type() == Token::TS_RIGHT_PAREN) {
    Next();
    return ParseStatement(yield, has_return, true, true);
  }
  SYNTAX_ERROR("SyntaxError ')' expected", Current());
}


template <typename UCharInputIterator>
Handle<ir::Node> Parser<UCharInputIterator>::ParseContinueStatement(bool yield) {
  LOG_PHASE(ParseContinueStatement);
  if (Current()->type() == Token::TS_CONTINUE) {
    TokenInfo info = *Current();
    Next();
    Handle<ir::Node> result;
    YATSC_SCOPED([&]{
      if (result) {
        result->SetInformationForNode(&info);
      }
    });
    if (Current()->type() == Token::TS_IDENTIFIER) {
      return result = New<ir::ContinueStatementView>(ParsePrimaryExpression(yield));
    }
    return result = New<ir::ContinueStatementView>();
  }
  SYNTAX_ERROR("SyntaxError 'continue' expected", Current());
}


template <typename UCharInputIterator>
Handle<ir::Node> Parser<UCharInputIterator>::ParseBreakStatement(bool yield) {
  LOG_PHASE(ParseBreakStatement);
  if (Current()->type() == Token::TS_BREAK) {
    TokenInfo info = *Current();
    Next();
    Handle<ir::Node> result;
    YATSC_SCOPED([&]{
      if (result) {
        result->SetInformationForNode(&info);
      }
    });
    if (Current()->type() == Token::TS_IDENTIFIER) {
      return result = New<ir::BreakStatementView>(ParsePrimaryExpression(yield));
    }
    return result = New<ir::BreakStatementView>();
  }
  SYNTAX_ERROR("SyntaxError 'break' expected", Current());
}


template <typename UCharInputIterator>
Handle<ir::Node> Parser<UCharInputIterator>::ParseReturnStatement(bool yield) {
  LOG_PHASE(ParseReturnStatement);
  if (Current()->type() == Token::TS_RETURN) {
    TokenInfo info = *Current();
    Handle<ir::Node> result;
    YATSC_SCOPED([&]{
      if (result) {
        result->SetInformationForNode(&info);
      }
    });
    
    Next();
    
    if (IsLineTermination()) {
      ConsumeLineTerminator();
      return result = New<ir::ReturnStatementView>();
    }
    
    return result = New<ir::ReturnStatementView>(ParseExpression(true, yield));
  }
  SYNTAX_ERROR("SyntaxError 'return' expected", Current());
}


template <typename UCharInputIterator>
Handle<ir::Node> Parser<UCharInputIterator>::ParseWithStatement(bool yield, bool has_return, bool breakable, bool continuable) {
  LOG_PHASE(ParseWithStatement);
  if (Current()->type() == Token::TS_WITH) {
    TokenInfo info = *Current();
    Handle<ir::Node> result;
    YATSC_SCOPED([&]{
      if (result) {
        result->SetInformationForNode(&info);
      }
    });
    
    Next();
    
    if (Current()->type() == Token::TS_LEFT_PAREN) {
      Next();
      Handle<ir::Node> expr = ParseExpression(true, yield);
      if (Current()->type() == Token::TS_RIGHT_PAREN) {
        Next();
        Handle<ir::Node> stmt = ParseStatement(yield, has_return, breakable, continuable);
        return result = New<ir::WithStatementView>(expr, stmt);
      }
      SYNTAX_ERROR("SyntaxError ')' expected", Current());
    }
    SYNTAX_ERROR("SyntaxError '(' expected", Current());
  }
  SYNTAX_ERROR("SyntaxError 'with' expected", Current());
}


template <typename UCharInputIterator>
Handle<ir::Node> Parser<UCharInputIterator>::ParseSwitchStatement(bool yield, bool has_return, bool continuable) {
  LOG_PHASE(ParseSwitchStatement);
  if (Current()->type() == Token::TS_SWITCH) {
    TokenInfo info = *Current();
    Next();
    if (Current()->type() == Token::TS_LEFT_PAREN) {
      Next();
      Handle<ir::Node> expr = ParseExpression(true, yield);
      if (Current()->type() == Token::TS_RIGHT_PAREN) {
        Next();
        if (Current()->type() == Token::TS_LEFT_BRACE) {
          Next();
          Handle<ir::Node> case_clauses = ParseCaseClauses(yield, has_return, continuable);
          if (Current()->type() == Token::TS_RIGHT_BRACE) {
            Next();
            Handle<ir::Node> switch_stmt = New<ir::SwitchStatementView>(expr, case_clauses);
            switch_stmt->SetInformationForNode(&info);
            return switch_stmt;
          }
          SYNTAX_ERROR("SyntaxError '}' expected", Current());
        }
        SYNTAX_ERROR("SyntaxError '{' expected", Current());
      }
      SYNTAX_ERROR("SyntaxError ')' expected", Current());
    }
    SYNTAX_ERROR("SyntaxError '(' expected", Current());
  }
  SYNTAX_ERROR("SyntaxError 'switch' expected", Current());
}


template <typename UCharInputIterator>
Handle<ir::Node> Parser<UCharInputIterator>::ParseCaseClauses(bool yield, bool has_return, bool continuable) {
  LOG_PHASE(ParseCaseClauses);
  Handle<ir::CaseListView> case_list = New<ir::CaseListView>();
  case_list->SetInformationForNode(Current());
  bool default_encounted = false;
  while (1) {
    bool normal_case = false;
    Handle<ir::Node> expr;
    TokenInfo info = *Current();
    
    switch (Current()->type()) {
      case Token::TS_CASE: {
        normal_case = true;
        Next();
        expr = ParseExpression(true, yield);
      }
      case Token::TS_DEFAULT: {
        if (!normal_case) {
          default_encounted = true;
          Next();
        }
        if (Current()->type() == Token::TS_COLON) {
          Next();
        } else {
          SYNTAX_ERROR("SyntaxError ':' expected", Current());
        }
        Handle<ir::Node> body = New<ir::CaseBody>();
        while (1) {
          if (Current()->type() == Token::TS_CASE ||
              Current()->type() == Token::TS_DEFAULT) {
            if (default_encounted && Current()->type() == Token::TS_DEFAULT) {
              SYNTAX_ERROR("SyntaxError More than one 'default' clause in switch statement", Current());
            }
            break;
          } else if (Current()->type() == Token::TS_RIGHT_BRACE) {
            break;
          }
          if (Current()->type() == Token::TS_LEFT_BRACE) {
            body->InsertLast(ParseBlockStatement(yield, has_return, true, continuable));
          } else {
            body->InsertLast(ParseStatementListItem(yield, has_return, true, continuable));
          }
        }
        
        Handle<ir::CaseView> case_view = New<ir::CaseView>(expr, body);
        case_view->SetInformationForNode(&info);
        case_list->InsertLast(case_view);
        break;
      }
      case Token::TS_RIGHT_BRACE: {
        return case_list;
      }
      default:
        SYNTAX_ERROR("SyntaxError unexpected token", Current());
    }
  }
}


template <typename UCharInputIterator>
Handle<ir::Node> Parser<UCharInputIterator>::ParseLabelledStatement(bool yield, bool has_return, bool breakable, bool continuable) {
  if (Current()->type() == Token::TS_IDENTIFIER) {
    Handle<ir::Node> identifier = ParseLabelIdentifier(yield);
    if (Current()->type() == Token::TS_COLON) {
      Next();
      Handle<ir::Node> stmt = ParseLabelledItem(yield, has_return, breakable, continuable);
      auto node = New<ir::LabelledStatementView>(identifier, stmt);
      node->SetInformationForNode(identifier);
      return node;
    }
    SYNTAX_ERROR("SyntaxError ':' expected", Current());
  }
  SYNTAX_ERROR("SyntaxError 'identifier' expected", Current());
}


template <typename UCharInputIterator>
Handle<ir::Node> Parser<UCharInputIterator>::ParseLabelledItem(bool yield, bool has_return, bool breakable, bool continuable) {
  if (Current()->type() == Token::TS_FUNCTION) {
    return ParseFunctionOverloads(yield, false, true);
  }
  return ParseStatement(yield, has_return, breakable, continuable);
}


template <typename UCharInputIterator>
Handle<ir::Node> Parser<UCharInputIterator>::ParseThrowStatement() {
  LOG_PHASE(ParseThrowStatement);
  if (Current()->type() == Token::TS_THROW) {
    TokenInfo info = *Current();
    Next();
    if (!IsLineTermination()) {
      Handle<ir::Node> expr = ParseExpression(false, false);
      Handle<ir::ThrowStatementView> throw_stmt = New<ir::ThrowStatementView>(expr);
      throw_stmt->SetInformationForNode(&info);
      return throw_stmt;
    }
    SYNTAX_ERROR("SyntaxError throw statement expected expression", (&info));
  }
  SYNTAX_ERROR("SyntaxError throw expected", Current());
}


template <typename UCharInputIterator>
Handle<ir::Node> Parser<UCharInputIterator>::ParseTryStatement(bool yield, bool has_return, bool breakable, bool continuable) {
  LOG_PHASE(ParseTryStatement);
  if (Current()->type() == Token::TS_TRY) {
    TokenInfo info = *Current();
    Next();
    Handle<ir::Node> block = ParseBlockStatement(yield, has_return, breakable, continuable);
    Handle<ir::Node> catch_block;
    Handle<ir::Node> finally_block;
    bool has_catch_or_finally = false;
    if (Current()->type() == Token::TS_CATCH) {
      has_catch_or_finally = true;
      catch_block = ParseCatchBlock(yield, has_return, breakable, continuable);
    }
    if (Current()->type() == Token::TS_FINALLY) {
      has_catch_or_finally = true;
      finally_block = ParseFinallyBlock(yield, has_return, breakable, continuable);
    }

    if (!has_catch_or_finally) {
      SYNTAX_ERROR("SyntaxError try statement need catch block or finally block", Current());
    }
    Handle<ir::TryStatementView> try_stmt = New<ir::TryStatementView>(block, catch_block, finally_block);
    try_stmt->SetInformationForNode(&info);
    return try_stmt;
  }
  SYNTAX_ERROR("SyntaxError 'try' expected", Current());
}


template <typename UCharInputIterator>
Handle<ir::Node> Parser<UCharInputIterator>::ParseCatchBlock(bool yield, bool has_return, bool breakable, bool continuable) {
  LOG_PHASE(ParseCatchBlock);
  if (Current()->type() == Token::TS_CATCH) {
    TokenInfo info = *Current();
    Next();
    if (Current()->type() == Token::TS_LEFT_PAREN) {
      Next();
      Handle<ir::Node> catch_parameter;
      if (Current()->type() == Token::TS_IDENTIFIER) {
        catch_parameter = ParseBindingIdentifier(false, false, yield);
      } else {
        catch_parameter = ParseBindingPattern(yield, false);
      }

      if (Current()->type() == Token::TS_RIGHT_PAREN) {
        Next();
        Handle<ir::Node> block = ParseBlockStatement(yield, has_return, breakable, continuable);
        Handle<ir::CatchStatementView> catch_stmt = New<ir::CatchStatementView>(catch_parameter, block);
        catch_stmt->SetInformationForNode(&info);
        return catch_stmt;
      }
      SYNTAX_ERROR("SyntaxError ')' expected", Current());
    }
    SYNTAX_ERROR("SyntaxError '(' expected", Current());
  }
  SYNTAX_ERROR("SyntaxError 'catch' expected", Current());
}


template <typename UCharInputIterator>
Handle<ir::Node> Parser<UCharInputIterator>::ParseFinallyBlock(bool yield, bool has_return, bool breakable, bool continuable) {
  LOG_PHASE(ParseFinallyBlock);
  if (Current()->type() == Token::TS_FINALLY) {
    TokenInfo info = *Current();
    Next();
    Handle<ir::Node> block = ParseBlockStatement(yield, has_return, breakable, continuable);
    Handle<ir::FinallyStatementView> finally_stmt = New<ir::FinallyStatementView>(block);
    finally_stmt->SetInformationForNode(&info);
    return finally_stmt;
  }
  SYNTAX_ERROR("SyntaxError 'finally' expected", Current());
}

template <typename UCharInputIterator>
Handle<ir::Node> Parser<UCharInputIterator>::ParseDebuggerStatement() {
  LOG_PHASE(ParseDebuggerStatement);
  if (Current()->type() == Token::TS_DEBUGGER) {
    Handle<ir::Node> ret = New<ir::DebuggerView>();
    ret->SetInformationForNode(Current());
    Next();
    return ret;
  }
  SYNTAX_ERROR("SyntaxError 'debugger' expected", Current());
}


template <typename UCharInputIterator>
Handle<ir::Node> Parser<UCharInputIterator>::ParseInterfaceDeclaration() {
  LOG_PHASE(ParseInterfaceDeclaration);
  if (Current()->type() == Token::TS_INTERFACE) {
    Next();
    Handle<ir::Node> name = ParseIdentifier();
    Handle<ir::Node> type_parameters;
    
    if (Current()->type() == Token::TS_LESS) {
      type_parameters = ParseTypeParameters();
    }
    
    auto extends = New<ir::InterfaceExtendsView>();

    if (Current()->type() == Token::TS_EXTENDS) {
      Next();
      while (1) {
        extends->InsertLast(ParseReferencedType());
        if (Current()->type() == Token::TS_COMMA) {
          Next();
        } else {
          break;
        }
      }
    }

    if (Current()->type() == Token::TS_LEFT_BRACE) {
      Handle<ir::Node> interface_body = ParseObjectTypeExpression();
      return New<ir::InterfaceView>(name, type_parameters, extends, interface_body);
    }
    SYNTAX_ERROR("SyntaxError '{' expected.", Current());
  }
  SYNTAX_ERROR("SyntaxError 'interface' expected.", Current());
}


template <typename UCharInputIterator>
Handle<ir::Node> Parser<UCharInputIterator>::ParseEnumDeclaration(bool yield, bool has_default) {
  LOG_PHASE(ParseEnumDeclaration);
  if (Current()->type() == Token::TS_ENUM) {
    TokenInfo info = *Current();
    Next();
    Handle<ir::Node> identifier = ParseIdentifier();
    if (Current()->type() == Token::TS_LEFT_BRACE) {
      Handle<ir::Node> enum_body = ParseEnumBody(yield, has_default);
      auto ret = New<ir::EnumDeclView>(identifier, enum_body);
      ret->SetInformationForNode(&info);
      return ret;
    }
    SYNTAX_ERROR("SyntaxError '{' expected.", Current());
  }
  SYNTAX_ERROR("SyntaxError 'enum' expected.", Current());
}


template <typename UCharInputIterator>
Handle<ir::Node> Parser<UCharInputIterator>::ParseEnumBody(bool yield, bool has_default) {
  LOG_PHASE(ParseEnumBody);
  if (Current()->type() == Token::TS_LEFT_BRACE) {
    auto ret = New<ir::EnumBodyView>();
    ret->SetInformationForNode(Current());
    Next();
    
    if (Current()->type() == Token::TS_RIGHT_BRACE) {
      Next();
      return ret;
    }
    
    while (1) {
      ret->InsertLast(ParseEnumProperty(yield, has_default));
      if (Current()->type() == Token::TS_COMMA) {
        Next();
        if (Current()->type() == Token::TS_RIGHT_BRACE) {
          Next();
          return ret;
        }
      } else if (Current()->type() == Token::TS_RIGHT_BRACE) {
        Next();
        return ret;
      } else {
        SYNTAX_ERROR("SyntaxError ',' or '}' expected.", Current());
      }
    }
  }
  SYNTAX_ERROR("SyntaxError '{' expected.", Current());
}


template <typename UCharInputIterator>
Handle<ir::Node> Parser<UCharInputIterator>::ParseEnumProperty(bool yield, bool has_default) {
  LOG_PHASE(ParseEnumProperty);
  Handle<ir::Node> prop = ParsePropertyName(yield, false);
  if (Current()->type() == Token::TS_ASSIGN) {
    Next();
    return CreateEnumFieldView(prop, ParseAssignmentExpression(true, yield));
  }
  return CreateEnumFieldView(prop, ir::Node::Null());
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
Handle<ir::Node> Parser<UCharInputIterator>::ParseClassDeclaration(bool yield, bool has_default) {
  LOG_PHASE(ParseClassDeclaration);
  if (Current()->type() == Token::TS_CLASS) {
    TokenInfo info = *Current();
    Next();
    Handle<ir::Node> name = ParseIdentifier();
    Handle<ir::Node> type_parameters;
    
    if (Current()->type() == Token::TS_LESS) {
      type_parameters = ParseTypeParameters();
    }
    
    Handle<ir::Node> bases = ParseClassBases();
    if (Current()->type() == Token::TS_LEFT_BRACE) {
      Next();
      Handle<ir::Node> body = ParseClassBody();
      auto class_decl = New<ir::ClassDeclView>(name, type_parameters, bases, body);
      class_decl->SetInformationForNode(&info);
      return class_decl;
    }
    SYNTAX_ERROR("SyntaxError '{' expected", Current());
  }
  SYNTAX_ERROR("SyntaxError 'class' expected", Current());
}


template <typename UCharInputIterator>
Handle<ir::Node> Parser<UCharInputIterator>::ParseClassBases() {
  LOG_PHASE(ParseClassBases);
  bool extends_keyword = false;
  auto bases = New<ir::ClassBasesView>();
  auto impls = New<ir::ClassImplsView>();
  bases->SetInformationForNode(Current());
  
  while (1) {
    if (Current()->type() == Token::TS_EXTENDS) {
      if (extends_keyword) {
        SYNTAX_ERROR("SyntaxError class extendable only one class", Current());
      }
      TokenInfo info = *Current();
      Next();
      extends_keyword = true;
      auto heritage = New<ir::ClassHeritageView>(ParseReferencedType());
      heritage->SetInformationForNode(&info);
      bases->set_base(heritage);
    } else if (Current()->type() == Token::TS_IMPLEMENTS) {
      Next();
      while (1) {
        impls->InsertLast(ParseReferencedType());
        if (Current()->type() != Token::TS_COMMA) {
          break;
        }
        Next();
      }
    } else if (Current()->type() == Token::TS_LEFT_BRACE) {
      if (impls->size() > 0) {
        bases->set_impls(impls);
      }
      return bases;
    } else {
      SYNTAX_ERROR("SyntaxError unexpected token", Current());
    }
  }
}


template <typename UCharInputIterator>
Handle<ir::Node> Parser<UCharInputIterator>::ParseClassBody() {
  LOG_PHASE(ParseClassBody);
  auto fields = New<ir::ClassFieldListView>();
  while (1) {
    if (Current()->type() != Token::TS_RIGHT_BRACE) {
      fields->InsertLast(ParseClassElement());
      if (IsLineTermination()) {
        ConsumeLineTerminator();
      } else if (Current()->type() != Token::TS_RIGHT_BRACE &&
                 Prev()->type() != Token::TS_RIGHT_BRACE) {
        PrintStackTrace();
        SYNTAX_ERROR("SyntaxError ';' expected", Prev());
      }
    } else {
      Next();
      return fields;
    }
  }
}


template <typename UCharInputIterator>
Handle<ir::Node> Parser<UCharInputIterator>::ParseClassElement() {
  LOG_PHASE(ParseClassElement);
  if (Current()->type() == Token::TS_LEFT_BRACKET) {
    return ParseIndexSignature();
  }
  
  Handle<ir::Node> mods = ParseFieldModifiers();
  AccessorType at = ParseAccessor();

  if (TokenInfo::IsKeyword(Current()->type())) {
    Current()->set_type(Token::TS_IDENTIFIER);
  }
  
  if (Current()->type() == Token::TS_IDENTIFIER) {
    if (Current()->value() == "constructor") {
      return ParseConstructorOverloads(mods);
    } else {
      RecordedParserState rps = parser_state();
      Next();
      if (Current()->type() == Token::TS_LEFT_PAREN ||
          Current()->type() == Token::TS_LESS) {
        RestoreParserState(rps);
        return ParseMemberFunctionOverloads(mods, &at);
      } else {
        RestoreParserState(rps);
        return ParseMemberVariable(mods);
      }
    }
  } else if (Current()->type() == Token::TS_MUL) {
    Next();
    return ParseGeneratorMethodOverloads(mods);
  }
  SYNTAX_ERROR("SyntaxError unexpected token", Current());
}


template <typename UCharInputIterator>
Handle<ir::Node> Parser<UCharInputIterator>::ParseFieldModifiers() {
  LOG_PHASE(ParseFieldModifiers);
  auto mods = New<ir::ClassFieldModifiersView>();
  mods->SetInformationForNode(Current());
  
  if (Current()->type() == Token::TS_STATIC) {
    Handle<ir::Node> mod = ParseFieldModifier();
    mods->InsertLast(mod);
    if (Current()->type() == Token::TS_PUBLIC ||
        Current()->type() == Token::TS_PROTECTED ||
        Current()->type() == Token::TS_PRIVATE) {
      mod = ParseFieldModifier();
      mods->InsertLast(mod);
    }
  } else if (Current()->type() == Token::TS_PUBLIC ||
             Current()->type() == Token::TS_PROTECTED ||
             Current()->type() == Token::TS_PRIVATE) {
    Handle<ir::Node> mod1 = ParseFieldModifier();
    if (Current()->type() == Token::TS_STATIC) {
      Handle<ir::Node> mod2 = ParseFieldModifier();
      mods->InsertLast(mod2);
      mods->InsertLast(mod1);
    } else {
      mods->InsertLast(mod1);
    }
  } else {
    auto pub = New<ir::ClassFieldAccessLevelView>(Token::TS_PUBLIC);
    pub->SetInformationForNode(Current());
    mods->InsertLast(pub);
  }
  
  return mods;
}


template <typename UCharInputIterator>
Handle<ir::Node> Parser<UCharInputIterator>::ParseFieldModifier() {
  LOG_PHASE(ParseFieldModifier);
  switch (Current()->type()) {
    case Token::TS_STATIC:
    case Token::TS_PUBLIC:
    case Token::TS_PROTECTED:
    case Token::TS_PRIVATE: {
      auto node = New<ir::ClassFieldAccessLevelView>(Current()->type());
      node->SetInformationForNode(Current());
      Next();
      return node;
    }
    default:
      return ir::Node::Null();
  }
}


template <typename UCharInputIterator>
Handle<ir::Node> Parser<UCharInputIterator>::ValidateOverload(Handle<ir::MemberFunctionDefinitionView> node, Handle<ir::Node> overloads) {
  LOG_PHASE(ValidateOverload);
  if (overloads->size() > 0) {
    Handle<ir::MemberFunctionOverloadView> last(overloads->last_child());
    if (!node->name()->string_equals(last->at(1))) {
      SYNTAX_ERROR_POS("SyntaxError member function overload must have a same name", node->at(1)->source_position());
    }
    if (!node->modifiers()->Equals(last->modifiers())) {
      Handle<ir::Node> target;
      if (node->modifiers()->size() > last->modifiers()->size()) {
        target = node->modifiers()->first_child();
      } else {
        target = last->modifiers()->first_child();
      }
      SYNTAX_ERROR_POS("SyntaxError member function overload must have same modifiers", target->source_position()); 
    }
  } else {
    Handle<ir::MemberFunctionOverloadView> fn(node);
    if (fn->getter()) {
      Handle<ir::CallSignatureView> call_sig(fn->call_signature());
      if (call_sig->param_list()->size() > 0) {
        SYNTAX_ERROR_POS("SyntaxError the formal parameter of getter function must be empty.", call_sig->param_list()->source_position());
      }
      
      if (call_sig->return_type()) {
        Handle<ir::Node> ret = call_sig->return_type();
        if (ret->HasSimpleTypeExprView()) {
          Handle<ir::Node> ret_type(ret->ToSimpleTypeExprView()->type_name());
          const UtfString& name = ret_type->string_value();
          if (name == "void" || name == "null") {
            SYNTAX_ERROR_POS("SyntaxError getter function must return value.", ret_type->source_position()); 
          }
        }
      }
    } else if (fn->setter()) {
      Handle<ir::CallSignatureView> call_sig(fn->call_signature());
      if (call_sig->param_list()->size() != 1) {
        SYNTAX_ERROR_POS("SyntaxError the setter function allowed only one parameter.", call_sig->param_list()->source_position());
      }
      if (call_sig->return_type()) {
        Handle<ir::Node> ret = call_sig->return_type();
        if (ret->HasSimpleTypeExprView()) {
          Handle<ir::Node> ret_type(ret->ToSimpleTypeExprView()->type_name());
          const UtfString& name = ret_type->string_value();
          if (name != "void" && name != "null") {
            SYNTAX_ERROR_POS("SyntaxError setter function must not return value.", ret_type->source_position()); 
          }
        }
      }
    }
  }
  return ir::Node::Null();
}


template <typename UCharInputIterator>
Handle<ir::Node> Parser<UCharInputIterator>::ParseConstructorOverloads(Handle<ir::Node> mods) {
  LOG_PHASE(ParseConstructorOverloads);
  auto overloads = New<ir::MemberFunctionOverloadsView>();
  bool first = true;
  while (1) {
    if ((Current()->type() == Token::TS_IDENTIFIER &&
        Current()->value() == "constructor") ||
        Current()->type() == Token::TS_PUBLIC ||
        Current()->type() == Token::TS_PRIVATE ||
        Current()->type() == Token::TS_PROTECTED) {
      Handle<ir::Node> fn = ParseConstructorOverloadOrImplementation(first, mods, overloads);
      if (fn->HasMemberFunctionOverloadView()) {
        overloads->InsertLast(fn);
        ValidateOverload(Handle<ir::MemberFunctionOverloadView>(fn), overloads);
      } else {
        ValidateOverload(Handle<ir::MemberFunctionView>(fn), overloads);
        return fn;
      }
    } else {
      SYNTAX_ERROR("SyntaxError incomplete constructor definition", Current());
    }
  }
}


template <typename UCharInputIterator>
Handle<ir::Node> Parser<UCharInputIterator>::ParseConstructorOverloadOrImplementation(
    bool first,
    Handle<ir::Node> mods,
    Handle<ir::Node> overloads) {
  
  LOG_PHASE(ParseConstructorOverloadOrImplementation);

  if (!first) {
    mods = ParseFieldModifiers();
  }
  
  if (Current()->type() == Token::TS_IDENTIFIER &&
      Current()->value() == "constructor") {
    TokenInfo info = *Current();
    Handle<ir::Node> name = ParseIdentifier();
    Handle<ir::Node> call_signature = ParseCallSignature(true);
    Handle<ir::Node> ret;
    if (Current()->type() == Token::TS_LEFT_BRACE) {
      Handle<ir::Node> body = ParseFunctionBody(false);
      ret = New<ir::MemberFunctionView>(mods, name, call_signature, overloads, body);
    } else if (overloads) {
      ret = New<ir::MemberFunctionOverloadView>(mods, name, call_signature);
      if (IsLineTermination()) {
        ConsumeLineTerminator();
      } else {
        SYNTAX_ERROR("SyntaxError ';' expected", Current());
      }
    } else {
      SYNTAX_ERROR("SyntaxError invalid constructor definition", (&info));
    }
    ret->SetInformationForNode(mods);
    return ret;
  }
  SYNTAX_ERROR("SyntaxError 'constructor' expected", Current());
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
Handle<ir::Node> Parser<UCharInputIterator>::ParseMemberFunctionOverloads(Handle<ir::Node> mods, AccessorType* at) {
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
      Handle<ir::Node> fn = ParseMemberFunctionOverloadOrImplementation(first, mods, at, overloads);

      // If function is overload decl,
      // add node to the overloads list.
      if (fn->HasMemberFunctionOverloadView()) {
        ValidateOverload(Handle<ir::MemberFunctionOverloadView>(fn), overloads);
        overloads->InsertLast(fn);
      } else {
        // Else, return node.
        ValidateOverload(Handle<ir::MemberFunctionView>(fn), overloads);
        return fn;
      }
      first = false;
    } else {
      SYNTAX_ERROR("SyntaxError incomplete member function definition", (&info));
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
Handle<ir::Node> Parser<UCharInputIterator>::ParseMemberFunctionOverloadOrImplementation(
    bool first,
    Handle<ir::Node> mods,
    AccessorType* acessor_type,
    Handle<ir::Node> overloads) {
  LOG_PHASE(ParseMemberFunctionOverloadOrImplementation);
  
  AccessorType at(false, false, TokenInfo());

  // If this method is not a first function that is parsed from ParseMemberFunctionOverloads,
  // parse modifiers.
  if (!first) {
    mods = ParseFieldModifiers();
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

    Handle<ir::Node> name = ParseIdentifier();    
    Handle<ir::Node> call_signature = ParseCallSignature(false);
    Handle<ir::Node> ret;

    // public something(): void {
    // -------------------------^ here
    if (Current()->type() == Token::TS_LEFT_BRACE) {
      Handle<ir::Node> body = ParseFunctionBody(false);
      ret = New<ir::MemberFunctionView>(at.getter, at.setter, false, mods, name, call_signature, overloads, body);
    } else if (overloads) {
      
      // Getter and setter is not allowed to overload function declaration.
      if (at.getter || at.setter) {
        SYNTAX_ERROR("SyntaxError overload is not allowed to getter and setter.", (&info));
      }
      ret = New<ir::MemberFunctionOverloadView>(mods, name, call_signature);
      if (IsLineTermination()) {
        ConsumeLineTerminator();
      } else {
        SYNTAX_ERROR("SyntaxError ';' expected", Current());
      }
    } else {
      SYNTAX_ERROR("SyntaxError invalid member function definition", (&info));
    }
    ret->SetInformationForNode(mods);
    return ret;
  }
  SYNTAX_ERROR("SyntaxError identifier expected", Current());
}


template <typename UCharInputIterator>
Handle<ir::Node> Parser<UCharInputIterator>::ParseGeneratorMethodOverloads(Handle<ir::Node> mods) {
  LOG_PHASE(ParseGeneratorMethodOverloads);
  auto overloads = New<ir::MemberFunctionOverloadsView>();
  bool first = true;
  while (1) {
    TokenInfo info = *Current();
    RecordedParserState rps = parser_state();
    Next();
    if (IsMemberFunctionOverloadsBegin(&info)) {
      RestoreParserState(rps);
      Handle<ir::Node> fn = ParseGeneratorMethodOverloadOrImplementation(first, mods, overloads);
      if (fn->HasMemberFunctionOverloadView()) {
        ValidateOverload(Handle<ir::MemberFunctionOverloadView>(fn), overloads);
        overloads->InsertLast(fn);
      } else {
        ValidateOverload(Handle<ir::MemberFunctionView>(fn), overloads);
        return fn;
      }
      first = false;
    } else {
      SYNTAX_ERROR("SyntaxError incomplete member function definition", Current());
    }
  }
}


template <typename UCharInputIterator>
Handle<ir::Node> Parser<UCharInputIterator>::ParseGeneratorMethodOverloadOrImplementation(
    bool first,
    Handle<ir::Node> mods,
    Handle<ir::Node> overloads) {
  
  LOG_PHASE(ParseGeneratorMethodOverloadOrImplementation);
  
  if (!first) {
    mods = ParseFieldModifiers();
  }

  TokenInfo info = *Current();
  if (Current()->type() == Token::TS_MUL) {
    Next();
  } else if (!first) {
    SYNTAX_ERROR("SyntaxError '*' expected", Current());
  }

  
  if (TokenInfo::IsKeyword(Current()->type())) {
    Current()->set_type(Token::TS_IDENTIFIER);
  }
  
    
  if (Current()->type() == Token::TS_IDENTIFIER) {
    Handle<ir::Node> name = ParseIdentifier();    
    Handle<ir::Node> call_signature = ParseCallSignature(false);
    Handle<ir::Node> ret;
      
    if (Current()->type() == Token::TS_LEFT_BRACE) {
      Handle<ir::Node> body = ParseFunctionBody(false);
      ret = New<ir::MemberFunctionView>(mods, name, call_signature, overloads, body);
    } else if (overloads) {
      ret = New<ir::MemberFunctionOverloadView>(mods, name, call_signature);
      if (IsLineTermination()) {
        ConsumeLineTerminator();
      } else {
        SYNTAX_ERROR("SyntaxError ';' expected", Current());
      }
    } else {
      SYNTAX_ERROR("SyntaxError invalid member function definition", (&info));
    }
      
    ret->SetInformationForNode(mods);
    return ret;
  }
  SYNTAX_ERROR("SyntaxError identifier expected", Current());
}


template <typename UCharInputIterator>
Handle<ir::Node> Parser<UCharInputIterator>::ParseMemberVariable(Handle<ir::Node> mods) {
  LOG_PHASE(ParseMemberVariable);

  if (TokenInfo::IsKeyword(Current()->type())) {
    Current()->set_type(Token::TS_IDENTIFIER);
  }
  
  if (Current()->type() == Token::TS_IDENTIFIER) {
    Handle<ir::Node> identifier = ParseIdentifier();
    Handle<ir::Node> value;
    Handle<ir::Node> type;
    if (Current()->type() == Token::TS_COLON) {
      Next();
      type = ParseTypeExpression();
    }
    if (Current()->type() == Token::TS_ASSIGN) {
      Next();
      value = ParseExpression(true, false);
    }
    auto member_variable = New<ir::MemberVariableView>(mods, identifier, type, value);
    member_variable->SetInformationForNode(mods);
    return member_variable;
  }
  SYNTAX_ERROR("SyntaxError 'identifier' expected", Current());
}


template <typename UCharInputIterator>
Handle<ir::Node> Parser<UCharInputIterator>::ParseFunctionOverloads(bool yield, bool has_default, bool declaration, bool is_export) {
  auto overloads = New<ir::FunctionOverloadsView>();
  bool first = true;
  while (1) {
    if (is_export && !first && Current()->type() == Token::TS_EXPORT) {
      Next();
    } else if (is_export && !first) {
      SYNTAX_ERROR("SyntaxError export expected.", Current());
    }
    first = false;
    if (Current()->type() == Token::TS_FUNCTION) {
      Handle<ir::Node> fn = ParseFunctionOverloadOrImplementation(overloads, yield, has_default, declaration);
      if (fn->HasFunctionOverloadView()) {
        Handle<ir::FunctionOverloadView> overload(fn);
        if (overloads->size() > 0) {
          Handle<ir::FunctionOverloadView> last(overloads->last_child());
          if (!last->name()) {
            SYNTAX_ERROR_POS("SyntaxError function overload must have a name", overload->source_position());
          } else if (!last->name()->string_equals(overload->name())) {
            SYNTAX_ERROR_POS("SyntaxError function overload must have a same name", overload->name()->source_position());
          }

          if (last->generator() != overload->generator()) {
            SYNTAX_ERROR_POS("SyntaxError generator function can only overloaded by generator function", overload->name()->source_position());
          }
        }
        overloads->InsertLast(fn);
      } else {
        return fn;
      }
    } else {
      SYNTAX_ERROR("SyntaxError incomplete function definition", Current());
    }
  }
}


template <typename UCharInputIterator>
Handle<ir::Node> Parser<UCharInputIterator>::ParseFunctionOverloadOrImplementation(Handle<ir::Node> overloads, bool yield, bool has_default, bool declaration) {
  LOG_PHASE(ParseFunctionOverloadOrImplementation);
  if (Current()->type() == Token::TS_FUNCTION) {
    bool generator = false;
    TokenInfo info = *Current();
    Next();
    if (Current()->type() == Token::TS_MUL) {
      generator = true;
      Next();
    }
    Handle<ir::Node> name;
    if (Current()->type() == Token::TS_IDENTIFIER) {
      name = ParseIdentifier();
    }

    if (declaration && !name) {
      SYNTAX_ERROR("SyntaxError function name required", Current());
    }
    
    Handle<ir::Node> call_signature = ParseCallSignature(false);
    Handle<ir::Node> ret;
    if (Current()->type() == Token::TS_LEFT_BRACE) {
      Handle<ir::Node> body = ParseFunctionBody(yield? yield: generator);
      ret = New<ir::FunctionView>(overloads, name, call_signature, body);
    } else if (overloads) {
      ret = New<ir::FunctionOverloadView>(generator, name, call_signature);
      if (IsLineTermination()) {
        ConsumeLineTerminator();
      } else {
        SYNTAX_ERROR("SyntaxError ';' expected", Current());
      }
    } else {
      SYNTAX_ERROR("SyntaxError invalid function definition", (&info));
    }
    ret->SetInformationForNode(&info);
    return ret;
  }
  SYNTAX_ERROR("SyntaxError 'function' expected", Current());
}


template <typename UCharInputIterator>
Handle<ir::Node> Parser<UCharInputIterator>::ParseParameterList(bool accesslevel_allowed) {
  LOG_PHASE(ParseParameterList);
  if (Current()->type() == Token::TS_LEFT_PAREN) {
    Handle<ir::ParamList> param_list = New<ir::ParamList>();
    param_list->SetInformationForNode(Current());
    Next();

    if (Current()->type() == Token::TS_RIGHT_PAREN) {
      Next();
      return param_list;
    }
    
    bool has_rest = false;
    while (1) {
      if (has_rest) {
        ARROW_PARAMETERS_ERROR_POS("Rest parameter must be at the end of the parameters", param_list->last_child()->source_position());
      }
      if (Current()->type() == Token::TS_IDENTIFIER ||
          Current()->type() == Token::TS_PRIVATE ||
          Current()->type() == Token::TS_PUBLIC ||
          Current()->type() == Token::TS_PROTECTED) {
        param_list->InsertLast(ParseParameter(false, accesslevel_allowed));
      } else if (Current()->type() == Token::TS_REST) {
        has_rest = true;
        TokenInfo token = (*Current());
        Next();
        Handle<ir::Node> node = New<ir::RestParamView>(ParseParameter(true, accesslevel_allowed));
        node->SetInformationForNode(&token);
        param_list->InsertLast(node);
      } else {
        SYNTAX_ERROR("SyntaxError unexpected token in formal parameter list", Current());
      }

      if (Current()->type() == Token::TS_COMMA) {
        Next();
      } else if (Current()->type() == Token::TS_RIGHT_PAREN) {
        Next();
        return param_list;
      } else {
        SYNTAX_ERROR("SyntaxError ')' or ',' expected in parameter list", Current());
      }
    }
  }
  SYNTAX_ERROR("SyntaxError '(' is expected in parameter list", Current());
}


template <typename UCharInputIterator>
Handle<ir::Node> Parser<UCharInputIterator>::ParseParameter(bool rest, bool accesslevel_allowed) {
  LOG_PHASE(ParseParameter);
  Handle<ir::Node> access_level;
  if (Current()->type() == Token::TS_PUBLIC || Current()->type() == Token::TS_PRIVATE) {
    if (accesslevel_allowed) {
      access_level = New<ir::ClassFieldAccessLevelView>(Current()->type());
      access_level->SetInformationForNode(Current());
    } else {
      ARROW_PARAMETERS_ERROR("SyntaxError 'private' or 'public' not allowed here", Current());
    }
    Next();
  }
  if (Current()->type() == Token::TS_IDENTIFIER) {
    Handle<ir::ParameterView> pv = New<ir::ParameterView>();
    pv->SetInformationForNode(Current());
    Handle<ir::NameView> nv = New<ir::NameView>(Current()->value());
    nv->SetInformationForNode(Current());
    pv->set_access_level(access_level);
    pv->set_name(nv);
    Next();
    if (Current()->type() == Token::TS_QUESTION_MARK) {
      if (rest) {
        ARROW_PARAMETERS_ERROR("SyntaxError optional parameter not allowed in rest parameter", Current());
      }
      Next();
      pv->set_optional(true);
    }
    if (Current()->type() == Token::TS_COLON) {
      Next();
      if (Current()->type() == Token::TS_STRING_LITERAL) {
        pv->set_type_expr(ParseStringLiteral());
      } else {
        pv->set_type_expr(ParseTypeExpression());
      }
    }
    if (Current()->type() == Token::TS_ASSIGN) {
      if (rest) {
        ARROW_PARAMETERS_ERROR("SyntaxError default parameter not allowed in rest parameter", Current());
      }
      Next();
      pv->set_value(ParseAssignmentExpression(true, false));
    }
    return pv;
  }
  SYNTAX_ERROR("SyntaxError identifier expected", Current());
}


template <typename UCharInputIterator>
Handle<ir::Node> Parser<UCharInputIterator>::ParseFunctionBody(bool yield) {
  LOG_PHASE(ParseFunctionBody);
  if (Current()->type() == Token::TS_LEFT_BRACE) {
    Handle<ir::Scope> scope = NewScope();
    set_current_scope(scope);
    auto block = New<ir::BlockView>(scope);
    block->SetInformationForNode(Current());
    Next();
    while (1) {
      if (Current()->type() == Token::TS_RIGHT_BRACE) {
        Next();
        break;
      }
      auto node = ParseStatementListItem(yield, true, false, false);
      block->InsertLast(node);
      if (IsLineTermination()) {
        ConsumeLineTerminator();
      }
    }
    set_current_scope(scope->parent_scope());
    return block;
  }
  SYNTAX_ERROR("SyntaxError unexpected token in 'function body'", Current());
}


template <typename UCharInputIterator>
Handle<ir::Node> Parser<UCharInputIterator>::ParseEmptyStatement() {
  if (Current()->type() == Token::LINE_TERMINATOR) {
    Next();
    auto node = New<ir::Empty>();
    node->SetInformationForNode(Current());
    return node;
  }
  SYNTAX_ERROR("SyntaxError ';' expected.", Current());
}
}
