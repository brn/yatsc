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

#ifndef YATSC_PARSER_PARSER_H
#define YATSC_PARSER_PARSER_H

#include <sstream>
#include "../utils/stl.h"
#include "./parser-base.h"
#include "../compiler-option.h"
#include "../utils/notificator.h"
#include "../compiler/module-info.h"
#include "../utils/path.h"
#include "../utils/maybe.h"
#include "./parser-state.h"


namespace yatsc {


#define SKIP_IF_ERROR_RECOVERY_ENABLED(advance_to_next, ...)  \
  if (IsErrorRecorveryEnabled()) {                            \
    SkipTokensUntil({__VA_ARGS__}, advance_to_next);          \
  } else {                                                    \
    return Failed();                                          \
  }


// Generate SyntaxError and push it to buffer.
// This macro return Failed() result.
// Usage. SYNTAX_ERROR("test " << message, cur_token())
#define SYNTAX_ERROR(message, item)             \
  SYNTAX_ERROR_INTERNAL(message, item)


// Generate SyntaxError and push it to buffer.
// This method do not return.
// Usage. SYNTAX_ERROR("test " << message, cur_token())
#define SYNTAX_ERROR_NO_RETURN(message, item)     \
  SYNTAX_ERROR_INTERNAL_NO_RETURN(message, item)


// Generate SyntaxError and push it to buffer.
// This method execute given expr.
// Usage. SYNTAX_ERROR("test " << message, cur_token())
#define SYNTAX_ERROR_AND(message, item, expr)     \
  SYNTAX_ERROR_INTERNAL_NO_RETURN(message, item); \
  expr


// Generate SyntaxError and skip token until given token found.
// This method return Failed() result.
#define SYNTAX_ERROR_AND_SKIP(message, item, token) \
  SYNTAX_ERROR_NO_RETURN(message, item);            \
  SkipTokensUntil({token}, false);                  \
  return Failed()


// Generate SyntaxError and skip token until given token found.
// This method return Failed() result.
#define SYNTAX_ERROR_AND_SKIP_NEXT(message, item, token)  \
  SYNTAX_ERROR_NO_RETURN(message, item);                  \
  SkipTokensUntil({token}, false);                        \
  Next();                                                 \
  return Failed()


// Check prase result, if parse has failed, return Failed(ir::Node).
#define CHECK_AST(result) if (!result) {return Failed();}


// Skip token until given token found if parse has failed, and if parse has not failed, goto next block.
// Usage. SKIP_TOKEN_OR(result, flag, TokenKind::kLineTerminator) {...}
#define SKIP_TOKEN_OR(result, flag, token)                          \
  if (!result) {flag = false;SkipTokensUntil({token}, false);} else


// Skip token until given token found if parse has failed.
// Usage. SKIP_TOKEN_IF(result, flag, TokenKind::kLineTerminator).
#define SKIP_TOKEN_IF(result, flag, token)          \
  SKIP_TOKEN_OR(result, flag, token) {flag = true;}


// Skip token until given token found if parse has failed.
// This method execute given expr.
// Usage. SKIP_TOKEN_AND(result, flag, TokenKind::kLineTerminator, return Failed()).
#define SKIP_TOKEN_IF_AND(result, flag, token, expr)  \
  if (!result) {                                      \
    flag = false;                                     \
    SkipTokensUntil({token}, false);                  \
    expr;                                             \
  } else {flag = true;}


// Push error to the buffer and return Failed() result.
#define SYNTAX_ERROR_INTERNAL(message, item)    \
  REPORT_SYNTAX_ERROR_INTERNAL(message, item);  \
  return Failed()


// Push error to the buffer.
#define SYNTAX_ERROR_INTERNAL_NO_RETURN(message, item)  \
  REPORT_SYNTAX_ERROR_INTERNAL(message, item)


#ifndef DEBUG

// Push error to the buffer.
#define REPORT_SYNTAX_ERROR_INTERNAL(message, item)             \
  module_info_->error_reporter()->SyntaxError(item) << message;

#else

// Push error to the buffer.
// This method debug only.
#define REPORT_SYNTAX_ERROR_INTERNAL(message, item)                     \
  module_info_->error_reporter()->SyntaxError(item) << message << '\n' << __FILE__ << ":" << __LINE__;
#endif


#ifdef DEBUG
// Logging current parse phase.
#define LOG_PHASE(name)                                                 \
  if (cur_token() != nullptr) {                                         \
    phase_buffer_ << indent_ << "Enter " << #name << ": CurrentToken = " << cur_token()->ToStringWithValue() << ",generic?[" << scanner_->nested_generic_count() << ']'; \
  } else {                                                              \
    phase_buffer_ << indent_ << "Enter " << #name << ": CurrentToken = null,generic?[" << scanner_->nested_generic_count() << ']'; \
  }                                                                     \
  phase_buffer_ << cur_token()->source_position().start_line_number() << '\n'; \
  indent_ += "  ";                                                      \
  auto err_size = module_info_->error_reporter()->size();               \
  YATSC_SCOPED([&]{                                                     \
    indent_ = indent_.substr(0, indent_.size() - 2);                    \
    if (this->cur_token() != nullptr) {                                 \
      phase_buffer_ << indent_ << "Exit " << #name << ": CurrentToken = " << cur_token()->ToStringWithValue() << ",generic?[" << scanner_->nested_generic_count() << "]" << (err_size != module_info_->error_reporter()->size()? "[Error!]": ""); \
    } else {                                                            \
      phase_buffer_ << indent_ << "Exit " << #name << ": CurrentToken = null,generic?[" << scanner_->nested_generic_count() << "]"<< (err_size != module_info_->error_reporter()->size()? "[Error!]": ""); \
    }                                                                   \
    phase_buffer_ << cur_token()->source_position().start_line_number() << '\n'; \
  })
#else
// Disabled.
#define LOG_PHASE(name)
#endif


typedef Maybe<Handle<ir::Node>> ParseResult;


class FatalParseError: std::exception {
 public:
  FatalParseError() = default;
};


template <typename UCharInputSourceIterator>
class Parser: public ParserBase {
  
