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
  auto file_scope = New<ir::FileScopeView>();
  
  while (Current()->type() != Token::END_OF_INPUT) {
    if (Current()->type() == Token::TS_IMPORT) {
      file_scope->InsertLast(ParseImportDeclaration());
    } else if (Current()->type() == Token::TS_IDENTIFIER &&
               Current()->value() == "module") {
      file_scope->InsertLast(ParseModuleImport());
    } else if (Current()->type() == Token::TS_EXPORT) {
      file_scope->InsertLast(ParseExportDeclaration());
    } else {
      file_scope->InsertLast(ParseStatementListItem(false, false, false, false));
    }
  }
  return file_scope;
}


template <typename UCharInputIterator>
Handle<ir::Node> Parser<UCharInputIterator>::ParseImportDeclaration() {
  LOG_PHASE(ParseImportDeclaration);
  if (Current()->type() == Token::TS_IMPORT) {
    TokenCursor begin = GetBufferCursorPosition();
    Next();
    if (Current()->type() == Token::TS_IDENTIFIER) {
      TokenCursor cursor = GetBufferCursorPosition();
      Next();
      if (Current()->type() == Token::TS_ASSIGN) {
        Next();
        TokenCursor back = GetBufferCursorPosition();
        SetBufferCursorPosition(cursor);
        Handle<ir::Node> identifier = ParseIdentifier();
        SetBufferCursorPosition(back);
        auto import_view = New<ir::ImportView>(identifier, ParseExternalModuleReference());
        import_view->SetInformationForNode(PeekBuffer(begin));
        return import_view;
      }
      SetBufferCursorPosition(cursor);
      Handle<ir::Node> import_clause = ParseImportClause();
      Handle<ir::Node> from_clause = ParseFromClause();
      auto import_view = New<ir::ImportView>(import_clause, from_clause);
      import_view->SetInformationForNode(PeekBuffer(begin));
      return import_view;
    } else if (Current()->type() == Token::TS_STRING_LITERAL) {
      Handle<ir::Node> module_specifier = ParseStringLiteral();
      auto import_view = New<ir::ImportView>(ir::Node::Null(), module_specifier);
      import_view->SetInformationForNode(PeekBuffer(begin));
      return import_view;
    }
  }
}


template <typename UCharInputIterator>
Handle<ir::Node> Parser<UCharInputIterator>::ParseExternalModuleReference() {
  LOG_PHASE(ParseExternalModuleReference);
  if (Current()->type() == Token::TS_IDENTIFIER &&
      Current()->value() == "require") {
    Next();
    if (Current()->type() == Token::TS_LEFT_PAREN) {
      Next();
      if (Current()->type() == Token::TS_STRING_LITERAL) {
        TokenCursor cursor = GetBufferCursorPosition();
        Next();
        if (Current()->type() == Token::TS_RIGHT_PAREN) {
          Next();
          return New<ir::ExternalModuleReference>(PeekBuffer(cursor)->value());
        }
        SYNTAX_ERROR("SyntaxError ')' expected.", Current());
      }
      SYNTAX_ERROR("SyntaxError string literal expected.", Current());
    }
    SYNTAX_ERROR("SyntaxError '(' expected.", Current());
  }
  SYNTAX_ERROR("SyntaxError 'require' expected.", Current());
}


template <typename UCharInputIterator>
Handle<ir::Node> Parser<UCharInputIterator>::ParseImportClause() {
  LOG_PHASE(ParseImportClause);
  auto clause = New<ir::ImportListView>();
  while (1) {
    if (Current()->type() == Token::TS_IDENTIFIER) {
      clause->InsertLast(ParseIdentifier());
    } else if (Current()->type() == Token::TS_LEFT_BRACE) {
      clause->InsertLast(ParseNamedImport());
    }
    if (Current()->type() == Token::TS_COMMA) {
      Next();
    } else if (Current()->type() == Token::TS_IDENTIFIER &&
               Current()->value() == "from") {
      return clause;
    } else {
      SYNTAX_ERROR("SyntaxError ',' or 'from' expected.", Current());
    }
  }
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
      if (identifier->HasNameView() &&
          Current()->type() == Token::TS_IDENTIFIER &&
          Current()->value() == "as") {
        Next();
        Handle<ir::Node> binding = ParseBindingIdentifier(false, false);
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
        SYNTAX_ERROR("SyntaxError unexpected token.", Current());
      }
    }
    return named_import_list;
  }
  SYNTAX_ERROR("SyntaxError '{' expected.", Current());
}


template <typename UCharInputIterator>
Handle<ir::Node> Parser<UCharInputIterator>::ParseFromClause() {
  LOG_PHASE(ParseFromClause);
  if (Current()->type() == Token::TS_IDENTIFIER &&
      Current()->value() == "from") {
    TokenCursor cursor = GetBufferCursorPosition();
    Next();
    Handle<ir::Node> module_specifier = ParseStringLiteral();
    module_specifier->SetInformationForNode(PeekBuffer(cursor));
    return module_specifier;
  }
  SYNTAX_ERROR("SyntaxError 'from' expected.", Current());
}


