// 
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


namespace yatsc {

// Return next token  if token buffer's cursor is in the end of input,
// otherwise return a current token from the token buffer and advance token buffer cursor.
template <typename UCharInputIterator>
TokenInfo* Parser<UCharInputIterator>::Next() {
  if (current_token_info_ != nullptr) {
    prev_token_info_ = *current_token_info_;
  }
  return current_token_info_ = scanner_->Scan();
}


// Return current token  if token buffer's cursor is in the end of input,
// otherwise return current token from token buffer.
template <typename UCharInputIterator>
TokenInfo* Parser<UCharInputIterator>::Current() YATSC_NOEXCEPT {
  return current_token_info_;
}


template <typename UCharInputIterator>
TokenInfo* Parser<UCharInputIterator>::Prev() YATSC_NOEXCEPT {
  return &prev_token_info_;
}


template <typename UCharInputIterator>
bool Parser<UCharInputIterator>::IsLineTermination() {
  return Current()->type() == Token::LINE_TERMINATOR ||
    Current()->type() == Token::END_OF_INPUT ||
    (Prev() != nullptr &&
     Prev()->has_line_break_before_next());
}


template <typename UCharInputIterator>
void Parser<UCharInputIterator>::ConsumeLineTerminator() {
  if (Current()->type() == Token::LINE_TERMINATOR) {
    Next();
  }
}
}

