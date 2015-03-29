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


#ifndef YATSC_PARSER_BARE_PARSER_H
#define YATSC_PARSER_BARE_PARSER_H

namespace yatsc {

// This parser is a base parser of the all other parser.
template <typename SemanticAction, typename ParseResult, typename ScannerConnector>
class BareParser {
 public:
  BareParser(ScannerConnector* scanner_connector);

//   ParseResult ParseStatementListItem();

//   ParseResult ParseStatementList();

//   ParseResult ParseStatement();

//   ParseResult ParseBlockStatement();

//   ParseResult ParseModuleStatement();

//   ParseResult ParseImportStatement();

//   ParseResult ParseExportStatement();

//   ParseResult ParseDeclaration(bool error);
  
//   ParseResult ParseDebuggerStatement();

//   ParseResult ParseLexicalDeclaration();

//   ParseResult ParseLexicalBinding(bool const_decl);

//   ParseResult ParseBindingPattern();

//   ParseResult ParseObjectBindingPattern();

//   ParseResult ParseArrayBindingPattern();

//   ParseResult ParseBindingProperty();

//   ParseResult ParseBindingElement();

//   ParseResult ParseBindingIdentifier();

//   ParseResult ParseVariableStatement();
  
//   ParseResult ParseVariableDeclaration();

//   ParseResult ParseIfStatement();

//   ParseResult ParseWhileStatement();

//   ParseResult ParseDoWhileStatement();

//   ParseResult ParseForStatement();

//   ParseResult ParseForIteration(Handle<ir::Node> reciever, Token*);

//   ParseResult ParseIterationBody();

//   ParseResult ParseContinueStatement();

//   ParseResult ParseBreakStatement();

//   ParseResult ParseReturnStatement();

//   ParseResult ParseWithStatement();

//   ParseResult ParseSwitchStatement();

//   ParseResult ParseCaseClauses();

//   ParseResult ParseLabelledStatement();

//   ParseResult ParseLabelledItem();

//   ParseResult ParseThrowStatement();

//   ParseResult ParseTryStatement();

//   ParseResult ParseCatchBlock();

//   ParseResult ParseFinallyBlock();

//   ParseResult ParseInterfaceDeclaration();

//   ParseResult ParseEnumDeclaration();

//   ParseResult ParseEnumBody();

//   ParseResult ParseEnumProperty();

//   Handle<ir::Node> CreateEnumFieldView(Handle<ir::Node> name, Handle<ir::Node> value);
  
//   ParseResult ParseClassDeclaration();

//   ParseResult ParseClassBases();

//   ParseResult ParseClassBody();

//   ParseResult ParseClassElement();

//   ParseResult ParseFieldModifiers();
  
//   ParseResult ParseFieldModifier();

//   bool IsAccessLevelModifier(Token* token) {return token->OneOf({TokenKind::kPublic, TokenKind::kProtected, TokenKind::kPrivate});};

//   ParseResult ParseConstructorOverloads(Handle<ir::Node> mods);

//   ParseResult ParseConstructorOverloadOrImplementation(bool first, Handle<ir::Node> mods, Handle<ir::Node> overloads);

//   bool IsMemberFunctionOverloadsBegin(Token* info);
  
//   ParseResult ParseMemberFunctionOverloads(Handle<ir::Node> mods, AccessorType* at);

//   ParseResult ParseMemberFunctionOverloadOrImplementation(
//       bool first, Handle<ir::Node> mods, AccessorType* at, Handle<ir::Node> overloads);

//   ParseResult ParseGeneratorMethodOverloads(Handle<ir::Node> mods);

//   ParseResult ParseGeneratorMethodOverloadOrImplementation(bool first, Handle<ir::Node> mods, Handle<ir::Node> overloads);

//   ParseResult ParseMemberVariable(Handle<ir::Node> mods);

//   ParseResult ParseFunctionOverloads(bool declaration, bool is_export);

//   ParseResult ParseFunctionOverloadOrImplementation(Handle<ir::Node> overloads, bool declaration);
 
//   ParseResult ParseParameterList(bool accesslevel_allowed);

//   ParseResult ParseRestParameter(bool accesslevel_allowed);
  
//   ParseResult ParseParameter(bool rest, bool accesslevel_allowed);

//   ParseResult ParseFunctionBody(bool generator);

//   ParseResult ParseTypeExpression();

//   ParseResult ParseType();

//   ParseResult ParseReferencedType();

//   ParseResult ParseGenericType();

//   ParseResult ParseTypeArguments();

//   ParseResult ParseTypeParameters();

//   ParseResult ParseTypeParameter();

//   ParseResult ParseTypeQueryExpression();

//   ParseResult ParseArrayType(Handle<ir::Node> type_expr);

//   ParseResult ParseObjectTypeExpression();

//   ParseResult ParseObjectTypeElement();

//   ParseResult ParseCallSignature(bool accesslevel_allowed, bool type_annotation, bool arrow_glyph_expected);

//   ParseResult ParseIndexSignature();

//   // Parse expression.
//   ParseResult ParseCoveredExpression();

//   ParseResult ParseCoveredTypeExpression();

//   ParseResult ParseCoveredExpressionSuffix(bool invalid_arrow_param, bool has_types, Handle<ir::Node> type_arguments, const ir::Node::List& covered_expr_node_list);

//   bool IsParsibleAsArrowFunctionFormalParameterList();

//   // Parse expression.
//   ParseResult ParseAsArrowFunction(Handle<ir::Node> type_list, const ir::Node::List&, Handle<ir::Node> ret_type);

//   // Parse expression.
//   ParseResult ParseAsTypeAssertion(Handle<ir::Node> type_list, const ir::Node::List&);

//   // Parse expression.
//   ParseResult ParseAsExpression(const ir::Node::List&);
  
//   // Parse expression.
//   ParseResult ParseExpression();

//   // Parse destructuring assignment.
//   ParseResult ParseAssignmentPattern();

//   // Parse destructuring assignment object pattern.
//   ParseResult ParseObjectAssignmentPattern();

//   // Parse destructuring assignment array pattern.
//   // To simplify, we parse AssignmentElementList together.
//   ParseResult ParseArrayAssignmentPattern();

//   // Parse destructuring assignment object pattern properties.
//   ParseResult ParseAssignmentPropertyList();

//   // Parse destructuring assignment object pattern property.
//   ParseResult ParseAssignmentProperty();

//   // Parse destructuring assignment array pattern element.
//   ParseResult ParseAssignmentElement();

//   // Parse destructuring assignment array pattern rest element.
//   ParseResult ParseAssignmentRestElement();

//   // Parse destructuring assignment target node.
//   ParseResult ParseDestructuringAssignmentTarget();

//   // Parse assignment expression.
//   ParseResult ParseAssignmentExpression();

//   ParseResult ParseArrowFunction(Handle<ir::Node> identifier);

//   ParseResult ParseArrowFunctionParameters(Handle<ir::Node> identifier);

//   ParseResult ParseConciseBody(Handle<ir::Node> call_sig);

//   // Parse conditional expression.
//   ParseResult ParseConditionalExpression();

// #define DEF_PARSE_BINARY_EXPR(name)                         \
//   ParseResult Parse##name##Expression();

//   DEF_PARSE_BINARY_EXPR(LogicalOR);
//   DEF_PARSE_BINARY_EXPR(LogicalAND);
//   DEF_PARSE_BINARY_EXPR(BitwiseOR);
//   DEF_PARSE_BINARY_EXPR(BitwiseXOR);
//   DEF_PARSE_BINARY_EXPR(BitwiseAND);
//   DEF_PARSE_BINARY_EXPR(Equality);
//   DEF_PARSE_BINARY_EXPR(Relational);
//   DEF_PARSE_BINARY_EXPR(Shift);
//   DEF_PARSE_BINARY_EXPR(Additive);
//   DEF_PARSE_BINARY_EXPR(Multiplicative);
// #undef DEF_PARSE_BINARY_EXPR

//   // Parse unary expression.
//   ParseResult ParseUnaryExpression();

//   // Parse postfix expression.
//   ParseResult ParsePostfixExpression();

//   ParseResult ParseLeftHandSideExpression();

//   // Parse new expression.
//   ParseResult ParseNewExpression();
  
//   // Parse member expression.
//   ParseResult ParseMemberExpression();

//   // Parser getprop or getelem expression.
//   ParseResult ParseGetPropOrElem(Handle<ir::Node> node, bool dot_only, bool is_error);

//   ParseResult ParseCallExpression();

//   ParseResult BuildArguments(ParseResult type_arguments_result, Handle<ir::Node> args);
  
//   ParseResult ParseArguments();

//   ParseResult ParsePrimaryExpression();

//   ParseResult ParseArrayLiteral();

//   ParseResult ParseSpreadElement();

//   ParseResult ParseArrayComprehension();

//   ParseResult ParseComprehension(bool generator);

//   ParseResult ParseComprehensionTail();

//   ParseResult ParseComprehensionFor();

//   ParseResult ParseComprehensionIf();

//   ParseResult ParseGeneratorComprehension();

//   ParseResult ParseYieldExpression();

//   ParseResult ParseForBinding();

//   ParseResult ParseObjectLiteral();

//   ParseResult ParsePropertyDefinition();

//   ParseResult ParsePropertyName();

//   ParseResult ParseLiteralPropertyName();

//   ParseResult ParseComputedPropertyName();

//   ParseResult ParseLiteral();

//   ParseResult ParseArrayInitializer();

//   ParseResult ParseIdentifierReference();

//   ParseResult ParseLabelIdentifier();

