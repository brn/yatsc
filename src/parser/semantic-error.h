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


#ifndef YATSC_PARSER_SEMANTIC_ERROR_H
#define YATSC_PARSER_SEMANTIC_ERROR_H

#include "./error-descriptor.h"
#include "../memory/heap.h"
#include "../utils/stl.h"
#include "../utils/utils.h"
#include "./token.h"
#include "../ir/node.h"

namespace yatsc {

class SemanticError {
 public:
  typedef Vector<Handle<ErrorDescriptor>> ErrorBuffer;

  
  SemanticError() = default;


  ~SemanticError() = default;


  bool HasError() const {return !buffer_.empty();}


  const ErrorBuffer& errors() YATSC_NO_SE {return buffer_;}


  ErrorBuffer::iterator begin() {return buffer_.begin();}


  ErrorBuffer::const_iterator cbegin() const {return buffer_.begin();}


  ErrorBuffer::iterator end() {return buffer_.end();}


  ErrorBuffer::const_iterator cend() const {return buffer_.cend();}


  Handle<ErrorDescriptor> SyntaxError(Handle<ErrorDescriptor> errd) {
    buffer_.push_back(errd);
    return errd << "SyntaxError ";
  }
  

  Handle<ErrorDescriptor> SyntaxError(const SourcePosition& source_position) {
    return SyntaxError(Heap::NewHandle<ErrorDescriptor>(source_position));
  }


  Handle<ErrorDescriptor> SyntaxError(TokenInfo* info) {
    return SyntaxError(info->source_position());
  }


  Handle<ErrorDescriptor> SyntaxError(const TokenInfo& info) {
    return SyntaxError(info.source_position());
  }


  template <typename T>
  Handle<ErrorDescriptor> SyntaxError(Handle<T> node) {
    return SyntaxError(node->source_position());
  }

 private:
  ErrorBuffer buffer_;
};

}

#endif
