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
ParseResult Parser<UCharInputIterator>::ParseModule() {
  LOG_PHASE(ParseModule);
  
  auto file_scope = New<ir::FileScopeView>(current_scope());
  bool success = true;
  while (Current()->type() != Token::END_OF_INPUT) {
    if (Current()->type() == Token::TS_IMPORT) {
      auto import_decl_result = ParseImportDeclaration();
      SKIP_TOKEN_OR(import_decl_result, success, Token::LINE_TERMINATOR) {
        file_scope->InsertLast(import_decl_result.value());
      }
    } else if (Current()->type() == Token::TS_IDENTIFIER &&
               Current()->value()->Equals("module")) {
      auto module_decl_result = ParseModuleImport();
      SKIP_TOKEN_OR(module_decl_result, success, Token::LINE_TERMINATOR) {
        file_scope->InsertLast(module_decl_result.value());
      }
    } else if (Current()->type() == Token::TS_EXPORT) {
      auto export_decl_result = ParseExportDeclaration();
      SKIP_TOKEN_OR(export_decl_result, success, Token::LINE_TERMINATOR) {
        file_scope->InsertLast(export_decl_result.value());
      }
    } else {
      if (Current()->type() == Token::TS_IDENTIFIER &&
          Current()->value()->Equals("declare")) {
        auto ambient_decl_result = ParseAmbientDeclaration(true);
        SKIP_TOKEN_OR(ambient_decl_result, success, Token::LINE_TERMINATOR) {
          file_scope->InsertLast(ambient_decl_result.value());
        }
      } else {
        auto stmt_list_result = ParseStatementListItem(false, false, false, false);
        SKIP_TOKEN_OR(stmt_list_result, success, Token::LINE_TERMINATOR) {
          file_scope->InsertLast(stmt_list_result.value());
        }
      }
    }
    
    if (IsLineTermination()) {
      ConsumeLineTerminator();
    }
  }
  return Success(file_scope);
}


template <typename UCharInputIterator>
ParseResult Parser<UCharInputIterator>::ParseImportDeclaration() {
  LOG_PHASE(ParseImportDeclaration);
  if (Current()->type() == Token::TS_IMPORT) {
    TokenInfo info = *Current();
    Next();
    if (Current()->type() == Token::TS_IDENTIFIER ||
        Current()->type() == Token::TS_LEFT_BRACE) {

      auto import_clause_result = ParseImportClause();
      CHECK_AST(import_clause_result);
      
      if (Current()->type() == Token::TS_ASSIGN) {
        Next();
        auto external_module_ref_result = ParseExternalModuleReference();
        CHECK_AST(external_module_ref_result);
        auto import_view = New<ir::ImportView>(import_clause_result.value(), external_module_ref_result.value());
        import_view->SetInformationForNode(&info);
        return Success(import_view);
      }
      auto from_clause_result = ParseFromClause();
      CHECK_AST(from_clause_result);
      auto import_view = New<ir::ImportView>(import_clause_result.value(), from_clause_result.value());
      import_view->SetInformationForNode(&info);
      return Success(import_view);
    } else if (Current()->type() == Token::TS_STRING_LITERAL) {
      auto module_specifier_result = ParseStringLiteral();
      CHECK_AST(module_specifier_result);
      auto import_view = New<ir::ImportView>(ir::Node::Null(), module_specifier_result.value());
      import_view->SetInformationForNode(&info);
      return Success(import_view);
    }
    SYNTAX_ERROR("identifier or '{' or string literal expected.", Current());
  }
  SYNTAX_ERROR("'import' expected.", Current());
}


