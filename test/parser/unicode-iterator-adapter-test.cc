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

#include "../gtest-header.h"
#include <string>
#include "../readfile.h"
#include "../../src/parser/unicode-iterator-adapter.h"
#include "../../src/parser/uchar.h"
#include "../compare-string.h"


::testing::AssertionResult Failed(const yatsc::UChar& uchar, const yatsc::UnicodeIteratorAdapter<yatsc::String::iterator>& un) {
  return ::testing::AssertionFailure() << "Invalid unicode charactor. at: " << un.current_position()
                                       << " value: " << uchar.uchar();
}


::testing::AssertionResult IsValid(const yatsc::UChar& uchar, const yatsc::UnicodeIteratorAdapter<yatsc::String::iterator>& un) {
  if (!uchar.IsInvalid() && static_cast<bool>(uchar)) {
    return ::testing::AssertionSuccess();
  }
  return Failed(uchar, un);
}


::testing::AssertionResult IsSurrogatePair(const yatsc::UChar& uchar, const yatsc::UnicodeIteratorAdapter<yatsc::String::iterator>& un) {
  if (uchar.IsSurrogatePair()) {
    return ::testing::AssertionSuccess();
  }
  return Failed(uchar, un);
}


::testing::AssertionResult IsNotSurrogatePair(const yatsc::UChar& uchar, const yatsc::UnicodeIteratorAdapter<yatsc::String::iterator>& un) {
  if (uchar.IsSurrogatePair()) {
    return Failed(uchar, un);
  }
  return ::testing::AssertionSuccess();
}


inline void UnicodeTest(const char* input, const char* expected, size_t expected_size) {
  auto source = yatsc::testing::ReadFile(input);
  auto result = yatsc::testing::ReadFile(expected);
  yatsc::String buffer;
  yatsc::String utf8_buffer;
  static const char* kFormat = "%#019x";
  auto end = source.end();
  yatsc::UnicodeIteratorAdapter<yatsc::String::iterator> un(source.begin());
  int index = 0;
  size_t size = 0;
  for (;un != end; std::advance(un, 1)) {
    const yatsc::UChar uc = *un;
    ASSERT_TRUE(IsValid(uc, un));
    if (uc.IsAscii()) {
      buffer.append(1, uc.ToAscii());
    } else {
      if (!uc.IsSurrogatePair()) {
        yatsc::SPrintf(buffer, true, kFormat, uc.uchar());
      } else {
        yatsc::SPrintf(buffer, true, kFormat, uc.ToHighSurrogate());
        yatsc::SPrintf(buffer, true, kFormat, uc.ToLowSurrogate());
      }
    }
    utf8_buffer.append(uc.utf8());
    size += uc.IsSurrogatePair()? 2: 1;
    index++;
  }
  ASSERT_EQ(expected_size, size);
  yatsc::testing::CompareBuffer(buffer, result);
  yatsc::testing::CompareBuffer(utf8_buffer, source);
}


TEST(UnicodeIteratorAdapter, parse_valid_utf8_code_test) {
  UnicodeTest("test/parser/unicode-test-cases/valid-utf8.txt",
              "test/parser/unicode-test-cases/valid-utf8.result.txt",
              148);
}


TEST(UnicodeIteratorAdapter, parse_valid_utf8_surrogate_pair_code_test) {
  UnicodeTest("test/parser/unicode-test-cases/valid-utf8-surrogate-pair.txt",
              "test/parser/unicode-test-cases/valid-utf8-surrogate-pair.result.txt",
              676);
}
