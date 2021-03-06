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
#include "../src/utils/stl.h"

namespace yatsc {namespace testing {

inline yatsc::Vector<yatsc::String> Split(yatsc::String buf, const char* delim) {
  yatsc::String::size_type pos = buf.find(delim);
  yatsc::String::size_type index = 0;
  yatsc::Vector<yatsc::String> ret;

  if (yatsc::String::npos == pos) {
    ret.push_back(buf);
    return std::move(ret);
  }

  while (1) {
    if (yatsc::String::npos != pos) {
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


inline yatsc::String Join(int pos, yatsc::Vector<yatsc::String> v) {
  typedef yatsc::Vector<yatsc::String> Vector;
  yatsc::StringStream ss;
  int i = 0;
  for (auto str: v) {
    ss << str << '\n';
    if (i == pos) {
      yatsc::String::size_type found = str.find_first_not_of(" ");
      yatsc::String::size_type found_end = str.find_last_not_of(" ");
      found = found == yatsc::String::npos? 0: found;
      found_end = found_end == yatsc::String::npos? str.size() - 1: found_end;
      if (found != yatsc::String::npos && found_end != yatsc::String::npos) {
        ss << yatsc::String(found, ' ')
           << yatsc::String((found_end - found) + 1, '^')
           << "\n";
      }
    }
    i++;
  }
  if (i <= pos) {
    ss << "\n" << yatsc::String(0, ' ')
       << yatsc::String(v.back().size(), '^')
       << "\n";
  }
  yatsc::String ret = std::move(ss.str());
  ret.substr(0, ret.size() - 1);
  return std::move(ret);
}


inline ::testing::AssertionResult DoCompareNode(int line_number, yatsc::String value, yatsc::String expected) {
  typedef yatsc::Vector<yatsc::String> Vector;
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


inline void CompareNode(int line_number, yatsc::String value, yatsc::String expected) {
  ASSERT_TRUE((DoCompareNode(line_number, value, expected)));
}

}}

#endif
