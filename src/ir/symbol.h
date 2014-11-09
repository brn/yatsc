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


#ifndef YATSC_PARSER_SYMBOL_H
#define YATSC_PARSER_SYMBOL_H

#include <functional>
#include "../utils/utils.h"
#include "../parser/literalbuffer.h"

namespace yatsc { namespace ir {



#define SYMBOL_LIST(DECLARE, DECLARE_FIRST, DECLARE_LAST) \
  DECLARE_FIRST(VariableName)                           \
  DECLARE(FunctionName)                                 \
  DECLARE(PropertyName)                                 \
  DECLARE(ClassName)                                    \
  DECLARE(InterfaceName)                                \
  DECLARE(ModuleName)                                   \
  DECLARE(EnumName)                                     \
  DECLARE(TypeParameterName)                            \
  DECLARE(LabelName)                                    \
  DECLARE_LAST(ImportedTypeName)
  

#define DECLARE_SYMBOL_FIRST(type) k##type = 1,
#define DECLARE_SYMBOL(type) k##type,
#define DECLARE_SYMBOL_LAST(type) k##type

enum class SymbolType: uint8_t {
  SYMBOL_LIST(DECLARE_SYMBOL, DECLARE_SYMBOL_FIRST, DECLARE_SYMBOL_LAST)
};

#undef DECLARE_SYMBOL_FIRST
#undef DECLARE_SYMBOL
#undef DECLARE_SYMBOL_LAST


class Symbol {
 public:
  Symbol(SymbolType symbol_type, const Literal* value)
      : type_(symbol_type),
        value_(value) {}

  
  ~Symbol() = default;

  YATSC_CONST_GETTER(Unique::Id, id, value_->id());

  
  YATSC_PROPERTY(SymbolType, type, type_);

  
  YATSC_CONST_GETTER(const Literal*, value, value_);


  size_t utf8_length() const {return value_->utf8_length();}

  
  size_t utf16_length() const {return value_->utf16_length();}


  const Utf16String& utf16_string() const {return value_->utf16_string();}


  const Utf8String& utf8_string() const {return value_->utf8_string();}


  const UC16* utf16_value() const {return value_->utf16_value();}

  
  const char* utf8_value() const {return value_->utf8_value();}

  bool Equals(Handle<Symbol> symbol) YATSC_NO_SE {
    return symbol->value_->Equals(value_);
  }

  bool Equals(const char* value) YATSC_NO_SE {
    return value_->Equals(value);
  }

#define DECLARE_SYMBOL(t)                       \
  bool Is##t() {return SymbolType::k##t == type_;}

  SYMBOL_LIST(DECLARE_SYMBOL, DECLARE_SYMBOL, DECLARE_SYMBOL);
#undef DECLARE_SYMBOL
  
 private:
  SymbolType type_;
  const Literal* value_;
};

}}

#undef SYMBOL_LIST
#endif
