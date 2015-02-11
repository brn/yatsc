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
#include "../../src/utils/dynamic-bitset.h"


class ValueNode {
 public:
  ValueNode(int i, bool v)
      : index(i), val(v) {}
  int index;
  bool val;
};


static const int kSize = 30;
static const int kMaxRank = 20;
static const int kMaxRrank = 10;
static const int kMaxIndex = 3240;
static ValueNode kValues[] = {
  ValueNode(10, true),   // 1
  ValueNode(20, false),  // 1
  ValueNode(30, true),   // 2
  ValueNode(40, true),   // 3
  ValueNode(50, false),  // 3
  ValueNode(60, true),   // 4
  
  ValueNode(70, true),   // 5
  ValueNode(80, false),  // 5
  ValueNode(90, true),   // 6
  ValueNode(100, true),  // 7
  ValueNode(110, false), // 7
  ValueNode(120, true),  // 8
  
  ValueNode(130, true),  // 9
  ValueNode(140, false), // 9
  ValueNode(150, true),  // 10
  ValueNode(160, true),  // 11
  ValueNode(170, false), // 11
  ValueNode(180, true),  // 12
  
  ValueNode(190, true),  // 13
  ValueNode(200, false), // 13
  ValueNode(210, true),  // 14
  ValueNode(220, true),  // 15
  ValueNode(230, false), // 15
  ValueNode(240, true),   // 16


  ValueNode(3190, true),  // 17
  ValueNode(3200, false), // 17
  ValueNode(3210, true),  // 18
  ValueNode(3220, true),  // 19
  ValueNode(3230, false), // 19
  ValueNode(3240, true)   // 20
};

static ValueNode kRValues[] = {
  ValueNode(10, false),
  ValueNode(20, true),
  ValueNode(30, false),
  ValueNode(40, false),
  ValueNode(50, true),
  ValueNode(60, false),
  
  ValueNode(70, false),
  ValueNode(80, true),
  ValueNode(90, false),
  ValueNode(100, false),
  ValueNode(110, true),
  ValueNode(120, false),
  
  ValueNode(130, false),
  ValueNode(140, true),
  ValueNode(150, false),
  ValueNode(160, false),  
  ValueNode(170, true),
  ValueNode(180, false),
  
  ValueNode(190, false),
  ValueNode(200, true),
  ValueNode(210, false),
  ValueNode(220, false),
  ValueNode(230, true),
  ValueNode(240, false),

  ValueNode(3190, false),
  ValueNode(3200, true),
  ValueNode(3210, false),
  ValueNode(3220, false),
  ValueNode(3230, true),
  ValueNode(3240, false)
};


void SetBits(yatsc::DynamicBitset* bitset) {
  for (int i = 0; i < kSize; i++) {
    bitset->Set(kValues[i].index, kValues[i].val);
  }
}


void CheckBits(yatsc::DynamicBitset* bitset) {
  for (int i = 0; i < kSize; i++) {
    ASSERT_EQ(bitset->Get(kValues[i].index), kValues[i].val);
  }
}


void CheckRank(yatsc::DynamicBitset* bitset) {
  ASSERT_EQ(bitset->Rank(kMaxIndex), kMaxRank);
}


void SetRbits(yatsc::DynamicBitset* bitset) {
  for (int i = 0; i < kSize; i++) {
    bitset->Set(kRValues[i].index, kRValues[i].val);
  }
}

void CheckRbits(yatsc::DynamicBitset* bitset) {
  for (int i = 0; i < kSize; i++) {
    ASSERT_EQ(kRValues[i].val, bitset->Get(kRValues[i].index));
  }
}

void CheckRrank(yatsc::DynamicBitset* bitset) {
  ASSERT_EQ(kMaxRrank, bitset->Rank(kMaxIndex));
}


TEST(DynamicBitset, Set) {
  using namespace yatsc;
  DynamicBitset bitset;
  SetBits(&bitset);
  CheckBits(&bitset);
  SetRbits(&bitset);
  CheckRbits(&bitset);
}


TEST(DynamicBitset, Rank) {
  using namespace yatsc;
  DynamicBitset bitset;
  SetBits(&bitset);
  CheckRank(&bitset);
  SetRbits(&bitset);
  CheckRrank(&bitset);
}
