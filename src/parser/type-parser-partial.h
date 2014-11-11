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
// type_parameters
//   : '<' type_parameter_list '>'
//   ;
// type_parameter_list
//   : type_parameter 
//   | type_parameter_list , type_parameter
//   ;
// type_parameter
//   : identifier constraint__opt
//   ;
// constraint
//   : 'extends' type
//   ;

template <typename UCharInputIterator>
ParseResult Parser<UCharInputIterator>::ParseTypeParameters() {
  LOG_PHASE(ParseTypeParameters);

  auto type_params = New<ir::TypeParametersView>();
  type_params->SetInformationForNode(Current());
  EnableNestedGenericTypeScanMode();
  YATSC_SCOPED([&] {DisableNestedGenericTypeScanMode();});
  Next();
  bool found = false;

  while (1) {
    if (Current()->type() == Token::TS_IDENTIFIER) {
      found = true;
      auto identifier_result = ParseIdentifier();
      CHECK_AST(identifier_result);
      if (Current()->type() == Token::TS_EXTENDS) {
        Next();
        auto ref_type_result = ParseReferencedType();
        CHECK_AST(ref_type_result);
        Handle<ir::Node> type_constraints = New<ir::TypeConstraintsView>(identifier_result.value(),
                                                                         ref_type_result.value());
        type_constraints->SetInformationForNode(identifier_result.value());
        type_params->InsertLast(type_constraints);
      } else {
        type_params->InsertLast(identifier_result.value());
      }
    } else if (Current()->type() == Token::TS_GREATER) {
      if (!found) {
        SYNTAX_ERROR("SyntaxError need type parameter", Current());
      }
      Next();
      return Success(type_params);
    } else if (Current()->type() == Token::TS_COMMA) {
      Next();
    } else {
      SYNTAX_ERROR("SyntaxError unexpected token", Current());
    }
  }
}


// type
//   : predefined_type 
//   | type_reference 
//   | type_query 
//   | type_literal
//   ;
//
// pre_defined_type 
//   : 'any'
//   | 'number'
//   | 'boolean'
//   | 'string'
//   | 'void'
//   ;
// type_literal 
//   : object_type 
//   | array_type 
//   | function_type 
//   | constructor_type
//   ;
template <typename UCharInputIterator>
ParseResult Parser<UCharInputIterator>::ParseTypeExpression() {
  LOG_PHASE(ParseTypeExpression);
  if (Current()->type() == Token::TS_VOID) {
    Current()->set_type(Token::TS_IDENTIFIER);
  }
  if (Current()->type() == Token::TS_NEW) {
    Next();
    auto call_sig_result = ParseCallSignature(false, true);
    CHECK_AST(call_sig_result);
    Handle<ir::Node> ret = New<ir::ConstructSignatureView>(call_sig_result.value());
    ret->SetInformationForNode(call_sig_result.value());
    return Success(ret);
  } else if (Current()->type() == Token::TS_TYPEOF) {
    auto type_query_result = ParseTypeQueryExpression();
    CHECK_AST(type_query_result);
    return ParseArrayType(type_query_result.value());
  } else if (Current()->type() == Token::TS_IDENTIFIER) {
    Current()->set_type(Token::TS_IDENTIFIER);
    auto ref_type_result = ParseReferencedType();
    CHECK_AST(ref_type_result);
    return ParseArrayType(ref_type_result.value());
  } else if (Current()->type() == Token::TS_LEFT_PAREN ||
             Current()->type() == Token::TS_LESS) {
    ParseResult type_params_result;
    if (Current()->type() == Token::TS_LESS) {
      type_params_result = ParseTypeParameters();
      CHECK_AST(type_params_result);
    }
    auto param_list_result = ParseParameterList(false);
    CHECK_AST(param_list_result);
    
    if (Current()->type() == Token::TS_ARROW_GLYPH) {
      Next();
      auto type_expr_result = ParseTypeExpression();
      CHECK_AST(type_expr_result);
      Handle<ir::Node> ft = New<ir::FunctionTypeExprView>(param_list_result.value(),
                                                          type_expr_result.value(),
                                                          type_params_result.value());
      ft->SetInformationForNode(param_list_result.value());
      return ParseArrayType(ft);
    }
    SYNTAX_ERROR("SyntaxError '=>' expected", Current());
  } else if (Current()->type() == Token::TS_LEFT_BRACE) {
    auto object_type_result = ParseObjectTypeExpression();
    CHECK_AST(object_type_result);
    return ParseArrayType(object_type_result.value());
  }
  SYNTAX_ERROR("SyntaxError unexpected token", Current());
}


