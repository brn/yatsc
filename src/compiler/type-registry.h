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

namespace yatsc {

// Forward declarations.

class LiteralBuffer;

namespace ir {
class Symbol;
class ModuleInfo;
class Node;
class StringType;
class NumberType;
class BooleanType;
class VoidType;
class AnyType;
class Type;
}


class GlobalTypeRegistry {
 public:
  GlobalTypeRegistry() {Initialize();}


  Handle<ir::Type> FindType(Handle<ir::Symbol> symbol) const;
  
  
 private:
  void Initialize();

  Handle<Type> DeclareBuiltin(const char* name, Handle<Type> type);
  
  LazyInitializer<UnsafeHashMap<UtfString, Handle<Type>>> type_map_;
  LazyInitializer<UnsafeZoneAllocator> unsafe_zone_allocator_;

  static Handle<ir::StringType> kStringType;
  static Handle<ir::NumberType> kNumberType;
  static Handle<ir::BooleanType> kBooleanType;
  static Handle<ir::VoidType> kVoidType;
  static Handle<ir::AnyType> kAnyType;
};


// Type registry for all declared types and built-in types.
class TypeRegistry {
 public:
  TypeRegistry(const GlobalTypeRegistry& global_type_registry, Handle<ModuleInfo> module_info);


  ~TypeRegistry();


  void Register(Handle<ir::Symbol> symbol, Handle<ir::Type> type);


  void RegisterExernalPhaiType(Handle<ir::Symbol> symbol);


  Handle<ir::Type> FindType(Handle<ir::Symbol> symbol) const;

  
  Handle<ir::Type> FindPropertyType(Handle<ir::Node> prop) const;
  

 private:  
  HashMap<Unique::Id, Handle<Type>> type_map_;
  const GlobalTypeRegistry& global_type_registry_;
  Handle<ModuleInfo> module_info_;
};

}

#endif