 public:
  
  Parser(const CompilerOption& co,
         Scanner<UCharInputSourceIterator>* scanner,
         const Notificator<void(const String&)>& notificator,
         Handle<ModuleInfo> module_info,
         Handle<ir::GlobalScope> global_scope)
      : ParserBase(co, notificator),
        record_mode_(0),
        error_recovery_(1),
        scanner_(scanner),
        module_info_(module_info),
        global_scope_(global_scope) {Initialize();}

  ParseResult Parse() {
    if (module_info_->IsDefinitionFile()) {
      return ParseDeclarationModule();
    } else {
      return ParseModule();
    }
  }

 private:  
  
  /**
   * Return a next Token.
   * @return Next Token.
   */
  YATSC_INLINE Token* Next();


  /**
   * Return current Token.
   * @return Current Token.
   */
  YATSC_INLINE Token* cur_token() YATSC_NOEXCEPT;


  YATSC_INLINE Token* prev_token() YATSC_NOEXCEPT;


  struct AccessorType {
    AccessorType(bool setter, bool getter, const Token& info)
        : setter(setter), getter(getter), token_info(info) {}
    bool setter;
    bool getter;
    Token token_info;
  };


  class EnclosureBalancer {
   public:
    EnclosureBalancer()
        : open_brace_count_(0),
          open_bracket_count_(0),
          open_paren_count_(0),
          close_brace_count_(0),
          close_bracket_count_(0),
          close_paren_count_(0){}


    void OpenBraceFound() {open_brace_count_++;}

    
    void CloseBraceFound() {close_brace_count_--;}

    
    void OpenBracketFound() {open_bracket_count_++;}

    
    void CloseBracketFound() {close_bracket_count_--;}

    
    void OpenParenFound() {open_paren_count_++;}

    
    void CloseParenFound() {close_paren_count_++;}

    
    bool IsBraceNotClose(long val) const {return open_brace_count_ > close_brace_count_;}

    
    bool IsBracketNotClose(long val) const {return open_bracket_count_ > close_brace_count_;}

    
    bool IsParenNotClose(long val) const {return open_paren_count_ > close_paren_count_;}

    
    int brace_difference() const {return open_brace_count_ - close_brace_count_;}


    int bracket_difference() const {return open_bracket_count_ - close_bracket_count_;}

    
    int paren_difference() const {return open_paren_count_ - close_paren_count_;}


    void BalanceBrace() {open_brace_count_ = close_brace_count_;}


    void BalanceParen() {open_paren_count_ = close_paren_count_;}


    void BalanceBracket() {open_bracket_count_ = close_bracket_count_;}
    
