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

#ifndef YATSC_PARSER_PARSER_BASE_H
#define YATSC_PARSER_PARSER_BASE_H

#include "../ir/node.h"
#include "../ir/irfactory.h"
#include "../ir/symbol.h"
#include "../parser/scanner.h"
#include "../compiler-option.h"
#include "../utils/utils.h"
#include "../utils/stl.h"
#include "../utils/notificator.h"
#include "../compiler/module-info.h"
#include "./error-descriptor.h"
#include "./error-reporter.h"


namespace yatsc {


class ParserBase: private Uncopyable, private Unmovable {
 public:
  
  ParserBase(const CompilerOption& co,
             const Notificator<void(const String&)>& notificator)
      : compiler_option_(co),
        notificator_(notificator),
        current_token_info_(nullptr) {}  

 protected:
  
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


  template <typename ... Args>
  void Notify(const char* key, Args ... args) {
    notificator_.NotifyForKey(key, std::forward<Args>(args)...);
  }


  bool CheckLineTermination(TokenInfo* info = nullptr);
  
  
  const CompilerOption& compiler_option_;
  const Notificator<void(const String&)>& notificator_;
  TokenInfo* current_token_info_;
  TokenInfo prev_token_info_;
  ir::IRFactory irfactory_;

#ifdef DEBUG
  String indent_;
#endif
};
} // yatsc

#endif // PARSER_PARSER_BASE_H
