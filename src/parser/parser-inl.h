// 
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

// Return next token and replace previous token and current token.
template <typename UCharInputIterator>
TokenInfo* Parser<UCharInputIterator>::Next() {
  if (current_token_info_ != nullptr) {
    prev_token_info_ = *current_token_info_;
  }
  return current_token_info_ = scanner_->Scan();
}


// Return current token.
template <typename UCharInputIterator>
TokenInfo* Parser<UCharInputIterator>::Current() YATSC_NOEXCEPT {
  return current_token_info_;
}



// Return previous token.
template <typename UCharInputIterator>
TokenInfo* Parser<UCharInputIterator>::Prev() YATSC_NOEXCEPT {
  return &prev_token_info_;
}


template <typename UCharInputIterator>
bool Parser<UCharInputIterator>::IsLineTermination() YATSC_NOEXCEPT {
  return Current()->type() == Token::LINE_TERMINATOR ||
    Current()->type() == Token::END_OF_INPUT ||
    (Prev() != nullptr &&
     Prev()->has_line_break_before_next());
}


template <typename UCharInputIterator>
void Parser<UCharInputIterator>::ConsumeLineTerminator() YATSC_NOEXCEPT {
  if (Current()->type() == Token::LINE_TERMINATOR) {
    Next();
  }
}


// Skip all tokens except given token and eof.
template <typename UCharInputIterator>
void Parser<UCharInputIterator>::SkipTokensIfErrorOccured(Token token) YATSC_NOEXCEPT {
  if (token == Token::LINE_TERMINATOR) {
    while (Current()->type() != Token::LINE_TERMINATOR) {
      if (Current()->has_line_break_before_next() ||
          Current()->type() == Token::TS_RIGHT_BRACE) {
        Next();
        break;
      }
      Next();
    }
  } else {
    while (Current()->type() != Token::END_OF_INPUT &&
           Current()->type() != token) {
      Next();
    }
  }
}


template <typename UCharInputIterator>
void Parser<UCharInputIterator>::Initialize() YATSC_NOEXCEPT {
  unsafe_zone_allocator_(sizeof(Parsed) * 10);
    
  scanner_->SetReferencePathCallback([&](const Literal* path){
    String dir = Path::Dirname(module_info_->module_name());
    Notify("Parser::ModuleFound", Path::Join(dir, path->utf8_value()));
  });

  scanner_->SetErrorCallback([&](const char* message, const SourcePosition& source_position) {
    module_info_->semantic_error()->SyntaxError(source_position) << message;
  });

  set_current_scope(NewScope());
    
  Next();
}


template <typename UCharInputIterator>
typename Parser<UCharInputIterator>::RecordedParserState Parser<UCharInputIterator>::parser_state() YATSC_NOEXCEPT {
  TokenInfo prev;
  TokenInfo current;
  Handle<ir::Scope> scope;
  if (Prev() != nullptr) {
    prev = *Prev();
  }
  if (Current() != nullptr) {
    current = *Current();
  }
  if (scope_) {
    scope = scope_;
  }
    
  return RecordedParserState(scanner_->char_position(), current, prev, scope, module_info_->semantic_error()->size());
}



template <typename UCharInputIterator>
void Parser<UCharInputIterator>::RestoreParserState(const RecordedParserState& rps) YATSC_NOEXCEPT {
  scanner_->RestoreScannerPosition(rps.rcp());
  *current_token_info_ = rps.current();
  prev_token_info_ = rps.prev();
  scope_ = rps.scope();
  Handle<SemanticError> se = module_info_->semantic_error();
  if (se->size() != rps.error_count()) {
    size_t diff = rps.error_count() - se->size();
    if (diff > se->size()) {
      diff = se->size();
    }
    for (size_t i = 0; i < diff; i++) {
      se->Pop();
    }
  }
}
}

