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

// Generate SyntaxError and throw it.
// Usage. SYNTAX_ERROR("test " << message, Current())
#define SYNTAX_ERROR(message, token)                  \
  SYNTAX_ERROR_POS(message, token->source_position())


// Generate ArrowParametersError and throw it.
// Usage. ARROW_PARAMETERS_ERROR("test " << message, Current())
#define ARROW_PARAMETERS_ERROR(message, token)                  \
  ARROW_PARAMETERS_ERROR_POS(message, token->source_position())


// Generate SyntaxError that is pointed specified position and throw it.
// Usage. SYNTAX_ERROR_POS("test " << message, node->source_position())
#define SYNTAX_ERROR_POS(message, pos)          \
  SYNTAX_ERROR__(message, pos, SyntaxError)


// Generate ArrowParametersError that is pointed specified position and throw it.
// Usage. ARROW_PARAMETERS_ERROR_POS("test " << message, node->source_position())
#define ARROW_PARAMETERS_ERROR_POS(message, pos)      \
  SYNTAX_ERROR__(message, pos, ArrowParametersError)



#ifndef DEBUG
// Throw error and return nullptr.
#define SYNTAX_ERROR__(message, pos, error)     \
  (*error_reporter_) << message;                \
  error_reporter_->Throw<error>(pos);           \
  return nullptr
#else
// Throw error that has source line and number for the error thrown position.
#define SYNTAX_ERROR__(message, pos, error)                             \
  (*error_reporter_) << message << '\n' << __FILE__ << ":" << __LINE__; \
  error_reporter_->Throw<error>(pos);                                   \
  return nullptr
#endif


