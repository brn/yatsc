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


#ifndef UTILS_DYNAMIC_BITSET_H
#define UTILS_DYNAMIC_BITSET_H

#include <array>
#include "./utils.h"

namespace yatsc {

class DynamicBitset {
 public:
  DynamicBitset();
  ~DynamicBitset();

  void Set(int index, bool val);

  bool Get(int index);

  int Rank(int index);

  int Select(int index);

 private:

  void Grow(int len);

  class BitNode {
   public:
    
    BitNode()
        : bits_(),
          rank_(0) {}
    
    ~BitNode() = default;

    BitNode(const BitNode& bit_node)
        : bits_(bit_node.bits_),
          rank_(bit_node.rank_) {}

    BitNode& operator = (const BitNode& bit_node) {
      bits_ = bit_node.bits_;
      rank_ = bit_node.rank_;
      return (*this);
    }

    void Set(int index, bool val);

    
    bool Get(int index);

    
    int Rank() {return rank_;}

   private:
    std::array<uint32_t, 5> bits_;
    uint8_t rank_;
  };

  static const int kGrowSize;
  int current_length_;
  BitNode* bit_node_;
};

}

#endif
