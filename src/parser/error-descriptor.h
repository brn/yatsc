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

#ifndef YATSC_PARSER_ERROR_DESCRIPTOR_H
#define YATSC_PARSER_ERROR_DESCRIPTOR_H

#include "./sourceposition.h"
#include "../utils/stl.h"
#include "../utils/utils.h"

namespace yatsc {

class ErrorDescriptor {
 public:
  ErrorDescriptor(const SourcePosition& source_position)
      : source_position_(source_position) {}

    
  ErrorDescriptor(ErrorDescriptor&& error_descriptor)
      : source_position_(std::move(error_descriptor.source_position_)),
        error_message_(std::move(error_descriptor.error_message_)) {}


  template <typename T>
  ErrorDescriptor& operator << (T&& message) {
    error_message_ << message;
    return *this;
  }


  String message() const {
    return std::move(String(error_message_.str()));
  }


  YATSC_CONST_GETTER(const SourcePosition&, source_position, source_position_);
    

 private:
  SourcePosition source_position_;
  StringStream error_message_;
};

}

#endif
