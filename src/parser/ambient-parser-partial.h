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

template <typename UCharInputIterator>
ParseResult Parser<UCharInputIterator>::ParseDeclarationModule() {
  LOG_PHASE(ParseDeclarationModule);
  auto ret = New<ir::FileScopeView>(current_scope());
  ret->SetInformationForNode(Current());

  bool success = true;
  
  while (1) {
    Handle<ir::ExportView> export_view;
    if (Current()->type() == Token::TS_EXPORT) {
      TokenInfo info = *Current();
      Next();
      if (Current()->type() == Token::TS_ASSIGN) {
        Next();
        auto assignment_expr_result = ParseAssignmentExpression(true, false);
        SKIP_TOKEN_OR(assignment_expr_result, success, Token::LINE_TERMINATOR) {
          ret->InsertLast(CreateExportView(assignment_expr_result.node(), ir::Node::Null(), &info, true));
        }
        continue;
      } else {
        export_view = New<ir::ExportView>();
        export_view->SetInformationForNode(&info);
      }
    }
    
    ParseResult parse_result;
    
    switch (Current()->type()) {
      case Token::TS_INTERFACE:
        parse_result = ParseInterfaceDeclaration();
        break;
      case Token::TS_IMPORT:
        parse_result = ParseImportDeclaration();
        break;
      default:
        parse_result = ParseAmbientDeclaration(true);
        break;
    }
    
    SKIP_TOKEN_OR(parse_result, success, Token::LINE_TERMINATOR) {
      if (IsLineTermination()) {
        ConsumeLineTerminator();
      }
    
      if (export_view) {
        export_view->set_export_clause(parse_result.node());
        parse_result = Success(export_view);
      }

      ret->InsertLast(parse_result.node());
    }
    
    if (Current()->type() == Token::END_OF_INPUT) {
      break;
    }
  }
  return Success(ret);
}

template <typename UCharInputInterator>
ParseResult Parser<UCharInputInterator>::ParseAmbientDeclaration(bool module_allowed) {
  LOG_PHASE(ParseAmbientDeclaration);
  if (Current()->type() == Token::TS_IDENTIFIER &&
      Current()->value()->Equals("declare")) {
    TokenInfo info = *Current();
    Next();
    switch (Current()->type()) {
      case Token::TS_VAR:
        return ParseAmbientVariableDeclaration(&info);
      case Token::TS_FUNCTION:
        return ParseAmbientFunctionDeclaration(&info);
      case Token::TS_CLASS:
        return ParseAmbientClassDeclaration(&info);
      case Token::TS_ENUM:
        return ParseAmbientEnumDeclaration(&info);
      default:
        if (Current()->type() == Token::TS_IDENTIFIER &&
            Current()->value()->Equals("module")) {
          if (!module_allowed) {
            SYNTAX_ERROR("ambient module declaration not allowed here.", Current());
          }
          return ParseAmbientModuleDeclaration(&info);
        }
        SYNTAX_ERROR("unexpected token.", Current());
    }
  }
  SYNTAX_ERROR("'declare' expected.", Current());
}


template <typename UCharInputInterator>
ParseResult Parser<UCharInputInterator>::ParseAmbientVariableDeclaration(TokenInfo* info) {
  LOG_PHASE(ParseAmbientVariableDeclaration);
  if (Current()->type() == Token::TS_VAR) {
    Next();
    if (Current()->type() == Token::TS_IDENTIFIER) {
      auto identifier_result = ParseIdentifier();
      CHECK_AST(identifier_result);
      ParseResult type_annotation_result;
      if (Current()->type() == Token::TS_COLON) {
        Next();
        type_annotation_result = ParseTypeExpression();
        CHECK_AST(type_annotation_result);
      }
      auto ret = New<ir::AmbientVariableView>(identifier_result.node(), type_annotation_result.node());
      ret->SetInformationForNode(info);
      return Success(ret);
    }
    SYNTAX_ERROR("'identifier' expected.", Current());
  }
  SYNTAX_ERROR("'var' expected.", Current());
}


