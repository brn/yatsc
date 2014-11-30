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
Token* Parser<UCharInputIterator>::Next() {
  if (current_token_info_ != nullptr && !current_token_info_->Is(TokenKind::kEof)) {
    prev_token_info_ = *current_token_info_;
  }
  current_token_info_ = scanner_->Scan();
  
  if (current_token_info_->Is(TokenKind::kIllegal)) {
    while (current_token_info_->Is(TokenKind::kIllegal)) {
      current_token_info_ = scanner_->Scan();
    }
  }

  return current_token_info_;
}


// Return current token.
template <typename UCharInputIterator>
Token* Parser<UCharInputIterator>::cur_token() YATSC_NOEXCEPT {
  return current_token_info_;
}



// Return previous token.
template <typename UCharInputIterator>
Token* Parser<UCharInputIterator>::prev_token() YATSC_NOEXCEPT {
  return &prev_token_info_;
}


template <typename UCharInputIterator>
bool Parser<UCharInputIterator>::IsLineTermination() YATSC_NOEXCEPT {
  return cur_token()->type() == TokenKind::kLineTerminator ||
    cur_token()->type() == TokenKind::kEof ||
    (prev_token() != nullptr &&
     prev_token()->has_line_break_before_next());
}


template <typename UCharInputIterator>
void Parser<UCharInputIterator>::ConsumeLineTerminator() YATSC_NOEXCEPT {
  if (cur_token()->type() == TokenKind::kLineTerminator) {
    Next();
  }
}


// Skip all tokens except given token and eof.
template <typename UCharInputIterator>
void Parser<UCharInputIterator>::SkipTokensUntil(std::initializer_list<TokenKind> kinds, bool move_to_next_token) YATSC_NOEXCEPT {
  while (!cur_token()->Is(TokenKind::kEof)) {
    for (auto kind: kinds) {
      if (kind == TokenKind::kLineTerminator) {
        if (cur_token()->has_line_break_before_next()) {
          goto END;
        }
      }
      if (cur_token()->Is(kind)) {
        goto END;
      }
    }
    Next();
  }

END:

  if (cur_token()->Is(TokenKind::kEof) && !IsInRecordMode()) {
    throw FatalParseError();
  }
  
  if (move_to_next_token) {
    Next();
  }
}


template <typename UCharInputIterator>
void Parser<UCharInputIterator>::SkipToNextStatement() YATSC_NOEXCEPT {
  while (!cur_token()->Is(TokenKind::kEof)) {
    if (cur_token()->OneOf({
          TokenKind::kLeftBrace,
            TokenKind::kLineTerminator,
            TokenKind::kIf,
            TokenKind::kFor,
            TokenKind::kWhile,
            TokenKind::kDo,
            TokenKind::kContinue,
            TokenKind::kBreak,
            TokenKind::kReturn,
            TokenKind::kWith,
            TokenKind::kSwitch,
            TokenKind::kThrow,
            TokenKind::kDebugger,
            TokenKind::kVar,
            TokenKind::kFunction,
            TokenKind::kClass,
            TokenKind::kEnum,
            TokenKind::kInterface,
            TokenKind::kLet,
            TokenKind::kConst,
            TokenKind::kImport,
            TokenKind::kExport,
            TokenKind::kModule
        })) {
      break;
    } else if (cur_token()->Is(TokenKind::kIdentifier) &&
               (cur_token()->value()->Equals("declare") ||
                cur_token()->value()->Equals("module"))) {
      break;
    }
    Next();
  }

  if (cur_token()->Is(TokenKind::kEof) && !IsInRecordMode()) {
    throw FatalParseError();
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
    module_info_->error_reporter()->SyntaxError(source_position) << message;
  });

  set_current_scope(NewScope());
    
  Next();
}


