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

#ifndef YATSC_COMPILER_OPTION_H_
#define YATSC_COMPILER_OPTION_H_

#include "utils/utils.h"

namespace yatsc {
enum class LanguageMode : uint8_t {
  ES3 = 0,
  ES5_STRICT,
  ES6
};

enum class ModuleType: uint8_t {
  TYPE_SCRIPT = 0,
  ES6
};


class CompilerOption {
 public:
  CompilerOption();
  
  YATSC_CONST_PROPERTY(LanguageMode, language_mode, language_mode_);

  YATSC_CONST_PROPERTY(LanguageMode, output_language_mode, output_language_mode_);
  
  YATSC_CONST_PROPERTY(ModuleType, module_type, module_type_);
  
 private:
  LanguageMode language_mode_;
  LanguageMode output_language_mode_;
  ModuleType module_type_;
};


class LanguageModeUtil : private Static {
 public:

  YATSC_INLINE static bool IsFutureReservedWord(const CompilerOption& co) {
    return co.language_mode() != LanguageMode::ES3;
  }
  
  YATSC_INLINE static bool IsOctalLiteralAllowed(const CompilerOption& co) {
    return co.language_mode() == LanguageMode::ES3;
  }

  
  YATSC_INLINE static bool IsBinaryLiteralAllowed(const CompilerOption& co) {
    return co.language_mode() == LanguageMode::ES6;
  }

  
  YATSC_INLINE static bool IsES6(const CompilerOption& co) {
    return co.language_mode() == LanguageMode::ES6;
  }

  
  YATSC_INLINE static bool IsES5Strict(const CompilerOption& co) {
    return co.language_mode() == LanguageMode::ES5_STRICT;
  }

  
  YATSC_INLINE static bool IsES3(const CompilerOption& co) {
    return co.language_mode() == LanguageMode::ES3;
  }

  YATSC_INLINE static const char* ToString(const CompilerOption& co) {
    switch (co.language_mode()) {
      case LanguageMode::ES3:
        return kEs3;
      case LanguageMode::ES5_STRICT:
        return kEs5Strict;
      case LanguageMode::ES6:
        return kEs6;
      default:
        return kEs3;
    }
  }

 private:
  static const char* kEs3;
  static const char* kEs5Strict;
  static const char* kEs6;
};


class ModuleTypeUtil: private Static {
 public:
  YATSC_INLINE static bool IsModuleKeywordAllowed(const CompilerOption& co) {
    return co.module_type() == ModuleType::TYPE_SCRIPT;
  }
};
}

#endif
