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
#include "../memory/heap.h"


namespace yatsc { namespace ir {
class Node;

#define TYPE_LIST(DEC, DEC_FIRST, DEC_LAST)     \
  DEC_FIRST(Phai)                               \
  DEC(TypeConstraints)                          \
  DEC(PlaceHolderType)                          \
  DEC(String)                                   \
  DEC(Number)                                   \
  DEC(Boolean)                                  \
  DEC(Void)                                     \
  DEC(Any)                                      \
  DEC(StructType)                               \
  DEC(Class)                                    \
  DEC(Interface)                                \
  DEC(CallSignature)                            \
      DEC_LAST(Enum)


enum class TypeId : uint8_t {
#define DECLARE(type) k##type,
#define DECLARE_FIRST(type) k##type = 1,
#define DECLARE_LAST(type) k##type
  TYPE_LIST(DECLARE, DECLARE_FIRST, DECLARE_LAST)
#undef DECLARE(type)
#undef DECLARE_FIRST(type)
#undef DECLARE_LAST(type)
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
    return type_id_ == TypeId::kBooleanType() ||
      type_id_ == TypeId::kStringType ||
      type_id_ == TypeId::kNumberType ||
      type_id_ == TypeId::kVoidType ||
      type_id_ == TypeId::kAnyType;
  }
  

  typedef uint8_t Modifier;

  class Modifiers: private Static {
   public:
    static Modifier kPublic = 0x1;
    static Modifier kPrivate = 0x2;
    static Modifier kProtected = 0x4;
    static Modifier kStatic = 0x8;
    static Modifier kExport = 0x16;
  };
  
  static Modifier ModifierFromToken(Token token) {
    return token == Token::TS_PUBLIC? Modifier::kPublic:
      token == Token::TS_PROTECTED? Modifier::kProtected:
      token == Token::TS_PRIVATE? Modifier::kPrivate:
      token == Token::TS_STATIC? Modifier::kStatic:
      token == Token::TS_EXPORT? Modifier::kExport: Modifier::kPublic;
  }
};



class TypedPropertyDescriptor {
 public:
  TypedPropertyDescriptor(Handle<Type> type, Handle<Node> prop, Type::Modifier modifier)
      : type_(type),
        node_(prop),
        modifier_(modifier) {}

  
  TypedPropertyDescriptor() = default;


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
      : Type(TypeId::kPlaceHolderType) {}


  YATSC_GETTER(Handle<Symbol>, symbol, symbol_);

  
 private:
  Handle<Symbol> symbol_;
};


class TypeConstraintsType: public Type {
 public:
  TypeConstraintsView(Handle<PlaceHolderType> derived, Handle<Type> base_type)
      : Type(TypeId::kTypeConstraintsType),
        derived_type_(derived),
        base_type_(base_type){}

  
  YATSC_GETTER(Handle<PlaceHolderType>, derived_type, derived_type_);


  YATSC_GETTER(Handle<Type>, base_type, base_type_);
  
  
 private:
  Handle<PlaceHolderType> derived_type_;
  Handle<Type> base_type_;
};


typedef HashMap<Unique::Id, Maybe<TypedPropertyDescriptor>> TypePropertyMap;
typedef std::pair<Unique::Id, Maybe<TypedPropertyDescriptor>> TypeDescriptor;
typedef IteratorRange<TypePropertyMap::iterator, TypePropertyMap::iterator> TypePropertyRange;


class GenericType: public Type {
  typedef HashMap<Unique::Id, Maybe<TypedPropertyDescriptor>> Map;
 public:
  GenericType(TypeId id)
      : Type(id) {}

  
  void AddTypeParameter(Handle<Type> type, Handle<Node> node) {
    type_param_map_.insert(std::make_pair(type->symbol()->id(), Just(TypedPropertyDescriptor(type, node))));
  }


  Maybe<TypedPropertyDescriptor> FindTypeParameter(Handle<Symbol> symbol) {
    auto it = type_param_map_.find(symbol->id());
    if (it == type_param_map_.end()) {
      return Nothing<TypedPropertyDescriptor>();
    }
    return it->second;
  }

 private:
  Mpa type_param_map_;
}


class PropertyType: public GenericType {
 public:
  
  PropertyType(TypeId id)
      : GenericType(id) {}
  

  void DeclareType(Handle<Symbol> symbol, Handle<Type> type, Handle<Node> prop, Type::Modifier modifier = Type::Modifier::kPublic) {
    properties_.insert(std::make_pair(symbol, Just(TypedPropertyDescriptor(type, prop, modifier))));
  }


  YATSC_INLINE Maybe<TypedPropertyDescriptor> FindDeclaredType(Handle<Symbol> symbol) {
    TypePropertyMap::iterator it = properties_.find(symbol);
    if (it == properties_.end()) {
      return Nothing<TypedPropertyDescriptor>();
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
      : Type(TypeId::kPhaiType) {}
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
  NumberType()
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
  typedef HashMap<Unique::Id, Maybe<TypedPropertyDescriptor>> Map;
 public:
  CallSignatureType()
      : Type(TypeId::kCallSignature) {}


  void AddParamType(Handle<Symbol> symbol, Handle<Node> node, Handle<Type> type) {
    param_map_.insert(std::make_pair(symbol->id(), Just(TypedPropertyDescriptor(type, node))));
  }

  
  Maybe<TypedPropertyDescriptor> FindParameterType(Handle<Symbol> symbol) {
    auto it = param_map_.find(symbol->id());
    if (it == param_map_.end()) {
      return Nothing<TypedPropertyDescriptor>();
    }
    return it->second;
  }


  YATSC_PROPERTY(Handle<Type>, return_type, return_type_);

  
 private:
  Map param_map_;
  Handle<Type> return_type_;
};


class ClassType: public PropertyType {
  typedef HashMap<Unique::Id, Maybe<TypedPropertyDescriptor>> Map;
 public:
  ClassType()
      : PropertyType(TypeId::kClass) {}
};


class InterfaceType: public PropertyType {
 public:
  InterfaceType()
      : PropertyType(TypeId::kInterfaceType) {}
};


template <typename Placeholder>
class InstanceType: public PropertyType {
  typedef HashMap<Unique::Id, Handle<Type>> Map;
 public:
  InstanceType(Handle<Placeholder> class_type)
      : PropertyType(TypeId::kInstance),
        class_type_(class_type) {}

  
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

  YATSC_GETTER(Handle<Placeholder>, generic_type, generic_type_);

 private:
  Handle<GenericType> generic_type_;
  Map parameterized_type_map_;
};


typedef InstanceType<ClassType> ClassInstanceType;
typedef InstanceType<InterfaceType> InterfaceInstanceType;
typedef InstanceType<CallSignatureType> CallSignatureInstanceType;


class EnumType: public PropertyType {
 public:
  EnumType()
      : PropertyType(TypeId::kEnumType) {}
};

}} // end yatsc::ir


#endif