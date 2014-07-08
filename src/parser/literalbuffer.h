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

#ifndef PARSER_LITERLBUFFER_H
#define PARSER_LITERLBUFFER_H

#include <unordered_map>
#include "./utfstring.h"

namespace yatsc {
class LiteralBuffer: private Unmovable, private Uncopyable {
  typedef std::unordered_map<std::string, UtfString*> UtfStringBuffer;
 public:
  LiteralBuffer() {}

  UtfString* InsertValue(const UtfString& utf_string, Regions* regions) {
    auto ret = buffer_.emplace(utf_string.utf8_value(), regions->New<UtfString>(utf_string));
    return ret.first->second;
  }
  
 private:
  UtfStringBuffer buffer_;
};
}

#endif