// type_reference 
//   : type_name [no line_terminator here] type_arguments__opt
//   ;
// type_name
//   : identifier 
//   | module_name '.' identifier
//   ;
// module_name
//   : identifier
//   | module_name '.' identifier
//   ;
template <typename UCharInputIterator>
ParseResult Parser<UCharInputIterator>::ParseReferencedType() {
  LOG_PHASE(ParseReferencedType);
  if (Current()->type() == Token::TS_IDENTIFIER) {
    auto primary_expr_result = ParsePrimaryExpression(false);
    CHECK_AST(primary_expr_result);
    if (Current()->type() == Token::TS_DOT) {
      primary_expr_result = ParseGetPropOrElem(primary_expr_result.value(), false, true, false);
      CHECK_AST(primary_expr_result);
    }
    
    if (!Current()->has_line_break_before_next() && Current()->type() == Token::TS_LESS) {
      auto type_arguments_result = ParseTypeArguments();
      CHECK_AST(type_arguments_result);
      Handle<ir::Node> ret = New<ir::GenericTypeExprView>(primary_expr_result.value(),
                                                          type_arguments_result.value());
      ret->SetInformationForNode(primary_expr_result.value());
      return Success(ret);
    }

    Handle<ir::Node> ret = New<ir::SimpleTypeExprView>(primary_expr_result.value());
    ret->SetInformationForNode(primary_expr_result.value());
    return Success(ret);
  }

  SYNTAX_ERROR("SyntaxError identifier expected", Current());
}


// type_query
//   : 'typeof' type_query_expression
//   ;
// type_query_expression: 
//   : identifier
//   | type_query_expression '.' identifier_name
//   ;
template <typename UCharInputIterator>
ParseResult Parser<UCharInputIterator>::ParseTypeQueryExpression() {
  LOG_PHASE(ParseTypeQueryExpression);
  Next();
  if (Current()->type() == Token::TS_IDENTIFIER) {
    auto primary_expr_result = ParsePrimaryExpression(false);
    CHECK_AST(primary_expr_result);
    if (Current()->type() == Token::TS_DOT) {
      primary_expr_result = ParseGetPropOrElem(primary_expr_result.value(), false, true, false);
      CHECK_AST(primary_expr_result);
    }
    Handle<ir::Node> ret = New<ir::TypeQueryView>(primary_expr_result.value());
    ret->SetInformationForNode(primary_expr_result.value());
    return Success(ret);
  }
  SYNTAX_ERROR("SyntaxError identifier expected", Current());
}


// type_arguments 
//   : '<' type_argument_list '>'
//   ;
// type_argument_list
//   : type_argument 
//   | type_argument_list ',' type_argument
//   ;
// type_argument
//   : type
//   ;
template <typename UCharInputIterator>
ParseResult Parser<UCharInputIterator>::ParseTypeArguments() {
  LOG_PHASE(ParseTypeArguments);

  Handle<ir::TypeArgumentsView> type_arguments = New<ir::TypeArgumentsView>();
  type_arguments->SetInformationForNode(Current());
  EnableNestedGenericTypeScanMode();
  YATSC_SCOPED([&] {DisableNestedGenericTypeScanMode();});
  Next();

  bool success = true;
    
  while (1) {
    auto type_expr_result = ParseTypeExpression();
    CHECK_AST(type_expr_result);
    type_arguments->InsertLast(type_expr_result.value());
    if (Current()->type() == Token::TS_COMMA) {
      Next();
    } else if (Current()->type() == Token::TS_GREATER) {
      Next();
      return Success(type_arguments);
    } else {
      SYNTAX_ERROR("SyntaxError '>' or ',' expected", Current());
    }
  }
}


