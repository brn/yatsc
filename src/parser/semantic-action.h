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


#include "../utils/utils.h"
#include "../ir/irfactory.h"
#include "../ir/nodeproxy.h"
#include "../ir/node.h"
#include "../ir/symbol.h"


namespace yatsc {

template <typename ResultType>
class SemanticAction {};


typedef ParseResult Maybe<Handle<ir::Node>>;


template <>
class SemanticAction<ParseResult> {
 public:

  // Action for ir::NameView.
  ParseResult Success(ir::NodeProxy<ir::NameView> proxy);


  // Set error information to this semantic action.
  template <typename T>
  ErrorDescriptor& ReportParseError(T item, Handle<ModuleInfo> module_info, const char* filename, int line);


  // Set warning information to this semantic action.
  template <typename T>
  ErrorDescriptor& ReportParseWarning(T item, Handle<ModuleInfo> module_info, const char* filename, int line);


  // Return failed type.
  ParseResult Failed();


  YATSC_INLINE bool IsTokenSkippable() YATSC_NO_SE;


 private:
  template <typename T, typename ... Args>
  Handle<T> New(Args ... args) {
    return irfactory_.New<T>(std::forward<Args>(args)...);
  }


  template <typename T>
  Handle<T> New(std::initializer_list<Handle<ir::Node>> list) {
    return irfactory_.New<T>(list);
  }


  YATSC_INLINE Handle<ir::Symbol> NewSymbol(ir::SymbolType type, const Literal* value) {
    return Heap::NewHandle<ir::Symbol>(type, value);
  }


  ir::IRFactory irfactory_;
};


template <>
class  SemanticAction<bool> {
 public:
  
  // Action for ir::NameView.
  bool Success(const ir::NodeProxy<ir::NameView>&) {return true;};


  // Set error information to this semantic action.
  template <typename T>
  YATSC_INLINE ErrorDescriptor& ReportParseError(T item, Handle<ModuleInfo> module_info, const char* filename, int line) YATSC_NO_SE {
    return SyntaxErrorBuilder<false>::Build(module_info);
  };


  // Set warning information to this semantic action.
  template <typename T>
  YATSC_INLINE ErrorDescriptor& ReportParseWarning(T item, Handle<ModuleInfo> module_info, const char* filename, int line) YATSC_NO_SE {
    return SyntaxErrorBuilder<false>::Build(module_info);
  }


  // Return failed type.
  bool Failed() {return false;};


  YATSC_INLINE bool IsTokenSkippable() YATSC_NO_SE {return false;}
};


typedef SemanticAction<bool> TestAction;
typedef SemanticAction<ParseResult> BuildAction;

}
