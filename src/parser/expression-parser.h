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

#ifndef PARSER_EXPRESSION_PARSER_H
#define PARSER_EXPRESSION_PARSER_H


#include "./parser-base.h"
#include "./parser-traits.h"

namespace yatsc {

/**
 * The expression parser traits.
 * This class parse expressions, but to parser expression,
 * need below requirements.
 * Module must be implemented follow methods.
 *
 * TokenInfo* Next();
 * TokenInfo* Current();
 * void RewindBuffer(size_t num);
 * TokenInfo* PeekBuffer(size_t num);
 */
template <typename Module>
class ExpressionParser {
  YATSC_CHECK_CONSTRAINTS(Module, ParserConstraints);  
 public:
  ExpressionParser(Module* module);

  ir::Node* Parse();
  
 VISIBLE_FOR_TESTING:
  ir::Node* ParseExpression(bool in, bool yield);

  ir::Node* ParseAssignmentPattern(bool yield);

  ir::Node* ParseObjectAssignmentPattern(bool yield);

  ir::Node* ParseArrayAssignmentPattern(bool yield);

  ir::Node* ParseAssignmentPropertyList(bool yield);

  ir::Node* ParseAssignmentElementList(bool yield);

  ir::Node* ParseAssignmentElisionElement(bool yield);

  ir::Node* ParseAssignmentProperty(bool yield);

  ir::Node* ParseAssignmentElement(bool yield);

  ir::Node* ParseAssignmentRestElement(bool yield);

  ir::Node* ParseDestructuringAssignmentTarget(bool yield);

  ir::Node* ParseAssignmentExpression(bool in, bool yield);

  ir::Node* ParseConditionalExpression(bool in, bool yield);

  ir::Node* ParseBinaryExpression(bool in, bool yield);

  ir::Node* ParseUnaryExpression(bool yield);

  ir::Node* ParsePostfixExpression(bool yield);

  ir::Node* ParseMemberExpression(bool yield);

  ir::Node* ParseNewExpression(bool yield);

  ir::Node* ParseCallExpression(bool yield);

  ir::Node* ParseArguments(bool yield);

  ir::Node* ParseArgumentList(bool yield);

  ir::Node* ParseLeftHandSideExpression(bool yield);

  ir::Node* ParsePrimaryExpression(bool yield);

  ir::Node* ParseArrayLiteral(bool yield);

  ir::Node* ParseElementList(bool yield);

  ir::Node* ParseElision(bool yield);

  ir::Node* ParseSpreadElement(bool yield);

  ir::Node* ParseArrayComprehension(bool yield);

  ir::Node* ParseComprehension(bool yield);

  ir::Node* ParseComprehensionTail(bool yield);

  ir::Node* ParseComprehensionFor(bool yield);

  ir::Node* ParseComprehensionIf(bool yield);

  ir::Node* ParseGeneratorComprehension(bool yield);

  ir::Node* ParseForBinding(bool yield);

  ir::Node* ParseObjectLiteral(bool yield);

  ir::Node* ParseDefinitionList(bool yield);

  ir::Node* ParsePropertyDefinition(bool yield);

  ir::Node* ParsePropertyName(bool yield);

  ir::Node* ParseLiteralPropertyName();

  ir::Node* ParseComputedPropertyName(bool yield);

  ir::Node* ParseCoverInitializedName(bool yield);

  ir::Node* ParseInitializer(bool in, bool yield);

  ir::Node* ParseLiteral();

  ir::Node* ParseValueLiteral();

  ir::Node* ParseIdentifierReference(bool yield);

  ir::Node* ParseBindingIdentifier(bool def, bool yield);

  ir::Node* ParseLabelIdentifier(bool yield);

  ir::Node* ParseIdentifier();
  
 private:
  Module* module_;
};
} // yatsc

#include "./expression-parser-inl.h"

#endif // PARSER_EXPRESSION_PARSER_H
