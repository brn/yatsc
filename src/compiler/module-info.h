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

#ifndef COMPILER_MODULE_INFO_H
#define COMPILER_MODULE_INFO_H

#include "../parser/sourcestream.h"
#include "../utils/stl.h"
#include "../parser/semantic-error.h"

namespace yatsc {

class ModuleInfo {
 public:
  ModuleInfo(const String& module_name, bool typescript)
      : source_stream_(Heap::NewHandle<SourceStream>(module_name.c_str())),
        module_name_(module_name),
        semantic_error_(Heap::NewHandle<SemanticError>()),
        typescript_(typescript) {}


  ModuleInfo(const String& module_name, const String& source_code, bool typescript)
      : source_stream_(SourceStream::FromSourceCode(module_name, source_code)),
        module_name_(module_name),
        semantic_error_(Heap::NewHandle<SemanticError>()),
        typescript_(typescript) {}


  YATSC_GETTER(Handle<SourceStream>, source_stream, source_stream_);


  YATSC_GETTER(Handle<SemanticError>, semantic_error, semantic_error_);
    
  
  YATSC_CONST_GETTER(const char*, module_name, module_name_.c_str());

  
  YATSC_CONST_GETTER(const String&, module_name_string, module_name_);


  YATSC_CONST_GETTER(bool, typescript, typescript_);


  bool IsDefinitionFile() const;


  bool HasError() const {return semantic_error_->HasError();}


  const char* raw_source_code() const {return source_stream_->raw_buffer();}


  static Handle<ModuleInfo> Create(const String& module_name) {return Create(module_name.c_str());}

  
  static Handle<ModuleInfo> Create(const char* module_name);
  
 private:
  Handle<SourceStream> source_stream_;
  String module_name_;
  Handle<SemanticError> semantic_error_;
  bool typescript_;
};

}

#endif
