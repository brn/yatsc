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
TokenInfo* ParserBase<UCharInputIterator>::Next() {
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
TokenInfo* ParserBase<UCharInputIterator>::Current() {
  if (!token_buffer_.CursorUpdated()) {
    return token_buffer_.Current();
  }
  return current_token_info_;
}


/**
 * Append token to the buffer.
 * A passed TokenInfo is copied by the copy constructor.
 */
template <typename UCharInputIterator>
void ParserBase<UCharInputIterator>::TokenBuffer::PushBack(TokenInfo* token_info) {
  buffer_->emplace_back(*token_info);
}


template <typename UCharInputIterator>
bool ParserBase<UCharInputIterator>::TokenBuffer::IsEmpty() YATSC_NO_SE {
  return buffer_->empty();
}


/**
 * Return next TokenInfo.
 * If cursor is end of buffer, return invalid token info.
 */
template <typename UCharInputIterator>
TokenInfo* ParserBase<UCharInputIterator>::TokenBuffer::Next() {
  if (cursor_ >= (buffer_->size() - 1)) {return &null_token_info_;}
  return &((*buffer_)[cursor_++]);
}


/**
 * Return current TokenInfo.
 * If cursor is end of buffer, return invalid token info.
 */
template <typename UCharInputIterator>
TokenInfo* ParserBase<UCharInputIterator>::TokenBuffer::Current() {
  if (cursor_ >= buffer_->size()) {return &null_token_info_;}
  return &((*buffer_)[cursor_]);
}


/**
 * Peek next TokenInfo.
 * If cursor is end of buffer, return invalid token info.
 */
template <typename UCharInputIterator>
TokenInfo* ParserBase<UCharInputIterator>::TokenBuffer::Peek(size_t pos) {
  if (pos < 0 || pos >= buffer_->size()) {return &null_token_info_;}
  return &((*buffer_)[pos]);
}


/**
 * Call token_buffer_.PushBack.
 */
template <typename UCharInputIterator>
void ParserBase<UCharInputIterator>::PushBackBuffer(TokenInfo* token_info) {
  token_buffer_.PushBack(token_info);
}


/**
 * Call token_buffer_.Rewind.
 */
template <typename UCharInputIterator>
void ParserBase<UCharInputIterator>::RewindBuffer(size_t num) {
  token_buffer_.Rewind(num);
}


/**
 * Call token_buffer_.Peek.
 */
template <typename UCharInputIterator>
TokenInfo* ParserBase<UCharInputIterator>::PeekBuffer(size_t num) {
  return token_buffer_.Peek(num);
}


/**
 * Call token_buffer_.cursor.
 */
template <typename UCharInputIterator>
TokenCursor ParserBase<UCharInputIterator>::GetBufferCursorPosition() YATSC_NOEXCEPT {
  return token_buffer_.cursor();
}


/**
 * Call token_buffer.SetCursorPosition.
 */
template <typename UCharInputIterator>
void ParserBase<UCharInputIterator>::SetBufferCursorPosition(size_t num) {
  token_buffer_.SetCursorPosition(num);
}

/**
 * Check whether cursor is end of buffer or not.
 */
template <typename UCharInputIterator>
bool ParserBase<UCharInputIterator>::TokenBuffer::CursorUpdated() {
  if (buffer_->empty()) {
    return true;
  }
  return cursor_ == (buffer_->size() - 1);
}
}
