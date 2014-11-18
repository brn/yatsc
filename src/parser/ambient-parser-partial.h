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
  ret->SetInformationForNode(cur_token());

  bool success = true;
  
  while (1) {
    Handle<ir::ExportView> export_view;
    if (cur_token()->type() == TokenKind::kExport) {
      Token info = *cur_token();
      Next();
      if (cur_token()->type() == TokenKind::kAssign) {
        Next();
        auto assignment_expr_result = ParseAssignmentExpression(true, false);
        SKIP_TOKEN_OR(assignment_expr_result, success, TokenKind::kLineTerminator) {
          ret->InsertLast(CreateExportView(assignment_expr_result.value(), ir::Node::Null(), &info, true));
        }
        continue;
      } else {
        export_view = New<ir::ExportView>();
        export_view->SetInformationForNode(&info);
      }
    }
    
    ParseResult parse_result;
    
    switch (cur_token()->type()) {
      case TokenKind::kInterface:
        parse_result = ParseInterfaceDeclaration();
        break;
      case TokenKind::kImport:
        parse_result = ParseImportDeclaration();
        break;
      default:
        parse_result = ParseAmbientDeclaration(true);
        break;
    }
    
    SKIP_TOKEN_OR(parse_result, success, TokenKind::kLineTerminator) {
      if (IsLineTermination()) {
        ConsumeLineTerminator();
      }
    
      if (export_view) {
        export_view->set_export_clause(parse_result.value());
        parse_result = Success(export_view);
      }

      ret->InsertLast(parse_result.value());
    }
    
    if (cur_token()->type() == TokenKind::kEof) {
      break;
    }
  }
  return Success(ret);
}

template <typename UCharInputInterator>
ParseResult Parser<UCharInputInterator>::ParseAmbientDeclaration(bool module_allowed) {
  LOG_PHASE(ParseAmbientDeclaration);
  if (cur_token()->type() == TokenKind::kIdentifier &&
      cur_token()->value()->Equals("declare")) {
    Token info = *cur_token();
    Next();
    switch (cur_token()->type()) {
      case TokenKind::kVar:
        return ParseAmbientVariableDeclaration(&info);
      case TokenKind::kFunction:
        return ParseAmbientFunctionDeclaration(&info);
      case TokenKind::kClass:
        return ParseAmbientClassDeclaration(&info);
      case TokenKind::kEnum:
        return ParseAmbientEnumDeclaration(&info);
      default:
        if (cur_token()->type() == TokenKind::kIdentifier &&
            cur_token()->value()->Equals("module")) {
          if (!module_allowed) {
            SYNTAX_ERROR_AND_SKIP_NEXT("ambient module declaration not allowed here.", cur_token(), TokenKind::kRightBrace);
          }
          return ParseAmbientModuleDeclaration(&info);
        }
        SYNTAX_ERROR("unexpected token.", cur_token());
    }
  }
  SYNTAX_ERROR("'declare' expected.", cur_token());
}


template <typename UCharInputInterator>
ParseResult Parser<UCharInputInterator>::ParseAmbientVariableDeclaration(Token* info) {
  LOG_PHASE(ParseAmbientVariableDeclaration);
  if (cur_token()->type() == TokenKind::kVar) {
    Next();
    if (cur_token()->type() == TokenKind::kIdentifier) {
      auto identifier_result = ParseIdentifier();
      CHECK_AST(identifier_result);
      ParseResult type_annotation_result;
      if (cur_token()->type() == TokenKind::kColon) {
        Next();
        type_annotation_result = ParseTypeExpression();
        CHECK_AST(type_annotation_result);
      }
      auto ret = New<ir::AmbientVariableView>(identifier_result.value(), type_annotation_result.value());
      ret->SetInformationForNode(info);
      return Success(ret);
    }
    SYNTAX_ERROR("'identifier' expected.", cur_token());
  }
  SYNTAX_ERROR("'var' expected.", cur_token());
}


