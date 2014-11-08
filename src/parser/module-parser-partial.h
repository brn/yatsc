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
Handle<ir::Node> Parser<UCharInputIterator>::ParseModule() {
  LOG_PHASE(ParseModule);
  Handle<ir::Scope> scope = NewScope();
  set_current_scope(scope);
  YATSC_SCOPED([&] {set_current_scope(scope->parent_scope());});
  
  auto file_scope = New<ir::FileScopeView>(scope);
  
  while (Current()->type() != Token::END_OF_INPUT) {
    if (Current()->type() == Token::TS_IMPORT) {
      auto node = ParseImportDeclaration();
      SKIP_TOKEN_IF(node, Token::LINE_TERMINATOR);
      file_scope->InsertLast(node);
    } else if (Current()->type() == Token::TS_IDENTIFIER &&
               Current()->value()->Equals("module")) {
      auto node = ParseModuleImport();
      SKIP_TOKEN_IF(node, Token::LINE_TERMINATOR);
      file_scope->InsertLast(node);
    } else if (Current()->type() == Token::TS_EXPORT) {
      auto node = ParseExportDeclaration();
      SKIP_TOKEN_IF(node, Token::LINE_TERMINATOR);
      file_scope->InsertLast(node);
    } else {
      if (Current()->type() == Token::TS_IDENTIFIER &&
          Current()->value()->Equals("declare")) {
        auto node = ParseAmbientDeclaration(true);
        SKIP_TOKEN_IF(node, Token::LINE_TERMINATOR);
        file_scope->InsertLast(node);
      } else {
        auto node = ParseStatementListItem(false, false, false, false);
        SKIP_TOKEN_IF(node, Token::LINE_TERMINATOR);
        file_scope->InsertLast(node);
      }
    }
    
    if (IsLineTermination()) {
      ConsumeLineTerminator();
    }
  }
  return file_scope;
}


template <typename UCharInputIterator>
Handle<ir::Node> Parser<UCharInputIterator>::ParseImportDeclaration() {
  LOG_PHASE(ParseImportDeclaration);
  if (Current()->type() == Token::TS_IMPORT) {
    TokenInfo info = *Current();
    Next();
    if (Current()->type() == Token::TS_IDENTIFIER ||
        Current()->type() == Token::TS_LEFT_BRACE) {

      Handle<ir::Node> import_clause = ParseImportClause();
      CHECK_AST(import_clause);
      
      if (Current()->type() == Token::TS_ASSIGN) {
        Next();
        auto external_module_ref = ParseExternalModuleReference();
        CHECK_AST(external_module_ref);
        auto import_view = New<ir::ImportView>(import_clause, external_module_ref);
        import_view->SetInformationForNode(&info);
        return import_view;
      }
      Handle<ir::Node> from_clause = ParseFromClause();
      CHECK_AST(from_clause);
      auto import_view = New<ir::ImportView>(import_clause, from_clause);
      import_view->SetInformationForNode(&info);
      return import_view;
    } else if (Current()->type() == Token::TS_STRING_LITERAL) {
      Handle<ir::Node> module_specifier = ParseStringLiteral();
      CHECK_AST(module_specifier);
      auto import_view = New<ir::ImportView>(ir::Node::Null(), module_specifier);
      import_view->SetInformationForNode(&info);
      return import_view;
    }
    SYNTAX_ERROR("identifier or '{' or string literal expected.", Current());
  }
  SYNTAX_ERROR("'import' expected.", Current());
}


template <typename UCharInputIterator>
Handle<ir::Node> Parser<UCharInputIterator>::ParseExternalModuleReference() {
  LOG_PHASE(ParseExternalModuleReference);
  if (Current()->type() == Token::TS_IDENTIFIER &&
      Current()->value()->Equals("require")) {
    Next();
    if (Current()->type() == Token::TS_LEFT_PAREN) {
      Next();
      if (Current()->type() == Token::TS_STRING_LITERAL) {
        TokenInfo info = *Current();
        Next();
        if (Current()->type() == Token::TS_RIGHT_PAREN) {
          Next();
          if (info.value()->utf8_length() > 0) {
            if (info.utf8_value()[0] == '.') {
              String dir = Path::Dirname(module_info_->module_name());
              Notify("Parser::ModuleFound", Path::Join(dir, info.utf8_value()));
            }
          }
          return New<ir::ExternalModuleReference>(NewSymbol(ir::SymbolType::kVariableName, info.value()));
        }
        SYNTAX_ERROR("')' expected.", Current());
      }
      SYNTAX_ERROR("string literal expected.", Current());
    }
    SYNTAX_ERROR("'(' expected.", Current());
  }
  SYNTAX_ERROR("'require' expected.", Current());
}


