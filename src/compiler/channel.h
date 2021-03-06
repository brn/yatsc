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


#ifndef COMPILER_CHANNEL_H
#define COMPILER_CHANNEL_H


#include <deque>
#include <thread>
#include <mutex>
#include <condition_variable>
#include "../memory/heap.h"
#include "./thread-pool-count.h"
#include "./thread-pool-queue.h"


namespace yatsc {
class Channel {
  typedef Handle<std::thread> ThreadHandle;
  typedef std::deque<ThreadHandle> ThreadPools;
 public :
  Channel(int limit);

  ~Channel();

  template <typename T>
  void send_request(T req) {
    thread_pool_queue_.set_request(req);
  }

  YATSC_INLINE int running_thread_count() const {return thread_pool_count_.running_thread_count();}


  YATSC_INLINE int current_thread_count() const {return thread_pool_count_.current_thread_count();}


  void Wait() {
    for (auto thread_pool: thread_pools_) {
      thread_pool->join();
    }
  }


  YATSC_INLINE void Shutdown() YATSC_NOEXCEPT {
    exit_ = true;
  }
  
 private :
  void Initialize();


  void CreateThreadPool(int i, bool additional);


  ThreadPoolQueue::Request Wait(bool);


  void Run(int id, bool additional);


  void ProcessRequest(const ThreadPoolQueue::Request &fn, int id);


  bool exit_;
  ThreadPoolCount thread_pool_count_;
  ThreadPools thread_pools_;
  ThreadPoolQueue thread_pool_queue_;
};

}

#endif
