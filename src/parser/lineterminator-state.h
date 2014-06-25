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

#ifndef PARSER_LINE_TERMINATOR_STATE_H_
#define PARSER_LINE_TERMINATOR_STATE_H_

#include <bitset>
#include "../utils/utils.h"

namespace yatsc {

// State of line terminator or break.
class LineTerminatorState: private Unmovable {
 public:
  LineTerminatorState() {}

  
  LineTerminatorState(const LineTerminatorState& line_terminator_state)
      : line_terminator_state_(line_terminator_state.line_terminator_state_) {}


  LineTerminatorState& operator = (const LineTerminatorState& line_terminator_state) {
    line_terminator_state_ = line_terminator_state.line_terminator_state_;
    return *this;
  }
  

  YATSC_INLINE void set_line_terminator_before_next() YATSC_NOEXCEPT {
    line_terminator_state_.set(0);
  }


  YATSC_INLINE void set_line_break_before_next() YATSC_NOEXCEPT {
    line_terminator_state_.set(1);
  }


  YATSC_INLINE bool has_line_terminator_before_next() YATSC_NO_SE {
    return 1 == line_terminator_state_.test(0);
  }


  YATSC_INLINE bool has_line_break_before_next() YATSC_NO_SE {
    return 1 == line_terminator_state_.test(1);
  }


  YATSC_INLINE void Clear() YATSC_NOEXCEPT {
    line_terminator_state_.reset();
  }
 private:
  std::bitset<8> line_terminator_state_;
};
}

#endif
