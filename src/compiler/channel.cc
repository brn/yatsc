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


#include <functional>
#include "channel.h"
#include "../memory/heap.h"

namespace yatsc {

Channel::Channel()
    : exit_(false),
      worker_count_(12) {Initialize();}


Channel::Channel(int limit)
    : exit_(false),
      worker_count_(limit || 1) {Initialize();}


Channel::~Channel() {
  exit_ = true;
  cond_.notify_all();
  while (worker_count_.running_thread_count()){}
}


inline void Channel::Initialize() {
  for (int i = 0; i < worker_count_.current_thread_count(); i++) {
    CreateWorker(i, false);
  }
}

  
inline WorkerQueue::Request Channel::Wait(bool additional) {
  std::unique_lock<std::mutex> lock(mutex_);
  int count = 0;
  while (worker_queue_.empty() && !exit_) {
    if (additional) {
      if (count > 0) {
        return WorkerQueue::Request();
      }
      count++;
      cond_.wait(lock, [&]{return worker_queue_.empty() && !exit_;});
    } else {
      cond_.wait(lock, [&]{return worker_queue_.empty() && !exit_;});
    }
  }
  return worker_queue_.pop_request();
}


inline void Channel::CreateWorker(int i, bool additional) {
  ThreadHandle thread_handle = Heap::NewHandle<std::thread>(std::bind(&Channel::Run, this, i, additional));
  thread_handle->detach();
  workers_.push_back(thread_handle);
}


inline void Channel::Run(int id, bool additional) {
  while (1) {
    WorkerQueue::Request fn = Wait(additional);
    if (fn) {
      ProcessRequest(fn, id);
    } else {
      worker_count_.sub_thread_count();
      return;
    }
  }
}


inline void Channel::ProcessRequest(const WorkerQueue::Request &fn, int id) {
  worker_count_.add_running_thread_count();
  fn();
  worker_count_.sub_running_thread_count();
}

}
