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


#include "./utils.h"
#include "./os.h"


namespace yatsc {

YATSC_NORETURN void Fatal__(const char* file, int line, const char* function, const std::string& message) {
  FPrintf(stderr, "Fatal error occured, so process no longer exist.\nin file %s at line %d\n%s\n%s\n",
          file, line, function, message.c_str());
  PrintStackTrace();
  std::terminate();
}


void Assert__(bool ok, const char* result, const char* expect, const char* file, int line, const char* function) {
  if (!ok) {
    FPrintf(stderr, "assertion failed -> %s == %s\n in file %s at line %d\nIn function %s\n",
            result, expect, file, line, function);
    PrintStackTrace();
    std::terminate();
  }
}

std::atomic_ullong Unique::id_;

}
