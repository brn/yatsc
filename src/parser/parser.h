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

#ifndef PARSER_PARSER_H
#define PARSER_PARSER_H

#include <memory>
#include "../ir/node.h"
#include "../ir/irfactory.h"
#include "../parser/scanner.h"
#include "../utils/mmap.h"

namespace yatsc {
template <typename UCharInputSourceIterator>
class Parser {
 public:
  Parser(Scanner<UCharInputSourceIterator>* scanner, ErrorReporter* error_reporter)
      : print_parser_phase_(false),
        scanner_(scanner),
        current_token_info_(nullptr),
        next_token_info_(nullptr),
        error_reporter_(error_reporter) {Next();}


  ir::Node* Parse();

 VISIBLE_FOR_TEST:
  ir::Node* ParseProgram();

  ir::Node* ParseSourceElements();

  ir::Node* ParseSourceElement();

  ir::Node* ParseStatementList();

  ir::Node* ParseStatement();

  ir::Node* ParseBlockStatement();

  ir::Node* ParseModuleStatement();

  ir::Node* ParseImportStatement();

  ir::Node* ParseExportStatement();

  ir::Node* ParseDebuggerStatement();

  ir::Node* ParseLexicalDeclaration(bool has_in, bool has_yield);

  ir::Node* ParseLexicalBinding(bool const_decl, bool has_in, bool has_yield);

  ir::Node* ParseBindingPattern(bool has_yield, bool generator_parameter);

  ir::Node* ParseObjectBindingPattern(bool has_yield, bool generator_parameter);

  ir::Node* ParseArrayBindingPattern(bool has_yield, bool generator_parameter);

  ir::Node* ParseBindingProperty(bool has_yield, bool generator_parameter);

  ir::Node* ParseBindingElement(bool has_yield, bool generator_parameter);

  ir::Node* ParseBindingIdentifier(bool default_allowed, bool has_in, bool has_yield);

  ir::Node* ParseVariableStatement(bool has_in, bool has_yield);
  
  ir::Node* ParseVariableDeclaration(bool has_in, bool has_yield);

  ir::Node* ParseIfStatement(bool has_yield);

  ir::Node* ParseWhileStatement(bool has_yield);

  ir::Node* ParseDoWhileStatement(bool has_yield);

  ir::Node* ParseForStatement(bool has_yield);

  ir::Node* ParseForIterationStatement(ir::Node* reciever, TokenInfo* info, bool has_yield);

  ir::Node* ParseContinueStatement();

  ir::Node* ParseBreakStatement();

  ir::Node* ParseReturnStatement(bool has_yield);

  ir::Node* ParseWithStatement(bool has_yield);

  ir::Node* ParseSwitchStatement();

  ir::Node* ParseCaseClauses();

  ir::Node* ParseLabelledStatement();

  ir::Node* ParseThrowStatement();

  ir::Node* ParseTryStatement();

  ir::Node* ParseCatchBlock();

  ir::Node* ParseFinallyBlock();

  ir::Node* ParseClassDeclaration();

  ir::Node* ParseClassFields();

  ir::Node* ParseExpression(bool has_in, bool has_yield);

  ir::Node* ParseAssignmentExpression(bool has_in, bool has_yield);

  ir::Node* ParseYieldExpression();

  ir::Node* ParseConditionalExpression(bool has_in, bool has_yield);

  ir::Node* ParseBinaryExpression(bool has_in, bool has_yield);

  ir::Node* ParseUnaryExpression();

  ir::Node* ParsePostfixExpression();

  ir::Node* ParseLeftHandSideExpression(bool has_in, bool has_yield);

  ir::Node* ParseTypeExpression();

  ir::Node* ParseReferencedType();

  ir::Node* ParseGenericType();

  ir::Node* ParseTypeArguments();

  ir::Node* ParseTypeParameters();

  ir::Node* ParseTypeQueryExpression();

  ir::Node* ParseArrayType(ir::Node* type_expr);

  ir::Node* ParseObjectTypeExpression();

  ir::Node* ParseObjectTypeElement();

  ir::Node* ParseCallExpression();

  ir::Node* ParseArguments();

  ir::Node* ParseParameterList(bool accesslevel_allowed);
  
