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

namespace yatsc {


// Generate SyntaxError and throw it.
// Usage. SYNTAX_ERROR("test " << message, Current())
#define SYNTAX_ERROR(message, item)             \
  SYNTAX_ERROR_INTERNAL(message, item)


// Generate SyntaxError and throw it.
// Usage. SYNTAX_ERROR("test " << message, Current())
#define SYNTAX_ERROR_NO_RETURN(message, item)   \
  SYNTAX_ERROR_INTERNAL_NO_RETURN(message, item)


// Generate SyntaxError and throw it.
// Usage. SYNTAX_ERROR("test " << message, Current())
#define SYNTAX_ERROR_AND(message, item, expr)     \
  SYNTAX_ERROR_INTERNAL_NO_RETURN(message, item);  \
  expr


#define CHECK_AST(result)                         \
  if (!result.success()) {return Failed(result.node());}


#define SKIP_TOKEN_OR(result, flag, token)       \
  if (!result.success()) {flag = false;SkipTokensIfErrorOccured(token);} else


#define SKIP_TOKEN_IF(result, flag, token)                   \
  SKIP_TOKEN_OR(result, flag, token) {flag = true;}


#define SKIP_TOKEN_IF_AND(result, flag, token, expr) \
  if (!result.success()) {                        \
    flag = false;                                 \
    SkipTokensIfErrorOccured(token);              \
    expr;                                         \
  } else {flag = true;}


// Throw error and return nullptr.
#define SYNTAX_ERROR_INTERNAL(message, item)    \
  REPORT_SYNTAX_ERROR_INTERNAL(message, item);  \
  return Failed()


#define SYNTAX_ERROR_INTERNAL_NO_RETURN(message, item)  \
  REPORT_SYNTAX_ERROR_INTERNAL(message, item)


#ifndef DEBUG

#define REPORT_SYNTAX_ERROR_INTERNAL(message, item) \
  module_info_->semantic_error()->SyntaxError(item) << message;

#else

#define REPORT_SYNTAX_ERROR_INTERNAL(message, item)               \
  module_info_->semantic_error()->SyntaxError(item) << message << '\n' << __FILE__ << ":" << __LINE__;
#endif


#ifdef DEBUG
// Logging current parse phase.
#define LOG_PHASE(name)                                          \
  if (Current() != nullptr) {                                           \
    phase_buffer_ << indent_ << "Enter " << #name << ": CurrentToken = " << Current()->ToStringWithValue() << ",generic?[" << scanner_->nested_generic_count() << "]\n"; \
  } else {                                                              \
    phase_buffer_ << indent_ << "Enter " << #name << ": CurrentToken = null,generic?[" << scanner_->nested_generic_count() << "]\n"; \
  }                                                                     \
  indent_ += "  ";                                                      \
  YATSC_SCOPED([&]{                                                     \
    indent_ = indent_.substr(0, indent_.size() - 2);                    \
    if (this->Current() != nullptr) {                                   \
      phase_buffer_ << indent_ << "Exit " << #name << ": CurrentToken = " << Current()->ToStringWithValue() << ",generic?[" << scanner_->nested_generic_count() << "]\n"; \
    } else {                                                            \
      phase_buffer_ << indent_ << "Exit " << #name << ": CurrentToken = null,generic?[" << scanner_->nested_generic_count() << "]\n"; \
    }                                                                   \
  })
#else
// Disabled.
#define LOG_PHASE(name)
#endif


class ParseResult {
 public:
  ParseResult()
      : success_(false) {}

  
  ParseResult(Handle<ir::Node> node, bool success)
      : node_(node),
        success_(success) {}

    
  ParseResult(const ParseResult& rs)
      : node_(rs.node_),
        success_(rs.success_) {}


  ParseResult(ParseResult&& rs)
      : node_(std::move(rs.node_)),
        success_(rs.success_) {}

    
  ~ParseResult() = default;
    

