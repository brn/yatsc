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

template <typename UCharInputSourceIterator>
ir::Node* Parser<UCharInputSourceIterator>::ParseProgram() {
  return nullptr;
}


// Parse root statements.
template <typename UCharInputIterator>
ir::Node* Parser<UCharInputIterator>::ParseSourceElements() {
  // BlockView* block_view = New<BlockView>();
  
  // while (1) {
  //   const TokenInfo* token_info = Peek();
  //   if (Token::END_OF_INPUT == token_info) {
  //     break;
  //   }
  //   ir::Node* statement = ParseSourceElement();
  //   block_view->InsertLast(statement);
  // }
  
  // return block_view;
  return nullptr;
}


// Parse root statement.
template <typename UCharInputIterator>
ir::Node* Parser<UCharInputIterator>::ParseSourceElement() {
  // ir::Node* result = nullptr;
  // const TokenInfo* token_info = Peek();
  // switch (token_info->type()) {
  //   case Token::TS_CLASS: {
  //     result = ParseClassDeclaration();
  //   }
  //     break;
      
  //   case Token::TS_FUNCTION: {
  //     result = ParseFunction();
  //   }
  //     break;

  //   case Token::TS_VAR: {
  //     result = ParseVariableDeclaration();
  //   }
  //     break;

  //   case Token::END_OF_INPUT: {
  //     SYNTAX_ERROR("Unexpected end of input.", token_info);
  //   }
  //     break;

  //   default:
  //     result = ParseStatement();
  // }
  // return result;
  return nullptr;
}


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
ir::Node* Parser<UCharInputIterator>::ParseStatement(bool yield, bool has_return, bool breakable, bool continuable) {
  LOG_PHASE(ParseStatement);
  ir::Node* result = nullptr;
  
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
      ir::Node* node = ParseVariableStatement(true, yield);
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
        TokenCursor cursor = GetBufferCursorPosition();
        Next();
        if (Current()->type() == Token::TS_COLON) {
          SetBufferCursorPosition(cursor);
          return ParseLabelledStatement(yield, has_return, breakable, continuable);
        }
        SetBufferCursorPosition(cursor);
      }
      TokenCursor cursor = GetBufferCursorPosition();
      result = ParseExpression(true, yield);
      result = New<ir::StatementView>(result);
      result->SetInformationForNode(PeekBuffer(cursor));
    }
  }
  
  return result;
}


template <typename UCharInputIterator>
ir::Node* Parser<UCharInputIterator>::ParseDeclaration(bool error, bool yield, bool has_default) {
  switch (Current()->type()) {
    case Token::TS_FUNCTION:
      return ParseFunctionOverloads(yield, has_default, true);
    case Token::TS_CLASS:
      return ParseClassDeclaration(yield, has_default);
    case Token::TS_LET:
    case Token::TS_CONST: {
      ir::Node* node = ParseLexicalDeclaration(true, yield);
      if (IsLineTermination()) {
        ConsumeLineTerminator();
      } else {
        SYNTAX_ERROR("SyntaxError ';' expected", Current());
      }
      return node;
    }
    default:
      if (!error) {
        return nullptr;
      }
      SYNTAX_ERROR("SyntaxError unexpected token", Current());
  }
}


template <typename UCharInputIterator>
ir::Node* Parser<UCharInputIterator>::ParseBlockStatement(bool yield, bool has_return, bool breakable, bool continuable) {
  LOG_PHASE(ParseBlockStatement);
  auto block_view = New<ir::BlockView>();
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
        ir::Node* statement = ParseStatementListItem(yield, has_return, breakable, continuable);
        block_view->InsertLast(statement);
      }
    }
    return block_view;
  }
  SYNTAX_ERROR("SyntaxError '{' expected", Current());
  return nullptr;
}


