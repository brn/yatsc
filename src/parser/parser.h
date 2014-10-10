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



#ifndef DEBUG
// Throw error and return nullptr.
#define SYNTAX_ERROR_INTERNAL(message, pos, error)  \
  (*error_reporter_) << message;                    \
  error_reporter_->Throw<error>(pos);               \
  return ir::Node::Null()
#else
// Throw error that has source line and number for the error thrown position.
#define SYNTAX_ERROR_INTERNAL(message, pos, error)                      \
  (*error_reporter_) << message << '\n' << __FILE__ << ":" << __LINE__; \
  error_reporter_->Throw<error>(pos);                                   \
  return ir::Node::Null()
#endif


#ifdef DEBUG
// Logging current parse phase.
#define LOG_PHASE(name)                                          \
  if (Current() != nullptr) {                                           \
    phase_buffer_ << indent_ << "Enter " << #name << ": CurrentToken = " << Current()->ToString() << "\n"; \
  } else {                                                              \
    phase_buffer_ << indent_ << "Enter " << #name << ": CurrentToken = null\n"; \
  }                                                                     \
  indent_ += "  ";                                                      \
  YATSC_SCOPED([&]{                                                     \
    indent_ = indent_.substr(0, indent_.size() - 2);                    \
    if (this->Current() != nullptr) {                                   \
      phase_buffer_ << indent_ << "Exit " << #name << ": CurrentToken = " << Current()->ToString() << "\n"; \
    } else {                                                            \
      phase_buffer_ << indent_ << "Exit " << #name << ": CurrentToken = null\n"; \
    }                                                                   \
  })
#else
// Disabled.
#define LOG_PHASE(name)
#endif

template <typename UCharInputSourceIterator>
class Parser: public ParserBase {
 public:
  Parser(const CompilerOption& co, Scanner<UCharInputSourceIterator>* scanner, ErrorReporter* error_reporter)
      : ParserBase(co, error_reporter),
        scanner_(scanner) {Next();}

 private:
  
  /**
   * Return a next TokenInfo.
   * @return Next TokenInfo.
   */
  YATSC_INLINE TokenInfo* Next();


  /**
   * Return current TokenInfo.
   * @return Current TokenInfo.
   */
  YATSC_INLINE TokenInfo* Current();
  

 VISIBLE_FOR_TESTING:
  Handle<ir::Node> ParseProgram();

  Handle<ir::Node> ParseSourceElements();

  Handle<ir::Node> ParseSourceElement();

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

  Handle<ir::Node> ParseForIteration(Handle<ir::Node> reciever, TokenCursor, bool yield, bool has_return);

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

  Handle<ir::Node> ParseClassDeclaration(bool yield, bool has_default);

  Handle<ir::Node> ParseClassBases();

  Handle<ir::Node> ParseClassBody();

  Handle<ir::Node> ParseClassElement();

  Handle<ir::Node> ParseFieldModifiers();
  
  Handle<ir::Node> ParseFieldModifier();

  Handle<ir::Node> ParseConstructorOverloads(Handle<ir::Node> mods);

  Handle<ir::Node> ParseConstructorOverloadOrImplementation(bool first, Handle<ir::Node> overloads);

  Handle<ir::Node> ParseMemberFunctionOverloads(Handle<ir::Node> mods);

  Handle<ir::Node> ParseMemberFunctionOverloadOrImplementation(bool first, Handle<ir::Node> overloads);

  Handle<ir::Node> ParseGeneratorMethodOverloads(Handle<ir::Node> mods);

  Handle<ir::Node> ParseGeneratorMethodOverloadOrImplementation(bool first, Handle<ir::Node> overloads);

  Handle<ir::Node> ParseMemberVariable(Handle<ir::Node> mods);

  Handle<ir::Node> ParseFunctionOverloads(bool yield, bool has_default, bool declaration);

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

  Handle<ir::Node> ParseCallSignature(bool accesslevel_allowed);
  
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

  // Parse member expression.
  Handle<ir::Node> ParseMemberExpression(bool yield);

  // Parser getprop or getelem expression.
  Handle<ir::Node> ParseGetPropOrElem(Handle<ir::Node> node, bool yield);

  Handle<ir::Node> ParseCallExpression(bool yield);

  Handle<ir::Node> ParseArguments(bool yield);

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

  bool IsLineTermination();

  void ConsumeLineTerminator();

  template <typename T>
  void SetModifiers(bool* first, Handle<ir::Node> mods, T fn);

  Handle<ir::Node> ValidateOverload(Handle<ir::MemberFunctionDefinitionView> node, Handle<ir::Node> overloads);

#if defined(UNIT_TEST) || defined(DEBUG)
  void PrintStackTrace() {
    Printf("%s\n", phase_buffer_.str().c_str());
  }
#endif

 private:
#if defined(DEBUG) || defined(UNIT_TEST)
  StringStream phase_buffer_;
#endif
  Scanner<UCharInputSourceIterator>* scanner_;
};
} // yatsc

#include "./parser-inl.h"
#include "./expression-parser-partial.h"
#include "./type-parser-partial.h"
#include "./statement-parser-partial.h"

#undef SYNTAX_ERROR
#undef ARROW_PARAMETERS_ERROR
#undef SYNTAX_ERROR_POS
#undef ARROW_PARAMETERS_ERROR_POS
#undef SYNTAX_ERROR_INTERAL
#undef LOG_PHASE

#endif