  YATSC_GETTER(Handle<ir::Node>, node, node_);

    
  YATSC_GETTER(bool, success, success_);


  YATSC_INLINE operator bool() const {return success_;}
    
    
 private:
  Handle<ir::Node> node_;
  bool success_;
};


template <typename UCharInputSourceIterator>
class Parser: public ParserBase {
  
 public:
  Parser(const CompilerOption& co,
         Scanner<UCharInputSourceIterator>* scanner,
         const Notificator<void(const String&)>& notificator,
         Handle<ModuleInfo> module_info)
      : ParserBase(co, notificator),
        scanner_(scanner),
        module_info_(module_info) {Initialize();}

  Handle<ir::Node> Parse() {
    if (module_info_->IsDefinitionFile()) {
      return ParseDeclarationModule().node();
    } else {
      return ParseModule().node();
    }
  };

 private:
  struct AccessorType {
    AccessorType(bool setter, bool getter, const TokenInfo& info)
        : setter(setter), getter(getter), token_info(info) {}
    bool setter;
    bool getter;
    TokenInfo token_info;
  };
  
  
  /**
   * Return a next TokenInfo.
   * @return Next TokenInfo.
   */
  YATSC_INLINE TokenInfo* Next();


  /**
   * Return current TokenInfo.
   * @return Current TokenInfo.
   */
  YATSC_INLINE TokenInfo* Current() YATSC_NOEXCEPT;


  YATSC_INLINE TokenInfo* Prev() YATSC_NOEXCEPT;
  

 VISIBLE_FOR_TESTING:

  ParseResult ParseStatementListItem(bool yield, bool has_return, bool breakable, bool continuable);

  ParseResult ParseStatementList(bool yield, bool has_return);

  ParseResult ParseStatement(bool yield, bool has_return, bool breakable, bool continuable);

  ParseResult ParseBlockStatement(bool yield, bool has_return, bool breakable, bool continuable);

  ParseResult ParseModuleStatement();

  ParseResult ParseImportStatement();

  ParseResult ParseExportStatement();

  ParseResult ParseDeclaration(bool error, bool yield, bool has_default);
  
  ParseResult ParseDebuggerStatement();

  ParseResult ParseLexicalDeclaration(bool in, bool yield);

  ParseResult ParseLexicalBinding(bool const_decl, bool in, bool yield);

  ParseResult ParseBindingPattern(bool yield, bool generator_parameter);

  ParseResult ParseObjectBindingPattern(bool yield, bool generator_parameter);

  ParseResult ParseArrayBindingPattern(bool yield, bool generator_parameter);

  ParseResult ParseBindingProperty(bool yield, bool generator_parameter);

  ParseResult ParseBindingElement(bool yield, bool generator_parameter);

  ParseResult ParseBindingIdentifier(bool default_allowed, bool in, bool yield);

  ParseResult ParseVariableStatement(bool in, bool yield);
  
  ParseResult ParseVariableDeclaration(bool in, bool yield);

  ParseResult ParseIfStatement(bool yield, bool has_return, bool breakable, bool continuable);

  ParseResult ParseWhileStatement(bool yield, bool has_return);

  ParseResult ParseDoWhileStatement(bool yield, bool has_return);

  ParseResult ParseForStatement(bool yield, bool has_return);

  ParseResult ParseForIteration(Handle<ir::Node> reciever, TokenInfo*, bool yield, bool has_return);

  ParseResult ParseIterationBody(bool yield, bool has_return);

  ParseResult ParseContinueStatement(bool yield);

  ParseResult ParseBreakStatement(bool yield);

  ParseResult ParseReturnStatement(bool yield);

  ParseResult ParseWithStatement(bool yield, bool has_return, bool breakable, bool continuable);

  ParseResult ParseSwitchStatement(bool yield, bool has_return, bool continuable);

  ParseResult ParseCaseClauses(bool yield, bool has_return, bool continuable);

  ParseResult ParseLabelledStatement(bool yield, bool has_return, bool breakable, bool continuable);

