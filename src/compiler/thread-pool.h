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


#ifndef COMPILER_THREAD_POOL_H
#define COMPILER_THREAD_POOL_H

#include <atomic>
#include <thread>
#include <mutex>
#include <functional>
#include "./channel.h"
#include "../utils/utils.h"
#include "../utils/stl.h"

namespace yatsc {
class ThreadPool {
 public :
  static int default_thread_pool_limit;


  ThreadPool();


  ~ThreadPool(){};


  template <typename T>
  void send_request(T);


  template <typename T>
  void send_requests(Vector<T>);


  YATSC_INLINE int running_thread_count() const {return channel_.running_thread_count();}


  YATSC_INLINE int current_thread_count() const {return channel_.current_thread_count();}


  YATSC_INLINE void Wait() {channel_.Wait();}


  YATSC_INLINE void Shutdown() {channel_.Shutdown();}


 private :

  static void Remove();


  Channel channel_;
};

}
#include "thread-pool-inl.h"
#endif
