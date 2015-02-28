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


namespace yatsc {

YATSC_INLINE bool DynamicBitset::Any() YATSC_NO_SE {
  return bit_node_[used_length_].rank() > 0;
}


YATSC_INLINE bool DynamicBitset::All() YATSC_NO_SE {
  return bit_node_[used_length_].rank() == used_length_ * kEachBitLength;
}


YATSC_INLINE bool DynamicBitset::None() YATSC_NO_SE {
  return !Any();
}


YATSC_INLINE size_t DynamicBitset::size() YATSC_NO_SE {
  return used_length_ * kEachBitLength;
}


YATSC_INLINE int DynamicBitset::GetLargeIndex(int index) YATSC_NO_SE {
  return index >> kIndexShifter;
}


YATSC_INLINE int DynamicBitset::GetLargePosition(int index, int node_index) YATSC_NO_SE {
  return index - node_index * kEachBitLength;
}


YATSC_INLINE bool DynamicBitset::BitNode::Get(int num) YATSC_NO_SE {
  Position pos = GetPosition(num);
  int bit_value = (0x01 << pos.small_position - 1);
  return (bits_[pos.large_index].bits[pos.small_index] & bit_value) == bit_value;
}


YATSC_INLINE int DynamicBitset::BitNode::SelfRank() YATSC_NO_SE {
  return GetSmallRank() + GetLargeRank();
}


YATSC_INLINE int DynamicBitset::BitNode::GetSmallRank() YATSC_NO_SE {
  return ((bits_[0].mbit & kMinMask) >> 4) + (bits_[0].mbit & kLargeMask);
}


YATSC_INLINE int DynamicBitset::BitNode::GetLargeRank() YATSC_NO_SE {
  return ((bits_[1].mbit & kMinMask) >> 4) + (bits_[1].mbit & kLargeMask);
}


YATSC_INLINE int DynamicBitset::BitNode::CountFlagedBit(uint32_t bits) YATSC_NO_SE {
  bits = (bits & 0x55555555) + (bits >> 1 & 0x55555555);
  bits = (bits & 0x33333333) + (bits >> 2 & 0x33333333);
  bits = (bits & 0x0f0f0f0f) + (bits >> 4 & 0x0f0f0f0f);
  bits = (bits & 0x00ff00ff) + (bits >> 8 & 0x00ff00ff);
  return (bits & 0x0000ffff) + (bits >>16 & 0x0000ffff);
}


YATSC_INLINE DynamicBitset::BitNode::Position DynamicBitset::BitNode::GetPosition(int num) YATSC_NO_SE {
  int large_index    = num >> 6;
  int large_position = num - large_index * kBitNodeBitSize;
  int small_index    = large_position >> 5;
  int small_position = large_position - small_index * kMinimumBitLength;
  return Position(large_index, small_index, small_position);
}

}
