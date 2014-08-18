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


namespace yatsc {
/**
 * Return next token  if token buffer's cursor is in the end of input,
 * otherwise return a current token from the token buffer and advance token buffer cursor.
 */
template <typename UCharInputIterator>
TokenInfo* Parser<UCharInputIterator>::Next() {
  if (!token_buffer_.CursorUpdated()) {
    return token_buffer_.Next();
  }
  current_token_info_ = scanner_->Scan();
  token_buffer_.PushBack(current_token_info_);
  return token_buffer_.Next();
}


/**
 * Return current token  if token buffer's cursor is in the end of input,
 * otherwise return current token from token buffer.
 */
template <typename UCharInputIterator>
TokenInfo* Parser<UCharInputIterator>::Current() {
  if (!token_buffer_.CursorUpdated()) {
    return token_buffer_.Current();
  }
  return current_token_info_;
}
}