  ParseResult ParseLabelledItem(bool yield, bool has_return, bool breakable, bool continuable);

  ParseResult ParseThrowStatement();

  ParseResult ParseTryStatement(bool yield, bool has_return, bool breakable, bool continuable);

  ParseResult ParseCatchBlock(bool yield, bool has_return, bool breakable, bool continuable);

  ParseResult ParseFinallyBlock(bool yield, bool has_return, bool breakable, bool continuable);

  ParseResult ParseInterfaceDeclaration();

  ParseResult ParseEnumDeclaration(bool yield, bool has_default);

  ParseResult ParseEnumBody(bool yield, bool has_default);

  ParseResult ParseEnumProperty(bool yield, bool has_default);

  Handle<ir::Node> CreateEnumFieldView(Handle<ir::Node> name, Handle<ir::Node> value);
  
  ParseResult ParseClassDeclaration(bool yield, bool has_default);

  ParseResult ParseClassBases();

  ParseResult ParseClassBody();

  ParseResult ParseClassElement();

  ParseResult ParseFieldModifiers();
  
  ParseResult ParseFieldModifier();

  ParseResult ParseConstructorOverloads(Handle<ir::Node> mods);

  ParseResult ParseConstructorOverloadOrImplementation(bool first, Handle<ir::Node> mods, Handle<ir::Node> overloads);

  bool IsMemberFunctionOverloadsBegin(TokenInfo* info);
  
  ParseResult ParseMemberFunctionOverloads(Handle<ir::Node> mods, AccessorType* at);

  ParseResult ParseMemberFunctionOverloadOrImplementation(
      bool first, Handle<ir::Node> mods, AccessorType* at, Handle<ir::Node> overloads);

  ParseResult ParseGeneratorMethodOverloads(Handle<ir::Node> mods);

  ParseResult ParseGeneratorMethodOverloadOrImplementation(bool first, Handle<ir::Node> mods, Handle<ir::Node> overloads);

  ParseResult ParseMemberVariable(Handle<ir::Node> mods);

  ParseResult ParseFunctionOverloads(bool yield, bool has_default, bool declaration, bool is_export);

  ParseResult ParseFunctionOverloadOrImplementation(Handle<ir::Node> overloads, bool yield, bool has_default, bool declaration);
 
  ParseResult ParseParameterList(bool accesslevel_allowed);
  
  ParseResult ParseParameter(bool rest, bool accesslevel_allowed);

  ParseResult ParseFunctionBody(bool yield);

  ParseResult ParseTypeExpression();

  ParseResult ParseReferencedType();

  ParseResult ParseGenericType();

  ParseResult ParseTypeArguments();

  ParseResult ParseTypeParameters();

  ParseResult ParseTypeQueryExpression();

  ParseResult ParseArrayType(Handle<ir::Node> type_expr);

  ParseResult ParseObjectTypeExpression();

  ParseResult ParseObjectTypeElement();

  ParseResult ParseCallSignature(bool accesslevel_allowed, bool annotation);

  ParseResult ParseIndexSignature();
  
  // Parse expression.
  ParseResult ParseExpression(bool in, bool yield);

  // Parse destructuring assignment.
  ParseResult ParseAssignmentPattern(bool yield);

  // Parse destructuring assignment object pattern.
  ParseResult ParseObjectAssignmentPattern(bool yield);

  // Parse destructuring assignment array pattern.
  // To simplify, we parse AssignmentElementList together.
  ParseResult ParseArrayAssignmentPattern(bool yield);

  // Parse destructuring assignment object pattern properties.
  ParseResult ParseAssignmentPropertyList(bool yield);

  // Parse destructuring assignment object pattern property.
  ParseResult ParseAssignmentProperty(bool yield);

  // Parse destructuring assignment array pattern element.
  ParseResult ParseAssignmentElement(bool yield);

