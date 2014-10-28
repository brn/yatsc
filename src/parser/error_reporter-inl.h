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
  
  Vector<String> line_source_list = GetLineSource(source_position);
  String line_source;
  StringStream line;
  line << source_position.start_line_number() << ": ";
  StringStream padding;

  for (size_t i = 0; i < line.str().size(); i++) {
    padding << ' ';
  }
  
  st_ << "\n" << module_info_->module_name() << ':' << source_position.start_line_number() <<
    ':' << source_position.start_col() << '-' << source_position.end_col() << '\n';

  size_t start_col = source_position.start_col();
  size_t end_col = source_position.end_col();
  size_t threshold = start_col - 1;
  size_t end = end_col > start_col? end_col - 1: end_col;
  size_t start = 0;
  size_t last_line = 2;

  if (line_source_list.size() == 3) {
    st_ << "\n" << (source_position.start_line_number() - 1) << ": " << line_source_list[0];
    line_source = std::move(line_source_list[1]);
  } else if (line_source_list.size() == 2) {
    line_source = std::move(line_source_list[0]);
    last_line = 1;
  } else {
    line_source = std::move(line_source_list[0]);
    last_line = 0;
  }
  
  if (line_source.size() > kMaxWidth) {
    st_ << "\n" << source_position.start_line_number() << ": "
        << line_source.substr(0 * kMaxWidth, kMaxWidth) << "\n" << padding.str();
    size_t wrap = 1;
    size_t count = 0;
    for (size_t i = start; i < end; i++, count++) {
      if (count == kMaxWidth) {
        st_ << "\n" << padding.str() << line_source.substr(wrap * kMaxWidth, kMaxWidth) << "\n" << padding.str();
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
      st_ << "\n" << line_source.substr(last);
    }
  } else {
    st_ << "\n" << source_position.start_line_number() << ": " << line_source << "\n" << padding.str();
    for (size_t i = start; i < end; i++) {
      if (i >= threshold) {
        st_ << '^';
      } else {
        st_ << '-';
      }
    }
  }

  if (last_line != 0) {
    st_ << "\n" << (source_position.start_line_number() + 1) << ": " << line_source[last_line];
  }
  st_ << '\n';
  
  String message = st_.str();
  Error e(message.c_str());
  st_.str("");
  st_.clear();
  st_ << std::dec;
  throw e;
}


Vector<String> ErrorReporter::GetLineSource(const SourcePosition& source_position) {
  size_t count = 0;
  String::size_type start = 0;
  Vector<String> line_source;
  
  while (1) {
    String::size_type end = source_.find("\n", start);

    if (String::npos != end) {
      count++;
    } else if (1 == source_position.start_line_number()) {
      line_source.push_back(std::move(String(source_)));
      return std::move(line_source);
    } else {
      line_source.push_back(source_);
      return std::move(line_source);
    }

    if (source_position.start_line_number() >= count &&
        source_position.start_line_number() <= count + 1) {
      line_source.push_back(std::move(source_.substr(start, end - start)));
    }
      
    start = end + 1;
  }
  return std::move(line_source);
}
}

#endif
