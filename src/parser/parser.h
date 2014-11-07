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
  SYNTAX_ERROR_POS(message, item)


// Generate SyntaxError and throw it.
// Usage. SYNTAX_ERROR("test " << message, Current())
#define SYNTAX_ERROR_NO_RETURN(message, item)   \
  SYNTAX_ERROR_POS_NO_RETURN(message, item)


#define CHECK_AST(node)                         \
  if (!node) {return node;}


#define SKIP_TOKEN_IF(node, token)              \
  if (!node) {SkipTokensIfErrorOccured(token);}


#define SKIP_TOKEN_OR(node, token)                    \
  SKIP_TOKEN_IF(node, token) else


#define SKIP_TOKEN_IF_AND_RETURN(node, token)   \
  if (!node) {                                  \
    SkipTokensIfErrorOccured(token);            \
    return ir::Node::Null();                    \
  }


// Throw error and return nullptr.
#define SYNTAX_ERROR_INTERNAL(message, item)    \
  REPORT_SYNTAX_ERROR_INTERNAL(message, item);  \
  return ir::Node::Null()


#define SYNTAX_ERROR_INTERNAL_NO_RETURN(message, item)  \
  REPORT_SYNTAX_ERROR_INTERNAL(message, item)


#ifndef DEBUG

#define REPORT_SYNTAX_ERROR_INTERNAL(message, item) \
  SyntaxError(item) << message;

#else

