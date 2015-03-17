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
#include "../utils/systeminfo.h"


namespace yatsc {

Compiler::Compiler(CompilerOption compiler_option)
    : compiler_option_(compiler_option) {
  thread_pool_(SystemInfo::GetOnlineProcessorCount() * 2);
  compilation_scheduler_(thread_pool_.Get());
  notificator_.AddListener("Parser::ModuleFound", [&](const String& module_name) {
    Schedule(module_name);
  });
}


Vector<Handle<CompilationUnit>> Compiler::Compile(const char* filename) {
  literal_buffer_  = Heap::NewHandle<LiteralBuffer>();
  global_scope_ = Heap::NewHandle<ir::GlobalScope>(literal_buffer_);
  
  Schedule(Path::Resolve(filename));
  thread_pool_->Wait();
  return result_list_;
}


void Compiler::Schedule(const String& filename) {
  auto module_info = ModuleInfo::Create(filename);
  
  if (!compilation_scheduler_->ShouldCompile(module_info)) {
    return;
  }
  compilation_scheduler_->AddCompilationCount(module_info);
  
  thread_pool_->send_request([module_info, this](int thread_id) mutable {
    Run(module_info);
    compilation_scheduler_->ReleaseCompilationCount();
  });
}


void Compiler::Run(Handle<ModuleInfo> module_info) {
  auto source_stream = module_info->source_stream();
  
  if (!source_stream->success()) {
    AddResult(Heap::NewHandle<CompilationUnit>(module_info));
  }
  printf("BEGIN %s\n", module_info->module_name());

  Scanner<SourceStream::iterator> scanner(
      source_stream->begin(),
      source_stream->end(),
      literal_buffer_.Get(),
      compiler_option_);
  
  Parser<SourceStream::iterator> parser(compiler_option_, &scanner, notificator_, module_info, global_scope_);
  
  try {
    ParseResult root_result = parser.Parse();
    if (!module_info->HasError() && root_result) {
      AddResult(Heap::NewHandle<CompilationUnit>(root_result.value(), module_info, literal_buffer_));
    } else {
      AddResult(Heap::NewHandle<CompilationUnit>(module_info));
    }
  } catch(const FatalParseError& fpe) {
    AddResult(Heap::NewHandle<CompilationUnit>(module_info));
  }

  printf("END %s %d\n", module_info->module_name(), compilation_scheduler_->count());
}


void Compiler::AddResult(Handle<CompilationUnit> result) {
  std::lock_guard<SpinLock> lock(lock_);
  result_list_.push_back(result);
}


}
