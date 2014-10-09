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

#include <random>
#include <thread>
#include <memory>
#include "../../gtest-header.h"
#include "../../signal-wait.h"
#include "../../../src/memory/heap-allocator/heap-allocator.h"
#include "../../../src/utils/systeminfo.h"
#include "../../../src/utils/utils.h"

namespace {
size_t thread_size = yatsc::SystemInfo::GetOnlineProcessorCount();
static const uint64_t kSize = 1000000u;
static const uint64_t kSmallSize = 10000u;
static const size_t kThreadSize = thread_size - 1 == 0? 1 : thread_size - 1;
static const int kThreadObjectSize = 100000;
static const int kStackSize = kThreadObjectSize * kThreadSize;
}

#define LOOP_FOR_THREAD_SIZE for (unsigned i = 0; i < kThreadSize; i++)
#define BUSY_WAIT(counter) while (counter != kThreadSize)


class Heap: public ::testing::Test {
 public:
  static void SetUpTestCase() {
    yatsc::Printf("[TEST CONFIG] DefaultObjectCreationCount %llu.\n", kSize);
    yatsc::Printf("[TEST CONFIG] DefaultThreadCount %d.\n", kThreadSize);
  }
};


class TestClass {
 public:
  size_t a;
  size_t b;
  size_t c;
  size_t d;
};


class Base {
 public:
  Base():x(1){}
  virtual ~Base(){}
  int x;
};


class Test0 {
 public:
  Test0(uint64_t* ok):ok(ok){}
  ~Test0() {(*ok)++;}
  uint64_t* ok;
};

template <typename T = uint64_t>
class Test1 : public Base{
 public:
  Test1(T* ok):Base(),ok(ok){}
  virtual ~Test1() {(*ok)++;}
  T* ok;
};


template <typename T = uint64_t>
class Test1D: public Test1<T> {
 public:
  Test1D(T* ok):Test1<T>(ok){}
};

template <typename T = uint64_t>
class Test2 : public Base {
 public:
  Test2(T* ok):Base(),ok(ok){}
  ~Test2() {(*ok)++;}
  T* ok;
  uint64_t padding1 YATSC_UNUSED;
  uint64_t padding2 YATSC_UNUSED;
};

template <typename T = uint64_t>
class Test3 : public Base {
 public:
  Test3(T* ok):Base(),ok(ok){}
  ~Test3() {(*ok)++;}
  T* ok;
  uint64_t padding1 YATSC_UNUSED;
  uint64_t padding2 YATSC_UNUSED;
  uint64_t padding3 YATSC_UNUSED;
  uint64_t padding4 YATSC_UNUSED;
};


class LargeObject : public Base {
 public:
  LargeObject(uint64_t* ok):Base(),ok(ok){}
  ~LargeObject() {(*ok)++;}
 private:
  uint64_t* ok;
  char padding[20000] YATSC_UNUSED;
};


class Deletable : public Base {
 public:
  Deletable(uint64_t* ok):Base(),ok(ok){}
  ~Deletable() = default;
  void Destruct() {(*ok)++;}
 private:
  uint64_t* ok;
};


TEST_F(Heap, New) {
  uint64_t ok = 0u;
  volatile Test1<uint64_t>* a;
  for (size_t i = 0u; i < kSize; i++) {
    volatile Test1<uint64_t>* v = yatsc::Heap::New<Test1<uint64_t>>(&ok);
    a = v;
    yatsc::Heap::Destruct(v);
  }
  ASSERT_EQ(ok, kSize);
}


TEST_F(Heap, NewPtr) {
  uint64_t ok = 0u;
  for (size_t i = 0u; i < kSize; i++) {
    auto a = new (yatsc::Heap::NewPtr(sizeof(Test1<uint64_t>))) Test1<uint64_t>(&ok);
    yatsc::Heap::Destruct(a);
  }
  ASSERT_EQ(ok, kSize);
}


TEST_F(Heap, NewHandle) {
  uint64_t ok = 0u;
  for (size_t i = 0u; i < kSize; i++) {
    yatsc::Handle<Test1<uint64_t>> a = yatsc::Heap::NewHandle<Test1D<uint64_t>>(&ok);
  }
  ASSERT_EQ(ok, kSize);
}


TEST_F(Heap, Allocate_loop) {
  volatile TestClass* v;
  for (uint64_t i = 0; i < kSize;i++) {
    volatile TestClass* x = yatsc::Heap::New<TestClass>();
    x->a = 1;
    x->b = 2;
    x->c = 3;
    x->d = 4;
    v = x;
  }
}


TEST_F(Heap, Allocate_loop_new) {
  volatile TestClass* v;
  for (uint64_t i = 0; i < kSize;i++) {
    volatile TestClass* x = new TestClass();
    x->a = 1;
    x->b = 2;
    x->c = 3;
    x->d = 4;
    v = x;
  }
}


TEST_F(Heap, random_New) {
  uint64_t ok = 0u;
  std::random_device rd;
	std::mt19937 mt(rd());
	std::uniform_int_distribution<size_t> size(1, 100);
  std::vector<Base*> v;
  v.reserve(kSize);
  for (uint64_t i = 0u; i < kSize; i++) {
    int s = size(mt);
    int t = s % 3 == 0;
    int f = s % 5 == 0;
    Base* ptr;
    if (t) {
      ptr = yatsc::Heap::New<Test1<>>(&ok);
    } else if (f) {
      ptr = yatsc::Heap::New<Test2<>>(&ok);
    } else {
      ptr = yatsc::Heap::New<Test3<>>(&ok);
    }
    ptr->x = 100;
    v.push_back(ptr);
  }
  size_t s = 0;
  for (auto x: v) {
    yatsc::Heap::Destruct(x);
  }
  ASSERT_EQ(kSize, ok);
}


