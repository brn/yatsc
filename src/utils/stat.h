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


#ifndef UTILS_STAT_H_
#define UTILS_STAT_H_

#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include "utils.h"

#ifdef _WIN32
#define STAT_FN(filename, statObj) ::_stat (filename, statObj)
#else
#define STAT_FN(filename, statObj) ::stat (filename, statObj)
#endif

#ifdef _WIN32
#define CTIME(str,buf) ::_ctime64_s(buf,200,str)
#else
#define CTIME(str,buf) ::ctime_r(str,buf)
#endif

#define MODE (fstat_.st_mode & S_IFMT)


namespace rasp {
class Stat{
 public :
  typedef enum {
    kFifo,
    kChr,
    kDir,
    kBlk,
    kReg,
    kLnk,
    kSock
  } FileType;

  RASP_INLINE Stat(const char* path) {
    is_exist_ = (STAT_FN(path, &fstat_) != -1);
  }


  ~Stat(){};


  RASP_INLINE bool IsExist() const { return is_exist_; }


  RASP_INLINE int Dev() const { return fstat_.st_dev;}


  RASP_INLINE int Ino() const { return fstat_.st_ino; }


  RASP_INLINE int NLink() const { return fstat_.st_nlink; }


  RASP_INLINE int UId() const { return fstat_.st_uid; }


  RASP_INLINE int GId() const { return fstat_.st_gid; };


  RASP_INLINE int RDev() const { return fstat_.st_rdev; };


  RASP_INLINE int Size() const { return fstat_.st_size; };


  RASP_INLINE const char* ATime() {
    CTIME(&(fstat_.st_atime),atime_);
    return atime_;
  }


  RASP_INLINE const char* MTime() {
    CTIME(&(fstat_.st_mtime),mtime_);
    return mtime_;
  }


  RASP_INLINE const char* CTime() {
    CTIME(&(fstat_.st_ctime),ctime_);
    return ctime_;
  }


  RASP_INLINE bool IsDir() { return MODE == S_IFDIR; }


  RASP_INLINE bool IsReg() { return MODE == S_IFREG; }


  RASP_INLINE bool IsChr() { return MODE == S_IFCHR; }


 private :

  bool is_exist_;
  char atime_[200];
  char mtime_[200];
  char ctime_[200];

#ifdef _WIN32
  struct _stat fstat_;
#else
  struct stat fstat_;
#endif
};
}

#undef STAT_FN
#undef CTIME
#undef MODE
#endif //UTILS_STAT_H_
