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

#include "../gtest-header.h"
#include <random>
#include <thread>
#include <memory>
#include "../../src/utils/regions.h"
#include "../../src/utils/systeminfo.h"
#include "../../src/utils/utils.h"

namespace {
size_t thread_size = yatsc::SystemInfo::GetOnlineProcessorCount();
static const uint64_t kSize = 1000000u;
static const size_t kThreadSize = thread_size - 1 == 0? 1 : thread_size - 1;
static const int kThreadObjectSize = 100000;
static const int kStackSize = kThreadObjectSize * kThreadSize;
}

#define LOOP_FOR_THREAD_SIZE for (unsigned i = 0; i < kThreadSize; i++)
#define BUSY_WAIT(counter) while (counter != kThreadSize)


class RegionsTest: public ::testing::Test {
 public:
  static void SetUpTestCase() {
    yatsc::Printf("[TEST CONFIG] DefaultObjectCreationCount %llu.\n", kSize);
    yatsc::Printf("[TEST CONFIG] DefaultThreadCount %d.\n", kThreadSize);
  }
};


class Test0 {
 public:
  Test0(uint64_t* ok):ok(ok){}
  ~Test0() {(*ok)++;}
  uint64_t* ok;
};

template <typename T = uint64_t>
class Test1 : public yatsc::RegionalObject {
 public:
  Test1(T* ok):yatsc::RegionalObject(),ok(ok){}
  ~Test1() {(*ok)++;}
  T* ok;
};

template <typename T = uint64_t>
class Test2 : public yatsc::RegionalObject  {
 public:
  Test2(T* ok):yatsc::RegionalObject(),ok(ok){}
  ~Test2() {(*ok)++;}
  T* ok;
  uint64_t padding1 YATSC_UNUSED;
  uint64_t padding2 YATSC_UNUSED;
};

template <typename T = uint64_t>
class Test3 : public yatsc::RegionalObject  {
 public:
  Test3(T* ok):yatsc::RegionalObject(),ok(ok){}
  ~Test3() {(*ok)++;}
  T* ok;
  uint64_t padding1 YATSC_UNUSED;
  uint64_t padding2 YATSC_UNUSED;
  uint64_t padding3 YATSC_UNUSED;
  uint64_t padding4 YATSC_UNUSED;
};


class LargeObject : public yatsc::RegionalObject  {
 public:
  LargeObject(uint64_t* ok):yatsc::RegionalObject(),ok(ok){}
  ~LargeObject() {(*ok)++;}
 private:
  uint64_t* ok;
  char padding[20000] YATSC_UNUSED;
};


class Deletable : public yatsc::RegionalObject  {
 public:
  Deletable(uint64_t* ok):yatsc::RegionalObject(),ok(ok){}
  ~Deletable() = default;
  void Destruct() {(*ok)++;}
 private:
  uint64_t* ok;
};


class Array : public yatsc::RegionalObject {
 public:
  uint64_t* ok;
  ~Array() {(*ok)++;}
};


TEST_F(RegionsTest, RegionsTest_allocate_from_chunk) {
  uint64_t ok = 0u;
  for (size_t i = 0u; i < kSize; i++) {
    auto a = new Test1<uint64_t>(&ok);
    delete a;
  }
  //ASSERT_EQ(ok, 1u);
}


// TEST_F(RegionsTest, RegionsTest_allocate_from_chunk_array) {
//   uint64_t ok = 0u;
//   yatsc::Regions p(1024);
//   p.NewArray<Test1<>>(kSize, &ok);
//   p.Destroy();
//   ASSERT_EQ(kSize, ok);
// }


// TEST_F(RegionsTest, RegionsTest_allocate_many_from_chunk) {
//   yatsc::Regions p(1024);
//   uint64_t ok = 0u;
//   for (uint64_t i = 0u; i < kSize; i++) {
//     p.New<Test1<>>(&ok);
//   }
//   p.Destroy();
//   ASSERT_EQ(kSize, ok);
// }


// TEST_F(RegionsTest, RegionsTest_allocate_many_from_chunk_random) {
//   uint64_t ok = 0u;
//   std::random_device rd;
// 	std::mt19937 mt(rd());
// 	std::uniform_int_distribution<size_t> size(1, 100);
//   yatsc::Regions p(1024);
//   for (uint64_t i = 0u; i < kSize; i++) {
//     int s = size(mt);
//     int t = s % 3 == 0;
//     int f = s % 5 == 0;
//     if (t) {
//       p.New<Test1<>>(&ok);
//     } else if (f) {
//       p.New<Test2<>>(&ok);
//     } else {
//       p.New<Test3<>>(&ok);
//     }
//   }
//   p.Destroy();
//   ASSERT_EQ(kSize, ok);
// }


