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

#ifndef UTILS_SYSTEMINFO_POSIX_INL_H_
#define UTILS_SYSTEMINFO_POSIX_INL_H_

#include <unistd.h>


namespace yatsc {

class SystemInfoPlatform {
 public:
  SystemInfoPlatform() {
    Initialize();
  }


  void Initialize() {
    proc_count_ = sysconf(_SC_NPROCESSORS_ONLN);
    page_size_ = sysconf(_SC_PAGESIZE);
    if (proc_count_ == -1) {
      proc_count_ = 1;
    }
    if (page_size_  == -1) {
      page_size_ = 4 KB;
    }
  }


  long GetOnlineProcessorCount() YATSC_NO_SE {
    return proc_count_;
  }


  long GetPageSize() YATSC_NO_SE {
    return page_size_;
  }

 private:
  long proc_count_;
  long page_size_;
};

}

#endif
