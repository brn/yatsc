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

#ifndef PARSER_SOURCESTREAM_H_
#define PARSER_SOURCESTREAM_H_

#include <iterator>
#include <string>
#include "../utils/error-reporter.h"
#include "../utils/os.h"
#include "../utils/stat.h"
#include "../utils/utils.h"
#include "../utils/unicode.h"


namespace yatsc {

class SourceStream : public MaybeFail, private Uncopyable {
 public:
  typedef std::string::iterator iterator;
  SourceStream(const char* filepath);

  YATSC_INLINE ~SourceStream() = default;
  
  
  YATSC_INLINE iterator begin() {return buffer_.begin();}
  

  YATSC_INLINE iterator end() {return buffer_.end();}


  YATSC_INLINE const char* buffer() {return buffer_.c_str();}


  YATSC_INLINE size_t size() const {return size_;}


 private:
  YATSC_INLINE void ReadBlock(FILE* fp);

  YATSC_INLINE void Initialize();

  static const char* kCantOpenInput;
  
  size_t size_;
  std::string filepath_;
  std::string buffer_;
};
}

#include "./sourcestream-inl.h"
#endif
