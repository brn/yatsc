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


#ifndef COMPILER_THREAD_POOL_QUEUE_H
#define COMPILER_THREAD_POOL_QUEUE_H

#include <deque>
#include <functional>
#include <condition_variable>
#include <mutex>
#include <thread>
#include "../utils/utils.h"

namespace yatsc {

class ThreadPoolQueue {
 public :
  typedef std::function<void()> Request;

  
  ThreadPoolQueue();

  
  ~ThreadPoolQueue();

  
  template <typename T>
  void set_request(T request) {
    std::unique_lock<std::mutex> lock(lock_);
    set_wait_.wait(lock, [&] {return !pop_;});
    Request fn = request;
    queue_.push_back(fn);
    pop_wait_.notify_one();
  }

  
  Request pop_request();

  
  bool empty() const {return queue_.empty();}


  size_t job_count() YATSC_NO_SE {return queue_.size();}

  
 private :
  typedef std::deque<Request> Queue;
  Queue queue_;
  bool pop_;
  std::mutex lock_;
  std::condition_variable set_wait_;
  std::condition_variable pop_wait_;
};

}

#endif