   private:
    int open_brace_count_;
    int open_bracket_count_;
    int open_paren_count_;
    int close_brace_count_;
    int close_bracket_count_;
    int close_paren_count_;
  };
  

  class RecordedParserState {
   public:
    RecordedParserState(const typename Scanner<UCharInputSourceIterator>::RecordedCharPosition& rcp,
                        const Token& current,
                        const Token& prev,
                        Handle<ir::Scope> current_scope,
                        EnclosureBalancer enclosure_balancer,
                        const ParserState& state,
                        size_t error_count)
        : rcp_(rcp),
          current_(current),
          prev_(prev),
          scope_(current_scope),
          enclosure_balancer_(enclosure_balancer),
          state_(state),
          error_count_(error_count) {}

    YATSC_CONST_GETTER(typename Scanner<UCharInputSourceIterator>::RecordedCharPosition, rcp, rcp_)


    YATSC_CONST_GETTER(const Token&, current, current_)


    YATSC_CONST_GETTER(const Token&, prev, prev_)


    YATSC_CONST_GETTER(Handle<ir::Scope>, scope, scope_)

    
    YATSC_CONST_GETTER(const EnclosureBalancer&, enclosure_balancer, enclosure_balancer_)


    YATSC_CONST_GETTER(const ParserState&, state, state_)

    
    YATSC_CONST_GETTER(size_t, error_count, error_count_)

   private:
    typename Scanner<UCharInputSourceIterator>::RecordedCharPosition rcp_;
    Token current_;
    Token prev_;
    Handle<ir::Scope> scope_;
    EnclosureBalancer enclosure_balancer_;
    ParserState state_;
    size_t error_count_;
  };


  class Parsed: public RbTreeNode<SourcePosition, Parsed*> {
   public:
    explicit Parsed(ParseResult parse_result, RecordedParserState rps)
        : RbTreeNode<SourcePosition, Parsed*>(),
        parse_result_(parse_result),
        parser_state_(rps) {}

    YATSC_GETTER(ParseResult, parse_result, parse_result_)


    YATSC_GETTER(RecordedParserState, parser_state, parser_state_)
    
   private:
    ParseResult parse_result_;
    RecordedParserState parser_state_;
  };


  template <bool Print>
  class DebugStream {
   public:

    template <typename T>
    DebugStream& operator << (T value) {
      //buffer_ << value;
      if (Print) {
        std::cout << value;
      }
      return *this;
    }


    void PrintStackTrace() {
      printf("%s\n", buffer_.str().c_str());
    }
  
   private:
    StringStream buffer_;
  };


  template <bool cplusplus_info>
  class SyntaxErrorBuilder: private Static {
   public:
    template <typename T>
    static ErrorDescriptor& Build(Handle<ModuleInfo> module_info, T item, const char* filename, int line) {
      if (cplusplus_info) {
        return module_info->error_reporter()->SyntaxError(item) << filename << ":" << line << "\n";
      }
      return module_info->error_reporter()->SyntaxError(item);
    }

    static ErrorDescriptor& Build(Handle<ModuleInfo> module_info) {
      return module_info->error_reporter()->Ignore();
    }


    template <typename T>
    static ErrorDescriptor& BuildWarning(Handle<ModuleInfo> module_info, T item, const char* filename, int line) {
      if (cplusplus_info) {
        return module_info->error_reporter()->Warning(item) << filename << ":" << line << "\n";
      }
      return module_info->error_reporter()->Warning(item);
    }
  };


  void Initialize() YATSC_NOEXCEPT;
  

  RecordedParserState parser_state() YATSC_NOEXCEPT;

  
  void RestoreParserState(const RecordedParserState& rps) YATSC_NOEXCEPT;
  

  ParseResult& Memoize(const SourcePosition& sp, ParseResult& result) YATSC_NOEXCEPT {
    auto record = this->unsafe_zone_allocator_->template New<Parsed>(result, parser_state());
    memo_.Insert(sp, record);
    return result;
  }


  Parsed* GetMemoizedRecord(const SourcePosition& sp) YATSC_NOEXCEPT {
    return memo_.Find(sp);
  }


  YATSC_INLINE void Declare(Handle<ir::Node> node) {
    scope_->Declare(node);
  }


  YATSC_PROPERTY(Handle<ir::Scope>, current_scope, scope_)


