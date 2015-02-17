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

const int DynamicBitset::kMinimumBitLength    = 32;
const int DynamicBitset::kMinimumBitBlockSize = 2;
const int DynamicBitset::kBitNodeBitSize      = kMinimumBitLength * kMinimumBitBlockSize;
const int DynamicBitset::kEachBitBlockSize    = 2;
const int DynamicBitset::kIndexShifter        = 7;
const int DynamicBitset::kEachBitLength       = kBitNodeBitSize * kEachBitBlockSize;
const int DynamicBitset::kMinMask             = 0xF0;
const int DynamicBitset::kLargeMask           = 0xF;
const int DynamicBitset::kGrowSize            = 3;


DynamicBitset::DynamicBitset(uint32_t bit)
    : current_length_(2),
      used_length_(0),
      bit_node_(Heap::NewTypedPtr<BitNode>(sizeof(BitNode) * kGrowSize)) {
  for (int i = 0; i < kMinimumBitLength; i++) {
    Set(i, static_cast<bool>(bit & (0x1 << (i - 1))));
  }
}


DynamicBitset::DynamicBitset(const char* str)
    : current_length_(2),
      used_length_(0),
      bit_node_(Heap::NewTypedPtr<BitNode>(sizeof(BitNode) * kGrowSize)) {
  for (size_t i = 0, len = strlen(str); i < len; i++) {
    Set(static_cast<int>(i), static_cast<bool>(str[i] - '0'));
  }
}


DynamicBitset::DynamicBitset()
    : current_length_(2),
      used_length_(0),
      bit_node_(Heap::NewTypedPtr<BitNode>(sizeof(BitNode) * kGrowSize)) {}


DynamicBitset::~DynamicBitset() {
  Heap::Destruct(bit_node_);
}


void DynamicBitset::Set(int index, bool val) {
  int node_index = GetLargeIndex(index);
  if (node_index >= current_length_) {
    Grow(node_index);
  }
  
  used_length_ = node_index > used_length_? node_index: used_length_;
  
  bit_node_[node_index].Set(GetLargePosition(index, node_index), val);
}


bool DynamicBitset::Get(int num) YATSC_NO_SE {
  int index = GetLargeIndex(num);
  if (index > current_length_) {
    return false;
  }
  return bit_node_[index].Get(GetLargePosition(num, index));
}


int DynamicBitset::Rank(int num) YATSC_NO_SE {
  int node_index = GetLargeIndex(num);
  int pos        = GetLargePosition(num, node_index);
  int ret        = 0;
  
  if (node_index == 0) {
    return bit_node_[0].GetSpecificRank(num);
  }
  
  int i = 0;
  for (; i < node_index && node_index < current_length_; i++) {
    ret += bit_node_[i].Rank();
  }
  ret += bit_node_[i].GetSpecificRank(pos);
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


int DynamicBitset::BitNode::GetSpecificRank(int num) {
  Position pos = GetPosition(num);
  int ret = 0;
  if (pos.large_index == 0) {
    if (pos.small_index == 0) {
      ret = CountFlagedBit(bits_[0].bits[0] >> (kMinimumBitLength - pos.small_position));
    } else {
      ret = (bits_[0].mbit >> 4) + CountFlagedBit(bits_[0].bits[1] >> (kMinimumBitLength - pos.small_position));
    }
  } else {
    if (pos.small_index == 0) {
      ret = GetSmallRank() + CountFlagedBit(bits_[1].bits[0] >> (kMinimumBitLength - pos.small_position));
    } else {
      ret = GetSmallRank() + (bits_[1].mbit >> 4) + CountFlagedBit(bits_[1].bits[1] >> (kMinimumBitLength - pos.small_position));
    }
  }
  return ret;
}


void DynamicBitset::BitNode::Set(int num, bool val) {
  Position pos = GetPosition(num);
  if (val) {
    bits_[pos.large_index].bits[pos.small_index] |= (0x01 << pos.small_position - 1);
  } else {
    bits_[pos.large_index].bits[pos.small_index] &= ~(0x01 << pos.small_position - 1);
  }
  
  if (pos.small_index == 0) {
    int min_val = bits_[pos.large_index].mbit & kLargeMask;
    bits_[pos.large_index].mbit = CountFlagedBit(bits_[pos.large_index].bits[0]) << 4;
    bits_[pos.large_index].mbit |= min_val;
  } else {
    int large_val = bits_[pos.large_index].mbit & kMinMask;
    bits_[pos.large_index].mbit = CountFlagedBit(bits_[pos.large_index].bits[1]);
    bits_[pos.large_index].mbit |= large_val;
  }
}
}
