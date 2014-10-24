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


#include <thread>
#include "./worker-count.h"

namespace yatsc {

static const int thread_default_count = 4;


int GetThreadCount() {
  if (std::thread::hardware_concurrency() >= thread_default_count) {
    return std::thread::hardware_concurrency();
  }
  return thread_default_count;
}


WorkerCount::WorkerCount(int limit)
    : limit_(limit),
      current_thread_count_(GetThreadCount()),
      running_thread_count_(0){}


WorkerCount::~WorkerCount(){}


int WorkerCount::current_thread_count() const {return static_cast<long>(current_thread_count_);}


int WorkerCount::running_thread_count() const {return static_cast<long>(running_thread_count_);}


void WorkerCount::add_thread_count() {++current_thread_count_;}


void WorkerCount::sub_thread_count() {--current_thread_count_;}


void WorkerCount::add_running_thread_count() {++running_thread_count_;}


void WorkerCount::sub_running_thread_count() {--running_thread_count_;}


bool WorkerCount::limit() const {return current_thread_count_ == limit_;}

}
