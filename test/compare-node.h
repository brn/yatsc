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

#ifndef TEST_COMPARE_NODE_H
#define TEST_COMPARE_NODE_H

#include <string>
#include <vector>
#include <sstream>
#include <gtest/gtest.h>

namespace yatsc {namespace testing {

inline std::vector<std::string> Split(std::string buf, const char* delim) {
  std::string::size_type pos = buf.find(delim);
  std::string::size_type index = 0;
  std::vector<std::string> ret;

  if (std::string::npos == pos) {
    ret.push_back(buf);
    return std::move(ret);
  }

  while (1) {
    if (std::string::npos != pos) {
      ret.push_back(buf.substr(index, pos - index));
    } else {
      ret.push_back(buf.substr(index));
      break;
    }
    index = pos + 1;
    pos = buf.find(delim, index);
  }
  return std::move(ret);
}


inline std::string Join(int pos, std::vector<std::string> v) {
  typedef std::vector<std::string> Vector;
  std::stringstream ss;
  int i = 0;
  for (auto str: v) {
    ss << str << '\n';
    if (i == pos) {
      std::string::size_type found = str.find_first_not_of(" ");
      std::string::size_type found_end = str.find_last_not_of(" ");
      found = found == std::string::npos? 0: found;
      found_end = found_end == std::string::npos? str.size() - 1: found_end;
      if (found != std::string::npos && found_end != std::string::npos) {
        ss << std::string(found, ' ')
           << std::string((found_end - found) + 1, '^')
           << "\n";
      }
    }
    i++;
  }
  if (i <= pos) {
    ss << "\n" << std::string(0, ' ')
       << std::string(v.back().size(), '^')
       << "\n";
  }
  std::string ret = std::move(ss.str());
  ret.substr(0, ret.size() - 1);
  return std::move(ret);
}


inline ::testing::AssertionResult DoCompareNode(int line_number, std::string value, std::string expected) {
  typedef std::vector<std::string> Vector;
  auto v = Split(value, "\n");
  auto e = Split(expected, "\n");
  Vector::iterator v_it = v.begin();
  Vector::iterator e_it = e.begin();
  Vector::iterator v_end = v.end();
  Vector::iterator e_end = e.end();
  int index = 0;

  while (1) {
    if ((*v_it) != (*e_it)) {
      return ::testing::AssertionFailure()
        << "\nExpectation is not match to the result.\n"
        << "at line " << line_number << "\n"
        << "value:  \n" << Join(index, v) << '\n'
        << "expected:  \n" << Join(index, e) << '\n';
    }
    ++v_it;
    ++e_it;
    index++;

    if (v_it == v_end) {
      if (e_it != e_end) {
        return ::testing::AssertionFailure()
          << "\nExpectation is longer than result.\n"
          << "at line " << line_number << "\n"
          << "value:  \n" << Join(index, v) << '\n'
          << "expected:  \n" << Join(index, e) << '\n';
      }
      break;
    } else if (e_it == e_end) {
      if (v_it != v_end) {
        return ::testing::AssertionFailure()
          << "\nExpectation is shorter than result.\n"
          << "at line " << line_number << "\n"
          << "value:  \n" << Join(index, v) << '\n'
          << "expected:  \n" << Join(index, e) << '\n';
      }
      break;
    }
  }

  return ::testing::AssertionSuccess();
}


inline void CompareNode(int line_number, std::string value, std::string expected) {
  ASSERT_TRUE((DoCompareNode(line_number, value, expected)));
}

}}

#endif
