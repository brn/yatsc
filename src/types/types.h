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

#include "../parser/token.h"
#include "../utils/stl.h"
#include "../utils/utils.h"
#include "../utils/unicode.h"

namespace yatsc {

#define TYPE_LIST(DECLARE, DECLARE_FIRST, DECLARE_LAST)   \
  DECLARE_FIRST(FunctionType)                   \
  DECLARE(ObjectType)                           \
  DECLARE(InterfaceType)                        \
  DECLARE(ClassType)                            \
  DECLARE(ModuleType)                           \
  DECLARE(TypeParameter)                        \
  DECLARE_LAST(EnumType)


#define DECLARE_TYPE_FIRST(type) k##type = 1,
#define DECLARE_TYPE(type) k##type,
#define DECLARE_TYPE_LAST(type) k##type

enum class TypeId: uint8_t {
  TYPE_LIST(DECLARE_TYPE, DECLARE_TYPE_FIRST, DECLARE_TYPE_LAST)
};

#undef DECLARE_TYPE_FIRST(type)
#undef DECLARE_TYPE(type)
#undef DECLARE_TYPE_LAST(type)


class Type {

 public:
  Type(TypeId type)
      : type_(type) {}

#define TYPE_CHECK(t)                           \
  YATSC_INLINE bool Is##t() YATSC_NO_SE {return return t == type_;}
  TYPE_LIST(TYPE_CHECK, TYPE_CHECK, TYPE_CHECK)
#undef TYPE_CHECK
  
 private:
  type_;
};


class ParametarizedType {
 public:
  ParametarizedType(const Utf16String& name, std::initializer_list<const Utf16String&> type_parameter)
      : name_(name),
        type_parameter_(type_parameter) {}

 private:
  
};


class TypeParameterDescriptor {
 public:
  TypeParameterDescriptor(const Utf16String& name, Handle<TypeParameterDescriptor> constraints)
      : name_(name),
        constraints_(constraints) {}

  const Utf16String& name() YATSC_NO_SE {return name_;}

  Handle<TypeParameterDescriptor> constraints() YATSC_NO_SE {return constraints_;}
  
 private:
  const Utf16String& name_;
  Handle<TypeParameterDescriptor> constraints_;
};


class MemberDescriptor {
 public:  
  MemberDescriptor(Token mod, Handle<Type> type, Handle<TypeParameterDescriptor> type_parameter)
      : mod_(mod),
        type_(type),
        type_parameter_(type_parameter) {}

 private:
  Token mod_;
  Handle<Type> type_;
  Handle<TypeParameterDescriptor> type_parameter_;
};


class FunctionType: public Type {
 public:
  FunctionExpression()
      :Type(TypeId::kFunctionType) {}

  void AddParameterType(Handle<Type> type);

  Vector<Handle<Type>>& parameters() {return parameter_types_;}

  YATSC_PROPERTY(Handle<Type>, return_type, return_type_);

 private:
  Vector<Handle<Type>> parameter_types_;
  Handle<Type> return_type_;
};


class ObjectType: public Type {

 public:
  typedef HashMap<Utf16String, Handle<Type>> Properties;
  
  ObjectTypeExpression()
      : Type(TypeId::kObjectType) {}

  void AddProperty(const Utf16String& name, Handle<Type> property);

  Properties& properties() {return property_map_;}

  void AddCall(Handle<Type> call);

  Vector<FunctionType>& calls() {return calls_;}
  
 private:
  HashMap<Utf16String, Handle<Type>> property_map_;
  Vector<FunctionType> calls_;
};


class MemberType: public Type {
 public:
  typedef HashMap<Utf16String, MemberDescriptor> Members;
  
  MemberType(TypeId type)
      : Type(type) {}

  void AddMember(Token op, const Utf16String& name, Handle<Type> property);

  Members& members() {return property_map_;}

 private:
  HashMap<Utf16String, MemberDescriptor> property_map_;
};


class InterfaceType: public MemberType {
 public:
  InterfaceType()
      : MemberType(TypeId::kInterfaceType) {}

  void AddTypeParameter(Handle<TypeParameterDescriptor> descriptor);

  Vector<Handle<TypeParameterDescriptor>>& type_parameters() YATSC_NO_SE {return type_parameters_;}

 private:
  Vector<Handle<Type>> extends_;
  Vector<Handle<TypeParameterDescriptor>> type_parameters_;
};


class ClassType: public MemberType {
 public:
  ClassType()
      : MemberType(TypeId::kClassType) {}

  void AddTypeParameter(Handle<TypeParameterDescriptor> descriptor);

  Vector<Handle<TypeParameterDescriptor>>& type_parameters() YATSC_NO_SE {return type_parameters_;}
  
  void AddImpls(Handle<Type> type);

  Vector<Handle<Type>>& impls() YATSC_NO_SE {return impls_;}

  YATSC_PROPERTY(Type, constructor, constructor_);

  YATSC_PROPERTY(Type, extends, extends_);

 private:
  Vector<Handle<Type>> impls_;
  Vector<Handle<TypeParameterDescriptor>> type_parameters_;
  Handle<Type> extend_;
  Handle<Type> constructor_;
};


class ModuleType: public MemberType {
 public:
  ModuleType()
      : MemberType(TypeId::kModuleType) {}
};


class EnumType: public Type {
 public:
  EnumType()
      : Type(TypeId::kEnumType) {}

  void AddValue(const Utf16String& name);

  bool HasValue(const Utf16String& name);

 private:
  HashSet<Utf16String>
};
}


#endif
