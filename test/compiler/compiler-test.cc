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
#include "../../src/parser/error-formatter.h"


inline ::testing::AssertionResult CheckCompilationResult(const yatsc::Vector<yatsc::Handle<yatsc::CompilationUnit>>& cu) {
  yatsc::StringStream ss;
  for (auto i: cu) {
    if (!i->success()) {
      ss << "Compile failed in module \"" << i->module_name() << "\"\n"
         << "Because\n";
      yatsc::ErrorFormatter ef(i->module_info());
      for (auto c: i->error_reporter()->errors()) {
        ss << ef.Format(c);
      }
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


TEST(Compiler, Compile_Products) {
  // RunCompiler(PRODUCT_DIR"/test/microsoft/typescript/src/compiler/tsc.ts");
  // RunCompiler(PRODUCT_DIR"/test/microsoft/typescript/src/services/services.ts");
  // RunCompiler(PRODUCT_DIR"/test/doppio/console/doppioh.ts");
  //RunCompiler(PRODUCT_DIR"/test/promises-typescript/lib/Promises.ts");
  RunCompiler("/Users/a12360/Documents/workspace/tagsys_tag_admin2/src/main/webapp/ts/src/main/tagmgr/site/siteid/draft/tag/customHtml/edit-main.ts");
  RunCompiler("/Users/a12360/Documents/workspace/tagsys_tag_admin2/src/main/webapp/ts/src/main/tagmgr/site/siteid/draft/macro/edit-main.ts");
}


TEST(Compiler, Compile_Ambient) {
  RunCompiler(PRODUCT_DIR"/test/microsoft/typescript/src/lib/core.d.ts");
  RunCompiler(PRODUCT_DIR"/test/microsoft/typescript/src/lib/dom.generated.d.ts");
  RunCompiler(PRODUCT_DIR"/test/microsoft/typescript/src/lib/extension.d.ts");
  RunCompiler(PRODUCT_DIR"/test/microsoft/typescript/src/lib/importcore.d.ts");
  RunCompiler(PRODUCT_DIR"/test/microsoft/typescript/src/lib/scriptHost.d.ts");
  RunCompiler(PRODUCT_DIR"/test/microsoft/typescript/src/lib/webworker.generated.d.ts");
  RunCompiler(PRODUCT_DIR"/test/microsoft/typescript/src/lib/webworker.importscripts.d.ts");
}
