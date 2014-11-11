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


#include <windows.h>
#include <imagehlp.h>

#pragma comment(lib, "imagehlp.lib")

namespace yatsc {

void PrintStackTrace() {
  unsigned int i;
  void* stack[ 100 ];
  unsigned short frames;
  SYMBOL_INFO* symbol;
  HANDLE process;
 
  process = GetCurrentProcess();
 
  SymInitialize(process, NULL, TRUE);
 
  frames = CaptureStackBackTrace(0, 100, stack, NULL);
  symbol = (SYMBOL_INFO*) calloc(sizeof(SYMBOL_INFO) + 256 * sizeof(char), 1);
  symbol->MaxNameLen = 255;
  symbol->SizeOfStruct = sizeof(SYMBOL_INFO);
 
  for(i = 0; i < frames; i++) {
    SymFromAddr(process, (DWORD64)(stack[i]), 0, symbol);
    printf("%i: %s - 0x%0X\n", frames - i - 1, symbol->Name, symbol->Address);
  }
 
  free(symbol);
}

}
