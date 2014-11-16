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

#include <atomic>
#include "./compilation-unit.h"
#include "../memory/heap.h"
#include "../utils/spinlock.h"
#include "../utils/notificator.h"
#include "../utils/stl.h"
#include "./thread-pool.h"
#include "./module-info.h"

namespace yatsc {

class Compiler {
 public:
  Compiler(CompilerOption compiler_option);

  
  Vector<Handle<CompilationUnit>> Compile(const char* filename);

  
 private:

  class CompilationScheduler {
   public:
    CompilationScheduler(ThreadPool* thread_pool)
        : thread_pool_(thread_pool) {
      count_ = 0;
    }

    
    YATSC_INLINE void AddCompilationCount(Handle<ModuleInfo> module_info) {
      ScopedSpinLock lock(lock_);
      ++count_;
      compiled_modules_.insert(module_info->module_name());
    };


    YATSC_INLINE void ReleaseCompilationCount() YATSC_NO_SE {
      if (--count_ == 0) {
        thread_pool_->Shutdown();
      }
    };


    YATSC_CONST_GETTER(int, count, count_.load(std::memory_order_relaxed));


    YATSC_INLINE bool ShouldCompile(Handle<ModuleInfo> module_info) YATSC_NO_SE {
      return compiled_modules_.find(module_info->module_name()) == compiled_modules_.end();
    }
   private:
    mutable std::atomic_int count_;
    ThreadPool* thread_pool_;
    HashSet<String> compiled_modules_;
    SpinLock lock_;
  };
  

  void Schedule(const String& filename);


  void Run(Handle<ModuleInfo> module_info);

  
  void AddResult(Handle<CompilationUnit> result);
  
  CompilerOption compiler_option_;
  LazyInitializer<CompilationScheduler> compilation_scheduler_;
  LazyInitializer<ThreadPool> thread_pool_;
  Vector<Handle<CompilationUnit>> result_list_;
  SpinLock lock_;
  Notificator<void(const String&)> notificator_;
  Handle<LiteralBuffer> literal_buffer_;
  Handle<ir::GlobalScope> global_scope_;
};

}

#endif