#ifdef DEBUG
// Logging current parse phase.
#define ENTER(name)                                                     \
  if (print_parser_phase_) {                                            \
    if (Current() != nullptr) {                               \
      Printf("%sEnter %s: CurrentToken = %s\n", indent_.c_str(), #name, Current()->ToString()); \
    } else {                                                            \
      Printf("%sEnter %s: CurrentToken = null\n", indent_.c_str(), #name); \
    }                                                                   \
  }                                                                     \
  indent_ += "  ";                                                      \
  YATSC_SCOPED([&]{                                                     \
      indent_ = indent_.substr(0, indent_.size() - 2);                  \
      if (this->print_parser_phase_) {                                  \
        if (this->Current() != nullptr) {                     \
          Printf("%sExit %s: CurrentToken = %s\n", indent_.c_str(), #name, Current()->ToString()); \
        } else {                                                        \
          Printf("%sExit %s: CurrentToken = null\n", indent_.c_str(), #name); \
        }                                                               \
      }                                                                 \
    })
#else
// Disabled.
#define ENTER(name)
#endif


// Parse source string and create Node.
template <typename UCharInputIterator>
ir::Node* Parser<UCharInputIterator>::Parse() {
  // auto file_scope_view = New<FileScopeView>();
  // file_scope_view->InsertLast(ParseProgram());
  // return file_scope_view;
  return nullptr;
}


// Parse program.
template <typename UCharInputIterator>
ir::Node* Parser<UCharInputIterator>::ParseProgram() {
  // ir::Node* source_elements = ParseSourceElements();
  // return source_elements;
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
ir::Node* Parser<UCharInputIterator>::ParseStatementList() {
  return nullptr;
}


template <typename UCharInputIterator>
ir::Node* Parser<UCharInputIterator>::ParseStatement() {
  // ir::Node* result = nullptr;
  
  // const TokenInfo* token_info = Peek();
  // switch (token_info->type()) {
  //   case Token::TS_LEFT_BRACE:
  //     result = ParseBlockStatement();
  //     break;

  //   case Token::TS_MODULE:
  //     result = ParseModuleStatement();
  //     break;

  //   case Token::TS_EXPORT:
  //     result = ParseExportStatement();
  //     break;

  //   case Token::TS_VAR:
  //     result = ParseVariableDeclaration();
  //     break;

  //   case Token::TS_IF:
  //     result = ParseIfStatement();
  //     break;

  //   case Token::TS_FOR:
  //     result = ParseForStatement();
  //     break;

  //   case Token::TS_WHILE:
  //     result = ParseWhileStatement();
  //     break;

  //   case Token::TS_DO:
  //     result = ParseDoWhileStatement();
  //     break;

  //   case Token::TS_CONTINUE:
  //     result = ParseContinueStatement();
  //     break;

  //   case Token::TS_BREAK:
  //     result = ParseBreakStatement();
  //     break;

  //   case Token::TS_RETURN:
  //     result = ParseReturnStatement();
  //     break;

  //   case Token::TS_WITH:
  //     result = ParseWithStatement();
  //     break;

  //   case Token::TS_SWITCH:
  //     result = ParseSwitchStatement();
  //     break;

  //   case Token::TS_THROW:
  //     result = ParseThrowStatement();
  //     break;

  //   case Token::TS_TRY:
  //     result = ParseTryStatement();
  //     break;

  //   case Token::TS_IMPORT:
  //     result = ParseImportStatement();
  //     break;

  //   case Token::TS_FUNCTION:
  //     result = ParseFunction();
  //     break;

  //   case Token::TS_DEBUGGER:
  //     result = ParseDebuggerStatement();
  //     break;

  //   case Token::END_OF_INPUT:
  //     SYNTAX_ERROR("Unexpected end of input.", token_info);
      
  //   default: {
  //     SYNTAX_ERROR("Illegal token.", token_info);
  //   }
  // }
  
  // return result;
  return nullptr;
}


template <typename UCharInputIterator>
ir::Node* Parser<UCharInputIterator>::ParseBlockStatement() {
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
ir::Node* Parser<UCharInputIterator>::ParseModuleStatement() {
  return nullptr;
}


template <typename UCharInputIterator>
ir::Node* Parser<UCharInputIterator>::ParseImportStatement() {
  return nullptr;
}


template <typename UCharInputIterator>
ir::Node* Parser<UCharInputIterator>::ParseExportStatement()  {
  return nullptr;
}


template <typename UCharInputIterator>
ir::Node* Parser<UCharInputIterator>::ParseDebuggerStatement() {
  return nullptr;
}


// lexical_declaration[in, yield]
//   : let_or_const binding_list[?in, ?yield]
//   ;
// let_or_const
//   : 'let'
//   | 'const'
//   ;
template <typename UCharInputIterator>
ir::Node* Parser<UCharInputIterator>::ParseLexicalDeclaration(bool has_in, bool has_yield) {
  ENTER(ParseLexicalDeclaration);
  if (Current()->type() == Token::TS_LET ||
      Current()->type() == Token::TS_CONST) {
    bool has_const = Current()->type() == Token::TS_CONST;
    ir::LexicalDeclView* lexical_decl = New<ir::LexicalDeclView>(Current()->type());
    lexical_decl->SetInformationForNode(Current());
    Next();
    while (1) {
      lexical_decl->InsertLast(ParseLexicalBinding(has_const, has_in, has_yield));
      if (Current()->type() == Token::TS_COMMA) {
        Next();
      } else if (Current()->has_line_break_before_next() ||
                 Current()->has_line_terminator_before_next() ||
                 Current()->type() == Token::END_OF_INPUT) {
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
ir::Node* Parser<UCharInputIterator>::ParseLexicalBinding(bool const_decl, bool has_in, bool has_yield) {
  ENTER(ParseLexicalBinding);
  ir::Node* value = nullptr;
  ir::Node* lhs = nullptr;
  if (Current()->type() == Token::TS_IDENTIFIER) {
    lhs = ParseBindingIdentifier(false, has_in, has_yield);
  } else {
    lhs = ParseBindingPattern(has_yield, false);
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
    value = ParseAssignmentExpression(has_in, has_yield);
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
ir::Node* Parser<UCharInputIterator>::ParseBindingIdentifier(bool default_allowed, bool has_in, bool has_yield) {
  ENTER(ParseBindingIdentifier);
  ir::Node* ret;
  if (Current()->type() == Token::TS_DEFAULT) {
    if (!default_allowed) {
      SYNTAX_ERROR("SyntaxError 'default' keyword not allowed here", Current());
    }
    ret = New<ir::DefaultView>();
  } else if (Current()->type() == Token::TS_YIELD) {
    ret = New<ir::YieldView>();
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
ir::Node* Parser<UCharInputIterator>::ParseBindingPattern(bool has_yield, bool generator_parameter) {
  ENTER(ParseBindingPattern);
  switch (Current()->type()) {
    case Token::TS_LEFT_BRACE:
      return ParseObjectBindingPattern(has_yield, generator_parameter);
    case Token::TS_LEFT_BRACKET:
      return ParseArrayBindingPattern(has_yield, generator_parameter);
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
ir::Node* Parser<UCharInputIterator>::ParseObjectBindingPattern(bool has_yield, bool generator_parameter) {
  ENTER(ParseObjectBindingPattern);
  if (Current()->type() == Token::TS_LEFT_BRACE) {
    ir::Node* binding_prop_list = New<ir::BindingPropListView>();
    binding_prop_list->SetInformationForNode(Current());
    Next();
    while (1) {
      binding_prop_list->InsertLast(ParseBindingProperty(has_yield, generator_parameter));
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
ir::Node* Parser<UCharInputIterator>::ParseArrayBindingPattern(bool has_yield, bool generator_parameter) {
  ENTER(ParseArrayBindingPattern);
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
        rest->set_parameter(ParseBindingIdentifier(false, true, has_yield));
        binding_array->InsertLast(rest);
        exit = true;
      } else {
        ir::Node* elem = ParseBindingElement(has_yield, generator_parameter);
        ir::Node* init = nullptr;
        if (Current()->type() == Token::TS_ASSIGN) {
          init = ParseAssignmentExpression(true, has_yield);
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
ir::Node* Parser<UCharInputIterator>::ParseBindingProperty(bool has_yield, bool generator_parameter) {
  ENTER(ParseBindingProperty);
  ir::Node* key = ParseObjectKey();
  ir::Node* elem = nullptr;
  ir::Node* init = nullptr;
  if (Current()->type() == Token::TS_COLON) {
    Next();
    elem = ParseBindingElement(has_yield, generator_parameter);
  } else if (key->HasStringView()) {
    SYNTAX_ERROR_POS("SyntaxError 'identifier' expected", key->source_position());
  } else if (Current()->type() == Token::TS_ASSIGN) {
    Next();
    init = ParseAssignmentExpression(true, has_yield);
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
ir::Node* Parser<UCharInputIterator>::ParseBindingElement(bool has_yield, bool generator_parameter) {
  ENTER(ParseBindingElement);
  switch (Current()->type()) {
    case Token::TS_IDENTIFIER:
    case Token::TS_YIELD:
    case Token::TS_DEFAULT:
      return ParseBindingIdentifier(false, false, has_yield);
    default:
      return ParseBindingPattern(has_yield, generator_parameter);
  }
}


template <typename UCharInputIterator>
ir::Node* Parser<UCharInputIterator>::ParseVariableDeclaration(bool has_in) {
  return nullptr;
}


template <typename UCharInputIterator>
ir::Node* Parser<UCharInputIterator>::ParseIfStatement() {
  return nullptr;
}


template <typename UCharInputIterator>
ir::Node* Parser<UCharInputIterator>::ParseWhileStatement() {
  return nullptr;
}


template <typename UCharInputIterator>
ir::Node* Parser<UCharInputIterator>::ParseDoWhileStatement() {
  return nullptr;
}


template <typename UCharInputIterator>
ir::Node* Parser<UCharInputIterator>::ParseForStatement() {
  return nullptr;
}


template <typename UCharInputIterator>
ir::Node* Parser<UCharInputIterator>::ParseContinueStatement() {
  return nullptr;
}


template <typename UCharInputIterator>
ir::Node* Parser<UCharInputIterator>::ParseBreakStatement() {
  return nullptr;
}


template <typename UCharInputIterator>
ir::Node* Parser<UCharInputIterator>::ParseReturnStatement() {
  return nullptr;
}


template <typename UCharInputIterator>
ir::Node* Parser<UCharInputIterator>::ParseWithStatement() {
  return nullptr;
}


template <typename UCharInputIterator>
ir::Node* Parser<UCharInputIterator>::ParseSwitchStatement() {
  return nullptr;
}


template <typename UCharInputIterator>
ir::Node* Parser<UCharInputIterator>::ParseCaseClauses() {
  return nullptr;
}


template <typename UCharInputIterator>
ir::Node* Parser<UCharInputIterator>::ParseLabelledStatement() {
  return nullptr;
}


template <typename UCharInputIterator>
ir::Node* Parser<UCharInputIterator>::ParseThrowStatement() {
  return nullptr;
}


template <typename UCharInputIterator>
ir::Node* Parser<UCharInputIterator>::ParseTryStatement() {
  return nullptr;
}


template <typename UCharInputIterator>
ir::Node* Parser<UCharInputIterator>::ParseCatchBlock() {
  return nullptr;
}


template <typename UCharInputIterator>
ir::Node* Parser<UCharInputIterator>::ParseFinallyBlock() {
  return nullptr;
}


template <typename UCharInputIterator>
ir::Node* Parser<UCharInputIterator>::ParseClassDeclaration() {
  return nullptr;
}


template <typename UCharInputIterator>
ir::Node* Parser<UCharInputIterator>::ParseClassFields() {
  return nullptr;
}


// Parse expression.
// like '(' expression ')' or assignment expression.
template <typename UCharInputIterator>
ir::Node* Parser<UCharInputIterator>::ParseExpression(bool has_in, bool has_yield) {
  ENTER(ParseExpression);
  ir::Node* assignment_expr = ParseAssignmentExpression(has_in, has_yield);

  // Parse comma expressions.
  if (Current()->type() == Token::TS_COMMA) {
    Next();
    ir::CommaExprView* comma_expr = New<ir::CommaExprView>({assignment_expr});
    comma_expr->SetInformationForNode(*Current());
    
    while (1) {
      assignment_expr = ParseAssignmentExpression(has_in, has_yield);
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


/**
 * Return true if token type is assignment operator.
 * @param type A token type.
 * @returns True if token type is assignment operator.
 */
bool IsAssignmentOp(Token type) {
  return type == Token::TS_ASSIGN || type == Token::TS_MUL_LET ||
    type == Token::TS_DIV_LET || type == Token::TS_MOD_LET ||
    type == Token::TS_ADD_LET || type == Token::TS_SUB_LET ||
    type == Token::TS_SHIFT_LEFT_LET || type == Token::TS_SHIFT_RIGHT_LET ||
    type == Token::TS_U_SHIFT_RIGHT_LET || type == Token::TS_AND_LET ||
    type == Token::TS_NOR_LET || type == Token::TS_OR_LET ||
    type == Token::TS_XOR_LET;
}


// Parse assignment expression.
// The assignment expression is like below
// 'a = b' or 'a += b'
// And, try parse arrow_function if token started as '(' or '<',
// if parse arrow_function is failed, simply parsed as conditional expression.
template <typename UCharInputIterator>
ir::Node* Parser<UCharInputIterator>::ParseAssignmentExpression(bool has_in, bool has_yield) {
  ENTER(ParseAssignmentExpression);
  ir::Node* node = nullptr;
  if (Current()->type() == Token::TS_LEFT_PAREN ||
      Current()->type() == Token::TS_LESS) {
    typename ParserState::ArrowFunctionScope scope(&parser_state_);
    // First try parse as arrow function.
    bool failed = false;
    try {
      {
        // Enable token recording mode.
        typename ParserState::RecordTokenScope token_scope(&parser_state_);
        token_buffer_.PushBack(Current());

        // parsae an arrow_function_parameters.
        node = ParseArrowFunctionParameters();
      }
      
    } catch (const SyntaxError& e) {
      // If parse failed, try parse as an parenthesized_expression in primary expression,
      // Do nothing.
      failed = true;
    } catch (const ArrowParametersError& a) {
      // If ArrowParameterError thrown, rethrow error because
      // that is a ParseError for the arrow_function_parameters.
      throw a;   
    } catch (const std::exception& e) {
      // Any errors except the SyntaxError and ArrowParametersError are goto here.
      throw e;
    }

    if (!failed) {
      token_buffer_.Clear();
      return ParseArrowFunctionBody(node);
    }
  }

  ir::Node* expr = ParseConditionalExpression(has_in, has_yield);

  if (expr->HasNameView() && Current()->type() == Token::TS_ARROW_GLYPH) {
    return ParseArrowFunction(expr);
  }
  
  // Expression is not an arrow_function.
  TokenInfo *token_info = Current();
  Token type = token_info->type();

  // Check assignment operators.
  if (IsAssignmentOp(type)) {
    Next();

    // Check left hand side expression is valid as an expression.
    // If left hand side expression is like 'func()',
    // that is invalid expression.
    if (expr->IsValidLhs()) {
      ir::Node* rhs = ParseAssignmentExpression(has_in, has_yield);
      ir::Node* result = New<ir::AssignmentView>(type, expr, rhs);
      result->SetInformationForNode(expr);
      return result;
    }
    SYNTAX_ERROR("invalid left hand side expression in 'assignment expression'", Current());
  }
  return expr;
}


template <typename UCharInputIterator>
ir::Node* Parser<UCharInputIterator>::ParseYieldExpression() {
  return nullptr;
}


// Parse conditional expression.
// The conditional expression is like below
// a > 100? b: c
//
// BNF
// conditional_expression
// 	: logical_or_expression
// 	| logical_or_expression '?' assignment_expression ':' assignment_expression
// 	;

// conditional_expression_no_in
// 	: logical_or_expression_no_in
// 	| logical_or_expression_no_in '?' assignment_expression_no_in ':' assignment_expression_no_in
// 	;
template <typename UCharInputIterator>
ir::Node* Parser<UCharInputIterator>::ParseConditionalExpression(bool has_in, bool has_yield) {
  ENTER(ParseConditionalExpression);
  ir::Node* expr = ParseBinaryExpression(has_in, has_yield);
  TokenInfo* ti = Current();
  if (ti->type() == Token::TS_QUESTION_MARK) {
    Next();
    ir::Node* left = ParseAssignmentExpression(has_in, has_yield);
    ti = Current();
    if (ti->type() == Token::TS_COLON) {
      Next();
      ir::Node* right = ParseAssignmentExpression(has_in, has_yield);
      ir::TemaryExprView* temary = New<ir::TemaryExprView>(expr, left, right);
      temary->SetInformationForNode(expr);
      temary->MarkAsInValidLhs();
      return temary;
    }
    SYNTAX_ERROR("SyntaxError unexpected token in 'temary expression'", Current());
  }
  return expr;
}


// multiplicative_expression
// 	: unary_expression
// 	| multiplicative_expression '*' unary_expression
// 	| multiplicative_expression '/' unary_expression
// 	| multiplicative_expression '%' unary_expression
// 	;

// additive_expression
// 	: multiplicative_expression
// 	| additive_expression '+' multiplicative_expression
// 	| additive_expression '-' multiplicative_expression
// 	;

// shift_expression
// 	: additive_expression
// 	| shift_expression SHIFT_LEFT additive_expression
// 	| shift_expression SHIFT_RIGHT additive_expression
// 	| shift_expression U_SHIFT_RIGHT additive_expression
// 	;

// relational_expression
// 	: shift_expression
// 	| relational_expression '<' shift_expression
// 	| relational_expression '>' shift_expression
// 	| relational_expression LESS_EQUAL shift_expression
// 	| relational_expression GRATER_EQUAL shift_expression
// 	| relational_expression INSTANCEOF shift_expression
// 	| relational_expression IN shift_expression
// 	;

// relational_expression_no_in
// 	: shift_expression
// 	| relational_expression_no_in '<' shift_expression
// 	| relational_expression_no_in '>' shift_expression
// 	| relational_expression_no_in LESS_EQUAL shift_expression
// 	| relational_expression_no_in GRATER_EQUAL shift_expression
// 	| relational_expression_no_in INSTANCEOF shift_expression
// 	;

// equality_expression
// 	: relational_expression
// 	| equality_expression EQUAL relational_expression
// 	| equality_expression NOT_EQUAL relational_expression
// 	| equality_expression EQ relational_expression
// 	| equality_expression NOT_EQ relational_expression
// 	;

// equality_expression_no_in
// 	: relational_expression_no_in
// 	| equality_expression_no_in EQUAL relational_expression_no_in
// 	| equality_expression_no_in NOT_EQUAL relational_expression_no_in
// 	| equality_expression_no_in EQ relational_expression_no_in
// 	| equality_expression_no_in NOT_EQ relational_expression_no_in
// 	;

// bitwise_and_expression
// 	: equality_expression
// 	| bitwise_and_expression '&' equality_expression
// 	;

// bitwise_and_expression_no_in
// 	: equality_expression_no_in
// 	| bitwise_and_expression_no_in '&' equality_expression_no_in
// 	;

// bitwise_xor_expression
// 	: bitwise_and_expression
// 	| bitwise_xor_expression '^' bitwise_and_expression
// 	;

// bitwise_xor_expression_no_in
// 	: bitwise_and_expression_no_in
// 	| bitwise_xor_expression_no_in '^' bitwise_and_expression_no_in
// 	;

// bitwise_or_expression
// 	: bitwise_xor_expression
// 	| bitwise_or_expression '|' bitwise_xor_expression
// 	;

// bitwise_or_expression_no_in
// 	: bitwise_xor_expression_no_in
// 	| bitwise_or_expression_no_in '|' bitwise_xor_expression_no_in
// 	;

// logical_and_expression
// 	: bitwise_or_expression
// 	| logical_and_expression LOGICAL_AND bitwise_or_expression
// 	;

// logical_and_expression_no_in
// 	: bitwise_or_expression_no_in
// 	| logical_and_expression_no_in LOGICAL_AND bitwise_or_expression_no_in
// 	;

// logical_or_expression
// 	: logical_and_expression
// 	| logical_or_expression LOGICAL_OR logical_and_expression
// 	;

// logical_or_expression_no_in
// 	: logical_and_expression_no_in
// 	| logical_or_expression_no_in LOGICAL_OR logical_and_expression_no_in
// 	;
template <typename UCharInputIterator>
ir::Node* Parser<UCharInputIterator>::ParseBinaryExpression(bool has_in, bool has_yield) {
  ENTER(ParseBinaryExpression);
  ir::Node* last = nullptr;
  ir::Node* first = nullptr;
  ir::Node* lhs = ParseUnaryExpression();
  ir::Node* expr = nullptr;
  while (1) {
    TokenInfo* token = Current();
    Token type = token->type();
    switch (token->type()) {
      case Token::TS_LOGICAL_AND :
      case Token::TS_LOGICAL_OR :
      case Token::TS_EQUAL :
      case Token::TS_NOT_EQUAL :
      case Token::TS_EQ :
      case Token::TS_NOT_EQ :
      case Token::TS_LESS_EQUAL :
      case Token::TS_GREATER_EQUAL :
      case Token::TS_INSTANCEOF :
      case Token::TS_LESS :
      case Token::TS_GREATER : {
        Next();
        ir::Node* rhs = ParseBinaryExpression(has_in, has_yield);
        if (last == nullptr) {
          expr = New<ir::BinaryExprView>(type, lhs, rhs);
          expr->SetInformationForNode(lhs);
          first = expr;
        } else {
          expr = New<ir::BinaryExprView>(type, last, rhs);
          expr->SetInformationForNode(last);
        }
        expr->MarkAsInValidLhs();
        last = expr;
      }
        break;

      case Token::TS_IN : {
        if (has_in) {
          Next();
          ir::Node* rhs = ParseBinaryExpression(has_in, has_yield);
          if (last == nullptr) {
            expr = New<ir::BinaryExprView>(type, lhs, rhs);
            expr->SetInformationForNode(lhs);
            first = expr;
          } else {
            expr = New<ir::BinaryExprView>(type, last, rhs);
            expr->SetInformationForNode(last);
          }
          expr->MarkAsInValidLhs();
          last = expr;
        } else {
          return (first == nullptr)? lhs : expr;
        }
      }
        break;
        
      case Token::TS_BIT_OR :
      case Token::TS_BIT_XOR :
      case Token::TS_BIT_AND :
      case Token::TS_PLUS :
      case Token::TS_MINUS :
      case Token::TS_MUL :
      case Token::TS_DIV :
      case Token::TS_MOD :
      case Token::TS_SHIFT_LEFT :
      case Token::TS_SHIFT_RIGHT :
      case Token::TS_U_SHIFT_RIGHT :{
        Next();
        ir::Node* rhs = ParseBinaryExpression(has_in, has_yield);
        if (last == nullptr) {
          expr = New<ir::BinaryExprView>(type, lhs, rhs);
          expr->SetInformationForNode(lhs);
          first = expr;
        } else {
          expr = New<ir::BinaryExprView>(type, last, rhs);
          expr->SetInformationForNode(last);
        }
        expr->MarkAsInValidLhs();
        last = expr;
      }
        break;
                                                                                                                                
      default :
        return (expr == nullptr)? lhs : expr;
    }
  }
}


template <typename UCharInputIterator>
ir::Node* Parser<UCharInputIterator>::ParseUnaryExpression() {
  ENTER(ParseUnaryExpression);
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
      ir::Node* node = ParseUnaryExpression();
      ir::Node* ret = New<ir::UnaryExprView>(type, node);
      ret->SetInformationForNode(node);
      return ret;
    }
    case Token::TS_LESS: {
      ir::Node* type_arguments = ParseTypeArguments();
      ir::Node* expr = ParseUnaryExpression();
      ir::Node* ret = New<ir::CastView>(type_arguments, expr);
      ret->SetInformationForNode(type_arguments);
      return ret;
    }
    default:
      return ParsePostfixExpression();
  }
}


template <typename UCharInputIterator>
ir::Node* Parser<UCharInputIterator>::ParsePostfixExpression() {
  ENTER(ParsePostfixExpression);
  ir::Node* node = ParseLeftHandSideExpression();
  if (Current()->type() == Token::TS_INCREMENT ||
      Current()->type() == Token::TS_DECREMENT) {
    ir::Node* ret = New<ir::PostfixView>(node, Current()->type());
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
ir::Node* Parser<UCharInputIterator>::ParseLeftHandSideExpression() {
  ENTER(ParseLeftHandSideExpression);
  if (Current()->type() == Token::TS_NEW) {
    return ParseMemberExpression();
  }
  return ParseCallExpression();
}


// type_parameters
//   : '<' type_parameter_list '>'
//   ;
// type_parameter_list
//   : type_parameter 
//   | type_parameter_list , type_parameter
//   ;
// type_parameter
//   : identifier constraint__opt
//   ;
// constraint
//   : 'extends' type
//   ;

template <typename UCharInputIterator>
ir::Node* Parser<UCharInputIterator>::ParseTypeParameters() {
  if (Current()->type() == Token::TS_LESS) {
    ir::TypeParametersView* type_params = New<ir::TypeParametersView>();
    type_params->SetInformationForNode(Current());
    Next();
    bool found = false;
    while (1) {
      if (Current()->type() == Token::TS_IDENTIFIER) {
        found = true;
        ir::Node* name = ParsePrimaryExpression();
        if (Current()->type() == Token::TS_EXTENDS) {
          Next();
          ir::Node* type_constraints = New<ir::TypeConstraintsView>(name, ParsePrimaryExpression());
          type_constraints->SetInformationForNode(name);
          type_params->InsertLast(type_constraints);
        } else {
          type_params->InsertLast(name);
        }
      } else if (Current()->type() == Token::TS_GREATER) {
        if (!found) {
          SYNTAX_ERROR("SyntaxError need type parameter", Current());
        }
        Next();
        return type_params;
      } else if (Current()->type() == Token::TS_COMMA) {
        Next();
      } else {
        SYNTAX_ERROR("SyntaxError unexpected token", Current());
      }
    }
  }
  SYNTAX_ERROR("SyntaxError '<' expected", Current());
}


// type
//   : predefined_type 
//   | type_reference 
//   | type_query 
//   | type_literal
//   ;
//
// pre_defined_type 
//   : 'any'
//   | 'number'
//   | 'boolean'
//   | 'string'
//   | 'void'
//   ;
// type_literal 
//   : object_type 
//   | array_type 
//   | function_type 
//   | constructor_type
//   ;
template <typename UCharInputIterator>
ir::Node* Parser<UCharInputIterator>::ParseTypeExpression() {
  ENTER(ParseTypeExpression);
  if (Current()->type() == Token::TS_IDENTIFIER) {
    return ParseArrayType(ParseReferencedType());
  } else if (Current()->type() == Token::TS_LEFT_PAREN) {
    ir::Node* types = ParseParameterList(false);
    if (Current()->type() == Token::TS_ARROW_GLYPH) {
      Next();
      ir::Node* ret_type = ParseTypeExpression();
      ir::Node* ft = New<ir::FunctionTypeExprView>(types, ret_type);
      ft->SetInformationForNode(types);
      return ParseArrayType(ft);
    }
    SYNTAX_ERROR("SyntaxError '=>' expected", Current());
  } else if (Current()->type() == Token::TS_LEFT_BRACE) {
    return ParseArrayType(ParseObjectTypeExpression());
  } else if (Current()->type() == Token::TS_TYPEOF) {
    return ParseArrayType(ParseTypeQueryExpression());
  }
  SYNTAX_ERROR("SyntaxError unexpected token", Current());
}


// type_reference 
//   : type_name [no line_terminator here] type_arguments__opt
//   ;
// type_name
//   : identifier 
//   | module_name '.' identifier
//   ;
// module_name
//   : identifier
//   | module_name '.' identifier
//   ;
template <typename UCharInputIterator>
ir::Node* Parser<UCharInputIterator>::ParseReferencedType() {
  ENTER(ParseReferencedType);
  if (Current()->type() == Token::TS_IDENTIFIER) {
    ir::Node* node = ParsePrimaryExpression();
    if (Current()->type() == Token::TS_DOT) {
      ir::GetPropView* get_prop_view = New<ir::GetPropView>();
      get_prop_view->SetInformationForNode(node);
      while (1) {
        if (Current()->type() == Token::TS_DOT) {
          Next();
          get_prop_view->InsertLast(ParsePrimaryExpression());
        } else {
          break;
        }
      }
      node = get_prop_view;
    }
    if (!Current()->has_line_break_before_next() && Current()->type() == Token::TS_LESS) {
      ir::Node* type_parameter = ParseTypeArguments();
      ir::Node* ret = New<ir::GenericTypeExprView>(node, type_parameter);
      ret->SetInformationForNode(node);
      return ret;
    }

    ir::Node* ret = New<ir::SimpleTypeExprView>(node);
    ret->SetInformationForNode(node);
    return ret;
  }

  SYNTAX_ERROR("SyntaxError identifier expected", Current());
}


// type_query
//   : 'typeof' type_query_expression
//   ;
// type_query_expression: 
//   : identifier
//   | type_query_expression '.' identifier_name
//   ;
template <typename UCharInputIterator>
ir::Node* Parser<UCharInputIterator>::ParseTypeQueryExpression() {
  if (Current()->type() == Token::TS_TYPEOF) {
    Next();
    if (Current()->type() == Token::TS_IDENTIFIER) {
      ir::Node* name = ParsePrimaryExpression();
      if (Current()->type() == Token::TS_DOT) {
        Next();
        ir::GetPropView* getprop = New<ir::GetPropView>();
        getprop->SetInformationForNode(name);
        ir::GetPropView* root = getprop;
        getprop->set_target(name);
        while (1) {
          if (Current()->type() == Token::TS_IDENTIFIER) {
            getprop->set_prop(ParsePrimaryExpression());
            if (Current()->type() == Token::TS_DOT) {
              ir::GetPropView* getprop_tmp = New<ir::GetPropView>();
              getprop_tmp->SetInformationForNode(name);
              getprop->set_target(getprop_tmp);
              getprop_tmp = getprop;
            } else {
              break;
            }
          } else {
            break;
          }
        }
        ir::Node* ret = New<ir::TypeQueryView>(root);
        ret->SetInformationForNode(root);
        return ret;
      }
      ir::Node* ret = New<ir::TypeQueryView>(name);
      ret->SetInformationForNode(name);
      return ret;
    }
    SYNTAX_ERROR("SyntaxError identifier expected", Current());
  }
  SYNTAX_ERROR("SyntaxError 'typeof' expected", Current());
}


// type_arguments 
//   : '<' type_argument_list '>'
//   ;
// type_argument_list
//   : type_argument 
//   | type_argument_list ',' type_argument
//   ;
// type_argument
//   : type
//   ;
template <typename UCharInputIterator>
ir::Node* Parser<UCharInputIterator>::ParseTypeArguments() {
  if (Current()->type() == Token::TS_LESS) {
    ir::TypeArgumentsView* type_arguments = New<ir::TypeArgumentsView>();
    type_arguments->SetInformationForNode(Current());
    Next();
    while (1) {
      type_arguments->InsertLast(ParseTypeExpression());
      if (Current()->type() == Token::TS_COMMA) {
        Next();
      } else if (Current()->type() == Token::TS_GREATER) {
        Next();
        return type_arguments;
      } else {
        SYNTAX_ERROR("SyntaxError '>' or ',' expected", Current());
      }
    }
  }
  SYNTAX_ERROR("SyntaxError '<' expected", Current());
}


// array_type
//   : element_type [no LineTerminator here] '[' ']'
//   ;
// element_type
//   : pre_defined_type
//   | type_reference
//   | type_query
//   | object_type
//   | array_type
//   ;
template <typename UCharInputIterator>
ir::Node* Parser<UCharInputIterator>::ParseArrayType(ir::Node* type_expr) {
  if (Current()->type() == Token::TS_LEFT_BRACKET) {
    Next();
    if (Current()->type() == Token::TS_RIGHT_BRACKET) {
      ir::Node* array_type = New<ir::ArrayTypeExprView>(type_expr);
      array_type->SetInformationForNode(Current());
      Next();
      return ParseArrayType(array_type);
    }
    SYNTAX_ERROR("SyntaxError ']' expected", Current());
  }
  return type_expr;
}


// object_type
//   : '{' type_body__opt '}'
//   ;
// type_body
//   : type_member_list ';'__opt
//   ;
// type_member_list
//   : type_member
//   | type_member_list ';' type_member
//   ;
template <typename UCharInputIterator>
ir::Node* Parser<UCharInputIterator>::ParseObjectTypeExpression() {
  ENTER(ParseObjectTypeExpression);
  if (Current()->type() == Token::TS_LEFT_BRACE) {
    Next();
    ir::ObjectTypeExprView* object_type = New<ir::ObjectTypeExprView>();
    object_type->SetInformationForNode(Current());
    while (Current()->type() != Token::TS_RIGHT_BRACE) {
      ir::Node* property = ParseObjectTypeElement();
      object_type->InsertLast(property);
    }
    return object_type;
  }
  SYNTAX_ERROR("SyntaxError '{' expected", Current());
}


// type_member
//   : property_signature
//   | call_signature
//   | construct_signature
//   | index_signature
//   | method_signature
//   ;
template <typename UCharInputIterator>
ir::Node* Parser<UCharInputIterator>::ParseObjectTypeElement() {
  ENTER(ParseObjectTypeElement);
  if (Current()->type() == Token::TS_NEW) {
    Next();
    ir::Node* call_sig = ParseCallSignature(false);
    ir::Node* ctor_sig = New<ir::ConstructSignatureView>(call_sig);
    ctor_sig->SetInformationForNode(call_sig);
  } else if (Current()->type() == Token::TS_LEFT_PAREN) {
    return ParseCallSignature(false);
  } else if (Current()->type() == Token::TS_LEFT_BRACKET) {
    Next();
    if (Current()->type() == Token::TS_IDENTIFIER) {
      ir::Node* name = ParseLiteral();
      if (Current()->type() == Token::TS_COLON) {
        Next();
        if (Current()->type() == Token::TS_IDENTIFIER &&
            (Current()->value() == "string" || Current()->value() == "number")) {
          ir::Node* ret = New<ir::AccessorTypeExprView>(name, ParsePrimaryExpression());
          ret->SetInformationForNode(Current());
          Next();
          return ret;
        } else {
          SYNTAX_ERROR("SyntaxError type name in indexSignature only allowed one of 'string' or 'number'", Current());
        }
      }
      SYNTAX_ERROR("SyntaxError ':' expected", Current());
    }
    SYNTAX_ERROR("SyntaxError identifier expected", Current());
  } else {
    bool optional = false;
    ir::Node* key = ParseObjectKey();
    if (Current()->type() == Token::TS_QUESTION_MARK) {
      optional = true;
      Next();
    }
    if (Current()->type() == Token::TS_LEFT_PAREN) {
      if (key->HasNameView()) {
        SYNTAX_ERROR_POS("SyntaxError invalid method name", key->source_position());
      }
      ir::Node* call_sig = ParseCallSignature(false);
      ir::Node* ret = New<ir::MethodSignatureView>(optional, key, call_sig);
      ret->SetInformationForNode(key);
      return ret;
    } else if (Current()->type() == Token::TS_COLON) {
      Next();
      ir::Node* type_expr = ParseTypeExpression();
      ir::Node* ret = New<ir::PropertySignatureView>(optional, key, type_expr);
      ret->SetInformationForNode(type_expr);
      return ret;
    } else if (Current()->type() == Token::TS_LEFT_BRACKET) {
      if (optional) {
        SYNTAX_ERROR("SyntaxError unexpected '?'", Current());
      }
      return ParseTypeExpression();
    }
    ir::Node* ret = New<ir::PropertySignatureView>(optional, key, nullptr);
    ret->SetInformationForNode(key);
    return ret;
  }
  SYNTAX_ERROR("SyntaxError unexpected token", Current());
}


// Parse call expression.
// call_expression
// 	: member_expression arguments
//  | super arguments
//  | super '.' identifier arguments
// 	| call_expression arguments
// 	| call_expression '[' expression ']'
// 	| call_expression '.' IDENTIFIER
template <typename UCharInputIterator>
ir::Node* Parser<UCharInputIterator>::ParseCallExpression() {
  ENTER(ParseCallExpression);
  ir::Node* target;
  if (Current()->type() == Token::TS_SUPER) {
    target = New<ir::SuperView>();
    target->SetInformationForNode(Current());
    Next();
    if (Current()->type() == Token::TS_DOT) {
      Next();
      ir::Node* literal = ParseLiteral();
      if (literal->HasNameView()) {
        target = New<ir::GetPropView>(target, literal);
        target->SetInformationForNode(literal);
      } else {
        SYNTAX_ERROR_POS("SyntaxError unexpected token", literal->source_position());
      }
    }
  } else {
    target = ParseMemberExpression();
  }

  ir::Node* type_arguments = nullptr;

  if (Current()->type() == Token::TS_LESS) {
    type_arguments = ParseTypeArguments();
  }
  
  if (Current()->type() == Token::TS_LEFT_PAREN) {
    ir::Node* args = ParseArguments();
    ir::Node* call = New<ir::CallView>(target, args, type_arguments);
    call->SetInformationForNode(target);
    type_arguments = nullptr;
    while (1) {
      switch (Current()->type()) {
        case Token::TS_LESS: {
          type_arguments = ParseTypeArguments();
        }
        case Token::TS_LEFT_PAREN: {
          ir::Node* args = ParseArguments();
          call = New<ir::CallView>(call, args, type_arguments);
          call->SetInformationForNode(args);
          type_arguments = nullptr;
          break;
        }
        case Token::TS_LEFT_BRACE:
        case Token::TS_DOT:
          if (type_arguments != nullptr) {
            SYNTAX_ERROR_POS("SyntaxError unexpected token", type_arguments->source_position());
          }
          call = ParseGetPropOrElem(call);
          break;
        default:
          return call;
      }
    }
  }
  return target;
}


// Parse arguments and arguments-list
// arguments
// 	: '(' ')'
// 	| '(' argument_list ')'
// 	;

// argument_list
// 	: assignment_expression
// 	| argument_list ',' assignment_expression
// 	;
template <typename UCharInputIterator>
ir::Node* Parser<UCharInputIterator>::ParseArguments() {
  ENTER(ParseArguments);
  if (Current()->type() == Token::TS_LEFT_PAREN) {
    ir::CallArgsView* args = New<ir::CallArgsView>();
    args->SetInformationForNode(Current());
    Next();
    if (Current()->type() == Token::TS_RIGHT_PAREN) {
      Next();
      return args;
    }
    while (1) {
      args->InsertLast(ParseAssignmentExpression(true, false));
      if (Current()->type() == Token::TS_COMMA) {
        continue;
      } else if (Current()->type() == Token::TS_RIGHT_PAREN) {
        Next();
        return args;
      }
      SYNTAX_ERROR("SyntaxError unexpected token in 'arguments'", Current());
    }
  }
  return nullptr;
}


// Parse member-expression
// bnf: member_expression
// 	    : primary_expression
// 	    | function_expression
// 	    | member_expression '[' expression ']'
// 	    | member_expression '.' IDENTIFIER
// 	    | NEW member_expression arguments
// 	    ;
template <typename UCharInputIterator>
ir::Node* Parser<UCharInputIterator>::ParseMemberExpression() {
  ENTER(ParseMemberExpression);
  // Not advance scanner.
  TokenInfo* token_info = Current();
  ir::Node* node;
  if (token_info->type() ==  Token::TS_NEW) {
    // Parse new Foo() expression.
    Next();
    ir::Node* member = ParseMemberExpression();

    ir::Node* type_arguments = nullptr;
    if (Current()->type() == Token::TS_LESS) {
      type_arguments = ParseTypeArguments();
    }

    // New expression can omit parens.
    // If paren exists, continue parsing.
    if (Current()->type() == Token::TS_LEFT_PAREN) {
      ir::Node* args = ParseArguments();
      node = New<ir::NewCallView>(member, args, type_arguments);
      node->SetInformationForNode(member);
      return ParseGetPropOrElem(node);
    } else {
      // Parens are not exists.
      // Immediate return.
      ir::Node* ret = New<ir::NewCallView>(member, nullptr, type_arguments);
      ret->SetInformationForNode(member);
      return ret;
    }

  } else {
    if (token_info->type() == Token::TS_FUNCTION) {
      // Parse function.
      node = ParseFunction();
    } else {
      node = ParsePrimaryExpression();
    }
    return ParseGetPropOrElem(node);
  }
}


// Parse member expression suffix.
// Like 'new foo.bar.baz()', 'new foo["bar"]', '(function(){return {a:1}}).a'
template <typename UCharInputIterator>
ir::Node* Parser<UCharInputIterator>::ParseGetPropOrElem(ir::Node* node) {
  ENTER(ParseGetPropOrElem);
  
  switch (Current()->type()) {
    case Token::TS_LEFT_BRACKET: {
      // [...] expression.
      Next();
      ir::Node* expr = ParseExpression(true, false);
      ir::Node* result = New<ir::GetElemView>(node, expr);
      result->SetInformationForNode(node);
      if (Current()->type() != Token::TS_RIGHT_BRACKET) {
        SYNTAX_ERROR("Unexpected token.", Current());
      }
      return result;
    }
    case Token::TS_DOT: {
      // a.b.c expression.
      Next();
      ir::Node* expr = ParseMemberExpression();
      if (!expr->HasNameView() && !expr->HasKeywordLiteralView() && !expr->HasGetPropView() && !expr->HasGetElemView()) {
        SYNTAX_ERROR_POS("SyntaxError identifier expected", expr->source_position());
      }
      ir::Node* ret = New<ir::GetPropView>(node, expr);
      ret->SetInformationForNode(node);
      return ret;
    }
    default:
      return node;
  }
}


// Parse primary-expression.
// bnf: primary_expression
//      : THIS
//      | IDENTIFIER
//      | literal
//      | array_literal
//      | object_literal
//      | '(' expression ')'
//      ;
template <typename UCharInputIterator>
ir::Node* Parser<UCharInputIterator>::ParsePrimaryExpression() {
  ENTER(ParsePrimaryExpression);

  // Allow regular expression in this context.
  AllowRegularExpr();
  TokenInfo* maybe_regexp = scanner_->CheckRegularExpression();
  if (maybe_regexp) {
    current_token_info_ = maybe_regexp;
  }
  
  // Not advance scanner.
  TokenInfo* token_info = Current();
  switch (token_info->type()) {
    case Token::TS_IDENTIFIER: {
      DisallowRegularExpr();
      // parse an identifier.
      ir::NameView* name = New<ir::NameView>(token_info->value());
      name->SetInformationForNode(Current());
      Next();
      return name;
    }
    case Token::TS_THIS: {
      DisallowRegularExpr();
      // parse a this.
      ir::Node* this_view = New<ir::ThisView>();
      this_view->SetInformationForNode(Current());
      Next();
      return this_view;
    }
    case Token::TS_LEFT_BRACE:
      DisallowRegularExpr();
      // parse an object literal.
      return ParseObjectLiteral();
    case Token::TS_LEFT_BRACKET:
      DisallowRegularExpr();
      // parse an array literal.
      return ParseArrayLiteral();
    case Token::TS_LEFT_PAREN: {
      DisallowRegularExpr();
      Next();
      ir::Node* node = ParseExpression(true, false);
      if (Current()->type() == Token::TS_RIGHT_PAREN) {
        Next();
        return node;
      }
      SYNTAX_ERROR("SyntaxError ')' expected", Current());
    }
    default:
      // parse a literal.
      return ParseLiteral();
  }
}


template <typename UCharInputIterator>
ir::Node* Parser<UCharInputIterator>::ParseObjectLiteral() {
  ENTER(ParseObjectLiteral);
  if (Current()->type() == Token::TS_LEFT_BRACE) {
    ir::ObjectLiteralView* object_literal = New<ir::ObjectLiteralView>();
    object_literal->SetInformationForNode(Current());
    Next();
    while (1) {
      ir::Node* key = ParseObjectKey();
      if (Current()->type() != Token::TS_COLON) {
        SYNTAX_ERROR("SyntaxError ':' expected", Current());
      }
      Next();
      ir::Node* value = ParseAssignmentExpression(true, false);
      ir::ObjectElementView* element = New<ir::ObjectElementView>(key, value);
      element->SetInformationForNode(key);
      object_literal->InsertLast(element);
      if (Current()->type() == Token::TS_COMMA) {
        Next();
      } else if (Current()->type() == Token::TS_RIGHT_BRACE) {
        Next();
        break;
      } else {
        SYNTAX_ERROR("SyntaxError expected ',' or '}'", Current());
      }
    }
    return object_literal;
  }
  SYNTAX_ERROR("Unexpected token.", Current());
}


template <typename UCharInputIterator>
ir::Node* Parser<UCharInputIterator>::ParseObjectKey() {
  ENTER(ParseObjectKey);
  ir::Node* ret = nullptr;
  YATSC_SCOPED([&]{
    if (ret != nullptr) {
      ret->SetInformationForNode(Current());
    }
    this->Next();
  });
  if (Current()->type() == Token::TS_IDENTIFIER) {
    return ret = New<ir::NameView>(Current()->value());
  } else if (Current()->type() == Token::TS_STRING_LITERAL) {
    return ret = New<ir::StringView>(Current()->value());
  } else if (Current()->type() == Token::TS_NUMERIC_LITERAL) {
    return ret = New<ir::NumberView>(Current()->value());
  } else {
    SYNTAX_ERROR("Invalid ObjectLiteral key.", Current());
  }
}


template <typename UCharInputIterator>
ir::Node* Parser<UCharInputIterator>::ParseArrayLiteral() {
  ENTER(ParseArrayLiteral);
  if (Current()->type() == Token::TS_LEFT_BRACKET) {
    ir::ArrayLiteralView* array_literal = New<ir::ArrayLiteralView>();
    array_literal->SetInformationForNode(Current());
    Next();
    while (1) {
      ir::Node* expr = ParseAssignmentExpression(true, false);
      array_literal->InsertLast(expr);
      if (Current()->type() == Token::TS_COMMA) {
        Next();
      } else if (Current()->type() == Token::TS_RIGHT_BRACKET) {
        Next();
        break;
      } else {
        SYNTAX_ERROR("SyntaxError unexpected token in 'array literal'", Current());
      }
    }
    return array_literal;
  }
  SYNTAX_ERROR("Unexpected token.", Current());
}


template <typename UCharInputIterator>
ir::Node* Parser<UCharInputIterator>::ParseLiteral() {
  ENTER(ParseLiteral);
  
  ir::Node* ret = nullptr;
  YATSC_SCOPED([&]{
    if (ret != nullptr) {
      ret->SetInformationForNode(this->Current());
    }
    DisallowRegularExpr();
    this->Next();
  })
  switch(Current()->type()) {
    case Token::TS_TRUE:
      // true value
      return ret = New<ir::TrueView>();
    case Token::TS_FALSE:
      // false value
      return ret = New<ir::FalseView>();
    case Token::TS_STRING_LITERAL:
      // 'aaaa' value
      return ret = New<ir::StringView>(Current()->value());
    case Token::TS_NUMERIC_LITERAL:
      // 1234.5 value
      return ret = New<ir::NumberView>(Current()->value());
    case Token::TS_REGULAR_EXPR:
      // /aaaaa/ value
      return ret = New<ir::RegularExprView>(Current()->value());
    case Token::TS_UNDEFINED:
      // undefined value
      return ret = New<ir::UndefinedView>();
    case Token::TS_NULL:
      // null value
      return ret = New<ir::NullView>();
    case Token::TS_NAN:
      // NaN value
      return ret = New<ir::NaNView>();
    default:
      // Any other token cause SyntaxError.
      SYNTAX_ERROR("SyntaxError literal expected", Current());
  }
}


template <typename UCharInputIterator>
ir::Node* Parser<UCharInputIterator>::ParseParameterList(bool accesslevel_allowed) {
  ENTER(ParseParameterList);
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
  ENTER(ParseParameter);
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
ir::Node* Parser<UCharInputIterator>::ParseFunction() {
  ENTER(ParseFunction);
  if (Current()->type() == Token::TS_FUNCTION) {
    TokenInfo token = (*Current());
    Next();
    ir::Node* name = nullptr;
    if (Current()->type() == Token::TS_IDENTIFIER) {
      name = ParseLiteral();
    }
    ir::Node* call_signature = ParseCallSignature(false);
    ir::Node* body = ParseFunctionBody();
    ir::Node* ret = New<ir::FunctionView>(name, call_signature, body);
    ret->SetInformationForNode(&token);
    return ret;
  }
  SYNTAX_ERROR("SyntaxError 'function' expected", Current());
}


template <typename UCharInputIterator>
ir::Node* Parser<UCharInputIterator>::ParseCallSignature(bool accesslevel_allowed) {
  ENTER(ParseCallSignature);
  ir::Node* type_parameters = nullptr;
  if (Current()->type() == Token::TS_LESS) {
    type_parameters = ParseTypeParameters();
  }
  if (Current()->type() == Token::TS_LEFT_PAREN) {
    TokenInfo token = (*Current());
    ir::Node* parameter_list = ParseParameterList(accesslevel_allowed);
    ir::Node* return_type = nullptr;
    if (Current()->type() == Token::TS_COLON) {
      Next();
      return_type = ParseTypeExpression();
    }
    ir::Node* ret = New<ir::CallSignatureView>(parameter_list, return_type, type_parameters);
    ret->SetInformationForNode(&token);
    return ret;
  }
  SYNTAX_ERROR("SyntaxError expected '('", Current());
}


template <typename UCharInputIterator>
ir::Node* Parser<UCharInputIterator>::ParseArrowFunction(ir::Node* identifier) {
  ir::Node* call_sig = ParseArrowFunctionParameters(identifier);
  return ParseArrowFunctionBody(call_sig);
}


template <typename UCharInputIterator>
ir::Node* Parser<UCharInputIterator>::ParseArrowFunctionParameters(ir::Node* identifier) {
  ENTER(ParseArrowFunction);

  ir::Node* call_sig = nullptr;
  
  if (identifier != nullptr) {
    call_sig = New<ir::CallSignatureView>(identifier, nullptr, nullptr);
    call_sig->SetInformationForNode(identifier);
  } else {  
    call_sig = ParseCallSignature(false);
  }
  if (Current()->type() != Token::TS_ARROW_GLYPH) {
    SYNTAX_ERROR("SyntaxError '=>' expected", Current());
  }
  Next();
  return call_sig;
}


template <typename UCharInputIterator>
ir::Node* Parser<UCharInputIterator>::ParseArrowFunctionBody(ir::Node* call_sig) {
  ir::Node* body;
  if (Current()->type() == Token::TS_LEFT_BRACE) {
    Next();
    body = ParseFunctionBody();
  } else {
    body = ParseAssignmentExpression(true, false);
  }
  ir::Node* ret = New<ir::ArrowFunctionView>(call_sig, body);
  ret->SetInformationForNode(call_sig);
  return ret;
}


template <typename UCharInputIterator>
ir::Node* Parser<UCharInputIterator>::ParseFunctionBody() {
  ENTER(ParseFunctionBody);
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


template <typename UCharInputIterator>
ir::Node* Parser<UCharInputIterator>::ParseFormalParameterList() {
  return nullptr;
}
}

#undef ENTER
#undef SYNTAX_ERROR