template <typename UCharInputIterator>
Handle<ir::Node> Parser<UCharInputIterator>::ParseModuleImport() {
  LOG_PHASE(ParseModuleImport);
  if (Current()->type() == Token::TS_IDENTIFIER &&
      Current()->value() == "module") {
    TokenCursor cursor = GetBufferCursorPosition();
    Next();
    Handle<ir::Node> binding;
    binding = ParseBindingIdentifier(false, false);
    if (Current()->type() == Token::TS_LEFT_BRACE) {
      SetBufferCursorPosition(cursor);
      return ParseTSModule(); 
    }
    Handle<ir::Node> module_specifier = ParseFromClause();
    auto ret = New<ir::ModuleImportView>(binding, module_specifier);
    ret->SetInformationForNode(PeekBuffer(cursor));
    return ret;
  }
  SYNTAX_ERROR("SyntaxError 'module' expected.", Current());
}


template <typename UCharInputIterator>
Handle<ir::Node> Parser<UCharInputIterator>::ParseTSModule() {
  LOG_PHASE(ParseTSModule);
  if (Current()->type() == Token::TS_IDENTIFIER &&
      Current()->value() == "module") {
    TokenCursor cursor = GetBufferCursorPosition();
    Next();

    Handle<ir::Node> identifier = ParseIdentifier();
    
    if (Current()->type() == Token::TS_LEFT_BRACE) {
      Handle<ir::Node> body = ParseTSModuleBody();
      auto ret = New<ir::ModuleDeclView>(identifier, body);
      ret->SetInformationForNode(Current());
      return ret;
    }
    SYNTAX_ERROR("SyntaxError '{' expected.", Current());
  }
  SYNTAX_ERROR("SyntaxError 'module' expected.", Current());
}


template <typename UCharInputIterator>
Handle<ir::Node> Parser<UCharInputIterator>::ParseTSModuleBody() {
  LOG_PHASE(ParseTSModule);
  if (Current()->type() == Token::TS_LEFT_BRACE) {
    auto block = New<ir::BlockView>();
    block->SetInformationForNode(Current());
    while (Current()->type() != Token::TS_RIGHT_BRACE) {
      if (Current()->type() == Token::TS_EXPORT) {
        switch (Current()->type()) {
          case Token::TS_VAR:
            block->InsertLast(ParseVariableStatement(true, false));
            break;
          case Token::TS_FUNCTION:
            block->InsertLast(ParseFunctionOverloads(false, false, true));
            break;
          case Token::TS_CLASS:
            block->InsertLast(ParseClassDeclaration(false, false));
            break;
          case Token::TS_INTERFACE:
            block->InsertLast(ParseInterfaceDeclaration());
            break;
          case Token::TS_ENUM:
            block->InsertLast(ParseEnumDeclaration(false, false));
            break;
          case Token::TS_IMPORT:
            block->InsertLast(ParseImportDeclaration());
            break;
          default:
            if (Current()->type() == Token::TS_IDENTIFIER &&
                Current()->value() == "module") {
              block->InsertLast(ParseModule());
            } else {
              SYNTAX_ERROR("SyntaxError unexpected token.", Current());
            }
        }
      } else {
        block->InsertLast(ParseStatementListItem(false, false, false, false));
      }
    }
    Next();
    return block;
  }
  SYNTAX_ERROR("SyntaxError '{' expected.", Current());
}


template <typename UCharInputIterator>
Handle<ir::Node> Parser<UCharInputIterator>::ParseExportDeclaration() {
  LOG_PHASE(ParseExportDeclaration);
  if (Current()->type() == Token::TS_EXPORT) {
    TokenCursor cursor = GetBufferCursorPosition();
    Next();
    if (Current()->type() == Token::TS_MUL) {
      Next();
      return CreateExportView(ParseFromClause(), ir::Node::Null(), PeekBuffer(cursor));
    }

    switch (Current()->type()) {
      case Token::TS_LEFT_BRACE: {
        Handle<ir::Node> export_clause = ParseExportClause();
        if (Current()->type() == Token::TS_IDENTIFIER &&
            Current()->value() == "from") {
          return CreateExportView(export_clause, ParseFromClause(), PeekBuffer(cursor));
        }
        return CreateExportView(export_clause, ir::Node::Null(), PeekBuffer(cursor));
      }
      case Token::TS_VAR:
        return CreateExportView(ParseVariableStatement(true, false), ir::Node::Null(), PeekBuffer(cursor));
      case Token::TS_CONST:
      case Token::TS_CLASS:
      case Token::TS_INTERFACE:
      case Token::TS_LET:
        return CreateExportView(ParseDeclaration(true, true, false), ir::Node::Null(), PeekBuffer(cursor));
      case Token::TS_DEFAULT:
      case Token::TS_ASSIGN: {
        Next();
        return CreateExportView(ParseAssignmentExpression(true, false), ir::Node::Null(), PeekBuffer(cursor), true);
      }
      default:
        SYNTAX_ERROR("SyntaxError unexpected token.", Current());
    }
  }
  SYNTAX_ERROR("SyntaxError 'export' expected.", Current());
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
      if (Current()->type() == Token::TS_IDENTIFIER &&
          Current()->value() == "as") {
        Handle<ir::Node> binding = ParseIdentifier();
        named_export_list->InsertLast(CreateNamedExportView(identifier, binding));
      } else {
        named_export_list->InsertLast(CreateNamedExportView(identifier, ir::Node::Null()));
      }
      if (Current()->type() == Token::TS_COMMA) {
        Next();
      } else if (Current()->type() == Token::TS_RIGHT_BRACE) {
        Next();
        return named_export_list;
      } else {
        SYNTAX_ERROR("SyntaxError ',' or '}' expected.", Current());
      }
    }
  }
  SYNTAX_ERROR("SyntaxError '{' expected.", Current());
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
