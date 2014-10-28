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
#include "./thread-pool.h"


namespace yatsc {

Compiler::Compiler(CompilerOption compiler_option)
    : compiler_option_(compiler_option) {
  compilation_scheduler_(&thread_pool_);
  notificator_.AddListener("Parser::ModuleFound", [&](Handle<ModuleInfo> module_info) {
    Schedule(module_info);
  });
}


Vector<Handle<CompilationUnit>> Compiler::Compile(const char* filename) {
  Handle<ModuleInfo> module_info = ModuleInfo::Create(filename);
  Schedule(module_info);
  thread_pool_.Wait();
  return result_list_;
}


void Compiler::Schedule(Handle<ModuleInfo> module_info) {
  if (!compilation_scheduler_->ShouldCompile(module_info)) {
    return;
  }
  compilation_scheduler_->AddCompilationCount(module_info);
  
  thread_pool_.send_request([module_info, this](int thread_id) {
    Run(module_info);
    compilation_scheduler_->ReleaseCompilationCount();
  });
}


void Compiler::Run(Handle<ModuleInfo> module_info) {
  SourceStream ss(module_info->module_name());
  if (!ss.success()) {
    AddResult(Heap::NewHandle<CompilationUnit>(module_info, ss.failed_message()));
  }
  printf("BEGIN %s\n", module_info->module_name());
  Handle<LiteralBuffer> literal_buffer = Heap::NewHandle<LiteralBuffer>();
  ErrorReporter er(ss.raw_buffer(), module_info);
  try {
    Scanner<UCharBuffer::iterator> scanner(
        ss.begin(),
        ss.end(),
        &er,
        literal_buffer.Get(),
        compiler_option_,
        module_info,
        [module_info, this](const UtfString& path){
          String dir = Path::Dirname(module_info->module_name());
          notificator_.NotifyForKey("Parser::ModuleFound", ModuleInfo::Create(Path::Join(dir, path.utf8_value())));
        });
    Parser<UCharBuffer::iterator> parser(compiler_option_, &scanner, notificator_, &er, module_info);
    Handle<ir::Node> root = parser.Parse();
    AddResult(Heap::NewHandle<CompilationUnit>(root, module_info, literal_buffer));
  } catch (const SyntaxError& e) {
    AddResult(Heap::NewHandle<CompilationUnit>(module_info, e.what()));
  } catch (const TokenException& e) {
    AddResult(Heap::NewHandle<CompilationUnit>(module_info, e.what()));
  } catch (const std::exception& e) {
    AddResult(Heap::NewHandle<CompilationUnit>(module_info, e.what()));
  } catch (...) {
    AddResult(Heap::NewHandle<CompilationUnit>(module_info, "Unhandled Error."));
  }
  printf("END %s %d\n", module_info->module_name(), compilation_scheduler_->count());
}


void Compiler::AddResult(Handle<CompilationUnit> result) {
  std::lock_guard<SpinLock> lock(lock_);
  result_list_.push_back(result);
}


}
