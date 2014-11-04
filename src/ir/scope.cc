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

namespace yatsc {namespace ir {

Scope::Scope(Handle<Scope> parent_scope)
    : parent_scope_(parent_scope) {}


Scope::Scope() {}


Scope::~Scope() {}


void Scope::Declare(Handle<Node> var) {
  if (var->HasVariableView()) {
    if (var->first_child()->HasNameView()) {
      declared_items_.insert(std::make_pair(var->first_child()->symbol()->id(), var));
    } else if (var->first_child()->HasBindingPropListView()) {
      Declare(var->first_child());
    }
  } else if (var->HasBindingPropListView()) {
    for (auto node: *var) {
      if (!node->node_list()[1]) {
        declared_items_.insert(std::make_pair(node->node_list()[1]->symbol()->id(), node->node_list()[1]));
      } else {
        Declare(node->node_list()[1]);
      }
    }
  } else if (var->HasFunctionView()) {
    Handle<ir::Node> name = var->node_list()[1];
    if (name && name->HasNameView()) {
      declared_items_.insert(std::make_pair(name->symbol()->id(), var));
    }
  }
}


}}
