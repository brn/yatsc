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


#define CHECK_AST(node)                         \
  if (!node) {return node;}


#define SKIP_TOKEN_IF(node, token)              \
  if (!node) {SkipTokensIfErrorOccured(token);}


#define SKIP_TOKEN_OR(node, token)                    \
  SKIP_TOKEN_IF(node, token) else


#define SKIP_TOKEN_IF_AND(node, token, expr)    \
  if (!node) {                                  \
    SkipTokensIfErrorOccured(token);            \
    expr;                                       \
  }


// Throw error and return nullptr.
#define SYNTAX_ERROR_INTERNAL(message, item)    \
  REPORT_SYNTAX_ERROR_INTERNAL(message, item);  \
  return ir::Node::Null()


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
    phase_buffer_ << indent_ << "Enter " << #name << ": CurrentToken = " << Current()->ToString() << ",generic?[" << scanner_->nested_generic_count() << "]\n"; \
  } else {                                                              \
    phase_buffer_ << indent_ << "Enter " << #name << ": CurrentToken = null,generic?[" << scanner_->nested_generic_count() << "]\n"; \
  }                                                                     \
  indent_ += "  ";                                                      \
  YATSC_SCOPED([&]{                                                     \
    indent_ = indent_.substr(0, indent_.size() - 2);                    \
    if (this->Current() != nullptr) {                                   \
      phase_buffer_ << indent_ << "Exit " << #name << ": CurrentToken = " << Current()->ToString() << ",generic?[" << scanner_->nested_generic_count() << "]\n"; \
    } else {                                                            \
      phase_buffer_ << indent_ << "Exit " << #name << ": CurrentToken = null,generic?[" << scanner_->nested_generic_count() << "]\n"; \
    }                                                                   \
  })
