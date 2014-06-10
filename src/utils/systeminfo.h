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

#ifndef UTILS_SYSTEMINFO_H_
#define UTILS_SYSTEMINFO_H_
#include <atomic>
#include "utils.h"

namespace rasp {

class SystemInfoPlatform;


class SystemInfo : private Static {
 public:
  static size_t GetOnlineProcessorCount();
  static size_t GetPageSize();
  static SystemInfoPlatform* GetPlatform();
 private:
  static void Initialize();
  static SystemInfoPlatform* system_info_platform_;
  static std::atomic<bool> initialized_;
};

}


#if defined(PLATFORM_WIN)
#include "systeminfo-win-inl.h"
#elif defined(PLATFORM_POSIX)
#include "systeminfo-posix-inl.h"
#endif

#endif
