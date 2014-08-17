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


#ifndef UTILS_UTILS_H_
#define UTILS_UTILS_H_

#include <atomic>
#include <cstdint>
#include <cstring>
#include <functional>
#include <sstream>
#include <string>
#include <new>
#include "os.h"
#include "../config.h"

namespace yatsc {

/**
 * Inline macro.
 */
#if !defined(DEBUG) && defined(HAVE_FORCE_INLINE)
#define YATSC_INLINE inline __forceinline
#elif !defined(DEBUG) && defined(HAVE_INLINE_ATTRIUTE)
#define YATSC_INLINE inline __attribute__((always_inline))
#else
#define YATSC_INLINE inline
#endif

#if defined(HAVE_NOEXCEPT)
#define YATSC_NOEXCEPT noexcept
#else
#define YATSC_NOEXCEPT throw()
#endif

#define YATSC_NO_SE const YATSC_NOEXCEPT


#if defined(HAVE_UNUSED_ATTRIBUTE)
#define YATSC_UNUSED __attribute__((unused))
#else
#define YATSC_UNUSED
#endif

#ifdef __func__
#define FUNCTION_NAME __func__
#elif defined(__FUNCTION__)
#define FUNCTION_NAME __FUNCTION__
#elif defined(__PRETTY_FUNCTION__)
#define FUNCTION_NAME __PRETTY_FUNCTION__
#elif defined(__FUNCDNAME__)
#define FUNCTION_NAME __FUNCDNAME__
#elif defined(__FUNCSIG__)
#define FUNCTION_NAME __FUNCSIG__
#ese
#define FUNCTION_NAME "anonymous()"
#endif


YATSC_INLINE void Assert__(bool ok, const char* result, const char* expect, const char* file, int line, const char* function) {
  if (!ok) {
    FPrintf(stderr, "assertion failed -> %s == %s\n in file %s at line %d\nIn function %s\n",
            result, expect, file, line, function);
    std::terminate();
  }
}


YATSC_INLINE void Fatal__(const char* file, int line, const char* function, const std::string& message) {
  FPrintf(stderr, "Fatal error occured, so process no longer exist.\nin file %s at line %d\n%s\n%s\n",
          file, line, function, message.c_str());
  std::terminate();
}


// ASSERT macro definition.
#ifdef DEBUG
#define ASSERT(expect, result) yatsc::Assert__(result == expect, #result, #expect, __FILE__, __LINE__, FUNCTION_NAME)
#elif defined(NDEBUG) || !defined(DEBUG)
#define ASSERT(expect, result)
#endif


#define YATSC_CHECK(expect, result) yatsc::Assert__(result == expect, #result, #expect, __FILE__, __LINE__, FUNCTION_NAME)

// ASSERT macro definition end.


#define FATAL(msg) {std::stringstream err_stream__;err_stream__ << msg;yatsc::Fatal__(__FILE__, __LINE__, FUNCTION_NAME, err_stream__.str());}

#define UNREACHABLE FATAL("UNREACHABLE CODE ACCESSED.")


typedef uint8_t Byte;
#ifdef PLATFORM_64BIT
typedef uint64_t Pointer;
#elif defined(PLATFORM_32BIT)
typedef uint32_t Pointer;
#endif

static const size_t kAlignment = sizeof(void*);
static const size_t kPointerSize = kAlignment;
static const size_t kSizeTSize = sizeof(size_t);


#define YATSC_ALIGN_OFFSET(offset, alignment)           \
  (offset + (alignment - 1)) & ~(alignment - 1)


#if defined(HAVE_STD_ALIGNMENT_OF)
#define YATSC_ALIGN(offset, type)  YATSC_ALIGN_OFFSET(offset, std::alignment_of<type>::value)
#elif defined(HaVE___ALIGNOF)
#define YATSC_ALIGN(offset, type)  YATSC_ALIGN_OFFSET(offset, __alignof(type))
#endif

#define KB * 1024
#define MB KB * 1024
#define GB MB * 1024


#define YATSC_GETTER(type, name, field)                    \
  YATSC_INLINE type name() YATSC_NOEXCEPT {return field;}

#define YATSC_CONST_GETTER(type, name, field)            \
  YATSC_INLINE type name() YATSC_NO_SE {return field;}


#define YATSC_SETTER(type, name, field)                              \
  YATSC_INLINE void set_##name(type name) YATSC_NOEXCEPT {field = name;}


#define YATSC_PROPERTY(type, name, field)        \
  YATSC_GETTER(type, name, field)                \
  YATSC_SETTER(type, name, field)


#define YATSC_CONST_PROPERTY(type, name, field)        \
  YATSC_CONST_GETTER(type, name, field)                \
  YATSC_SETTER(type, name, field)


#define YATSC_CHECK_CONSTRAINTS(type_var, concept)                      \
  typedef void (concept <type_var>::* func##type_var##concept)();       \
  template <func##type_var##concept Tp1_>                               \
  struct concept_checking_##type_var##concept { };                      \
  typedef concept_checking_##type_var##concept<&concept <type_var>::constraints> \
  concept_checking_typedef_##type_var##concept


template <typename T>
class YatscScoped__ {
 public:
  YatscScoped__(T cb)
      : cb_(cb) {}

  ~YatscScoped__() {
    cb_();
  }

 private:
  T cb_;
};

#define YATSC_SCOPED_INNER__L(exit_function, line) auto exit_function_of_scope##line = exit_function;YatscScoped__<decltype(exit_function_of_scope##line)> yatsc_scoped_once##line(exit_function_of_scope##line);
#define YATSC_SCOPED_INNER__(exit_function, line) YATSC_SCOPED_INNER__L(exit_function, line);
#define YATSC_SCOPED(exit_function) YATSC_SCOPED_INNER__(exit_function, __LINE__)


#ifdef UNIT_TEST
#define VISIBLE_FOR_TESTING public
#else
#define VISIBLE_FOR_TESTING private
#endif



/**
 * Class traits.
 * Represent class which is not allowed to instantiation.
 */
class Static {
  Static() = delete;
  Static(const Static&) = delete;
  Static(Static&&) = delete;
  Static& operator = (const Static&) = delete;
};


/**
 * Class traits.
 * Represent class which is not allowed to copy.
 */
class Uncopyable {
public:
  Uncopyable() = default;
  virtual ~Uncopyable() = default;
  Uncopyable(const Uncopyable&) = delete;
  Uncopyable& operator = (const Uncopyable&) = delete;
};


class Unmovable {
 public:
  Unmovable() = default;
  virtual ~Unmovable() = default;
  Unmovable(Unmovable&&) = delete;
  Unmovable& operator = (Unmovable&&) = delete;
};


/**
 * Bitmask utility.
 * Borrowed from http://d.hatena.ne.jp/tt_clown/20090616/p1
 */
template <int LowerBits, typename Type = uint32_t>
class Bitmask {
 public:
  static const Type full = ~(Type(0));
  static const Type upper = ~((Type(1) << LowerBits) - 1);
  static const Type lower = (Type(1) << LowerBits) - 1;
};


/**
 * Generic strlen.
 */
template <typename T>
YATSC_INLINE size_t Strlen(const T* str) {
  return strlen(static_cast<const char*>(str));
}


template <typename T, bool kInitOnce = true>
class LazyInitializer {
 public:
  LazyInitializer() {lock_.clear();}
  ~LazyInitializer() {
    reinterpret_cast<T*>(heap_)->~T();
  }
  
  template <typename ... Args>
  T* operator()(Args ... args) {
    if (kInitOnce) {
      YATSC_CHECK(false, lock_.test_and_set());
    }
    return new(heap_) T(args...);
  }
 private:
  std::atomic_flag lock_;
  Byte heap_[sizeof(T)];
};


template <typename T, typename U, typename... Ts>
struct is_all_same : 
  std::integral_constant<bool, is_all_same<T, U>::value & is_all_same<U, Ts...>::value>{ };

template <typename T>
struct is_all_same<T, T> : std::true_type { };

template <typename T, typename U>
struct is_all_same<T, U> : std::false_type { };

}

#endif
