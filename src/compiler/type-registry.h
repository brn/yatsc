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

#ifndef YATSC_COMPILER_TYPE_REGISTRY
#define YATSC_COMPILER_TYPE_REGISTRY

#include "../memory/heap.h"
#include "../utils/stl.h"
#include "../parser/utfstring.h"
#include "../ir/node.h"
#include "../ir/types.h"

namespace yatsc {

// Forward declarations.

class LiteralBuffer;
class ErrorReporter;

namespace ir {
class Symbol;
class ModuleInfo;
class StringType;
class NumberType;
class BooleanType;
class VoidType;
class AnyType;
class PhaiType;
}


// Type registry for all declared types and built-in types.
class TypeRegistry {
 public:
  TypeRegistry() = default;


  ~TypeRegistry() = default;


  void Register(Handle<ir::Symbol> symbol,
                Handle<ir::Type> type,
                Handle<ir::Node> node,
                ir::Type::Modifier mod);


  void RegisterExernalPhaiType(Handle<ir::Symbol> symbol);


  Maybe<ir::GatheredTypeInfo> FindType(Handle<ir::Symbol> symbol) const;

  
  Maybe<ir::GatheredTypeInfo> FindPropertyType(Handle<ir::Node> prop);
  

 private:  
  HashMap<Unique::Id, Maybe<ir::GatheredTypeInfo>> type_map_;
  HashMap<Unique::Id, Handle<ir::Type>> phai_map_;
};


class GlobalTypeRegistry: public TypeRegistry {
 public:
  GlobalTypeRegistry()
      : TypeRegistry() {Initialize();}


  YATSC_CONST_GETTER(Handle<ir::StringType>, string_type, string_type_);
  YATSC_CONST_GETTER(Handle<ir::NumberType>, number_type, number_type_);
  YATSC_CONST_GETTER(Handle<ir::BooleanType>, boolean_type, boolean_type_);
  YATSC_CONST_GETTER(Handle<ir::VoidType>, void_type, void_type_);
  YATSC_CONST_GETTER(Handle<ir::AnyType>, any_type, any_type_);
  YATSC_CONST_GETTER(Handle<ir::AnyType>, phai_type, phai_type_);
  
  
 private:
  void Initialize();

  Handle<ir::Type> DeclareBuiltin(const char* name, Handle<ir::Type> type);

  Handle<ir::StringType> string_type_;
  Handle<ir::NumberType> number_type_;
  Handle<ir::BooleanType> boolean_type_;
  Handle<ir::VoidType> void_type_;
  Handle<ir::AnyType> any_type_;
  Handle<ir::PhaiType> phai_type_;
};

}

#endif