#define REPORT_SYNTAX_ERROR_INTERNAL(message, item)               \
  SyntaxError(item) << message << '\n' << __FILE__ << ":" << __LINE__;
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
         const Notificator<void(Handle<ModuleInfo>)>& notificator,
         ErrorReporter* error_reporter, Handle<ModuleInfo> module_info)
      : ParserBase(co, notificator, error_reporter),
        scanner_(scanner),
        module_info_(module_info) {
    Next();
  }

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

  Handle<ir::Node> ParseStatementListItem(bool yield, bool has_return, bool breakable, bool continuable, bool* ok);

  Handle<ir::Node> ParseStatementList(bool yield, bool has_return, bool* ok);

  Handle<ir::Node> ParseStatement(bool yield, bool has_return, bool breakable, bool continuable, bool* ok);

  Handle<ir::Node> ParseBlockStatement(bool yield, bool has_return, bool breakable, bool continuable, bool* ok);

  Handle<ir::Node> ParseModuleStatement(bool* ok);

  Handle<ir::Node> ParseImportStatement(bool* ok);

  Handle<ir::Node> ParseExportStatement(bool* ok);

  Handle<ir::Node> ParseDeclaration(bool error, bool yield, bool has_default, bool* ok);
  
  Handle<ir::Node> ParseDebuggerStatement(bool* ok);

  Handle<ir::Node> ParseLexicalDeclaration(bool in, bool yield, bool* ok);

  Handle<ir::Node> ParseLexicalBinding(bool const_decl, bool in, bool yield, bool* ok);

  Handle<ir::Node> ParseBindingPattern(bool yield, bool generator_parameter, bool* ok);

  Handle<ir::Node> ParseObjectBindingPattern(bool yield, bool generator_parameter, bool* ok);

  Handle<ir::Node> ParseArrayBindingPattern(bool yield, bool generator_parameter, bool* ok);

  Handle<ir::Node> ParseBindingProperty(bool yield, bool generator_parameter, bool* ok);

  Handle<ir::Node> ParseBindingElement(bool yield, bool generator_parameter, bool* ok);

  Handle<ir::Node> ParseBindingIdentifier(bool default_allowed, bool in, bool yield, bool* ok);

  Handle<ir::Node> ParseVariableStatement(bool in, bool yield, bool* ok);
  
  Handle<ir::Node> ParseVariableDeclaration(bool in, bool yield, bool* ok);

  Handle<ir::Node> ParseIfStatement(bool yield, bool has_return, bool breakable, bool continuable, bool* ok);

  Handle<ir::Node> ParseWhileStatement(bool yield, bool has_return, bool* ok);

  Handle<ir::Node> ParseDoWhileStatement(bool yield, bool has_return, bool* ok);

  Handle<ir::Node> ParseForStatement(bool yield, bool has_return, bool* ok);

  Handle<ir::Node> ParseForIteration(Handle<ir::Node> reciever, TokenInfo*, bool yield, bool has_return, bool* ok);

  Handle<ir::Node> ParseIterationBody(bool yield, bool has_return, bool* ok);

  Handle<ir::Node> ParseContinueStatement(bool yield, bool* ok);

  Handle<ir::Node> ParseBreakStatement(bool yield, bool* ok);

  Handle<ir::Node> ParseReturnStatement(bool yield, bool* ok);

  Handle<ir::Node> ParseWithStatement(bool yield, bool has_return, bool breakable, bool continuable, bool* ok);

  Handle<ir::Node> ParseSwitchStatement(bool yield, bool has_return, bool continuable, bool* ok);

  Handle<ir::Node> ParseCaseClauses(bool yield, bool has_return, bool continuable, bool* ok);

  Handle<ir::Node> ParseLabelledStatement(bool yield, bool has_return, bool breakable, bool continuable, bool* ok);

  Handle<ir::Node> ParseLabelledItem(bool yield, bool has_return, bool breakable, bool continuable, bool* ok);

  Handle<ir::Node> ParseThrowStatement(bool* ok);

  Handle<ir::Node> ParseTryStatement(bool yield, bool has_return, bool breakable, bool continuable, bool* ok);

  Handle<ir::Node> ParseCatchBlock(bool yield, bool has_return, bool breakable, bool continuable, bool* ok);

  Handle<ir::Node> ParseFinallyBlock(bool yield, bool has_return, bool breakable, bool continuable, bool* ok);

  Handle<ir::Node> ParseInterfaceDeclaration(bool* ok);

  Handle<ir::Node> ParseEnumDeclaration(bool yield, bool has_default, bool* ok);

  Handle<ir::Node> ParseEnumBody(bool yield, bool has_default, bool* ok);

  Handle<ir::Node> ParseEnumProperty(bool yield, bool has_default, bool* ok);

  Handle<ir::Node> CreateEnumFieldView(Handle<ir::Node> name, Handle<ir::Node> value);
  
  Handle<ir::Node> ParseClassDeclaration(bool yield, bool has_default, bool* ok);

  Handle<ir::Node> ParseClassBases(bool* ok);

  Handle<ir::Node> ParseClassBody(bool* ok);

  Handle<ir::Node> ParseClassElement(bool* ok);

  Handle<ir::Node> ParseFieldModifiers(bool* ok);
  
  Handle<ir::Node> ParseFieldModifier(bool* ok);

  Handle<ir::Node> ParseConstructorOverloads(Handle<ir::Node> mods, bool* ok);

  Handle<ir::Node> ParseConstructorOverloadOrImplementation(bool first, Handle<ir::Node> mods, Handle<ir::Node> overloads, bool* ok);

  bool IsMemberFunctionOverloadsBegin(TokenInfo* info);
  
  Handle<ir::Node> ParseMemberFunctionOverloads(Handle<ir::Node> mods, AccessorType* at, bool* ok);

  Handle<ir::Node> ParseMemberFunctionOverloadOrImplementation(
      bool first, Handle<ir::Node> mods, AccessorType* at, Handle<ir::Node> overloads, bool* ok);

  Handle<ir::Node> ParseGeneratorMethodOverloads(Handle<ir::Node> mods, bool* ok);

  Handle<ir::Node> ParseGeneratorMethodOverloadOrImplementation(bool first, Handle<ir::Node> mods, Handle<ir::Node> overloads, bool* ok);

  Handle<ir::Node> ParseMemberVariable(Handle<ir::Node> mods, bool* ok);

  Handle<ir::Node> ParseFunctionOverloads(bool yield, bool has_default, bool declaration, bool is_export, bool* ok);

  Handle<ir::Node> ParseFunctionOverloadOrImplementation(Handle<ir::Node> overloads, bool yield, bool has_default, bool declaration, bool* ok);
 
  Handle<ir::Node> ParseParameterList(bool accesslevel_allowed, bool* ok);
  
  Handle<ir::Node> ParseParameter(bool rest, bool accesslevel_allowed, bool* ok);

  Handle<ir::Node> ParseFunctionBody(bool yield, bool* ok);

  Handle<ir::Node> ParseTypeExpression(bool* ok);

  Handle<ir::Node> ParseReferencedType(bool* ok);

  Handle<ir::Node> ParseGenericType(bool* ok);

  Handle<ir::Node> ParseTypeArguments(bool* ok);

  Handle<ir::Node> ParseTypeParameters(bool* ok);

  Handle<ir::Node> ParseTypeQueryExpression(bool* ok);

  Handle<ir::Node> ParseArrayType(Handle<ir::Node> type_expr, bool* ok);

  Handle<ir::Node> ParseObjectTypeExpression(bool* ok);

  Handle<ir::Node> ParseObjectTypeElement(bool* ok);

  Handle<ir::Node> ParseCallSignature(bool accesslevel_allowed, bool annotation, bool* ok);

  Handle<ir::Node> ParseIndexSignature(bool* ok);
  
  // Parse expression.
  Handle<ir::Node> ParseExpression(bool in, bool yield, bool* ok);

  // Parse destructuring assignment.
  Handle<ir::Node> ParseAssignmentPattern(bool yield, bool* ok);

  // Parse destructuring assignment object pattern.
  Handle<ir::Node> ParseObjectAssignmentPattern(bool yield, bool* ok);

  // Parse destructuring assignment array pattern.
  // To simplify, we parse AssignmentElementList together.
  Handle<ir::Node> ParseArrayAssignmentPattern(bool yield, bool* ok);

  // Parse destructuring assignment object pattern properties.
  Handle<ir::Node> ParseAssignmentPropertyList(bool yield, bool* ok);

  // Parse destructuring assignment object pattern property.
  Handle<ir::Node> ParseAssignmentProperty(bool yield, bool* ok);

  // Parse destructuring assignment array pattern element.
  Handle<ir::Node> ParseAssignmentElement(bool yield, bool* ok);

  // Parse destructuring assignment array pattern rest element.
  Handle<ir::Node> ParseAssignmentRestElement(bool yield, bool* ok);

  // Parse destructuring assignment target node.
  Handle<ir::Node> ParseDestructuringAssignmentTarget(bool yield, bool* ok);

  // Parse assignment expression.
  Handle<ir::Node> ParseAssignmentExpression(bool in, bool yield, bool* ok);

  Handle<ir::Node> ParseArrowFunction(bool in, bool yield, Handle<ir::Node> identifier, bool* ok);

  Handle<ir::Node> ParseArrowFunctionParameters(bool yield, Handle<ir::Node> identifier, bool* ok);

  Handle<ir::Node> ParseConciseBody(bool in, Handle<ir::Node> call_sig, bool* ok);

  // Parse conditional expression.
  Handle<ir::Node> ParseConditionalExpression(bool in, bool yield, bool* ok);

