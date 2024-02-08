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

#include <cstdint>

extern "C" __uint256_t __evm_builtin_modpow(__uint256_t, __uint256_t, __uint256_t);

using uint256_t = __uint256_t;
using int256_t = __int256_t;

#define evm_printf __builtin_evm_printf

namespace evm {

struct Signature {
  uint256_t hi;
  uint256_t lo;

  bool check(uint256_t pubkey, void* data, unsigned size) {
    // TODO: Implement, looks like precompiled contract #1 is suitable for this.
    return true;
  }
};

void require(bool expr, int code = 1) {
  if (!expr) {
    __builtin_evm_revert(0, 0);
  }
}

uint32_t now() {
  return __builtin_evm_now();
}

namespace detail {
namespace constants {
static constexpr unsigned MAGIC_SEN_MESSAGE = 0x1e44ca3b;
}
}

unsigned send_message(uint256_t address, uint32_t value, uint32_t data_size, uint256_t data) {
  uint256_t gas = uint256_t(detail::constants::MAGIC_SEN_MESSAGE) << 224;
  return __builtin_evm_send_msg(address, value, true, false, data, data_size);
}

struct InternalMessage {
  uint256_t address;
  uint64_t amount;
  uint32_t data_size;
  uint8_t data[];
};

template<typename T, unsigned Id>
struct StorItem {
  static T load() {
    return __builtin_evm_sload(Id);
  }
  static void save(T v) {
    return __builtin_evm_sstore(Id, v);
  }
};



#define EVM_STORAGE \
  namespace detail { enum { COUNTER_BASE = __COUNTER__ }; } \
  namespace stor

#define ITEM(type, name) \
  using name = evm::StorItem<type, __COUNTER__ - detail::COUNTER_BASE - 1>



template<typename T>
class Array {
public:
//  Array(uint8_t* data): data_(data) {}

  static Array* CreateFromBuffer(void* data) {
    return reinterpret_cast<Array*>(data);
  }

  bool IsEmpty() const {
    return GetSize() == 0;
  }

  unsigned GetSize() const {
    return size_;
//    return data_ == nullptr ? 0 : *reinterpret_cast<uint32_t*>(data_);
  }

  const T* GetData() const {
    return data_;
//    return IsEmpty() ? nullptr : reinterpret_cast<T*>(data_ + sizeof(uint32_t));
  }

  T* GetData() {
    return data_;
  }

  template<typename Func>
  void foreach(Func func) {
    T* current = GetData();
    for (unsigned i = 0; i < GetSize(); i++) {
      func(current);
      current++;
    }
  }
private:
  uint32_t size_;
  T data_[0];
};

}  // namespace evm

#endif // LLVM_EVM_SDK_H
