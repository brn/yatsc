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

#include "./parser-util.h"

namespace yatsc {

// Expression[In, Yield]
//   AssignmentExpression[?In, ?Yield]
//   Expression[?In, ?Yield] , AssignmentExpression[?In, ?Yield]
//
template <typename UCharInputIterator>
ir::Node* ExpressionParser<UCharInputIterator>::ParseExpression(bool in, bool yield) {
  PARSER_LOG_PHASE(ParseExpression);
  ir::Node* assignment_expr = ParseAssignmentExpression(in, yield);

  // Parse comma expressions.
  if (parser()->Current()->type() == Token::TS_COMMA) {
    Next();
    ir::CommaExprView* comma_expr = New<ir::CommaExprView>({assignment_expr});
    comma_expr->SetInformationForNode(*Current());
    
    while (1) {
      assignment_expr = ParseAssignmentExpression(in, yield);
      comma_expr->InsertLast(assignment_expr);
      if (parser()->Current()->type() == Token::TS_COMMA) {
        Next();
      } else {
        return comma_expr;
      }
    }
  }
  
  return assignment_expr;
}


// AssignmentPattern[Yield]
//   ObjectAssignmentPattern[?Yield]
//   ArrayAssignmentPattern[?Yield]
//
template <typename UCharInputIterator>
ir::Node* ExpressionParser<UCharInputIterator>::ParseAssignmentPattern(bool yield) {
  TokenCursor cursor = parser()->GetBufferCursorPosition();
  ir::Node* node = nullptr;
  switch(parser()->Current()->type()) {
    case Token::TS_LEFT_BRACE: {
      node = ParseObjectPattern(yield);
      break;
    }
    case Token::TS_LEFT_BRACKET: {
      node = ParseArrayPattern(yield);
      break;
    }
    default:
      PARSER_SYNTAX_ERROR("SyntaxError unexpected token", parser()->PeekBuffer(cursor));
  }

  node->SetInformationForNode(parser()->PeekBuffer(cursor));
  return node;
}


// ObjectAssignmentPattern[Yield]
//   { }
//   { AssignmentPropertyList[?Yield] }
//   { AssignmentPropertyList[?Yield] , }
//
template <typename UCharInputIterator>
ir::Node* ExpressionParser<UCharInputIterator>::ParseObjectAssignmentPattern(bool yield) {
  if (parser()->Current()->type() == Token::TS_LEFT_BRACE) {
    parser()->Next();
    ir::Node* node = ParserAssignmentPropertyList();
    if (parser()->Current()->type() == Token::TS_COMMA) {
      parser()->Next();
    }
    if (parser()->Current()->type() == Token::TS_RIGHT_BRACE) {
      parser()->Next();
      return node;
    }
    PARSER_SYNTAX_ERROR("SyntaxError unexpected token", parser()->Current());
  }
}


// ArrayAssignmentPattern[Yield]
//   [ Elision(opt) AssignmentRestElement[?Yield](opt) ]
//   [ AssignmentElementList[?Yield] ]
//   [ AssignmentElementList[?Yield] , Elision(opt) AssignmentRestElement[?Yield](opt) ]
//
// AssignmentElementList[Yield]
//   AssignmentElisionElement[?Yield]
//   AssignmentElementList[?Yield] , AssignmentElisionElement[?Yield]
//
template <typename UCharInputIterator>
ir::Node* ExpressionParser<UCharInputIterator>::ParseArrayAssignmentPattern(bool yield) {
  if (parser()->Current()->type() == Token::TS_LEFT_BRACKET) {
    bool has_rest = false;
    auto array_view = parser()->New<ir::BindingArrayView>();
    array_view->SetInformationForNode(parser()->Current());
    parser()->Next();


    // '[, ...' pattern
    if (parser()->Current()->type() == Token::TS_COMMA) {
      parser()->Next();
      array_view->InsertLast(parser()->New<ir::UndefinedView>());
      array_view->InsertLast(parser()->New<ir::UndefinedView>());
    }
    
    if (parser()->Current()->type() == Token::TS_REST) {
      array_view->InsertLast(ParseAssignmentRestElement(yield));
      has_rest = true;
    }

    if (parser()->Current()->type() == Token::TS_RIGHT_BRACKET) {
      if (array_view->size() == 0) {
        PARSER_SYNTAX_ERROR("SyntaxError destructuring assignment left hand side is not allowed empty array", parser()->Current());
      }
      parser()->Next();
      return array_view;
    }

    // The ParameterRest is not allowed in any position of array pattern except the last element.
    if (has_rest) {
      PARSER_SYNTAX_ERROR("SyntaxError destructuring assignment rest must be the end of element", parser()->Current());
    }

    while (1) {
      if (parser()->Current()->type() == Token::TS_COMMA) {
        array_view->InsertLast(parser()->New<ir::UndefinedView>());
        array_view->InsertLast(parser()->New<ir::UndefinedView>());
        parser()->Next();
      } else {
        array_view->InsertLast(ParseAssignmentElement());
      }
      switch (parser()->Current()->type()) {
        case Token::TS_COMMA:
          parser()->Next();
          break;
        case Token::TS_RIGHT_BRACKET:
          parser()->Next();
          return array_view;
        default:
          PARSER_SYNTAX_ERROR("SyntaxError unexpected token", parser()->Current());
      }
    }
  }
  SYNTAX_ERROR("SyntaxError unexpected token", parser()->Current());
}


// Parse destructuring assignment object pattern properties.
// 
// AssignmentPropertyList[Yield]
//   AssignmentProperty[?Yield]
//   AssignmentPropertyList[?Yield] , AssignmentProperty[?Yield]
//
template <typename UCharInputIterator>
ir::Node* ExpressionParser<UCharInputIterator>::ParseAssignmentPropertyList(bool yield) {
  auto prop_list = parser()->New<ir::BindingPropListView>();
  while (1) {
    prop_list->InsertLast(ParseAssignmentProperty(yield));
    if (parser()->Current()->type() == Token::TS_COMMA) {
      parser()->Next();
      continue;
    }
    if (parser()->Current()->type() == Token::TS_RIGHT_BRACE) {
      return prop_list;
    }
  }

  // UNREACHABLE
  return nullptr;
}


// Parse destructuring assignment object pattern property.
//
// AssignmentProperty[Yield] :
//   IdentifierReference[?Yield] Initializer[In,?Yield](opt)
//   PropertyName : AssignmentElement[?Yield]
//
template <typename UCharInputIterator>
ir::Node* ExpressionParser<UCharInputIterator>::ParseAssignmentProperty(bool yield) {
  TokenCursor cursor = parser()->GetBufferCursorPosition();
  
  ir::Node* property_name = nullptr;
  ir::Node* elem = nullptr;

  // Check whether property name is identifier reference or not.
  bool identifier = false;
  
  if ((yield && parser()->Current()->type() == Token::TS_YIELD) ||
      parser()->Current()->type() == Token::TS_IDENTIFIER) {
    identifier = true;
    property_name = ParseIdentifierReference();
  } else if (parser()->Current()->type() == Token::TS_STRING_LITERAL ||
             parser()->Current()->type() == Token::TS_NUMERIC_LITERAL ||
             parser()->Current()->type() == Token::TS_LEFT_BRACKET) {
    property_name = ParsePropertyName(yield, false);
  }
  
  ir::Node* init = nullptr;

  // Initializer is only allowed, if property name is identifier reference.
  if (identifier && parser()->Current()->type() == Token::TS_ASSIGN) {
    init = ParseInitializer(true, yield);
  } else if (parser()->Current()->type() == Token::TS_COLON) {
    parser()->Next();
    elem = ParseAssignmentElement(yield);
  }

  // All destructuring assignment element is convert to BindingElementView.
  auto binding_element = parser()->New<ir::BindingElementView>(property_name, elem, init);
  binding_element->SetInformationForNode(parser()->PeekBuffer(cursor));
  return binding_element;
}


// AssignmentElement[Yield]
//   DestructuringAssignmentTarget[?Yield] Initializer[In,?Yield](opt)
//
template <typename UCharInputIterator>
ir::Node* ExpressionParser<UCharInputIterator>::ParseAssignmentElement(bool yield) {
  ir::Node* target = ParseDestructuringAssignmentTarget(yield);
  ir::Node* init = nullptr;
  if (parser()->Current()->type() == Token::TS_ASSIGN) {
    init = ParseInitializer(true, yield);
  }

  // All destructuring assignment element is convert to BindingElementView.
  auto binding_element = parser()->New<ir::BindingElementView>(nullptr, target, init);
  binding_element->SetInformationForNode(parser()->PeekBuffer(cursor));
  return binding_element;
}


// AssignmentRestElement[Yield]
//   ... DestructuringAssignmentTarget[?Yield]
//
template <typename UCharInputIterator>
ir::Node* ExpressionParser<UCharInputIterator>::ParseAssignmentRestElement(bool yield) {
  if (parser()->Current()->type() == Token::TS_REST) {
    TokenCursor cursor = parser()->GetBufferCursorPosition();
    parser()->Next();
    ir::Node* target = ParseDestructuringAssignmentTarget(yield);
    auto rest = parser()->New<ir::RestParamView>(target);
    rest->SetInformationForNode(parser()->PeekBuffer(cursor));
  }
  PARSER_SYNTAX_ERROR("SyntaxError '...' expected", parser()->Current());
}


// DestructuringAssignmentTarget[Yield]
//   LeftHandSideExpression[?Yield]
//
template <typename UCharInputIterator>
ir::Node* ExpressionParser<UCharInputIterator>::ParseDestructuringAssignmentTarget(bool yield) {
  return ParseLeftHandSideExpression(yield);
}
}
