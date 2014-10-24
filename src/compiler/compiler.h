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


#ifndef COMPILER_COMPILER_H
#define COMPILER_COMPILER_H

#include "./compilation-unit.h"
#include "../memory/heap.h"
#include "../utils/spinlock.h"
#include "./worker.h"

namespace yatsc {

class Compiler {
 public:
  Compiler(CompilerOption compiler_option);

  
  Vector<Handle<CompilationUnit>> Compile(const char* filename);

  
 private:

  void DoCompile(const char* filename);

  
  void AddResult(Handle<CompilationUnit> result);
  
  CompilerOption compiler_option_;
  Worker* worker_;
  Vector<Handle<CompilationUnit>> result_list_;
  SpinLock lock_;
};

}

#endif
