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


#ifndef YATSC_IR_TYPES_H
#define YATSC_IR_TYPES_H


#include "../utils/utils.h"
#include "../utils/stl.h"
#include "../utils/maybe.h"
#include "../memory/heap.h"
#include "../parser/token.h"
#include "./symbol.h"


namespace yatsc { namespace ir {
class Node;

#define TYPE_LIST(DEC, DEC_FIRST, DEC_LAST)     \
  DEC_FIRST(Phai)                               \
  DEC(Array)                                    \
  DEC(TypeConstraints)                          \
  DEC(PlaceHolder)                              \
  DEC(String)                                   \
  DEC(Number)                                   \
  DEC(Boolean)                                  \
  DEC(Void)                                     \
  DEC(Any)                                      \
  DEC(Struct)                                   \
  DEC(Class)                                    \
  DEC(Interface)                                \
  DEC(CallSignature)                            \
  DEC(Instance)                                 \
  DEC_LAST(Enum)


enum class TypeId : uint8_t {
#define DECLARE(type) k##type,
#define DECLARE_FIRST(type) k##type = 1,
#define DECLARE_LAST(type) k##type
  TYPE_LIST(DECLARE, DECLARE_FIRST, DECLARE_LAST)
#undef DECLARE
#undef DECLARE_FIRST
#undef DECLARE_LAST
};


class Type {
 public:
  Type(TypeId id)
      : type_id_(id) {}
  
#define DECLARE(type)                           \
  YATSC_INLINE bool Is##type() YATSC_NO_SE {    \
    return type_id_ == TypeId::k##type;         \
  }
  TYPE_LIST(DECLARE, DECLARE, DECLARE)
#undef DECLARE

  
  bool IsBuiltInType() {
    return type_id_ == TypeId::kBoolean ||
      type_id_ == TypeId::kString ||
      type_id_ == TypeId::kNumber ||
      type_id_ == TypeId::kVoid ||
      type_id_ == TypeId::kAny;
  }


  bool IsComplexDataType() {
    return type_id_ == TypeId::kInstance ||
      type_id_ == TypeId::kStruct ||
      type_id_ == TypeId::kClass ||
      type_id_ == TypeId::kInterface ||
      type_id_ == TypeId::kEnum;
  }
  

  enum class Modifier: uint8_t {
    kPublic = 0x1,
    kPrivate = 0x2,
    kProtected = 0x4,
    kStatic = 0x8,
    kExport = 0x16
  };
  
  static Modifier ModifierFromToken(TokenKind token) {
    return token == TokenKind::kPublic? Modifier::kPublic:
      token == TokenKind::kProtected? Modifier::kProtected:
      token == TokenKind::kPrivate? Modifier::kPrivate:
      token == TokenKind::kStatic? Modifier::kStatic:
      token == TokenKind::kExport? Modifier::kExport: Modifier::kPublic;
  }

 private:
  TypeId type_id_;
};


class GatheredTypeInfo {
 public:
  GatheredTypeInfo(Handle<Type> type, Handle<Node> prop, Type::Modifier modifier)
      : type_(type),
        node_(prop),
        modifier_(modifier) {}

  
  GatheredTypeInfo() = default;


  GatheredTypeInfo(const GatheredTypeInfo& tp)
      : type_(tp.type_),
        node_(tp.node_),
        modifier_(tp.modifier_) {}


  GatheredTypeInfo(GatheredTypeInfo&& tp)
      : type_(std::move(tp.type_)),
        node_(std::move(tp.node_)),
        modifier_(tp.modifier_) {}


  GatheredTypeInfo& operator = (const GatheredTypeInfo& tp) {
    GatheredTypeInfo ret(tp);
    type_ = ret.type_;
    node_ = ret.node_;
    modifier_ = ret.modifier_;
    return *this;
  }


  GatheredTypeInfo& operator = (GatheredTypeInfo&& tp) {
    GatheredTypeInfo ret(tp);
    type_ = ret.type_;
    node_ = ret.node_;
    modifier_ = ret.modifier_;
    return *this;
  }


  YATSC_GETTER(Handle<Type>, type, type_);
  YATSC_GETTER(Handle<Node>, node, node_);
  YATSC_GETTER(Type::Modifier, modifier, modifier_);
  

 private:
  Handle<Type> type_;
  Handle<Node> node_;
  Type::Modifier modifier_;
};


class PlaceHolderType: public Type {
 public:
  PlaceHolderType(Handle<Symbol> symbol)
      : Type(TypeId::kPlaceHolder) {}


  YATSC_GETTER(Handle<Symbol>, symbol, symbol_);

  
 private:
  Handle<Symbol> symbol_;
};


class TypeConstraintsType: public Type {
 public:
  TypeConstraintsType(Handle<PlaceHolderType> derived, Handle<Type> base_type)
      : Type(TypeId::kTypeConstraints),
        derived_type_(derived),
        base_type_(base_type){}

  
  YATSC_GETTER(Handle<PlaceHolderType>, derived_type, derived_type_);


  YATSC_GETTER(Handle<Type>, base_type, base_type_);
  
  
 private:
  Handle<PlaceHolderType> derived_type_;
  Handle<Type> base_type_;
};


typedef HashMap<Unique::Id, Maybe<GatheredTypeInfo>> TypePropertyMap;
typedef std::pair<Unique::Id, Maybe<GatheredTypeInfo>> TypeDescriptor;
typedef IteratorRange<TypePropertyMap::iterator, TypePropertyMap::iterator> TypePropertyRange;


class GenericType: public Type {
  typedef HashMap<Unique::Id, Maybe<GatheredTypeInfo>> Map;
 public:
  GenericType(TypeId id)
      : Type(id) {}

  
  void AddTypeParameter(Handle<Symbol> symbol, Handle<Type> type, Handle<Node> node) {
    type_param_map_.insert(std::make_pair(symbol->id(), Just(GatheredTypeInfo(type, node, Type::Modifier::kPublic))));
  }


