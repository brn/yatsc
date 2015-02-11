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


#include "./dynamic-bitset.h"
#include "../memory/heap.h"

namespace yatsc {

static const int kAvailableBitLength = 27;
static const int kEachBitBlockSize = 5;
static const int kIndexShifter = 7;
static const int kEachBitLength = kAvailableBitLength * kEachBitBlockSize;


DynamicBitset::DynamicBitset()
    : current_length_(2),
      bit_node_(Heap::NewTypedPtr<BitNode>(sizeof(BitNode) * kGrowSize)) {}


DynamicBitset::~DynamicBitset() {
  Heap::Destruct(bit_node_);
}


YATSC_INLINE int GetIndex(int index) YATSC_NOEXCEPT {
  return index >> kIndexShifter;
}


YATSC_INLINE int GetPosition(int index, int node_index) YATSC_NOEXCEPT {
  return index - node_index * 128;
}


void DynamicBitset::Set(int index, bool val) {
  int node_index = GetIndex(index);
  if (node_index >= current_length_) {
    Grow(node_index);
  }
  bit_node_[node_index].Set(GetPosition(index, node_index), val);
}


bool DynamicBitset::Get(int num) {
  int index = GetIndex(num);
  if (index > current_length_) {
    return false;
  }
  return bit_node_[index].Get(GetPosition(num, index));
}


int DynamicBitset::Rank(int num) {
  int node_index = GetIndex(num);
  int pos = GetPosition(node_index, pos);
  int ret = 0;
  if (node_index == 0) {
    return bit_node_[0].Rank();
  }
  for (int i = 0; i <= node_index && node_index < current_length_; i++) {
    ret += bit_node_[i].Rank();
  }
  return ret;
}


int DynamicBitset::Select(int index) {
}


void DynamicBitset::Grow(int pos) {
  int next_size = pos + kGrowSize;
  BitNode* new_node = Heap::NewTypedPtr<BitNode>(sizeof(BitNode) * (next_size));
  for (int i = 0, len = current_length_; i < len; i++) {
    new_node[i] = bit_node_[i];
  }
  current_length_ = next_size;
  Heap::Destruct(bit_node_);
  bit_node_ = new_node;
}


const int DynamicBitset::kGrowSize = 3;


int CountFlagedBit(uint32_t bits) {
  bits = (bits & 0x55555555) + (bits >> 1 & 0x55555555);
  bits = (bits & 0x33333333) + (bits >> 2 & 0x33333333);
  bits = (bits & 0x0f0f0f0f) + (bits >> 4 & 0x0f0f0f0f);
  bits = (bits & 0x00ff00ff) + (bits >> 8 & 0x00ff00ff);
  return (bits & 0x0000ffff) + (bits >>16 & 0x0000ffff);
}


void DynamicBitset::BitNode::Set(int index, bool val) {
  int pos = index >> 5;
  if (val) {
    bits_[pos] |= (0x01 << index - 1);
  } else {
    bits_[pos] &= ~(0x01 << index - 1);
  }
  rank_ = CountFlagedBit(bits_[0])
    + CountFlagedBit(bits_[1])
    + CountFlagedBit(bits_[2])
    + CountFlagedBit(bits_[3]);
}


bool DynamicBitset::BitNode::Get(int index) {
  int pos = index >> 5;
  int bit_value = (0x01 << index - 1);
  return (bits_[pos] & bit_value) == bit_value;
}
}
