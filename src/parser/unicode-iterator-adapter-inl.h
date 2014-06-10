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

#ifndef PARSER_UNICODE_ITERATOR_ADAPTER_INL_H_
#define PARSER_UNICODE_ITERATOR_ADAPTER_INL_H_


namespace rasp {
/**
 * Constructor
 * @param begin random access iterator.
 */
template <typename InputIterator>
UnicodeIteratorAdapter<InputIterator>::UnicodeIteratorAdapter(InputIterator begin)
    : current_position_(0),
      line_number_(1),
      begin_(begin) {}


/**
 * Copy constructor.
 */
template <typename InputIterator>
template <typename T>
UnicodeIteratorAdapter<InputIterator>::UnicodeIteratorAdapter(const UnicodeIteratorAdapter<T>& it)
    : current_position_(it.current_position_),
      line_number_(it.line_number_),
      begin_(it.begin_) {}


/**
 * Move constructor.
 */
template <typename InputIterator>
template <typename T>
UnicodeIteratorAdapter<InputIterator>::UnicodeIteratorAdapter(UnicodeIteratorAdapter<T>&& it)
    : current_position_(it.current_position_),
      line_number_(it.line_number_),
      begin_(std::move(it.begin_)) {}



template <typename InputIterator>
UChar UnicodeIteratorAdapter<InputIterator>::Convert () const {
  UC8Bytes utf8;
  auto byte_count = utf8::GetByteCount(*begin_);
  auto next = ConvertUtf8ToUcs2(byte_count, &utf8);
  // invalidate.
  if (next != 0) {
    return UChar(next, utf8);
  }
  // invalid UChar.
  return UChar();
}


template <typename InputIterator>
uint32_t UnicodeIteratorAdapter<InputIterator>::ConvertUtf8ToUcs2(size_t byte_count, UC8Bytes* utf8) const {
  switch (byte_count) {
    case 1:
      return ConvertAscii(utf8);
    case 2:
      return Convert2Byte(utf8);
    case 3:
      return Convert3Byte(utf8);
    case 4:
      return Convert4Byte(utf8);
    default:
      return 0;
  }
}


template <typename InputIterator>
uint32_t UnicodeIteratorAdapter<InputIterator>::Convert2Byte(UC8Bytes* utf8) const {
  const uint8_t kMinimumRange = 0x00000080;
  auto c = *begin_;
  if (utf8::IsNotNull(c)) {
    (*utf8)[0] = c;
    auto next = unicode::Mask<5>(c) << 6;
    c = *(begin_ + 1);
    if (utf8::IsValidSequence(c)) {
      next = next | unicode::Mask<6>(c);
      if (next > kMinimumRange) {
        (*utf8)[1] = c;
        (*utf8)[2] = '\0';
        return next;
      }
    }
  }
  return 0;
}


template <typename InputIterator>
uint32_t UnicodeIteratorAdapter<InputIterator>::Convert3Byte(UC8Bytes* utf8) const {
  const int kMinimumRange = 0x00000800;
  auto c = *begin_;
  if (utf8::IsNotNull(c)) {
    (*utf8)[0] = c;
    auto next = unicode::Mask<4>(c) << 12;
    c = *(begin_ + 1);
    if (utf8::IsValidSequence(c)) {
      (*utf8)[1] = c;
      next = next | unicode::Mask<6>(c) << 6;
      c = *(begin_ + 2);
      if (utf8::IsValidSequence(c)) {
        next = next | unicode::Mask<6>(c);
        if (next > kMinimumRange && utf16::IsOutOfSurrogateRange(next)) {
          (*utf8)[2] = c;
          (*utf8)[3] = '\0';
          return next;
        }
      }
    }
  }
  return 0;
}


template <typename InputIterator>
uint32_t UnicodeIteratorAdapter<InputIterator>::Convert4Byte(UC8Bytes* utf8) const {
  const int kMinimumRange = 0x000010000;
  auto c = *begin_;
  if (utf8::IsNotNull(c)) {
    (*utf8)[0] = c;
    auto next = unicode::Mask<3>(c) << 18;
    c = *(begin_ + 1);
    if (utf8::IsValidSequence(c)) {
      (*utf8)[1] = c;
      next = next | unicode::Mask<6>(c) << 12;
      c = *(begin_ + 2);
      if (utf8::IsValidSequence(c)) {
        (*utf8)[2] = c;
        next = next | unicode::Mask<6>(c) << 6;
        c = *(begin_ + 3);
        if (utf8::IsValidSequence(c)) {
          next = next | unicode::Mask<6>(c);
          if (next >= kMinimumRange && next <= 0x10FFFF) {
            (*utf8)[3] = c;
            (*utf8)[4] = '\0';
            return next;
          }
        }
      }
    }
  }
  return 0;
}
}

#endif
