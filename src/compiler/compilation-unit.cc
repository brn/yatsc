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


#include "./compilation-unit.h"

namespace yatsc {

CompilationUnit::CompilationUnit(Handle<ir::Node> root,
                                 Handle<ModuleInfo> module_info,
                                 Handle<LiteralBuffer> literal_buffer)
    : root_(root),
      module_info_(module_info),
      literal_buffer_(literal_buffer) {}


CompilationUnit::CompilationUnit(Handle<ModuleInfo> module_info)
    : module_info_(module_info) {}


CompilationUnit::CompilationUnit(const CompilationUnit& compilation_unit)
    : root_(compilation_unit.root_),
      module_info_(compilation_unit.module_info_),
      literal_buffer_(compilation_unit.literal_buffer_) {}


CompilationUnit::CompilationUnit(CompilationUnit&& compilation_unit)
    : root_(std::move(compilation_unit.root_)),
      module_info_(std::move(compilation_unit.module_info_)),
      literal_buffer_(std::move(compilation_unit.literal_buffer_)) {}

}
