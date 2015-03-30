// The MIT License (MIT)
// 
// Copyright (c) Taketoshi Aono(brn)
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

#ifndef IR_IR_FACTORY_H
#define IR_IR_FACTORY_H

#include "../utils/utils.h"
#include "../memory/heap.h"

namespace yatsc {namespace ir {

class IRFactory : private Uncopyable {
 public:

  IRFactory() = default;
  
  template <typename NodeName, typename ... Args>
  inline NodeName* New(Args ... args) {
    auto ret = unsafe_zone_allocator_.New<NodeName>(std::forward<Args>(args)...);
    ret->set_unsafe_zone_allocator(&unsafe_zone_allocator_);
    return ret;
  }

 private:
  UnsafeZoneAllocator unsafe_zone_allocator_;
};

}}

#endif