  Handle<ir::Scope> NewScope() {return Heap::NewHandle<ir::Scope>(current_scope(), global_scope_);}


  void EnableErrorRecovery() YATSC_NOEXCEPT {error_recovery_++;}


  void DisableErrorRecovery() YATSC_NOEXCEPT {error_recovery_--;}
  

  bool IsErrorRecorveryEnabled() YATSC_NO_SE {return error_recovery_ > 0;}

  
  void SkipTokensUntil(std::initializer_list<TokenKind> kinds, bool move_to_next_token);


  YATSC_INLINE ParseResult Success(Handle<ir::Node> result) YATSC_NOEXCEPT {
    state_.ExitErrorRecovery();
    return Just(result);
  }
  

  YATSC_INLINE ParseResult Failed() YATSC_NO_SE {return Nothing<Handle<ir::Node>>();}


  YATSC_INLINE Handle<ir::Node> Null() const {return ir::Node::Null();}


  void BalanceEnclosureIfNotBalanced(Token* token, TokenKind kind, bool move_to_next_token);


  YATSC_INLINE void OpenBraceFound() YATSC_NOEXCEPT {enclosure_balancer_.OpenBraceFound();};


  YATSC_INLINE void CloseBraceFound() YATSC_NOEXCEPT {enclosure_balancer_.CloseBraceFound();}


  YATSC_INLINE void OpenBracketFound() YATSC_NOEXCEPT {enclosure_balancer_.OpenBracketFound();}

  
  YATSC_INLINE void CloseBracketFound() YATSC_NOEXCEPT {enclosure_balancer_.CloseBracketFound();}

  
  YATSC_INLINE void OpenParenFound() YATSC_NOEXCEPT {enclosure_balancer_.OpenParenFound();}


  YATSC_INLINE void CloseParenFound() YATSC_NOEXCEPT {enclosure_balancer_.CloseParenFound();}


  YATSC_INLINE int brace_difference() YATSC_NO_SE {return enclosure_balancer_.brace_difference();}

  
  YATSC_INLINE int bracket_difference() YATSC_NO_SE {return enclosure_balancer_.bracket_difference();}


  YATSC_INLINE int paren_difference() YATSC_NO_SE {return enclosure_balancer_.paren_difference();}


  YATSC_INLINE bool IsInRecordMode() YATSC_NO_SE {return record_mode_ != 0;}


  YATSC_INLINE void EnterRecordMode() {record_mode_++;}


  YATSC_INLINE void ExitRecordMode() {record_mode_--;}


  template <typename T>
  ErrorDescriptor& ReportParseError(T item, const char* filename, int line);


  template <typename T>
  ErrorDescriptor& ReportParseWarning(T item, const char* filename, int line);

  
  template <typename T>
  void UnexpectedEndOfInput(T item, const char* filename, int line);


  void SkipIllegalTokens();


  void TryConsume(TokenKind kind) {if (cur_token()->Is(kind)) {Next();}}


  void CheckEof(const char* filename, int line) {if (cur_token()->Is(TokenKind::kEof)) {UnexpectedEndOfInput(&prev_token_info_, filename, line);}};


  void SkipToNextStatement();  

  int record_mode_;
  int error_recovery_;
#if defined(DEBUG) || defined(UNIT_TEST)
  DebugStream<false> phase_buffer_;
#endif
  Scanner<UCharInputSourceIterator>* scanner_;
  Handle<ModuleInfo> module_info_;
  Handle<ir::Scope> scope_;
  LazyInitializer<UnsafeZoneAllocator> unsafe_zone_allocator_;
  IntrusiveRbTree<SourcePosition, Parsed*> memo_;
  Handle<ir::GlobalScope> global_scope_;
  EnclosureBalancer enclosure_balancer_;
  ParserState state_;
  

 VISIBLE_FOR_TESTING:

  ParseResult ParseStatementListItem();

  ParseResult ParseStatementList();

  ParseResult ParseStatement();

  ParseResult ParseBlockStatement();

  ParseResult ParseModuleStatement();

  ParseResult ParseImportStatement();

  ParseResult ParseExportStatement();

  ParseResult ParseDeclaration(bool error);
  
  ParseResult ParseDebuggerStatement();

  ParseResult ParseLexicalDeclaration();

  ParseResult ParseLexicalBinding(bool const_decl);

  ParseResult ParseBindingPattern();

