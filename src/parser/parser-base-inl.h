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
 * Append token to the buffer.
 * A passed TokenInfo is copied by the copy constructor.
 */
void ParserBase::TokenBuffer::PushBack(TokenInfo* token_info) {
  buffer_.emplace_back(*token_info);
}



bool ParserBase::TokenBuffer::IsEmpty() YATSC_NO_SE {
  return buffer_.empty();
}


/**
 * Return next TokenInfo.
 * If cursor is end of buffer, return invalid token info.
 */
TokenInfo* ParserBase::TokenBuffer::Next() {
  if (cursor_ >= (buffer_.size() - 1)) {return &null_token_info_;}
  return &(buffer_[cursor_++]);
}


/**
 * Return current TokenInfo.
 * If cursor is end of buffer, return invalid token info.
 */
TokenInfo* ParserBase::TokenBuffer::Current() {
  if (cursor_ >= buffer_.size()) {return &null_token_info_;}
  return &(buffer_[cursor_]);
}


/**
 * Peek next TokenInfo.
 * If cursor is end of buffer, return invalid token info.
 */
TokenInfo* ParserBase::TokenBuffer::Peek(size_t pos) {
  if (pos < 0 || pos >= buffer_.size()) {return &null_token_info_;}
  return &(buffer_[pos]);
}


/**
 * Call token_buffer_.PushBack.
 */
void ParserBase::PushBackBuffer(TokenInfo* token_info) {
  token_buffer_.PushBack(token_info);
}


/**
 * Call token_buffer_.Rewind.
 */
void ParserBase::RewindBuffer(size_t num) {
  token_buffer_.Rewind(num);
}


/**
 * Call token_buffer_.Peek.
 */
TokenInfo* ParserBase::PeekBuffer(size_t num) {
  return token_buffer_.Peek(num);
}


/**
 * Call token_buffer_.cursor.
 */
TokenCursor ParserBase::GetBufferCursorPosition() YATSC_NOEXCEPT {
  return token_buffer_.cursor();
}


/**
 * Call token_buffer.SetCursorPosition.
 */
void ParserBase::SetBufferCursorPosition(size_t num) YATSC_NOEXCEPT {
  token_buffer_.SetCursorPosition(num);
}

/**
 * Check whether cursor is end of buffer or not.
 */
bool ParserBase::TokenBuffer::CursorUpdated() {
  if (buffer_.empty()) {
    return true;
  }
  return cursor_ == (buffer_.size() - 1);
}
}
