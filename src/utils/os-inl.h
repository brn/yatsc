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


#include <stdarg.h>
#include <stdlib.h>
#include <assert.h>
#include <string>
#ifdef _WIN32
#include <sys/utime.h>
#include <windows.h>
#else
#include <utime.h>
#endif

namespace yatsc {

#ifdef _WIN32

inline int VAArgs_(const char* format, va_list args) {
  return _vscprintf(format, args) + 1;
}

template <typename T, typename Traits, typename Allocator>
void Strerror(std::basic_string<T, std::char_traits<Traits>, Allocator>* buf, int err) {
  char buffer[95];
  if (strerror_s(buffer, err) == 0) {
    buf->assign(buffer);
  } else {
    buf->assign("Strerror error.");
  }
}


template <typename T, typename Traits, typename Allocator>
void SPrintf(std::basic_string<T, std::char_traits<Traits>, Allocator>& buf, bool append, const char* format, ...) {
  va_list args;
  va_start(args, format);
  int len = VAArgs_(format, args);
  char* buffer = static_cast<char*>(malloc(len * sizeof(char)));
  assert(buffer != NULL);
  vsprintf_s(buffer, len, format, args);
  if (!append) {
    buf.assign(buffer);
  } else {
    buf.append(buffer);
  }
  free(buffer);
  va_end(args);
}


template <typename T, typename Traits, typename Allocator>
void VSPrintf(std::basic_string<T, std::char_traits<Traits>, Allocator>& buf, bool append, const char* format, va_list args) {
  int len = VAArgs_(format, args);
  char* buffer = static_cast<char*>(malloc((len + 10) * sizeof(char)));
  assert(buffer != NULL);
  vsprintf_s(buffer, len + 10, format, args);
  if (!append) {
    buf.assign(buffer);
  } else {
    buf.append(buffer);
  }
  free(buffer);
}


template <typename T, typename Traits, typename Allocator>
void GetEnv(std::basic_string<T, std::char_traits<Traits>, Allocator> *buf, const char* env) {
  size_t size = 0;
  char buffer[1];
  getenv_s(&size, buffer, 1, env);
  char* tmp = new char[size + 1];
  getenv_s(&size, tmp, size, env);
  buf->assign(tmp);
  delete[] tmp;
}


template <typename T, typename Traits, typename Allocator>
int Asctime(std::basic_string<T, std::char_traits<Traits>, Allocator>* buf, tm* tm) {
  char buffer[27];
  int ret = asctime_s(buffer, tm);
  buf->assign(buffer);
  return ret;
}


template <typename T, typename Traits, typename Allocator>
void GetLastError(std::basic_string<T, std::char_traits<Traits>, Allocator>* buf) {
  LPVOID msg_buffer;
  FormatMessage(
      FORMAT_MESSAGE_ALLOCATE_BUFFER | 
      FORMAT_MESSAGE_FROM_SYSTEM | 
      FORMAT_MESSAGE_IGNORE_INSERTS,
      NULL, ::GetLastError(),
      MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), 
      (LPTSTR)&msg_buffer, 0, NULL);
  buf->assign(reinterpret_cast<const char*>(msg_buffer));
  LocalFree(msg_buffer);
}



#else

template <typename T, typename Traits, typename Allocator>
void Strerror(std::basic_string<T, std::char_traits<Traits>, Allocator>* buf, int err) {
  char buffer[95];
  strerror_r(err, buffer, 95);
  buf->assign(buffer);
}


template <typename T, typename Traits, typename Allocator>
void SPrintf(std::basic_string<T, std::char_traits<Traits>, Allocator>& buffer, bool append, const char* format, ...) {
  va_list args;
  va_start(args, format);
  char* buf = NULL;
  vasprintf(&buf, format, args);
  if (!append) {
    buffer.assign(buf);
  } else {
    buffer.append(buf);
  }
  free(buf);
}


template <typename T, typename Traits, typename Allocator>
void VSPrintf(std::basic_string<T, std::char_traits<Traits>, Allocator>& buffer, bool append, const char* format, va_list args) {
  char* buf = NULL;
  vasprintf(&buf, format, args);
  if (!append) {
    buffer.assign(buf);
  } else {
    buffer.append(buf);
  }
  free(buf);
}


template <typename T, typename Traits, typename Allocator>
void GetEnv(std::basic_string<T, std::char_traits<Traits>, Allocator> *buf, const char* env) {
  char* ret = getenv(env);
  if (ret != NULL) {
    buf->assign(ret);
    free(ret);
  }
}


template <typename T, typename Traits, typename Allocator>
int Asctime(std::basic_string<T, std::char_traits<Traits>, Allocator>* buf, tm* tm) {
  char buffer[27];
  asctime_r(tm, buffer);
  buf->assign(buffer);
  return 0;
}


template <typename T, typename Traits, typename Allocator>
void GetLastError(std::basic_string<T, std::char_traits<Traits>, Allocator>* buf) {
  Strerror(buf, K_ERRNO);
}

#endif
}
