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
#include "./symbol.h"
#include "./types.h"


namespace yatsc {
class LiteralBuffer;

namespace ir {

class Node;
class Scope;
class GlobalScope;



typedef Vector<Handle<Scope>> Scopes;
typedef MultiHashMap<Unique::Id, GatheredTypeInfo> DeclaredMap;
typedef IteratorRange<DeclaredMap::const_iterator, DeclaredMap::const_iterator> DeclaredRange;
typedef IteratorRange<Scopes::iterator, Scopes::iterator> ScopeRange;


class Scope: private Uncopyable {
 public:
  Scope(Handle<Scope> parent_scope, Handle<GlobalScope> global_scope);


  Scope();


  ~Scope();


  void Declare(Handle<Node> variable);


  void Declare(Handle<Node> variable, Handle<ir::Type> type);


  YATSC_INLINE Maybe<DeclaredRange> FindDeclaredItem(Handle<Symbol> name) {
    DeclaredRange range = declared_items_.equal_range(name->id());
    if (range.first == declared_items_.end()) {
      if (parent_scope_) {
        return parent_scope_->FindDeclaredItem(name);
      } else if (global_scope_) {
        return global_scope_->FindDeclaredItem(name);
      }
      return Nothing<DeclaredRange>();
    }
    return Just(range);
  };


  void AddChild(Handle<Scope> child) {
    child_scope_list_.push_back(child);
  };


  YATSC_INLINE ScopeRange Children() {return MakeRange(child_scope_list_.begin(), child_scope_list_.end());}


  YATSC_PROPERTY(Handle<Scope>, parent_scope, parent_scope_);

 private:
  DeclaredMap declared_items_;
  Handle<Scope> parent_scope_;
  Handle<GlobalScope> global_scope_;
  Scopes child_scope_list_;
};


class GlobalScope: public Scope {
 public:
  GlobalScope(Handle<LiteralBuffer> literal_buffer)
      : Scope(),
        literal_buffer_(literal_buffer) {Initialize();}


  YATSC_CONST_GETTER(Handle<StringType>, string_type, string_type_);
  YATSC_CONST_GETTER(Handle<NumberType>, number_type, number_type_);
  YATSC_CONST_GETTER(Handle<BooleanType>, boolean_type, boolean_type_);
  YATSC_CONST_GETTER(Handle<VoidType>, void_type, void_type_);
  YATSC_CONST_GETTER(Handle<AnyType>, any_type, any_type_);
  YATSC_CONST_GETTER(Handle<PhaiType>, phai_type, phai_type_);

  
 private:
  void Initialize();

  Handle<ir::Type> DeclareBuiltin(const char* name, Handle<ir::Type> type);

  Handle<LiteralBuffer> literal_buffer_;


  Handle<StringType> string_type_;
  Handle<NumberType> number_type_;
  Handle<BooleanType> boolean_type_;
  Handle<VoidType> void_type_;
  Handle<AnyType> any_type_;
  Handle<PhaiType> phai_type_;
};

}}

#endif
