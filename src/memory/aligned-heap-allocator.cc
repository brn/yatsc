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


#include <exception>
#include "../utils/utils.h"
#include "./aligned-heap-allocator.h"
#include "./virtual-heap-allocator.h"


static const size_t kAllocateAlignment = yatsc::kAlignment;
static const int kMaxRetry = 10;

namespace yatsc {

// Allocate aligned memory block.
// In this function, we allocate the bigger memory block than specified and roundup head block.
// To allocate the aligned memory block, we try to reserve bigger memory block
// and remove this block after get address of the target memory blocks,
// after we can get target memory block address, roundup this address and recommit this block,
// but, this memory block is already removed and other process may be already used,
// so we retry reserve another memory block until kMaxRetry count.
void* AlignedHeapAllocator::Allocate(size_t size, size_t alignment) {
  if (alignment <= kAllocateAlignment) {
    // If kAllocatedAlignment is enough.
    return VirtualHeapAllocator::Map(nullptr, size,
                                     VirtualHeapAllocator::Prot::READ | VirtualHeapAllocator::Prot::WRITE,
                                     VirtualHeapAllocator::Flags::ANONYMOUS | VirtualHeapAllocator::Flags::PRIVATE);
  } else {
    // If kAllocateAlignment is not enough alignment.
    Byte* allocatedAddress = nullptr;
    Byte* alignedAddress;
    size_t allocatedSize;
    void* ret = nullptr;
    size_t roundup = 0;
    size_t unused = 0;
    int retry = 0;

    // Reserve extra memory to roundup the head address.
    allocatedSize = size + (alignment - kAllocateAlignment);
    
    while (ret == nullptr) {
      retry++;

      if (retry > kMaxRetry) {
        throw new std::bad_alloc();
      }

      // First allocation, we reserve target range memory block.
      // [--roundup---|----------target-----------|---unused---]
      allocatedAddress = reinterpret_cast<Byte*>(VirtualHeapAllocator::Map(
          nullptr, allocatedSize,
          VirtualHeapAllocator::Prot::NONE,
          VirtualHeapAllocator::Flags::ANONYMOUS | VirtualHeapAllocator::Flags::PRIVATE));
      
      if (allocatedAddress == nullptr) {
        continue;
      }

      // Remove reserved block.
      VirtualHeapAllocator::Unmap(allocatedAddress, allocatedSize);

      // Roundup allocatedAddress with the multiples of alignment.
      alignment--;
      alignedAddress = reinterpret_cast<Byte*>((reinterpret_cast<uintptr_t>(allocatedAddress + alignment)) & ~alignment);

      // The roundup value of head addr.
      roundup = static_cast<uintptr_t>(alignedAddress - allocatedAddress);

      // Restore alignment.
      alignment++;
      // The 'unused' is tail unused memory block.
      unused = alignment - kAllocateAlignment - roundup;

      // Commit found memory block again.
      // But,this commit may be failed,
      // because other process may use this block.
      ret = VirtualHeapAllocator::Map(alignedAddress, size - roundup - unused,
                                      VirtualHeapAllocator::Prot::READ | VirtualHeapAllocator::Prot::WRITE,
                                      VirtualHeapAllocator::Flags::ANONYMOUS | VirtualHeapAllocator::Flags::PRIVATE);

      // Check commited memory block address is less than the alignedAddress.
      if (ret != nullptr &&
          reinterpret_cast<uintptr_t>(ret) > reinterpret_cast<uintptr_t>(alignedAddress)) {
        VirtualHeapAllocator::Unmap(ret, size - roundup - unused);
        // TODO Really continue?
        continue;
      }
    }
    
    return alignedAddress;
  }
}


void AlignedHeapAllocator::Deallocate(void* addr, size_t size) {
  VirtualHeapAllocator::Unmap(addr, size);
}

}
