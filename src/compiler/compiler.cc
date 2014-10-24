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


#include "./compiler.h"
#include "../parser/literalbuffer.h"
#include "../parser/parser.h"
#include "../parser/scanner.h"
#include "../parser/sourcestream.h"
#include "../utils/path.h"
#include "./module-info.h"
#include "./worker.h"


namespace yatsc {

Compiler::Compiler(CompilerOption compiler_option)
    : compiler_option_(compiler_option),
      worker_(Worker::default_worker()){}


Vector<Handle<CompilationUnit>> Compiler::Compile(const char* filename) {
  DoCompile(filename);
  while (worker_->running_thread_count() == 0) {}
  while (worker_->running_thread_count()) {}
  return result_list_;
}


void Compiler::DoCompile(const char* filename) {
  worker_->send_request([&]{
    SourceStream ss(filename);
    if (!ss.success()) {
      AddResult(Heap::NewHandle<CompilationUnit>(ss.failed_message()));
    }
    Handle<LiteralBuffer> literal_buffer = Heap::NewHandle<LiteralBuffer>();
    Handle<ModuleInfo> module_info = Heap::NewHandle<ModuleInfo>(Path::Resolve(filename), Path::Extname(filename) == ".ts");
    ErrorReporter er(ss.raw_buffer(), module_info);
    Scanner<UCharBuffer::iterator> scanner(ss.begin(), ss.end(), &er, literal_buffer.Get(), compiler_option_, module_info);
    Parser<UCharBuffer::iterator> parser(compiler_option_, &scanner, &er, module_info);
    try {
      Handle<ir::Node> root = parser.Parse();
      AddResult(Heap::NewHandle<CompilationUnit>(root, literal_buffer));
    } catch (const SyntaxError& e) {
      AddResult(Heap::NewHandle<CompilationUnit>(e.what()));
    } catch (const std::exception& e) {
      AddResult(Heap::NewHandle<CompilationUnit>(e.what()));
    }
  });
}


void Compiler::AddResult(Handle<CompilationUnit> result) {
  std::lock_guard<SpinLock> lock(lock_);
  result_list_.push_back(result);
}

}
