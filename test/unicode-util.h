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


#ifndef TEST_UNICODE_UTIL_H_
#define TEST_UNICODE_UTIL_H_

#include <utility>
#include <vector>
#include <string.h>
#include "../src/utils/stl.h"
#include "../src/parser/uchar.h"
#include "../src/parser/unicode-iterator-adapter.h"

namespace yatsc {
namespace testing {
template <typename CharBuffer>
inline yatsc::Vector<UChar> AsciiToUCharVector(const CharBuffer& code) {
  yatsc::UCharBuffer v;
  yatsc::UnicodeIteratorAdapter<typename CharBuffer::const_iterator> uia(code.cbegin());
  for (;uia != code.end(); ++uia) {
    v.push_back(std::move(*uia));
  }
  return v;
}
}} //namespace yatsc::testing

#endif
