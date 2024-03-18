#include <cassert>
#include "storage_vector_gen.h"

using namespace evm;

static constexpr const char* STORAGE_YAML = R"(
types:
- name: Foo
  fields:
  - a: i32
  - b: Vector<Bar>
  - c: Vector<i32>
- name: Bar
  fields:
  - a: i32
  - b: i32
  - c: Vector<i32>
variables:
- arr: Vector<u256>
- arr_foo: Vector<Foo>
- map_arr: Map<Foo>
)";

//! DEPLOY
//! CALL        function: :set_data, input: [9876543210, 123456]

[[evm]] void set_data(uint256_t k, int v) {
  assert(stor::arr.empty());
  assert(stor::arr_foo.empty());
  //! CHECK_STOR  key: "sha3(0)", result: 123456
  stor::arr.push().set(v);
  //! CHECK_STOR  key: "sha3(0) + 1", result: 123456 + 1
  stor::arr.push().set(v + 1);
  assert(stor::arr.size() == 2);
  assert(stor::arr[0].get() == v);
  assert(stor::arr[1].get() == v + 1);

  // Test `arr_foo`
  assert(stor::arr_foo.empty());
  auto foo = stor::arr_foo.push();
  assert(stor::arr_foo.size() == 1);

  //! CHECK_STOR  key: "sha3(1)", result: 123456 + 2
  foo.a = v + 2;
  foo.c.push();
  assert(foo.c.size() == 1);

  assert(foo.b.empty());
  auto bar = foo.b.push();
  assert(foo.b.size() == 1);
  //! let :arr_foo_b, sha3(sha3(1) + 1)
  //! CHECK_STOR  key: "arr_foo_b", result: 123456 + 3
  bar.a = v + 3;
  //! CHECK_STOR  key: "arr_foo_b + 1", result: 123456 + 4
  bar.b = v + 4;
  assert(bar.c.empty());
  //! let :arr_foo_b_c, sha3(arr_foo_b + 2)
  //! CHECK_STOR  key: "arr_foo_b_c", result: 123456 + 5
  bar.c.push().set(v + 5);
  //! CHECK_STOR  key: "arr_foo_b_c + 1", result: 123456 + 6
  bar.c.push().set(v + 6);
  assert(bar.c.size() == 2);

  // Test `map_arr`
  foo = stor::map_arr[k];

  //! CHECK_STOR  key: "sha3(2, 9876543210)", result: 123456 + 7
  foo.a = v + 7;

  assert(foo.b.empty());
  bar = foo.b.push();
  assert(foo.b.size() == 1);
  //! let :map_foo, sha3(2, 9876543210)
  //! let :map_foo_b, sha3(map_foo + 1)
  //! CHECK_STOR  key: "map_foo_b", result: 123456 + 8
  bar.a = v + 8;
  //! CHECK_STOR  key: "map_foo_b + 1", result: 123456 + 9
  bar.b = v + 9;
  assert(bar.c.empty());
  //! let :map_foo_b_c, sha3(map_foo_b + 2)
  //! CHECK_STOR  key: "map_foo_b_c", result: 123456 + 10
  bar.c.push().set(v + 10);
  //! CHECK_STOR  key: "map_foo_b_c + 1", result: 123456 + 11
  bar.c.push().set(v + 11);
  assert(bar.c.size() == 2);
}

//! CALL function: :remove, input: [123456]
//! let :arr_foo, sha3(1)
//! CHECK_STOR  key: "arr_foo + 0", result: 0
//! CHECK_STOR  key: "arr_foo + 1", result: 0
//! CHECK_STOR  key: "arr_foo + 2", result: 0

[[evm]] void remove(int v) {
  assert(stor::arr.size() == 2);

  assert(stor::arr[0].get() == v);
  assert(stor::arr[1].get() == v + 1);

  stor::arr.pop();
  assert(stor::arr.size() == 1);
  assert(stor::arr[1].get() == 0);
  stor::arr.pop();
  assert(stor::arr.size() == 0);
  assert(stor::arr[0].get() == 0);

  assert(stor::arr_foo.size() == 1);
  auto foo = stor::arr_foo[0];
  assert(foo.a == v + 2);
  assert(!foo.b.empty());
  assert(!foo.c.empty());

  stor::arr_foo.pop();
  assert(stor::arr_foo.empty());
}