template <typename UCharInputInterator>
ParseResult Parser<UCharInputInterator>::ParseAmbientFunctionDeclaration(TokenInfo* info) {
  LOG_PHASE(ParseAmbientFunctionDeclaration);
  bool generator = false;

  if (Current()->type() == Token::TS_FUNCTION) {
    Next();
    if (Current()->type() == Token::TS_IDENTIFIER) {
      auto identifier_result = ParseIdentifier();
      CHECK_AST(identifier_result);
      if (Current()->type() == Token::TS_MUL) {
        generator = true;
        Next();
      }
      auto call_sig_result = ParseCallSignature(false, false);
      CHECK_AST(call_sig_result);
      auto ret = New<ir::AmbientFunctionDeclarationView>(generator, identifier_result.node(), call_sig_result.node());
      ret->SetInformationForNode(info);
      return Success(ret);
    }
  }
  SYNTAX_ERROR("'function' expected.", Current());
}


template <typename UCharInputInterator>
ParseResult Parser<UCharInputInterator>::ParseAmbientClassDeclaration(TokenInfo* info) {
  LOG_PHASE(ParseAmbientClassDeclaration);
  if (Current()->type() == Token::TS_CLASS) {
    Next();
    auto identifier_result = ParseIdentifier();
    CHECK_AST(identifier_result);
    ParseResult type_parameters_result;
    if (Current()->type() == Token::TS_LESS) {
     type_parameters_result = ParseTypeParameters();
     CHECK_AST(type_parameters_result);
    }
    auto class_bases_result = ParseClassBases();
    CHECK_AST(class_bases_result);
    if (Current()->type() == Token::TS_LEFT_BRACE) {
      auto ambient_class_body_result = ParseAmbientClassBody();
      CHECK_AST(ambient_class_body_result);
      auto ret = New<ir::AmbientClassDeclarationView>(identifier_result.node(),
                                                      type_parameters_result.node(),
                                                      class_bases_result.node(),
                                                      ambient_class_body_result.node());
      ret->SetInformationForNode(info);
      return Success(ret);
    }
    SYNTAX_ERROR("'{' expected.", Current());
  }
  SYNTAX_ERROR("'class' expected.", Current());
}


template <typename UCharInputInterator>
ParseResult Parser<UCharInputInterator>::ParseAmbientClassBody() {
  LOG_PHASE(ParseAmbientClassBody);
  if (Current()->type() == Token::TS_LEFT_BRACE) {
    auto body = New<ir::AmbientClassFieldsView>();
    body->SetInformationForNode(Current());
    Next();

    bool constructor_parsed = false;
    bool success = true;

    while (1) {
      if (Current()->type() == Token::TS_RIGHT_BRACE) {
        Next();
        return Success(body);
      } else {
        auto ambient_class_element_result = ParseAmbientClassElement();
        SKIP_TOKEN_OR(ambient_class_element_result, success, Token::LINE_TERMINATOR) {
          body->InsertLast(ambient_class_element_result.node());
        }
        if (IsLineTermination()) {
          ConsumeLineTerminator();
        } else if (Current()->type() != Token::TS_RIGHT_BRACE &&
                   Prev()->type() != Token::TS_RIGHT_BRACE) {
          SYNTAX_ERROR("';' expected", Prev());
        }
      }
    }
  }
  SYNTAX_ERROR("'{' expected.", Current());
}


template <typename UCharInputInterator>
ParseResult Parser<UCharInputInterator>::ParseAmbientClassElement() {
  LOG_PHASE(ParseAmbientClassElement);
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
      return ParseAmbientConstructor(field_modifiers_result.node());
    } else {
      RecordedParserState rps = parser_state();
      Next();
      if (Current()->type() == Token::TS_LEFT_PAREN ||
          Current()->type() == Token::TS_LESS) {
        RestoreParserState(rps);
        return ParseAmbientMemberFunction(field_modifiers_result.node(), &at);
      } else {
        RestoreParserState(rps);
        return ParseAmbientMemberVariable(field_modifiers_result.node());
      }
    }
  } else if (Current()->type() == Token::TS_MUL) {
    Next();
    return ParseAmbientGeneratorMethod(field_modifiers_result.node());
  }
  SYNTAX_ERROR("unexpected token", Current());
}


