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

#ifndef UTILS_PATH_H
#define UTILS_PATH_H

#include "./utils.h"
#include "./stl.h"

namespace yatsc {

// Treat filesystem path string.
class Path : private Static {
 public:
  // Return resolved path string.
  static String Resolve(const String& path) {
    return Resolve(path.c_str());
  }

  
  // Return resolved path string.
  static String Resolve(const char* path);


  // Return resolved path string.
  static String Join(const String& base, const String& path) {
    return Join(base.c_str(), path.c_str());
  }


  // Return resolved path string.
  static String Join(const char* base, const String& path) {
    return Join(base, path.c_str());
  }


  // Return resolved path string.
  static String Join(const String& base, const char* path) {
    return Join(base.c_str(), path);
  }

  
  // Return resolved path string.
  static String Join(const char* base, const char* path);


  // Return relative path.
  static String Relative(const String& from, const String& to) {
    return Relative(from.c_str(), to.c_str());
  }


  // Return relative path.
  static String Relative(const char* from, const char* to);

  
  // Return last segment of path.
  static String Basename(const String& path) {
    return Basename(path.c_str());
  }


  // Return last segment of path.
  static String Basename(const char* path);


  // Return directory segment of path.
  static String Dirname(const String& path) {
    return Dirname(path.c_str());
  }


  // Return directory segment of path.
  static String Dirname(const char* path);


  // Return file extension if exists.
  static String Extname(const String& path) {
    return Extname(path.c_str());
  }


  static String Extname(const char* path);


  // Return current working directory.
  static String current_directory();

  
  // Return user home directory.
  static String home_directory();
  

  // Normalize path string.
  static void NormalizePath(const char* path, std::string* buf);

 private:
  static void NormalizeArray(Vector<String>*);
};

}

#endif

