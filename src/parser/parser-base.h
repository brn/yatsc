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

#ifndef YATSC_PARSER_PARSER_BASE_H
#define YATSC_PARSER_PARSER_BASE_H

#include "../ir/node.h"
#include "../ir/irfactory.h"
#include "../ir/symbol.h"
#include "../parser/scanner.h"
#include "../compiler-option.h"
#include "../utils/utils.h"
#include "../utils/stl.h"
#include "../utils/notificator.h"
#include "../compiler/module-info.h"


namespace yatsc {


class ErrorDescriptor {
 public:
  ErrorDescriptor(SourcePosition source_position)
      : source_position_(source_position) {}

    
  ErrorDescriptor(ErrorDescriptor&& error_descriptor)
      : source_position_(std::move(error_descriptor.source_position_)),
        error_message_(std::move(error_descriptor.error_message_)) {}


  template <typename T>
  ErrorDescriptor& operator << (T&& message) {
    error_message_ << message;
    return *this;
  }


  YATSC_CONST_GETTER(const SourcePosition&, source_position, source_position_);
    

 private:
  SourcePosition source_position_;
  StringStream error_message_;
};


class ParserBase: private Uncopyable, private Unmovable {
 public:
  
  /**
   * @param scanner Scanner instance that is not consume token yet.
   * @param error_reporter ErrorReporter instance to use report parse error.
   */
  ParserBase(const CompilerOption& co, const Notificator<void(Handle<ModuleInfo>)>& notificator, ErrorReporter* error_reporter)
      : compiler_option_(co),
        notificator_(notificator),
        current_token_info_(nullptr),
        error_reporter_(error_reporter) {}


 protected:

  typedef Vector<Handle<ErrorDescriptor>> ErrorBuffer;

  
  template <typename T, typename ... Args>
  Handle<T> New(Args ... args) {
    return irfactory_.New<T>(std::forward<Args>(args)...);
  }


  template <typename T>
  Handle<T> New(std::initializer_list<Handle<ir::Node>> list) {
    return irfactory_.New<T>(list);
  }


  YATSC_INLINE Handle<ir::Symbol> NewSymbol(ir::SymbolType type, const Literal* value) {
    return Heap::NewHandle<ir::Symbol>(type, value);
  }

    

  template <typename ... Args>
  void Notify(const char* key, Args ... args) {
    notificator_.NotifyForKey(key, std::forward<Args>(args)...);
  }


  bool CheckLineTermination(TokenInfo* info = nullptr);


  // Add new error to the error message array.
  void AddErrorMessage(const String& message);


  // Return true if error has occurred.
  bool HasPendingError() YATSC_NO_SE {return !errors_.empty()};


  // Create a new handled ErrorDescript instance.
  Handle<ErrorDescriptor> SyntaxError(const SourcePosition& source_position) {
    auto ret = Heap::NewHandle<ErrorDescriptor>(source_position);
    ret << "SyntaxError ";
    errors_.push_back(ret);
    return ret;
  }


  Handle<ErrorDescriptor> SyntaxError(TokenInfo* info) {
    return SyntaxError(info->source_position());
  }


  Handle<ErrorDescriptor> SyntaxError(const TokenInfo& info) {
    return SyntaxError(info.source_position());
  }


  Handle<ErrorDescriptor> SyntaxError(Handle<ir::Node> node) {
    return SyntaxError(node->source_position());
  }
  
  
  const CompilerOption& compiler_option_;
  const Notificator<void(Handle<ModuleInfo>)>& notificator_;
  TokenInfo* current_token_info_;
  TokenInfo prev_token_info_;
  ir::IRFactory irfactory_;
  ErrorReporter* error_reporter_;
  ErrorBuffer errors_;

#ifdef DEBUG
  String indent_;
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

#endif // PARSER_PARSER_BASE_H