  Maybe<GatheredTypeInfo> FindTypeParameter(Handle<Symbol> symbol) {
    auto it = type_param_map_.find(symbol->id());
    if (it == type_param_map_.end()) {
      return Nothing<GatheredTypeInfo>();
    }
    return it->second;
  }

 private:
  Map type_param_map_;
};


class ArrayType: public Type {
 public:
  ArrayType(Handle<Type> type)
      : Type(TypeId::kArray),
        type_(type) {}

  YATSC_CONST_PROPERTY(Handle<Type>, type, type_);
  
 private:
  Handle<Type> type_;
};


class PropertyType: public GenericType {
 public:
  
  PropertyType(TypeId id)
      : GenericType(id) {}
  

  void DeclareType(Handle<Symbol> symbol, Handle<Type> type, Handle<Node> prop, Type::Modifier modifier = Type::Modifier::kPublic) {
    properties_.insert(std::make_pair(symbol, Just(GatheredTypeInfo(type, prop, modifier))));
  }


  YATSC_INLINE Maybe<GatheredTypeInfo> FindDeclaredType(Handle<Symbol> symbol) {
    TypePropertyMap::iterator it = properties_.find(symbol);
    if (it == properties_.end()) {
      return Nothing<GatheredTypeInfo>();
    }
    return it->second;
  };


  YATSC_INLINE TypePropertyRange declared_types() {return MakeRange(properties_.begin(), properties_.end());}
  
 private:
  TypePropertyMap properties_;
};


class PhaiType: public Type {
 public:
  PhaiType()
      : Type(TypeId::kPhai) {}
};


class StringType: public Type {
 public:
  StringType()
      : Type(TypeId::kString) {}

  
  YATSC_PROPERTY(Handle<Type>, box_type, box_type_);

 private:
  Handle<Type> box_type_;
};


class NumberType: public Type {
 public:
  NumberType()
      : Type(TypeId::kNumber) {}

  YATSC_PROPERTY(Handle<Type>, box_type, box_type_);

 private:
  Handle<Type> box_type_;
};


class BooleanType: public Type {
 public:
  BooleanType()
      : Type(TypeId::kBoolean) {}

  YATSC_PROPERTY(Handle<Type>, box_type, box_type_);

 private:
  Handle<Type> box_type_;
};


class VoidType: public Type {
 public:
  VoidType()
      : Type(TypeId::kVoid) {}
};


class AnyType: public Type {
 public:
  AnyType()
      : Type(TypeId::kAny) {}
};


class StructType: public PropertyType {
 public:
  StructType()
      : PropertyType(TypeId::kStruct) {}
};


class CallSignatureType: public GenericType {
  typedef HashMap<Unique::Id, Maybe<GatheredTypeInfo>> Map;
 public:
  CallSignatureType()
      : GenericType(TypeId::kCallSignature) {}


  void AddParamType(Handle<Symbol> symbol, Handle<Node> node, Handle<Type> type) {
    param_map_.insert(std::make_pair(symbol->id(), Just(GatheredTypeInfo(type, node, Type::Modifier::kPublic))));
  }

  
  Maybe<GatheredTypeInfo> FindParameterType(Handle<Symbol> symbol) {
    auto it = param_map_.find(symbol->id());
    if (it == param_map_.end()) {
      return Nothing<GatheredTypeInfo>();
    }
    return it->second;
  }


  YATSC_PROPERTY(Handle<Type>, return_type, return_type_);

  
 private:
  Map param_map_;
  Handle<Type> return_type_;
};


class ClassType: public PropertyType {
 public:
  ClassType()
      : PropertyType(TypeId::kClass) {}
};


class InterfaceType: public PropertyType {
 public:
  InterfaceType()
      : PropertyType(TypeId::kInterface) {}
};


template <typename Placeholder>
class InstanceType: public PropertyType {
  typedef HashMap<Unique::Id, Handle<Type>> Map;
 public:
  InstanceType(Handle<Placeholder> generic_type)
      : PropertyType(TypeId::kInstance),
        generic_type_(generic_type) {}

  
  void add_type_parameter(Handle<Symbol> symbol, Handle<Type> type) {
    parameterized_type_map_.insert(std::make_pair(symbol->id(), type));
  }


  Handle<Type> FindTypeParameter(Handle<Symbol> symbol) {
    Map::iterator it = parameterized_type_map_.find(symbol->id());
    if (it == parameterized_type_map_.end()) {
      return Handle<Type>();
    }
    return it->second;
  }

  YATSC_CONST_GETTER(Handle<GenericType>, generic_type, generic_type_);

 private:
  Handle<Placeholder> generic_type_;
  Map parameterized_type_map_;
};


typedef InstanceType<ClassType> ClassInstanceType;
typedef InstanceType<InterfaceType> InterfaceInstanceType;
typedef InstanceType<CallSignatureType> CallSignatureInstanceType;


class EnumType: public PropertyType {
 public:
  EnumType()
      : PropertyType(TypeId::kEnum) {}
};

}} // end yatsc::ir


#endif