  ParseResult ParseObjectBindingPattern();

  ParseResult ParseArrayBindingPattern();

  ParseResult ParseBindingProperty();

  ParseResult ParseBindingElement();

  ParseResult ParseBindingIdentifier();

  ParseResult ParseVariableStatement();
  
  ParseResult ParseVariableDeclaration();

  ParseResult ParseIfStatement();

  ParseResult ParseWhileStatement();

  ParseResult ParseDoWhileStatement();

  ParseResult ParseForStatement();

  ParseResult ParseForIteration(Handle<ir::Node> reciever, Token*);

  ParseResult ParseIterationBody();

  ParseResult ParseContinueStatement();

  ParseResult ParseBreakStatement();

  ParseResult ParseReturnStatement();

  ParseResult ParseWithStatement();

  ParseResult ParseSwitchStatement();

  ParseResult ParseCaseClauses();

  ParseResult ParseLabelledStatement();

  ParseResult ParseLabelledItem();

  ParseResult ParseThrowStatement();

  ParseResult ParseTryStatement();

  ParseResult ParseCatchBlock();

  ParseResult ParseFinallyBlock();

  ParseResult ParseInterfaceDeclaration();

  ParseResult ParseEnumDeclaration();

  ParseResult ParseEnumBody();

  ParseResult ParseEnumProperty();

  Handle<ir::Node> CreateEnumFieldView(Handle<ir::Node> name, Handle<ir::Node> value);
  
  ParseResult ParseClassDeclaration();

  ParseResult ParseClassBases();

  ParseResult ParseClassBody();

  ParseResult ParseClassElement();

  ParseResult ParseFieldModifiers();
  
  ParseResult ParseFieldModifier();

  bool IsAccessLevelModifier(Token* token) {return token->OneOf({TokenKind::kPublic, TokenKind::kProtected, TokenKind::kPrivate});};

  ParseResult ParseConstructorOverloads(Handle<ir::Node> mods);

  ParseResult ParseConstructorOverloadOrImplementation(bool first, Handle<ir::Node> mods, Handle<ir::Node> overloads);

  bool IsMemberFunctionOverloadsBegin(Token* info);
  
  ParseResult ParseMemberFunctionOverloads(Handle<ir::Node> mods, AccessorType* at);

  ParseResult ParseMemberFunctionOverloadOrImplementation(
      bool first, Handle<ir::Node> mods, AccessorType* at, Handle<ir::Node> overloads);

  ParseResult ParseGeneratorMethodOverloads(Handle<ir::Node> mods);

  ParseResult ParseGeneratorMethodOverloadOrImplementation(bool first, Handle<ir::Node> mods, Handle<ir::Node> overloads);

  ParseResult ParseMemberVariable(Handle<ir::Node> mods);

  ParseResult ParseFunctionOverloads(bool declaration, bool is_export);

  ParseResult ParseFunctionOverloadOrImplementation(Handle<ir::Node> overloads, bool declaration);
 
  ParseResult ParseParameterList(bool accesslevel_allowed);
  
  ParseResult ParseParameter(bool rest, bool accesslevel_allowed);

  ParseResult ParseFunctionBody(bool generator);

  ParseResult ParseTypeExpression();

  ParseResult ParseType();

  ParseResult ParseReferencedType();

  ParseResult ParseGenericType();

  ParseResult ParseTypeArguments();

  ParseResult ParseTypeParameters();

  ParseResult ParseTypeParameter();

  ParseResult ParseTypeQueryExpression();

  ParseResult ParseArrayType(Handle<ir::Node> type_expr);

  ParseResult ParseObjectTypeExpression();

  ParseResult ParseObjectTypeElement();

  ParseResult ParseCallSignature(bool accesslevel_allowed, bool type_annotation, bool arrow_glyph_expected);

  ParseResult ParseIndexSignature();

  // Parse expression.
  ParseResult ParseCoveredExpression();

  // Parse expression.
  ParseResult ParseAsArrowFunction(Handle<ir::Node> type_list, const ir::Node::List&, Handle<ir::Node> ret_type);

  // Parse expression.
  ParseResult ParseAsTypeAssertion(Handle<ir::Node> type_list, const ir::Node::List&);

  // Parse expression.
  ParseResult ParseAsExpression(const ir::Node::List&);
  
  // Parse expression.
  ParseResult ParseExpression();