template <typename UCharInputInterator>
ParseResult Parser<UCharInputInterator>::ParseAmbientFunctionDeclaration(Token* info) {
  LOG_PHASE(ParseAmbientFunctionDeclaration);
  bool generator = false;

  if (cur_token()->type() == TokenKind::kFunction) {
    Next();
    if (cur_token()->type() == TokenKind::kIdentifier) {
      auto identifier_result = ParseIdentifier();
      CHECK_AST(identifier_result);
      if (cur_token()->type() == TokenKind::kMul) {
        generator = true;
        Next();
      }
      auto call_sig_result = ParseCallSignature(false, false);
      CHECK_AST(call_sig_result);
      auto ret = New<ir::AmbientFunctionDeclarationView>(generator, identifier_result.value(), call_sig_result.value());
      ret->SetInformationForNode(info);
      return Success(ret);
    }
  }
  SYNTAX_ERROR("'function' expected.", cur_token());
}


template <typename UCharInputInterator>
ParseResult Parser<UCharInputInterator>::ParseAmbientClassDeclaration(Token* info) {
  LOG_PHASE(ParseAmbientClassDeclaration);
  if (cur_token()->type() == TokenKind::kClass) {
    Next();
    auto identifier_result = ParseIdentifier();
    CHECK_AST(identifier_result);
    ParseResult type_parameters_result;
    if (cur_token()->type() == TokenKind::kLess) {
     type_parameters_result = ParseTypeParameters();
     CHECK_AST(type_parameters_result);
    }
    auto class_bases_result = ParseClassBases();
    CHECK_AST(class_bases_result);
    if (cur_token()->type() == TokenKind::kLeftBrace) {
      auto ambient_class_body_result = ParseAmbientClassBody();
      CHECK_AST(ambient_class_body_result);
      auto ret = New<ir::AmbientClassDeclarationView>(identifier_result.value(),
                                                      type_parameters_result.value(),
                                                      class_bases_result.value(),
                                                      ambient_class_body_result.value());
      ret->SetInformationForNode(info);
      return Success(ret);
    }
    SYNTAX_ERROR("'{' expected.", cur_token());
  }
  SYNTAX_ERROR("'class' expected.", cur_token());
}


template <typename UCharInputInterator>
ParseResult Parser<UCharInputInterator>::ParseAmbientClassBody() {
  LOG_PHASE(ParseAmbientClassBody);
  if (cur_token()->type() == TokenKind::kLeftBrace) {
    auto body = New<ir::AmbientClassFieldsView>();
    body->SetInformationForNode(cur_token());
    Next();

    bool success = true;

    while (1) {
      if (cur_token()->type() == TokenKind::kRightBrace) {
        Next();
        return Success(body);
      } else {
        auto ambient_class_element_result = ParseAmbientClassElement();
        SKIP_TOKEN_OR(ambient_class_element_result, success, TokenKind::kLineTerminator) {
          body->InsertLast(ambient_class_element_result.value());
        }
        if (IsLineTermination()) {
          ConsumeLineTerminator();
        } else if (cur_token()->type() != TokenKind::kRightBrace &&
                   prev_token()->type() != TokenKind::kRightBrace) {
          SYNTAX_ERROR("';' expected", prev_token());
        }
      }
    }
  }
  SYNTAX_ERROR("'{' expected.", cur_token());
}


template <typename UCharInputInterator>
ParseResult Parser<UCharInputInterator>::ParseAmbientClassElement() {
  LOG_PHASE(ParseAmbientClassElement);
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
      return ParseAmbientConstructor(field_modifiers_result.value());
    } else {
      RecordedParserState rps = parser_state();
      Next();
      if (cur_token()->type() == TokenKind::kLeftParen ||
          cur_token()->type() == TokenKind::kLess) {
        RestoreParserState(rps);
        return ParseAmbientMemberFunction(field_modifiers_result.value(), &at);
      } else {
        RestoreParserState(rps);
        return ParseAmbientMemberVariable(field_modifiers_result.value());
      }
    }
  } else if (cur_token()->type() == TokenKind::kMul) {
    Next();
    return ParseAmbientGeneratorMethod(field_modifiers_result.value());
  }
  SYNTAX_ERROR("unexpected token", cur_token());
}