// array_type
//   : element_type [no LineTerminator here] '[' ']'
//   ;
// element_type
//   : pre_defined_type
//   | type_reference
//   | type_query
//   | object_type
//   | array_type
//   ;
template <typename UCharInputIterator>
ParseResult Parser<UCharInputIterator>::ParseArrayType(Handle<ir::Node> type_expr) {
  if (Current()->type() == Token::TS_LEFT_BRACKET) {
    Next();
    if (Current()->type() == Token::TS_RIGHT_BRACKET) {
      Handle<ir::Node> array_type = New<ir::ArrayTypeExprView>(type_expr);
      array_type->SetInformationForNode(Current());
      Next();
      return ParseArrayType(array_type);
    }
    SYNTAX_ERROR("SyntaxError ']' expected", Current());
  }
  return Success(type_expr);
}


// object_type
//   : '{' type_body__opt '}'
//   ;
// type_body
//   : type_member_list ';'__opt
//   ;
// type_member_list
//   : type_member
//   | type_member_list ';' type_member
//   ;
template <typename UCharInputIterator>
ParseResult Parser<UCharInputIterator>::ParseObjectTypeExpression() {
  LOG_PHASE(ParseObjectTypeExpression);
  if (Current()->type() == Token::TS_LEFT_BRACE) {
    Next();
    Handle<ir::ObjectTypeExprView> object_type = New<ir::ObjectTypeExprView>();
    object_type->SetInformationForNode(Current());

    bool success = true;
    
    while (Current()->type() != Token::TS_RIGHT_BRACE) {
      auto object_element_result = ParseObjectTypeElement();
      SKIP_TOKEN_OR(object_element_result, success, Token::TS_RIGHT_BRACE) {
        object_type->InsertLast(object_element_result.value());
      }
      if (IsLineTermination()) {
        ConsumeLineTerminator();
      } else if (Current()->type() != Token::TS_RIGHT_BRACE &&
                 Prev()->type() != Token::TS_RIGHT_BRACE) {
        SYNTAX_ERROR("SyntaxError ';' expected", Prev());
      }
    }
    Next();
    return Success(object_type);
  }
  SYNTAX_ERROR("SyntaxError '{' expected", Current());
}


// type_member
//   : property_signature
//   | call_signature
//   | construct_signature
//   | index_signature
//   | method_signature
//   ;
template <typename UCharInputIterator>
ParseResult Parser<UCharInputIterator>::ParseObjectTypeElement() {
  LOG_PHASE(ParseObjectTypeElement);
  if (Current()->type() == Token::TS_NEW) {
    Next();
    auto call_sig_result = ParseCallSignature(false, true);
    CHECK_AST(call_sig_result);
    Handle<ir::Node> ret = New<ir::ConstructSignatureView>(call_sig_result.value());
    ret->SetInformationForNode(call_sig_result.value());
    return Success(ret);
  } else if (Current()->type() == Token::TS_LEFT_PAREN ||
             Current()->type() == Token::TS_LESS) {
    return ParseCallSignature(false, true);
  } else if (Current()->type() == Token::TS_LEFT_BRACKET) {
    return ParseIndexSignature();
  } else {
    bool optional = false;
    bool generator = false;
    AccessorType at = ParseAccessor();
    ParseResult key_result;

    if (Current()->type() == Token::TS_MUL) {
      generator = true;
      Next();
    }

    TokenInfo info = *Current();

    if (TokenInfo::IsKeyword(Current()->type())) {
      Current()->set_type(Token::TS_IDENTIFIER);
    }
    if (Current()->type() == Token::TS_IDENTIFIER) {
      key_result = ParseIdentifierReference(false);
    } else {
      key_result = ParsePropertyName(false, false);
    }

    if (!key_result) {
      if (at.getter || at.setter) {
        key_result = Success(New<ir::NameView>(NewSymbol(ir::SymbolType::kPropertyName, info.value())));
        key_result.value()->SetInformationForNode(&info);
      } else {
        SYNTAX_ERROR("identifier expected.", (&info));
      }
    }
    
    if (Current()->type() == Token::TS_QUESTION_MARK) {
      optional = true;
      Next();
    }
    
    if (Current()->type() == Token::TS_LEFT_PAREN || Current()->type() == Token::TS_LESS) {
      if (!key_result.value()->HasNameView()) {
        SYNTAX_ERROR("SyntaxError invalid method name", key_result.value());
      }
      auto call_sig_result = ParseCallSignature(false, false);
      CHECK_AST(call_sig_result);
      auto ret = New<ir::MethodSignatureView>(optional, at.getter, at.setter, generator, key_result.value(), call_sig_result.value());
      ret->SetInformationForNode(key_result.value());
      return Success(ret);
    } else if (Current()->type() == Token::TS_COLON) {
      Next();
      auto type_expr_result = ParseTypeExpression();
      CHECK_AST(type_expr_result);
      auto ret = New<ir::PropertySignatureView>(optional, key_result.value(), type_expr_result.value());
      ret->SetInformationForNode(type_expr_result.value());
      return Success(ret);
    }
    Handle<ir::Node> ret = New<ir::PropertySignatureView>(optional, key_result.value(), ir::Node::Null());
    ret->SetInformationForNode(key_result.value());
    return Success(ret);
  }
  SYNTAX_ERROR("SyntaxError unexpected token", Current());
}


