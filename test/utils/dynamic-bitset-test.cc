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
static const char* kBitStr = "101101101101101101101101101101";
static const bool kBitStrValue[] = {
  true, false, true, true, false, true,
  true, false, true, true, false, true,
  true, false, true, true, false, true,
  true, false, true, true, false, true,
  true, false, true, true, false, true
};

static ValueNode kValues[] = {
  ValueNode(10, true),   // 1
  ValueNode(20, false),  // 1
  ValueNode(30, true),   // 2
  ValueNode(40, true),   // 3
  ValueNode(50, false),  // 3
  ValueNode(63, true),   // 4
  
  ValueNode(70, true),   // 5
  ValueNode(80, false),  // 5
  ValueNode(90, true),   // 6
  ValueNode(100, true),  // 7
  ValueNode(110, false), // 7
  ValueNode(127, true),  // 8
  
  ValueNode(137, true),  // 9
  ValueNode(147, false), // 9
  ValueNode(157, true),  // 10
  ValueNode(167, true),  // 11
  ValueNode(177, false), // 11
  ValueNode(191, true),  // 12
  
  ValueNode(201, true),  // 13
  ValueNode(211, false), // 13
  ValueNode(221, true),  // 14
  ValueNode(231, true),  // 15
  ValueNode(241, false), // 15
  ValueNode(255, true),   // 16


  ValueNode(3200, true),  // 17
  ValueNode(3210, false), // 17
  ValueNode(3220, true),  // 18
  ValueNode(3230, true),  // 19
  ValueNode(3240, false), // 19
  ValueNode(3263, true)   // 20
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


void SetBitsAndCheckSelect(yatsc::DynamicBitset* bitset) {
  int j = 0;
  for (int i = 0; i < 5; i++) {
    int k = 0;
    for (;k < 6; ++k && j++) {
      bitset->Set(kValues[j].index, kValues[j].val);
    }
    ASSERT_EQ(kValues[j - 1].index + 1, bitset->Select(4 * (i + 1)));
  }
}


void CheckStrBits(yatsc::DynamicBitset* bitset) {
  for (int i = 0, len = 30; i < len; i++) {
    ASSERT_EQ(bitset->Get(i), kBitStrValue[i]);
  }
}


void SetBitsAndCheckRank(yatsc::DynamicBitset* bitset) {
  int j = 0;
  for (int i = 0; i < 5; i++) {
    int k = 0;
    for (;k < 6; ++k && j++) {
      bitset->Set(kValues[j].index, kValues[j].val);
    }
    ASSERT_EQ(4 * (i + 1), bitset->Rank(kValues[j - 1].index));
  }
}


void SetRbits(yatsc::DynamicBitset* bitset) {
  for (int i = 0; i < kSize; i++) {
    bitset->Set(kValues[i].index, !kValues[i].val);
  }
}

void CheckRbits(yatsc::DynamicBitset* bitset) {
  for (int i = 0; i < kSize; i++) {
    ASSERT_EQ(bitset->Get(kValues[i].index), !kValues[i].val);
  }
}


void SetRbitsAndCheckRrank(yatsc::DynamicBitset* bitset) {
  int j = 0;
  for (int i = 0; i < 5; i++) {
    int k = 0;
    for (;k < 6; ++k && j++) {
      bitset->Set(kValues[j].index, !kValues[j].val);
    }
    ASSERT_EQ(2 * (i + 1), bitset->Rank(kValues[j - 1].index));
  }
}


TEST(DynamicBitset, Set) {
  using namespace yatsc;
  DynamicBitset bitset;
  SetBits(&bitset);
  CheckBits(&bitset);
  SetRbits(&bitset);
  CheckRbits(&bitset);
}

TEST(DynamicBitset, Set_from_string) {
  using namespace yatsc;
  DynamicBitset bitset(kBitStr);
  CheckStrBits(&bitset);
}


TEST(DynamicBitset, Rank) {
  using namespace yatsc;
  DynamicBitset bitset;
  SetBitsAndCheckRank(&bitset);
  SetRbitsAndCheckRrank(&bitset);
}


TEST(DynamicBitset, Select) {
  using namespace yatsc;
  DynamicBitset bitset;
  SetBits(&bitset);
  SetBitsAndCheckSelect(&bitset);
}
