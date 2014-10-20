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
    Next();
    bool found = false;
    while (1) {
      ParseNestedGenericTypeEnd();
      if (Current()->type() == Token::TS_IDENTIFIER ||
          TokenInfo::IsKeyword(Current()->type())) {
        found = true;
        Handle<ir::Node> name = ParseIdentifier();
        if (Current()->type() == Token::TS_EXTENDS) {
          Next();
          Handle<ir::Node> type_constraints = New<ir::TypeConstraintsView>(name, ParsePrimaryExpression(false));
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
    Handle<ir::Node> ret = New<ir::ConstructSignatureView>(call_sig);
    ret->SetInformationForNode(call_sig);
    return ret;
  } else if (Current()->type() == Token::TS_TYPEOF) {
    return ParseArrayType(ParseTypeQueryExpression());
  } else if (Current()->type() == Token::TS_IDENTIFIER ||
      TokenInfo::IsKeyword(Current()->type())) {
    Current()->set_type(Token::TS_IDENTIFIER);
    return ParseArrayType(ParseReferencedType());
  } else if (Current()->type() == Token::TS_LEFT_PAREN) {
    Handle<ir::Node> types = ParseParameterList(false);
    if (Current()->type() == Token::TS_ARROW_GLYPH) {
      Next();
      Handle<ir::Node> ret_type = ParseTypeExpression();
      Handle<ir::Node> ft = New<ir::FunctionTypeExprView>(types, ret_type);
      ft->SetInformationForNode(types);
      return ParseArrayType(ft);
    }
    SYNTAX_ERROR("SyntaxError '=>' expected", Current());
  } else if (Current()->type() == Token::TS_LEFT_BRACE) {
    return ParseArrayType(ParseObjectTypeExpression());
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
    if (Current()->type() == Token::TS_DOT) {
      Handle<ir::GetPropView> get_prop_view = New<ir::GetPropView>();
      get_prop_view->SetInformationForNode(node);
      while (1) {
        if (Current()->type() == Token::TS_DOT) {
          Next();
          get_prop_view->InsertLast(ParsePrimaryExpression(false));
        } else {
          break;
        }
      }
      node = get_prop_view;
    }
    if (!Current()->has_line_break_before_next() && Current()->type() == Token::TS_LESS) {
      Handle<ir::Node> type_parameter = ParseTypeArguments();
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
      if (Current()->type() == Token::TS_DOT) {
        Next();
        Handle<ir::GetPropView> getprop = New<ir::GetPropView>();
        getprop->SetInformationForNode(name);
        Handle<ir::GetPropView> root = getprop;
        getprop->set_target(name);
        while (1) {
          if (Current()->type() == Token::TS_IDENTIFIER) {
            getprop->set_prop(ParsePrimaryExpression(false));
            if (Current()->type() == Token::TS_DOT) {
              Handle<ir::GetPropView> getprop_tmp = New<ir::GetPropView>();
              getprop_tmp->SetInformationForNode(name);
              getprop->set_target(getprop_tmp);
              getprop_tmp = getprop;
            } else {
              break;
            }
          } else {
            break;
          }
        }
        Handle<ir::Node> ret = New<ir::TypeQueryView>(root);
        ret->SetInformationForNode(root);
        return ret;
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
    Next();
    while (1) {
      type_arguments->InsertLast(ParseTypeExpression());
      ParseNestedGenericTypeEnd();
      if (Current()->type() == Token::TS_SHIFT_RIGHT) {
        BackOneChar();
        ScanGenericType();
        Next();
        NotScanNestedGenericType();
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
      object_type->InsertLast(property);
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
    Handle<ir::Node> ret = New<ir::ConstructSignatureView>(call_sig);
    ret->SetInformationForNode(call_sig);
    return ret;
  } else if (Current()->type() == Token::TS_LEFT_PAREN) {
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
    try {
      if (Current()->type() == Token::TS_IDENTIFIER) {
        key = ParseIdentifierReference(false);
      } else {
        key = ParsePropertyName(false, false);
      }
    } catch (const SyntaxError& e) {
      if (at.getter || at.setter) {
        key = New<ir::NameView>(info.value());
        key->SetInformationForNode(&info);
      } else {
        throw e;
      }
    }
    
    if (Current()->type() == Token::TS_QUESTION_MARK) {
      optional = true;
      Next();
    }
    if (Current()->type() == Token::TS_LEFT_PAREN || Current()->type() == Token::TS_LESS) {
      if (!key->HasNameView()) {
        SYNTAX_ERROR_POS("SyntaxError invalid method name", key->source_position());
      }
      Handle<ir::Node> call_sig = ParseCallSignature(false, true);
      Handle<ir::Node> ret = New<ir::MethodSignatureView>(optional, at.getter, at.setter, generator, key, call_sig);
      ret->SetInformationForNode(key);
      return ret;
    } else if (Current()->type() == Token::TS_COLON) {
      Next();
      Handle<ir::Node> type_expr = ParseTypeExpression();
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
  }
  if (Current()->type() == Token::TS_LEFT_PAREN) {
    TokenInfo token = (*Current());
    Handle<ir::Node> parameter_list = ParseParameterList(accesslevel_allowed);
    Handle<ir::Node> return_type;
    if (Current()->type() == Token::TS_COLON) {
      Next();
      return_type = ParseTypeExpression();
    } else if (annotation) {
      if (Current()->type() == Token::TS_ARROW_GLYPH) {
        Next();
        return_type = ParseTypeExpression();
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
    if (Current()->type() == Token::TS_COLON) {
      Next();
      if (Current()->type() == Token::TS_IDENTIFIER) {
        bool string_type = Current()->value() == "string";
        bool number_type = Current()->value() == "number";
        if (string_type || number_type) {
          Next();
          if (Current()->type() == Token::TS_RIGHT_BRACKET) {
            Next();
            if (Current()->type() == Token::TS_COLON) {
              Next();
              Handle<ir::Node> type = ParseTypeExpression();
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


template <typename UCharInputIterator>
void Parser<UCharInputIterator>::ParseNestedGenericTypeEnd() {
  if (Current()->type() == Token::TS_SHIFT_RIGHT) {
    BackOneChar();
    ScanGenericType();
    Next();
    NotScanNestedGenericType();
  }
}
}
