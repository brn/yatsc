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

#ifndef PARSER_UNICODE_ITERATOR_ADAPTER_H_
#define PARSER_UNICODE_ITERATOR_ADAPTER_H_

#include <array>
#include <stdint.h>
#include <iterator>
#include <utility>
#include "../utils/utils.h"
#include "../utils/unicode.h"
#include "./uchar.h"


namespace yatsc {
/**
 * This class is the utf-8 to utf-16 convertor adapter for forward_iterator.
 */
template <typename InputIterator>
class UnicodeIteratorAdapter : public std::iterator<std::forward_iterator_tag, UChar> {
  // We require template parameter InputIterator which satisfy randowm_access_iterator_tag
  //BOOST_CONCEPT_ASSERT((boost::RandomAccessIterator<InputIterator>));
 public:
  
  /**
   * @param begin utf-8 iterator
   */
  UnicodeIteratorAdapter(InputIterator begin);
  UnicodeIteratorAdapter(const UnicodeIteratorAdapter<InputIterator>& un);
  UnicodeIteratorAdapter(UnicodeIteratorAdapter<InputIterator>&& un);
  template <typename T>
  UnicodeIteratorAdapter(const UnicodeIteratorAdapter<T>& un);
  template <typename T>
  UnicodeIteratorAdapter(UnicodeIteratorAdapter<T>&& un);
  ~UnicodeIteratorAdapter() = default;
  YATSC_INLINE UnicodeIteratorAdapter& operator = (InputIterator iter) {begin_ = iter;return *this;}
  YATSC_INLINE UnicodeIteratorAdapter& operator = (const UnicodeIteratorAdapter<InputIterator>& iter) {begin_ = iter.begin_;return *this;}
  YATSC_INLINE UnicodeIteratorAdapter& operator = (UnicodeIteratorAdapter<InputIterator>&& iter) {begin_ = std::move(iter.begin_);return *this;}

  template <typename T>
  YATSC_INLINE UnicodeIteratorAdapter& operator = (const UnicodeIteratorAdapter<T>& iter) {begin_ = iter.begin_;}
  template <typename T>
  YATSC_INLINE UnicodeIteratorAdapter& operator = (UnicodeIteratorAdapter<T>&& iter) {begin_ = std::move(iter.begin_);}
  

  /**
   * Implementation of equality compare to satisfy forward_iterator concept.
   */
  YATSC_INLINE bool operator == (const InputIterator& iter){return begin_ == iter;}


  /**
   * Implementation of equality compare to satisfy forward_iterator concept.
   */
  YATSC_INLINE bool operator != (const InputIterator& iter){return begin_ != iter;}


  /**
   * Implementation of equality compare to satisfy forward_iterator concept.
   */
  YATSC_INLINE bool operator == (const UnicodeIteratorAdapter<InputIterator>& iter){return begin_ == iter.begin_;}


  /**
   * Implementation of equality compare to satisfy forward_iterator concept.
   */
  YATSC_INLINE bool operator != (const UnicodeIteratorAdapter<InputIterator>& iter){return begin_ != iter.begin_;}


  /**
   * Implementation of equality compare to satisfy forward_iterator concept.
   */
  YATSC_INLINE bool operator == (char iter){return *begin_ == iter;}


  /**
   * Implementation of equality compare to satisfy forward_iterator concept.
   */
  YATSC_INLINE bool operator != (char iter){return *begin_ != iter;}

  
  /**
   * Implementation of dereference to satisfy forward_iterator concept.
   */
  YATSC_INLINE UChar operator* () const {return Convert();}


  /**
   * Implementation of increment to satisfy forward_iterator concept.
   */
  YATSC_INLINE UnicodeIteratorAdapter& operator ++() {Advance();return *this;}


  /**
   * Implementation of forward access to satisfy forward_iterator concept.
   */
  inline UnicodeIteratorAdapter& operator +=(int c) {
    while (c--) Advance();
    return *this;
  }


  /**
   * Implementation of add to satisfy forward_iterator concept.
   */
  inline const UnicodeIteratorAdapter operator + (int c) {
    UnicodeIteratorAdapter ua(*this);
    while (c--) ++ua;
    return ua;
  }
  

  /**
   * Return current line number.
   * @return line number
   */
  YATSC_INLINE UC32 line_number() const {return line_number_;}


  /**
   * Return current char cursor position.
   * @return current char position.
   */
  YATSC_INLINE UC32 current_position() const {return current_position_;}


  /**
   * Unwrap iterator.
   * @return wapped iterator.
   */
  YATSC_INLINE const InputIterator& base() const {return begin_;}

  
 private:

  /**
   * Convert curent utf-8 byte sequence to utf-32 byte sequence.
   * @return the utf-32 byte sequence representations.
   */
  UChar Convert() const;


  /**
   * Convert current utf-8 byte sequence to ucs2 code set.
   */
  UC32 ConvertUtf8ToUcs2(size_t byte_count, UC8Bytes* utf8) const;


  /**
   * Advance iterator.
   */
  inline void Advance() {
    UC8 next = *begin_;
    auto byte_count = utf8::GetByteCount(next);
    if (next == '\n') {
      current_position_ = 1;
      line_number_++;
    } else {
      current_position_++;
    }
    std::advance(begin_, byte_count);
  }

  
  /**
   * Convert a utf-8 byte sequence.
   * @param utf8 buffer to reserve utf-8 byte sequence.
   * @return utf-32 byte sequence.
   */
  inline UC32 ConvertAscii(UC8Bytes* utf8) const {
    UC8 uc = *begin_;
    (*utf8)[0] = uc;
    (*utf8)[1] = '\0';
    return unicode::Mask<8>(uc);
  }
  

  /**
   * Convert 2 utf-8 byte sequence.
   * @param utf8 buffer to reserve utf-8 byte sequence.
   * @return utf-32 byte sequence.
   */
  inline UC32 Convert2Byte(UC8Bytes* utf8) const;


  /**
   * Convert 3 utf-8 byte sequence.
   * @param utf8 buffer to reserve utf-8 byte sequence.
   * @return utf-32 byte sequence.
   */
  inline UC32 Convert3Byte(UC8Bytes* utf8) const;


  /**
   * Convert 4 utf-8 byte sequence.
   * @param utf8 buffer to reserve utf-8 byte sequence.
   * @return utf-32 byte sequence.
   */
  inline UC32 Convert4Byte(UC8Bytes* utf8) const;

  
  UC32 current_position_;
  UC32 line_number_;
  InputIterator begin_;
};
}

#include "./unicode-iterator-adapter-inl.h"

#endif
