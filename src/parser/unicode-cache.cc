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

#include "unicode-cache.h"
#include "../utils/unicode.h"

namespace yatsc {

UChar UnicodeCache::ascii_cache_[] = {
  UChar(unicode::u8(unicode::Mask<8>(0x0)), UC8Bytes{{'\0'}}),
  UChar(unicode::u8(unicode::Mask<8>(0x1)), UC8Bytes{{0x1, '\0'}}),
  UChar(unicode::u8(unicode::Mask<8>(0x2)), UC8Bytes{{0x2, '\0'}}),
  UChar(unicode::u8(unicode::Mask<8>(0x3)), UC8Bytes{{0x3, '\0'}}),
  UChar(unicode::u8(unicode::Mask<8>(0x4)), UC8Bytes{{0x4, '\0'}}),
  UChar(unicode::u8(unicode::Mask<8>(0x5)), UC8Bytes{{0x5, '\0'}}),
  UChar(unicode::u8(unicode::Mask<8>(0x6)), UC8Bytes{{0x6, '\0'}}),
  UChar(unicode::u8(unicode::Mask<8>(0x7)), UC8Bytes{{0x7, '\0'}}),
  UChar(unicode::u8(unicode::Mask<8>(0x8)), UC8Bytes{{0x8, '\0'}}),
  UChar(unicode::u8(unicode::Mask<8>(0x9)), UC8Bytes{{0x9, '\0'}}),
  UChar(unicode::u8(unicode::Mask<8>(0xa)), UC8Bytes{{0xa, '\0'}}),
  UChar(unicode::u8(unicode::Mask<8>(0xb)), UC8Bytes{{0xb, '\0'}}),
  UChar(unicode::u8(unicode::Mask<8>(0xc)), UC8Bytes{{0xc, '\0'}}),
  UChar(unicode::u8(unicode::Mask<8>(0xd)), UC8Bytes{{0xd, '\0'}}),
  UChar(unicode::u8(unicode::Mask<8>(0xe)), UC8Bytes{{0xe, '\0'}}),
  UChar(unicode::u8(unicode::Mask<8>(0xf)), UC8Bytes{{0xf, '\0'}}),
  UChar(unicode::u8(unicode::Mask<8>(0x10)), UC8Bytes{{0x10, '\0'}}),
  UChar(unicode::u8(unicode::Mask<8>(0x11)), UC8Bytes{{0x11, '\0'}}),
  UChar(unicode::u8(unicode::Mask<8>(0x12)), UC8Bytes{{0x12, '\0'}}),
  UChar(unicode::u8(unicode::Mask<8>(0x13)), UC8Bytes{{0x13, '\0'}}),
  UChar(unicode::u8(unicode::Mask<8>(0x14)), UC8Bytes{{0x14, '\0'}}),
  UChar(unicode::u8(unicode::Mask<8>(0x15)), UC8Bytes{{0x15, '\0'}}),
  UChar(unicode::u8(unicode::Mask<8>(0x16)), UC8Bytes{{0x16, '\0'}}),
  UChar(unicode::u8(unicode::Mask<8>(0x17)), UC8Bytes{{0x17, '\0'}}),
  UChar(unicode::u8(unicode::Mask<8>(0x18)), UC8Bytes{{0x18, '\0'}}),
  UChar(unicode::u8(unicode::Mask<8>(0x19)), UC8Bytes{{0x19, '\0'}}),
  UChar(unicode::u8(unicode::Mask<8>(0x1a)), UC8Bytes{{0x1a, '\0'}}),
  UChar(unicode::u8(unicode::Mask<8>(0x1b)), UC8Bytes{{0x1b, '\0'}}),
  UChar(unicode::u8(unicode::Mask<8>(0x1c)), UC8Bytes{{0x1c, '\0'}}),
  UChar(unicode::u8(unicode::Mask<8>(0x1d)), UC8Bytes{{0x1d, '\0'}}),
  UChar(unicode::u8(unicode::Mask<8>(0x1e)), UC8Bytes{{0x1e, '\0'}}),
  UChar(unicode::u8(unicode::Mask<8>(0x1f)), UC8Bytes{{0x1f, '\0'}}),
  UChar(unicode::u8(unicode::Mask<8>(0x20)), UC8Bytes{{0x20, '\0'}}),
  UChar(unicode::u8(unicode::Mask<8>(0x21)), UC8Bytes{{0x21, '\0'}}),
  UChar(unicode::u8(unicode::Mask<8>(0x22)), UC8Bytes{{0x22, '\0'}}),
  UChar(unicode::u8(unicode::Mask<8>(0x23)), UC8Bytes{{0x23, '\0'}}),
  UChar(unicode::u8(unicode::Mask<8>(0x24)), UC8Bytes{{0x24, '\0'}}),
  UChar(unicode::u8(unicode::Mask<8>(0x25)), UC8Bytes{{0x25, '\0'}}),
  UChar(unicode::u8(unicode::Mask<8>(0x26)), UC8Bytes{{0x26, '\0'}}),
  UChar(unicode::u8(unicode::Mask<8>(0x27)), UC8Bytes{{0x27, '\0'}}),
  UChar(unicode::u8(unicode::Mask<8>(0x28)), UC8Bytes{{0x28, '\0'}}),
  UChar(unicode::u8(unicode::Mask<8>(0x29)), UC8Bytes{{0x29, '\0'}}),
  UChar(unicode::u8(unicode::Mask<8>(0x2a)), UC8Bytes{{0x2a, '\0'}}),
  UChar(unicode::u8(unicode::Mask<8>(0x2b)), UC8Bytes{{0x2b, '\0'}}),
  UChar(unicode::u8(unicode::Mask<8>(0x2c)), UC8Bytes{{0x2c, '\0'}}),
  UChar(unicode::u8(unicode::Mask<8>(0x2d)), UC8Bytes{{0x2d, '\0'}}),
  UChar(unicode::u8(unicode::Mask<8>(0x2e)), UC8Bytes{{0x2e, '\0'}}),
  UChar(unicode::u8(unicode::Mask<8>(0x2f)), UC8Bytes{{0x2f, '\0'}}),
  UChar(unicode::u8(unicode::Mask<8>(0x30)), UC8Bytes{{0x30, '\0'}}),
  UChar(unicode::u8(unicode::Mask<8>(0x31)), UC8Bytes{{0x31, '\0'}}),
  UChar(unicode::u8(unicode::Mask<8>(0x32)), UC8Bytes{{0x32, '\0'}}),
  UChar(unicode::u8(unicode::Mask<8>(0x33)), UC8Bytes{{0x33, '\0'}}),
  UChar(unicode::u8(unicode::Mask<8>(0x34)), UC8Bytes{{0x34, '\0'}}),
  UChar(unicode::u8(unicode::Mask<8>(0x35)), UC8Bytes{{0x35, '\0'}}),
  UChar(unicode::u8(unicode::Mask<8>(0x36)), UC8Bytes{{0x36, '\0'}}),
  UChar(unicode::u8(unicode::Mask<8>(0x37)), UC8Bytes{{0x37, '\0'}}),
  UChar(unicode::u8(unicode::Mask<8>(0x38)), UC8Bytes{{0x38, '\0'}}),
  UChar(unicode::u8(unicode::Mask<8>(0x39)), UC8Bytes{{0x39, '\0'}}),
  UChar(unicode::u8(unicode::Mask<8>(0x3a)), UC8Bytes{{0x3a, '\0'}}),
  UChar(unicode::u8(unicode::Mask<8>(0x3b)), UC8Bytes{{0x3b, '\0'}}),
  UChar(unicode::u8(unicode::Mask<8>(0x3c)), UC8Bytes{{0x3c, '\0'}}),
  UChar(unicode::u8(unicode::Mask<8>(0x3d)), UC8Bytes{{0x3d, '\0'}}),
  UChar(unicode::u8(unicode::Mask<8>(0x3e)), UC8Bytes{{0x3e, '\0'}}),
  UChar(unicode::u8(unicode::Mask<8>(0x3f)), UC8Bytes{{0x3f, '\0'}}),
  UChar(unicode::u8(unicode::Mask<8>(0x40)), UC8Bytes{{0x40, '\0'}}),
  UChar(unicode::u8(unicode::Mask<8>(0x41)), UC8Bytes{{0x41, '\0'}}),
  UChar(unicode::u8(unicode::Mask<8>(0x42)), UC8Bytes{{0x42, '\0'}}),
  UChar(unicode::u8(unicode::Mask<8>(0x43)), UC8Bytes{{0x43, '\0'}}),
  UChar(unicode::u8(unicode::Mask<8>(0x44)), UC8Bytes{{0x44, '\0'}}),
  UChar(unicode::u8(unicode::Mask<8>(0x45)), UC8Bytes{{0x45, '\0'}}),
  UChar(unicode::u8(unicode::Mask<8>(0x46)), UC8Bytes{{0x46, '\0'}}),
  UChar(unicode::u8(unicode::Mask<8>(0x47)), UC8Bytes{{0x47, '\0'}}),
  UChar(unicode::u8(unicode::Mask<8>(0x48)), UC8Bytes{{0x48, '\0'}}),
  UChar(unicode::u8(unicode::Mask<8>(0x49)), UC8Bytes{{0x49, '\0'}}),
  UChar(unicode::u8(unicode::Mask<8>(0x4a)), UC8Bytes{{0x4a, '\0'}}),
  UChar(unicode::u8(unicode::Mask<8>(0x4b)), UC8Bytes{{0x4b, '\0'}}),
  UChar(unicode::u8(unicode::Mask<8>(0x4c)), UC8Bytes{{0x4c, '\0'}}),
  UChar(unicode::u8(unicode::Mask<8>(0x4d)), UC8Bytes{{0x4d, '\0'}}),
  UChar(unicode::u8(unicode::Mask<8>(0x4e)), UC8Bytes{{0x4e, '\0'}}),
  UChar(unicode::u8(unicode::Mask<8>(0x4f)), UC8Bytes{{0x4f, '\0'}}),
  UChar(unicode::u8(unicode::Mask<8>(0x50)), UC8Bytes{{0x50, '\0'}}),
  UChar(unicode::u8(unicode::Mask<8>(0x51)), UC8Bytes{{0x51, '\0'}}),
  UChar(unicode::u8(unicode::Mask<8>(0x52)), UC8Bytes{{0x52, '\0'}}),
  UChar(unicode::u8(unicode::Mask<8>(0x53)), UC8Bytes{{0x53, '\0'}}),
  UChar(unicode::u8(unicode::Mask<8>(0x54)), UC8Bytes{{0x54, '\0'}}),
  UChar(unicode::u8(unicode::Mask<8>(0x55)), UC8Bytes{{0x55, '\0'}}),
  UChar(unicode::u8(unicode::Mask<8>(0x56)), UC8Bytes{{0x56, '\0'}}),
  UChar(unicode::u8(unicode::Mask<8>(0x57)), UC8Bytes{{0x57, '\0'}}),
  UChar(unicode::u8(unicode::Mask<8>(0x58)), UC8Bytes{{0x58, '\0'}}),
  UChar(unicode::u8(unicode::Mask<8>(0x59)), UC8Bytes{{0x59, '\0'}}),
  UChar(unicode::u8(unicode::Mask<8>(0x5a)), UC8Bytes{{0x5a, '\0'}}),
  UChar(unicode::u8(unicode::Mask<8>(0x5b)), UC8Bytes{{0x5b, '\0'}}),
  UChar(unicode::u8(unicode::Mask<8>(0x5c)), UC8Bytes{{0x5c, '\0'}}),
  UChar(unicode::u8(unicode::Mask<8>(0x5d)), UC8Bytes{{0x5d, '\0'}}),
  UChar(unicode::u8(unicode::Mask<8>(0x5e)), UC8Bytes{{0x5e, '\0'}}),
  UChar(unicode::u8(unicode::Mask<8>(0x5f)), UC8Bytes{{0x5f, '\0'}}),
  UChar(unicode::u8(unicode::Mask<8>(0x60)), UC8Bytes{{0x60, '\0'}}),
  UChar(unicode::u8(unicode::Mask<8>(0x61)), UC8Bytes{{0x61, '\0'}}),
  UChar(unicode::u8(unicode::Mask<8>(0x62)), UC8Bytes{{0x62, '\0'}}),
  UChar(unicode::u8(unicode::Mask<8>(0x63)), UC8Bytes{{0x63, '\0'}}),
  UChar(unicode::u8(unicode::Mask<8>(0x64)), UC8Bytes{{0x64, '\0'}}),
  UChar(unicode::u8(unicode::Mask<8>(0x65)), UC8Bytes{{0x65, '\0'}}),
  UChar(unicode::u8(unicode::Mask<8>(0x66)), UC8Bytes{{0x66, '\0'}}),
  UChar(unicode::u8(unicode::Mask<8>(0x67)), UC8Bytes{{0x67, '\0'}}),
  UChar(unicode::u8(unicode::Mask<8>(0x68)), UC8Bytes{{0x68, '\0'}}),
  UChar(unicode::u8(unicode::Mask<8>(0x69)), UC8Bytes{{0x69, '\0'}}),
  UChar(unicode::u8(unicode::Mask<8>(0x6a)), UC8Bytes{{0x6a, '\0'}}),
  UChar(unicode::u8(unicode::Mask<8>(0x6b)), UC8Bytes{{0x6b, '\0'}}),
  UChar(unicode::u8(unicode::Mask<8>(0x6c)), UC8Bytes{{0x6c, '\0'}}),
  UChar(unicode::u8(unicode::Mask<8>(0x6d)), UC8Bytes{{0x6d, '\0'}}),
  UChar(unicode::u8(unicode::Mask<8>(0x6e)), UC8Bytes{{0x6e, '\0'}}),
  UChar(unicode::u8(unicode::Mask<8>(0x6f)), UC8Bytes{{0x6f, '\0'}}),
  UChar(unicode::u8(unicode::Mask<8>(0x70)), UC8Bytes{{0x70, '\0'}}),
  UChar(unicode::u8(unicode::Mask<8>(0x71)), UC8Bytes{{0x71, '\0'}}),
  UChar(unicode::u8(unicode::Mask<8>(0x72)), UC8Bytes{{0x72, '\0'}}),
  UChar(unicode::u8(unicode::Mask<8>(0x73)), UC8Bytes{{0x73, '\0'}}),
  UChar(unicode::u8(unicode::Mask<8>(0x74)), UC8Bytes{{0x74, '\0'}}),
  UChar(unicode::u8(unicode::Mask<8>(0x75)), UC8Bytes{{0x75, '\0'}}),
  UChar(unicode::u8(unicode::Mask<8>(0x76)), UC8Bytes{{0x76, '\0'}}),
  UChar(unicode::u8(unicode::Mask<8>(0x77)), UC8Bytes{{0x77, '\0'}}),
  UChar(unicode::u8(unicode::Mask<8>(0x78)), UC8Bytes{{0x78, '\0'}}),
  UChar(unicode::u8(unicode::Mask<8>(0x79)), UC8Bytes{{0x79, '\0'}}),
  UChar(unicode::u8(unicode::Mask<8>(0x7a)), UC8Bytes{{0x7a, '\0'}}),
  UChar(unicode::u8(unicode::Mask<8>(0x7b)), UC8Bytes{{0x7b, '\0'}}),
  UChar(unicode::u8(unicode::Mask<8>(0x7c)), UC8Bytes{{0x7c, '\0'}}),
  UChar(unicode::u8(unicode::Mask<8>(0x7d)), UC8Bytes{{0x7d, '\0'}}),
  UChar(unicode::u8(unicode::Mask<8>(0x7e)), UC8Bytes{{0x7e, '\0'}}),
  UChar(unicode::u8(unicode::Mask<8>(0x7f)), UC8Bytes{{0x7f, '\0'}})
};

}