  ir::Node* ParseParameter(bool rest, bool accesslevel_allowed);

  ir::Node* ParseMemberExpression();

  ir::Node* ParseGetPropOrElem(ir::Node* node);

  ir::Node* ParsePrimaryExpression();

  ir::Node* ParseTypedParameterOrNameExpression();

  ir::Node* ParseObjectLiteral();

  ir::Node* ParseArrayLiteral();

  ir::Node* ParseLiteral();

  ir::Node* ParseFunction();

  ir::Node* ParseArrowFunction(ir::Node* identifier = nullptr);
  
  ir::Node* ParseArrowFunctionParameters(ir::Node* identifier = nullptr);

  ir::Node* ParseArrowFunctionBody(ir::Node* call_sig);

  ir::Node* ParseFunctionBody();

  ir::Node* ParseFormalParameterList();

  ir::Node* ParseCallSignature(bool accesslevel_allowed);

  ir::Node* ParseObjectKey();

  void EnablePrintParsePhase() {print_parser_phase_ = true;};

  void DisablePrintParsePhase() {print_parser_phase_ = false;};
  
 private:

  typedef size_t TokenCursor;

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
    YATSC_INLINE void PushBack(TokenInfo* token_info) {
      buffer_->emplace_back(*token_info);
    }


    /**
     * Check whether buffer is empty or not.
     * @returns true if buffer is empty, otherwise false.
     */
    YATSC_INLINE bool IsEmpty() YATSC_NO_SE {
      return buffer_->empty();
    }


    /**
     * Return current token and advance cursor.
     * @returns Current token.
     */
    YATSC_INLINE TokenInfo* Next() {
      if (cursor_ >= (buffer_->size() - 1)) {return &null_token_info_;}
      return &((*buffer_)[cursor_++]);
    }


    /**
     * Retrun current token.
     * @returns Current token.
     */
    YATSC_INLINE TokenInfo* Current() {
      if (cursor_ >= buffer_->size()) {return &null_token_info_;}
      return &((*buffer_)[cursor_]);
    }


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
    YATSC_INLINE TokenInfo* Peek(size_t pos) {
      if (pos < 0 || pos >= buffer_->size()) {return &null_token_info_;}
      return &((*buffer_)[pos]);
    }


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
    YATSC_INLINE bool CursorUpdated() {
      if (buffer_->empty()) {
        return true;
      }
      return cursor_ == (buffer_->size() - 1);
    }

   private:
    
    typedef std::vector<TokenInfo, Mmap::MmapStandardAllocator<TokenInfo>> Vector;
    Mmap mmap_;
    TokenCursor cursor_;
    Vector* buffer_;
    LazyInitializer<Vector> buffer_init_once_;
    TokenInfo null_token_info_;
  };


  /**
   * Accept '/' as regular expression begging token.
   */
  YATSC_INLINE void AllowRegularExpr() {
    scanner_->AllowRegularExpression();
  }


  /**
   * Deny '/' as regular expression begging token.
   */
  YATSC_INLINE void DisallowRegularExpr() {
    scanner_->DisallowRegularExpression();
  }


  /**
   * Return a next token if token buffer's cursor is in the end of input,
   * otherwise return a current token from the token buffer and advance token buffer cursor.
   */
  YATSC_INLINE TokenInfo* Next() {
    if (!token_buffer_.CursorUpdated()) {
      return token_buffer_.Next();
    }
    current_token_info_ = scanner_->Scan();
    token_buffer_.PushBack(current_token_info_);
    return token_buffer_.Next();
  }


  /**
   * Return current token if token buffer's cursor is in the end of input,
   * otherwise return current token from token buffer.
   */
  YATSC_INLINE TokenInfo* Current() {
    if (!token_buffer_.CursorUpdated()) {
      return token_buffer_.Current();
    }
    return current_token_info_;
  }
  
  
  template <typename T, typename ... Args>
  T* New(Args ... args) {
    return irfactory_.New<T>(std::forward<Args>(args)...);
  }


  template <typename T>
  T* New(std::initializer_list<ir::Node*> list) {
    return irfactory_.New<T>(list);
  }


  bool CheckLineTermination(TokenInfo* info = nullptr);
  

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
}

#include "./parser-inl.h"

#endif
