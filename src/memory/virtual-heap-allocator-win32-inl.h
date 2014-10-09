//
// The MIT License (MIT)
//
// Copyright (c) 2013 Taketoshi Aono(brn)
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

#include <windows.h>

namespace yatsc {

// Map virtual memory block with the mmap.
inline void* VirtualHeapAllocator::Map(void* addr, size_t size, uint8_t prot, uint8_t flags, uint8_t type) {
  DWORD fl_protect = 0;
  DWORD fl_allocation_type = 0;

  if (prot != VirtualHeapAllocator::Prot::NONE) {
    if ((prot & 0x7) == 0x7 || (prot & 0x6) == 0x6) {
      fl_protect = PAGE_EXECUTE_READWRITE;
    } else if ((prot & 0x5) == 0x5) {
      fl_protect = PAGE_EXECUTE_READ;
    } else if ((prot & VirtualHeapAllocator::Prot::WRITE) == VirtualHeapAllocator::Prot::WRITE ||
               (prot & 0x3) == 0x3) {
      fl_protect = PAGE_READWRITE;
    } else if ((prot & VirtualHeapAllocator::Prot::READ) == VirtualHeapAllocator::Prot::READ) {
      fl_protect = PAGE_READONLY;
    } else if ((prot & VirtualHeapAllocator::Prot::EXEC) == VirtualHeapAllocator::Prot::EXEC) {
      fl_protect = PAGE_EXECUTE;
    }
  } else {
    fl_protect = PAGE_READONLY;
  }
  
  if (type == Type::COMMIT || flags != VirtualHeapAllocator::Flags::NONE) {
    fl_allocation_type = MEM_COMMIT;
  } else if (type == Type::RESERVE) {
    fl_allocation_type = MEM_RESERVE;
  }

  void* ret = ::VirtualAlloc(addr, size, fl_allocation_type, fl_protect);
  // used_ += size;
  // printf("Req: %lu KB, Used: %llu MB\n", size / 1024, used_.load() / 1024 / 1024);
  
  if (ret == NULL) {
    return nullptr;
  }
  return ret;
}


// Unmap virtual memory block with the munmap.
inline void VirtualHeapAllocator::Unmap(void* addr, size_t size, uint8_t type) {
  used_ -= size;
  if (type == Type::RELEASE) {
    ::VirtualFree(addr, size, MEM_RELEASE);
  } else {
    ::VirtualFree(addr, size, MEM_DECOMMIT);
  }
}

}