template <typename UCharInputIterator>
ir::Node* Parser<UCharInputIterator>::ParseStatementListItem(bool yield, bool has_return, bool breakable, bool continuable) {
  ir::Node* node = ParseDeclaration(false, yield, false);
  if (node == nullptr) {
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
ir::Node* Parser<UCharInputIterator>::ParseLexicalDeclaration(bool in, bool yield) {
  LOG_PHASE(ParseLexicalDeclaration);
  if (Current()->type() == Token::TS_LET ||
      Current()->type() == Token::TS_CONST) {
    bool has_const = Current()->type() == Token::TS_CONST;
    ir::LexicalDeclView* lexical_decl = New<ir::LexicalDeclView>(Current()->type());
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
ir::Node* Parser<UCharInputIterator>::ParseLexicalBinding(bool const_decl, bool in, bool yield) {
  LOG_PHASE(ParseLexicalBinding);
  ir::Node* value = nullptr;
  ir::Node* lhs = nullptr;
  if (Current()->type() == Token::TS_IDENTIFIER) {
    lhs = ParseBindingIdentifier(false, in, yield);
  } else {
    lhs = ParseBindingPattern(yield, false);
  }

  if (lhs == nullptr) {
    SYNTAX_ERROR("SyntaxError left hand side of lexical binding is invalid", Current());
  }

  ir::Node* type_expr = nullptr;
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

  ir::Node* ret = New<ir::VariableView>(lhs, value, type_expr);
  ret->SetInformationForNode(lhs);
  return ret;
}


// binding_identifier[default, yield]
//   : [+default] 'default'
//   | [~yield] 'yield' identifier
//   | identifier
//   ;
template <typename UCharInputIterator>
ir::Node* Parser<UCharInputIterator>::ParseBindingIdentifier(bool default_allowed, bool in, bool yield) {
  LOG_PHASE(ParseBindingIdentifier);
  ir::Node* ret;
  if (Current()->type() == Token::TS_DEFAULT) {
    if (!default_allowed) {
      SYNTAX_ERROR("SyntaxError 'default' keyword not allowed here", Current());
    }
    ret = New<ir::DefaultView>();
  } else if (Current()->type() == Token::TS_YIELD) {
    ret = New<ir::YieldView>(false, nullptr);
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
ir::Node* Parser<UCharInputIterator>::ParseBindingPattern(bool yield, bool generator_parameter) {
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
ir::Node* Parser<UCharInputIterator>::ParseObjectBindingPattern(bool yield, bool generator_parameter) {
  LOG_PHASE(ParseObjectBindingPattern);
  if (Current()->type() == Token::TS_LEFT_BRACE) {
    ir::Node* binding_prop_list = New<ir::BindingPropListView>();
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
ir::Node* Parser<UCharInputIterator>::ParseArrayBindingPattern(bool yield, bool generator_parameter) {
  LOG_PHASE(ParseArrayBindingPattern);
  if (Current()->type() == Token::TS_LEFT_BRACKET) {
    ir::Node* binding_array = New<ir::BindingArrayView>();
    binding_array->SetInformationForNode(Current());
    Next();
    bool exit = false;
    while (1) {
      if (Current()->type() == Token::TS_COMMA) {
        Next();
        binding_array->InsertLast(nullptr);
      }
      if (Current()->type() == Token::TS_REST) {
        ir::RestParamView* rest = New<ir::RestParamView>();
        rest->SetInformationForNode(Current());
        Next();
        rest->set_parameter(ParseBindingIdentifier(false, true, yield));
        binding_array->InsertLast(rest);
        exit = true;
      } else {
        ir::Node* elem = ParseBindingElement(yield, generator_parameter);
        ir::Node* init = nullptr;
        if (Current()->type() == Token::TS_ASSIGN) {
          init = ParseAssignmentExpression(true, yield);
        }
        ir::Node* ret = New<ir::BindingElementView>(nullptr, elem, init);
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
ir::Node* Parser<UCharInputIterator>::ParseBindingProperty(bool yield, bool generator_parameter) {
  LOG_PHASE(ParseBindingProperty);
  ir::Node* key = nullptr;
  ir::Node* elem = nullptr;
  ir::Node* init = nullptr;

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
  ir::Node* ret = New<ir::BindingElementView>(key, elem, init);
  ret->SetInformationForNode(key);
  return ret;
}


// binding_element[yield, generator_parameter ]
//   : single_name_binding[?yield, ?generator_parameter]
//   | [+generator_parameter] binding_pattern[?yield,generator_parameter] initializer__opt[in]
//   | [~generator_parameter] binding_pattern[?yield] initializer__opt[in, ?yield]
//   ;
template <typename UCharInputIterator>
ir::Node* Parser<UCharInputIterator>::ParseBindingElement(bool yield, bool generator_parameter) {
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
ir::Node* Parser<UCharInputIterator>::ParseVariableStatement(bool in, bool yield) {
  if (Current()->type() == Token::TS_VAR) {
    Next();
    ir::VariableDeclView* vars = New<ir::VariableDeclView>();
    while (1) {
      ir::Node* node = ParseVariableDeclaration(in, yield);
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
ir::Node* Parser<UCharInputIterator>::ParseVariableDeclaration(bool in, bool yield) {
  LOG_PHASE(ParseVariableDeclaration);
  ir::Node* value = nullptr;
  ir::Node* lhs = nullptr;
  if (Current()->type() == Token::TS_IDENTIFIER) {
    lhs = ParseBindingIdentifier(false, in, yield);
  } else {
    lhs = ParseBindingPattern(yield, false);
  }

  if (lhs == nullptr) {
    SYNTAX_ERROR("SyntaxError left hand side of variable declaration is invalid", Current());
  }

  ir::Node* type_expr = nullptr;
  if (Current()->type() == Token::TS_COLON) {
    Next();
    type_expr = ParseTypeExpression();
  }
  
  if (Current()->type() == Token::TS_ASSIGN) {
    Next();
    value = ParseAssignmentExpression(in, yield);
  }

  ir::Node* ret = New<ir::VariableView>(lhs, value, type_expr);
  ret->SetInformationForNode(lhs);
  return ret;
}


// if_statement[yield, return]
//   : 'if' '(' expression[in, ?yield] ')' statement[?yield, ?return] 'else' statement[?yield, ?return]
//   | 'if' '(' expression[in, ?yield] ')' statement[?yield, ?return]
//   ;
template <typename UCharInputIterator>
ir::Node* Parser<UCharInputIterator>::ParseIfStatement(bool yield, bool has_return, bool breakable, bool continuable) {
  LOG_PHASE(ParseIfStatement);
  if (Current()->type() == Token::TS_IF) {
    TokenCursor cursor = GetBufferCursorPosition();
    Next();
    if (Current()->type() == Token::TS_LEFT_PAREN) {
      Next();
      ir::Node* expr = ParseExpression(true, yield);
      if (Current()->type() == Token::TS_RIGHT_PAREN) {
        Next();
        ir::Node* then_stmt = ParseStatement(yield, has_return, breakable, continuable);
        ir::Node* else_stmt = nullptr;
        if (Current()->type() == Token::TS_ELSE) {
          Next();
          else_stmt = ParseStatement(yield, has_return, breakable, continuable);
        }
        ir::IfStatementView* if_stmt = New<ir::IfStatementView>(expr, then_stmt, else_stmt);
        if_stmt->SetInformationForNode(PeekBuffer(cursor));
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
ir::Node* Parser<UCharInputIterator>::ParseWhileStatement(bool yield, bool has_return) {
  LOG_PHASE(ParseWhileStatement);
  if (Current()->type() == Token::TS_WHILE) {
    TokenInfo info = *(Current());
    Next();
    if (Current()->type() == Token::TS_LEFT_PAREN) {
      Next();
      ir::Node* expr = ParseExpression(true, yield);
      ir::WhileStatementView* while_stmt = New<ir::WhileStatementView>(expr, ParseIterationBody(yield, has_return));
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
ir::Node* Parser<UCharInputIterator>::ParseDoWhileStatement(bool yield, bool has_return) {
  LOG_PHASE(ParseDoWhileStatement);
  if (Current()->type() == Token::TS_DO) {
    TokenInfo info = *(Current());
    Next();
    ir::Node* stmt = ParseStatement(yield, has_return, true, true);
    if (Current()->type() == Token::TS_WHILE) {
      Next();
      if (Current()->type() == Token::TS_LEFT_PAREN) {
        Next();
        ir::Node* expr = ParseExpression(true, yield);
        if (Current()->type() == Token::TS_RIGHT_PAREN) {
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
ir::Node* Parser<UCharInputIterator>::ParseForStatement(bool yield, bool has_return) {
  LOG_PHASE(ParseForStatement);
  if (Current()->type() == Token::TS_FOR) {
    TokenCursor cursor = GetBufferCursorPosition();
    Next();
    if (Current()->type() == Token::TS_LEFT_PAREN) {
      Next();
      ir::Node* reciever = nullptr;
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
          TokenCursor cursor = token_buffer_.cursor();
          reciever = ParseExpression(true, yield);
          if (Current()->type() != Token::LINE_TERMINATOR) {
            token_buffer_.SetCursorPosition(cursor);
            reciever = ParseLeftHandSideExpression(yield);
          }
        }
      }
      return ParseForIteration(reciever, cursor, yield, has_return);
    }
    SYNTAX_ERROR("SyntaxError '(' expected", Current());
  }
  SYNTAX_ERROR("SyntaxError 'for' expected", Current());
}


template <typename UCharInputIterator>
ir::Node* Parser<UCharInputIterator>::ParseForIteration(ir::Node* reciever, TokenCursor cursor, bool yield, bool has_return) {
  LOG_PHASE(ParseForIterationStatement);

  ir::Node* second = nullptr;
  ir::Node* third = nullptr;
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

  ir::Node* body = ParseIterationBody(yield, has_return);
  ir::Node* ret = nullptr;
  
  if (for_in) {
    ret = New<ir::ForInStatementView>(reciever, second, body);
  } else if (for_of) {
    ret = New<ir::ForOfStatementView>(reciever, second, body);
  } else {
    ret = New<ir::ForStatementView>(reciever, second, third, body);
  }
    
  ret->SetInformationForNode(PeekBuffer(cursor));
  return ret;
}


template <typename UCharInputIterator>
ir::Node* Parser<UCharInputIterator>::ParseIterationBody(bool yield, bool has_return) {
  if (Current()->type() == Token::TS_RIGHT_PAREN) {
    Next();
    return ParseStatement(yield, has_return, true, true);
  }
  SYNTAX_ERROR("SyntaxError ')' expected", Current());
}


template <typename UCharInputIterator>
ir::Node* Parser<UCharInputIterator>::ParseContinueStatement(bool yield) {
  LOG_PHASE(ParseContinueStatement);
  if (Current()->type() == Token::TS_CONTINUE) {
    TokenCursor cursor = token_buffer_.cursor();
    Next();
    ir::Node* result = nullptr;
    YATSC_SCOPED([&]{
      if (result != nullptr) {
        result->SetInformationForNode(token_buffer_.Peek(cursor));
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
ir::Node* Parser<UCharInputIterator>::ParseBreakStatement(bool yield) {
  LOG_PHASE(ParseBreakStatement);
  if (Current()->type() == Token::TS_BREAK) {
    TokenCursor cursor = token_buffer_.cursor();
    Next();
    ir::Node* result = nullptr;
    YATSC_SCOPED([&]{
      if (result != nullptr) {
        result->SetInformationForNode(token_buffer_.Peek(cursor));
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
ir::Node* Parser<UCharInputIterator>::ParseReturnStatement(bool yield) {
  LOG_PHASE(ParseReturnStatement);
  if (Current()->type() == Token::TS_RETURN) {
    TokenCursor cursor = GetBufferCursorPosition();
    ir::Node* result = nullptr;
    YATSC_SCOPED([&]{
      if (result != nullptr) {
        result->SetInformationForNode(PeekBuffer(cursor));
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
ir::Node* Parser<UCharInputIterator>::ParseWithStatement(bool yield, bool has_return, bool breakable, bool continuable) {
  LOG_PHASE(ParseWithStatement);
  if (Current()->type() == Token::TS_WITH) {
    TokenCursor cursor = GetBufferCursorPosition();
    ir::Node* result = nullptr;
    YATSC_SCOPED([&]{
      if (result != nullptr) {
        result->SetInformationForNode(PeekBuffer(cursor));
      }
    });
    
    Next();
    
    if (Current()->type() == Token::TS_LEFT_PAREN) {
      Next();
      ir::Node* expr = ParseExpression(true, yield);
      if (Current()->type() == Token::TS_RIGHT_PAREN) {
        Next();
        ir::Node* stmt = ParseStatement(yield, has_return, breakable, continuable);
        return result = New<ir::WithStatementView>(expr, stmt);
      }
      SYNTAX_ERROR("SyntaxError ')' expected", Current());
    }
    SYNTAX_ERROR("SyntaxError '(' expected", Current());
  }
  SYNTAX_ERROR("SyntaxError 'with' expected", Current());
}


template <typename UCharInputIterator>
ir::Node* Parser<UCharInputIterator>::ParseSwitchStatement(bool yield, bool has_return, bool continuable) {
  LOG_PHASE(ParseSwitchStatement);
  if (Current()->type() == Token::TS_SWITCH) {
    TokenCursor cursor = token_buffer_.cursor();
    Next();
    if (Current()->type() == Token::TS_LEFT_PAREN) {
      Next();
      ir::Node* expr = ParseExpression(true, yield);
      if (Current()->type() == Token::TS_RIGHT_PAREN) {
        Next();
        if (Current()->type() == Token::TS_LEFT_BRACE) {
          Next();
          ir::Node* case_clauses = ParseCaseClauses(yield, has_return, continuable);
          if (Current()->type() == Token::TS_RIGHT_BRACE) {
            Next();
            ir::Node* switch_stmt = New<ir::SwitchStatementView>(expr, case_clauses);
            switch_stmt->SetInformationForNode(token_buffer_.Peek(cursor));
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
ir::Node* Parser<UCharInputIterator>::ParseCaseClauses(bool yield, bool has_return, bool continuable) {
  LOG_PHASE(ParseCaseClauses);
  ir::CaseListView* case_list = New<ir::CaseListView>();
  case_list->SetInformationForNode(Current());
  bool default_encounted = false;
  while (1) {
    bool normal_case = false;
    ir::Node* expr = nullptr;
    TokenCursor cursor = token_buffer_.cursor();
    
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
        ir::Node* body = New<ir::CaseBody>();
        while (1) {
          if (Current()->type() == Token::TS_LEFT_BRACE) {
            body->InsertLast(ParseBlockStatement(yield, has_return, true, continuable));
          } else {
            body->InsertLast(ParseStatementListItem(yield, has_return, true, continuable));
          }
          switch (Current()->type()) {
            case Token::TS_CASE:
            case Token::TS_DEFAULT:
              if (default_encounted && Current()->type() == Token::TS_DEFAULT) {
                SYNTAX_ERROR("SyntaxError More than one 'default' clause in switch statement", Current());
              }
              goto END;
            case Token::TS_RIGHT_BRACE:
              goto END;
            default:
              SYNTAX_ERROR("SyntaxError unexpected token", Current());
          }
        }
     END:
        ir::CaseView* case_view = New<ir::CaseView>(expr, body);
        case_view->SetInformationForNode(token_buffer_.Peek(cursor));
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
ir::Node* Parser<UCharInputIterator>::ParseLabelledStatement(bool yield, bool has_return, bool breakable, bool continuable) {
  if (Current()->type() == Token::TS_IDENTIFIER) {
    ir::Node* identifier = ParseLabelIdentifier(yield);
    if (Current()->type() == Token::TS_COLON) {
      Next();
      ir::Node* stmt = ParseLabelledItem(yield, has_return, breakable, continuable);
      auto node = New<ir::LabelledStatementView>(identifier, stmt);
      node->SetInformationForNode(identifier);
      return node;
    }
    SYNTAX_ERROR("SyntaxError ':' expected", Current());
  }
  SYNTAX_ERROR("SyntaxError 'identifier' expected", Current());
}


template <typename UCharInputIterator>
ir::Node* Parser<UCharInputIterator>::ParseLabelledItem(bool yield, bool has_return, bool breakable, bool continuable) {
  if (Current()->type() == Token::TS_FUNCTION) {
    return ParseFunctionOverloads(yield, false, true);
  }
  return ParseStatement(yield, has_return, breakable, continuable);
}


template <typename UCharInputIterator>
ir::Node* Parser<UCharInputIterator>::ParseThrowStatement() {
  LOG_PHASE(ParseThrowStatement);
  if (Current()->type() == Token::TS_THROW) {
    if (!IsLineTermination()) {
      TokenCursor cursor = token_buffer_.cursor();
      Next();
      ir::Node* expr = ParseExpression(false, false);
      ir::ThrowStatementView* throw_stmt = New<ir::ThrowStatementView>(expr);
      throw_stmt->SetInformationForNode(token_buffer_.Peek(cursor));
      return throw_stmt;
    }
    SYNTAX_ERROR("SyntaxError throw statement expected expression", Current());
  }
  SYNTAX_ERROR("SyntaxError throw expected", Current());
}


template <typename UCharInputIterator>
ir::Node* Parser<UCharInputIterator>::ParseTryStatement(bool yield, bool has_return, bool breakable, bool continuable) {
  LOG_PHASE(ParseTryStatement);
  if (Current()->type() == Token::TS_TRY) {
    TokenCursor cursor = token_buffer_.cursor();
    Next();
    ir::Node* block = ParseBlockStatement(yield, has_return, breakable, continuable);
    ir::Node* catch_block = nullptr;
    ir::Node* finally_block = nullptr;
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
    ir::TryStatementView* try_stmt = New<ir::TryStatementView>(block, catch_block, finally_block);
    try_stmt->SetInformationForNode(token_buffer_.Peek(cursor));
    return try_stmt;
  }
  SYNTAX_ERROR("SyntaxError 'try' expected", Current());
}


template <typename UCharInputIterator>
ir::Node* Parser<UCharInputIterator>::ParseCatchBlock(bool yield, bool has_return, bool breakable, bool continuable) {
  LOG_PHASE(ParseCatchBlock);
  if (Current()->type() == Token::TS_CATCH) {
    TokenCursor cursor = token_buffer_.cursor();
    Next();
    if (Current()->type() == Token::TS_LEFT_PAREN) {
      Next();
      ir::Node* catch_parameter = nullptr;
      if (Current()->type() == Token::TS_IDENTIFIER) {
        catch_parameter = ParseBindingIdentifier(false, false, yield);
      } else {
        catch_parameter = ParseBindingPattern(yield, false);
      }

      if (Current()->type() == Token::TS_RIGHT_PAREN) {
        Next();
        ir::Node* block = ParseBlockStatement(yield, has_return, breakable, continuable);
        ir::CatchStatementView* catch_stmt = New<ir::CatchStatementView>(catch_parameter, block);
        catch_stmt->SetInformationForNode(token_buffer_.Peek(cursor));
        return catch_stmt;
      }
      SYNTAX_ERROR("SyntaxError ')' expected", Current());
    }
    SYNTAX_ERROR("SyntaxError '(' expected", Current());
  }
  SYNTAX_ERROR("SyntaxError 'catch' expected", Current());
}


template <typename UCharInputIterator>
ir::Node* Parser<UCharInputIterator>::ParseFinallyBlock(bool yield, bool has_return, bool breakable, bool continuable) {
  LOG_PHASE(ParseFinallyBlock);
  if (Current()->type() == Token::TS_FINALLY) {
    TokenCursor cursor = token_buffer_.cursor();
    Next();
    ir::Node* block = ParseBlockStatement(yield, has_return, breakable, continuable);
    ir::FinallyStatementView* finally_stmt = New<ir::FinallyStatementView>(block);
    finally_stmt->SetInformationForNode(token_buffer_.Peek(cursor));
    return finally_stmt;
  }
  SYNTAX_ERROR("SyntaxError 'finally' expected", Current());
}

template <typename UCharInputIterator>
ir::Node* Parser<UCharInputIterator>::ParseDebuggerStatement() {
  LOG_PHASE(ParseDebuggerStatement);
  if (Current()->type() == Token::TS_DEBUGGER) {
    ir::Node* ret = New<ir::DebuggerView>();
    ret->SetInformationForNode(Current());
    Next();
    return ret;
  }
  SYNTAX_ERROR("SyntaxError 'debugger' expected", Current());
}


template <typename UCharInputIterator>
ir::Node* Parser<UCharInputIterator>::ParseClassDeclaration(bool yield, bool has_default) {
  LOG_PHASE(ParseClassDeclaration);
  if (Current()->type() == Token::TS_CLASS) {
    TokenCursor cursor = GetBufferCursorPosition();
    Next();
    ir::Node* name = ParseBindingIdentifier(false, false);
    ir::Node* bases = ParseClassBases();
    if (Current()->type() == Token::TS_LEFT_BRACE) {
      Next();
      ir::Node* body = ParseClassBody();
      auto class_decl = New<ir::ClassDeclView>(name, bases, body);
      class_decl->SetInformationForNode(PeekBuffer(cursor));
      return class_decl;
    }
    SYNTAX_ERROR("SyntaxError '{' expected", Current());
  }
  SYNTAX_ERROR("SyntaxError 'class' expected", Current());
}


template <typename UCharInputIterator>
ir::Node* Parser<UCharInputIterator>::ParseClassBases() {
  bool extends_keyword = false;
  auto bases = New<ir::ClassBasesView>();
  auto impls = New<ir::ClassImplsView>();
  bases->SetInformationForNode(Current());
  
  while (1) {
    if (Current()->type() == Token::TS_EXTENDS) {
      if (extends_keyword) {
        SYNTAX_ERROR("SyntaxError class extendable only one class", Current());
      }
      TokenCursor heritage_cursor = GetBufferCursorPosition();
      Next();
      extends_keyword = true;
      auto heritage = New<ir::ClassHeritageView>(ParseReferencedType());
      heritage->SetInformationForNode(PeekBuffer(heritage_cursor));
      bases->set_base(heritage);
    } else if (Current()->type() == Token::TS_IMPLEMENTS) {
      Next();
      impls->InsertLast(ParseReferencedType());
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
ir::Node* Parser<UCharInputIterator>::ParseClassBody() {
  auto fields = New<ir::ClassFieldListView>();
  while (1) {
    if (Current()->type() != Token::TS_RIGHT_BRACE) {
      fields->InsertLast(ParseClassElement());
    } else {
      return fields;
    }
  }
}


template <typename UCharInputIterator>
ir::Node* Parser<UCharInputIterator>::ParseClassElement() {
  ir::Node* mods = ParseFieldModifiers();
  if (Current()->type() == Token::TS_IDENTIFIER) {
    if (Current()->value() == "constructor") {
      return ParseConstructorOverloads(mods);
    } else {
      TokenCursor cursor = GetBufferCursorPosition();
      Next();
      if (Current()->type() == Token::TS_LEFT_PAREN) {
        SetBufferCursorPosition(cursor);
        return ParseMemberFunctionOverloads(mods);
      } else {
        SetBufferCursorPosition(cursor);
        return ParseMemberVariable(mods);
      }
    }
  } else if (Current()->type() == Token::TS_MUL) {
    return ParseGeneratorMethodOverloads(mods);
  }
  SYNTAX_ERROR("SyntaxError unexpected token", Current());
}


template <typename UCharInputIterator>
ir::Node* Parser<UCharInputIterator>::ParseFieldModifiers() {
  LOG_PHASE(ParseFieldModifiers);
  auto mods = New<ir::ClassFieldModifiersView>();
  mods->SetInformationForNode(Current());
  
  if (Current()->type() == Token::TS_STATIC) {
    ir::Node* mod = ParseFieldModifier();
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
    ir::Node* mod1 = ParseFieldModifier();
    if (Current()->type() == Token::TS_STATIC) {
      ir::Node* mod2 = ParseFieldModifier();
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
ir::Node* Parser<UCharInputIterator>::ParseFieldModifier() {  
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
      return nullptr;
  }
}


template <typename UCharInputIterator>
template <typename T>
void Parser<UCharInputIterator>::SetModifiers(bool* first, ir::Node* mods, T fn) {
  if (*first) {
    fn->set_modifiers(mods);
    *first = false;
    if (mods->size() > 0) {
      fn->SetInformationForNode(mods);
    }
  }
}


template <typename UCharInputIterator>
ir::Node* Parser<UCharInputIterator>::ParseConstructorOverloads(ir::Node* mods) {
  LOG_PHASE(ParseConstructorOverloads);
  auto overloads = New<ir::MemberFunctionOverloadsView>();
  bool first = true;
  while (1) {
    if ((Current()->type() == Token::TS_IDENTIFIER &&
        Current()->value() == "constructor") ||
        Current()->type() == Token::TS_PUBLIC ||
        Current()->type() == Token::TS_PRIVATE ||
        Current()->type() == Token::TS_PROTECTED) {
      ir::Node* fn = ParseConstructorOverloadOrImplementation(first, overloads);
      if (fn->HasMemberFunctionOverloadView()) {
        SetModifiers(&first, mods, fn->ToMemberFunctionOverloadView());
        overloads->InsertLast(fn);
        ValidateOverload(fn->ToMemberFunctionOverloadView(), overloads);
      } else {
        SetModifiers(&first, mods, fn->ToMemberFunctionView());
        ValidateOverload(fn->ToMemberFunctionView(), overloads);
        return fn;
      }
    } else {
      SYNTAX_ERROR("SyntaxError incomplete constructor definition", Current());
    }
  }
}


template <typename UCharInputIterator>
ir::Node* Parser<UCharInputIterator>::ParseConstructorOverloadOrImplementation(bool first, ir::Node* overloads) {
  LOG_PHASE(ParseConstructorOverloadOrImplementation);
  ir::Node* mods = nullptr;
  if (!first) {
    mods = ParseFieldModifiers();
  }
  if (Current()->type() == Token::TS_IDENTIFIER &&
      Current()->value() == "constructor") {
    TokenCursor cursor = GetBufferCursorPosition();
    ir::Node* name = ParseIdentifier();
    ir::Node* call_signature = ParseCallSignature(true);
    ir::Node* ret = nullptr;
    if (Current()->type() == Token::TS_LEFT_BRACE) {
      ir::Node* body = ParseFunctionBody(false);
      ret = New<ir::MemberFunctionView>(mods, name, call_signature, overloads, body);
    } else if (overloads != nullptr) {
      ret = New<ir::MemberFunctionOverloadView>(mods, name, call_signature);
      if (IsLineTermination()) {
        ConsumeLineTerminator();
      } else {
        SYNTAX_ERROR("SyntaxError ';' expected", Current());
      }
    } else {
      SYNTAX_ERROR("SyntaxError invalid constructor definition", PeekBuffer(cursor));
    }
    ret->SetInformationForNode(PeekBuffer(cursor));
    return ret;
  }
  SYNTAX_ERROR("SyntaxError 'constructor' expected", Current());
}


template <typename UCharInputIterator>
void Parser<UCharInputIterator>::ValidateOverload(ir::MemberFunctionDefinitionView* node, ir::Node* overloads) {
  if (overloads->size() > 0) {
    auto last = overloads->last_child()->ToMemberFunctionOverloadView();
    if (!node->name()->string_equals(last->at(1))) {
      SYNTAX_ERROR_POS("SyntaxError member function overload must have a same name", node->at(1)->source_position());
    }
    if (!node->modifiers()->Equals(last->modifiers())) {
      ir::Node* target = nullptr;
      if (node->modifiers()->size() > last->modifiers()->size()) {
        target = node->modifiers()->first_child();
      } else {
        target = last->modifiers()->first_child();
      }
      SYNTAX_ERROR_POS("SyntaxError member function overload must have same modifiers", target->source_position()); 
    }
  }
}


template <typename UCharInputIterator>
ir::Node* Parser<UCharInputIterator>::ParseMemberFunctionOverloads(ir::Node* mods) {
  LOG_PHASE(ParseMemberFunctionOverloads);
  auto overloads = New<ir::MemberFunctionOverloadsView>();
  bool first = true;
  while (1) {
    TokenCursor cursor = GetBufferCursorPosition();
    Next();
    if (PeekBuffer(cursor)->type() == Token::TS_IDENTIFIER ||
        PeekBuffer(cursor)->type() == Token::TS_PUBLIC ||
        PeekBuffer(cursor)->type() == Token::TS_PRIVATE ||
        PeekBuffer(cursor)->type() == Token::TS_STATIC ||
        PeekBuffer(cursor)->type() == Token::TS_PROTECTED) {
      SetBufferCursorPosition(cursor);
      ir::Node* fn = ParseMemberFunctionOverloadOrImplementation(first, overloads);
      if (fn->HasMemberFunctionOverloadView()) {
        SetModifiers(&first, mods, fn->ToMemberFunctionOverloadView());
        ValidateOverload(fn->ToMemberFunctionOverloadView(), overloads);
        overloads->InsertLast(fn);
      } else {
        SetModifiers(&first, mods, fn->ToMemberFunctionView());
        ValidateOverload(fn->ToMemberFunctionView(), overloads);
        return fn;
      }
    } else {
      SYNTAX_ERROR("SyntaxError incomplete member function definition", PeekBuffer(cursor));
    }
  }
}


template <typename UCharInputIterator>
ir::Node* Parser<UCharInputIterator>::ParseMemberFunctionOverloadOrImplementation(bool first, ir::Node* overloads) {
  LOG_PHASE(ParseMemberFunctionOverloadOrImplementation);
  ir::Node* mods = nullptr;
  if (!first) {
    mods = ParseFieldModifiers();
  }
  if (Current()->type() == Token::TS_IDENTIFIER) {
    TokenCursor cursor = GetBufferCursorPosition();
    ir::Node* name = ParseIdentifier();    
    ir::Node* call_signature = ParseCallSignature(false);
    ir::Node* ret = nullptr;
    if (Current()->type() == Token::TS_LEFT_BRACE) {
      ir::Node* body = ParseFunctionBody(false);
      ret = New<ir::MemberFunctionView>(mods, name, call_signature, overloads, body);
    } else if (overloads != nullptr) {
      ret = New<ir::MemberFunctionOverloadView>(mods, name, call_signature);
      if (IsLineTermination()) {
        ConsumeLineTerminator();
      } else {
        SYNTAX_ERROR("SyntaxError ';' expected", Current());
      }
    } else {
      SYNTAX_ERROR("SyntaxError invalid member function definition", PeekBuffer(cursor));
    }
    ret->SetInformationForNode(PeekBuffer(cursor));
    return ret;
  }
  SYNTAX_ERROR("SyntaxError identifier expected", Current());
}


template <typename UCharInputIterator>
ir::Node* Parser<UCharInputIterator>::ParseGeneratorMethodOverloads(ir::Node* mods) {
  LOG_PHASE(ParseGeneratorMethodOverloads);
  auto overloads = New<ir::MemberFunctionOverloadsView>();
  bool first = true;
  while (1) {
    TokenCursor cursor = GetBufferCursorPosition();
    Next();
    if ((PeekBuffer(cursor)->type() == Token::TS_IDENTIFIER ||
         PeekBuffer(cursor)->type() == Token::TS_PUBLIC ||
         PeekBuffer(cursor)->type() == Token::TS_PRIVATE ||
         PeekBuffer(cursor)->type() == Token::TS_STATIC ||
         PeekBuffer(cursor)->type() == Token::TS_PROTECTED) &&
        Current()->type() == Token::TS_LEFT_PAREN) {
      SetBufferCursorPosition(cursor);
      ir::Node* fn = ParseGeneratorMethodOverloadOrImplementation(first, overloads);
      if (fn->HasMemberFunctionOverloadView()) {
        SetModifiers(&first, mods, fn->ToMemberFunctionView());
        ValidateOverload(fn->ToMemberFunctionOverloadView(), overloads);
        overloads->InsertLast(fn);
      } else {
        SetModifiers(&first, mods, fn->ToMemberFunctionView());
        ValidateOverload(fn->ToMemberFunctionView(), overloads);
        return fn;
      }
    } else {
      SYNTAX_ERROR("SyntaxError incomplete member function definition", Current());
    }
  }
}


template <typename UCharInputIterator>
ir::Node* Parser<UCharInputIterator>::ParseGeneratorMethodOverloadOrImplementation(bool first, ir::Node* overloads) {
  LOG_PHASE(ParseGeneratorMethodOverloadOrImplementation);
  ir::Node* mods = nullptr;
  if (!first) {
    mods = ParseFieldModifiers();
  }
  if (Current()->type() == Token::TS_MUL) {
    TokenCursor cursor = GetBufferCursorPosition();
    Next();
    if (Current()->type() == Token::TS_IDENTIFIER) {
      Next();
      ir::Node* name = ParseIdentifier();    
      ir::Node* call_signature = ParseCallSignature(false);
      ir::Node* ret = nullptr;
      if (Current()->type() == Token::TS_LEFT_BRACE) {
        ir::Node* body = ParseFunctionBody(false);
        ret = New<ir::MemberFunctionView>(mods, name, call_signature, overloads, body);
      } else if (overloads != nullptr) {
        ret = New<ir::MemberFunctionOverloadView>(mods, name, call_signature);
        if (IsLineTermination()) {
          ConsumeLineTerminator();
        } else {
          SYNTAX_ERROR("SyntaxError ';' expected", Current());
        }
      } else {
        SYNTAX_ERROR("SyntaxError invalid member function definition", PeekBuffer(cursor));
      }
      ret->SetInformationForNode(PeekBuffer(cursor));
      return ret;
    }
    SYNTAX_ERROR("SyntaxError identifier expected", Current());
  }
  SYNTAX_ERROR("SyntaxError '*' expected", Current());
}


template <typename UCharInputIterator>
ir::Node* Parser<UCharInputIterator>::ParseMemberVariable(ir::Node* mods) {
  if (Current()->type() == Token::TS_IDENTIFIER) {
    ir::Node* identifier = ParseIdentifier();
    ir::Node* value = nullptr;
    ir::Node* type = nullptr;
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
ir::Node* Parser<UCharInputIterator>::ParseFunctionOverloads(bool yield, bool has_default, bool declaration) {
  auto overloads = New<ir::FunctionOverloadsView>();
  while (1) {
    if (Current()->type() == Token::TS_FUNCTION) {
      ir::Node* fn = ParseFunctionOverloadOrImplementation(overloads, yield, has_default, declaration);
      if (fn->HasFunctionOverloadView()) {
        ir::FunctionOverloadView* overload = fn->ToFunctionOverloadView();
        if (overloads->size() > 0) {
          auto last = overloads->last_child()->ToFunctionOverloadView();
          if (last->name() == nullptr) {
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
ir::Node* Parser<UCharInputIterator>::ParseFunctionOverloadOrImplementation(ir::Node* overloads, bool yield, bool has_default, bool declaration) {
  LOG_PHASE(ParseFunction);
  if (Current()->type() == Token::TS_FUNCTION) {
    bool generator = false;
    TokenCursor cursor = GetBufferCursorPosition();
    Next();
    if (Current()->type() == Token::TS_MUL) {
      generator = true;
      Next();
    }
    ir::Node* name = nullptr;
    if (Current()->type() == Token::TS_IDENTIFIER) {
      name = ParseIdentifier();
    }

    if (declaration && name == nullptr) {
      SYNTAX_ERROR("SyntaxError function name required", Current());
    }
    
    ir::Node* call_signature = ParseCallSignature(false);
    ir::Node* ret = nullptr;
    if (Current()->type() == Token::TS_LEFT_BRACE) {
      ir::Node* body = ParseFunctionBody(yield? yield: generator);
      ret = New<ir::FunctionView>(overloads, name, call_signature, body);
    } else if (overloads != nullptr) {
      ret = New<ir::FunctionOverloadView>(generator, name, call_signature);
      if (IsLineTermination()) {
        ConsumeLineTerminator();
      } else {
        SYNTAX_ERROR("SyntaxError ';' expected", Current());
      }
    } else {
      SYNTAX_ERROR("SyntaxError invalid function definition", PeekBuffer(cursor));
    }
    ret->SetInformationForNode(PeekBuffer(cursor));
    return ret;
  }
  SYNTAX_ERROR("SyntaxError 'function' expected", Current());
}


template <typename UCharInputIterator>
ir::Node* Parser<UCharInputIterator>::ParseParameterList(bool accesslevel_allowed) {
  LOG_PHASE(ParseParameterList);
  if (Current()->type() == Token::TS_LEFT_PAREN) {
    ir::ParamList* param_list = New<ir::ParamList>();
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
      if (Current()->type() == Token::TS_IDENTIFIER) {
        param_list->InsertLast(ParseParameter(false, accesslevel_allowed));
      } else if (Current()->type() == Token::TS_REST) {
        has_rest = true;
        TokenInfo token = (*Current());
        Next();
        ir::Node* node = New<ir::RestParamView>(ParseParameter(true, accesslevel_allowed));
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
ir::Node* Parser<UCharInputIterator>::ParseParameter(bool rest, bool accesslevel_allowed) {
  LOG_PHASE(ParseParameter);
  ir::Node* access_level = nullptr;
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
    ir::ParameterView* pv = New<ir::ParameterView>();
    pv->SetInformationForNode(Current());
    ir::NameView* nv = New<ir::NameView>(Current()->value());
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
      pv->set_type_expr(ParseTypeExpression());
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
ir::Node* Parser<UCharInputIterator>::ParseFunctionBody(bool yield) {
  LOG_PHASE(ParseFunctionBody);
  if (Current()->type() == Token::TS_LEFT_BRACE) {
    auto block = New<ir::BlockView>();
    block->SetInformationForNode(Current());
    Next();
    while (1) {
      if (Current()->type() == Token::TS_RIGHT_BRACE) {
        Next();
        break;
      }
      auto node = ParseStatementListItem(yield, true, false, false);
      block->InsertLast(node);
    }
    return block;
  }
  SYNTAX_ERROR("SyntaxError unexpected token in 'function body'", Current());
}


template <typename UCharInputIterator>
ir::Node* Parser<UCharInputIterator>::ParseEmptyStatement() {
  auto node = New<ir::Empty>();
  node->SetInformationForNode(Current());
  return node;
}
}
