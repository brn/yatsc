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


#include "../utils/maybe.h"
#include "../utils/utils.h"
#include "../ir/nodeproxy.h"
#include "../ir/node.h"


namespace yatsc {


template <>
template <typename T>
ResultType SemanticAction<ParseResult>::ReportParseError(
    T item,
    Handle<ModuleInfo> module_info,
    ParserState* parser_state,
    const char* filename,
    int line) {
  if (parser_state.IsInErrorRecoveryMode()) {
    return SyntaxErrorBuilder<DEBUG_BOOL>::Build(module_info);
  }
  parser_state.EnterErrorRecovery();
  return SyntaxErrorBuilder<DEBUG_BOOL>::Build(module_info, item, filename, line);
}


template <>
template <typename T>
ResultType SemanticAction<ParseResult>::ReportParseWarning(
    T item,
    Handle<ModuleInfo> module_info,
    ParserState* parser_state,
    const char* filename,
    int line) {
  return SyntaxErrorBuilder<DEBUG_BOOL>::BuildWarning(module_info, item, filename, line);
}


// Build up ir::NameView proxy NodeProxy<ir::NameView>.
template <>
ParseResult SemanticAction<ParseResult>::Success(ir::NodeProxy<ir::NameView> proxy) {
  auto node = New<ir::NameView>(NewSymbol(ir::SymbolType::kVariableName, proxy.token()->value()));
  node->SetInformationForNode(proxy.token());
  return Just(node);
}


template <>
ParseResult SemanticAction<ParseResult>::Failed() {
  return Nothing<ir::Node>();
}


template <ParseResult>
YATSC_INLINE bool SemanticAction<ParseResult>::IsTokenSkippable() YATSC_NO_SE {
  return true;
}

}
