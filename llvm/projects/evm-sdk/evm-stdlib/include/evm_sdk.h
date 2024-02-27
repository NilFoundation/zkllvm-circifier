//---------------------------------------------------------------------------//
// Copyright 2023 =nil; Foundation
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
#ifndef LLVM_EVM_SDK_H
#define LLVM_EVM_SDK_H

#include <type_traits>

extern "C" __uint256_t __evm_builtin_modpow(__uint256_t, __uint256_t, __uint256_t);

using uint256_t = __uint256_t;
using int256_t = __int256_t;

#define evm_printf __builtin_evm_printf

namespace evm {

using Address = __uint256_t;
using Gwei = __uint256_t;

void require(bool expr) {
  if (!expr) {
    __builtin_evm_revert(0, 0);
  }
}

uint256_t now() {
  return __builtin_evm_timestamp();
}

}  // namespace evm

#endif // LLVM_EVM_SDK_H
