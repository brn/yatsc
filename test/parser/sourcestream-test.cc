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
#include "../readfile.h"
#include "../compare-string.h"
#include "../../src/parser/sourcestream.h"

const char filename[] = "test/parser/sourcestream-test-cases/jquery.js";


TEST(SourceStream, read_all_ok) {
  yatsc::SourceStream st(filename);
  std::string expected = yatsc::testing::ReadFile(filename);
  yatsc::testing::CompareBuffer(st.buffer(), expected.c_str());
}


TEST(SourceStream, iterator_ok) {
  yatsc::SourceStream st(filename);
  std::string expected = yatsc::testing::ReadFile(filename);
  ASSERT_TRUE(st.success());
  auto it = st.begin();
  auto end = st.end();
  size_t i = 0;
  while (it != end) {
    yatsc::UC8 expectation = static_cast<yatsc::UC8>(expected.at(i));
    yatsc::UC8 actual = *it;
    ASSERT_EQ(expectation, actual);
    ++it;
    i++;
  }
}


TEST(SourceStream, load_error) {
  yatsc::SourceStream st("un-exists");
  ASSERT_FALSE(st.success());
  ASSERT_EQ(st.size(), 0u);
  ASSERT_GT(st.failed_message().size(), 0U);
}
