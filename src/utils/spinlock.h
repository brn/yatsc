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


#ifndef UTILS_SPIN_LOCK_H_
#define UTILS_SPIN_LOCK_H_

#include <atomic>
#include <mutex>
#include "utils.h"

namespace rasp {
class SpinLock {
 public:
  SpinLock() {unlock();}
  ~SpinLock() = default;
  RASP_INLINE void lock() RASP_NOEXCEPT {
    while (lock_.test_and_set(std::memory_order_seq_cst)) {}
  }

  
  RASP_INLINE void unlock() RASP_NOEXCEPT {
    lock_.clear(std::memory_order_seq_cst);
  }

 private:
  std::atomic_flag lock_;
};


typedef std::lock_guard<SpinLock> ScopedSpinLock;
typedef std::unique_lock<SpinLock> UniqueSpinLock;
}

#endif
