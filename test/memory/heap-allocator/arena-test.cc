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
#include "../../../src/memory/heap-allocator/arena.h"


class TestClass {
 public:
  size_t a;
  size_t b;
  size_t c;
  size_t d;
};


TEST(CentralArena, Allocate) {
  yatsc::heap::CentralArena arena;
  auto x = new (arena.Allocate(sizeof(TestClass))) TestClass();
  x->a = 1;
  x->b = 2;
  x->c = 3;
  x->d = 4;
}


TEST(CentralArena, Allocate_loop) {
  yatsc::heap::CentralArena arena;
  volatile TestClass* v;
  for (int i = 0; i < 1000000;i++) {
    volatile TestClass* x = new (arena.Allocate(sizeof(TestClass))) TestClass();
    x->a = 1;
    x->b = 2;
    x->c = 3;
    x->d = 4;
    v = x;
  }
}


TEST(CentralArena, Allocate_loop_new) {
  //yatsc::heap::CentralArena arena;
  volatile TestClass* v;
  for (int i = 0; i < 1000000;i++) {
    volatile TestClass* x = new TestClass();
    x->a = 1;
    x->b = 2;
    x->c = 3;
    x->d = 4;
    v = x;
  }
}


TEST(CentralArena, Allocate_loop_delete) {
  yatsc::heap::CentralArena arena;
  volatile TestClass* v;
  for (int i = 0; i < 1000000;i++) {
    volatile TestClass* x = new (arena.Allocate(sizeof(TestClass))) TestClass();
    x->a = 1;
    x->b = 2;
    x->c = 3;
    x->d = 4;
    v = x;
    arena.Dealloc(x);
  }
}


TEST(CentralArena, Allocate_loop_new_delete) {
  //yatsc::heap::CentralArena arena;
  volatile TestClass* v;
  for (int i = 0; i < 1000000;i++) {
    volatile TestClass* x = new TestClass();
    x->a = 1;
    x->b = 2;
    x->c = 3;
    x->d = 4;
    v = x;
    delete x;
  }
}

