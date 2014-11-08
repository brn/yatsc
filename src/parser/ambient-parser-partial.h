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
Handle<ir::Node> Parser<UCharInputIterator>::ParseDeclarationModule() {
  LOG_PHASE(ParseDeclarationModule);
  Handle<ir::Scope> scope = NewScope();
  set_current_scope(scope);
  YATSC_SCOPED([&] {set_current_scope(scope->parent_scope());});
  auto ret = New<ir::FileScopeView>(scope);
  ret->SetInformationForNode(Current());
  while (1) {
    Handle<ir::ExportView> export_view;
    if (Current()->type() == Token::TS_EXPORT) {
      TokenInfo info = *Current();
      Next();
      if (Current()->type() == Token::TS_ASSIGN) {
        Next();
        auto expr = ParseAssignmentExpression(true, false);
        SKIP_TOKEN_OR(expr, Token::LINE_TERMINATOR) {
          ret->InsertLast(CreateExportView(expr, ir::Node::Null(), &info, true));
        }
        continue;
      } else {
        export_view = New<ir::ExportView>();
        export_view->SetInformationForNode(&info);
      }
    }
    Handle<ir::Node> node;
    switch (Current()->type()) {
      case Token::TS_INTERFACE:
        node = ParseInterfaceDeclaration();
        break;
      case Token::TS_IMPORT:
        node = ParseImportDeclaration();
        break;
      default:
        node = ParseAmbientDeclaration(true);
        break;
    }
    
    SKIP_TOKEN_OR(node, Token::LINE_TERMINATOR) {
      if (IsLineTermination()) {
        ConsumeLineTerminator();
      }
    
      if (export_view) {
        export_view->set_export_clause(node);
        node = export_view;
      }

      ret->InsertLast(node);
    }
    
    if (Current()->type() == Token::END_OF_INPUT) {
      break;
    }
  }
  return ret;
}

