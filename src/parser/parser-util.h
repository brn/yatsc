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

#ifndef PARSER_PARSER_UTIL_H
#define PARSER_PARSER_UTIL_H

namespace yatsc {
// Generate SyntaxError and throw it.
// Usage. SYNTAX_ERROR("test " << message, Current())
#define PARSER_SYNTAX_ERROR(message, token)                   \
  PARSER_SYNTAX_ERROR_POS(message, token->source_position())


// Generate ArrowParametersError and throw it.
// Usage. ARROW_PARAMETERS_ERROR("test " << message, Current())
#define PARSER_ARROW_PARAMETERS_ERROR(message, token)                   \
  PARSER_ARROW_PARAMETERS_ERROR_POS(message, token->source_position())


// Generate SyntaxError that is pointed specified position and throw it.
// Usage. SYNTAX_ERROR_POS("test " << message, node->source_position())
#define PARSER_SYNTAX_ERROR_POS(message, pos)       \
  PARSER_SYNTAX_ERROR__(message, pos, SyntaxError)


// Generate ArrowParametersError that is pointed specified position and throw it.
// Usage. ARROW_PARAMETERS_ERROR_POS("test " << message, node->source_position())
#define PARSER_ARROW_PARAMETERS_ERROR_POS(message, pos)     \
  PARSER_SYNTAX_ERROR__(message, pos, ArrowParametersError)



#ifndef DEBUG
// Throw error and return nullptr.
#define PARSER_SYNTAX_ERROR__(message, pos, error)  \
  (*error_reporter_) << message;                    \
  error_reporter_->Throw<error>(pos);               \
  return nullptr
#else
// Throw error that has source line and number for the error thrown position.
#define PARSER_SYNTAX_ERROR__(message, pos, error)                      \
  (*error_reporter_) << message << '\n' << __FILE__ << ":" << __LINE__; \
  error_reporter_->Throw<error>(pos);                                   \
  return nullptr
#endif


#ifdef DEBUG
// Logging current parse phase.
#define PARSER_LOG_PHASE(name)                                          \
  if (print_parser_phase_) {                                            \
    if (Current() != nullptr) {                                         \
      Printf("%sEnter %s: CurrentToken = %s\n", indent_.c_str(), #name, Current()->ToString()); \
    } else {                                                            \
      Printf("%sEnter %s: CurrentToken = null\n", indent_.c_str(), #name); \
    }                                                                   \
  }                                                                     \
  indent_ += "  ";                                                      \
  YATSC_SCOPED([&]{                                                     \
    indent_ = indent_.substr(0, indent_.size() - 2);                    \
    if (this->print_parser_phase_) {                                    \
      if (this->Current() != nullptr) {                                 \
        Printf("%sExit %s: CurrentToken = %s\n", indent_.c_str(), #name, Current()->ToString()); \
      } else {                                                          \
        Printf("%sExit %s: CurrentToken = null\n", indent_.c_str(), #name); \
      }                                                                 \
    }                                                                   \
  })
#else
// Disabled.
#define PARSER_LOG_PHASE(name)
#endif

} // yatsc

#endif // PARSER_PARSER_UTIL_H