template <typename UCharInputIterator>
ParseResult Parser<UCharInputIterator>::ParseAmbientConstructor(Handle<ir::Node> mods) {
  LOG_PHASE(ParseAmbientConstructor);
  
  if ((Current()->type() == Token::TS_IDENTIFIER &&
       Current()->value()->Equals("constructor")) ||
      Current()->type() == Token::TS_PUBLIC ||
      Current()->type() == Token::TS_PRIVATE ||
      Current()->type() == Token::TS_PROTECTED) {
    
    if (Current()->type() == Token::TS_IDENTIFIER &&
        Current()->value()->Equals("constructor")) {
      TokenInfo info = *Current();
      Next();
      auto call_sig_result = ParseCallSignature(true, false);
      CHECK_AST(call_sig_result);
      auto ret = New<ir::AmbientConstructorView>(mods, call_sig_result.node());
      ret->SetInformationForNode(mods);
      return Success(ret);
    }
    SYNTAX_ERROR("'constructor' expected", Current());
  }
  SYNTAX_ERROR("unexpected token.", Current());
}


template <typename UCharInputIterator>
ParseResult Parser<UCharInputIterator>::ParseAmbientMemberFunction(Handle<ir::Node> mods, AccessorType* accessor_type) {
  LOG_PHASE(ParseAmbientMemberFunction);
  
  if (Current()->type() == Token::TS_IDENTIFIER ||
      Current()->type() == Token::TS_PUBLIC ||
      Current()->type() == Token::TS_PRIVATE ||
      Current()->type() == Token::TS_PROTECTED) {   
    
    if (Current()->type() == Token::TS_IDENTIFIER) {
      TokenInfo info = *Current();
      auto identifier_result = ParseIdentifier();
      CHECK_AST(identifier_result);
      auto call_sig_result = ParseCallSignature(true, false);
      CHECK_AST(call_sig_result);
      auto ret = New<ir::AmbientMemberFunctionView>(accessor_type->getter,
                                                    accessor_type->setter,
                                                    false,
                                                    mods,
                                                    identifier_result.node(),
                                                    call_sig_result.node());
      ret->SetInformationForNode(mods);
      return Success(ret);
    }
    SYNTAX_ERROR("'identifier' expected", Current());
  }
  SYNTAX_ERROR("unexpected token.", Current());
}


template <typename UCharInputIterator>
ParseResult Parser<UCharInputIterator>::ParseAmbientGeneratorMethod(Handle<ir::Node> mods) {
  LOG_PHASE(ParseAmbientGeneratorMethod);

  if (Current()->type() == Token::TS_MUL) {
    Next();
  } else {
    SYNTAX_ERROR("'*' expected", Current());
  }
  
  if (Current()->type() == Token::TS_IDENTIFIER ||
      Current()->type() == Token::TS_PUBLIC ||
      Current()->type() == Token::TS_PRIVATE ||
      Current()->type() == Token::TS_PROTECTED) {
    
    if (Current()->type() == Token::TS_IDENTIFIER) {
      TokenInfo info = *Current();
      auto identifier_result = ParseIdentifier();
      CHECK_AST(identifier_result);
      auto call_sig_result = ParseCallSignature(true, false);
      CHECK_AST(call_sig_result);
      auto ret = New<ir::AmbientMemberFunctionView>(false, false, true, mods,
                                                    identifier_result.node(),
                                                    call_sig_result.node());
      ret->SetInformationForNode(mods);
      return Success(ret);
    }
    SYNTAX_ERROR("'identifier' expected", Current());
  }
  SYNTAX_ERROR("unexpected token.", Current());
}


