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

#ifndef UTILS_STL_H
#define UTILS_STL_H

#include <string>
#include <sstream>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include "../memory/heap.h"

namespace yatsc {
typedef std::basic_string<char, std::char_traits<char>, StandardAllocator<char>> String;
typedef std::basic_stringstream<char, std::char_traits<char>, StandardAllocator<char>> StringStream;
template<typename T>
using Vector = std::vector<T, StandardAllocator<T>>;
template<typename Key, typename Value>
using MultiHashMap = std::unordered_multimap<Key,
                                             Value,
                                             std::hash<Key>,
                                             std::equal_to<Key>,
                                             StandardAllocator<std::pair<const Key, Value>>>;

template<typename Key, typename Value>
using HashMap = std::unordered_map<Key,
                                   Value,
                                   std::hash<Key>,
                                   std::equal_to<Key>,
                                   StandardAllocator<std::pair<const Key, Value>>>;

template<typename Key>
using HashSet = std::unordered_set<Key,
                                   std::hash<Key>,
                                   std::equal_to<Key>,
                                   StandardAllocator<Key>>;


template <typename Begin, typename End>
using IteratorRange = std::pair<Begin, End>;


template <typename Begin, typename End>
inline IteratorRange<Begin, End> MakeRange(Begin begin, End end) {
  return std::make_pair(begin, end);
}
}

#endif
