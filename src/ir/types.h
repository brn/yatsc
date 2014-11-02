Y// The MIT License (MIT)
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


namespace yatsc {

class Type {
};


class MemberDescriptor {
 public:
  enum class Modifiers: uint8_t {
    kPublic = 0,
    kPrivate,
    kProtected,
    kExport
  };
  
  MemberDescriptor(Modifiers mod, Handle<Type> type)
      : mod_(mod),
        type_(type) {}

 private:
  Modifiers mod_;
  Handle<Type> type_;
};


class FunctionType: public Type {
 public:
  FunctionExpression()
      :Type() {}

 private:
  Vector<Handle<Type>> parameter_types_;
  Handle<Type> return_type_;
};


class ObjectType: public Type {

 public:
  ObjectTypeExpression()
      : Type() {}

  
 private:
  HashMap<Utf16String, Handle<Type>> property_map_;
  Vector<FunctionType> calls_;
};


class PropertiesType: public Type {
 public:
  PropertiesType()
      : Type() {}

 private:
  HashMap<Utf16String, MemberDescriptor> property_map_;
};


class InterfaceType: public PropertiesType {
 public:
  InterfaceType()
      : PropertiesType() {}

 private:
  Vector<Handle<Type>> extends_;
  
};


class ClassType: public PropertiesType {
 public:
  ClassType()
      : PropertiesType() {}

 private:
  Vector<Handle<Type>> impls_;
  Handle<Type> extend_;
};


class ModuleType: public PropertiesType {
 public:
  ModuleType()
      : PropertiesType() {}
};
}


#endif
