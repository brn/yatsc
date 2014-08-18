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


template <typename UCharInputIterator>
ir::Node* Parser<UCharInputIterator>::ParseStatement(bool yield, bool has_return) {
  LOG_PHASE(ParseStatement);
  ir::Node* result = nullptr;
  
  switch (Current()->type()) {
    case Token::TS_LEFT_BRACE:
      result = ParseBlockStatement(yield, has_return);
      break;

    // case Token::TS_MODULE:
    //   result = ParseModuleStatement();
    //   break;

    // case Token::TS_EXPORT:
    //   result = ParseExportStatement();
    //   break;

    case Token::TS_VAR:
      result = ParseVariableDeclaration(true, yield);
      break;

    case Token::TS_IF:
      result = ParseIfStatement(yield, has_return);
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
      result = ParseContinueStatement(yield);
      break;

    case Token::TS_BREAK:
      result = ParseBreakStatement();
      break;

    case Token::TS_RETURN:
      result = ParseReturnStatement(yield);
      break;

    case Token::TS_WITH:
      result = ParseWithStatement(yield, has_return);
      break;

    case Token::TS_SWITCH:
      result = ParseSwitchStatement(yield, has_return);
      break;

    case Token::TS_THROW:
      result = ParseThrowStatement();
      break;

    case Token::TS_TRY:
      result = ParseTryStatement(yield, has_return);
      break;

    // case Token::TS_IMPORT:
    //   result = ParseImportStatement();
    //   break;

    case Token::TS_FUNCTION:
      result = ParseFunction(yield, false);
      break;

    case Token::TS_DEBUGGER:
      result = ParseDebuggerStatement();
      break;

    case Token::END_OF_INPUT:
      SYNTAX_ERROR("SyntaxError Unexpected end of input", Current());
      
    default: {
      TokenCursor cursor = token_buffer_.cursor();
      result = ParseExpression(true, yield);
      result = New<ir::StatementView>(result);
      result->SetInformationForNode(token_buffer_.Peek(cursor));
    }
  }
  
  return result;
}


template <typename UCharInputIterator>
ir::Node* Parser<UCharInputIterator>::ParseDeclaration(bool error, bool yield, bool has_default) {
  switch (Current()->type()) {
    case Token::TS_FUNCTION:
      return ParseFunction(yield, has_default);
    case Token::TS_CLASS:
      return ParseClassDeclaration(yield, has_default);
    case Token::TS_LET:
    case Token::TS_CONST:
      return ParseLexicalDeclaration(true, yield);
    default:
      if (!error) {
        return nullptr;
      }
      SYNTAX_ERROR("SyntaxError unexpected token", Current());
  }
}


template <typename UCharInputIterator>
ir::Node* Parser<UCharInputIterator>::ParseBlockStatement(bool yield, bool has_return) {
  // const TokenInfo* token_info = Seek();
  // BlockView* block_view = New<BlockView>();

  // if (token_info->type() == Token::TS_LEFT_BRACE) {
  //   while (1) {
  //     ir::Node* statement = ParseSourceElement();
  //     block_view->InsertLast(statement);
  //     token_info = Seek();
  //     if (token_info->type() == Token::TS_RIGHT_BRACE) {
  //       Scan();
  //       break;
  //     } else if (token->type() == Token::END_OF_INPUT) {
  //       SYNTAX_ERROR("Unexpected end of input.", token_info);
  //     }
  //   }
  //   return block_view;
  // }
  // SYNTAX_ERROR("Illegal token " << token_info->value()->ToUtf8Value());
  return nullptr;
}


