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


#ifndef YATSC_PARSER_ERROR_REPORTER_H
#define YATSC_PARSER_ERROR_REPORTER_H

#include "./error-descriptor.h"
#include "../memory/heap.h"
#include "../utils/stl.h"
#include "../utils/utils.h"
#include "./token.h"
#include "../ir/node.h"

namespace yatsc {

class ErrorReporter {
 public:
  typedef Vector<Handle<ErrorDescriptor>> ErrorBuffer;
  typedef ErrorBuffer WarningBuffer;

  
  ErrorReporter() = default;


  ~ErrorReporter() = default;


  bool HasError() const {return !buffer_.empty();}


  const ErrorBuffer& errors() YATSC_NO_SE {return buffer_;}


  const WarningBuffer& warnings() YATSC_NO_SE {return warning_buffer_;}


  size_t size() const {return buffer_.size();}


  void Pop() {
    buffer_.pop_back();
  }


  ErrorBuffer::iterator begin() {return buffer_.begin();}


  ErrorBuffer::const_iterator cbegin() const {return buffer_.begin();}


  ErrorBuffer::iterator end() {return buffer_.end();}


  ErrorBuffer::const_iterator cend() const {return buffer_.cend();}


  ErrorDescriptor& Ignore() {
    return ignore_desc_;
  }


  ErrorDescriptor& SyntaxError(Handle<ErrorDescriptor> errd) {
    buffer_.push_back(errd);
    return (*errd) << "SyntaxError ";
  }
  

  ErrorDescriptor& SyntaxError(const SourcePosition& source_position) {
    return SyntaxError(Heap::NewHandle<ErrorDescriptor>(source_position));
  }


  ErrorDescriptor& SyntaxError(Token* info) {
    return SyntaxError(info->source_position());
  }


  ErrorDescriptor& SyntaxError(const Token& info) {
    return SyntaxError(info.source_position());
  }


  ErrorDescriptor& SyntaxError(ir::Node* node) {
    return SyntaxError(node->source_position());
  }


  ErrorDescriptor& Warning(Handle<ErrorDescriptor> errd) {
    warning_buffer_.push_back(errd);
    return (*errd) << "Warning ";
  }
  

  ErrorDescriptor& Warning(const SourcePosition& source_position) {
    return Warning(Heap::NewHandle<ErrorDescriptor>(source_position));
  }


  ErrorDescriptor& Warning(Token* info) {
    return Warning(info->source_position());
  }


  ErrorDescriptor& Warning(const Token& info) {
    return Warning(info.source_position());
  }


  ErrorDescriptor& Warning(ir::Node* node) {
    return Warning(node->source_position());
  }


  ErrorDescriptor& SemanticError(Handle<ErrorDescriptor> errd) {
    buffer_.push_back(errd);
    return (*errd) << "SemanticError ";
  }


  ErrorDescriptor& SemanticError(const SourcePosition& source_position) {
    return SemanticError(Heap::NewHandle<ErrorDescriptor>(source_position));
  }


  ErrorDescriptor& SemanticError(Token* info) {
    return SemanticError(info->source_position());
  }


  ErrorDescriptor& SemanticError(const Token& info) {
    return SemanticError(info.source_position());
  }


  ErrorDescriptor& SemanticError(ir::Node* node) {
    return SemanticError(node->source_position());
  }

 private:
  static ErrorDescriptor ignore_desc_;
  ErrorBuffer buffer_;
  WarningBuffer warning_buffer_;
};


// template <bool cplusplus_info>
// class SyntaxErrorBuilder: private Static {
//  public:
//   template <typename T>
//   static ErrorDescriptor& Build(Handle<ModuleInfo> module_info, T item, const char* filename, int line) {
//     if (cplusplus_info) {
//       return module_info->error_reporter()->SyntaxError(item) << filename << ":" << line << "\n";
//     }
//     return module_info->error_reporter()->SyntaxError(item);
//   }

//   static ErrorDescriptor& Build(Handle<ModuleInfo> module_info) {
//     return module_info->error_reporter()->Ignore();
//   }


//   template <typename T>
//   static ErrorDescriptor& BuildWarning(Handle<ModuleInfo> module_info, T item, const char* filename, int line) {
//     if (cplusplus_info) {
//       return module_info->error_reporter()->Warning(item) << filename << ":" << line << "\n";
//     }
//     return module_info->error_reporter()->Warning(item);
//   }
// };

}

#endif