template <typename UCharInputIterator>
ParseResult Parser<UCharInputIterator>::ParseAmbientMemberVariable(Handle<ir::Node> mods) {
  LOG_PHASE(ParseAmbientMemberVariable);
  if (Current()->type() == Token::TS_IDENTIFIER) {
    auto identifier_result = ParseIdentifier();
    CHECK_AST(identifier_result);
    ParseResult type_expr_result;
    
    if (Current()->type() == Token::TS_COLON) {
      Next();
      type_expr_result = ParseTypeExpression();
      CHECK_AST(type_expr_result);
    }

    auto member_variable = New<ir::AmbientMemberVariableView>(mods, identifier_result.node(), type_expr_result.node());
    member_variable->SetInformationForNode(mods);
    return Success(member_variable);
  }
  SYNTAX_ERROR("'identifier' expected", Current());
}


template <typename UCharInputIterator>
ParseResult Parser<UCharInputIterator>::ParseAmbientEnumDeclaration(TokenInfo* info) {
  LOG_PHASE(ParseAmbientEnumDeclaration);
  if (Current()->type() == Token::TS_ENUM) {
    Next();
    auto identifier_result = ParseIdentifier();
    auto ambient_enum_body_result = ParseAmbientEnumBody();
    CHECK_AST(identifier_result);
    CHECK_AST(ambient_enum_body_result);
    auto ret = New<ir::AmbientEnumDeclarationView>(identifier_result.node(), ambient_enum_body_result.node());
    ret->SetInformationForNode(info);
    return Success(ret);
  }
  SYNTAX_ERROR("'enum' expected.", Current());
}


