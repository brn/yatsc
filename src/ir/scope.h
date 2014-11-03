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


#ifndef YATSC_IR_SCOPE_H
#define YATSC_IR_SCOPE_H

#include "../utils/utils.h"
#include "../utils/stl.h"
#include "../utils/unicode.h"

namespace yatsc {namespace ir {

class Node;
class Scope;


typedef Vector<Handle<Scope>> Scopes;
typedef MultiHashMap<Utf16String, Handle<Node>> DeclaredMap;
typedef IteratorRange<DeclaredMap::const_iterator, DeclaredMap::const_iterator> DeclaredRange;
typedef IteratorRange<Scopes::iterator, Scopes::iterator> ScopeRange;

class Scope: private Uncopyable {
 public:
  Scope(Handle<Scope> parent_scope);


  Scope();


  ~Scope();


  void Declare(Handle<Node> variable);


  YATSC_INLINE DeclaredRange FindDeclaredItem(const Utf16String& name) {
    return declared_items_.equal_range(name);
  };


  void AddChild(Handle<Scope> child) {
    child_scope_list_.push_back(child);
  };


  YATSC_INLINE ScopeRange Children() {return MakeRange(child_scope_list_.begin(), child_scope_list_.end());}


  YATSC_PROPERTY(Handle<Scope>, parent_scope, parent_scope_);


 private:
  DeclaredMap declared_items_;
  Handle<Scope> parent_scope_;
  Scopes child_scope_list_;
};

}}

#endif
