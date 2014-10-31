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


#ifndef YATSC_IR_PROPERTIES_H
#define YATSC_IR_PROPERTIES_H

#include "../utils/utils.h"
#include "../utils/unicode.h"
#include "../utils/stl.h"

namespace yatsc {namespace ir {

class Node;
class Scope;


typedef HashMap<Utf16String, Handle<Node>> PropertyMap;
typedef IteratorRange<PropertyMap::iterator, PropertyMap::iterator> PropertyRange;
typedef std::pair<Utf16String, Handle<Node>> Property;

class Properties: private Uncopyable {
 public:

  Properties() = default;


  ~Properties() = default;


  void Declare(const Utf16String& name, Handle<Node> prop) {
    properties_.insert(std::make_pair(name, prop));
  }


  YATSC_INLINE Property FindDeclaredItem(const Utf16String& name) {
    return *(properties_.find(name));
  };


  YATSC_INLINE PropertyRange declared_items() {return MakeRange(properties_.begin(), properties_.end());}

 private:
  HashMap<Utf16String, Handle<Node>> properties_;
};

}}

#endif
