//
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


#ifndef UTILS_OS_H_
#define UTILS_OS_H_

#include <errno.h>
#include <string>
#include <stdexcept>
#include <stdlib.h>
#include "./stl.h"

#ifdef _WIN32
#define K_ERRNO _doserrno
#else
#define K_ERRNO errno
#endif

namespace yatsc {
void Strerror(String* buf, int err);
void Printf(const char* format, ...);
void SPrintf(String& buf, bool append, const char* format, ...);
void VSPrintf(String& buf, bool append, const char* format, va_list args);
void VFPrintf(FILE* fp, const char* format, ...);
void FPrintf(FILE* fp, const char* format, ...);
FILE* FOpen(const char* filename, const char* mode);
size_t FRead(void* buffer, size_t buffer_size, size_t element_size, size_t count, FILE* fp);
void FClose(FILE* fp);
void GetEnv(String *buf, const char* env);
bool Sleep(int nano_time);
int Utime(const char* path);
time_t Time(time_t* time);
int Asctime(String* buf, tm* tm);
int LocalTime(tm* t, time_t* time);
void OnExit(void(*callback)());
void GetLastError(String* buf);
FILE* POpen(const char* name, const char* mode);
void PClose(FILE* fp);
char* Strdup(const char* path);
void Strcpy(char* dest, const char* src, size_t length);


class FileIOException : public std::exception {
 public:
  FileIOException(const char* message)
      : std::exception() {
    message_ = Strdup(message);
  }

  FileIOException(FileIOException&& e) {
    if (message_ != nullptr) {
      free(reinterpret_cast<void*>(message_));
      message_ = nullptr;
    }
    message_ = Strdup(e.message_);
    if (e.message_ != nullptr) {
      free(reinterpret_cast<void*>(e.message_));
      e.message_ = nullptr;
    }
  }


  ~FileIOException() {
    if (message_ != nullptr) {
      free(reinterpret_cast<void*>(message_));
    }
  }
  
  
  const char* what() const throw() {return message_;}

 private:
  char* message_ = nullptr;
};
}

#endif
