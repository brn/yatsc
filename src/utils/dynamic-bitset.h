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

// Implementation of dynamic bit vector that is able to grow length of bit
// until the int max size.
// And support Rank(O(Log(N))) Select.
class DynamicBitset {
 public:
  // Construct bits from binary expression strings, like "010100011".
  explicit DynamicBitset(const char* bit_str);

  
  // Construct bits from uint32_t values.
  explicit DynamicBitset(uint32_t bit);

  
  // Default
  DynamicBitset();

  
  // Destructor.
  ~DynamicBitset();


  // Set or unset bit of the specified index.
  void Set(int index, bool val);


  // Get bit value of specified index.
  bool Get(int index) YATSC_NO_SE;


  // Count flaged bit number from zero to specified index.
  int Rank(int index) YATSC_NO_SE;

  
  int Select(int index);


  // Return true if any bit is flaged.
  YATSC_INLINE bool Any() YATSC_NO_SE;


  // Return true if all bits are flaged.
  YATSC_INLINE bool All() YATSC_NO_SE;


  // Return true if any bits are not flaged.
  YATSC_INLINE bool None() YATSC_NO_SE;


  // Return current used bits size.
  YATSC_INLINE size_t size() YATSC_NO_SE;

 private:

  void Grow(int len);

  YATSC_INLINE int GetLargeIndex(int index) YATSC_NO_SE;


  YATSC_INLINE int GetLargePosition(int index, int node_index) YATSC_NO_SE;


  YATSC_INLINE void MarkAsUpdated() YATSC_NO_SE {updated_ = true;}


  YATSC_INLINE void UnmarkUpdated() YATSC_NO_SE {updated_ = false;}


  YATSC_INLINE bool IsUpdated() YATSC_NO_SE {return updated_;}
  

  class BitNode {
   public:
    
    BitNode() {}
    
    ~BitNode() = default;

    BitNode(const BitNode& bit_node)
        : bits_(bit_node.bits_) {}

    BitNode& operator = (const BitNode& bit_node) {
      bits_ = bit_node.bits_;
      return (*this);
    }

    void Set(int index, bool val);

    
    YATSC_INLINE bool Get(int index) YATSC_NO_SE;

    
    YATSC_INLINE int SelfRank() YATSC_NO_SE;

    
    int GetSpecificRank(int num);

    
    int SelectSpecific(int num) YATSC_NO_SE;

    
    YATSC_PROPERTY(uint16_t, rank, rank_);

   private:
    class Position {
     public:
      Position(int large_index, int small_index, int small_position)
          : large_index(large_index),
            small_index(small_index),
            small_position(small_position) {}
      int large_index;
      int small_index;
      int small_position;
    };
    
    class Bit {
     public:
      Bit()
          : bits({0, 0}),
            mbit(0) {}
      Bit(const Bit& bit)
          : bits(bit.bits),
            mbit(bit.mbit) {}
      std::array<uint32_t, 2> bits; // 64
      uint8_t mbit;
    };

    YATSC_INLINE int GetSmallRank() YATSC_NO_SE;

    YATSC_INLINE int GetLargeRank() YATSC_NO_SE;

    YATSC_INLINE int CountFlagedBit(uint32_t bits) YATSC_NO_SE;

    YATSC_INLINE Position GetPosition(int num) YATSC_NO_SE;

    int IndexOf(int index, uint32_t bits, int all_index) YATSC_NO_SE;
    
    std::array<Bit, 2> bits_; // 128
    uint16_t rank_;
  };

  static const int kGrowSize;
  static const int kMinimumBitLength;
  static const int kMinimumBitBlockSize;
  static const int kBitNodeBitSize;
  static const int kEachBitBlockSize;
  static const int kIndexShifter;
  static const int kEachBitLength;
  static const int kMinMask;
  static const int kLargeMask;
  static const uint32_t kAllFlaged;
  static const uint32_t k31BitMask;
  
  int current_length_;
  size_t used_length_;
  mutable bool updated_;
  BitNode* bit_node_;
};

}

#include "dynamic-bitset-inl.h"

#endif
