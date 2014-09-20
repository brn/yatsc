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
static const int kMaxWidth = 100;

std::string Replace(std::string str1, std::string str2, std::string str3) {
  std::string::size_type  pos(str1.find(str2));

  while (pos != std::string::npos) {
    str1.replace(pos, str2.length(), str3);
    pos = str1.find(str2, pos + str3.length());
  }

  return str1;
}


inline ::testing::AssertionResult CompareUchar(const std::string& value, const std::string expected, int i) {
  char a = value.at(i);
  char b = expected.at(i);
  if (a == b) {
    return ::testing::AssertionSuccess();
  }

  std::string cloned_value = value;
  std::string cloned_expectation = expected;

  cloned_value = Replace(cloned_value, std::string("\n"), std::string(" "));
  cloned_expectation = Replace(cloned_expectation, std::string("\n"), std::string(" "));

  size_t begin = i + 10 > kMaxWidth? (i + 10) - kMaxWidth: 0;
  return ::testing::AssertionFailure()
      << "The value of index " << i << " expected " << b << " but got " << a
      << "\nvalue:    " << cloned_value.substr(begin, kMaxWidth + 10) << '\n'
      << "           " << std::string(kMaxWidth - 11, '-') << "^"
      << "\nexpected: " << cloned_expectation.substr(begin, kMaxWidth) << '\n'
      << "           " << std::string(kMaxWidth - 11, '-') << "^"
      << '\n';
}


inline void CompareBuffer(const std::string& buffer, const std::string& expectation) {
  for (int i = 0, len = buffer.size(); i < len; i++) {
    ASSERT_TRUE(CompareUchar(buffer, expectation, i));
    i++;
  }
}


inline void CompareBuffer(const char* buffer, const char* expectation) {
  std::string b(buffer);
  std::string e(expectation);
  CompareBuffer(b, e);
}

}} //namespace yatsc::testing

#endif
