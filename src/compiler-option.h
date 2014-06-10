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

#ifndef COMPILER_OPTION_H_
#define COMPILER_OPTION_H_

#include "utils/utils.h"

namespace rasp {
enum class LanguageMode : uint8_t {
  ES3 = 0,
  ES5_STRICT,
  HARMONY
};


class CompilerOption {
 public:
  CompilerOption();
  RASP_INLINE void set_language_mode(LanguageMode language_mode) {
    language_mode_ = language_mode;
  }
  
  RASP_INLINE LanguageMode language_mode() const {
    return language_mode_;
  }
  
 private:
  LanguageMode language_mode_;
};


class LanguageModeUtil : private Static {
 public:
  RASP_INLINE static bool IsOctalLiteralAllowed(const CompilerOption& co) {
    return co.language_mode() == LanguageMode::ES3;
  }

  
  RASP_INLINE static bool IsBinaryLiteralAllowed(const CompilerOption& co) {
    return co.language_mode() == LanguageMode::HARMONY;
  }

  
  RASP_INLINE static bool IsHarmony(const CompilerOption& co) {
    return co.language_mode() == LanguageMode::HARMONY;
  }

  
  RASP_INLINE static bool IsES5Strict(const CompilerOption& co) {
    return co.language_mode() == LanguageMode::ES5_STRICT;
  }

  
  RASP_INLINE static bool IsES3(const CompilerOption& co) {
    return co.language_mode() == LanguageMode::ES3;
  }
};
}

#endif
