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


#include "./type-registry.h"
#include "../parser/error-reporter.h"
#include "../ir/node.h"
#include "../ir/types.h"


namespace yatsc {

Maybe<ir::GatheredTypeInfo> GlobalTypeRegistry::FindType(Handle<ir::Symbol> symbol) const {
  auto it = type_map_.find(symbol->utf16_string());
  if (it == type_map_.end()) {
    return Nothing<ir::GatheredTypeInfo>();
  }
  return it->second;
}


void GlobalTypeRegistry::Initialize() {
  // unsafe_zone_allocator_(sizeof(Maybe<ir::GatheredTypeInfo>) * 5);
  // type_map_(UnsafeZoneStdAllocator<Maybe<ir::GatheredTypeInfo>>(unsafe_zone_allocator_.Get()));
  
  string_type_  = DeclareBuiltin("string",  Heap::NewHandle<ir::StringType>());
  number_type_  = DeclareBuiltin("number",  Heap::NewHandle<ir::NumberType>());
  boolean_type_ = DeclareBuiltin("boolean", Heap::NewHandle<ir::BooleanType>());
  void_type_    = DeclareBuiltin("void",    Heap::NewHandle<ir::VoidType>());
  any_type_     = DeclareBuiltin("any",     Heap::NewHandle<ir::AnyType>());
  phai_type_ = Heap::NewHandle<ir::PhaiType>();
}


Handle<ir::Type> GlobalTypeRegistry::DeclareBuiltin(const char* name, Handle<ir::Type> type) {
  UtfString str(name);
  auto maybe = Just(ir::GatheredTypeInfo(type, ir::Node::Null(), ir::Type::Modifier::kPublic));
  type_map_.insert(std::make_pair(str.utf16_string(), maybe));
  return type;
}


TypeRegistry::TypeRegistry(const GlobalTypeRegistry& global_type_registry, Handle<ErrorReporter> error_reporter)
    : global_type_registry_(global_type_registry),
      error_reporter_(error_reporter) {}


TypeRegistry::~TypeRegistry() {}


void TypeRegistry::Register(Handle<ir::Symbol> symbol, Handle<ir::Type> type, Handle<ir::Node> node, ir::Type::Modifier mod) {
  if (type_map_.find(symbol->id()) == type_map_.end()) {
    auto just = Just(ir::GatheredTypeInfo(type, node, mod));
    type_map_.insert(std::make_pair(symbol->id(), just));
  }
}


void TypeRegistry::RegisterExernalPhaiType(Handle<ir::Symbol> symbol) {
  if (phai_map_.find(symbol->id()) == phai_map_.end()) {
    phai_map_.insert(std::make_pair(symbol->id(), global_type_registry_.phai_type()));
  }
}


Maybe<ir::GatheredTypeInfo> TypeRegistry::FindType(Handle<ir::Symbol> name) const {
  auto it = type_map_.find(name->id());
  if (it == type_map_.end()) {
    return global_type_registry_.FindType(name);
  }
  return it->second;
}


Maybe<ir::GatheredTypeInfo> TypeRegistry::FindPropertyType(Handle<ir::Node> prop) {
  // if (prop->HasGetPropView()) {
  //   Handle<ir::GetPropView> get_prop(prop);
  //   Maybe<ir::GatheredTypeInfo> gathered_type_info;
  //   if (!get_prop->prop()->HasNameView()) {
  //     // ERROR!
  //     error_reporter_->SemanticError(get_prop->prop()->source_position()) << "type expected.";
  //     return Nothing<ir::GatheredTypeInfo>();
  //   }
  //   if (get_prop->target()->HasGetPropView()) {
  //     gathered_type_info = FindPropertyType(get_prop->target());
  //   } else if (get_prop->target()->HasNameView()) {
  //     gathered_type_info = FindType(get_prop->target()->symbol());
  //   } else {
  //     return Just(GatheredTypeInfo(global_type_registry_.phai_type(), prop, ir::Type::Modifier::kPublic));
  //   }
    
  //   if (gathered_type_info) {
  //     Handle<ir::Type> type = gathered_type_info->value().type();
  //     if (type->IsVoid()) {
  //       // ERROR!
  //       error_reporter_->SemanticError(prop->source_position()) << "the void type is not contains data member.";
  //       return Nothing<ir::GatheredTypeInfo>();
  //     } else if (type->IsAny()) {
  //       return gathered_type_info;
  //     }

  //     if (type->IsComplexDataType()) {
  //       Handle<ir::PropertyType> property = type;
  //       return property->FindDeclaredType(get_prop->prop()->symbol());
  //     }

  //     error_reporter_->SemanticError(prop->source_position()) << "simple data type is not containts data member.";
  //     return Nothing<ir::GatheredTypeInfo>();
  //   }

  //   return 
  // }
  return Nothing<ir::GatheredTypeInfo>();
}
}
