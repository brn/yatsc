// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <atomic>
#include "tls.h"


namespace yatsc {
// In order to make TLS destructors work, we need to keep around a function
// pointer to the destructor for each slot. We keep this array of pointers in a
// global (static) array.
// We use the single OS-level TLS slot (giving us one pointer per thread) to
// hold a pointer to a per-thread array (table) of slots that we allocate to
// Chromium consumers.

// g_native_tls_key is the one native TLS that we use.  It stores our table.
std::atomic_uint_fast32_t g_native_tls_key(static_cast<uint32_t>(PlatformThreadLocalStorage::TLS_KEY_OUT_OF_INDEXES));

// g_last_used_tls_key is the high-water-mark of allocated thread local storage.
// Each allocation is an index into our g_tls_destructors[].  Each such index is
// assigned to the instance variable slot_ in a ThreadLocalStorage::Slot
// instance.  We reserve the value slot_ == 0 to indicate that the corresponding
// instance of ThreadLocalStorage::Slot has been freed (i.e., destructor called,
// etc.).  This reserved use of 0 is then stated as the initial value of
// g_last_used_tls_key, so that the first issued index will be 1.
std::atomic_uint_fast32_t g_last_used_tls_key(0);

// The maximum number of 'slots' in our thread local storage stack.
const int kThreadLocalStorageSize = 64;

// The maximum number of times to try to clear slots by calling destructors.
// Use pthread naming convention for clarity.
const int kMaxDestructorIterations = kThreadLocalStorageSize;

// An array of destructor function pointers for the slots.  If a slot has a
// destructor, it will be stored in its corresponding entry in this array.
// The elements are volatile to ensure that when the compiler reads the value
// to potentially call the destructor, it does so once, and that value is tested
// for null-ness and then used. Yes, that would be a weird de-optimization,
// but I can imagine some register machines where it was just as easy to
// re-fetch an array element, and I want to be sure a call to free the key
// (i.e., null out the destructor entry) that happens on a separate thread can't
// hurt the racy calls to the destructors on another thread.
volatile ThreadLocalStorage::TLSDestructorFunc g_tls_destructors[kThreadLocalStorageSize];

// This function is called to initialize our entire Chromium TLS system.
// It may be called very early, and we need to complete most all of the setup
// (initialization) before calling *any* memory allocator functions, which may
// recursively depend on this initialization.
// As a result, we use Atomics, and avoid anything (like a singleton) that might
// require memory allocations.
void** ConstructTlsVector() {
  PlatformThreadLocalStorage::TLSKey key = g_native_tls_key.load(std::memory_order_relaxed);
  
  if (key == PlatformThreadLocalStorage::TLS_KEY_OUT_OF_INDEXES) {
    YATSC_CHECK(true, PlatformThreadLocalStorage::AllocTLS(&key));

    // The TLS_KEY_OUT_OF_INDEXES is used to find out whether the key is set or
    // not in NoBarrier_CompareAndSwap, but Posix doesn't have invalid key, we
    // define an almost impossible value be it.
    // If we really get TLS_KEY_OUT_OF_INDEXES as value of key, just alloc
    // another TLS slot.
    if (key == PlatformThreadLocalStorage::TLS_KEY_OUT_OF_INDEXES) {
      PlatformThreadLocalStorage::TLSKey tmp = key;
      YATSC_CHECK(true, PlatformThreadLocalStorage::AllocTLS(&key) &&
                key != PlatformThreadLocalStorage::TLS_KEY_OUT_OF_INDEXES);
      PlatformThreadLocalStorage::FreeTLS(tmp);
    }

    uint_fast32_t out_of_index = static_cast<uint32_t>(PlatformThreadLocalStorage::TLS_KEY_OUT_OF_INDEXES);
    // Atomically test-and-set the tls_key.  If the key is
    // TLS_KEY_OUT_OF_INDEXES, go ahead and set it.  Otherwise, do nothing, as
    // another thread already did our dirty work.
    if (g_native_tls_key.compare_exchange_weak(out_of_index, key) &&
        out_of_index != PlatformThreadLocalStorage::TLS_KEY_OUT_OF_INDEXES) {
      // We've been shortcut. Another thread replaced g_native_tls_key first so
      // we need to destroy our index and use the one the other thread got
      // first.
      PlatformThreadLocalStorage::FreeTLS(key);
      key = g_native_tls_key.load(std::memory_order_relaxed);
    }
  }
  YATSC_CHECK(true, !PlatformThreadLocalStorage::GetTLSValue(key));

  // Some allocators, such as TCMalloc, make use of thread local storage.
  // As a result, any attempt to call new (or malloc) will lazily cause such a
  // system to initialize, which will include registering for a TLS key.  If we
  // are not careful here, then that request to create a key will call new back,
  // and we'll have an infinite loop.  We avoid that as follows:
  // Use a stack allocated vector, so that we don't have dependence on our
  // allocator until our service is in place.  (i.e., don't even call new until
  // after we're setup)
  void* stack_allocated_tls_data[kThreadLocalStorageSize];
  memset(stack_allocated_tls_data, 0, sizeof(stack_allocated_tls_data));
  // Ensure that any rentrant calls change the temp version.
  PlatformThreadLocalStorage::SetTLSValue(key, stack_allocated_tls_data);

  // Allocate an array to store our data.
  void** tls_data = new void*[kThreadLocalStorageSize];
  memcpy(tls_data, stack_allocated_tls_data, sizeof(stack_allocated_tls_data));
  PlatformThreadLocalStorage::SetTLSValue(key, tls_data);
  return tls_data;
}


void OnThreadExitInternal(void* value) {
  ASSERT(true, value != nullptr);
  void** tls_data = static_cast<void**>(value);
  // Some allocators, such as TCMalloc, use TLS.  As a result, when a thread
  // terminates, one of the destructor calls we make may be to shut down an
  // allocator.  We have to be careful that after we've shutdown all of the
  // known destructors (perchance including an allocator), that we don't call
  // the allocator and cause it to resurrect itself (with no possibly destructor
  // call to follow).  We handle this problem as follows:
  // Switch to using a stack allocated vector, so that we don't have dependence
  // on our allocator after we have called all g_tls_destructors.  (i.e., don't
  // even call delete[] after we're done with destructors.)
  void* stack_allocated_tls_data[kThreadLocalStorageSize];
  memcpy(stack_allocated_tls_data, tls_data, sizeof(stack_allocated_tls_data));
  // Ensure that any re-entrant calls change the temp version.
  PlatformThreadLocalStorage::TLSKey key = g_native_tls_key.load(std::memory_order_relaxed);
  PlatformThreadLocalStorage::SetTLSValue(key, stack_allocated_tls_data);
  delete[] tls_data;  // Our last dependence on an allocator.

  int remaining_attempts = kMaxDestructorIterations;
  bool need_to_scan_destructors = true;
  while (need_to_scan_destructors) {
    need_to_scan_destructors = false;
    // Try to destroy the first-created-slot (which is slot 1) in our last
    // destructor call.  That user was able to function, and define a slot with
    // no other services running, so perhaps it is a basic service (like an
    // allocator) and should also be destroyed last.  If we get the order wrong,
    // then we'll itterate several more times, so it is really not that
    // critical (but it might help).
    std::atomic_uint_fast32_t last_used_tls_key(g_last_used_tls_key.load(std::memory_order_relaxed));
    for (int slot = last_used_tls_key; slot > 0; --slot) {
      void* value = stack_allocated_tls_data[slot];
      if (value == nullptr)
        continue;

      ThreadLocalStorage::TLSDestructorFunc destructor =
          g_tls_destructors[slot];
      if (destructor == nullptr)
        continue;
      stack_allocated_tls_data[slot] = nullptr;  // pre-clear the slot.
      destructor(value);
      // Any destructor might have called a different service, which then set
      // a different slot to a non-NULL value.  Hence we need to check
      // the whole vector again.  This is a pthread standard.
      need_to_scan_destructors = true;
    }
    if (--remaining_attempts <= 0) {
      break;
    }
  }

  // Remove our stack allocated vector.
  PlatformThreadLocalStorage::SetTLSValue(key, nullptr);
}

#if defined(PLATFORM_WIN)
void PlatformThreadLocalStorage::OnThreadExit() {
  PlatformThreadLocalStorage::TLSKey key = g_native_tls_key.load(std::memory_order_relaxed);
  if (key == PlatformThreadLocalStorage::TLS_KEY_OUT_OF_INDEXES)
    return;
  void *tls_data = GetTLSValue(key);
  // Maybe we have never initialized TLS for this thread.
  if (!tls_data)
    return;
  OnThreadExitInternal(tls_data);
}
#elif defined(PLATFORM_POSIX)
void PlatformThreadLocalStorage::OnThreadExit(void* value) {
  OnThreadExitInternal(value);
}
#endif  // defined(OS_WIN)


ThreadLocalStorage::Slot::Slot(TLSDestructorFunc destructor) {
  initialized_ = false;
  slot_ = 0;
  Initialize(destructor);
}

bool ThreadLocalStorage::StaticSlot::Initialize(TLSDestructorFunc destructor) {
  PlatformThreadLocalStorage::TLSKey key = g_native_tls_key.load(std::memory_order_relaxed);
  if (key == PlatformThreadLocalStorage::TLS_KEY_OUT_OF_INDEXES ||
      !PlatformThreadLocalStorage::GetTLSValue(key))
    ConstructTlsVector();

  // Grab a new slot.
  g_last_used_tls_key.fetch_add(1);
  slot_ = g_last_used_tls_key.load(std::memory_order_relaxed);
  ASSERT(true, slot_ > 0);
  ASSERT(true, slot_ < kThreadLocalStorageSize);

  // Setup our destructor.
  g_tls_destructors[slot_] = destructor;
  initialized_ = true;
  return true;
}

void ThreadLocalStorage::StaticSlot::Free() {
  // At this time, we don't reclaim old indices for TLS slots.
  // So all we need to do is wipe the destructor.
  ASSERT(true, slot_ > 0);
  ASSERT(true, slot_ < kThreadLocalStorageSize);
  g_tls_destructors[slot_] = nullptr;
  slot_ = 0;
  initialized_ = false;
}

void* ThreadLocalStorage::StaticSlot::Get() const {
  void** tls_data = static_cast<void**>(
      PlatformThreadLocalStorage::GetTLSValue(g_native_tls_key.load(std::memory_order_relaxed)));
  if (!tls_data)
    tls_data = ConstructTlsVector();
  ASSERT(true, slot_ > 0);
  ASSERT(true, slot_ < kThreadLocalStorageSize);
  return tls_data[slot_];
}


void ThreadLocalStorage::StaticSlot::Set(void* value) {
  void** tls_data = static_cast<void**>(
      PlatformThreadLocalStorage::GetTLSValue(g_native_tls_key.load(std::memory_order_relaxed)));
  if (!tls_data)
    tls_data = ConstructTlsVector();
  ASSERT(true, slot_ > 0);
  ASSERT(true, slot_ < kThreadLocalStorageSize);
  tls_data[slot_] = value;
}

}  // namespace yatsc


