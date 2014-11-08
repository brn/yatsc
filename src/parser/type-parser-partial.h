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
Handle<ir::Node> Parser<UCharInputIterator>::ParseTypeParameters() {
  LOG_PHASE(ParseTypeParameters);
  if (Current()->type() == Token::TS_LESS) {
    Handle<ir::TypeParametersView> type_params = New<ir::TypeParametersView>();
    type_params->SetInformationForNode(Current());
    EnableNestedGenericTypeScanMode();
    YATSC_SCOPED([&] {DisableNestedGenericTypeScanMode();});
    Next();
    bool found = false;
    while (1) {
      if (Current()->type() == Token::TS_IDENTIFIER) {
        found = true;
        Handle<ir::Node> name = ParseIdentifier();
        CHECK_AST(name);
        if (Current()->type() == Token::TS_EXTENDS) {
          Next();
          auto node = ParseReferencedType();
          CHECK_AST(node);
          Handle<ir::Node> type_constraints = New<ir::TypeConstraintsView>(name, node);
          type_constraints->SetInformationForNode(name);
          type_params->InsertLast(type_constraints);
        } else {
          type_params->InsertLast(name);
        }
      } else if (Current()->type() == Token::TS_GREATER) {
        if (!found) {
          SYNTAX_ERROR("SyntaxError need type parameter", Current());
        }
        Next();
        return type_params;
      } else if (Current()->type() == Token::TS_COMMA) {
        Next();
      } else {
        SYNTAX_ERROR("SyntaxError unexpected token", Current());
      }
    }
  }
  SYNTAX_ERROR("SyntaxError '<' expected", Current());
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
Handle<ir::Node> Parser<UCharInputIterator>::ParseTypeExpression() {
  LOG_PHASE(ParseTypeExpression);
  if (Current()->type() == Token::TS_VOID) {
    Current()->set_type(Token::TS_IDENTIFIER);
  }
  if (Current()->type() == Token::TS_NEW) {
    Next();
    Handle<ir::Node> call_sig = ParseCallSignature(false, true);
    CHECK_AST(call_sig);
    Handle<ir::Node> ret = New<ir::ConstructSignatureView>(call_sig);
    ret->SetInformationForNode(call_sig);
    return ret;
  } else if (Current()->type() == Token::TS_TYPEOF) {
    auto node = ParseTypeQueryExpression();
    CHECK_AST(node);
    return ParseArrayType(node);
  } else if (Current()->type() == Token::TS_IDENTIFIER) {
    Current()->set_type(Token::TS_IDENTIFIER);
    auto node = ParseReferencedType();
    CHECK_AST(node);
    return ParseArrayType(node);
  } else if (Current()->type() == Token::TS_LEFT_PAREN ||
             Current()->type() == Token::TS_LESS) {
    Handle<ir::Node> type_params;
    if (Current()->type() == Token::TS_LESS) {
      type_params = ParseTypeParameters();
      CHECK_AST(type_params);
    }
    Handle<ir::Node> types = ParseParameterList(false);
    CHECK_AST(types);
    if (Current()->type() == Token::TS_ARROW_GLYPH) {
      Next();
      Handle<ir::Node> ret_type = ParseTypeExpression();
      CHECK_AST(ret_type);
      Handle<ir::Node> ft = New<ir::FunctionTypeExprView>(types, ret_type, type_params);
      ft->SetInformationForNode(types);
      return ParseArrayType(ft);
    }
    SYNTAX_ERROR("SyntaxError '=>' expected", Current());
  } else if (Current()->type() == Token::TS_LEFT_BRACE) {
    auto node = ParseObjectTypeExpression();
    CHECK_AST(node);
    return ParseArrayType(node);
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
Handle<ir::Node> Parser<UCharInputIterator>::ParseReferencedType() {
  LOG_PHASE(ParseReferencedType);
  if (Current()->type() == Token::TS_IDENTIFIER) {
    Handle<ir::Node> node = ParsePrimaryExpression(false);
    CHECK_AST(node);
    if (Current()->type() == Token::TS_DOT) {
      node = ParseGetPropOrElem(node, false, true, false);
      CHECK_AST(node);
    }
    if (!Current()->has_line_break_before_next() && Current()->type() == Token::TS_LESS) {
      Handle<ir::Node> type_parameter = ParseTypeArguments();
      CHECK_AST(type_parameter);
      Handle<ir::Node> ret = New<ir::GenericTypeExprView>(node, type_parameter);
      ret->SetInformationForNode(node);
      return ret;
    }

    Handle<ir::Node> ret = New<ir::SimpleTypeExprView>(node);
    ret->SetInformationForNode(node);
    return ret;
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
Handle<ir::Node> Parser<UCharInputIterator>::ParseTypeQueryExpression() {
  if (Current()->type() == Token::TS_TYPEOF) {
    Next();
    if (Current()->type() == Token::TS_IDENTIFIER) {
      Handle<ir::Node> name = ParsePrimaryExpression(false);
      CHECK_AST(name);
      if (Current()->type() == Token::TS_DOT) {
        name = ParseGetPropOrElem(name, false, true, false);
        CHECK_AST(name);
      }
      Handle<ir::Node> ret = New<ir::TypeQueryView>(name);
      ret->SetInformationForNode(name);
      return ret;
    }
    SYNTAX_ERROR("SyntaxError identifier expected", Current());
  }
  SYNTAX_ERROR("SyntaxError 'typeof' expected", Current());
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
Handle<ir::Node> Parser<UCharInputIterator>::ParseTypeArguments() {
  LOG_PHASE(ParseTypeArguments);
  if (Current()->type() == Token::TS_LESS) {
    Handle<ir::TypeArgumentsView> type_arguments = New<ir::TypeArgumentsView>();
    type_arguments->SetInformationForNode(Current());
    EnableNestedGenericTypeScanMode();
    YATSC_SCOPED([&] {DisableNestedGenericTypeScanMode();});
    Next();
    while (1) {
      auto node = ParseTypeExpression();
      SKIP_TOKEN_OR(node, Token::TS_GREATER) {
        type_arguments->InsertLast(node);
      }
      if (Current()->type() == Token::TS_COMMA) {
        Next();
      } else if (Current()->type() == Token::TS_GREATER) {
        Next();
        return type_arguments;
      } else {
        SYNTAX_ERROR("SyntaxError '>' or ',' expected", Current());
      }
    }
  }
  SYNTAX_ERROR("SyntaxError '<' expected", Current());
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
Handle<ir::Node> Parser<UCharInputIterator>::ParseArrayType(Handle<ir::Node> type_expr) {
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
  return type_expr;
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
Handle<ir::Node> Parser<UCharInputIterator>::ParseObjectTypeExpression() {
  LOG_PHASE(ParseObjectTypeExpression);
  if (Current()->type() == Token::TS_LEFT_BRACE) {
    Next();
    Handle<ir::ObjectTypeExprView> object_type = New<ir::ObjectTypeExprView>();
    object_type->SetInformationForNode(Current());
    while (Current()->type() != Token::TS_RIGHT_BRACE) {
      Handle<ir::Node> property = ParseObjectTypeElement();
      SKIP_TOKEN_OR(property, Token::TS_RIGHT_BRACE) {
        object_type->InsertLast(property);
      }
      if (IsLineTermination()) {
        ConsumeLineTerminator();
      } else if (Current()->type() != Token::TS_RIGHT_BRACE &&
                 Prev()->type() != Token::TS_RIGHT_BRACE) {
        SYNTAX_ERROR("SyntaxError ';' expected", Prev());
      }
    }
    Next();
    return object_type;
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
Handle<ir::Node> Parser<UCharInputIterator>::ParseObjectTypeElement() {
  LOG_PHASE(ParseObjectTypeElement);
  if (Current()->type() == Token::TS_NEW) {
    Next();
    Handle<ir::Node> call_sig = ParseCallSignature(false, true);
    CHECK_AST(call_sig);
    Handle<ir::Node> ret = New<ir::ConstructSignatureView>(call_sig);
    ret->SetInformationForNode(call_sig);
    return ret;
  } else if (Current()->type() == Token::TS_LEFT_PAREN ||
             Current()->type() == Token::TS_LESS) {
    return ParseCallSignature(false, true);
  } else if (Current()->type() == Token::TS_LEFT_BRACKET) {
    return ParseIndexSignature();
  } else {
    bool optional = false;
    bool generator = false;
    AccessorType at = ParseAccessor();
    Handle<ir::Node> key;

    if (Current()->type() == Token::TS_MUL) {
      generator = true;
      Next();
    }

    TokenInfo info = *Current();

      if (TokenInfo::IsKeyword(Current()->type())) {
        Current()->set_type(Token::TS_IDENTIFIER);
      }
      if (Current()->type() == Token::TS_IDENTIFIER) {
        key = ParseIdentifierReference(false);
      } else {
        key = ParsePropertyName(false, false);
      }

      if (!key) {      
        if (at.getter || at.setter) {
          key = New<ir::NameView>(NewSymbol(ir::SymbolType::kPropertyName, info.value()));
          key->SetInformationForNode(&info);
        } else {
          return ir::Node::Null();
        }
      }
    
    if (Current()->type() == Token::TS_QUESTION_MARK) {
      optional = true;
      Next();
    }
    if (Current()->type() == Token::TS_LEFT_PAREN || Current()->type() == Token::TS_LESS) {
      if (!key->HasNameView()) {
        SYNTAX_ERROR("SyntaxError invalid method name", key);
      }
      Handle<ir::Node> call_sig = ParseCallSignature(false, false);
      CHECK_AST(call_sig);
      Handle<ir::Node> ret = New<ir::MethodSignatureView>(optional, at.getter, at.setter, generator, key, call_sig);
      ret->SetInformationForNode(key);
      return ret;
    } else if (Current()->type() == Token::TS_COLON) {
      Next();
      Handle<ir::Node> type_expr = ParseTypeExpression();
      CHECK_AST(type_expr);
      Handle<ir::Node> ret = New<ir::PropertySignatureView>(optional, key, type_expr);
      ret->SetInformationForNode(type_expr);
      return ret;
    }
    Handle<ir::Node> ret = New<ir::PropertySignatureView>(optional, key, ir::Node::Null());
    ret->SetInformationForNode(key);
    return ret;
  }
  SYNTAX_ERROR("SyntaxError unexpected token", Current());
}


template <typename UCharInputIterator>
Handle<ir::Node> Parser<UCharInputIterator>::ParseCallSignature(bool accesslevel_allowed, bool annotation) {
  LOG_PHASE(ParseCallSignature);
  Handle<ir::Node> type_parameters;
  if (Current()->type() == Token::TS_LESS) {
    type_parameters = ParseTypeParameters();
    CHECK_AST(type_parameters);
  }
  if (Current()->type() == Token::TS_LEFT_PAREN) {
    TokenInfo token = (*Current());
    Handle<ir::Node> parameter_list = ParseParameterList(accesslevel_allowed);
    CHECK_AST(parameter_list);
    Handle<ir::Node> return_type;
    if (Current()->type() == Token::TS_COLON) {
      Next();
      return_type = ParseTypeExpression();
      CHECK_AST(return_type);
    } else if (annotation) {
      if (Current()->type() == Token::TS_ARROW_GLYPH) {
        Next();
        return_type = ParseTypeExpression();
        CHECK_AST(return_type);
      } else {
        SYNTAX_ERROR("SyntaxError '=>' expected.", Current());
      }
    }
    Handle<ir::Node> ret = New<ir::CallSignatureView>(parameter_list, return_type, type_parameters);
    ret->SetInformationForNode(&token);
    return ret;
  }
  SYNTAX_ERROR("SyntaxError expected '('", Current());
}


template <typename UCharInputIterator>
Handle<ir::Node> Parser<UCharInputIterator>::ParseIndexSignature() {
  if (Current()->type() == Token::TS_LEFT_BRACKET) {
    Next();
    Handle<ir::Node> identifier = ParseIdentifier();
    CHECK_AST(identifier);
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
              Handle<ir::Node> type = ParseTypeExpression();
              CHECK_AST(type);
              return New<ir::IndexSignatureView>(identifier, type, string_type);
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