#else
// Disabled.
#define LOG_PHASE(name)
#endif

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
      return ParseDeclarationModule();
    } else {
      return ParseModule();
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

  typedef std::pair<Handle<ir::Node>, Handle<ir::Node>> NodePair;
  
  
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

  Handle<ir::Node> ParseStatementListItem(bool yield, bool has_return, bool breakable, bool continuable);

  Handle<ir::Node> ParseStatementList(bool yield, bool has_return);

  Handle<ir::Node> ParseStatement(bool yield, bool has_return, bool breakable, bool continuable);

  Handle<ir::Node> ParseBlockStatement(bool yield, bool has_return, bool breakable, bool continuable);

  Handle<ir::Node> ParseModuleStatement();

  Handle<ir::Node> ParseImportStatement();

  Handle<ir::Node> ParseExportStatement();

  Handle<ir::Node> ParseDeclaration(bool error, bool yield, bool has_default);
  
  Handle<ir::Node> ParseDebuggerStatement();

  Handle<ir::Node> ParseLexicalDeclaration(bool in, bool yield);

  Handle<ir::Node> ParseLexicalBinding(bool const_decl, bool in, bool yield);

  Handle<ir::Node> ParseBindingPattern(bool yield, bool generator_parameter);

  Handle<ir::Node> ParseObjectBindingPattern(bool yield, bool generator_parameter);

  Handle<ir::Node> ParseArrayBindingPattern(bool yield, bool generator_parameter);

  Handle<ir::Node> ParseBindingProperty(bool yield, bool generator_parameter);

  Handle<ir::Node> ParseBindingElement(bool yield, bool generator_parameter);

  Handle<ir::Node> ParseBindingIdentifier(bool default_allowed, bool in, bool yield);

  Handle<ir::Node> ParseVariableStatement(bool in, bool yield);
  
  Handle<ir::Node> ParseVariableDeclaration(bool in, bool yield);

  Handle<ir::Node> ParseIfStatement(bool yield, bool has_return, bool breakable, bool continuable);

  Handle<ir::Node> ParseWhileStatement(bool yield, bool has_return);

  Handle<ir::Node> ParseDoWhileStatement(bool yield, bool has_return);

  Handle<ir::Node> ParseForStatement(bool yield, bool has_return);

  Handle<ir::Node> ParseForIteration(Handle<ir::Node> reciever, TokenInfo*, bool yield, bool has_return);

  Handle<ir::Node> ParseIterationBody(bool yield, bool has_return);

  Handle<ir::Node> ParseContinueStatement(bool yield);

  Handle<ir::Node> ParseBreakStatement(bool yield);

  Handle<ir::Node> ParseReturnStatement(bool yield);

  Handle<ir::Node> ParseWithStatement(bool yield, bool has_return, bool breakable, bool continuable);

  Handle<ir::Node> ParseSwitchStatement(bool yield, bool has_return, bool continuable);

  Handle<ir::Node> ParseCaseClauses(bool yield, bool has_return, bool continuable);

  Handle<ir::Node> ParseLabelledStatement(bool yield, bool has_return, bool breakable, bool continuable);

  Handle<ir::Node> ParseLabelledItem(bool yield, bool has_return, bool breakable, bool continuable);

  Handle<ir::Node> ParseThrowStatement();

  Handle<ir::Node> ParseTryStatement(bool yield, bool has_return, bool breakable, bool continuable);

  Handle<ir::Node> ParseCatchBlock(bool yield, bool has_return, bool breakable, bool continuable);

  Handle<ir::Node> ParseFinallyBlock(bool yield, bool has_return, bool breakable, bool continuable);

  Handle<ir::Node> ParseInterfaceDeclaration();

  Handle<ir::Node> ParseEnumDeclaration(bool yield, bool has_default);

  Handle<ir::Node> ParseEnumBody(bool yield, bool has_default);

  Handle<ir::Node> ParseEnumProperty(bool yield, bool has_default);

  Handle<ir::Node> CreateEnumFieldView(Handle<ir::Node> name, Handle<ir::Node> value);
  
  Handle<ir::Node> ParseClassDeclaration(bool yield, bool has_default);

  Handle<ir::Node> ParseClassBases();

  Handle<ir::Node> ParseClassBody();

  Handle<ir::Node> ParseClassElement();

  Handle<ir::Node> ParseFieldModifiers();
  
  Handle<ir::Node> ParseFieldModifier();

  Handle<ir::Node> ParseConstructorOverloads(Handle<ir::Node> mods);

  Handle<ir::Node> ParseConstructorOverloadOrImplementation(bool first, Handle<ir::Node> mods, Handle<ir::Node> overloads);

  bool IsMemberFunctionOverloadsBegin(TokenInfo* info);
  
  Handle<ir::Node> ParseMemberFunctionOverloads(Handle<ir::Node> mods, AccessorType* at);

  Handle<ir::Node> ParseMemberFunctionOverloadOrImplementation(
      bool first, Handle<ir::Node> mods, AccessorType* at, Handle<ir::Node> overloads);

  Handle<ir::Node> ParseGeneratorMethodOverloads(Handle<ir::Node> mods);

  Handle<ir::Node> ParseGeneratorMethodOverloadOrImplementation(bool first, Handle<ir::Node> mods, Handle<ir::Node> overloads);

  Handle<ir::Node> ParseMemberVariable(Handle<ir::Node> mods);

  Handle<ir::Node> ParseFunctionOverloads(bool yield, bool has_default, bool declaration, bool is_export);

  Handle<ir::Node> ParseFunctionOverloadOrImplementation(Handle<ir::Node> overloads, bool yield, bool has_default, bool declaration);
 
  Handle<ir::Node> ParseParameterList(bool accesslevel_allowed);
  
  Handle<ir::Node> ParseParameter(bool rest, bool accesslevel_allowed);

  Handle<ir::Node> ParseFunctionBody(bool yield);

  Handle<ir::Node> ParseTypeExpression();

  Handle<ir::Node> ParseReferencedType();

  Handle<ir::Node> ParseGenericType();

  Handle<ir::Node> ParseTypeArguments();

  Handle<ir::Node> ParseTypeParameters();

  Handle<ir::Node> ParseTypeQueryExpression();

  Handle<ir::Node> ParseArrayType(Handle<ir::Node> type_expr);

  Handle<ir::Node> ParseObjectTypeExpression();

  Handle<ir::Node> ParseObjectTypeElement();

  Handle<ir::Node> ParseCallSignature(bool accesslevel_allowed, bool annotation);

  Handle<ir::Node> ParseIndexSignature();
  
  // Parse expression.
  Handle<ir::Node> ParseExpression(bool in, bool yield);

  // Parse destructuring assignment.
  Handle<ir::Node> ParseAssignmentPattern(bool yield);

  // Parse destructuring assignment object pattern.
  Handle<ir::Node> ParseObjectAssignmentPattern(bool yield);

  // Parse destructuring assignment array pattern.
  // To simplify, we parse AssignmentElementList together.
  Handle<ir::Node> ParseArrayAssignmentPattern(bool yield);

  // Parse destructuring assignment object pattern properties.
  Handle<ir::Node> ParseAssignmentPropertyList(bool yield);

  // Parse destructuring assignment object pattern property.
  Handle<ir::Node> ParseAssignmentProperty(bool yield);

  // Parse destructuring assignment array pattern element.
  Handle<ir::Node> ParseAssignmentElement(bool yield);

  // Parse destructuring assignment array pattern rest element.
  Handle<ir::Node> ParseAssignmentRestElement(bool yield);

  // Parse destructuring assignment target node.
  Handle<ir::Node> ParseDestructuringAssignmentTarget(bool yield);

  // Parse assignment expression.
  Handle<ir::Node> ParseAssignmentExpression(bool in, bool yield);

  Handle<ir::Node> ParseArrowFunction(bool in, bool yield, Handle<ir::Node> identifier);

  Handle<ir::Node> ParseArrowFunctionParameters(bool yield, Handle<ir::Node> identifier);

  Handle<ir::Node> ParseConciseBody(bool in, Handle<ir::Node> call_sig);

  // Parse conditional expression.
  Handle<ir::Node> ParseConditionalExpression(bool in, bool yield);

#define DEF_PARSE_BINARY_EXPR(name)                       \
  Handle<ir::Node> Parse##name##Expression(bool in, bool yield);

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
  Handle<ir::Node> ParseUnaryExpression(bool yield);

  // Parse postfix expression.
  Handle<ir::Node> ParsePostfixExpression(bool yield);

  Handle<ir::Node> ParseLeftHandSideExpression(bool yield);

  // Parse new expression.
  Handle<ir::Node> ParseNewExpression(bool yield);
  
  // Parse member expression.
  Handle<ir::Node> ParseMemberExpression(bool yield);

  // Parser getprop or getelem expression.
  Handle<ir::Node> ParseGetPropOrElem(Handle<ir::Node> node, bool yield, bool dot_only, bool is_error);

  Handle<ir::Node> ParseCallExpression(bool yield);

  NodePair ParseArguments(bool yield);

  NodePair InvalidPair() {return NodePair(ir::Node::Null(), ir::Node::Null());}

  Handle<ir::Node> ParsePrimaryExpression(bool yield);

  Handle<ir::Node> ParseArrayLiteral(bool yield);

  Handle<ir::Node> ParseSpreadElement(bool yield);

  Handle<ir::Node> ParseArrayComprehension(bool yield);

  Handle<ir::Node> ParseComprehension(bool generator, bool yield);

  Handle<ir::Node> ParseComprehensionTail(bool yield);

  Handle<ir::Node> ParseComprehensionFor(bool yield);

  Handle<ir::Node> ParseComprehensionIf(bool yield);

  Handle<ir::Node> ParseGeneratorComprehension(bool yield);

  Handle<ir::Node> ParseYieldExpression(bool in);

  Handle<ir::Node> ParseForBinding(bool yield);

  Handle<ir::Node> ParseObjectLiteral(bool yield);

  Handle<ir::Node> ParsePropertyDefinition(bool yield);

  Handle<ir::Node> ParsePropertyName(bool yield, bool generator_parameter);

  Handle<ir::Node> ParseLiteralPropertyName();

  Handle<ir::Node> ParseComputedPropertyName(bool yield);

  Handle<ir::Node> ParseLiteral();

  Handle<ir::Node> ParseValueLiteral();

  Handle<ir::Node> ParseArrayInitializer(bool yield);

  Handle<ir::Node> ParseIdentifierReference(bool yield);

  Handle<ir::Node> ParseBindingIdentifier(bool default_allowed, bool yield);

  Handle<ir::Node> ParseLabelIdentifier(bool yield);

  Handle<ir::Node> ParseIdentifier();

  Handle<ir::Node> ParseStringLiteral();

  Handle<ir::Node> ParseNumericLiteral();

  Handle<ir::Node> ParseBooleanLiteral();

  Handle<ir::Node> ParseUndefinedLiteral();

  Handle<ir::Node> ParseNaNLiteral();

  Handle<ir::Node> ParseRegularExpression();

  Handle<ir::Node> ParseTemplateLiteral();

  Handle<ir::Node> ParseEmptyStatement();

  Handle<ir::Node> ParseModule();

  Handle<ir::Node> ParseImportDeclaration();

  Handle<ir::Node> ParseExternalModuleReference();

  Handle<ir::Node> ParseImportClause();

  Handle<ir::Node> ParseNamedImport();

  Handle<ir::Node> ParseFromClause();

  Handle<ir::Node> ParseModuleImport();

  Handle<ir::Node> ParseTSModule(Handle<ir::Node> identifier, TokenInfo* token_info);

  Handle<ir::Node> ParseTSModuleBody();

  Handle<ir::Node> ParseExportDeclaration();

  Handle<ir::Node> CreateExportView(
      Handle<ir::Node> export_clause,
      Handle<ir::Node> from_clause,
      TokenInfo* token_info,
      bool default_export);

  Handle<ir::Node> ParseExportClause();

  Handle<ir::Node> CreateNamedExportView(
      Handle<ir::Node> identifier,
      Handle<ir::Node> binding);


  // Ambient
  Handle<ir::Node> ParseDeclarationModule();
  
  Handle<ir::Node> ParseAmbientDeclaration(bool module_allowed);

  Handle<ir::Node> ParseAmbientVariableDeclaration(TokenInfo* info);

  Handle<ir::Node> ParseAmbientFunctionDeclaration(TokenInfo* info);

  Handle<ir::Node> ParseAmbientClassDeclaration(TokenInfo* info);

  Handle<ir::Node> ParseAmbientClassBody();

  Handle<ir::Node> ParseAmbientClassElement();

  Handle<ir::Node> ParseAmbientConstructor(Handle<ir::Node> mods);

  Handle<ir::Node> ParseAmbientMemberFunction(Handle<ir::Node> mods, AccessorType* acessor_type);

  Handle<ir::Node> ParseAmbientGeneratorMethod(Handle<ir::Node> mods);

  Handle<ir::Node> ParseAmbientMemberVariable(Handle<ir::Node> mods);

  Handle<ir::Node> ParseAmbientEnumDeclaration(TokenInfo* info);

  Handle<ir::Node> ParseAmbientEnumBody();

  Handle<ir::Node> ParseAmbientEnumProperty();

  Handle<ir::Node> CreateAmbientEnumFieldView(Handle<ir::Node> name, Handle<ir::Node> value);

  Handle<ir::Node> ParseAmbientModuleDeclaration(TokenInfo* info);

  Handle<ir::Node> ParseAmbientModuleBody(bool external);

  Handle<ir::Node> ParseAmbientModuleElement(bool external);
  

  bool IsLineTermination();

  void ConsumeLineTerminator();

  void EnableNestedGenericTypeScanMode() {scanner_->EnableNestedGenericTypeScanMode();}

  void DisableNestedGenericTypeScanMode() {scanner_->DisableNestedGenericTypeScanMode();}

  AccessorType ParseAccessor();

  Handle<ir::Node> ValidateOverload(Handle<ir::MemberFunctionDefinitionView> node, Handle<ir::Node> overloads);
  
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