#ifdef PLATFORM_WIN

namespace {
// Thread Termination Callbacks.
// Windows doesn't support a per-thread destructor with its
// TLS primitives.  So, we build it manually by inserting a
// function to be called on each thread's exit.
// This magic is from http://www.codeproject.com/threads/tls.asp
// and it works for VC++ 7.0 and later.

// Force a reference to _tls_used to make the linker create the TLS directory
// if it's not already there.  (e.g. if __declspec(thread) is not used).
// Force a reference to p_thread_callback_base to prevent whole program
// optimization from discarding the variable.
#ifdef _WIN64

#pragma comment(linker, "/INCLUDE:_tls_used")
#pragma comment(linker, "/INCLUDE:p_thread_callback_base")

#else  // _WIN64

#pragma comment(linker, "/INCLUDE:__tls_used")
#pragma comment(linker, "/INCLUDE:_p_thread_callback_base")

#endif  // _WIN64

// Static callback function to call with each thread termination.
void NTAPI OnThreadExit(PVOID, DWORD reason, PVOID) {
  // On XP SP0 & SP1, the DLL_PROCESS_ATTACH is never seen. It is sent on SP2+
  // and on W2K and W2K3. So don't assume it is sent.
  if (DLL_THREAD_DETACH == reason || DLL_PROCESS_DETACH == reason)
    yatsc::PlatformThreadLocalStorage::OnThreadExit();
}
} // namespace