  // Parse destructuring assignment array pattern rest element.
  ParseResult ParseAssignmentRestElement(bool yield);

  // Parse destructuring assignment target node.
  ParseResult ParseDestructuringAssignmentTarget(bool yield);

  // Parse assignment expression.
  ParseResult ParseAssignmentExpression(bool in, bool yield);

  ParseResult ParseArrowFunction(bool in, bool yield, Handle<ir::Node> identifier);

  ParseResult ParseArrowFunctionParameters(bool yield, Handle<ir::Node> identifier);

  ParseResult ParseConciseBody(bool in, Handle<ir::Node> call_sig);

  // Parse conditional expression.
  ParseResult ParseConditionalExpression(bool in, bool yield);

#define DEF_PARSE_BINARY_EXPR(name)                       \
  ParseResult Parse##name##Expression(bool in, bool yield);

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
  ParseResult ParseUnaryExpression(bool yield);

  // Parse postfix expression.
  ParseResult ParsePostfixExpression(bool yield);

  ParseResult ParseLeftHandSideExpression(bool yield);

  // Parse new expression.
  ParseResult ParseNewExpression(bool yield);
  
  // Parse member expression.
  ParseResult ParseMemberExpression(bool yield);

  // Parser getprop or getelem expression.
  ParseResult ParseGetPropOrElem(Handle<ir::Node> node, bool yield, bool dot_only, bool is_error);

  ParseResult ParseCallExpression(bool yield);

  ParseResult BuildArguments(ParseResult type_arguments_result, Handle<ir::Node> args, bool success);
  
  ParseResult ParseArguments(bool yield);

  ParseResult ParsePrimaryExpression(bool yield);

  ParseResult ParseArrayLiteral(bool yield);

  ParseResult ParseSpreadElement(bool yield);

  ParseResult ParseArrayComprehension(bool yield);

  ParseResult ParseComprehension(bool generator, bool yield);

  ParseResult ParseComprehensionTail(bool yield);

  ParseResult ParseComprehensionFor(bool yield);

  ParseResult ParseComprehensionIf(bool yield);

  ParseResult ParseGeneratorComprehension(bool yield);

  ParseResult ParseYieldExpression(bool in);

  ParseResult ParseForBinding(bool yield);

  ParseResult ParseObjectLiteral(bool yield);

  ParseResult ParsePropertyDefinition(bool yield);

  ParseResult ParsePropertyName(bool yield, bool generator_parameter);

  ParseResult ParseLiteralPropertyName();

  ParseResult ParseComputedPropertyName(bool yield);

  ParseResult ParseLiteral();

  ParseResult ParseValueLiteral();

  ParseResult ParseArrayInitializer(bool yield);

  ParseResult ParseIdentifierReference(bool yield);

  ParseResult ParseBindingIdentifier(bool default_allowed, bool yield);

  ParseResult ParseLabelIdentifier(bool yield);

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

  ParseResult ParseTSModule(Handle<ir::Node> identifier, TokenInfo* token_info);

  ParseResult ParseTSModuleBody();

  ParseResult ParseExportDeclaration();

  Handle<ir::Node> CreateExportView(
      Handle<ir::Node> export_clause,
      Handle<ir::Node> from_clause,
      TokenInfo* token_info,
      bool default_export);

  ParseResult ParseExportClause();

  Handle<ir::Node> CreateNamedExportView(
      Handle<ir::Node> identifier,
      Handle<ir::Node> binding);


  // Ambient
  ParseResult ParseDeclarationModule();
  
  ParseResult ParseAmbientDeclaration(bool module_allowed);

  ParseResult ParseAmbientVariableDeclaration(TokenInfo* info);

  ParseResult ParseAmbientFunctionDeclaration(TokenInfo* info);

  ParseResult ParseAmbientClassDeclaration(TokenInfo* info);

  ParseResult ParseAmbientClassBody();

  ParseResult ParseAmbientClassElement();

  ParseResult ParseAmbientConstructor(Handle<ir::Node> mods);

