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

#ifndef YATSC_PARSER_ERROR_FORMATTER_H
#define YATSC_PARSER_ERROR_FORMATTER_H

#include "../utils/utils.h"
#include "../utils/stl.h"
#include "../memory/heap.h"
#include "./error-reporter.h"

namespace yatsc {

class ModuleInfo;
class ErrorDescriptor;
class SourcePosition;

class ErrorFormatter {
 public:
  ErrorFormatter(Handle<ModuleInfo> module_info)
      : module_info_(module_info) {}

  
  String Format(Handle<ErrorDescriptor> error_descriptor) const {
    return Format(*error_descriptor);
  }


  String Format(const ErrorDescriptor&) const;


  void Print(FILE* fp, Handle<ErrorDescriptor> error_descriptor) const {Print(fp, *error_descriptor);}


  void Print(FILE* fp, const ErrorDescriptor& error_descriptor) const {
    FPrintf(fp, "%s\n", Format(error_descriptor).c_str());
  }


  void Print(FILE* fp, Handle<ErrorReporter> error_reporter) const {
    Print(fp, *error_reporter);
  }
  
  
  void Print(FILE* fp, const ErrorReporter& error_reporter) const {
    for (auto c: error_reporter.errors()) {
      Print(fp, *c);
    }
  }

 private:
  Vector<String> GetLineSource(const SourcePosition& source_position) const;
  
  Handle<ModuleInfo> module_info_;
};

}

#endif
