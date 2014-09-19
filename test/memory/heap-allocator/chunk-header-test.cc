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
#include "../../../src/memory/heap-allocator/chunk-header.h"

class TestClass {
 public:
  size_t a;
  size_t b;
  size_t c;
  size_t d;
};

yatsc::heap::ChunkHeader* header = yatsc::heap::ChunkHeader::New(sizeof(::TestClass));

TEST(ChunkHeaderTest, test) {
  
  for (int i = 0; i < 1000000; i++) {
    ::TestClass* t = new (header->Distribute()) ::TestClass();
    t->a = 1;
    t->b = 2;
    t->c = 3;
    t->d = 4;
    ASSERT_EQ(t->a, 1);
    ASSERT_EQ(t->b, 2);
    ASSERT_EQ(t->c, 3);
    ASSERT_EQ(t->d, 4);
  }
}


TEST(ChunkHeaderTest, test2) {
  //yatsc::heap::ChunkHeader* header = yatsc::heap::ChunkHeader::New(sizeof(::TestClass));
  for (int i = 0; i < 1000000; i++) {
    ::TestClass* t = new ::TestClass();
    t->a = 1;
    t->b = 2;
    t->c = 3;
    t->d = 4;
    ASSERT_EQ(t->a, 1);
    ASSERT_EQ(t->b, 2);
    ASSERT_EQ(t->c, 3);
    ASSERT_EQ(t->d, 4);
  }
}
