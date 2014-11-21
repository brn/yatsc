// The MIT License (MIT)
// 
// Copyright (c) 2013 Taketoshi Aono(brn)
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

#include "./error-formatter.h"
#include "./error-descriptor.h"
#include "./sourceposition.h"
#include "../compiler/module-info.h"

namespace yatsc {

String ErrorFormatter::Format(const ErrorDescriptor& error_descriptor) const {
  static const size_t kMaxWidth = 120;
  const SourcePosition source_position = error_descriptor.source_position();
  
  Vector<String> line_source_list = GetLineSource(source_position);
  String line_source;
  StringStream line;
  size_t start_line_number = source_position.start_line_number();
  line << source_position.start_line_number() << ": ";
  StringStream padding;
  StringStream message;

  message << error_descriptor.message() << "\n";
  
  for (size_t i = 0; i < line.str().size(); i++) {
    padding << ' ';
  }
  
  message << '\n' << module_info_->module_name() << ':' << source_position.start_line_number() <<
    ':' << source_position.start_col() << '-' << source_position.end_col() << '\n';

  size_t start_col = source_position.start_col();
  size_t end_col = source_position.end_col();
  size_t threshold = start_col - 1;
  size_t end = end_col > start_col? end_col - 1: end_col;
  size_t start = 0;
  size_t last_line = 2;
  
  if (line_source_list.size() == 3) {
    if (start_line_number == 1) {
      line_source = std::move(line_source_list[0]);
      last_line = 1;
    } else {
      message << '\n' << (source_position.start_line_number() - 1) << ": " << line_source_list[0];
      line_source = std::move(line_source_list[1]);
    }
  } else if (line_source_list.size() == 2) {
    if (start_line_number == 1) {
      line_source = std::move(line_source_list[0]);
      last_line = 1;
    } else {
      message << '\n' << (source_position.start_line_number() - 1) << ": " << line_source_list[0];
      line_source = std::move(line_source_list[1]);
      last_line = 0;
    }
  } else {
    line_source = std::move(line_source_list[0]);
    last_line = 0;
  }
  
  if (line_source.size() > kMaxWidth) {
    message << '\n' << source_position.start_line_number() << ": "
        << line_source.substr(0 * kMaxWidth, kMaxWidth) << '\n' << padding.str();
    size_t wrap = 1;
    size_t count = 0;
    for (size_t i = start; i < end; i++, count++) {
      if (count == kMaxWidth) {
        message << '\n' << padding.str() << line_source.substr(wrap * kMaxWidth, kMaxWidth) << '\n' << padding.str();
        wrap++;
        count = 0;
      }
      if (i >= threshold) {
        message << '^';
      } else {
        message << '-';
      }
    }
    size_t last = wrap * kMaxWidth;
    if (last < line_source.size()) {
      message << '\n' << line_source.substr(last);
    }
  } else {
    message << '\n' << source_position.start_line_number() << ": " << line_source << '\n' << padding.str();
    for (size_t i = start; i < end; i++) {
      if (i >= threshold) {
        message << '^';
      } else {
        message << '-';
      }
    }
  }

  if (last_line != 0) {
    message << '\n' << (source_position.start_line_number() + (last_line - 1)) << ": " << line_source_list[last_line];
  }
  message << '\n';
  return std::move(message.str());
}


Vector<String> ErrorFormatter::GetLineSource(const SourcePosition& source_position) const {
  size_t count = 0;
  String::size_type start = 0;
  Vector<String> line_source;
  String raw_source_code(module_info_->raw_source_code());

  if (1 == source_position.start_line_number()) {
    line_source.push_back(raw_source_code);
    return std::move(line_source);
  }
  
  while (1) {
    String::size_type end = raw_source_code.find('\n', start);
    
    count++;
    
    if (String::npos == end && count == 1) {
      line_source.push_back(raw_source_code);
      return std::move(line_source);
    }

    if (source_position.start_line_number() >= count - 1 &&
        source_position.start_line_number() <= count + 1) {
      line_source.push_back(std::move(raw_source_code.substr(start, end - start)));
    } else if (source_position.start_line_number() + 2 == count) {
      break;
    }

    if (String::npos == end) {
      break;
    }
      
    start = end + 1;
  }
  return std::move(line_source);
}
}
