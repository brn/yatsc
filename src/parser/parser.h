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

namespace yatsc {

// Generate SyntaxError and throw it.
// Usage. SYNTAX_ERROR("test " << message, Current())
#define SYNTAX_ERROR(message, token)                   \
  SYNTAX_ERROR_POS(message, token->source_position())


// Generate ArrowParametersError and throw it.
// Usage. ARROW_PARAMETERS_ERROR("test " << message, Current())
#define ARROW_PARAMETERS_ERROR(message, token)                   \
  ARROW_PARAMETERS_ERROR_POS(message, token->source_position())


// Generate SyntaxError that is pointed specified position and throw it.
// Usage. SYNTAX_ERROR_POS("test " << message, node->source_position())
#define SYNTAX_ERROR_POS(message, pos)       \
  SYNTAX_ERROR_INTERNAL(message, pos, SyntaxError)


// Generate ArrowParametersError that is pointed specified position and throw it.
// Usage. ARROW_PARAMETERS_ERROR_POS("test " << message, node->source_position())
#define ARROW_PARAMETERS_ERROR_POS(message, pos)     \
  SYNTAX_ERROR_INTERNAL(message, pos, ArrowParametersError)


// Generate SyntaxError and throw it.
// Usage. SYNTAX_ERROR("test " << message, Current())
#define SYNTAX_ERROR_NO_RETURN(message, token)                   \
  SYNTAX_ERROR_POS_NO_RETURN(message, token->source_position())


// Generate ArrowParametersError and throw it.
// Usage. ARROW_PARAMETERS_ERROR("test " << message, Current())
#define ARROW_PARAMETERS_ERROR_NO_RETURN(message, token)                   \
  ARROW_PARAMETERS_ERROR_POS_NO_RETURN(message, token->source_position())


// Generate SyntaxError that is pointed specified position and throw it.
// Usage. SYNTAX_ERROR_POS("test " << message, node->source_position())
#define SYNTAX_ERROR_POS_NO_RETURN(message, pos)       \
  SYNTAX_ERROR_INTERNAL_NO_RETURN(message, pos, SyntaxError)


// Generate ArrowParametersError that is pointed specified position and throw it.
// Usage. ARROW_PARAMETERS_ERROR_POS("test " << message, node->source_position())
#define ARROW_PARAMETERS_ERROR_POS_NO_RETURN(message, pos)     \
  SYNTAX_ERROR_INTERNAL_NO_RETURN(message, pos, ArrowParametersError)



#ifndef DEBUG
// Throw error and return nullptr.
#define SYNTAX_ERROR_INTERNAL(message, pos, error)    \
  REPORT_SYNTAX_ERROR_INTERNAL(message, pos, error);  \
  return ir::Node::Null()


#define SYNTAX_ERROR_INTERNAL_NO_RETURN(message, pos, error)  \
    REPORT_SYNTAX_ERROR_INTERNAL(message, pos, error)


#define REPORT_SYNTAX_ERROR_INTERNAL(message, pos, error) \
  (*error_reporter_) << message;                          \
  error_reporter_->Throw<error>(pos)

#else
// Throw error that has source line and number for the error thrown position.
#define SYNTAX_ERROR_INTERNAL(message, pos, error)    \
  REPORT_SYNTAX_ERROR_INTERNAL(message, pos, error);  \
  return ir::Node::Null()


// Throw error that has source line and number for the error thrown position.
#define SYNTAX_ERROR_INTERNAL_NO_RETURN(message, pos, error)  \
  REPORT_SYNTAX_ERROR_INTERNAL(message, pos, error)


#define REPORT_SYNTAX_ERROR_INTERNAL(message, pos, error)               \
  (*error_reporter_) << message << '\n' << __FILE__ << ":" << __LINE__; \
  error_reporter_->Throw<error>(pos)
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
  Parser(const CompilerOption& co, Scanner<UCharInputSourceIterator>* scanner, ErrorReporter* error_reporter, Handle<ModuleInfo> module_info)
      : ParserBase(co, error_reporter),
        scanner_(scanner),
        module_info_(module_info) {Next();}

  Handle<ir::Node> Parse() {return ParseModule();};

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

  Handle<ir::Node> ParseFunctionOverloads(bool yield, bool has_default, bool declaration, bool is_export = false);

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

  Handle<ir::Node> ParseCallSignature(bool accesslevel_allowed, bool annotation = false);

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

  Handle<ir::Node> ParseArrowFunction(bool in, bool yield, Handle<ir::Node> identifier = nullptr);

  Handle<ir::Node> ParseArrowFunctionParameters(bool yield, Handle<ir::Node> identifier = nullptr);

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
  Handle<ir::Node> ParseGetPropOrElem(Handle<ir::Node> node, bool yield, bool dot_only = false);

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
      bool default_export = false);

  Handle<ir::Node> ParseExportClause();

  Handle<ir::Node> CreateNamedExportView(
      Handle<ir::Node> identifier,
      Handle<ir::Node> binding);

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
    RecordedParserState(const typename Scanner<UCharInputSourceIterator>::RecordedCharPosition& rcp, const TokenInfo& current, const TokenInfo& prev)
        : rcp_(rcp),
          current_(current),
          prev_(prev) {}

    YATSC_CONST_GETTER(typename Scanner<UCharInputSourceIterator>::RecordedCharPosition, rcp, rcp_);


    YATSC_CONST_GETTER(const TokenInfo&, current, current_);


    YATSC_CONST_GETTER(const TokenInfo&, prev, prev_);

   private:
    typename Scanner<UCharInputSourceIterator>::RecordedCharPosition rcp_;
    TokenInfo current_;
    TokenInfo prev_;
  };

  RecordedParserState parser_state() YATSC_NOEXCEPT {
    TokenInfo prev;
    TokenInfo current;
    if (Prev() != nullptr) {
      prev = *Prev();
    }
    if (Current() != nullptr) {
      current = *Current();
    }
    
    return RecordedParserState(scanner_->char_position(), current, prev);
  }

  void RestoreParserState(const RecordedParserState& rps) {
    scanner_->RestoreScannerPosition(rps.rcp());
    *current_token_info_ = rps.current();
    prev_token_info_ = rps.prev();
  }
  
#if defined(DEBUG) || defined(UNIT_TEST)
  StringStream phase_buffer_;
#endif
  Scanner<UCharInputSourceIterator>* scanner_;
  Handle<ModuleInfo> module_info_;
};
} // yatsc

#include "./parser-inl.h"
#include "./expression-parser-partial.h"
#include "./type-parser-partial.h"
#include "./statement-parser-partial.h"
#include "./module-parser-partial.h"

#undef SYNTAX_ERROR
#undef ARROW_PARAMETERS_ERROR
#undef SYNTAX_ERROR_POS
#undef ARROW_PARAMETERS_ERROR_POS
#undef SYNTAX_ERROR_INTERAL
#undef LOG_PHASE

#endif
