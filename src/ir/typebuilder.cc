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


#include "./typebuilder.h"

namespace yatsc {namespace ir {


class InternalTypeBuilder: private Static {
 public:
  static Handle<Type> Create(Handle<SimpleTypeExprView> type_expr, TypeRegistry* type_registry) {
    Handle<Node> node = type_expr->type_name();
    if (node->HasNameView()) {
      if (node->symbol()->Equals("string")) {
        return TypeRegistry::kStringType;
      } else if (node->symbol()->Equals("number")) {
        return TypeRegistry::kNumberType;
      } else if (node->symbol()->Equals("boolean")) {
        return TypeRegistry::kBooleanType;
      } else if (node->symbol()->Equals("void")) {
        return TypeRegistry::kVoidType;
      } else if (node->symbol()->Equals("any")) {
        return TypeRegistry::kAnyType;
      } else {
        Handle<Type> result = type_registry->FindType(node->symbol());
        if (result) {
          type_registry->Register(node->symbol(), result);
        } else {
          type_registry->RegisterExernalPhaiType(node->symbol(), node);
        }
        return result;
      }
    }
  }
  

  static Handle<Type> Create(Handle<ClassDeclView> type_expr, TypeRegistry* type_registry, Handle<Node> node) {
    ir::Node::List node_list = node->node_list();
    auto result = Heap::NewHandle<ClassType>();
    if (node_list[1]) {
      for (auto t: node_list[1]) {
        if (t->HasTypeConstraintsView()) {
          Handle<ir::Node> derived = t->first_child();
          Handle<ir::Node> base = t->last_child();
          result->AddTypeParameter(
              Heap::NewHandle<TypeConstraintsType>(Heap::NewHandle<PlaceHolderType>(derived->symbol()), TypeBuilder::Create(base)), node);
        } else if (t->HasNameView()) {
          result->AddTypeParameter(Heap::NewHandle<PlaceHolderType>(t->symbol()), node);
        }
      }
    }

    if (node_list[2]) {
      auto bases = node_list[2]->node_list();
      if (bases[0]) {
        if (bases[0]->HasGetPropView() ||
            bases[0]->HasGetElemView()) {
          auto prop = type_registry->FindPropertyType(bases[0]);
          if (!prop) {
            prop = TypeRegistry::kPhaiType;
          }
          result->set_extends(prop);
        } else {
          auto t = type_registry->FindType(bases[0]->symbol());
          if (!t) {
            t = TypeRegistry::kPhaiType;
          }
          result->set_extends(t);
        }
      }
    }
    
    if (node_list[3]) {
      for (auto t: node_list[3]->node_list()) {

        modifiers = t->node_list[0];
        Type::Modifier mod = Type::Modifiers::None;
        for (auto modifier: modifiers->node_list()) {
          mod |= Type::ModifierFromToken(modifier->operand());
        }
        
        result->DeclareType(t->node_list[1]->symbol(), Create(Handle<MemberFunctionView>(t)), t, mod);
      }
    }
    type_registry->Register(node->node_list[0]->symbol(), result);
    return result;
  }


  static Handle<Type> Create(Handle<InterfaceView> type_expr, TypeRegistry* type_registry, Handle<Node> node) {
    
  }
}


Handle<Type> TypeBuilder::Create(Handle<Node> node) {
  switch (node->type()) {
    case NodeType::kSimpleTypeExprView:
      return InternalTypeBuilder::Create(Handle<SimpleTypeExprView>(node), type_registry_);
    case NodeType::kClassDeclView:
      return InternalTypeBuilder::Create(Handle<ClassDeclView>(node), type_registry_, node);
    case NodeType::kInterfaceType:
      return InternalTypeBuilder::Create(Handle<InterfaceType>(node), type_registry_, node);
  }
}

}}
