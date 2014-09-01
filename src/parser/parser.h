/*
 * The MIT License (MIT)
 * 
 * Copyright (c) 2013 Taketoshi Aono(brn)
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#ifndef YATSC_PARSER_PARSER_H
#define YATSC_PARSER_PARSER_H

#include <sstream>
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
  return nullptr
#else
// Throw error that has source line and number for the error thrown position.
#define SYNTAX_ERROR_INTERNAL(message, pos, error)                      \
  (*error_reporter_) << message << '\n' << __FILE__ << ":" << __LINE__; \
  error_reporter_->Throw<error>(pos);                                   \
  return nullptr
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

  class ParserState {
   public:
    ParserState()
        : breakable_(false) {}

    YATSC_INLINE bool IsBreakable() YATSC_NO_SE {
      return breakable_;
    }

    YATSC_INLINE void set_breakable(bool breakable) YATSC_NOEXCEPT {breakable_ = breakable;}
    
   private:
    bool breakable_;
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
  YATSC_INLINE TokenInfo* Current();
  

 VISIBLE_FOR_TESTING:
  ir::Node* ParseProgram();

  ir::Node* ParseSourceElements();

  ir::Node* ParseSourceElement();

  ir::Node* ParseStatementListItem(bool yield, bool has_return);

  ir::Node* ParseStatementList(bool yield, bool has_return);

  ir::Node* ParseStatement(bool yield, bool has_return);

  ir::Node* ParseBlockStatement(bool yield, bool has_return);

  ir::Node* ParseModuleStatement();

  ir::Node* ParseImportStatement();

  ir::Node* ParseExportStatement();

  ir::Node* ParseDeclaration(bool error, bool yield, bool has_default);
  
  ir::Node* ParseDebuggerStatement();

  ir::Node* ParseLexicalDeclaration(bool in, bool yield);

  ir::Node* ParseLexicalBinding(bool const_decl, bool in, bool yield);

  ir::Node* ParseBindingPattern(bool yield, bool generator_parameter);

  ir::Node* ParseObjectBindingPattern(bool yield, bool generator_parameter);

  ir::Node* ParseArrayBindingPattern(bool yield, bool generator_parameter);

  ir::Node* ParseBindingProperty(bool yield, bool generator_parameter);

  ir::Node* ParseBindingElement(bool yield, bool generator_parameter);

  ir::Node* ParseBindingIdentifier(bool default_allowed, bool in, bool yield);

  ir::Node* ParseVariableStatement(bool in, bool yield);
  
  ir::Node* ParseVariableDeclaration(bool in, bool yield);

  ir::Node* ParseIfStatement(bool yield, bool has_return);

  ir::Node* ParseWhileStatement(bool yield, bool has_return);

  ir::Node* ParseDoWhileStatement(bool yield, bool has_return);

  ir::Node* ParseForStatement(bool yield, bool has_return);

  ir::Node* ParseForIteration(ir::Node* reciever, TokenCursor, bool yield, bool has_return);

  ir::Node* ParseIterationBody(bool yield, bool has_return);

  ir::Node* ParseContinueStatement(bool yield);

  ir::Node* ParseBreakStatement(bool yield);

  ir::Node* ParseReturnStatement(bool yield);

  ir::Node* ParseWithStatement(bool yield, bool has_return);

  ir::Node* ParseSwitchStatement(bool yield, bool has_return);

  ir::Node* ParseCaseClauses(bool yield, bool has_return);

  ir::Node* ParseLabelledStatement();

  ir::Node* ParseThrowStatement();

  ir::Node* ParseTryStatement(bool yield, bool has_return);

  ir::Node* ParseCatchBlock(bool yield, bool has_return);

  ir::Node* ParseFinallyBlock(bool yield, bool has_return);

  ir::Node* ParseClassDeclaration(bool yield, bool has_default);

  ir::Node* ParseFunction(bool yield, bool has_default);

  ir::Node* ParseParameterList(bool accesslevel_allowed);
  
  ir::Node* ParseParameter(bool rest, bool accesslevel_allowed);

  ir::Node* ParseFunctionBody(bool yield);

  ir::Node* ParseTypeExpression();

  ir::Node* ParseReferencedType();

  ir::Node* ParseGenericType();

  ir::Node* ParseTypeArguments();

  ir::Node* ParseTypeParameters();

  ir::Node* ParseTypeQueryExpression();

  ir::Node* ParseArrayType(ir::Node* type_expr);

  ir::Node* ParseObjectTypeExpression();

  ir::Node* ParseObjectTypeElement();

  ir::Node* ParseCallSignature(bool accesslevel_allowed);
  
  // Parse expression.
  ir::Node* ParseExpression(bool in, bool yield);

  // Parse destructuring assignment.
  ir::Node* ParseAssignmentPattern(bool yield);

  // Parse destructuring assignment object pattern.
  ir::Node* ParseObjectAssignmentPattern(bool yield);

  // Parse destructuring assignment array pattern.
  // To simplify, we parse AssignmentElementList together.
  ir::Node* ParseArrayAssignmentPattern(bool yield);

  // Parse destructuring assignment object pattern properties.
  ir::Node* ParseAssignmentPropertyList(bool yield);

  // Parse destructuring assignment object pattern property.
  ir::Node* ParseAssignmentProperty(bool yield);

  // Parse destructuring assignment array pattern element.
  ir::Node* ParseAssignmentElement(bool yield);

  // Parse destructuring assignment array pattern rest element.
  ir::Node* ParseAssignmentRestElement(bool yield);

  // Parse destructuring assignment target node.
  ir::Node* ParseDestructuringAssignmentTarget(bool yield);

  // Parse assignment expression.
  ir::Node* ParseAssignmentExpression(bool in, bool yield);

  ir::Node* ParseArrowFunction(bool in, bool yield, ir::Node* identifier = nullptr);

  ir::Node* ParseArrowFunctionParameters(bool yield, ir::Node* identifier = nullptr);

  ir::Node* ParseConciseBody(bool in, ir::Node* call_sig);

  // Parse conditional expression.
  ir::Node* ParseConditionalExpression(bool in, bool yield);

  // Parse binary expression.
  // To simplify, we parser all binary expression(like MultiplicativeExpression AdditiveExpression, etc...) together.
  ir::Node* ParseBinaryExpression(bool in, bool yield);

  // Parse unary expression.
  ir::Node* ParseUnaryExpression(bool yield);

  // Parse postfix expression.
  ir::Node* ParsePostfixExpression(bool yield);

  ir::Node* ParseLeftHandSideExpression(bool yield);

  // Parse member expression.
  ir::Node* ParseMemberExpression(bool yield);

  // Parser getprop or getelem expression.
  ir::Node* ParseGetPropOrElem(ir::Node* node, bool yield);

  ir::Node* ParseCallExpression(bool yield);

  ir::Node* ParseArguments(bool yield);

  ir::Node* ParsePrimaryExpression(bool yield);

  ir::Node* ParseArrayLiteral(bool yield);

  ir::Node* ParseSpreadElement(bool yield);

  ir::Node* ParseArrayComprehension(bool yield);

  ir::Node* ParseComprehension(bool yield);

  ir::Node* ParseComprehensionTail(bool yield);

  ir::Node* ParseComprehensionFor(bool yield);

  ir::Node* ParseComprehensionIf(bool yield);

  ir::Node* ParseGeneratorComprehension(bool yield);

  ir::Node* ParseYieldExpression(bool in);

  ir::Node* ParseForBinding(bool yield);

  ir::Node* ParseObjectLiteral(bool yield);

  ir::Node* ParsePropertyDefinition(bool yield);

  ir::Node* ParsePropertyName(bool yield, bool generator_parameter);

  ir::Node* ParseLiteralPropertyName();

  ir::Node* ParseComputedPropertyName(bool yield);

  ir::Node* ParseLiteral();

  ir::Node* ParseValueLiteral();

  ir::Node* ParseArrayInitializer(bool yield);

  ir::Node* ParseIdentifierReference(bool yield);

  ir::Node* ParseBindingIdentifier(bool default_allowed, bool yield);

  ir::Node* ParseLabelIdentifier(bool yield);

  ir::Node* ParseIdentifier();

  ir::Node* ParseStringLiteral();

  ir::Node* ParseNumericLiteral();

  ir::Node* ParseBooleanLiteral();

  ir::Node* ParseUndefinedLiteral();

  ir::Node* ParseNaNLiteral();

  ir::Node* ParseRegularExpression();

  ir::Node* ParseTemplateLiteral();

  ir::Node* ParseEmptyStatement();

  bool IsLineTermination();

  void ConsumeLineTerminator();

#ifdef DEBUG
  void PrintStackTrace() {
    Printf("%s\n", phase_buffer_.str().c_str());
  }
#endif

 private:
#ifdef DEBUG
  std::stringstream phase_buffer_;
#endif
  Scanner<UCharInputSourceIterator>* scanner_;
  ParserState parser_state_;
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
