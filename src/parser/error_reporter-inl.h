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

#ifndef PARSER_ERROR_REPORTER_INL_H
#define PARSER_ERROR_REPORTER_INL_H

namespace yatsc {

template <typename Error>
void ErrorReporter::Throw(const SourcePosition& source_position) {
  static const size_t kMaxWidth = 120;
  String line_source = GetLineSource(source_position);
  st_ << "\n" << module_info_->module_name() << ':' << source_position.start_line_number() <<
    ':' << source_position.start_col() << '-' << source_position.end_col();

  size_t start_col = source_position.start_col();
  size_t end_col = source_position.end_col();
  size_t threshold = start_col - 1;
  size_t end = end_col > start_col? end_col - 1: end_col;
  size_t start = 0;
  
  if (line_source.size() > kMaxWidth) {
    st_ << "\n" << line_source.substr(0 * kMaxWidth, kMaxWidth) << "\n";
    size_t wrap = 1;
    size_t count = 0;
    for (size_t i = start; i < end; i++, count++) {
      if (count == kMaxWidth) {
        st_ << "\n" << line_source.substr(wrap * kMaxWidth, kMaxWidth) << "\n";
        wrap++;
        count = 0;
      }
      if (i >= threshold) {
        st_ << '^';
      } else {
        st_ << '-';
      }
    }
    size_t last = wrap * kMaxWidth;
    if (last < line_source.size()) {
      st_ << "\n" << line_source.substr(last, line_source.size() - last);
    }
  } else {
    st_ << "\n" << line_source << "\n";
    for (size_t i = start; i < end; i++) {
      if (i >= threshold) {
        st_ << '^';
      } else {
        st_ << '-';
      }
    }
  }
  
  String message = st_.str();
  Error e(message.c_str());
  st_.str("");
  st_.clear();
  st_ << std::dec;
  throw e;
}


String ErrorReporter::GetLineSource(const SourcePosition& source_position) {
  size_t count = 0;
  String::size_type start = 0;
  
  while (1) {
    String::size_type end = source_.find("\n", start);

    if (String::npos != end) {
      count++;
    } else if (1 == source_position.start_line_number()) {
      return std::move(String(source_));
    } else {
      return std::move(String(""));
    }

    if (source_position.start_line_number() == count) {
      return std::move(source_.substr(start, end - start));
    }
      
    start = end + 1;
  }
  return std::move(String(""));
}
}

#endif
