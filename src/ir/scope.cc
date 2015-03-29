// The MIT License (MIT)
// 
// Copyright (c) Taketoshi Aono(brn)
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


#include "./scope.h"
#include "./node.h"
#include "./types.h"
#include "../parser/literalbuffer.h"
#include "../parser/utfstring.h"

namespace yatsc {namespace ir {

Scope::Scope(Handle<Scope> parent_scope, Handle<GlobalScope> global_scope)
    : parent_scope_(parent_scope),
      global_scope_(global_scope) {}


Scope::Scope() {}


Scope::~Scope() {}


void Scope::Declare(Node* var, Handle<Type> type) {
  if (var->HasVariableView()) {
    if (var->first_child()->HasNameView()) {
      auto info = GatheredTypeInfo(type, var, ir::Type::Modifier::kPublic);
      declared_items_.insert(std::make_pair(var->first_child()->symbol()->utf8_value(), info));
    } else if (var->first_child()->HasBindingPropListView()) {
      Declare(var->first_child());
    }
  } else if (var->HasBindingPropListView()) {
    for (auto node: *var) {
      if (!node->node_list()[0]) {
        auto info = GatheredTypeInfo(type, node->node_list()[0], ir::Type::Modifier::kPublic);
        declared_items_.insert(std::make_pair(node->node_list()[0]->symbol()->utf8_value(), info));
      } else {
        Declare(node->node_list()[0]);
      }
    }
  } else if (var->HasFunctionView()) {
    Node* name = var->node_list()[1];
    if (name && name->HasNameView()) {
      auto info = GatheredTypeInfo(type, var, ir::Type::Modifier::kPublic);
      declared_items_.insert(std::make_pair(name->symbol()->utf8_value(), info));
    }
  } else if (var->HasClassDeclView() || var->HasInterfaceView() || var->HasEnumDeclView()) {
    Node* name = var->first_child();
    if (name && name->HasNameView()) {
      auto info = GatheredTypeInfo(type, var, ir::Type::Modifier::kPublic);
      declared_items_.insert(std::make_pair(name->symbol()->utf8_value(), info));
    }
  }
}


void Scope::Declare(Node* var) {
  Declare(var, global_scope_->phai_type());
}


void GlobalScope::Initialize() {
  // string_type_ = DeclareBuiltin("string",  Heap::NewHandle<ir::StringType>());
  // number_type_ = DeclareBuiltin("number",  Heap::NewHandle<ir::NumberType>());
  // boolean_type_ = DeclareBuiltin("boolean", Heap::NewHandle<ir::BooleanType>());
  // void_type_ = DeclareBuiltin("void",    Heap::NewHandle<ir::VoidType>());
  // any_type_ = DeclareBuiltin("any",     Heap::NewHandle<ir::AnyType>());
  //phai_type_ = Heap::NewHandle<ir::PhaiType>();
}


// Handle<ir::Type> GlobalScope::DeclareBuiltin(const char* name, Handle<ir::Type> type) {
//   UtfString utf_string(name);
//   auto literal = literal_buffer_->InsertValue(utf_string);
//   auto iv = ir::InterfaceView>(
//       Heap::NewIntrusive<ir::NameView>(Heap::NewHandle<ir::Symbol>(ir::SymbolType::kInterfaceName, literal)),
//       ir::Node::Null(),
//       Heap::NewIntrusive<ir::InterfaceExtendsView>(),
//       Heap::NewIntrusive<ir::ObjectTypeExprView>());
//   Declare(iv, type);
//   return type;
// }


}}
