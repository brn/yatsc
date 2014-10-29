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


#include "../gtest-header.h"
#include "../../src/compiler/compiler.h"


inline ::testing::AssertionResult CheckCompilationResult(const yatsc::Vector<yatsc::Handle<yatsc::CompilationUnit>>& cu) {
  yatsc::StringStream ss;
  for (auto i: cu) {
    if (!i->success()) {
      ss << "Compile failed in module \"" << i->module_name() << "\"\n"
         << "Because\n" << i->error_message() << "\n";
    }
  }
  if (ss.str().size() > 0) {
    return ::testing::AssertionFailure() << ss.str();
  }
  return ::testing::AssertionSuccess();
}


inline void RunCompiler(const char* name) {
  yatsc::CompilerOption compiler_option;
  yatsc::Compiler compiler(compiler_option);
  yatsc::Vector<yatsc::Handle<yatsc::CompilationUnit>> cu = compiler.Compile(name);
  ASSERT_TRUE(CheckCompilationResult(cu));
}


TEST(Compiler, Compile) {
  RunCompiler("test/microsoft/typescript/src/compiler/tsc.ts");
  RunCompiler("test/microsoft/typescript/src/services/services.ts");
  //RunCompiler("/Users/aono_taketoshi/Documents/workspace/tagsys_tag_admin2/src/main/webapp/ts/src/main/tagmgr/site/siteId/report/summary-main.ts");

}
