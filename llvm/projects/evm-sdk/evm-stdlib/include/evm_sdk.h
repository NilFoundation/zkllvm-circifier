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
#include <type_traits>

extern "C" __uint256_t __evm_builtin_modpow(__uint256_t, __uint256_t, __uint256_t);

using uint256_t = __uint256_t;
using int256_t = __int256_t;

#define evm_printf __builtin_evm_printf

#define DVM_PACKED __attribute__((packed))

constexpr uint256_t operator "" _gwei(unsigned long long v) {
  constexpr unsigned GWEI_VALUE = 1'000'000'000;
  return uint256_t(static_cast<unsigned long long>(v * GWEI_VALUE));
}

namespace evm {

struct DVM_PACKED Address {
  int8_t workchain_id;
  uint256_t address;
};

void require(bool expr, uint256_t code = 0) {
  if (!expr) {
    __builtin_evm_revert((void*)&code, sizeof(code));
  }
}

uint256_t now() {
  return uint256_t(__builtin_evm_timestamp());
}

Address caller() {
  return {-1, uint256_t(__builtin_evm_caller())};
}

Address my_address() {
  return {-1, uint256_t(__builtin_evm_address())};
}

uint256_t callvalue() {
  return __builtin_evm_callvalue();
}

void send_message(const void* data, unsigned size) {
  __builtin_evm_log1(data, size, 0x0ec3c86d);
}

void* return_data(void* data, unsigned size) {
  static constexpr unsigned RETURN_DATA_SIZE_SLOT_ADDRESS = 32;
  __uint256_t* p = (__uint256_t*)RETURN_DATA_SIZE_SLOT_ADDRESS;
  // For arbitrary-sized data returned from the contract, we save its size in
  // memory address 32. Linker then reads it and properly returns to the host.
  *p = size;
  return data;
}

struct Signature {
  uint256_t hi;
  uint256_t lo;

  bool check(uint256_t pubkey, void* data, unsigned size) {
    // TODO: Implement, looks like precompiled contract #1 is suitable for this.
    return true;
  }
};

/**
 * Header for all messages transmitted between contracts.
 */
struct DVM_PACKED MessageHeader {
  uint256_t address_from;
  uint256_t address_to;
  uint256_t amount;
};

/**
 * Dynamic array with fixed size. It has precomputed size, stored in the first
 * word, followed by array's elements.
 * @tparam T type of element
 */
template<typename T>
class DVM_PACKED Vector {
public:
  static Vector& create_in_buffer(void* data, unsigned size) {
    Vector* self = reinterpret_cast<Vector*>(data);
    self->size_ = size;
    return *self;
  }

  static unsigned required_size(unsigned num) {
    return sizeof(size_) + sizeof(T) * num;
  }

  bool empty() const {
    return size() == 0;
  }

  unsigned size() const {
    return size_;
  }

  const T* data() const {
    return data_;
  }

  T* data() {
    return data_;
  }

  T& operator[](unsigned index) {
    return data_[index];
  }

  template<typename Func>
  void foreach(Func func) {
    T* current = data();
    for (unsigned i = 0; i < size(); i++) {
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