// TEST_F(RegionsTest, RegionsTest_allocate_many_from_chunk_and_dealloc) {
//   uint64_t ok = 0u;
//   yatsc::Regions p(1024);
//   for (uint64_t i = 0u; i < kSize; i++) {
//     Test1<>* t = p.New<Test1<>>(&ok);
//     p.Dealloc(t);
//   }
//   p.Destroy();
//   ASSERT_EQ(kSize, ok);
// }


// TEST_F(RegionsTest, RegionsTest_allocate_many_from_chunk_random_and_dealloc) {
//   uint64_t ok = 0u;
//   std::random_device rd;
// 	std::mt19937 mt(rd());
// 	std::uniform_int_distribution<size_t> size(1, 100);
//   yatsc::Regions p(1024);
//   void* last = nullptr;
//   for (uint64_t i = 0u; i < kSize; i++) {
//     int s = size(mt);
//     int ss = s % 6 == 0;
//     int t = s % 3 == 0;
//     int f = s % 5 == 0;

//     if (ss) {
//       if (last != nullptr) {
//         p.Dealloc(last);
//       }
//     }
    
//     if (t) {
//       last = p.New<Test1<>>(&ok);
//     } else if (f) {
//       last = p.New<Test2<>>(&ok);
//     } else {
//       last = p.New<Test3<>>(&ok);
//     }
//   }
//   p.Destroy();
//   ASSERT_EQ(kSize, ok);
// }


// TEST_F(RegionsTest, RegionsTest_allocate_big_object) {
//   uint64_t ok = 0u;
//   yatsc::Regions p(8);
//   p.New<LargeObject>(&ok);
//   p.Destroy();
//   ASSERT_EQ(ok, 1u);
// }


// TEST_F(RegionsTest, RegionsTest_allocate_many_big_object) {
//   static const uint64_t kSize = 10000;
//   uint64_t ok = 0u;
//   yatsc::Regions p(8);
//   for (uint64_t i = 0u; i < kSize; i++) {
//     p.New<LargeObject>(&ok);
//   }
//   p.Destroy();
//   ASSERT_EQ(kSize, ok);
// }


// TEST_F(RegionsTest, RegionsTest_allocate_many_big_object_and_dealloc) {
//   static const uint64_t kSize = 10000;
//   uint64_t ok = 0u;
//   yatsc::Regions p(8);
//   for (uint64_t i = 0u; i < kSize; i++) {
//     auto t = p.New<LargeObject>(&ok);
//     p.Dealloc(t);
//   }
//   p.Destroy();
//   ASSERT_EQ(kSize, ok);
// }



// TEST_F(RegionsTest, RegionsTest_performance1) {
//   yatsc::Regions p(1024);
//   uint64_t ok = 0u;
//   for (uint64_t i = 0u; i < kSize; i++) {
//     p.New<Test1<>>(&ok);
//   }
//   p.Destroy();
//   ASSERT_EQ(kSize, ok);
// }


// TEST_F(RegionsTest, RegionsTest_performance2) {
//   uint64_t ok = 0u;
//   {
//     std::vector<std::shared_ptr<Test0>> list(kSize);
//     for (uint64_t i = 0u; i < kSize; i++) {
//       list[i] = std::make_shared<Test0>(&ok);
//     }
//   }
//   ASSERT_EQ(kSize, ok);
// }


// TEST_F(RegionsTest, RegionsTest_performance3) {
//   uint64_t ok = 0u;
//   std::vector<Test0*> stack(kSize);
//   for (uint64_t i = 0u; i < kSize; i++) {
//     stack[i] = new Test0(&ok);
//   }
//   for (uint64_t i = 0u; i < kSize; i++) {
//     delete stack[i];
//   }
//   ASSERT_EQ(kSize, ok);
// }


// TEST_F(RegionsTest, RegionsTest_thread) {
//   uint64_t ok = 0u;
//   yatsc::Regions p(1024);
//   std::atomic<unsigned> index(0);
  
//   auto fn = [&]() {
//     for (uint64_t i = 0u; i < kThreadObjectSize; i++) {
//       p.New<Test1<>>(&ok);
//     }
//     index++;
//   };
  
