#pragma once

#include <tvm/to_std_tuple.hpp>

namespace tvm {

class __attribute((tvm_tuple)) slice {
public:
  slice() : sl_((__tvm_slice)__builtin_tvm_pushnull()) {}
  slice(__tvm_slice sl) : sl_(sl) {}

  static slice create_empty() {
    return __builtin_tvm_pushslice_empty();
  }

  static slice null() {
    return slice();
  }

  unsigned sbits() const { return __builtin_tvm_sbits(sl_); }
  unsigned srefs() const { return __builtin_tvm_srefs(sl_); }
  std::tuple<unsigned, unsigned> sbitrefs() const {
    return to_std_tuple(__builtin_tvm_sbitrefs(sl_));
  }
  unsigned depth() const { return __builtin_tvm_sdepth(sl_); }

  bool empty() const { return __builtin_tvm_sdempty(sl_); }
  bool srempty() const { return __builtin_tvm_srempty(sl_); }

  operator bool() const { return !isnull(); }
  bool isnull() const { return __builtin_tvm_isnull_slice(sl_); }

  unsigned hash() { return __builtin_tvm_hashsu(sl_); }

  __tvm_slice get() const { return sl_; }

  bool operator == (slice v) const {
    return __builtin_tvm_sdeq(sl_, v);
  }
  operator __tvm_slice() const { return get(); }

  __tvm_slice sl_;
};

} // namespace tvm