  ParseResult ParseIdentifier();

//   ParseResult ParseStringLiteral();

//   ParseResult ParseNumericLiteral();

//   ParseResult ParseBooleanLiteral();

//   ParseResult ParseUndefinedLiteral();

//   ParseResult ParseNaNLiteral();

//   ParseResult ParseRegularExpression();

//   ParseResult ParseTemplateLiteral();

//   ParseResult ParseEmptyStatement();

//   ParseResult ParseModule();

//   ParseResult ParseImportDeclaration();

//   ParseResult ParseExternalModuleReference();

//   ParseResult ParseImportClause();

//   ParseResult ParseNamedImport();

//   ParseResult ParseFromClause();

//   ParseResult ParseModuleImport();

//   ParseResult ParseTSModule(Handle<ir::Node> identifier, Token* token_info);

//   ParseResult ParseTSModuleBody();

//   ParseResult ParseExportDeclaration();

//   Handle<ir::Node> CreateExportView(
//       Handle<ir::Node> export_clause,
//       Handle<ir::Node> from_clause,
//       Token* token_info,
//       bool default_export);

//   ParseResult ParseExportClause();

//   Handle<ir::Node> CreateNamedExportView(
//       Handle<ir::Node> identifier,
//       Handle<ir::Node> binding);


//   // Ambient
//   ParseResult ParseDeclarationModule();
  
//   ParseResult ParseAmbientDeclaration(bool module_allowed);

//   ParseResult ParseAmbientVariableDeclaration(Token* info);

//   ParseResult ParseAmbientFunctionDeclaration(Token* info);

//   ParseResult ParseAmbientClassDeclaration(Token* info);

//   ParseResult ParseAmbientClassBody();

//   ParseResult ParseAmbientClassElement();

//   ParseResult ParseAmbientConstructor(Handle<ir::Node> mods);

//   ParseResult ParseAmbientMemberFunction(Handle<ir::Node> mods, AccessorType* acessor_type);

//   ParseResult ParseAmbientGeneratorMethod(Handle<ir::Node> mods);

//   ParseResult ParseAmbientMemberVariable(Handle<ir::Node> mods);

//   ParseResult ParseAmbientEnumDeclaration(Token* info);

//   ParseResult ParseAmbientEnumBody();

//   ParseResult ParseAmbientEnumProperty();

//   Handle<ir::Node> CreateAmbientEnumFieldView(Handle<ir::Node> name, Handle<ir::Node> value);

//   ParseResult ParseAmbientModuleDeclaration(Token* info);

//   ParseResult ParseAmbientModuleBody(bool external);

//   ParseResult ParseAmbientModuleElement(bool external);

 private:

  YATSC_INLINE Token* token() {return current_token_;}


  YATSC_INLINE void ConsumeToken() {current_token_ = scanner_connector_->Scan();}


  YATSC_INLINE Token* Peek() {return scanner_connector_->Peek();}
  

  Token* current_token_;
  ScannerConnector* scanner_connector_;
  SemanticAction sem_action_;
};

}

#endif