//   std::vector<std::thread*> threads;
//   LOOP_FOR_THREAD_SIZE {
//     auto th = new std::thread(fn);
//     threads.push_back(th);
//   }
//   LOOP_FOR_THREAD_SIZE {
//     threads[i]->detach();
//     delete threads[i];
//   }

//   BUSY_WAIT(index) {}
  
//   p.Destroy();
//   ASSERT_EQ(kThreadObjectSize * kThreadSize, ok);
// }


// TEST_F(RegionsTest, RegionsTest_thread_new) {
//   uint64_t ok = 0u;
//   std::vector<Test0*> stack(kStackSize);
//   std::atomic<unsigned> index(0);
//   auto fn = [&](int id) {
//     int current = kThreadObjectSize * id;
//     for (uint64_t i = 0u; i < kThreadObjectSize; i++) {
//       stack[current + i] = new Test0(&ok);
//     }
//     index++;
//   };
  
//   std::vector<std::thread*> threads;
//   LOOP_FOR_THREAD_SIZE {
//     auto th = new std::thread(fn, i);
//     threads.push_back(th);
//   }
//   LOOP_FOR_THREAD_SIZE {
//     threads[i]->detach();
//     delete threads[i];
//   }

//   BUSY_WAIT(index) {}

//   for (int i = 0; i < kStackSize; i++) {
//     delete stack[i];
//   }
  
//   ASSERT_EQ(kStackSize, ok);
// }


// TEST_F(RegionsTest, RegionsTest_thread_random) {
//   uint64_t ok = 0u;
//   yatsc::Regions p(1024);
//   std::atomic<unsigned> index(0);
//   auto fn = [&]() {
//     std::random_device rd;
//     std::mt19937 mt(rd());
//     std::uniform_int_distribution<size_t> size(1, 100);
//     for (uint64_t i = 0u; i < kThreadObjectSize; i++) {
//       int s = size(mt);
//       int t = s % 3 == 0;
//       int f = s % 5 == 0;
//       if (t) {
//         p.New<Test1<>>(&ok);
//       } else if (f) {
//         p.New<Test2<>>(&ok);
//       } else {
//         p.New<Test3<>>(&ok);
//       }
//     }
//     index++;
//   };
  
//   std::vector<std::thread*> threads;
//   LOOP_FOR_THREAD_SIZE {
//     auto th = new std::thread(fn);
//     threads.push_back(th);
//   }
//   LOOP_FOR_THREAD_SIZE {
//     threads[i]->detach();
//     delete threads[i];
//   }

//   BUSY_WAIT(index) {}
  
//   p.Destroy();
//   ASSERT_EQ(kThreadObjectSize * kThreadSize, ok);
// }


// TEST_F(RegionsTest, RegionsTest_thread_random_dealloc) {
//   std::atomic<uint64_t> ok(0u);
//   yatsc::Regions p(1024);
//   std::atomic<unsigned> index(0);
//   std::atomic<bool> wait(true);
//   auto fn = [&]() {
//     while (wait.load()) {}
//     std::random_device rd;
//     std::mt19937 mt(rd());
//     std::uniform_int_distribution<size_t> size(1, 100);
//     void* last = nullptr;
//     int dc =0;
//     for (uint64_t i = 0u; i < kThreadObjectSize; i++) {
//       int s = size(mt);
//       int ss = s % 6 == 0;
//       int t = s % 3 == 0;
//       int f = s % 5 == 0;

//       if (ss) {
//         if (last != nullptr) {
//           dc++;
//           p.Dealloc(last);
//         }
//       }
    
//       if (t) {
//         last = p.New<Test1<std::atomic<uint64_t>>>(&ok);
//       } else if (f) {
//         last = p.New<Test2<std::atomic<uint64_t>>>(&ok);
//       } else {
//         last = p.New<Test3<std::atomic<uint64_t>>>(&ok);
//       }
//     }
//     index++;
//   };
  
//   std::vector<std::thread*> threads;
//   LOOP_FOR_THREAD_SIZE {
//     auto th = new std::thread(fn);
//     threads.push_back(th);
//   }
//   LOOP_FOR_THREAD_SIZE {
//     threads[i]->detach();
//     delete threads[i];
//   }
//   wait.store(false);

//   BUSY_WAIT(index) {}
//   p.Destroy();
//   ASSERT_EQ(kStackSize, ok);
// }
