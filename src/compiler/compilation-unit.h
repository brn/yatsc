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


#ifndef COMPILER_COMPILATION_UNIT_H
#define COMPILER_COMPILATION_UNIT_H

#include "../memory/heap.h"
#include "../parser/literalbuffer.h"
#include "../utils/stl.h"
#include "../ir/node.h"
#include "./module-info.h"


namespace yatsc {

class CompilationUnit {
 public:
  CompilationUnit(Handle<ir::Node> root, Handle<ModuleInfo> module_info, Handle<LiteralBuffer> literal_buffer);

  
  CompilationUnit(Handle<ModuleInfo> module_info, const char* error_message);


  CompilationUnit(Handle<ModuleInfo> module_info, const String& error_message);

  
  CompilationUnit(const CompilationUnit& compilation_unit);

  
  CompilationUnit(CompilationUnit&& compilation_unit);


  bool success() {return error_message_.empty();}


  YATSC_CONST_GETTER(const char*, module_name, module_info_->module_name());


  YATSC_CONST_GETTER(const char*, error_message, error_message_.c_str());

  
 private:
  String error_message_;
  Handle<ir::Node> root_;
  Handle<ModuleInfo> module_info_;
  Handle<LiteralBuffer> literal_buffer_;
};

}

#endif