template <typename UCharInputIterator>
Handle<ir::Node> Parser<UCharInputIterator>::ParseImportClause() {
  LOG_PHASE(ParseImportClause);
  Handle<ir::Node> first;
  Handle<ir::Node> second;

  if (Current()->type() == Token::TS_IDENTIFIER) {
    first = ParseIdentifier();
    CHECK_AST(first);
    if (Current()->type() == Token::TS_COMMA) {
      Next();
      if (Current()->type() == Token::TS_LEFT_BRACE) {
        second = ParseNamedImport();
        CHECK_AST(second);
      } 
    }
  } else if (Current()->type() == Token::TS_LEFT_BRACE) {
    first = ParseNamedImport();
    CHECK_AST(first);
    if (Current()->type() == Token::TS_COMMA) {
      Next();
      if (Current()->type() == Token::TS_IDENTIFIER) {
        second = ParseIdentifier();
        CHECK_AST(second);
      } 
    }
  }
  
  auto ret = New<ir::ImportClauseView>(first, second);
  ret->SetInformationForNode(first);
  return ret;
}


template <typename UCharInputIterator>
Handle<ir::Node> Parser<UCharInputIterator>::ParseNamedImport() {
  LOG_PHASE(ParseNamedImport);
  if (Current()->type() == Token::TS_LEFT_BRACE) {
    auto named_import_list = New<ir::NamedImportListView>();
    named_import_list->SetInformationForNode(Current());
    Next();
    
    while (1) {
      Handle<ir::Node> identifier = ParseBindingIdentifier(false, false);
      CHECK_AST(identifier);
      if (identifier->HasNameView() &&
          Current()->type() == Token::TS_IDENTIFIER &&
          Current()->value()->Equals("as")) {
        Next();
        Handle<ir::Node> binding = ParseBindingIdentifier(false, false);
        CHECK_AST(binding);
        auto named_import = New<ir::NamedImportView>(identifier, binding);
        named_import->SetInformationForNode(identifier);
        named_import_list->InsertLast(named_import);
      } else {
        named_import_list->InsertLast(identifier);
      }
      
      if (Current()->type() == Token::TS_COMMA) {
        Next();
      } else if (Current()->type() == Token::TS_RIGHT_BRACE) {
        Next();
        break;
      } else {
        SYNTAX_ERROR("unexpected token.", Current());
      }
    }
    return named_import_list;
  }
  SYNTAX_ERROR("'{' expected.", Current());
}


template <typename UCharInputIterator>
Handle<ir::Node> Parser<UCharInputIterator>::ParseFromClause() {
  LOG_PHASE(ParseFromClause);
  if (Current()->type() == Token::TS_IDENTIFIER &&
      Current()->value()->Equals("from")) {
    TokenInfo info = *Current();
    Next();
    Handle<ir::Node> module_specifier = ParseStringLiteral();
    CHECK_AST(module_specifier);
    module_specifier->SetInformationForNode(&info);
    return module_specifier;
  }
  SYNTAX_ERROR("'from' expected.", Current());
}


template <typename UCharInputIterator>
Handle<ir::Node> Parser<UCharInputIterator>::ParseModuleImport() {
  LOG_PHASE(ParseModuleImport);
  if (Current()->type() == Token::TS_IDENTIFIER &&
      Current()->value()->Equals("module")) {
    TokenInfo info = *Current();
    Next();

    RecordedParserState rps = parser_state();
    Handle<ir::Node> binding = ParseBindingIdentifier(false, false);
    CHECK_AST(binding);

    bool member = false;
    if (Current()->type() == Token::TS_DOT) {
      RestoreParserState(rps);
      binding = ParseMemberExpression(false);
      CHECK_AST(binding);
      member = true;
    }
    
    if (Current()->type() == Token::TS_LEFT_BRACE) {
      return ParseTSModule(binding, &info);
    }

    if (member) {
      SYNTAX_ERROR("unexpected token.", Current());
    }
    Handle<ir::Node> module_specifier = ParseFromClause();
    CHECK_AST(module_specifier);
    auto ret = New<ir::ModuleImportView>(binding, module_specifier);
    ret->SetInformationForNode(&info);
    return ret;
  }
  SYNTAX_ERROR("'module' expected.", Current());
}


