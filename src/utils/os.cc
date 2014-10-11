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


#include <stdarg.h>
#include <stdlib.h>
#include <assert.h>
#ifdef _WIN32
#include <time.h>
#include <sys/utime.h>
#include <windows.h>
#else
#include <utime.h>
#endif
#include "os.h"
#include "utils.h"

namespace yatsc {

#ifdef _WIN32

void Printf(const char* format, ...) {
  va_list args;
  va_start(args, format);
  vprintf_s(format, args);
  va_end(args);
}


void VFPrintf(FILE* fp, const char* format, va_list arg) {
  vfprintf_s(fp, format, arg);
}


void FPrintf(FILE* fp, const char* format, ...) {
  va_list args;
  va_start(args, format);
  vfprintf_s(fp, format, args);
  va_end(args);
}


FILE* FOpen(const char* filename, const char* mode) {
  FILE *fp = _fsopen(filename, mode, _SH_DENYNO);
  if (fp == NULL) {
    std::string buf;
    Strerror(&buf, _doserrno);
    throw std::move(FileIOException(buf.c_str()));
  }
  return fp;
}

size_t FRead(void* buffer, size_t buffer_size, size_t element_size, size_t count, FILE* fp) {
  return fread_s(buffer ,buffer_size, element_size, count, fp);
}

void FClose(FILE* fp) {
  fclose(fp);
}


bool Sleep(int nano_time) {
  ::Sleep(nano_time);
  return true;
}

int Utime(const char* path) {
  K_ERRNO = _utime(path, NULL);
  return K_ERRNO;
}

time_t Time(time_t* t) {
  return ::time(t);
}

int LocalTime(tm* t, time_t* time) {
  return localtime_s(t, time);
}

void AtExit(void(*callback)()) {
  atexit(callback);
}

FILE* POpen(const char* name, const char* mode) {
  return ::_popen(name, mode);
}

void PClose(FILE* fp) {
  ::_pclose(fp);
}

char* Strdup(const char* path) {
  return _strdup(path);
}

void Strcpy(char* dest, const char* src, size_t length) {
  strcpy_s(dest, length, src);
}


#else


void Printf(const char* format, ...) {
  va_list args;
  va_start(args, format);
  vprintf(format, args);
  va_end(args);
}

void VFPrintf(FILE* fp, const char* format, va_list arg) {
  vfprintf(fp, format, arg);
}

void FPrintf(FILE* fp, const char* format, ...) {
  va_list args;
  va_start(args, format);
  vfprintf(fp, format, args);
  va_end(args);
}

FILE* FOpen(const char* filename, const char* mode) {
  FILE* fp = fopen(filename, mode);
  if (fp == NULL) {
    String buf;
    Strerror(&buf, errno);
    throw std::move(FileIOException(buf.c_str()));
  }
  return fp;
}

size_t FRead(void* buffer, size_t buffer_size, size_t element_size, size_t count, FILE* fp) {
  return fread(buffer ,element_size, count, fp);
}

void FClose(FILE* fp) {
  fclose(fp);
}

bool Sleep(int nano_time) {
  struct timespec req;
  struct timespec rem;
  if (nano_time > 1000) {
    req.tv_sec = nano_time / 1000;
    nano_time = (nano_time << 1) / 10;
  } else {
    req.tv_sec = 0;
  }
  req.tv_nsec = nano_time;
  int ret = nanosleep(&req, &rem);
  return ret == 0 || ret == EINTR;
}

int Utime(const char* path) {
  K_ERRNO = utime(path, NULL);
  return K_ERRNO;
}

time_t Time(time_t* time) {
  return ::time(time);
}

int LocalTime(tm* t, time_t* time) {
  localtime_r(time, t);
  return 0;
}

void AtExit(void(*callback)()) {
  atexit(callback);
}

FILE* POpen(const char* name, const char* mode) {
  return popen(name, mode);
}

void PClose(FILE* fp) {
  pclose(fp);
}

char* Strdup(const char* path) {
  return strdup(path);
}

void Strcpy(char* dest, const char* src, size_t length) {
  strcpy(dest, src);
}

#endif
}