  // Parse destructuring assignment.
  ParseResult ParseAssignmentPattern();

  // Parse destructuring assignment object pattern.
  ParseResult ParseObjectAssignmentPattern();

  // Parse destructuring assignment array pattern.
  // To simplify, we parse AssignmentElementList together.
  ParseResult ParseArrayAssignmentPattern();

  // Parse destructuring assignment object pattern properties.
  ParseResult ParseAssignmentPropertyList();

  // Parse destructuring assignment object pattern property.
  ParseResult ParseAssignmentProperty();

  // Parse destructuring assignment array pattern element.
  ParseResult ParseAssignmentElement();

  // Parse destructuring assignment array pattern rest element.
  ParseResult ParseAssignmentRestElement();

  // Parse destructuring assignment target node.
  ParseResult ParseDestructuringAssignmentTarget();

  // Parse assignment expression.
  ParseResult ParseAssignmentExpression();

  ParseResult ParseArrowFunction(Handle<ir::Node> identifier);

  ParseResult ParseArrowFunctionParameters(Handle<ir::Node> identifier);

  ParseResult ParseConciseBody(Handle<ir::Node> call_sig);

  // Parse conditional expression.
  ParseResult ParseConditionalExpression();

#define DEF_PARSE_BINARY_EXPR(name)                         \
  ParseResult Parse##name##Expression();

  DEF_PARSE_BINARY_EXPR(LogicalOR);
  DEF_PARSE_BINARY_EXPR(LogicalAND);
  DEF_PARSE_BINARY_EXPR(BitwiseOR);
  DEF_PARSE_BINARY_EXPR(BitwiseXOR);
  DEF_PARSE_BINARY_EXPR(BitwiseAND);
  DEF_PARSE_BINARY_EXPR(Equality);
  DEF_PARSE_BINARY_EXPR(Relational);
  DEF_PARSE_BINARY_EXPR(Shift);
  DEF_PARSE_BINARY_EXPR(Additive);
  DEF_PARSE_BINARY_EXPR(Multiplicative);
#undef DEF_PARSE_BINARY_EXPR

  // Parse unary expression.
  ParseResult ParseUnaryExpression();

  // Parse postfix expression.
  ParseResult ParsePostfixExpression();

  ParseResult ParseLeftHandSideExpression();

  // Parse new expression.
  ParseResult ParseNewExpression();
  
  // Parse member expression.
  ParseResult ParseMemberExpression();

  // Parser getprop or getelem expression.
  ParseResult ParseGetPropOrElem(Handle<ir::Node> node, bool dot_only, bool is_error);

  ParseResult ParseCallExpression();

  ParseResult BuildArguments(ParseResult type_arguments_result, Handle<ir::Node> args);
  
  ParseResult ParseArguments();

  ParseResult ParsePrimaryExpression();

  ParseResult ParseArrayLiteral();

  ParseResult ParseSpreadElement();

  ParseResult ParseArrayComprehension();

  ParseResult ParseComprehension(bool generator);

  ParseResult ParseComprehensionTail();

  ParseResult ParseComprehensionFor();

  ParseResult ParseComprehensionIf();

  ParseResult ParseGeneratorComprehension();

  ParseResult ParseYieldExpression();

  ParseResult ParseForBinding();

  ParseResult ParseObjectLiteral();

  ParseResult ParsePropertyDefinition();

  ParseResult ParsePropertyName();

  ParseResult ParseLiteralPropertyName();

  ParseResult ParseComputedPropertyName();

  ParseResult ParseLiteral();

  ParseResult ParseValueLiteral();

  ParseResult ParseArrayInitializer();

  ParseResult ParseIdentifierReference();

  ParseResult ParseLabelIdentifier();

  ParseResult ParseIdentifier();

  ParseResult ParseStringLiteral();

  ParseResult ParseNumericLiteral();

  ParseResult ParseBooleanLiteral();

  ParseResult ParseUndefinedLiteral();

  ParseResult ParseNaNLiteral();

  ParseResult ParseRegularExpression();

  ParseResult ParseTemplateLiteral();

  ParseResult ParseEmptyStatement();

  ParseResult ParseModule();

  ParseResult ParseImportDeclaration();

  ParseResult ParseExternalModuleReference();

  ParseResult ParseImportClause();

  ParseResult ParseNamedImport();

  ParseResult ParseFromClause();