template <typename UCharInputIterator>
ir::Node* Parser<UCharInputIterator>::ParseStatementListItem(bool yield, bool has_return) {
  ir::Node* node = ParseDeclaration(false, yield, false);
  if (node == nullptr) {
    return ParseStatement(yield, has_return);
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
      } else if (CheckLineTermination(token_buffer_.Rewind(1))) {
        if (Current()->type() == Token::LINE_TERMINATOR) {Next();}
        break;
      } else {
        SYNTAX_ERROR("SyntaxError unexpected token in lexical declaration", Current());
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
  ir::Node* key = ParsePropertyDefinition(yield);
  ir::Node* elem = nullptr;
  ir::Node* init = nullptr;
  if (Current()->type() == Token::TS_COLON) {
    Next();
    elem = ParseBindingElement(yield, generator_parameter);
  } else if (key->HasStringView()) {
    SYNTAX_ERROR_POS("SyntaxError 'identifier' expected", key->source_position());
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
      ir::Node* node = ParseVariableDeclaration();
      vars->InsertLast(node);
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
ir::Node* Parser<UCharInputIterator>::ParseIfStatement(bool yield, bool has_return) {
  if (Current()->type() == Token::TS_IF) {
    TokenInfo info = *(Current());
    Next();
    if (Current()->type() == Token::TS_LEFT_PAREN) {
      Next();
      ir::Node* expr = ParseExpression(true, yield);
      if (Current()->type() == Token::TS_RIGHT_PAREN) {
        ir::Node* then_stmt = ParseStatement(yield, has_return);
        ir::Node* else_stmt = nullptr;
        if (Current()->type() == Token::TS_ELSE) {
          else_stmt = ParseStatement(yield, has_return);
        }
        ir::IfStatementView* if_stmt = New<ir::IfStatementView>(expr, then_stmt, else_stmt);
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
ir::Node* Parser<UCharInputIterator>::ParseWhileStatement(bool yield, bool has_return) {
  if (Current()->type() == Token::TS_WHILE) {
    TokenInfo info = *(Current());
    Next();
    if (Current()->type() == Token::TS_LEFT_PAREN) {
      Next();
      ir::Node* expr = ParseExpression(true, yield);
      if (Current()->type() == Token::TS_RIGHT_PAREN) {
        Next();
        ir::Node* stmt = ParseStatement(yield, has_return);
        ir::WhileStatementView* while_stmt = New<ir::WhileStatementView>(expr, stmt);
        while_stmt->SetInformationForNode(&info);
        return while_stmt;
      }
      SYNTAX_ERROR("SyntaxError ')' expected", Current());
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
  if (Current()->type() == Token::TS_DO) {
    TokenInfo info = *(Current());
    Next();
    ir::Node* stmt = ParseStatement(yield, has_return);
    if (Current()->type() == Token::TS_WHILE) {
      Next();
      if (Current()->type() == Token::TS_LEFT_PAREN) {
        ir::Node* expr = ParseExpression(true, yield);
        if (Current()->type() == Token::TS_RIGHT_PAREN) {
          CheckLineTermination();
          ir::DoWhileStatementView* do_while = New<ir::DoWhileStatementView>(expr, stmt);
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
  if (Current() == Token::TS_FOR) {
    TokenInfo info = *(Current());
    Next();
    if (Current() == Token::TS_LEFT_PAREN) {
      Next();
      ir::Node* reciever = nullptr;
      switch (Current()->type()) {
        case Token::TS_VAR:
          reciever = ParseVariableStatement(true, yield);
        case Token::TS_LET:
          reciever = ParseLexicalDeclaration(true, yield);
        default: {
          TokenCursor cursor = token_buffer_.cursor();
          reciever = ParseExpression(true, yield);
          if (Current()->type() != Token::LINE_TERMINATOR) {
            token_buffer_.SetCursorPosition(cursor);
            reciever = ParseLeftHandSideExpression(true, yield);
          }
        }
      }
      return ParseForIterationStatement(reciever, &info, yield, has_return);
    }
    SYNTAX_ERROR("SyntaxError '(' expected", Current());
  }
  SYNTAX_ERROR("SyntaxError 'for' expected", Current());
}


template <typename UCharInputIterator>
ir::Node* Parser<UCharInputIterator>::ParseForIterationStatement(ir::Node* reciever, TokenInfo* token_info, bool yield, bool has_return) {
  ir::Node* target = nullptr;
  if (Current()->type() == Token::LINE_TERMINATOR) {
    // for (var i = 0; i < 10; i++) ...
    Next();
    ir::Node* cond = ParseExpression(true, yield);
    if (Current()->type() == Token::LINE_TERMINATOR) {
      Next();
      ir::Node* incr = ParseExpression(true, yield);
      if (Current()->type() == Token::TS_RIGHT_PAREN) {
        Next();
        ir::Node* stmt = ParseStatement(yield, has_return);
        ir::ForStatementView* for_stmt = New<ir::ForStatementView>(reciever, cond, incr, stmt);
        for_stmt->SetInformationForNode(token_info);
        return for_stmt;
      }
    }
  } else if (Current()->type() == Token::TS_IDENTIFIER &&
             Current()->value() == "of") {
    // for (var i in obj) ...
    Next();
    target = ParseAssignmentExpression(true, yield);
    if (Current()->type() == Token::TS_RIGHT_PAREN) {
      Next();
      ir::Node* stmt = ParseStatement(yield, has_return);
      ir::ForOfStatementView* for_stmt = New<ir::ForOfStatementView>(reciever, target, stmt);
      for_stmt->SetInformationForNode(token_info);
      return for_stmt;
    }
    SYNTAX_ERROR("SyntaxError ')' expected", Current());
  } else if (Current()->type() == Token::TS_IN) {
    // for (var i of obj) ...
    Next();
    target = ParseAssignmentExpression(false, yield);
    if (Current()->type() == Token::TS_RIGHT_PAREN) {
      Next();
      ir::Node* stmt = ParseStatement(yield, has_return);
      ir::ForInStatementView* for_stmt = New<ir::ForInStatementView>(reciever, target, stmt);
      for_stmt->SetInformationForNode(token_info);
      return for_stmt;
    }
    SYNTAX_ERROR("SyntaxError 'in' expected", Current());
  }
  SYNTAX_ERROR("SyntaxError ';' or 'in' or 'of' expected", Current());
}


template <typename UCharInputIterator>
ir::Node* Parser<UCharInputIterator>::ParseContinueStatement() {
  if (Current()->type() == Token::TS_CONTINUE) {
    TokenCursor cursor = token_buffer_.cursor();
    Next();
    ir::Node* result;
    YATSC_SCOPED([&]{
      result->SetInformationForNode(token_buffer_.Peek(cursor));
    });
    if (Current()->type() == Token::TS_IDENTIFIER) {
      return result = New<ir::ContinueStatementView>(ParsePrimaryExpression());
    }
    return result = New<ir::ContinueStatementView>();
  }
  SYNTAX_ERROR("SyntaxError 'continue' expected", Current());
}


template <typename UCharInputIterator>
ir::Node* Parser<UCharInputIterator>::ParseBreakStatement() {
  if (Current()->type() == Token::TS_BREAK) {
    TokenCursor cursor = token_buffer_.cursor();
    Next();
    ir::Node* result;
    YATSC_SCOPED([&]{
      result->SetInformationForNode(token_buffer_.Peek(cursor));
    });
    if (Current()->type() == Token::TS_IDENTIFIER) {
      return result = New<ir::BreakStatementView>(ParsePrimaryExpression());
    }
    return result = New<ir::BreakStatementView>();
  }
  SYNTAX_ERROR("SyntaxError 'break' expected", Current());
}


template <typename UCharInputIterator>
ir::Node* Parser<UCharInputIterator>::ParseReturnStatement(bool yield) {
  if (Current()->type() == Token::TS_RETURN) {
    TokenCursor cursor = token_buffer_.cursor();
    ir::Node* result;
    YATSC_SCOPED([&]{
      result->SetInformationForNode(token_buffer_.Peek(cursor));
    });
    if (CheckLineTermination()) {
      return result = New<ir::ReturnStatementView>();
    }
    ir::Node* expr = ParseExpression(true, yield);
    return result = New<ir::ReturnStatementView>(expr);
  }
  SYNTAX_ERROR("SyntaxError 'return' expected", Current());
}


template <typename UCharInputIterator>
ir::Node* Parser<UCharInputIterator>::ParseWithStatement(bool yield, bool has_return) {
  if (Current()->type() == Token::TS_WITH) {
    TokenCursor cursor = token_buffer_.cursor();
    ir::Node* result;
    YATSC_SCOPED([&]{
      result->SetInformationForNode(token_buffer_.Peek(cursor));
    });
    Next();
    if (Current()->type() == Token::TS_LEFT_PAREN) {
      Next();
      ir::Node* expr = ParseExpression(true, yield);
      if (Current()->type() == Token::TS_RIGHT_PAREN) {
        Next();
        ir::Node stmt = ParseStatement(yield, has_return);
        return New<ir::WithStatementView>(expr, stmt);
      }
      SYNTAX_ERROR("SyntaxError ')' expected", Current());
    }
    SYNTAX_ERROR("SyntaxError '(' expected", Current());
  }
  SYNTAX_ERROR("SyntaxError 'with' expected", Current());
}


template <typename UCharInputIterator>
ir::Node* Parser<UCharInputIterator>::ParseSwitchStatement(bool yield, bool has_return) {
  if (Current()->type() == Token::TS_SWITCH) {
    TokenCursor cursor = token_buffer_.cursor();
    Next();
    if (Current()->type() == Token::TS_LEFT_PAREN) {
      Next();
      ir::Node* expr = ParseExpression(true, yield);
      if (Current()->type() == Token::TS_RIGHT_PAREN) {
        if (Current()->type() == Token::TS_LEFT_BRACE) {
          Next();
          ir::Node* case_clauses = ParseCaseClauses(yield, has_return);
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
ir::Node* Parser<UCharInputIterator>::ParseCaseClauses(bool yield, bool has_return) {
  ir::CaseListView* case_list = New<ir::CaseListView>();
  case_list->SetInformationForNode(Current());
  while (1) {
    ir::Node* expr = nullptr;
    TokenCursor cursor = token_buffer_.cursor();
    switch (Current()->type()) {
      case Token::TS_CASE: {
        Next();
        expr = ParseExpression(true, yield);
      }
      case Token::TS_DEFAULT: {
        ir::Node* body = New<ir::CaseBody>();
        while (1) {
          if (Current()->type() == Token::TS_LEFT_BRACE) {
            body->InsertLast(ParseBlockStatement(yield));
          } else {
            body->InsertLast(ParseStatementListItem(yield, has_return));
          }
          switch (Current()->type()) {
            case Token::TS_CASE:
            case Token::TS_DEFAULT:
              continue;
            default:
              SYNTAX_ERROR("SyntaxError unexpected token", Current());
          }
        }
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
ir::Node* Parser<UCharInputIterator>::ParseLabelledStatement() {
  return nullptr;
}


template <typename UCharInputIterator>
ir::Node* Parser<UCharInputIterator>::ParseThrowStatement() {
  if (Current()->type() == Token::TS_THROW) {
    if (!CheckLineTermination()) {
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
ir::Node* Parser<UCharInputIterator>::ParseTryStatement(bool yield, bool has_return) {
  if (Current()->type() == Token::TS_TRY) {
    TokenCursor cursor = token_buffer_.cursor();
    Next();
    ir::Node* block = ParseBlockStatement(yield, has_return);
    ir::Node* catch_block = nullptr;
    ir::Node* finally_block = nullptr;
    bool has_catch_or_finally = false;
    if (Current()->type() == Token::TS_CATCH) {
      has_catch_or_finally = true;
      catch_block = ParseCatchBlock(yield, has_return);
    }
    if (Current()->type() == Token::TS_FINALLY) {
      has_catch_or_finally = true;
      finally_block = ParseFinallyBlock(yield, has_return);
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
ir::Node* Parser<UCharInputIterator>::ParseCatchBlock(bool yield, bool has_return) {
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
        ir::Node* block = ParseBlockStatement(yield, has_return);
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
ir::Node* Parser<UCharInputIterator>::ParseFinallyBlock(bool yield, bool has_return) {
  if (Current()->type() == Token::TS_FINALLY) {
    TokenCursor cursor = token_buffer_.cursor();
    Next();
    ir::Node* block = ParseBlockStatement(yield, has_return);
    ir::FinallyStatementView* finally_stmt = New<ir::FinallyStatementView>(block);
    finally_stmt->SetInformationForNode(token_buffer_.Peek(cursor));
    return finally_stmt;
  }
  SYNTAX_ERROR("SyntaxError 'finally' expected", Current());
}

template <typename UCharInputIterator>
ir::Node* Parser<UCharInputIterator>::ParseDebuggerStatement() {
  if (Current()->type() == Token::TS_DEBUGGER) {
    ir::Node* ret = New<ir::DebuggerView>();
    ret->SetInformationForNode(Current());
    Next();
    return ret;
  }
}


template <typename UCharInputIterator>
ir::Node* Parser<UCharInputIterator>::ParseClassDeclaration() {
  return nullptr;
}


template <typename UCharInputIterator>
ir::Node* Parser<UCharInputIterator>::ParseFunction(bool yield, bool has_default) {
  LOG_PHASE(ParseFunction);
  if (Current()->type() == Token::TS_FUNCTION) {
    bool generator = false;
    TokenInfo token = (*Current());
    Next();
    if (Current()->type() == Token::TS_MOD) {
      generator = true;
      Next();
    }
    ir::Node* name = nullptr;
    if (Current()->type() == Token::TS_IDENTIFIER) {
      name = ParseLiteral();
    }
    ir::Node* call_signature = ParseCallSignature(false);
    ir::Node* body = ParseFunctionBody(yield? yield: generator);
    ir::Node* ret = New<ir::FunctionView>(name, call_signature, body);
    ret->SetInformationForNode(&token);
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
      auto node = ParseSourceElement();
      block->InsertLast(node);
      if (Current()->type() == Token::TS_RIGHT_BRACE) {
        break;
      }
    }
    return block;
  }
  SYNTAX_ERROR("SyntaxError unexpected token in 'function body'", Current());
}
}