// .CRT$XLA to .CRT$XLZ is an array of PIMAGE_TLS_CALLBACK pointers that are
// called automatically by the OS loader code (not the CRT) when the module is
// loaded and on thread creation. They are NOT called if the module has been
// loaded by a LoadLibrary() call. It must have implicitly been loaded at
// process startup.
// By implicitly loaded, I mean that it is directly referenced by the main EXE
// or by one of its dependent DLLs. Delay-loaded DLL doesn't count as being
// implicitly loaded.
//
// See VC\crt\src\tlssup.c for reference.

// extern "C" suppresses C++ name mangling so we know the symbol name for the
// linker /INCLUDE:symbol pragma above.
extern "C" {
// The linker must not discard p_thread_callback_base.  (We force a reference
// to this variable with a linker /INCLUDE:symbol pragma to ensure that.) If
// this variable is discarded, the OnThreadExit function will never be called.
#ifdef _WIN64

// .CRT section is merged with .rdata on x64 so it must be constant data.
#pragma const_seg(".CRT$XLB")
// When defining a const variable, it must have external linkage to be sure the
// linker doesn't discard it.
extern const PIMAGE_TLS_CALLBACK p_thread_callback_base;
const PIMAGE_TLS_CALLBACK p_thread_callback_base = OnThreadExit;

// Reset the default section.
#pragma const_seg()

#else  // _WIN64

#pragma data_seg(".CRT$XLB")
PIMAGE_TLS_CALLBACK p_thread_callback_base = OnThreadExit;

// Reset the default section.
#pragma data_seg()

#endif  // _WIN64
}  // extern "C"

#endif
