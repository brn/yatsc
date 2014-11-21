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
  while (cur_token()->type() != TokenKind::kEof) {
    if (cur_token()->type() == TokenKind::kImport) {
      auto import_decl_result = ParseImportDeclaration();
      SKIP_TOKEN_OR(import_decl_result, success, TokenKind::kLineTerminator) {
        file_scope->InsertLast(import_decl_result.value());
      }
    } else if (cur_token()->type() == TokenKind::kIdentifier &&
               cur_token()->value()->Equals("module")) {
      auto module_decl_result = ParseModuleImport();
      SKIP_TOKEN_OR(module_decl_result, success, TokenKind::kLineTerminator) {
        file_scope->InsertLast(module_decl_result.value());
      }
    } else if (cur_token()->type() == TokenKind::kExport) {
      auto export_decl_result = ParseExportDeclaration();
      SKIP_TOKEN_OR(export_decl_result, success, TokenKind::kLineTerminator) {
        file_scope->InsertLast(export_decl_result.value());
      }
    } else {
      if (cur_token()->type() == TokenKind::kIdentifier &&
          cur_token()->value()->Equals("declare")) {
        auto ambient_decl_result = ParseAmbientDeclaration(true);
        SKIP_TOKEN_OR(ambient_decl_result, success, TokenKind::kLineTerminator) {
          file_scope->InsertLast(ambient_decl_result.value());
        }
      } else {
        auto stmt_list_result = ParseStatementListItem(false, false, false, false);
        SKIP_TOKEN_OR(stmt_list_result, success, TokenKind::kLineTerminator) {
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
  if (cur_token()->type() == TokenKind::kImport) {
    Token info = *cur_token();
    Next();
    if (cur_token()->type() == TokenKind::kIdentifier ||
        cur_token()->type() == TokenKind::kLeftBrace) {

      auto import_clause_result = ParseImportClause();
      CHECK_AST(import_clause_result);
      
      if (cur_token()->type() == TokenKind::kAssign) {
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
    } else if (cur_token()->type() == TokenKind::kStringLiteral) {
      auto module_specifier_result = ParseStringLiteral();
      CHECK_AST(module_specifier_result);
      auto import_view = New<ir::ImportView>(ir::Node::Null(), module_specifier_result.value());
      import_view->SetInformationForNode(&info);
      return Success(import_view);
    }
    SYNTAX_ERROR("identifier or '{' or string literal expected.", cur_token());
  }
  SYNTAX_ERROR("'import' expected.", cur_token());
}


template <typename UCharInputIterator>
ParseResult Parser<UCharInputIterator>::ParseExternalModuleReference() {
  LOG_PHASE(ParseExternalModuleReference);
  if (cur_token()->type() == TokenKind::kIdentifier &&
      cur_token()->value()->Equals("require")) {
    Next();
    if (cur_token()->type() == TokenKind::kLeftParen) {
      Next();
      if (cur_token()->type() == TokenKind::kStringLiteral) {
        Token info = *cur_token();
        Next();
        if (cur_token()->type() == TokenKind::kRightParen) {
          Next();
          if (info.value()->utf8_length() > 0) {
            if (info.utf8_value()[0] == '.') {
              String dir = Path::Dirname(module_info_->module_name());
              Notify("Parser::ModuleFound", Path::Join(dir, info.utf8_value()));
            }
          }
          return Success(New<ir::ExternalModuleReference>(NewSymbol(ir::SymbolType::kVariableName, info.value())));
        }
        SYNTAX_ERROR("')' expected.", cur_token());
      }
      SYNTAX_ERROR("string literal expected.", cur_token());
    }
    SYNTAX_ERROR("'(' expected.", cur_token());
  }
  SYNTAX_ERROR("'require' expected.", cur_token());
}


template <typename UCharInputIterator>
ParseResult Parser<UCharInputIterator>::ParseImportClause() {
  LOG_PHASE(ParseImportClause);
  ParseResult first_result;
  ParseResult second_result;

  if (cur_token()->type() == TokenKind::kIdentifier) {
    first_result = ParseIdentifier();
    CHECK_AST(first_result);
    if (cur_token()->type() == TokenKind::kComma) {
      Next();
      if (cur_token()->type() == TokenKind::kLeftBrace) {
        second_result = ParseNamedImport();
        CHECK_AST(second_result);
      } 
    }
  } else if (cur_token()->type() == TokenKind::kLeftBrace) {
    first_result = ParseNamedImport();
    CHECK_AST(first_result);
    if (cur_token()->type() == TokenKind::kComma) {
      Next();
      if (cur_token()->type() == TokenKind::kIdentifier) {
        second_result = ParseIdentifier();
        CHECK_AST(second_result);
      } 
    }
  }
  
  auto ret = New<ir::ImportClauseView>(first_result.or(ir::Node::Null()), second_result.or(ir::Node::Null()));
  ret->SetInformationForNode(first_result.value());
  return Success(ret);
}


template <typename UCharInputIterator>
ParseResult Parser<UCharInputIterator>::ParseNamedImport() {
  LOG_PHASE(ParseNamedImport);
  if (cur_token()->type() == TokenKind::kLeftBrace) {
    auto named_import_list = New<ir::NamedImportListView>();
    named_import_list->SetInformationForNode(cur_token());
    Next();
    bool success = true;
    
    while (1) {
      auto identifier_result = ParseBindingIdentifier(false, false);
      CHECK_AST(identifier_result);
      if (identifier_result.value()->HasNameView() &&
          cur_token()->type() == TokenKind::kIdentifier &&
          cur_token()->value()->Equals("as")) {
        Next();
        auto binding_identifier_result = ParseBindingIdentifier(false, false);
        CHECK_AST(binding_identifier_result);
        auto named_import = New<ir::NamedImportView>(identifier_result.value(), binding_identifier_result.value());
        named_import->SetInformationForNode(identifier_result.value());
        named_import_list->InsertLast(named_import);
      } else {
        named_import_list->InsertLast(identifier_result.value());
      }
      
      if (cur_token()->type() == TokenKind::kComma) {
        Next();
      } else if (cur_token()->type() == TokenKind::kRightBrace) {
        Next();
        break;
      } else {
        SYNTAX_ERROR("unexpected token.", cur_token());
      }
    }
    return Success(named_import_list);
  }
  SYNTAX_ERROR("'{' expected.", cur_token());
}


template <typename UCharInputIterator>
ParseResult Parser<UCharInputIterator>::ParseFromClause() {
  LOG_PHASE(ParseFromClause);
  if (cur_token()->type() == TokenKind::kIdentifier &&
      cur_token()->value()->Equals("from")) {
    Token info = *cur_token();
    Next();
    return ParseStringLiteral();
  }
  SYNTAX_ERROR("'from' expected.", cur_token());
}


template <typename UCharInputIterator>
ParseResult Parser<UCharInputIterator>::ParseModuleImport() {
  LOG_PHASE(ParseModuleImport);
  if (cur_token()->type() == TokenKind::kIdentifier &&
      cur_token()->value()->Equals("module")) {
    Token info = *cur_token();
    Next();

    RecordedParserState rps = parser_state();
    auto binding_identifier_result = ParseBindingIdentifier(false, false);
    CHECK_AST(binding_identifier_result);

    bool member = false;
    if (cur_token()->type() == TokenKind::kDot) {
      RestoreParserState(rps);
      binding_identifier_result = ParseMemberExpression(false);
      CHECK_AST(binding_identifier_result);
      member = true;
    }
    
    if (cur_token()->type() == TokenKind::kLeftBrace) {
      return ParseTSModule(binding_identifier_result.value(), &info);
    }

    if (member) {
      SYNTAX_ERROR("unexpected token.", cur_token());
    }
    auto module_specifier_result = ParseFromClause();
    CHECK_AST(module_specifier_result);
    auto ret = New<ir::ModuleImportView>(binding_identifier_result.value(), module_specifier_result.value());
    ret->SetInformationForNode(&info);
    return Success(ret);
  }
  SYNTAX_ERROR("'module' expected.", cur_token());
}


template <typename UCharInputIterator>
ParseResult Parser<UCharInputIterator>::ParseTSModule(Handle<ir::Node> identifier, Token* info) {
  LOG_PHASE(ParseTSModule);
  if (cur_token()->type() == TokenKind::kLeftBrace) {
    auto ts_module_body_result = ParseTSModuleBody();
    CHECK_AST(ts_module_body_result);
    auto ret = New<ir::ModuleDeclView>(identifier, ts_module_body_result.value());
    ret->SetInformationForNode(info);
    return Success(ret);
  }
  SYNTAX_ERROR("'{' expected.", cur_token());
}


template <typename UCharInputIterator>
ParseResult Parser<UCharInputIterator>::ParseTSModuleBody() {
  LOG_PHASE(ParseTSModuleBody);
  if (cur_token()->type() == TokenKind::kLeftBrace) {
    Handle<ir::Scope> scope = NewScope();
    set_current_scope(scope);
    YATSC_SCOPED([&] {set_current_scope(scope->parent_scope());})
    auto block = New<ir::BlockView>(scope);
    block->SetInformationForNode(cur_token());
    Next();

    bool success = true;
    
    while (cur_token()->type() != TokenKind::kRightBrace) {
      if (cur_token()->type() == TokenKind::kExport) {
        Next();
        switch (cur_token()->type()) {
          case TokenKind::kVar: {
            auto variable_stmt_result = ParseVariableStatement(true, false);
            SKIP_TOKEN_OR(variable_stmt_result, success, TokenKind::kRightBrace) {
              block->InsertLast(variable_stmt_result.value());
            }
            break;
          }
          case TokenKind::kFunction: {
            auto function_overloads_result = ParseFunctionOverloads(false, false, true, true);
            SKIP_TOKEN_OR(function_overloads_result, success, TokenKind::kRightBrace) {
              block->InsertLast(function_overloads_result.value());
            }
            break;
          }
          case TokenKind::kClass: {
            auto class_decl_result = ParseClassDeclaration(false, false);
            SKIP_TOKEN_OR(class_decl_result, success, TokenKind::kRightBrace) {
              block->InsertLast(class_decl_result.value());
            }
            break;
          }
          case TokenKind::kInterface: {
            auto interface_decl_result = ParseInterfaceDeclaration();
            SKIP_TOKEN_OR(interface_decl_result, success, TokenKind::kRightBrace) {
              block->InsertLast(interface_decl_result.value());
            }
            break;
          }
          case TokenKind::kEnum: {
            auto enum_decl_result = ParseEnumDeclaration(false, false);
            SKIP_TOKEN_OR(enum_decl_result, success, TokenKind::kRightBrace) {
              block->InsertLast(enum_decl_result.value());
            }
            break;
          }
          case TokenKind::kImport: {
            auto variable_stmt_result = ParseVariableStatement(true, false);
            SKIP_TOKEN_OR(variable_stmt_result, success, TokenKind::kRightBrace) {
              block->InsertLast(variable_stmt_result.value());
            }
            break;
          }
          default:
            if (cur_token()->type() == TokenKind::kIdentifier &&
                cur_token()->value()->Equals("module")) {
              auto module_import_result = ParseModuleImport();
              SKIP_TOKEN_OR(module_import_result, success, TokenKind::kRightBrace) {
                block->InsertLast(module_import_result.value());
              }
            } else if (cur_token()->type() == TokenKind::kIdentifier &&
                       cur_token()->value()->Equals("declare")) {
              auto ambient_decl_result = ParseAmbientDeclaration(false);
              SKIP_TOKEN_OR(ambient_decl_result, success, TokenKind::kRightBrace) {
                block->InsertLast(ambient_decl_result.value());
              }
            } else {
              SYNTAX_ERROR("unexpected token.", cur_token());
            }
        }
      } else if (cur_token()->type() == TokenKind::kIdentifier &&
                 cur_token()->value()->Equals("module")) {
        auto module_import_result = ParseModuleImport();
        SKIP_TOKEN_OR(module_import_result, success, TokenKind::kRightBrace) {
          block->InsertLast(module_import_result.value());
        }
      } else {
        auto stmt_list_result = ParseStatementListItem(false, false, false, false);
        SKIP_TOKEN_OR(stmt_list_result, success, TokenKind::kRightBrace) {
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
  SYNTAX_ERROR("'{' expected.", cur_token());
}


template <typename UCharInputIterator>
ParseResult Parser<UCharInputIterator>::ParseExportDeclaration() {
  LOG_PHASE(ParseExportDeclaration);
  if (cur_token()->type() == TokenKind::kExport) {

    Token info = *cur_token();
    Next();
    if (cur_token()->type() == TokenKind::kMul) {
      Next();
      auto from_clause_result = ParseFromClause();
      CHECK_AST(from_clause_result);
      return Success(CreateExportView(ir::Node::Null(), from_clause_result.value(), &info, false));
    }

    switch (cur_token()->type()) {
      case TokenKind::kLeftBrace: {
        auto export_clause_result = ParseExportClause();
        CHECK_AST(export_clause_result);
        if (cur_token()->type() == TokenKind::kIdentifier &&
            cur_token()->value()->Equals("from")) {
          auto from_clause_result = ParseFromClause();
          CHECK_AST(from_clause_result);
          return Success(CreateExportView(export_clause_result.value(), from_clause_result.value(), &info, false));
        }
        return Success(CreateExportView(export_clause_result.value(), ir::Node::Null(), &info, false));
      }
      case TokenKind::kVar: {
        auto variable_stmt_result = ParseVariableStatement(true, false);
        CHECK_AST(variable_stmt_result);
        return Success(CreateExportView(variable_stmt_result.value(), ir::Node::Null(), &info, false));
      }
      case TokenKind::kConst:
      case TokenKind::kClass:
      case TokenKind::kInterface:
      case TokenKind::kLet:
      case TokenKind::kFunction:
      case TokenKind::kEnum: {
        auto decl_result = ParseDeclaration(true, true, false);
        CHECK_AST(decl_result);
        return Success(CreateExportView(decl_result.value(), ir::Node::Null(), &info, false));
      }
      case TokenKind::kDefault:
      case TokenKind::kAssign: {
        Next();
        auto assignment_expr_result = ParseAssignmentExpression(true, false);
        CHECK_AST(assignment_expr_result);
        return Success(CreateExportView(assignment_expr_result.value(), ir::Node::Null(), &info, true));
      }
      default:
        if (cur_token()->type() == TokenKind::kIdentifier &&
            cur_token()->value()->Equals("declare")) {
          auto ambient_decl_result = ParseAmbientDeclaration(true);
          CHECK_AST(ambient_decl_result);
          return Success(CreateExportView(ambient_decl_result.value(), ir::Node::Null(), &info, true));
        }
        SYNTAX_ERROR("unexpected token.", cur_token());
    }
  }
  SYNTAX_ERROR("'export' expected.", cur_token());
}


template <typename UCharInputIterator>
Handle<ir::Node> Parser<UCharInputIterator>::CreateExportView(
    Handle<ir::Node> export_clause,
    Handle<ir::Node> from_clause,
    Token* token_info,
    bool default_export) {
  LOG_PHASE(CreateExportView);
  auto export_view = New<ir::ExportView>(default_export, export_clause, from_clause);
  export_view->SetInformationForNode(token_info);
  return export_view;
}


template <typename UCharInputIterator>
ParseResult Parser<UCharInputIterator>::ParseExportClause() {
  LOG_PHASE(ParseExportClause);
  if (cur_token()->type() == TokenKind::kLeftBrace) {
    auto named_export_list = New<ir::NamedExportListView>();
    named_export_list->SetInformationForNode(cur_token());
    Next();

    bool success = true;
    
    while (1) {
      auto identifier_result = ParseIdentifier();
      SKIP_TOKEN_OR(identifier_result, success, TokenKind::kRightBrace) {
        if (cur_token()->type() == TokenKind::kIdentifier &&
            cur_token()->value()->Equals("as")) {
          Next();
          auto binding_identifier_result = ParseIdentifier();
          SKIP_TOKEN_OR(binding_identifier_result, success, TokenKind::kRightBrace) {
            named_export_list->InsertLast(CreateNamedExportView(identifier_result.value(), binding_identifier_result.value()));
          }
        } else {
          named_export_list->InsertLast(CreateNamedExportView(identifier_result.value(), ir::Node::Null()));
        }
      }
      if (cur_token()->type() == TokenKind::kComma) {
        Next();
      } else if (cur_token()->type() == TokenKind::kRightBrace) {
        Next();
        return Success(named_export_list);
      } else {
        SYNTAX_ERROR("',' or '}' expected.", cur_token());
      }
    }
  }
  SYNTAX_ERROR("'{' expected.", cur_token());
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