  ParseResult ParseAmbientMemberFunction(Handle<ir::Node> mods, AccessorType* acessor_type);

  ParseResult ParseAmbientGeneratorMethod(Handle<ir::Node> mods);

  ParseResult ParseAmbientMemberVariable(Handle<ir::Node> mods);

  ParseResult ParseAmbientEnumDeclaration(TokenInfo* info);

  ParseResult ParseAmbientEnumBody();

  ParseResult ParseAmbientEnumProperty();

  Handle<ir::Node> CreateAmbientEnumFieldView(Handle<ir::Node> name, Handle<ir::Node> value);

  ParseResult ParseAmbientModuleDeclaration(TokenInfo* info);

  ParseResult ParseAmbientModuleBody(bool external);

  ParseResult ParseAmbientModuleElement(bool external);
  

  bool IsLineTermination();

  void ConsumeLineTerminator();

  void EnableNestedGenericTypeScanMode() {scanner_->EnableNestedGenericTypeScanMode();}

  void DisableNestedGenericTypeScanMode() {scanner_->DisableNestedGenericTypeScanMode();}

  AccessorType ParseAccessor();

  void ValidateOverload(Handle<ir::MemberFunctionDefinitionView> node, Handle<ir::Node> overloads);
  
#if defined(UNIT_TEST) || defined(DEBUG)
  void PrintStackTrace() {
    Printf("%s\n", phase_buffer_.str().c_str());
  }
#endif

 private:
  class RecordedParserState {
   public:
    RecordedParserState(const typename Scanner<UCharInputSourceIterator>::RecordedCharPosition& rcp,
                        const TokenInfo& current,
                        const TokenInfo& prev,
                        Handle<ir::Scope> current_scope,
                        size_t error_count)
        : rcp_(rcp),
          current_(current),
          prev_(prev),
          scope_(current_scope),
          error_count_(error_count) {}

    YATSC_CONST_GETTER(typename Scanner<UCharInputSourceIterator>::RecordedCharPosition, rcp, rcp_);


    YATSC_CONST_GETTER(const TokenInfo&, current, current_);


    YATSC_CONST_GETTER(const TokenInfo&, prev, prev_);


    YATSC_CONST_GETTER(Handle<ir::Scope>, scope, scope_);

    YATSC_CONST_GETTER(size_t, error_count, error_count_);

   private:
    typename Scanner<UCharInputSourceIterator>::RecordedCharPosition rcp_;
    TokenInfo current_;
    TokenInfo prev_;
    Handle<ir::Scope> scope_;
    size_t error_count_;
  };


  void Initialize() {
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
  

  RecordedParserState parser_state() YATSC_NOEXCEPT {
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

  void RestoreParserState(const RecordedParserState& rps) {
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


  YATSC_INLINE void Declare(Handle<ir::Node> node) {
    scope_->Declare(node);
  }


  YATSC_PROPERTY(Handle<ir::Scope>, current_scope, scope_);


  Handle<ir::Scope> NewScope() {return Heap::NewHandle<ir::Scope>(current_scope());}

  
  void SkipTokensIfErrorOccured(Token token);


  YATSC_INLINE ParseResult Success(Handle<ir::Node> result) YATSC_NO_SE {return ParseResult(result, true);}


  YATSC_INLINE ParseResult Failed(Handle<ir::Node> result) YATSC_NO_SE {return ParseResult(result, false);}


  YATSC_INLINE ParseResult Failed() YATSC_NO_SE {return ParseResult(ir::Node::Null(), false);}


  YATSC_INLINE ParseResult Result(Handle<ir::Node> node, bool success) {return success? Success(node): Failed(node);}
  
#if defined(DEBUG) || defined(UNIT_TEST)
  StringStream phase_buffer_;
#endif
  Scanner<UCharInputSourceIterator>* scanner_;
  Handle<ModuleInfo> module_info_;
  Handle<ir::Scope> scope_;
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
