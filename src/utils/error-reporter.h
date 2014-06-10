/*
 * The MIT License (MIT)
 * 
 * Copyright (c) 2013 Taketoshi Aono(brn)
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */


#ifndef ERROR_REPORTER_H_
#define ERROR_REPORTER_H_

#include <iostream>
#include <stdexcept>
#include <sstream>
#include <stdarg.h>
#include "utils.h"
#include "os.h"



namespace rasp {
class ErrorReporter {
 public:
  
  RASP_INLINE ErrorReporter(const char* message)
      : message_(message){}
  

  RASP_INLINE ErrorReporter(const std::string& message):
      message_(std::move(message)){}


  RASP_INLINE ErrorReporter(const ErrorReporter& e)
      : message_(e.message_){}


  RASP_INLINE ErrorReporter(const ErrorReporter&& e)
      : message_(std::move(e.message_)){}

  
  RASP_INLINE void Report(bool is_exit = false) {
    std::cerr << message_ << std::endl;
    if (is_exit) exit(1);
  }

 private:
  std::string message_;
};


class MaybeFail {
 public:
  RASP_INLINE MaybeFail()
      : success_(true){}
  
  virtual ~MaybeFail() = default;

  
  RASP_INLINE bool success() const {
    return success_;
  }


  RASP_INLINE std::stringstream& Fail() {
    success_ = false;
    return message_stream_;
  }

  
  RASP_INLINE std::string failed_message() const {
    return message_stream_.str();
  }
  
 private:
  bool success_;
  std::stringstream message_stream_;
};

} //namespace rasp


#endif