template <typename UCharInputIterator>
ParseResult Parser<UCharInputIterator>::ParseCallSignature(bool accesslevel_allowed, bool annotation) {
  LOG_PHASE(ParseCallSignature);
  ParseResult type_parameters_result;
  
  if (Current()->type() == Token::TS_LESS) {
    type_parameters_result = ParseTypeParameters();
    CHECK_AST(type_parameters_result);
  }
  
  if (Current()->type() == Token::TS_LEFT_PAREN) {
    TokenInfo token = (*Current());
    auto param_list_result = ParseParameterList(accesslevel_allowed);
    CHECK_AST(param_list_result);
    ParseResult return_type_result;
    if (Current()->type() == Token::TS_COLON) {
      Next();
      return_type_result = ParseTypeExpression();
      if (!return_type_result) {
        return_type_result = ParseResult();
      }
    } else if (annotation) {
      if (Current()->type() == Token::TS_ARROW_GLYPH) {
        Next();
        return_type_result = ParseTypeExpression();
        if (!return_type_result) {
          return_type_result = ParseResult();
        }
      } else {
        SYNTAX_ERROR("SyntaxError '=>' expected.", Current());
      }
    }
    
    auto ret = New<ir::CallSignatureView>(param_list_result.value(),
                                          return_type_result.value(),
                                          type_parameters_result.value());
    ret->SetInformationForNode(&token);
    return Success(ret);
  }
  SYNTAX_ERROR("SyntaxError expected '('", Current());
}


template <typename UCharInputIterator>
ParseResult Parser<UCharInputIterator>::ParseIndexSignature() {
  if (Current()->type() == Token::TS_LEFT_BRACKET) {
    Next();
    auto identifier_result = ParseIdentifier();
    CHECK_AST(identifier_result);
    if (Current()->type() == Token::TS_COLON) {
      Next();
      if (Current()->type() == Token::TS_IDENTIFIER) {
        bool string_type = Current()->value()->Equals("string");
        bool number_type = Current()->value()->Equals("number");
        if (string_type || number_type) {
          Next();
          if (Current()->type() == Token::TS_RIGHT_BRACKET) {
            Next();
            if (Current()->type() == Token::TS_COLON) {
              Next();
              auto type_expr_result = ParseTypeExpression();
              CHECK_AST(type_expr_result);
              return Success(New<ir::IndexSignatureView>(identifier_result.value(), type_expr_result.value(), string_type));
            }
            SYNTAX_ERROR("SyntaxError ':' expected.", Current());
          }
          SYNTAX_ERROR("SyntaxError ']' expected.", Current());
        }        
        SYNTAX_ERROR("SYNTAX_ERROR The IndexSignature must have a type 'string' or 'number'.", Current());
      }
      SYNTAX_ERROR("SYNTAX_ERROR The IndexSignature must have a simple identifier.", Current());
    }
    SYNTAX_ERROR("SYNTAX_ERROR The IndexSignature must have a type.", Current());
  }
  SYNTAX_ERROR("SyntaxError '[' expected.", Current());
}
}
