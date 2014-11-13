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
      char* heap = reinterpret_cast<char*>(Heap::NewPtr(size_ + 1));
      setvbuf(fp, heap, _IOFBF, size_ + 1);
      ReadBlock(fp);
      FClose(fp);
      Heap::Delete(heap);
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
  raw_buffer_ = reinterpret_cast<char*>(Heap::NewPtr(size_ + 1));
  size_t next = FRead(raw_buffer_, size_, sizeof(UC8), size_, fp);
  if (next > 0) {
    raw_buffer_[size_] = '\0';
  }
}
  

Handle<SourceStream> SourceStream::FromSourceCode(const char* name, const char* code) {
  auto ret = Heap::NewHandle<SourceStream>();
  ret->size_ = strlen(code);
  ret->filepath_ = name;
  strcpy(ret->raw_buffer_, code);
  return ret;
}


const char *SourceStream::kCantOpenInput = "Can not open input file: ";
}