template <typename UCharInputIterator>
ParseResult Parser<UCharInputIterator>::ParseAmbientConstructor(Handle<ir::Node> mods) {
  LOG_PHASE(ParseAmbientConstructor);
  
  if ((cur_token()->type() == TokenKind::kIdentifier &&
       cur_token()->value()->Equals("constructor")) ||
      cur_token()->type() == TokenKind::kPublic ||
      cur_token()->type() == TokenKind::kPrivate ||
      cur_token()->type() == TokenKind::kProtected) {
    
    if (cur_token()->type() == TokenKind::kIdentifier &&
        cur_token()->value()->Equals("constructor")) {
      Token info = *cur_token();
      Next();
      auto call_sig_result = ParseCallSignature(true, false);
      CHECK_AST(call_sig_result);
      auto ret = New<ir::AmbientConstructorView>(mods, call_sig_result.value());
      ret->SetInformationForNode(mods);
      return Success(ret);
    }
    SYNTAX_ERROR("'constructor' expected", cur_token());
  }
  SYNTAX_ERROR("unexpected token.", cur_token());
}


template <typename UCharInputIterator>
ParseResult Parser<UCharInputIterator>::ParseAmbientMemberFunction(Handle<ir::Node> mods, AccessorType* accessor_type) {
  LOG_PHASE(ParseAmbientMemberFunction);
  
  if (cur_token()->type() == TokenKind::kIdentifier ||
      cur_token()->type() == TokenKind::kPublic ||
      cur_token()->type() == TokenKind::kPrivate ||
      cur_token()->type() == TokenKind::kProtected) {   
    
    if (cur_token()->type() == TokenKind::kIdentifier) {
      Token info = *cur_token();
      auto identifier_result = ParseIdentifier();
      CHECK_AST(identifier_result);
      auto call_sig_result = ParseCallSignature(true, false);
      CHECK_AST(call_sig_result);
      auto ret = New<ir::AmbientMemberFunctionView>(accessor_type->getter,
                                                    accessor_type->setter,
                                                    false,
                                                    mods,
                                                    identifier_result.value(),
                                                    call_sig_result.value());
      ret->SetInformationForNode(mods);
      return Success(ret);
    }
    SYNTAX_ERROR("'identifier' expected", cur_token());
  }
  SYNTAX_ERROR("unexpected token.", cur_token());
}


template <typename UCharInputIterator>
ParseResult Parser<UCharInputIterator>::ParseAmbientGeneratorMethod(Handle<ir::Node> mods) {
  LOG_PHASE(ParseAmbientGeneratorMethod);

  if (cur_token()->type() == TokenKind::kMul) {
    Next();
  } else {
    SYNTAX_ERROR("'*' expected", cur_token());
  }
  
  if (cur_token()->type() == TokenKind::kIdentifier ||
      cur_token()->type() == TokenKind::kPublic ||
      cur_token()->type() == TokenKind::kPrivate ||
      cur_token()->type() == TokenKind::kProtected) {
    
    if (cur_token()->type() == TokenKind::kIdentifier) {
      Token info = *cur_token();
      auto identifier_result = ParseIdentifier();
      CHECK_AST(identifier_result);
      auto call_sig_result = ParseCallSignature(true, false);
      CHECK_AST(call_sig_result);
      auto ret = New<ir::AmbientMemberFunctionView>(false, false, true, mods,
                                                    identifier_result.value(),
                                                    call_sig_result.value());
      ret->SetInformationForNode(mods);
      return Success(ret);
    }
    SYNTAX_ERROR("'identifier' expected", cur_token());
  }
  SYNTAX_ERROR("unexpected token.", cur_token());
}