TEST_F(Heap, random_new_) {
  uint64_t ok = 0u;
  std::random_device rd;
	std::mt19937 mt(rd());
	std::uniform_int_distribution<size_t> size(1, 100);
  std::vector<Base*> v;
  v.reserve(kSize);
  Base* ptr;
  for (uint64_t i = 0u; i < kSize; i++) {
    int s = size(mt);
    int t = s % 3 == 0;
    int f = s % 5 == 0;
    if (t) {
      ptr = new Test1<>(&ok);
    } else if (f) {
      ptr = new Test2<>(&ok);
    } else {
      ptr = new Test3<>(&ok);
    }
    v.push_back(ptr);
  }
  for (auto x: v) {
    delete x;
  }
  
  ASSERT_EQ(kSize, ok);
}


TEST_F(Heap, Random_Big) {
  uint64_t ok = 0u;
  std::vector<Base*, yatsc::StandardAllocator<Base*>> v;
  for (uint64_t i = 0u; i < kSmallSize; i++) {
    v.push_back(new LargeObject(&ok));
  }
  for (auto x: v) {
    delete x;
  }
  
  ASSERT_EQ(kSmallSize, ok);
}


TEST_F(Heap, Random_Big_normal) {
  uint64_t ok = 0u;
  std::vector<Base*> v;
  for (uint64_t i = 0u; i < kSmallSize; i++) {
    v.push_back(new LargeObject(&ok));
  }
  for (auto x: v) {
    delete x;
  }
  
  ASSERT_EQ(kSmallSize, ok);
}


TEST_F(Heap, New_many_from_chunk_random_and_dealloc) {
  uint64_t ok = 0u;
  std::random_device rd;
	std::mt19937 mt(rd());
	std::uniform_int_distribution<size_t> size(1, 100);
  Base* last = nullptr;
  int count = 0;
  for (uint64_t i = 0u; i < kSize; i++) {
    int s = size(mt);
    int ss = s % 6 == 0;
    int t = s % 3 == 0;
    int f = s % 5 == 0;

    if (ss) {
      if (last != nullptr) {
        count++;
        yatsc::Heap::Destruct(last);
      }
    }
    
    if (t) {
      last = yatsc::Heap::New<Test1<>>(&ok);
    } else if (f) {
      last = yatsc::Heap::New<Test2<>>(&ok);
    } else {
      last = yatsc::Heap::New<Test3<>>(&ok);
    }
  }
  ASSERT_EQ(count, ok);
}


TEST_F(Heap, New_allocate_big_object) {
  uint64_t ok = 0u;
  auto l = yatsc::Heap::New<LargeObject>(&ok);
  yatsc::Heap::Destruct(l);
  ASSERT_EQ(ok, 1u);
}


TEST_F(Heap, New_allocate_many_big_object) {
  static const uint64_t kSize = 10000;
  uint64_t ok = 0u;
  for (uint64_t i = 0u; i < kSize; i++) {
    yatsc::Heap::New<LargeObject>(&ok);
  }
}


TEST_F(Heap, New_allocate_many_big_object_and_dealloc) {
  static const uint64_t kSize = 10000;
  uint64_t ok = 0u;
  for (uint64_t i = 0u; i < kSize; i++) {
    auto t = yatsc::Heap::New<LargeObject>(&ok);
    yatsc::Heap::Destruct(t);
  }
  
  ASSERT_EQ(kSize, ok);
}



TEST_F(Heap, New_thread_random_dealloc) {
  std::atomic<uint64_t> ok(0u);
  std::atomic<unsigned> index(0);
  std::atomic<bool> wait(true);
  std::atomic_int dc;
  dc.store(0);
  auto fn = [&]() {
    while (wait.load()) {}
    std::random_device rd;
    std::mt19937 mt(rd());
    std::uniform_int_distribution<size_t> size(1, 100);
    volatile Base* last = nullptr;
    for (uint64_t i = 0u; i < kThreadObjectSize; i++) {
      int s = size(mt);
      int ss = s % 6 == 0;
      int t = s % 3 == 0;
      int f = s % 5 == 0;

      if (ss) {
        if (last != nullptr) {
          dc++;
          yatsc::Heap::Destruct(last);
        }
      }
    
      if (t) {
        last = yatsc::Heap::New<Test1<std::atomic<uint64_t>>>(&ok);
      } else if (f) {
        last = yatsc::Heap::New<Test2<std::atomic<uint64_t>>>(&ok);
      } else {
        last = yatsc::Heap::New<Test3<std::atomic<uint64_t>>>(&ok);
      }
    }
    index++;
  };
  
  std::vector<std::thread*> threads;
  LOOP_FOR_THREAD_SIZE {
    auto th = new std::thread(fn);
    threads.push_back(th);
  }
  LOOP_FOR_THREAD_SIZE {
    threads[i]->detach();
    delete threads[i];
  }
  wait.store(false);

  BUSY_WAIT(index) {}
  
  ASSERT_EQ(dc.load(), ok);
}

