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


namespace yatsc {
void SourceStream::Initialize() {
  Stat stat(filepath_.c_str());
  bool exists = stat.IsExist();
  if (exists && stat.IsReg()) {
    size_ = stat.Size();
    if (buffer_.capacity() < (size_ + 1)) {
      buffer_.reserve(size_ + 1);
    }
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
  char* buffer = new char[size_ + 1];
  size_t next = FRead(buffer, size_, sizeof(UC8), size_ - 1, fp);
  if (next > 0) {
    buffer[next] = '\0';
    buffer_.append(buffer);
  }
  delete[] buffer;
}
}