template <typename UCharInputInterator>
Handle<ir::Node> Parser<UCharInputInterator>::ParseAmbientDeclaration(bool module_allowed) {
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
Handle<ir::Node> Parser<UCharInputInterator>::ParseAmbientVariableDeclaration(TokenInfo* info) {
  LOG_PHASE(ParseAmbientVariableDeclaration);
  if (Current()->type() == Token::TS_VAR) {
    Next();
    if (Current()->type() == Token::TS_IDENTIFIER) {
      Handle<ir::Node> identifier = ParseIdentifier();
      CHECK_AST(identifier);
      Handle<ir::Node> type_annotation;
      if (Current()->type() == Token::TS_COLON) {
        Next();
        type_annotation = ParseTypeExpression();
        CHECK_AST(type_annotation);
      }
      auto ret = New<ir::AmbientVariableView>(identifier, type_annotation);
      ret->SetInformationForNode(info);
      return ret;
    }
    SYNTAX_ERROR("'identifier' expected.", Current());
  }
  SYNTAX_ERROR("'var' expected.", Current());
}


template <typename UCharInputInterator>
Handle<ir::Node> Parser<UCharInputInterator>::ParseAmbientFunctionDeclaration(TokenInfo* info) {
  LOG_PHASE(ParseAmbientFunctionDeclaration);
  bool generator = false;

  if (Current()->type() == Token::TS_FUNCTION) {
    Next();
    if (Current()->type() == Token::TS_IDENTIFIER) {
      Handle<ir::Node> identifier = ParseIdentifier();
      CHECK_AST(identifier);
      if (Current()->type() == Token::TS_MUL) {
        generator = true;
        Next();
      }
      Handle<ir::Node> call_sig = ParseCallSignature(false, false);
      CHECK_AST(call_sig);
      auto ret = New<ir::AmbientFunctionDeclarationView>(generator, identifier, call_sig);
      ret->SetInformationForNode(info);
      return ret;
    }
  }
  SYNTAX_ERROR("'function' expected.", Current());
}


template <typename UCharInputInterator>
Handle<ir::Node> Parser<UCharInputInterator>::ParseAmbientClassDeclaration(TokenInfo* info) {
  LOG_PHASE(ParseAmbientClassDeclaration);
  if (Current()->type() == Token::TS_CLASS) {
    Next();
    Handle<ir::Node> identifier = ParseIdentifier();
    CHECK_AST(identifier);
    Handle<ir::Node> type_parameters;
    if (Current()->type() == Token::TS_LESS) {
     type_parameters = ParseTypeParameters();
     CHECK_AST(type_parameters);
    }
    Handle<ir::Node> bases = ParseClassBases();
    CHECK_AST(bases);
    if (Current()->type() == Token::TS_LEFT_BRACE) {
      Handle<ir::Node> body = ParseAmbientClassBody();
      CHECK_AST(body);
      auto ret = New<ir::AmbientClassDeclarationView>(identifier, type_parameters, bases, body);
      ret->SetInformationForNode(info);
      return ret;
    }
    SYNTAX_ERROR("'{' expected.", Current());
  }
  SYNTAX_ERROR("'class' expected.", Current());
}


template <typename UCharInputInterator>
Handle<ir::Node> Parser<UCharInputInterator>::ParseAmbientClassBody() {
  LOG_PHASE(ParseAmbientClassBody);
  if (Current()->type() == Token::TS_LEFT_BRACE) {
    auto body = New<ir::AmbientClassFieldsView>();
    body->SetInformationForNode(Current());
    Next();

    bool constructor_parsed = false;

    while (1) {
      if (Current()->type() == Token::TS_RIGHT_BRACE) {
        Next();
        return body;
      } else {
        auto node = ParseAmbientClassElement();
        SKIP_TOKEN_OR(node, Token::LINE_TERMINATOR) {
          body->InsertLast(node);
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
Handle<ir::Node> Parser<UCharInputInterator>::ParseAmbientClassElement() {
  LOG_PHASE(ParseAmbientClassElement);
  if (Current()->type() == Token::TS_LEFT_BRACKET) {
    return ParseIndexSignature();
  }

  Handle<ir::Node> mods = ParseFieldModifiers();
  CHECK_AST(mods);
  AccessorType at = ParseAccessor();

  if (TokenInfo::IsKeyword(Current()->type())) {
    Current()->set_type(Token::TS_IDENTIFIER);
  }
  
  if (Current()->type() == Token::TS_IDENTIFIER) {
    if (Current()->value()->Equals("constructor")) {
      return ParseAmbientConstructor(mods);
    } else {
      RecordedParserState rps = parser_state();
      Next();
      if (Current()->type() == Token::TS_LEFT_PAREN ||
          Current()->type() == Token::TS_LESS) {
        RestoreParserState(rps);
        return ParseAmbientMemberFunction(mods, &at);
      } else {
        RestoreParserState(rps);
        return ParseAmbientMemberVariable(mods);
      }
    }
  } else if (Current()->type() == Token::TS_MUL) {
    Next();
    return ParseAmbientGeneratorMethod(mods);
  }
  SYNTAX_ERROR("unexpected token", Current());
}


template <typename UCharInputIterator>
Handle<ir::Node> Parser<UCharInputIterator>::ParseAmbientConstructor(Handle<ir::Node> mods) {
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
      Handle<ir::Node> call_signature = ParseCallSignature(true, false);
      CHECK_AST(call_signature);
      auto ret = New<ir::AmbientConstructorView>(mods, call_signature);
      ret->SetInformationForNode(mods);
      return ret;
    }
    SYNTAX_ERROR("'constructor' expected", Current());
  }
  SYNTAX_ERROR("unexpected token.", Current());
}


template <typename UCharInputIterator>
Handle<ir::Node> Parser<UCharInputIterator>::ParseAmbientMemberFunction(Handle<ir::Node> mods, AccessorType* accessor_type) {
  LOG_PHASE(ParseAmbientMemberFunction);
  
  if (Current()->type() == Token::TS_IDENTIFIER ||
      Current()->type() == Token::TS_PUBLIC ||
      Current()->type() == Token::TS_PRIVATE ||
      Current()->type() == Token::TS_PROTECTED) {   
    
    if (Current()->type() == Token::TS_IDENTIFIER) {
      TokenInfo info = *Current();
      Handle<ir::Node> name = ParseIdentifier();
      CHECK_AST(name);
      Handle<ir::Node> call_signature = ParseCallSignature(true, false);
      CHECK_AST(call_signature);
      auto ret = New<ir::AmbientMemberFunctionView>(accessor_type->getter, accessor_type->setter, false, mods, name, call_signature);
      ret->SetInformationForNode(mods);
      return ret;
    }
    SYNTAX_ERROR("'identifier' expected", Current());
  }
  SYNTAX_ERROR("unexpected token.", Current());
}


template <typename UCharInputIterator>
Handle<ir::Node> Parser<UCharInputIterator>::ParseAmbientGeneratorMethod(Handle<ir::Node> mods) {
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
      Handle<ir::Node> name = ParseIdentifier();
      CHECK_AST(name);
      Handle<ir::Node> call_signature = ParseCallSignature(true, false);
      CHECK_AST(call_signature);
      auto ret = New<ir::AmbientMemberFunctionView>(false, false, true, mods, name, call_signature);
      ret->SetInformationForNode(mods);
      return ret;
    }
    SYNTAX_ERROR("'identifier' expected", Current());
  }
  SYNTAX_ERROR("unexpected token.", Current());
}


template <typename UCharInputIterator>
Handle<ir::Node> Parser<UCharInputIterator>::ParseAmbientMemberVariable(Handle<ir::Node> mods) {
  LOG_PHASE(ParseAmbientMemberVariable);
  if (Current()->type() == Token::TS_IDENTIFIER) {
    Handle<ir::Node> identifier = ParseIdentifier();
    CHECK_AST(identifier);
    Handle<ir::Node> type;
    
    if (Current()->type() == Token::TS_COLON) {
      Next();
      type = ParseTypeExpression();
      CHECK_AST(type);
    }

    auto member_variable = New<ir::AmbientMemberVariableView>(mods, identifier, type);
    member_variable->SetInformationForNode(mods);
    return member_variable;
  }
  SYNTAX_ERROR("'identifier' expected", Current());
}


template <typename UCharInputIterator>
Handle<ir::Node> Parser<UCharInputIterator>::ParseAmbientEnumDeclaration(TokenInfo* info) {
  LOG_PHASE(ParseAmbientEnumDeclaration);
  if (Current()->type() == Token::TS_ENUM) {
    Next();
    Handle<ir::Node> identifier = ParseIdentifier();
    Handle<ir::Node> body = ParseAmbientEnumBody();
    CHECK_AST(identifier);
    CHECK_AST(body);
    auto ret = New<ir::AmbientEnumDeclarationView>(identifier, body);
    ret->SetInformationForNode(info);
    return ret;
  }
  SYNTAX_ERROR("'enum' expected.", Current());
}


template <typename UCharInputIterator>
Handle<ir::Node> Parser<UCharInputIterator>::ParseAmbientEnumBody() {
  LOG_PHASE(ParseAmbientEnumBody);
  if (Current()->type() == Token::TS_LEFT_BRACE) {
    auto ret = New<ir::AmbientEnumBodyView>();
    ret->SetInformationForNode(Current());
    Next();

    if (Current()->type() == Token::TS_RIGHT_BRACE) {
      Next();
      return ret;
    }
    
    while (1) {
      auto node = ParseAmbientEnumProperty();
      SKIP_TOKEN_OR(node, Token::TS_RIGHT_BRACE) {
        ret->InsertLast(node);
      }
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
        SYNTAX_ERROR("',' or '}' expected.", Current());
      }
    }
  }
  SYNTAX_ERROR("'{' exepected.", Current());
}


template <typename UCharInputIterator>
Handle<ir::Node> Parser<UCharInputIterator>::ParseAmbientEnumProperty() {
  LOG_PHASE(ParseAmbientEnumProperty);
  Handle<ir::Node> prop = ParsePropertyName(false, false);
  CHECK_AST(prop);
  if (Current()->type() == Token::TS_ASSIGN) {
    Next();
    auto node = ParseNumericLiteral();
    CHECK_AST(node);
    return CreateAmbientEnumFieldView(prop, node);
  }
  return CreateAmbientEnumFieldView(prop, ir::Node::Null());
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
Handle<ir::Node> Parser<UCharInputIterator>::ParseAmbientModuleDeclaration(TokenInfo* info) {
  LOG_PHASE(ParseAmbientModuleDeclaration);

  if (Current()->type() == Token::TS_IDENTIFIER &&
      Current()->value()->Equals("module")) {
    Next();

    Handle<ir::Node> identifier;
    bool external = false;
    if (Current()->type() == Token::TS_STRING_LITERAL) {
      identifier = ParseStringLiteral();
      external = true;
    } else {
      identifier = ParseIdentifier();
    }
    CHECK_AST(identifier);
    
    Handle<ir::Node> body = ParseAmbientModuleBody(external);
    CHECK_AST(body);
    auto ret = New<ir::AmbientModuleView>(external, identifier, body);
    ret->SetInformationForNode(info);
    return ret;
  }
  SYNTAX_ERROR("'module' expected.", Current());
}


template <typename UCharInputIterator>
Handle<ir::Node> Parser<UCharInputIterator>::ParseAmbientModuleBody(bool external) {
  LOG_PHASE(ParseAmbientModuleBody);
  if (Current()->type() == Token::TS_LEFT_BRACE) {
    Handle<ir::Node> body = New<ir::AmbientModuleBody>();
    body->SetInformationForNode(Current());
    Next();
    
    while (1) {
      if (Current()->type() == Token::TS_RIGHT_BRACE) {
        Next();
        return body;
      } else if (Current()->type() == Token::END_OF_INPUT) {
        SYNTAX_ERROR("unexpected end of input.", Current());
      } else {
        auto node = ParseAmbientModuleElement(external);
        SKIP_TOKEN_OR(node, Token::LINE_TERMINATOR) {
          body->InsertLast(node);
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
Handle<ir::Node> Parser<UCharInputIterator>::ParseAmbientModuleElement(bool external) {
  LOG_PHASE(ParseAmbientModuleElement);
  Handle<ir::ExportView> export_view;
  if (Current()->type() == Token::TS_EXPORT) {
    TokenInfo info = *Current();
    export_view = New<ir::ExportView>();
    export_view->SetInformationForNode(Current());
    Next();
    if (external && Current()->type() == Token::TS_ASSIGN) {
      Next();
      auto node = ParseAssignmentExpression(true, false);
      CHECK_AST(node);
      return CreateExportView(node, ir::Node::Null(), &info, true);
    } else if (Current()->type() == Token::TS_ASSIGN) {
      SYNTAX_ERROR("export assignment is not allowed here.", Current());
    }
  }

  TokenInfo info = *Current();
  Handle<ir::Node> node;
  switch (Current()->type()) {
    case Token::TS_VAR:
      node = ParseAmbientVariableDeclaration(&info);
      break;
    case Token::TS_FUNCTION:
      node = ParseAmbientFunctionDeclaration(&info);
      break;
    case Token::TS_CLASS:
      node = ParseAmbientClassDeclaration(&info);
      break;
    case Token::TS_INTERFACE: {
      node = ParseInterfaceDeclaration();
      CHECK_AST(node);
      node->SetInformationForNode(&info);
      break;
    }
    case Token::TS_ENUM:
      node = ParseAmbientEnumDeclaration(&info);
      break;
    case Token::TS_IMPORT:
      node = ParseImportDeclaration();
      CHECK_AST(node);
      if (!external) {
        Handle<ir::Node> maybe_from = node->ToExportView()->from_clause();
        if (maybe_from) {
          if (maybe_from->HasExternalModuleReference()) {
            SYNTAX_ERROR("'require' not allowed here.", maybe_from);
          }
        }
      }
      node->SetInformationForNode(&info);
      break;
    default:
      if (Current()->type() == Token::TS_IDENTIFIER &&
          Current()->value()->Equals("module")) {
        node = ParseAmbientModuleDeclaration(&info);
      } else {
        SYNTAX_ERROR("unexpected token.", Current());
      }
  }
  
  CHECK_AST(node);

  if (export_view) {
    export_view->set_export_clause(node);
    return export_view;
  }
  return node;
}
}