template <typename UCharInputIterator>
ParseResult Parser<UCharInputIterator>::ParseAmbientEnumBody() {
  LOG_PHASE(ParseAmbientEnumBody);
  if (Current()->type() == Token::TS_LEFT_BRACE) {
    auto ret = New<ir::AmbientEnumBodyView>();
    ret->SetInformationForNode(Current());
    Next();

    if (Current()->type() == Token::TS_RIGHT_BRACE) {
      Next();
      return Success(ret);
    }
    
    bool success = true;
    
    while (1) {
      auto ambient_enum_prop_result = ParseAmbientEnumProperty();
      SKIP_TOKEN_OR(ambient_enum_prop_result, success, Token::TS_RIGHT_BRACE) {
        ret->InsertLast(ambient_enum_prop_result.node());
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
  SYNTAX_ERROR("'{' exepected.", Current());
}


template <typename UCharInputIterator>
ParseResult Parser<UCharInputIterator>::ParseAmbientEnumProperty() {
  LOG_PHASE(ParseAmbientEnumProperty);
  auto prop_result = ParsePropertyName(false, false);
  CHECK_AST(prop_result);
  if (Current()->type() == Token::TS_ASSIGN) {
    Next();
    auto numeric_literal_result = ParseNumericLiteral();
    CHECK_AST(numeric_literal_result);
    return Success(CreateAmbientEnumFieldView(prop_result.node(), numeric_literal_result.node()));
  }
  return Success(CreateAmbientEnumFieldView(prop_result.node(), ir::Node::Null()));
}


template <typename UCharInputIterator>
Handle<ir::Node> Parser<UCharInputIterator>::CreateAmbientEnumFieldView(
    Handle<ir::Node> name,
    Handle<ir::Node> value) {
  LOG_PHASE(CreateAmbientEnumFieldView);
  auto ret = New<ir::AmbientEnumFieldView>(name, value);
  ret->SetInformationForNode(name);
  return ret;
}


template <typename UCharInputIterator>
ParseResult Parser<UCharInputIterator>::ParseAmbientModuleDeclaration(TokenInfo* info) {
  LOG_PHASE(ParseAmbientModuleDeclaration);

  if (Current()->type() == Token::TS_IDENTIFIER &&
      Current()->value()->Equals("module")) {
    Next();

    ParseResult identifier_result;
    bool external = false;
    if (Current()->type() == Token::TS_STRING_LITERAL) {
      identifier_result = ParseStringLiteral();
      external = true;
    } else {
      identifier_result = ParseIdentifier();
    }
    CHECK_AST(identifier_result);
    
    auto ambient_module_body_result = ParseAmbientModuleBody(external);
    CHECK_AST(ambient_module_body_result);
    auto ret = New<ir::AmbientModuleView>(external, identifier_result.node(), ambient_module_body_result.node());
    ret->SetInformationForNode(info);
    return Success(ret);
  }
  SYNTAX_ERROR("'module' expected.", Current());
}


template <typename UCharInputIterator>
ParseResult Parser<UCharInputIterator>::ParseAmbientModuleBody(bool external) {
  LOG_PHASE(ParseAmbientModuleBody);
  if (Current()->type() == Token::TS_LEFT_BRACE) {
    Handle<ir::Node> body = New<ir::AmbientModuleBody>();
    body->SetInformationForNode(Current());
    Next();
    
    bool success = true;
    
    while (1) {
      if (Current()->type() == Token::TS_RIGHT_BRACE) {
        Next();
        return Success(body);
      } else if (Current()->type() == Token::END_OF_INPUT) {
        SYNTAX_ERROR("unexpected end of input.", Current());
      } else {
        auto ambient_module_element_result = ParseAmbientModuleElement(external);
        SKIP_TOKEN_OR(ambient_module_element_result, success, Token::LINE_TERMINATOR) {
          body->InsertLast(ambient_module_element_result.node());
        }
      }
      if (IsLineTermination()) {
        ConsumeLineTerminator();
      }
    }
  }
  SYNTAX_ERROR("'{' expected.", Current());
}


template <typename UCharInputIterator>
ParseResult Parser<UCharInputIterator>::ParseAmbientModuleElement(bool external) {
  LOG_PHASE(ParseAmbientModuleElement);
  Handle<ir::ExportView> export_view;
  
  if (Current()->type() == Token::TS_EXPORT) {
    TokenInfo info = *Current();
    export_view = New<ir::ExportView>();
    export_view->SetInformationForNode(Current());
    Next();
    if (external && Current()->type() == Token::TS_ASSIGN) {
      Next();
      auto assignment_expr_result = ParseAssignmentExpression(true, false);
      CHECK_AST(assignment_expr_result);
      return Success(CreateExportView(assignment_expr_result.node(), ir::Node::Null(), &info, true));
    } else if (Current()->type() == Token::TS_ASSIGN) {
      SYNTAX_ERROR("export assignment is not allowed here.", Current());
    }
  }

  TokenInfo info = *Current();
  ParseResult parse_result;
  switch (Current()->type()) {
    case Token::TS_VAR:
      parse_result = ParseAmbientVariableDeclaration(&info);
      break;
    case Token::TS_FUNCTION:
      parse_result = ParseAmbientFunctionDeclaration(&info);
      break;
    case Token::TS_CLASS:
      parse_result = ParseAmbientClassDeclaration(&info);
      break;
    case Token::TS_INTERFACE: {
      parse_result = ParseInterfaceDeclaration();
      CHECK_AST(parse_result);
      parse_result.node()->SetInformationForNode(&info);
      break;
    }
    case Token::TS_ENUM:
      parse_result = ParseAmbientEnumDeclaration(&info);
      break;
    case Token::TS_IMPORT:
      parse_result = ParseImportDeclaration();
      CHECK_AST(parse_result);
      if (!external) {
        Handle<ir::Node> maybe_from = parse_result.node()->ToExportView()->from_clause();
        if (maybe_from) {
          if (maybe_from->HasExternalModuleReference()) {
            SYNTAX_ERROR("'require' not allowed here.", maybe_from);
          }
        }
      }
      parse_result.node()->SetInformationForNode(&info);
      break;
    default:
      if (Current()->type() == Token::TS_IDENTIFIER &&
          Current()->value()->Equals("module")) {
        parse_result = ParseAmbientModuleDeclaration(&info);
      } else {
        SYNTAX_ERROR("unexpected token.", Current());
      }
  }
  
  CHECK_AST(parse_result);

  if (export_view) {
    export_view->set_export_clause(parse_result.node());
    return Success(export_view);
  }
  return parse_result;
}
}
