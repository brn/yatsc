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


#include <celero/Celero.h>
#include <random>
#include <thread>
#include <memory>
#include "../../../src/memory/heap-allocator/heap-allocator.h"
#include "../../../src/utils/systeminfo.h"
#include "../../../src/utils/utils.h"

namespace {
size_t thread_size = yatsc::SystemInfo::GetOnlineProcessorCount();
static const uint64_t kSize = 10000u;
static const size_t kThreadSize = thread_size - 1 == 0? 1 : thread_size - 1;
static const int kThreadObjectSize = 2000;
static const int kSamples = 10;
}


#define LOOP_FOR_THREAD_SIZE for (unsigned i = 0; i < kThreadSize; i++)
#define BUSY_WAIT(counter) while (counter != kThreadSize)


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


CELERO_MAIN;


BASELINE(HeapAllocation, Baseline, kSamples, 100) {
  yatsc::SpinLock lock;
  std::vector<Base*> vec;
  vec.reserve(kThreadObjectSize * kThreadSize);
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
    Base* last = nullptr;
    for (uint64_t i = 0u; i < kThreadObjectSize; i++) {
      int s = size(mt);
      int ss = s % 6 == 0;
      int t = s % 3 == 0;
      int f = s % 5 == 0;
    
      if (t) {
        last = new Test1<std::atomic<uint64_t>>(&ok);
      } else if (f) {
        last = new Test2<std::atomic<uint64_t>>(&ok);
      } else {
        last = new Test3<std::atomic<uint64_t>>(&ok);
      }
      lock.lock();
      vec.push_back(last);
      lock.unlock();
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
  for (auto x: vec) {
    delete x;
  }
}


BENCHMARK(HeapAllocation, Heap_New, kSamples, 100) {
  yatsc::SpinLock lock;
  std::vector<Base*> vec;
  vec.reserve(kThreadObjectSize * kThreadSize);
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
    Base* last = nullptr;
    for (uint64_t i = 0u; i < kThreadObjectSize; i++) {
      int s = size(mt);
      int ss = s % 6 == 0;
      int t = s % 3 == 0;
      int f = s % 5 == 0;
    
      if (t) {
        last = yatsc::Heap::New<Test1<std::atomic<uint64_t>>>(&ok);
      } else if (f) {
        last = yatsc::Heap::New<Test2<std::atomic<uint64_t>>>(&ok);
      } else {
        last = yatsc::Heap::New<Test3<std::atomic<uint64_t>>>(&ok);
      }
      lock.lock();
      vec.push_back(last);
      lock.unlock();
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
  for (auto x: vec) {
    yatsc::Heap::Destruct(x);
  }
}


BASELINE(HeapAllocationSingle, Baseline, kSamples, 100) {
  std::vector<Base*> vec;
  vec.reserve(kSize);
  uint64_t ok = 0u;
  volatile Test1<uint64_t>* a;
  for (size_t i = 0u; i < kSize; i++) {
    auto v = new Test1<uint64_t>(&ok);
    a = v;
    vec.push_back(v);
  }
  for (auto x: vec) {
    delete x;
  }
}

static int x = 0;
BENCHMARK(HeapAllocationSingle, New, kSamples, 100) {
  std::vector<Base*> vec;
  vec.reserve(kSize);

  uint64_t ok = 0u;
  volatile Test1<uint64_t>* a;
  for (size_t i = 0u; i < kSize; i++) {
    auto v = yatsc::Heap::New<Test1<uint64_t>>(&ok);
    a = v;
    vec.push_back(v);
  }
  for (auto x: vec) {
    yatsc::Heap::Destruct(x);
  }
  // PROCESS_MEMORY_COUNTERS info;
  // ::GetProcessMemoryInfo(::GetCurrentProcess(),&info,sizeof info);
  // printf("Used: %d MB PageFault: %d\n", info.WorkingSetSize / 1024 / 1024,  info.PageFaultCount);
}
