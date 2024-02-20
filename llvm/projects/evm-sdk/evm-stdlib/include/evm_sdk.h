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

void require(bool expr, int code = 1) {
  if (!expr) {
    __builtin_evm_revert(0, 0);
  }
}

namespace stor {

template <class T, unsigned key_>
struct SlotStatic {
  void set(const T &v) {
    __builtin_evm_sstore(key_, v);
  }

  T get() const {
    return __builtin_evm_sload(key_);
  }
};

template <class T>
struct Slot {

  explicit Slot(__int256_t key) : key_(key) {}

  void set(const T &v) {
    __builtin_evm_sstore(key_, v);
  }

  T get() const {
    return __builtin_evm_sload(key_);
  }

  template <class TT> void operator=(TT) = delete;

private:
  __int256_t key_;
};

template <typename T, __int256_t Index>
struct DictStatic {
  auto operator[](__int256_t k) {
    auto k1 = __builtin_evm_sha3_vargs(Index, k);
    if constexpr (std::is_integral_v<T>) {
      return Slot<T>(k1);
    } else {
      return T(k1);
    }
  }
};

template <typename T, __int256_t Pos>
struct Dict {
  Dict(__int256_t slot) : slot_(slot) {}

  auto operator[](__int256_t k) {
    auto k1 = __builtin_evm_sha3_vargs(slot_, Pos, k);
    if constexpr (std::is_integral_v<T>) {
      return Slot<T>(k1);
    } else {
      return T(k1);
    }
  }

private:
  __int256_t slot_;
};

}  // namespace stor

}  // namespace evm

#endif // LLVM_EVM_SDK_H