#define DEF_PARSE_BINARY_EXPR(name)                       \
  Handle<ir::Node> Parse##name##Expression(bool in, bool yield, bool* ok);

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
  Handle<ir::Node> ParseUnaryExpression(bool yield, bool* ok);

  // Parse postfix expression.
  Handle<ir::Node> ParsePostfixExpression(bool yield, bool* ok);

  Handle<ir::Node> ParseLeftHandSideExpression(bool yield, bool* ok);

  // Parse new expression.
  Handle<ir::Node> ParseNewExpression(bool yield, bool* ok);
  
  // Parse member expression.
  Handle<ir::Node> ParseMemberExpression(bool yield, bool* ok);

  // Parser getprop or getelem expression.
  Handle<ir::Node> ParseGetPropOrElem(Handle<ir::Node> node, bool yield, bool dot_only, bool is_error, bool* ok);

  Handle<ir::Node> ParseCallExpression(bool yield, bool* ok);

  NodePair ParseArguments(bool yield, bool* ok);

  NodePair InvalidPair() {return NodePair(ir::Node::Null(), ir::Node::Null());}

  Handle<ir::Node> ParsePrimaryExpression(bool yield, bool* ok);

  Handle<ir::Node> ParseArrayLiteral(bool yield, bool* ok);

  Handle<ir::Node> ParseSpreadElement(bool yield, bool* ok);

  Handle<ir::Node> ParseArrayComprehension(bool yield, bool* ok);

  Handle<ir::Node> ParseComprehension(bool generator, bool yield, bool* ok);

  Handle<ir::Node> ParseComprehensionTail(bool yield, bool* ok);

  Handle<ir::Node> ParseComprehensionFor(bool yield, bool* ok);

  Handle<ir::Node> ParseComprehensionIf(bool yield, bool* ok);

  Handle<ir::Node> ParseGeneratorComprehension(bool yield, bool* ok);

  Handle<ir::Node> ParseYieldExpression(bool in, bool* ok);

  Handle<ir::Node> ParseForBinding(bool yield, bool* ok);

  Handle<ir::Node> ParseObjectLiteral(bool yield, bool* ok);

  Handle<ir::Node> ParsePropertyDefinition(bool yield, bool* ok);

  Handle<ir::Node> ParsePropertyName(bool yield, bool generator_parameter, bool* ok);

  Handle<ir::Node> ParseLiteralPropertyName(bool* ok);

  Handle<ir::Node> ParseComputedPropertyName(bool yield, bool* ok);

  Handle<ir::Node> ParseLiteral(bool* ok);

  Handle<ir::Node> ParseValueLiteral(bool* ok);

  Handle<ir::Node> ParseArrayInitializer(bool yield, bool* ok);

  Handle<ir::Node> ParseIdentifierReference(bool yield, bool* ok);

  Handle<ir::Node> ParseBindingIdentifier(bool default_allowed, bool yield, bool* ok);

  Handle<ir::Node> ParseLabelIdentifier(bool yield, bool* ok);

  Handle<ir::Node> ParseIdentifier(bool* ok);

  Handle<ir::Node> ParseStringLiteral(bool* ok);

  Handle<ir::Node> ParseNumericLiteral(bool* ok);

  Handle<ir::Node> ParseBooleanLiteral(bool* ok);

  Handle<ir::Node> ParseUndefinedLiteral(bool* ok);

  Handle<ir::Node> ParseNaNLiteral(bool* ok);

  Handle<ir::Node> ParseRegularExpression(bool* ok);

  Handle<ir::Node> ParseTemplateLiteral(bool* ok);

  Handle<ir::Node> ParseEmptyStatement(bool* ok);

  Handle<ir::Node> ParseModule(bool* ok);

  Handle<ir::Node> ParseImportDeclaration(bool* ok);

  Handle<ir::Node> ParseExternalModuleReference(bool* ok);

  Handle<ir::Node> ParseImportClause(bool* ok);

  Handle<ir::Node> ParseNamedImport(bool* ok);

  Handle<ir::Node> ParseFromClause(bool* ok);

  Handle<ir::Node> ParseModuleImport(bool* ok);

  Handle<ir::Node> ParseTSModule(Handle<ir::Node> identifier, TokenInfo* token_info, bool* ok);

  Handle<ir::Node> ParseTSModuleBody(bool* ok);

  Handle<ir::Node> ParseExportDeclaration(bool* ok);

  Handle<ir::Node> CreateExportView(
      Handle<ir::Node> export_clause,
      Handle<ir::Node> from_clause,
      TokenInfo* token_info,
      bool default_export,
      bool* ok);

  Handle<ir::Node> ParseExportClause(bool* ok);

  Handle<ir::Node> CreateNamedExportView(
      Handle<ir::Node> identifier,
      Handle<ir::Node> binding,
      bool* ok);


  // Ambient
  Handle<ir::Node> ParseDeclarationModule(bool* ok);
  
  Handle<ir::Node> ParseAmbientDeclaration(bool module_allowed, bool* ok);

  Handle<ir::Node> ParseAmbientVariableDeclaration(TokenInfo* info, bool* ok);

  Handle<ir::Node> ParseAmbientFunctionDeclaration(TokenInfo* info, bool* ok);

  Handle<ir::Node> ParseAmbientClassDeclaration(TokenInfo* info, bool* ok);

  Handle<ir::Node> ParseAmbientClassBody(bool* ok);

  Handle<ir::Node> ParseAmbientClassElement(bool* ok);

  Handle<ir::Node> ParseAmbientConstructor(Handle<ir::Node> mods, bool* ok);

  Handle<ir::Node> ParseAmbientMemberFunction(Handle<ir::Node> mods, AccessorType* acessor_type, bool* ok);

  Handle<ir::Node> ParseAmbientGeneratorMethod(Handle<ir::Node> mods, bool* ok);

  Handle<ir::Node> ParseAmbientMemberVariable(Handle<ir::Node> mods, bool* ok);

  Handle<ir::Node> ParseAmbientEnumDeclaration(TokenInfo* info, bool* ok);

  Handle<ir::Node> ParseAmbientEnumBody(bool* ok);

  Handle<ir::Node> ParseAmbientEnumProperty(bool* ok);

  Handle<ir::Node> CreateAmbientEnumFieldView(Handle<ir::Node> name, Handle<ir::Node> value, bool* ok);

  Handle<ir::Node> ParseAmbientModuleDeclaration(TokenInfo* info, bool* ok);

  Handle<ir::Node> ParseAmbientModuleBody(bool external, bool* ok);

  Handle<ir::Node> ParseAmbientModuleElement(bool external, bool* ok);
  

  bool IsLineTermination();

  void ConsumeLineTerminator();

  void EnableNestedGenericTypeScanMode() {scanner_->EnableNestedGenericTypeScanMode();}

  void DisableNestedGenericTypeScanMode() {scanner_->DisableNestedGenericTypeScanMode();}

  AccessorType ParseAccessor();

  Handle<ir::Node> ValidateOverload(Handle<ir::MemberFunctionDefinitionView> node, Handle<ir::Node> overloads, bool* ok);
  
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
                        Handle<ir::Scope> current_scope)
        : rcp_(rcp),
          current_(current),
          prev_(prev),
          scope_(current_scope){}

    YATSC_CONST_GETTER(typename Scanner<UCharInputSourceIterator>::RecordedCharPosition, rcp, rcp_);


    YATSC_CONST_GETTER(const TokenInfo&, current, current_);


    YATSC_CONST_GETTER(const TokenInfo&, prev, prev_);


    YATSC_CONST_GETTER(Handle<ir::Scope>, scope, scope_);

   private:
    typename Scanner<UCharInputSourceIterator>::RecordedCharPosition rcp_;
    TokenInfo current_;
    TokenInfo prev_;
    Handle<ir::Scope> scope_;
  };

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
    
    return RecordedParserState(scanner_->char_position(), current, prev, scope);
  }

  void RestoreParserState(const RecordedParserState& rps) {
    scanner_->RestoreScannerPosition(rps.rcp());
    *current_token_info_ = rps.current();
    prev_token_info_ = rps.prev();
    scope_ = rps.scope();
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
