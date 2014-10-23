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

#include <string.h>
#include <stdlib.h>
#include <vector>
#include <string>
#include <sstream>
#ifdef PLATFORM_WIN32
#include <windows.h>
#elif defined PLATFORM_POSIX
#include <unistd.h>
#include <sys/stat.h>
#include <errno.h>
#include <stdio.h>
#include <sys/types.h>
#include <dirent.h>
#endif
#include "../config.h"
#include "./utils.h"
#include "./stat.h"
#include "./path.h"
#include "./os.h"


#ifdef _WIN32
#define HOME "HOMEPATH"
#else
#define HOME "HOME"
#endif

#ifndef MAXPATHLEN
#define MAXPATHLEN 10000
#else
#undef MAXPATHLEN
#define MAXPATHLEN 10000
#endif


#if defined _WIN32
#define FULL_PATH(path, tmp) tmp = _fullpath(NULL, path, 0)
#else
#define FULL_PATH(path, tmp) tmp = realpath(path, NULL)
#endif


namespace yatsc {
static std::mutex mutex_;

typedef Vector<String> PathArray;

char* GetDirectoryFromPath(const char* path, char* buffer) {
  int index = strlen(path);
  bool is_slashed = false;
  while (index--) {
    if (is_slashed) {
      break;
    }
    if (path[index] == '/') {
      is_slashed = true;
    }
  }
  buffer = Strdup(path);
  if (is_slashed) {
    buffer[index + 1] = 0;
  }
  return buffer;
}


char* GetFileNameFromPath(const char* path, char* buffer) {
  int len = strlen(path);
  if (path[len - 1] == '/') {
    return NULL;
  }
  const char* ptr = strrchr(path, '/');
  if (ptr) {
    buffer = Strdup(ptr + 1);
  } else {
    buffer = Strdup(path);
  }
  return buffer;
}


void ConvertBackSlash(String* buffer) {
  size_t len = buffer->size();
  for (int i = 0; i < len; i++) {
    if ((*buffer)[i] == '\\') {
      (*buffer)[i] = '/';
    }
  }
}


char* GetExtension(const char* path, char* buffer) {
  bool has = false;
  int begin = strlen(path) - 1;
  for (int i = begin; i < -1; i--) {
    if (path[i] == '.' && i < begin) {
      has = true;
      buffer = Strdup(&(path[i + 1]));
      break;
    }
  }
  if (!has) {
    return NULL;
  }
  return buffer;
}


void GetAbsolutePath(const char* path, String* buffer) {
  if (strcmp(path, "/") == 0) {
    return;
  }
  char *tmp;
  FULL_PATH(path, tmp);
  if (tmp != NULL) {
#ifdef _WIN32
    ConvertBackSlash(buffer);
#endif
    (*buffer) = tmp;
    free(tmp);
  } else {
    FATAL("GetAbsolutePath failed.");
  }
}


void Path::NormalizeArray(Vector<String>* path_array) {
  Vector<String>::iterator it = path_array->begin();
  while (it != path_array->end()) {
    if (it->size() == 0) {
      it = path_array->erase(it);
      if (it == path_array->end()) return;
    } else if (*it == ".") {
      it = path_array->erase(it);
    } else if (*it == "..") {
      it = path_array->erase(it - 1);
      it = path_array->erase(it);
    }
    ++it;
  }
}


PathArray GetPathArray(const char* path) {
  PathArray array;
  String tmp(path);
  ConvertBackSlash(&tmp);
  size_t index = 0;
  
  if (tmp[0] == '/') {
    index = 1;
  }

  String::size_type pos(tmp.find("/", index));

  if (tmp[0] == '/') {
    index = 0;
  }

  if (*(tmp.rbegin()) == '/') {
    tmp.erase(tmp.size() - 1, 1);
  }
  
  while (1) {
    array.push_back(tmp.substr(index, pos - index));
    index = pos + 1;
    pos = tmp.find("/", index);
    if (pos == String::npos) {
      array.push_back(tmp.substr(index, tmp.size() - index));
      return std::move(array);
    }
  }
  return std::move(array);
}


String JoinPathArray(const PathArray& array) {
  StringStream ret;
  int i = 0;
  for (auto c: array) {
    if (i == 0) {
      ret << c;
    } else {
      ret << "/" << c;
    }
    i++;
  }
  return std::move(ret.str());
}


String Path::Resolve(const char* path) {
  String cwd = current_directory();
  return Join(cwd.c_str(), path);
}


String Path::Join(const char* base, const char* path) {
  if (strcmp(base, path) == 0) {
    return String(base);
  }
  
  PathArray base_array = GetPathArray(base);
  PathArray target_array = GetPathArray(path);
  
  if (Stat(base).IsReg()) {
    base_array.pop_back();
  }

  if (base_array[0] != target_array[0]) {
    base_array.insert(base_array.end(), target_array.begin(), target_array.end());
    NormalizeArray(&base_array);
    return std::move(JoinPathArray(base_array));
  }
  
  NormalizeArray(&target_array);
  return std::move(JoinPathArray(target_array));
}


String Path::Basename(const char* path) {
  PathArray target_array = GetPathArray(path);
  return std::move(target_array.back());
}


String Path::Dirname(const char* path) {
  PathArray target_array = GetPathArray(path);
  target_array.pop_back();
  NormalizeArray(&target_array);
  return std::move(JoinPathArray(target_array));
}


String Path::Extname(const char* path) {
  PathArray target_array = GetPathArray(path);
  NormalizeArray(&target_array);
  size_t pos = target_array.back().find_last_of(".");
  if (pos != String::npos) {
    return target_array.back().substr(pos, target_array.back().size() - pos);
  }
  return String();
}


String Path::Relative(const char* from, const char* to) {
  if (strcmp(from, to) == 0) {
    return String(from);
  }
  
  PathArray base_array = GetPathArray(from);
  PathArray target_array = GetPathArray(to);

  int i = 0;
  int base_size = base_array.size();
  int target_size = target_array.size();
  String ret;
  
  while ((i < base_size) || (i < target_size)) {    
    if (i >= base_size) {
      ret += target_array.at(i);
      ret += "/";
    } else if (i >= target_size) {
      StringStream st;
      while (i < base_size) {
        st << "../";
        i++;
      }
      ret += std::move(st.str());
    } else if (base_array.at(i).compare(target_array.at(i)) != 0) {
      StringStream st;
      while (i < base_size) {
        st << "../";
        base_array.pop_back();
        base_size = base_array.size();
      }
      while (i < target_size) {
        st << target_array[ i ];
        st << "/";
        i++;
      }
      ret += std::move(st.str());
    }
    i++;
  }
  if (ret.size() > 1 && ret.at(ret.size() - 1) == '/') {
    ret.erase(ret.size() - 1, 1);
  }
  return std::move(ret);
}


String Path::current_directory() {
  std::lock_guard<std::mutex> lock(mutex_);
#define GW_BUF_SIZE 1000
#ifdef _WIN32
    char tmp[GW_BUF_SIZE];
    DWORD isSuccess = GetCurrentDirectory(sizeof(tmp), tmp);
    if (!isSuccess) {
      FATAL("GetCwd failed.");
    }
    String current_dir = tmp;
    ConvertBackSlash(&current_dir);
#else
    char tmp[GW_BUF_SIZE];
    char* dir = getcwd(tmp, sizeof (tmp));
    if (dir == NULL) {
      FATAL(stderr, "GetCwd fail.");
    };
    String current_dir(dir);
#endif
    return std::move(current_dir);
}


String Path::home_directory() {
  std::lock_guard<std::mutex> lock(mutex_);
  String buf;
#ifdef _WIN32
  const char* drive = getenv("HOMEDRIVE");
  const char* home = getenv(HOME);
  if (home && drive) {
    SPrintf(buf, "%s/%s", drive, home);
    GetAbsolutePath(buf.c_str(), &buf);
  }
#else
  const char* tmp = getenv(HOME);
  if (home) {
    buf = tmp;
  }
#endif
  return std::move(buf);
}

}
