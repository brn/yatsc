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

#ifndef PARSER_PARSER_BASE_H
#define PARSER_PARSER_BASE_H

#include "../ir/node.h"
#include "../ir/irfactory.h"
#include "../parser/scanner.h"
#include "../utils/mmap.h"
#include "./parser-util.h"


namespace yatsc {
template <typename UCharInputSourceIterator>
class ParserBase: private Uncopyable, private Unmovable {
 public:
  typedef size_t TokenCursor;
  
  /**
   * @param scanner Scanner instance that is not consume token yet.
   * @param error_reporter ErrorReporter instance to use report parse error.
   */
  ParserBase(Scanner<UCharInputSourceIterator>* scanner, ErrorReporter* error_reporter)
      : print_parser_phase_(false),
        scanner_(scanner),
        current_token_info_(nullptr),
        next_token_info_(nullptr),
        error_reporter_(error_reporter) {Next();}


 VISIBLE_FOR_TESTING:
  
  /**
   * Enable printing current parser phase.
   */
  void EnablePrintParsePhase() {print_parser_phase_ = true;};

  /**
   * Disable printing current parser phase.
   */
  void DisablePrintParsePhase() {print_parser_phase_ = false;};


 protected:
  
  /**
   * Return a next TokenInfo.
   * @return Next TokenInfo.
   */
  YATSC_INLINE TokenInfo* Next();


  /**
   * Return current TokenInfo.
   * @return Current TokenInfo.
   */
  YATSC_INLINE TokenInfo* Current();


  /**
   * Append TokenInfo to the TokenBuffer.
   * @param token_info Newly appended TokenInfo.
   */
  YATSC_INLINE void PushBackBuffer(TokenInfo* token_info);
  

  /**
   * Rewind TokenBuffer cursor position by the passed position.
   * @param num Rewind position.
   */
  YATSC_INLINE void RewindBuffer(size_t num);


  /**
   * Peek forward TokenInfo.
   * @param Forward position.
   */
  YATSC_INLINE TokenInfo* PeekBuffer(size_t num = 1);


  /**
   * Return current TokenBuffer cursor position.
   * @returns Current TokenBuffer cursor position.
   */
  YATSC_INLINE TokenCursor GetBufferCursorPosition() YATSC_NOEXCEPT;


  /**
   * Set TokenBuffer cursor position.
   * @param cursor New TokenBuffer cursor position.
   */
  YATSC_INLINE void SetBufferCursorPosition(TokenCursor cursor) YATSC_NOEXCEPT;

  
  template <typename T, typename ... Args>
  T* New(Args ... args) {
    return irfactory_.New<T>(std::forward<Args>(args)...);
  }


  template <typename T>
  T* New(std::initializer_list<ir::Node*> list) {
    return irfactory_.New<T>(list);
  }


  bool CheckLineTermination(TokenInfo* info = nullptr);
  

  /**
   * The buffer of the tokens.
   */
  class TokenBuffer: private Uncopyable, private Unmovable {
   public:
    
    TokenBuffer()
        : cursor_(0) {
      // Initialize std::vector for mmap allocator.
      buffer_ = buffer_init_once_(Mmap::MmapStandardAllocator<TokenInfo>(&mmap_));
    }


    /**
     * Append a token to the buffer.
     * @param token_info A new token.
     */
    YATSC_INLINE void PushBack(TokenInfo* token_info);


    /**
     * Check whether buffer is empty or not.
     * @returns true if buffer is empty.
     */
    YATSC_INLINE bool IsEmpty();


    /**
     * Return current token and advance cursor.
     * @returns Current token.
     */
    YATSC_INLINE TokenInfo* Next();


    /**
     * Retrun current token.
     * @returns Current token.
     */
    YATSC_INLINE TokenInfo* Current();


    /**
     * Peek next token.
     * @returns Next token.
     */
    YATSC_INLINE TokenInfo* Peek() {
      return Peek(cursor_ + 1);
    }


    /**
     * Rewind cursor with passed number.
     * @returns Next token.
     */
    YATSC_INLINE TokenInfo* Rewind(size_t num) {
      return Peek(cursor_ - num);
    }


    /**
     * Peek specified index token.
     * @param pos The position.
     */
    YATSC_INLINE TokenInfo* Peek(size_t pos);


    /**
     * Set current token position.
     * @param pos New cursor position.
     */
    YATSC_INLINE void SetCursorPosition(TokenCursor pos) YATSC_NOEXCEPT {
      ASSERT(pos >= 0u, true);
      cursor_ = pos;
    }


    /**
     * Return current cursor position.
     * @returns Current cursor position.
     */
    YATSC_INLINE TokenCursor cursor() YATSC_NOEXCEPT {
      return cursor_;
    }


    /**
     * Check whether cursor position is end of buffer or not.
     * @returns true if cursor position is end of buffer, otherwise false.
     */
    YATSC_INLINE bool CursorUpdated();

   private:
    typedef std::vector<TokenInfo, Mmap::MmapStandardAllocator<TokenInfo>> Vector;
    Mmap mmap_;
    TokenCursor cursor_;
    Vector* buffer_;
    LazyInitializer<Vector> buffer_init_once_;
    TokenInfo null_token_info_;
  };
  

  bool print_parser_phase_;
  Scanner<UCharInputSourceIterator>* scanner_;
  TokenInfo* current_token_info_;
  TokenInfo* next_token_info_;
  ir::IRFactory irfactory_;
  ErrorReporter* error_reporter_;
  TokenBuffer token_buffer_;

#ifdef DEBUG
  std::string indent_;
#endif
};

  // Syntax error exception.
class SyntaxError: public std::exception {
 public:
  SyntaxError(const std::string& message)
      : std::exception(),
        message_(message) {}

  
  /**
   * Return the reason of the error.
   * @returns The error message.
   */
  const char* what() const throw() {return message_.c_str();}
  
 private:
  std::string message_;
};


// Arrow function parameters exception.
class ArrowParametersError: public std::exception {
 public:
  ArrowParametersError(const std::string& message)
      : std::exception(),
        message_(message) {}

  
  /**
   * Return the reason of the error.
   * @returns The error message.
   */
  const char* what() const throw() {return message_.c_str();}
  
 private:
  std::string message_;
};
  
} // yatsc

#include "./parser-base-inl.h"

#endif // PARSER_PARSER_BASE_H
