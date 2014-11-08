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


#include <stdio.h>
#include "sourcestream.h"
#include "unicode-iterator-adapter.h"

namespace yatsc {

SourceStream::SourceStream(const char* filepath)
    : MaybeFail(),
      filepath_(filepath) {
  Initialize();
}


void SourceStream::Initialize() {
  Stat stat(filepath_.c_str());
  bool exists = stat.IsExist();
  if (exists && stat.IsReg()) {
    size_ = stat.Size();
    try {
      FILE* fp = FOpen(filepath_.c_str(), "r");
      ReadBlock(fp);
      FClose(fp);
    } catch (const FileIOException& e) {
      Fail() << kCantOpenInput << filepath_
             << "\nbecause: " << e.what();
    }
  } else {
    size_ = 0;
    Fail() << kCantOpenInput << filepath_
           << "\nbeacause: " << "No such file or directory";
  }
}


void SourceStream::ReadBlock(FILE* fp)  {
  if (raw_buffer_.capacity() > size_ + 1) {
    raw_buffer_.reserve(size_ + 1);
    buffer_.reserve(size_);
  }
  
  char* str = reinterpret_cast<char*>(Heap::NewPtr(size_ + 1));
  size_t next = FRead(str, size_, sizeof(UC8), size_, fp);
  if (next > 0) {
    str[next] = '\0';
    raw_buffer_ = str;
  }
  
  auto it = UnicodeIteratorAdapter<char*>(str);
  for (;it != '\0'; ++it) {
    buffer_.push_back(std::move(*it));
  }

  Heap::Delete(str);
}
  

Handle<SourceStream> SourceStream::FromSourceCode(const char* name, const char* code) {
  auto ret = Heap::NewHandle<SourceStream>();
  ret->size_ = strlen(code);
  ret->filepath_ = name;
  ret->raw_buffer_ = code;
  auto it = UnicodeIteratorAdapter<char*>(const_cast<char*>(code));
  for (;it != '\0'; ++it) {
    ret->buffer_.push_back(std::move(*it));
  }
  return ret;
}


const char *SourceStream::kCantOpenInput = "Can not open input file: ";
}
