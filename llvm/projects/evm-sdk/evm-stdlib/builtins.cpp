//---------------------------------------------------------------------------//
// Copyright 2022 =nil; Foundation
//
// MIT License
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
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
//---------------------------------------------------------------------------//

static constexpr char debruijn32[32] = {
    0, 31, 9, 30, 3, 8, 13, 29, 2, 5, 7, 21, 12, 24, 28, 19,
    1, 10, 4, 14, 6, 22, 25, 20, 11, 15, 23, 26, 16, 27, 17, 18
};

static const int mod_37_bit_position[] = {
    32, 0, 1, 26, 2, 23, 27, 0, 3, 16, 24, 30, 28, 11, 0, 13, 4,
    7, 17, 0, 25, 22, 31, 15, 29, 10, 12, 6, 0, 21, 14, 9, 5,
    20, 8, 19, 18
};

extern "C" int __evm_builtin_clz(unsigned v) {
  v |= v>>1;
  v |= v>>2;
  v |= v>>4;
  v |= v>>8;
  v |= v>>16;
  v++;
  return debruijn32[v * 0x076be629 >> 27];
}

extern "C" int __evm_builtin_clzll(unsigned long long v) {
  unsigned high32 = v >> 32;
  unsigned low32 = v & 0xffffffff;
  if (high32) {
    return __evm_builtin_clz(high32);
  }
  return __evm_builtin_clz(low32) + 32;
}

extern "C" int __evm_builtin_ctz(unsigned v) {
  return mod_37_bit_position[(-v & v) % 37];
}

extern "C" int __evm_builtin_ctzll(unsigned long long v) {
  unsigned high32 = v >> 32;
  unsigned low32 = v & 0xffffffff;
  if (low32) {
    return __evm_builtin_ctz(low32);
  }
  return __evm_builtin_ctz(high32) + 32;
}