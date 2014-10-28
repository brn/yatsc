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


Channel::Channel(int limit)
    : exit_(false),
      thread_pool_count_(limit || 1) {Initialize();}


Channel::~Channel() {
  exit_ = true;
  while (thread_pool_count_.running_thread_count()){}
  while (thread_pool_count_.current_thread_count()){}
}


inline void Channel::Initialize() {
  for (int i = 0; i < thread_pool_count_.current_thread_count(); i++) {
    CreateThreadPool(i, false);
  }
}


inline void Channel::CreateThreadPool(int i, bool additional) {
  ThreadHandle thread_handle = Heap::NewHandle<std::thread>(std::bind(&Channel::Run, this, i, additional));
  thread_pools_.push_back(thread_handle);
}


inline void Channel::Run(int id, bool additional) {
  printf("Thread %d begin\n", id);
  while (!exit_) {
    if (thread_pool_queue_.empty()) {
      std::this_thread::sleep_for(std::chrono::milliseconds(100));
      continue;
    }
    ThreadPoolQueue::Request fn = thread_pool_queue_.pop_request();
    ProcessRequest(fn, id);
  }
  thread_pool_count_.sub_thread_count();
  printf("Thread %d exit\n", id);
}


inline void Channel::ProcessRequest(const ThreadPoolQueue::Request &fn, int id) {
  if (fn) {
    thread_pool_count_.add_running_thread_count();
    fn(id);
    thread_pool_count_.sub_running_thread_count();
  }
}

}
