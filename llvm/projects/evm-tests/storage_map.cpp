#include <cassert>
#include "storage_map_gen.h"

using namespace evm;

static constexpr const char* STORAGE_YAML = R"(
variables:
- gv: i32
- map: Map<Foo>
- foo: Foo

types:
- name: Foo
  flags: [storage]
  fields:
  - a: i32
  - b: i32
  - c: Map<Bar>
  - d: Map<i32>
  - bar: Bar
- name: Bar
  flags: [storage]
  fields:
  - a: i32
  - b: i32
  - d: Map<i32>
)";

//! let :value, 123456
//! DEPLOY
//! CALL        function: :get_map_d, input: [10, 20], result: 0
//! CALL        function: :set_data, input: [10, 20, value]
//! CALL        function: :get_map_d, input: [10, 20], result: value

[[evm]] void set_data(int k, int kk, int v) {
  auto f = stor::map[k];

  //! CHECK_STOR  key: "sha3(sha3(1, 10) + 3, 20)", result: value
  f.d[kk].set(v);
  //! CHECK_STOR  key: "sha3(1, 10) + 0", result: value + 1
  f.a.set(v + 1);
  //! CHECK_STOR  key: "sha3(1, 10) + 1", result: value + 2
  f.b.set(v + 2);

  //! CHECK_STOR  key: "sha3(1, 10) + 4 + 1", result: value + 2
  f.bar.b.set(v + 2);

  //! CHECK_STOR  key: "sha3(sha3(1, 10) + 2, 20) + 1", result: value + 3
  auto bar = f.c[kk];
  bar.b.set(v + 3);

  //! CHECK_STOR  key: 3, result: value
  stor::foo.b.set(v);
  //! CHECK_STOR  key: 7, result: value + 1
  stor::foo.bar.b.set(v + 1);
}

[[evm]] __int256_t get_map_d(int k, int kk) {
  return stor::map[k].d[kk].get();
}

//! CALL function: :remove_map, input: [10, value]
//! (0..6).each do |i|
//!   CHECK_STOR  key: sha3(1, 10) + i, result: 0
//! end
[[evm]] void remove_map(int k, int v) {
  auto f = stor::map[k];
  // Check some values are not zero
  assert(f.a.get() == v + 1);
  assert(f.b.get() == v + 2);
  assert(f.bar.b.get() == v + 2);

  stor::map.remove(k);
}

//! CALL function: :test_sha3_vargs, input: [456789]
[[evm]] void test_sha3_vargs(unsigned v) {

  //! CHECK_STOR  key: "sha3(2)", result: 456789
  auto key = __builtin_evm_sha3_vargs(2);
  __builtin_evm_sstore(key ,v);

  //! CHECK_STOR  key: "sha3(2, 34, 56, 78, 90)", result: 456789 + 1
  key = __builtin_evm_sha3_vargs(2, 34, 56, 78, 90);
  __builtin_evm_sstore(key, v + 1);
}