  ParseResult ParseModuleImport();

  ParseResult ParseTSModule(Handle<ir::Node> identifier, Token* token_info);

  ParseResult ParseTSModuleBody();

  ParseResult ParseExportDeclaration();

  Handle<ir::Node> CreateExportView(
      Handle<ir::Node> export_clause,
      Handle<ir::Node> from_clause,
      Token* token_info,
      bool default_export);

  ParseResult ParseExportClause();

  Handle<ir::Node> CreateNamedExportView(
      Handle<ir::Node> identifier,
      Handle<ir::Node> binding);


  // Ambient
  ParseResult ParseDeclarationModule();
  
  ParseResult ParseAmbientDeclaration(bool module_allowed);

  ParseResult ParseAmbientVariableDeclaration(Token* info);

  ParseResult ParseAmbientFunctionDeclaration(Token* info);

  ParseResult ParseAmbientClassDeclaration(Token* info);

  ParseResult ParseAmbientClassBody();

  ParseResult ParseAmbientClassElement();

  ParseResult ParseAmbientConstructor(Handle<ir::Node> mods);

  ParseResult ParseAmbientMemberFunction(Handle<ir::Node> mods, AccessorType* acessor_type);

  ParseResult ParseAmbientGeneratorMethod(Handle<ir::Node> mods);

  ParseResult ParseAmbientMemberVariable(Handle<ir::Node> mods);

  ParseResult ParseAmbientEnumDeclaration(Token* info);

  ParseResult ParseAmbientEnumBody();

  ParseResult ParseAmbientEnumProperty();

  Handle<ir::Node> CreateAmbientEnumFieldView(Handle<ir::Node> name, Handle<ir::Node> value);

  ParseResult ParseAmbientModuleDeclaration(Token* info);

  ParseResult ParseAmbientModuleBody(bool external);

  ParseResult ParseAmbientModuleElement(bool external);
  

  bool IsLineTermination() YATSC_NOEXCEPT;

  void ConsumeLineTerminator() YATSC_NOEXCEPT;

  void EnableNestedGenericTypeScanMode() YATSC_NOEXCEPT {scanner_->EnableNestedGenericTypeScanMode();}

  void DisableNestedGenericTypeScanMode() YATSC_NOEXCEPT {scanner_->DisableNestedGenericTypeScanMode();}

  template <typename T>
  ParseResult TryParse(T fn) {try {return fn();} catch(const FatalParseError e) {return Failed();}}
  
  AccessorType ParseAccessor();

  void ValidateOverload(Handle<ir::MemberFunctionDefinitionView> node, Handle<ir::Node> overloads);

  class TokenPack {
   public:
    TokenPack(Token& current, Token& prev, const typename Scanner<UCharInputSourceIterator>::RecordedCharPosition& rcp)
        : current_token_(current),
          prev_token_(prev),
          rcp_(rcp) {}

    
    YATSC_CONST_GETTER(Token, current_token, current_token_)
    YATSC_CONST_GETTER(Token, prev_token, prev_token_)
    YATSC_CONST_GETTER(typename Scanner<UCharInputSourceIterator>::RecordedCharPosition, rcp, rcp_)

    Token current_token_;
    Token prev_token_;
    typename Scanner<UCharInputSourceIterator>::RecordedCharPosition rcp_;
  };

  TokenPack token_pack() {
    return TokenPack(*current_token_info_, prev_token_info_, scanner_->char_position());
  }

  void RestoreTokens(const TokenPack& token_pack) {
    *current_token_info_ = token_pack.current_token();
    prev_token_info_ = token_pack.prev_token();
    scanner_->RestoreScannerPosition(token_pack.rcp());
  }
  
  
#if defined(UNIT_TEST) || defined(DEBUG)
  void PrintStackTrace() {
    phase_buffer_.PrintStackTrace();
  }
#endif
};
} // yatsc

#include "./parser-inl.h"
#include "./expression-parser-partial.h"
#include "./type-parser-partial.h"
#include "./statement-parser-partial.h"
#include "./module-parser-partial.h"
#include "./ambient-parser-partial.h"

#undef SYNTAX_ERROR
#undef ARROW_PARAMETERS_ERROR
#undef SYNTAX_ERROR_POS
#undef ARROW_PARAMETERS_ERROR_POS
#undef SYNTAX_ERROR_INTERAL
#undef LOG_PHASE

#endif
