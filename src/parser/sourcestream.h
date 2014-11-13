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
#include "./uchar.h"
#include "./unicode-iterator-adapter.h"
#include "../utils/error-reporter.h"
#include "../utils/os.h"
#include "../utils/stat.h"
#include "../utils/stl.h"
#include "../utils/utils.h"
#include "../utils/unicode.h"
#include "../memory/heap.h"


namespace yatsc {

class SourceStream : public MaybeFail, private Uncopyable {
 public:
  typedef UnicodeIteratorAdapter<char*> iterator;

  SourceStream(const char* filepath);

  SourceStream() = default;
  

  YATSC_INLINE ~SourceStream() = default;
  
  
  YATSC_INLINE UnicodeIteratorAdapter<char*> begin() {return UnicodeIteratorAdapter<char*>(raw_buffer_);}
  

  YATSC_INLINE UnicodeIteratorAdapter<char*> end() {return UnicodeIteratorAdapter<char*>(raw_buffer_ + size_);}


  YATSC_INLINE const char* raw_buffer() YATSC_NO_SE {return raw_buffer_;}


  YATSC_INLINE size_t size() YATSC_NO_SE {return size_;}


  static Handle<SourceStream> FromSourceCode(const String& name, const String& code) {
    return FromSourceCode(name.c_str(), code.c_str());
  }
  

  static Handle<SourceStream> FromSourceCode(const char* name, const char* code);
  

 private:
  void ReadBlock(FILE* fp);

  void Initialize();

  static const char* kCantOpenInput;
  
  size_t size_;
  String filepath_;
  char* raw_buffer_;
};
}
#endif