template <typename UCharInputIterator>
ParseResult Parser<UCharInputIterator>::ParseExternalModuleReference() {
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
          return Success(New<ir::ExternalModuleReference>(NewSymbol(ir::SymbolType::kVariableName, info.value())));
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
ParseResult Parser<UCharInputIterator>::ParseImportClause() {
  LOG_PHASE(ParseImportClause);
  ParseResult first_result;
  ParseResult second_result;

  if (Current()->type() == Token::TS_IDENTIFIER) {
    first_result = ParseIdentifier();
    CHECK_AST(first_result);
    if (Current()->type() == Token::TS_COMMA) {
      Next();
      if (Current()->type() == Token::TS_LEFT_BRACE) {
        second_result = ParseNamedImport();
        CHECK_AST(second_result);
      } 
    }
  } else if (Current()->type() == Token::TS_LEFT_BRACE) {
    first_result = ParseNamedImport();
    CHECK_AST(first_result);
    if (Current()->type() == Token::TS_COMMA) {
      Next();
      if (Current()->type() == Token::TS_IDENTIFIER) {
        second_result = ParseIdentifier();
        CHECK_AST(second_result);
      } 
    }
  }
  
  auto ret = New<ir::ImportClauseView>(first_result.value(), second_result.value());
  ret->SetInformationForNode(first_result.value());
  return Success(ret);
}


template <typename UCharInputIterator>
ParseResult Parser<UCharInputIterator>::ParseNamedImport() {
  LOG_PHASE(ParseNamedImport);
  if (Current()->type() == Token::TS_LEFT_BRACE) {
    auto named_import_list = New<ir::NamedImportListView>();
    named_import_list->SetInformationForNode(Current());
    Next();
    bool success = true;
    
    while (1) {
      auto identifier_result = ParseBindingIdentifier(false, false);
      CHECK_AST(identifier_result);
      if (identifier_result.value()->HasNameView() &&
          Current()->type() == Token::TS_IDENTIFIER &&
          Current()->value()->Equals("as")) {
        Next();
        auto binding_identifier_result = ParseBindingIdentifier(false, false);
        CHECK_AST(binding_identifier_result);
        auto named_import = New<ir::NamedImportView>(identifier_result.value(), binding_identifier_result.value());
        named_import->SetInformationForNode(identifier_result.value());
        named_import_list->InsertLast(named_import);
      } else {
        named_import_list->InsertLast(identifier_result.value());
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
    return Success(named_import_list);
  }
  SYNTAX_ERROR("'{' expected.", Current());
}


template <typename UCharInputIterator>
ParseResult Parser<UCharInputIterator>::ParseFromClause() {
  LOG_PHASE(ParseFromClause);
  if (Current()->type() == Token::TS_IDENTIFIER &&
      Current()->value()->Equals("from")) {
    TokenInfo info = *Current();
    Next();
    return ParseStringLiteral();
  }
  SYNTAX_ERROR("'from' expected.", Current());
}


template <typename UCharInputIterator>
ParseResult Parser<UCharInputIterator>::ParseModuleImport() {
  LOG_PHASE(ParseModuleImport);
  if (Current()->type() == Token::TS_IDENTIFIER &&
      Current()->value()->Equals("module")) {
    TokenInfo info = *Current();
    Next();

    RecordedParserState rps = parser_state();
    auto binding_identifier_result = ParseBindingIdentifier(false, false);
    CHECK_AST(binding_identifier_result);

    bool member = false;
    if (Current()->type() == Token::TS_DOT) {
      RestoreParserState(rps);
      binding_identifier_result = ParseMemberExpression(false);
      CHECK_AST(binding_identifier_result);
      member = true;
    }
    
    if (Current()->type() == Token::TS_LEFT_BRACE) {
      return ParseTSModule(binding_identifier_result.value(), &info);
    }

    if (member) {
      SYNTAX_ERROR("unexpected token.", Current());
    }
    auto module_specifier_result = ParseFromClause();
    CHECK_AST(module_specifier_result);
    auto ret = New<ir::ModuleImportView>(binding_identifier_result.value(), module_specifier_result.value());
    ret->SetInformationForNode(&info);
    return Success(ret);
  }
  SYNTAX_ERROR("'module' expected.", Current());
}


template <typename UCharInputIterator>
ParseResult Parser<UCharInputIterator>::ParseTSModule(Handle<ir::Node> identifier, TokenInfo* info) {
  LOG_PHASE(ParseTSModule);
  if (Current()->type() == Token::TS_LEFT_BRACE) {
    auto ts_module_body_result = ParseTSModuleBody();
    CHECK_AST(ts_module_body_result);
    auto ret = New<ir::ModuleDeclView>(identifier, ts_module_body_result.value());
    ret->SetInformationForNode(info);
    return Success(ret);
  }
  SYNTAX_ERROR("'{' expected.", Current());
}


template <typename UCharInputIterator>
ParseResult Parser<UCharInputIterator>::ParseTSModuleBody() {
  LOG_PHASE(ParseTSModuleBody);
  if (Current()->type() == Token::TS_LEFT_BRACE) {
    Handle<ir::Scope> scope = NewScope();
    set_current_scope(scope);
    YATSC_SCOPED([&] {set_current_scope(scope->parent_scope());})
    auto block = New<ir::BlockView>(scope);
    block->SetInformationForNode(Current());
    Next();

    bool success = true;
    
    while (Current()->type() != Token::TS_RIGHT_BRACE) {
      if (Current()->type() == Token::TS_EXPORT) {
        Next();
        switch (Current()->type()) {
          case Token::TS_VAR: {
            auto variable_stmt_result = ParseVariableStatement(true, false);
            SKIP_TOKEN_OR(variable_stmt_result, success, Token::TS_RIGHT_BRACE) {
              block->InsertLast(variable_stmt_result.value());
            }
            break;
          }
          case Token::TS_FUNCTION: {
            auto function_overloads_result = ParseFunctionOverloads(false, false, true, true);
            SKIP_TOKEN_OR(function_overloads_result, success, Token::TS_RIGHT_BRACE) {
              block->InsertLast(function_overloads_result.value());
            }
            break;
          }
          case Token::TS_CLASS: {
            auto class_decl_result = ParseClassDeclaration(false, false);
            SKIP_TOKEN_OR(class_decl_result, success, Token::TS_RIGHT_BRACE) {
              block->InsertLast(class_decl_result.value());
            }
            break;
          }
          case Token::TS_INTERFACE: {
            auto interface_decl_result = ParseInterfaceDeclaration();
            SKIP_TOKEN_OR(interface_decl_result, success, Token::TS_RIGHT_BRACE) {
              block->InsertLast(interface_decl_result.value());
            }
            break;
          }
          case Token::TS_ENUM: {
            auto enum_decl_result = ParseEnumDeclaration(false, false);
            SKIP_TOKEN_OR(enum_decl_result, success, Token::TS_RIGHT_BRACE) {
              block->InsertLast(enum_decl_result.value());
            }
            break;
          }
          case Token::TS_IMPORT: {
            auto variable_stmt_result = ParseVariableStatement(true, false);
            SKIP_TOKEN_OR(variable_stmt_result, success, Token::TS_RIGHT_BRACE) {
              block->InsertLast(variable_stmt_result.value());
            }
            break;
          }
          default:
            if (Current()->type() == Token::TS_IDENTIFIER &&
                Current()->value()->Equals("module")) {
              auto module_import_result = ParseModuleImport();
              SKIP_TOKEN_OR(module_import_result, success, Token::TS_RIGHT_BRACE) {
                block->InsertLast(module_import_result.value());
              }
            } else if (Current()->type() == Token::TS_IDENTIFIER &&
                       Current()->value()->Equals("declare")) {
              auto ambient_decl_result = ParseAmbientDeclaration(false);
              SKIP_TOKEN_OR(ambient_decl_result, success, Token::TS_RIGHT_BRACE) {
                block->InsertLast(ambient_decl_result.value());
              }
            } else {
              SYNTAX_ERROR("unexpected token.", Current());
            }
        }
      } else if (Current()->type() == Token::TS_IDENTIFIER &&
                 Current()->value()->Equals("module")) {
        auto module_import_result = ParseModuleImport();
        SKIP_TOKEN_OR(module_import_result, success, Token::TS_RIGHT_BRACE) {
          block->InsertLast(module_import_result.value());
        }
      } else {
        auto stmt_list_result = ParseStatementListItem(false, false, false, false);
        SKIP_TOKEN_OR(stmt_list_result, success, Token::TS_RIGHT_BRACE) {
          block->InsertLast(stmt_list_result.value());
        }
      }
      
      if (IsLineTermination()) {
        ConsumeLineTerminator();
      }
    }
    Next();
    return Success(block);
  }
  SYNTAX_ERROR("'{' expected.", Current());
}


template <typename UCharInputIterator>
ParseResult Parser<UCharInputIterator>::ParseExportDeclaration() {
  LOG_PHASE(ParseExportDeclaration);
  if (Current()->type() == Token::TS_EXPORT) {

    TokenInfo info = *Current();
    Next();
    if (Current()->type() == Token::TS_MUL) {
      Next();
      auto from_clause_result = ParseFromClause();
      CHECK_AST(from_clause_result);
      return Success(CreateExportView(ir::Node::Null(), from_clause_result.value(), &info, false));
    }

    switch (Current()->type()) {
      case Token::TS_LEFT_BRACE: {
        auto export_clause_result = ParseExportClause();
        CHECK_AST(export_clause_result);
        if (Current()->type() == Token::TS_IDENTIFIER &&
            Current()->value()->Equals("from")) {
          auto from_clause_result = ParseFromClause();
          CHECK_AST(from_clause_result);
          return Success(CreateExportView(export_clause_result.value(), from_clause_result.value(), &info, false));
        }
        return Success(CreateExportView(export_clause_result.value(), ir::Node::Null(), &info, false));
      }
      case Token::TS_VAR: {
        auto variable_stmt_result = ParseVariableStatement(true, false);
        CHECK_AST(variable_stmt_result);
        return Success(CreateExportView(variable_stmt_result.value(), ir::Node::Null(), &info, false));
      }
      case Token::TS_CONST:
      case Token::TS_CLASS:
      case Token::TS_INTERFACE:
      case Token::TS_LET:
      case Token::TS_FUNCTION:
      case Token::TS_ENUM: {
        auto decl_result = ParseDeclaration(true, true, false);
        CHECK_AST(decl_result);
        return Success(CreateExportView(decl_result.value(), ir::Node::Null(), &info, false));
      }
      case Token::TS_DEFAULT:
      case Token::TS_ASSIGN: {
        Next();
        auto assignment_expr_result = ParseAssignmentExpression(true, false);
        CHECK_AST(assignment_expr_result);
        return Success(CreateExportView(assignment_expr_result.value(), ir::Node::Null(), &info, true));
      }
      default:
        if (Current()->type() == Token::TS_IDENTIFIER &&
            Current()->value()->Equals("declare")) {
          auto ambient_decl_result = ParseAmbientDeclaration(true);
          CHECK_AST(ambient_decl_result);
          return Success(CreateExportView(ambient_decl_result.value(), ir::Node::Null(), &info, true));
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
ParseResult Parser<UCharInputIterator>::ParseExportClause() {
  LOG_PHASE(ParseExportClause);
  if (Current()->type() == Token::TS_LEFT_BRACE) {
    auto named_export_list = New<ir::NamedExportListView>();
    named_export_list->SetInformationForNode(Current());
    Next();

    bool success = true;
    
    while (1) {
      auto identifier_result = ParseIdentifier();
      SKIP_TOKEN_OR(identifier_result, success, Token::TS_RIGHT_BRACE) {
        if (Current()->type() == Token::TS_IDENTIFIER &&
            Current()->value()->Equals("as")) {
          Next();
          auto binding_identifier_result = ParseIdentifier();
          SKIP_TOKEN_OR(binding_identifier_result, success, Token::TS_RIGHT_BRACE) {
            named_export_list->InsertLast(CreateNamedExportView(identifier_result.value(), binding_identifier_result.value()));
          }
        } else {
          named_export_list->InsertLast(CreateNamedExportView(identifier_result.value(), ir::Node::Null()));
        }
      }
      if (Current()->type() == Token::TS_COMMA) {
        Next();
      } else if (Current()->type() == Token::TS_RIGHT_BRACE) {
        Next();
        return Success(named_export_list);
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