template <typename UCharInputIterator>
typename Parser<UCharInputIterator>::RecordedParserState Parser<UCharInputIterator>::parser_state() YATSC_NOEXCEPT {
  EnterRecordMode();
  
  Token prev;
  Token current;
  Handle<ir::Scope> scope;
  if (prev_token() != nullptr) {
    prev = *prev_token();
  }
  if (cur_token() != nullptr) {
    current = *cur_token();
  }
  if (scope_) {
    scope = scope_;
  }
    
  return RecordedParserState(
      scanner_->char_position(),
      current,
      prev,
      scope,
      enclosure_balancer_,
      module_info_->error_reporter()->size());
}



template <typename UCharInputIterator>
void Parser<UCharInputIterator>::RestoreParserState(const RecordedParserState& rps) YATSC_NOEXCEPT {
  ExitRecordMode();
  
  scanner_->RestoreScannerPosition(rps.rcp());
  *current_token_info_ = rps.current();
  prev_token_info_ = rps.prev();
  scope_ = rps.scope();
  enclosure_balancer_ = rps.enclosure_balancer();
  Handle<ErrorReporter> se = module_info_->error_reporter();
  
  if (se->size() != rps.error_count()) {
    int diff = abs(static_cast<int>(rps.error_count()) - static_cast<int>(se->size()));
    for (int i = 0; i < diff; i++) {
      se->Pop();
    }
  }
}


template<typename UCharInputIterator>
void Parser<UCharInputIterator>::BalanceEnclosureIfNotBalanced(Token* token, TokenKind kind, bool move_to_next_token) {
  int difference = 1;

  switch (kind) {
    case TokenKind::kRightBrace: {
      enclosure_balancer_.BalanceBrace();
      difference = enclosure_balancer_.brace_difference();
      break;
    }
    case TokenKind::kRightParen: {
      enclosure_balancer_.BalanceParen();
      difference = enclosure_balancer_.paren_difference();
      break;
    }
    case TokenKind::kRightBracket: {
      enclosure_balancer_.BalanceBracket();
      difference = enclosure_balancer_.bracket_difference();
      break;
    }
    default:
      ;
  }
  
  if (difference < 0) {
    switch (kind) {
      case TokenKind::kRightBrace: {
        ReportParseError(token, YATSC_SOURCEINFO_ARGS)
          << "extra '}' found.";
        break;
      }
        
      case TokenKind::kRightParen: {
        ReportParseError(token, YATSC_SOURCEINFO_ARGS)
          << "extra ')' found.";
        break;
      }
        
      case TokenKind::kRightBracket: {
        ReportParseError(token, YATSC_SOURCEINFO_ARGS)
          << "extra ']' found.";
        break;
      }
        
      default:
        ;
    }
    return;
  }
  
  while (difference != 0 &&
         !cur_token()->Is(TokenKind::kEof)) {
    if (cur_token()->Is(kind)) {
      difference--;
    }
    Next();
  }

  if (cur_token()->Is(TokenKind::kEof) && !IsInRecordMode()) {
    throw FatalParseError();
  }

  if (move_to_next_token) {
    Next();
  }
}


template <typename UCharInputIterator>
template <typename T>
Handle<ErrorDescriptor> Parser<UCharInputIterator>::ReportParseError(T item, const char* filename, int line) {
  return SyntaxErrorBuilder<DEBUG_BOOL>::Build(module_info_, item, filename, line);
}


template <typename UCharInputIterator>
template <typename T>
Handle<ErrorDescriptor> Parser<UCharInputIterator>::ReportParseWarning(T item, const char* filename, int line) {
  return SyntaxErrorBuilder<DEBUG_BOOL>::BuildWarning(module_info_, item, filename, line);
}


template <typename UCharInputIterator>
template <typename T>
void Parser<UCharInputIterator>::UnexpectedEndOfInput(T item, const char* filename, int line) {
  SyntaxErrorBuilder<DEBUG_BOOL>::Build(module_info_, item, filename, line) << "Unexpected end of input.";
  if (!IsInRecordMode()) {
    throw FatalParseError();
  }
}


template <typename UCharInputIterator>
void Parser<UCharInputIterator>::SkipIllegalTokens() {
  while (cur_token()->Is(TokenKind::kIllegal)) {Next();}
  if (cur_token()->Is(TokenKind::kEof) && !IsInRecordMode()) {
    throw FatalParseError();
  }
}
}