template <typename UCharInputIterator>
ParseResult Parser<UCharInputIterator>::ParseAmbientMemberVariable(Handle<ir::Node> mods) {
  LOG_PHASE(ParseAmbientMemberVariable);
  if (cur_token()->type() == TokenKind::kIdentifier) {
    auto identifier_result = ParseIdentifier();
    CHECK_AST(identifier_result);
    ParseResult type_expr_result;
    
    if (cur_token()->type() == TokenKind::kColon) {
      Next();
      type_expr_result = ParseTypeExpression();
      CHECK_AST(type_expr_result);
    }

    auto member_variable = New<ir::AmbientMemberVariableView>(mods, identifier_result.value(), type_expr_result.value());
    member_variable->SetInformationForNode(mods);
    return Success(member_variable);
  }
  SYNTAX_ERROR("'identifier' expected", cur_token());
}


template <typename UCharInputIterator>
ParseResult Parser<UCharInputIterator>::ParseAmbientEnumDeclaration(Token* info) {
  LOG_PHASE(ParseAmbientEnumDeclaration);
  if (cur_token()->type() == TokenKind::kEnum) {
    Next();
    auto identifier_result = ParseIdentifier();
    auto ambient_enum_body_result = ParseAmbientEnumBody();
    CHECK_AST(identifier_result);
    CHECK_AST(ambient_enum_body_result);
    auto ret = New<ir::AmbientEnumDeclarationView>(identifier_result.value(), ambient_enum_body_result.value());
    ret->SetInformationForNode(info);
    return Success(ret);
  }
  SYNTAX_ERROR("'enum' expected.", cur_token());
}


