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


#ifndef PARSER_SOURCE_POSITION_H
#define PARSER_SOURCE_POSITION_H

#include "../utils/utils.h"

namespace yatsc {
class SourcePosition: private Unmovable {
 public:
  typedef long Id;
  
  SourcePosition()
      : start_col_(0),
        end_col_(0),
        start_line_number_(0),
        end_line_number_(0){}
  
  
  SourcePosition(size_t start_col, size_t end_col, size_t start_line_number, size_t end_line_number)
      : start_col_(start_col),
        end_col_(end_col),
        start_line_number_(start_line_number),
        end_line_number_(end_line_number){}


  SourcePosition(const SourcePosition& source_position)
      : start_col_(source_position.start_col_),
        end_col_(source_position.end_col_),
        start_line_number_(source_position.start_line_number_),
        end_line_number_(source_position.end_line_number_){}


  inline void operator = (const SourcePosition& source_position) {
    start_col_ = source_position.start_col_;
    end_col_ = source_position.end_col_;
    start_line_number_ = source_position.start_line_number_;
    end_line_number_ = source_position.end_line_number_;
  }


  inline bool operator > (const SourcePosition& source_position) YATSC_NO_SE {
    if (start_line_number_ == source_position.start_line_number_) {
      return start_col_ > source_position.start_col_;
    }
    return start_line_number_ > source_position.start_line_number_;
  }


  inline bool operator < (const SourcePosition& source_position) YATSC_NO_SE  {
    if (start_line_number_ == source_position.start_line_number_) {
      return start_col_ < source_position.start_col_;
    }
    return start_line_number_ < source_position.start_line_number_;
  }


  inline bool operator >= (const SourcePosition& source_position) YATSC_NO_SE  {
    if (start_line_number_ == source_position.start_line_number_) {
      return start_col_ >= source_position.start_col_;
    }
    return start_line_number_ >= source_position.start_line_number_;
  }


  inline bool operator <= (const SourcePosition& source_position) YATSC_NO_SE  {
    if (start_line_number_ == source_position.start_line_number_) {
      return start_col_ <= source_position.start_col_;
    }
    return start_line_number_ <= source_position.start_line_number_;
  }


  inline bool operator == (const SourcePosition& source_position) YATSC_NO_SE  {
    return start_line_number_ == source_position.start_line_number_ &&
      start_col_ == source_position.start_col_ &&
      end_line_number_ == source_position.end_line_number_ &&
      end_col_ == source_position.end_col_;
  }


  YATSC_CONST_PROPERTY(size_t, start_col, start_col_)
  YATSC_CONST_PROPERTY(size_t, end_col, end_col_)
  YATSC_CONST_PROPERTY(size_t, start_line_number, start_line_number_)
  YATSC_CONST_PROPERTY(size_t, end_line_number, end_line_number_)


 private:
  size_t start_col_;
  size_t end_col_;
  size_t start_line_number_;
  size_t end_line_number_;
};
}

#endif
