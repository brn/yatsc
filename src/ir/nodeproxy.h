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


#include "./node.h"

#define COPYABLE_AND_MOVABLE(name)                                      \
  public:                                                               \
                                                                        \
  name(const name& node_proxy)                                          \
  : token_(node_proxy.token_) {}                                        \
                                                                        \
                                                                        \
  name(name&& node_proxy)                                               \
      : token_(std::move(node_proxy.token_)) {}                         \
                                                                        \
                                                                        \
  name& operator = (name node_proxy) YATSC_NOEXCEPT {                   \
    if (this != &parser_state) {                                        \
      node_proxy.Swap(*this);                                           \
    }                                                                   \
    return *this;                                                       \
  }                                                                     \
                                                                        \
                                                                        \
  name& operator = (name&& node_proxy) YATSC_NOEXCEPT {                 \
    if (this != &node_proxy) {                                          \
      name proxy(node_proxy);                                           \
      proxy.Swap(*this);                                                \
    }                                                                   \
    return *this;                                                       \
  }


namespace yatsc { namespace ir {

template <typename NodeType>
class NodeProxy {
};



// Proxy class for ir::NameView.
// This class only has getter of identifier token.
template <>
class NodeProxy<NameView> {
 public:
  NodeProxy(const Token* token)
      : token_(&token) {}


  COPYABLE_AND_MOVABLE(NodeProxy<NameView>);


  // The getter method for identifier.
  YATSC_CONST_GETTER(Token*, token, &token_);
  

 private:
  void Swap(NodeProxy<NameView>& b) YATSC_NOEXCEPT {
    token_ = b.token_;
  }

  
  Token token_;
};

}}