template <typename UCharInputIterator>
Handle<ir::Node> Parser<UCharInputIterator>::ParseTSModule(Handle<ir::Node> identifier, TokenInfo* info) {
  LOG_PHASE(ParseTSModule);
  if (Current()->type() == Token::TS_LEFT_BRACE) {
    Handle<ir::Node> body = ParseTSModuleBody();
    CHECK_AST(body);
    auto ret = New<ir::ModuleDeclView>(identifier, body);
    ret->SetInformationForNode(info);
    return ret;
  }
  SYNTAX_ERROR("'{' expected.", Current());
}


template <typename UCharInputIterator>
Handle<ir::Node> Parser<UCharInputIterator>::ParseTSModuleBody() {
  LOG_PHASE(ParseTSModuleBody);
  if (Current()->type() == Token::TS_LEFT_BRACE) {
    Handle<ir::Scope> scope = NewScope();
    set_current_scope(scope);
    YATSC_SCOPED([&] {set_current_scope(scope->parent_scope());})
    auto block = New<ir::BlockView>(scope);
    block->SetInformationForNode(Current());
    Next();
    
    while (Current()->type() != Token::TS_RIGHT_BRACE) {
      if (Current()->type() == Token::TS_EXPORT) {
        Next();
        switch (Current()->type()) {
          case Token::TS_VAR: {
            auto node = ParseVariableStatement(true, false);
            SKIP_TOKEN_OR(node, Token::TS_RIGHT_BRACE) {
              block->InsertLast(node);
            }
            break;
          }
          case Token::TS_FUNCTION: {
            auto node = ParseFunctionOverloads(false, false, true, true);
            SKIP_TOKEN_OR(node, Token::TS_RIGHT_BRACE) {
              block->InsertLast(node);
            }
            break;
          }
          case Token::TS_CLASS: {
            auto node = ParseClassDeclaration(false, false);
            SKIP_TOKEN_OR(node, Token::TS_RIGHT_BRACE) {
              block->InsertLast(node);
            }
            break;
          }
          case Token::TS_INTERFACE: {
            auto node = ParseInterfaceDeclaration();
            SKIP_TOKEN_OR(node, Token::TS_RIGHT_BRACE) {
              block->InsertLast(node);
            }
            break;
          }
          case Token::TS_ENUM: {
            auto node = ParseEnumDeclaration(false, false);
            SKIP_TOKEN_OR(node, Token::TS_RIGHT_BRACE) {
              block->InsertLast(node);
            }
            break;
          }
          case Token::TS_IMPORT: {
            auto node = ParseVariableStatement(true, false);
            SKIP_TOKEN_OR(node, Token::TS_RIGHT_BRACE) {
              block->InsertLast(node);
            }
            break;
          }
          default:
            if (Current()->type() == Token::TS_IDENTIFIER &&
                Current()->value()->Equals("module")) {
              auto node = ParseModuleImport();
              SKIP_TOKEN_OR(node, Token::TS_RIGHT_BRACE) {
                block->InsertLast(node);
              }
            } else if (Current()->type() == Token::TS_IDENTIFIER &&
                       Current()->value()->Equals("declare")) {
              auto node = ParseAmbientDeclaration(false);
              SKIP_TOKEN_OR(node, Token::TS_RIGHT_BRACE) {
                block->InsertLast(node);
              }
            } else {
              SYNTAX_ERROR("unexpected token.", Current());
            }
        }
      } else if (Current()->type() == Token::TS_IDENTIFIER &&
                 Current()->value()->Equals("module")) {
        auto node = ParseModuleImport();
        SKIP_TOKEN_OR(node, Token::TS_RIGHT_BRACE) {
          block->InsertLast(node);
        }
      } else {
        auto node = ParseStatementListItem(false, false, false, false);
        SKIP_TOKEN_OR(node, Token::TS_RIGHT_BRACE) {
          block->InsertLast(node);
        }
      }
      
      if (IsLineTermination()) {
        ConsumeLineTerminator();
      }
    }
    Next();
    return block;
  }
  SYNTAX_ERROR("'{' expected.", Current());
}


