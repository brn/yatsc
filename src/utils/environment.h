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


#ifndef ENVIRONMENT_H
#define ENVIRONMENT_H

#include "./tls.h"
#include "../parser/literalbuffer.h"

namespace yatsc {
class Environment : private Uncopyable {
 public:
  static Environment* Create() YATSC_NOEXCEPT {
    Environment* environment = reinterpret_cast<Environment*>(tls_.Get());
    if (environment == nullptr) {
      environment = new Environment();
      tls_.Set(environment);
    }
    return environment;
  }


  YATSC_INLINE LiteralBuffer* literal_buffer() YATSC_NOEXCEPT {
    return &literal_buffer_;
  }

 private:
  Environment() = default;

  LiteralBuffer literal_buffer_;
  
  static ThreadLocalStorage::Slot tls_;
};
}

#endif
