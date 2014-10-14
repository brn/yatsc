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
  while (Current()->type() != Token::END_OF_INPUT) {
    if (Current()->type() == Token::TS_IMPORT) {
      ParseImportDeclaration();
    }
  }
}


template <typename UCharInputIterator>
Handle<ir::Node> Parser<UCharInputIterator>::ParseImportDeclaration() {
  if (Current()->type() == Token::TS_IMPORT) {
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
        return New<ir::ImportView>(identifier, ParseExternalModuleReference());
      }
      SetBufferCursorPosition(cursor);
      return ParseImportClause();
    }
  }
}


template <typename UCharInputIterator>
Handle<ir::Node> Parser<UCharInputIterator>::ParseExternalModuleReference() {
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
      break;
    } else {
      SYNTAX_ERROR("SyntaxError ',' or 'from' expected.", Current());
    }
  }
}


template <typename UCharInputIterator>
Handle<ir::Node> Parser<UCharInputIterator>::ParseNamedImport() {
  
}

}
