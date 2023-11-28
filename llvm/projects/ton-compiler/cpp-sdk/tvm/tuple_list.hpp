#pragma once

#include <tvm/cell.hpp>

namespace tvm {

/**
 * This class aims for creating lisp-like lists using native tvm_tuple type.
 * For example, list of [1, 2, 3] will be [1, [2, [3, nil]]].
 * Mostly it is used in getter methods for returning lists from contracts.
 * TON getter invoker understands this kind of lists and transform it into
 * proper form for higher-level languages such as TS or Python.
 */
class TupleList {
public:
  __always_inline TupleList() : tuple_(__builtin_tvm_nil()) {}

  __always_inline void push(unsigned v) {
    tuple_ = __builtin_tvm_tpush(tuple_, v);
  }

  __always_inline void push(cell v) {
    tuple_ = __builtin_tvm_tpush(tuple_, v.get());
  }

  __always_inline void push(TupleList v) {
    tuple_ = __builtin_tvm_tpush(tuple_, v.tuple());
  }

  template <unsigned bits> __always_inline void push(uint_t<bits> v) {
    tuple_ = __builtin_tvm_tpush(tuple_, v.get());
  }

  template <unsigned bits> __always_inline void push(int_t<bits> v) {
    tuple_ = __builtin_tvm_tpush(tuple_, v.get());
  }

  __always_inline __tvm_tuple tuple() { return tuple_; }

private:
  __tvm_tuple tuple_;
};

} // namespace tvm
