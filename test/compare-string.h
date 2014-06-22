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

#ifndef TEST_COMPARE_STRING_H_
#define TEST_COMPARE_STRING_H_

#include <gtest/gtest.h>
#include <string>

namespace yatsc { namespace testing {
inline ::testing::AssertionResult CompareUchar(const std::string& value, const std::string expected, int i) {
  char a = value.at(i);
  char b = expected.at(i);
  if (a == b) {
    return ::testing::AssertionSuccess();
  }
  return ::testing::AssertionFailure()
      << "The value of index " << i << " expected " << b << " but got " << a
      << "\nvalue:    " << value.substr(0, i) << '\n'
      << "           " << std::string(i - 2, '-') << "^"
      << "\nexpected: " << expected.substr(0, i) << '\n'
      << "           " << std::string(i - 2, '-') << "^"
      << '\n';
}


inline void CompareString(const std::string& buffer, const std::string& expectation) {
  for (int i = 0, len = buffer.size(); i < len; i++) {
    ASSERT_TRUE(CompareUchar(buffer, expectation, i));
    i++;
  }
}


inline void CompareString(const char* buffer, const char* expectation) {
  std::string b(buffer);
  std::string e(expectation);
  CompareString(b, e);
}

}} //namespace yatsc::testing

#endif
