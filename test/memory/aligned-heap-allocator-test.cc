/*
 * The MIT License (MIT)
 * 
 * Copyright (c) 2013 Taketoshi Aono(brn)
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include <gtest/gtest.h>
#include <stdint.h>
#include <vector>
#include "../../src/memory/aligned-heap-allocator.h"

class Test1 {
 public:
  size_t a;
  size_t b;
  size_t c;
};

class Test2: public Test1 {
 public:
  size_t d;
  size_t e;
  size_t f;
};

TEST(AlignedHeapAllocator, test) {
  yatsc::Byte* addr = reinterpret_cast<yatsc::Byte*>(yatsc::AlignedHeapAllocator::Allocate(32 KB, 64 KB));
  Test1* i = new(addr) Test1();
  Test2* i2 = new(addr + sizeof(Test1)) Test2();
  Test2* i3 = new(addr + sizeof(Test1) + sizeof(Test1)) Test2();
  i->c = 10;
  i2->f = 10;
  
  ASSERT_EQ(i->c, 10);
  ASSERT_EQ(i2->f, 10);
  printf("%p %p\n", addr, addr + 64 KB);
  printf("%p %p\n", reinterpret_cast<yatsc::Byte*>(reinterpret_cast<uintptr_t>(i2) & ~(128 KB)), i2);
  printf("%p %p\n", reinterpret_cast<yatsc::Byte*>(reinterpret_cast<uintptr_t>(addr + 63 KB) & ~0xFFFF), addr + 63 KB);
}



TEST(AlignedHeapAllocator, test_loop) {
  static const int kSize = 1000000;
  std::vector<Test1*> v;
  v.reserve(kSize);
  // for (int i = 0; i < kSize; i++) {
  //   yatsc::Byte* addr = reinterpret_cast<yatsc::Byte*>(yatsc::AlignedHeapAllocator::Allocate(32 KB, 1 MB));
  //   v.push_back(new (addr) Test2());
  // }
}