template <typename UCharInputIterator>
Handle<ir::Node> Parser<UCharInputIterator>::ParseExportDeclaration() {
  LOG_PHASE(ParseExportDeclaration);
  if (Current()->type() == Token::TS_EXPORT) {

    TokenInfo info = *Current();
    Next();
    if (Current()->type() == Token::TS_MUL) {
      Next();
      auto from_clause = ParseFromClause();
      CHECK_AST(from_clause);
      return CreateExportView(ir::Node::Null(), from_clause, &info, false);
    }

    switch (Current()->type()) {
      case Token::TS_LEFT_BRACE: {
        Handle<ir::Node> export_clause = ParseExportClause();
        CHECK_AST(export_clause);
        if (Current()->type() == Token::TS_IDENTIFIER &&
            Current()->value()->Equals("from")) {
          auto from_clause = ParseFromClause();
          CHECK_AST(from_clause);
          return CreateExportView(export_clause, from_clause, &info, false);
        }
        return CreateExportView(export_clause, ir::Node::Null(), &info, false);
      }
      case Token::TS_VAR: {
        auto var = ParseVariableStatement(true, false);
        CHECK_AST(var);
        return CreateExportView(var, ir::Node::Null(), &info, false);
      }
      case Token::TS_CONST:
      case Token::TS_CLASS:
      case Token::TS_INTERFACE:
      case Token::TS_LET:
      case Token::TS_FUNCTION:
      case Token::TS_ENUM: {
        auto decl = ParseDeclaration(true, true, false);
        CHECK_AST(decl);
        return CreateExportView(decl, ir::Node::Null(), &info, false);
      }
      case Token::TS_DEFAULT:
      case Token::TS_ASSIGN: {
        Next();
        auto expr = ParseAssignmentExpression(true, false);
        CHECK_AST(expr);
        return CreateExportView(expr, ir::Node::Null(), &info, true);
      }
      default:
        if (Current()->type() == Token::TS_IDENTIFIER &&
            Current()->value()->Equals("declare")) {
          auto node = ParseAmbientDeclaration(true);
          CHECK_AST(node);
          return CreateExportView(node, ir::Node::Null(), &info, true);
        }
        SYNTAX_ERROR("unexpected token.", Current());
    }
  }
  SYNTAX_ERROR("'export' expected.", Current());
}


template <typename UCharInputIterator>
Handle<ir::Node> Parser<UCharInputIterator>::CreateExportView(
    Handle<ir::Node> export_clause,
    Handle<ir::Node> from_clause,
    TokenInfo* token_info,
    bool default_export) {
  LOG_PHASE(CreateExportView);
  auto export_view = New<ir::ExportView>(default_export, export_clause, from_clause);
  export_view->SetInformationForNode(token_info);
  return export_view;
}


template <typename UCharInputIterator>
Handle<ir::Node> Parser<UCharInputIterator>::ParseExportClause() {
  LOG_PHASE(ParseExportClause);
  if (Current()->type() == Token::TS_LEFT_BRACE) {
    auto named_export_list = New<ir::NamedExportListView>();
    named_export_list->SetInformationForNode(Current());
    Next();
    while (1) {
      Handle<ir::Node> identifier = ParseIdentifier();
      SKIP_TOKEN_OR(identifier, Token::TS_RIGHT_BRACE) {
        if (Current()->type() == Token::TS_IDENTIFIER &&
            Current()->value()->Equals("as")) {
          Next();
          Handle<ir::Node> binding = ParseIdentifier();
          SKIP_TOKEN_OR(binding, Token::TS_RIGHT_BRACE) {
            named_export_list->InsertLast(CreateNamedExportView(identifier, binding));
          }
        } else {
          named_export_list->InsertLast(CreateNamedExportView(identifier, ir::Node::Null()));
        }
      }
      if (Current()->type() == Token::TS_COMMA) {
        Next();
      } else if (Current()->type() == Token::TS_RIGHT_BRACE) {
        Next();
        return named_export_list;
      } else {
        SYNTAX_ERROR("',' or '}' expected.", Current());
      }
    }
  }
  SYNTAX_ERROR("'{' expected.", Current());
}


template <typename UCharInputIterator>
Handle<ir::Node> Parser<UCharInputIterator>::CreateNamedExportView(
    Handle<ir::Node> identifier,
    Handle<ir::Node> binding) {
  LOG_PHASE(CreateNamedExportView);
  auto named_export = New<ir::NamedExportView>(identifier, binding);
  named_export->SetInformationForNode(identifier);
  return named_export;
}
}
