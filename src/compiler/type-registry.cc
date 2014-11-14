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


#include "../ir/node.h"
#include "../ir/type.h"


namespace yatsc {

Handle<ir::Type> GlobalTypeRegistry::FindType(Handle<ir::Symbol> symbol) const {
  auto it = type_map_->find(symbol->utf16_string());
  if (it == type_map_->end()) {
    return Handle<ir::Type>();
  }
  return it->second;
}


void GlobalTypeRegistry::Initialize() {
  unsafe_zone_allocator_(sizeof(ir::Type) * 5);
  type_map_(*unsafe_zone_allocator_);
  
  kStringType  = DeclareBuiltin("string",  Heap::NewHandle<ir::StringType>());
  kNumberType  = DeclareBuiltin("number",  Heap::NewHandle<ir::NumberType>());
  kBooleanType = DeclareBuiltin("boolean", Heap::NewHandle<ir::BooleanType>());
  kVoidType    = DeclareBuiltin("void",    Heap::NewHandle<ir::VoidType>());
  kAnyType     = DeclareBuiltin("any",     Heap::NewHandle<ir::AnyType>());
}


void GlobalTypeRegistry::DeclareBuiltin(const char* name, Handle<ir::Type> type) {
  UtfString str(name);
  type_map_->insert(std::make_pair(str, type));
  return type;
}


Handle<ir::StringType> GlobalTypeRegistry::kStringType;
Handle<ir::NumberType> GlobalTypeRegistry::kNumberType;
Handle<ir::BooleanType> GlobalTypeRegistry::kBooleanType;
Handle<ir::VoidType> GlobalTypeRegistry::kVoidType;
Handle<ir::AnyType> GlobalTypeRegistry::kAnyType;


TypeRegistry::TypeRegistry(const GlobalTypeRegistry& global_type_registry,
                           Handle<ModuleInfo> module_info)
    : global_type_registry_(global_type_registry),
      module_info_(module_info) {}


TypeRegistry::~TypeRegistry() {}


void TypeRegistry::Register(Handle<Symbol> symbol, Handle<ir::Type> type) {
  if (type_map_.find(symbol->id()) == type_map_.end()) {
    type_map_.insert(std::make_pair(symbol->id(), type));
  }
}


void TypeRegistry::RegisterExernalPhaiType(Handle<ir::Symbol> symbol) {
  if (external_type->HasNameView()) {
    type_map_.insert(std::make_pair(symbol->id(), kPhaiType));
  }
}


Handle<ir::Type> TypeRegistry::FindType(Handle<Symbol> name) const {
  auto it = type_map_.find(name->id());
  if (it == type_map_.end()) {
    return global_type_registry_->FindType(name);
  }
  return it->second;
}


Handle<ir::Type> TypeRegistry::FindPropertyType(Handle<ir::Node> prop) const {
  if (prop->HasGetPropView()) {
    Handle<GetPropView> get_prop(prop);
    Handle<ir::Type> prop_type;
    if (!get_prop->prop()->HasNameView()) {
      // ERROR!
      module_info->errors()->SemanticError("Type expected.", get_prop->prop()->source_position());
      return Handle<ir::Type>();
    }
    if (get_prop->target()->HasGetPropView()) {
      prop_type = FindPropertyType(get_prop->target());
    } else if (get_prop->target()->HasNameView()) {
      prop_type = FindType(get_prop->target()->symbol());
    } else {
      return kPhaiType;
    }
    
    if (prop_type->IsVoidType()) {
      // ERROR!
      module_info->errors()->SemanticError("The void type is not contains data member.", prop->source_position());
      return Handle<ir::Type>();
    } else if (prop_type->IsAnyType()) {
      return kAnyType;
    }

    return prop_type->FindDeclaredType(get_prop->prop()->symbol());
  }
}
}
