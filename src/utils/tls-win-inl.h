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

// Borrowed from http://src.chromium.org/chrome/trunk/src/base/threading/thread_local_storage_win.cc

#ifndef UTILS_TLS_WIN_INL_H_
#define UTILS_TLS_WIN_INL_H_

#include <windows.h>
#include "tls.h"

namespace rasp {

bool PlatformThreadLocalStorage::AllocTLS(TLSKey* key) {
  TLSKey value = TlsAlloc();
  if (value != TLS_OUT_OF_INDEXES) {
    *key = value;
    return true;
  }
  return false;
}

void PlatformThreadLocalStorage::FreeTLS(TLSKey key) {
  BOOL ret = TlsFree(key);
  ASSERT(true, !!ret);
}

void* PlatformThreadLocalStorage::GetTLSValue(TLSKey key) {
  return TlsGetValue(key);
}

void PlatformThreadLocalStorage::SetTLSValue(TLSKey key, void* value) {
  BOOL ret = TlsSetValue(key, value);
  ASSERT(true, !!ret);
}

}  // namespace rasp

#endif // UTILS_TLS_WIN_INL_H_
