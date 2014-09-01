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
ir::Node* Parser<UCharInputIterator>::ParseTypeParameters() {
  if (Current()->type() == Token::TS_LESS) {
    ir::TypeParametersView* type_params = New<ir::TypeParametersView>();
    type_params->SetInformationForNode(Current());
    Next();
    bool found = false;
    while (1) {
      if (Current()->type() == Token::TS_IDENTIFIER) {
        found = true;
        ir::Node* name = ParsePrimaryExpression(false);
        if (Current()->type() == Token::TS_EXTENDS) {
          Next();
          ir::Node* type_constraints = New<ir::TypeConstraintsView>(name, ParsePrimaryExpression(false));
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
ir::Node* Parser<UCharInputIterator>::ParseTypeExpression() {
  LOG_PHASE(ParseTypeExpression);
  if (Current()->type() == Token::TS_IDENTIFIER) {
    return ParseArrayType(ParseReferencedType());
  } else if (Current()->type() == Token::TS_LEFT_PAREN) {
    ir::Node* types = ParseParameterList(false);
    if (Current()->type() == Token::TS_ARROW_GLYPH) {
      Next();
      ir::Node* ret_type = ParseTypeExpression();
      ir::Node* ft = New<ir::FunctionTypeExprView>(types, ret_type);
      ft->SetInformationForNode(types);
      return ParseArrayType(ft);
    }
    SYNTAX_ERROR("SyntaxError '=>' expected", Current());
  } else if (Current()->type() == Token::TS_LEFT_BRACE) {
    return ParseArrayType(ParseObjectTypeExpression());
  } else if (Current()->type() == Token::TS_TYPEOF) {
    return ParseArrayType(ParseTypeQueryExpression());
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
ir::Node* Parser<UCharInputIterator>::ParseReferencedType() {
  LOG_PHASE(ParseReferencedType);
  if (Current()->type() == Token::TS_IDENTIFIER) {
    ir::Node* node = ParsePrimaryExpression(false);
    if (Current()->type() == Token::TS_DOT) {
      ir::GetPropView* get_prop_view = New<ir::GetPropView>();
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
      ir::Node* type_parameter = ParseTypeArguments();
      ir::Node* ret = New<ir::GenericTypeExprView>(node, type_parameter);
      ret->SetInformationForNode(node);
      return ret;
    }

    ir::Node* ret = New<ir::SimpleTypeExprView>(node);
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
ir::Node* Parser<UCharInputIterator>::ParseTypeQueryExpression() {
  if (Current()->type() == Token::TS_TYPEOF) {
    Next();
    if (Current()->type() == Token::TS_IDENTIFIER) {
      ir::Node* name = ParsePrimaryExpression(false);
      if (Current()->type() == Token::TS_DOT) {
        Next();
        ir::GetPropView* getprop = New<ir::GetPropView>();
        getprop->SetInformationForNode(name);
        ir::GetPropView* root = getprop;
        getprop->set_target(name);
        while (1) {
          if (Current()->type() == Token::TS_IDENTIFIER) {
            getprop->set_prop(ParsePrimaryExpression(false));
            if (Current()->type() == Token::TS_DOT) {
              ir::GetPropView* getprop_tmp = New<ir::GetPropView>();
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
        ir::Node* ret = New<ir::TypeQueryView>(root);
        ret->SetInformationForNode(root);
        return ret;
      }
      ir::Node* ret = New<ir::TypeQueryView>(name);
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
ir::Node* Parser<UCharInputIterator>::ParseTypeArguments() {
  if (Current()->type() == Token::TS_LESS) {
    ir::TypeArgumentsView* type_arguments = New<ir::TypeArgumentsView>();
    type_arguments->SetInformationForNode(Current());
    Next();
    while (1) {
      type_arguments->InsertLast(ParseTypeExpression());
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
ir::Node* Parser<UCharInputIterator>::ParseArrayType(ir::Node* type_expr) {
  if (Current()->type() == Token::TS_LEFT_BRACKET) {
    Next();
    if (Current()->type() == Token::TS_RIGHT_BRACKET) {
      ir::Node* array_type = New<ir::ArrayTypeExprView>(type_expr);
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
ir::Node* Parser<UCharInputIterator>::ParseObjectTypeExpression() {
  LOG_PHASE(ParseObjectTypeExpression);
  if (Current()->type() == Token::TS_LEFT_BRACE) {
    Next();
    ir::ObjectTypeExprView* object_type = New<ir::ObjectTypeExprView>();
    object_type->SetInformationForNode(Current());
    while (Current()->type() != Token::TS_RIGHT_BRACE) {
      ir::Node* property = ParseObjectTypeElement();
      object_type->InsertLast(property);
    }
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
ir::Node* Parser<UCharInputIterator>::ParseObjectTypeElement() {
  LOG_PHASE(ParseObjectTypeElement);
  if (Current()->type() == Token::TS_NEW) {
    Next();
    ir::Node* call_sig = ParseCallSignature(false);
    ir::Node* ctor_sig = New<ir::ConstructSignatureView>(call_sig);
    ctor_sig->SetInformationForNode(call_sig);
  } else if (Current()->type() == Token::TS_LEFT_PAREN) {
    return ParseCallSignature(false);
  } else if (Current()->type() == Token::TS_LEFT_BRACKET) {
    Next();
    if (Current()->type() == Token::TS_IDENTIFIER) {
      ir::Node* name = ParseLiteral();
      if (Current()->type() == Token::TS_COLON) {
        Next();
        if (Current()->type() == Token::TS_IDENTIFIER &&
            (Current()->value() == "string" || Current()->value() == "number")) {
          ir::Node* ret = New<ir::AccessorTypeExprView>(name, ParsePrimaryExpression(false));
          ret->SetInformationForNode(Current());
          Next();
          return ret;
        } else {
          SYNTAX_ERROR("SyntaxError type name in indexSignature only allowed one of 'string' or 'number'", Current());
        }
      }
      SYNTAX_ERROR("SyntaxError ':' expected", Current());
    }
    SYNTAX_ERROR("SyntaxError identifier expected", Current());
  } else {
    bool optional = false;
    ir::Node* key = ParsePropertyDefinition(false);
    if (Current()->type() == Token::TS_QUESTION_MARK) {
      optional = true;
      Next();
    }
    if (Current()->type() == Token::TS_LEFT_PAREN) {
      if (key->HasNameView()) {
        SYNTAX_ERROR_POS("SyntaxError invalid method name", key->source_position());
      }
      ir::Node* call_sig = ParseCallSignature(false);
      ir::Node* ret = New<ir::MethodSignatureView>(optional, key, call_sig);
      ret->SetInformationForNode(key);
      return ret;
    } else if (Current()->type() == Token::TS_COLON) {
      Next();
      ir::Node* type_expr = ParseTypeExpression();
      ir::Node* ret = New<ir::PropertySignatureView>(optional, key, type_expr);
      ret->SetInformationForNode(type_expr);
      return ret;
    } else if (Current()->type() == Token::TS_LEFT_BRACKET) {
      if (optional) {
        SYNTAX_ERROR("SyntaxError unexpected '?'", Current());
      }
      return ParseTypeExpression();
    }
    ir::Node* ret = New<ir::PropertySignatureView>(optional, key, nullptr);
    ret->SetInformationForNode(key);
    return ret;
  }
  SYNTAX_ERROR("SyntaxError unexpected token", Current());
}


template <typename UCharInputIterator>
ir::Node* Parser<UCharInputIterator>::ParseCallSignature(bool accesslevel_allowed) {
  LOG_PHASE(ParseCallSignature);
  ir::Node* type_parameters = nullptr;
  if (Current()->type() == Token::TS_LESS) {
    type_parameters = ParseTypeParameters();
  }
  if (Current()->type() == Token::TS_LEFT_PAREN) {
    TokenInfo token = (*Current());
    ir::Node* parameter_list = ParseParameterList(accesslevel_allowed);
    ir::Node* return_type = nullptr;
    if (Current()->type() == Token::TS_COLON) {
      Next();
      return_type = ParseTypeExpression();
    }
    ir::Node* ret = New<ir::CallSignatureView>(parameter_list, return_type, type_parameters);
    ret->SetInformationForNode(&token);
    return ret;
  }
  SYNTAX_ERROR("SyntaxError expected '('", Current());
}
}