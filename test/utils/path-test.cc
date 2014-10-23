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


#include "../gtest-header.h"
#include "../../src/utils/path.h"


TEST(Path, Resolve) {
  yatsc::String ret = yatsc::Path::Resolve("../yatsc/src/parser//parser.h");
}


TEST(Path, Join) {
  yatsc::String ret = yatsc::Path::Join("/usr/local/yatsc/", "../yatsc/./src/parser//parser.h");
  ASSERT_STREQ(ret.c_str(), "/usr/local/yatsc/src/parser/parser.h");
  ret = yatsc::Path::Join("/usr/local/yatsc/", "/usr/yatsc/./src/parser//parser.h");
  ASSERT_STREQ(ret.c_str(), "/usr/yatsc/src/parser/parser.h");
}


TEST(Path, Basename) {
  yatsc::String ret = yatsc::Path::Basename("../yatsc/src/parser//parser.h");
  ASSERT_STREQ(ret.c_str(), "parser.h");
}


TEST(Path, Dirname) {
  yatsc::String ret = yatsc::Path::Dirname("/usr/local/yatsc/src/parser//parser.h");
  ASSERT_STREQ(ret.c_str(), "/usr/local/yatsc/src/parser");
}


TEST(Path, Extname) {
  yatsc::String ret = yatsc::Path::Extname("/usr/local/yatsc/src/parser/.//parser.h");
  ASSERT_STREQ(ret.c_str(), ".h");
}