template <typename UCharInputIterator>
ParseResult Parser<UCharInputIterator>::ParseAmbientEnumBody() {
  LOG_PHASE(ParseAmbientEnumBody);
  if (cur_token()->type() == TokenKind::kLeftBrace) {
    auto ret = New<ir::AmbientEnumBodyView>();
    ret->SetInformationForNode(cur_token());
    Next();

    if (cur_token()->type() == TokenKind::kRightBrace) {
      Next();
      return Success(ret);
    }
    
    bool success = true;
    
    while (1) {
      auto ambient_enum_prop_result = ParseAmbientEnumProperty();
      SKIP_TOKEN_OR(ambient_enum_prop_result, success, TokenKind::kRightBrace) {
        ret->InsertLast(ambient_enum_prop_result.value());
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
  SYNTAX_ERROR("'{' exepected.", cur_token());
}


template <typename UCharInputIterator>
ParseResult Parser<UCharInputIterator>::ParseAmbientEnumProperty() {
  LOG_PHASE(ParseAmbientEnumProperty);
  auto prop_result = ParsePropertyName(false, false);
  CHECK_AST(prop_result);
  if (cur_token()->type() == TokenKind::kAssign) {
    Next();
    auto numeric_literal_result = ParseNumericLiteral();
    CHECK_AST(numeric_literal_result);
    return Success(CreateAmbientEnumFieldView(prop_result.value(), numeric_literal_result.value()));
  }
  return Success(CreateAmbientEnumFieldView(prop_result.value(), ir::Node::Null()));
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
ParseResult Parser<UCharInputIterator>::ParseAmbientModuleDeclaration(Token* info) {
  LOG_PHASE(ParseAmbientModuleDeclaration);

  if (cur_token()->type() == TokenKind::kIdentifier &&
      cur_token()->value()->Equals("module")) {
    Next();

    ParseResult identifier_result;
    bool external = false;
    if (cur_token()->type() == TokenKind::kStringLiteral) {
      identifier_result = ParseStringLiteral();
      external = true;
    } else {
      identifier_result = ParseIdentifier();
    }
    CHECK_AST(identifier_result);
    
    auto ambient_module_body_result = ParseAmbientModuleBody(external);
    CHECK_AST(ambient_module_body_result);
    auto ret = New<ir::AmbientModuleView>(external, identifier_result.value(), ambient_module_body_result.value());
    ret->SetInformationForNode(info);
    return Success(ret);
  }
  SYNTAX_ERROR("'module' expected.", cur_token());
}


template <typename UCharInputIterator>
ParseResult Parser<UCharInputIterator>::ParseAmbientModuleBody(bool external) {
  LOG_PHASE(ParseAmbientModuleBody);
  if (cur_token()->type() == TokenKind::kLeftBrace) {
    Handle<ir::Node> body = New<ir::AmbientModuleBody>();
    body->SetInformationForNode(cur_token());
    Next();
    
    bool success = true;
    
    while (1) {
      if (cur_token()->type() == TokenKind::kRightBrace) {
        Next();
        return Success(body);
      } else if (cur_token()->type() == TokenKind::kEof) {
        SYNTAX_ERROR("unexpected end of input.", cur_token());
      } else {
        auto ambient_module_element_result = ParseAmbientModuleElement(external);
        SKIP_TOKEN_OR(ambient_module_element_result, success, TokenKind::kLineTerminator) {
          body->InsertLast(ambient_module_element_result.value());
        }
      }
      if (IsLineTermination()) {
        ConsumeLineTerminator();
      }
    }
  }
  SYNTAX_ERROR("'{' expected.", cur_token());
}


template <typename UCharInputIterator>
ParseResult Parser<UCharInputIterator>::ParseAmbientModuleElement(bool external) {
  LOG_PHASE(ParseAmbientModuleElement);
  Handle<ir::ExportView> export_view;
  
  if (cur_token()->type() == TokenKind::kExport) {
    Token info = *cur_token();
    export_view = New<ir::ExportView>();
    export_view->SetInformationForNode(cur_token());
    Next();
    if (external && cur_token()->type() == TokenKind::kAssign) {
      Next();
      auto assignment_expr_result = ParseAssignmentExpression(true, false);
      CHECK_AST(assignment_expr_result);
      return Success(CreateExportView(assignment_expr_result.value(), ir::Node::Null(), &info, true));
    } else if (cur_token()->type() == TokenKind::kAssign) {
      SYNTAX_ERROR("export assignment is not allowed here.", cur_token());
    }
  }

  Token info = *cur_token();
  ParseResult parse_result;
  switch (cur_token()->type()) {
    case TokenKind::kVar:
      parse_result = ParseAmbientVariableDeclaration(&info);
      break;
    case TokenKind::kFunction:
      parse_result = ParseAmbientFunctionDeclaration(&info);
      break;
    case TokenKind::kClass:
      parse_result = ParseAmbientClassDeclaration(&info);
      break;
    case TokenKind::kInterface: {
      parse_result = ParseInterfaceDeclaration();
      CHECK_AST(parse_result);
      parse_result.value()->SetInformationForNode(&info);
      break;
    }
    case TokenKind::kEnum:
      parse_result = ParseAmbientEnumDeclaration(&info);
      break;
    case TokenKind::kImport:
      parse_result = ParseImportDeclaration();
      CHECK_AST(parse_result);
      if (!external) {
        Handle<ir::Node> maybe_from = parse_result.value()->ToExportView()->from_clause();
        if (maybe_from) {
          if (maybe_from->HasExternalModuleReference()) {
            SYNTAX_ERROR("'require' not allowed here.", maybe_from);
          }
        }
      }
      parse_result.value()->SetInformationForNode(&info);
      break;
    default:
      if (cur_token()->type() == TokenKind::kIdentifier &&
          cur_token()->value()->Equals("module")) {
        parse_result = ParseAmbientModuleDeclaration(&info);
      } else {
        SYNTAX_ERROR("unexpected token.", cur_token());
      }
  }
  
  CHECK_AST(parse_result);

  if (export_view) {
    export_view->set_export_clause(parse_result.value());
    return Success(export_view);
  }
  return parse_result;
}
}
