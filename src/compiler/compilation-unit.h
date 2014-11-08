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
#include "../parser/semantic-error.h"
#include "../parser/sourcestream.h"
#include "../utils/stl.h"
#include "../ir/node.h"
#include "./module-info.h"


namespace yatsc {

class CompilationUnit {
 public:
  CompilationUnit(Handle<ir::Node> root, Handle<ModuleInfo> module_info, Handle<LiteralBuffer> literal_buffer);

  
  CompilationUnit(Handle<ModuleInfo> module_info);

  
  CompilationUnit(const CompilationUnit& compilation_unit);

  
  CompilationUnit(CompilationUnit&& compilation_unit);


  bool success() const {return !module_info_->HasError();}


  YATSC_CONST_GETTER(const char*, module_name, module_info_->module_name());


  YATSC_GETTER(Handle<SemanticError>, semantic_error, module_info_->semantic_error());


  YATSC_GETTER(Handle<ModuleInfo>, module_info, module_info_);

  
 private:
  Handle<ir::Node> root_;
  Handle<ModuleInfo> module_info_;
  Handle<LiteralBuffer> literal_buffer_;
};

}

#endif
