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


#include <unistd.h>
#include <execinfo.h>
#include <cxxabi.h>


namespace yatsc {
void PrintStackTrace() {
  static const int kSize = 20;
  void *array[kSize];
  
  // get void*'s for all entries on the stack
  int size = backtrace(array, kSize);
  char** symbols =  backtrace_symbols(array, size);
  for (int i = 0; i < size; i++) {
    int status = 0;
    char* demangled = abi::__cxa_demangle(symbols[i], 0, 0, &status);
    if (status == 0) {
      printf("%s\n", demangled);
    } else {
      printf("%s\n", symbols[i]);
    }
    free(demangled);
  }
  free(symbols);
}
}
