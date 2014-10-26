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

#include "./module-info.h"
#include "../utils/path.h"
#include "../utils/stat.h"

namespace yatsc {

Handle<ModuleInfo> ModuleInfo::Create(const char* module_name) {
  bool typescript = false;
  String name = module_name;
  String ext = Path::Extname(name);
  if (ext == ".ts") {
    typescript = true;
  } else if (ext.empty()) {
    if (Stat(name + ".ts").IsExistsAndFile()) {
      typescript = true;
      name += ".ts";
    } else if (Stat(name + ".js").IsExistsAndFile()) {
      name += ".js";
    } else if (Stat(name + ".html").IsExistsAndFile()) {
      name += ".html";
    }
  }

  return Heap::NewHandle<ModuleInfo>(Path::Resolve(module_name), typescript);
}

}
