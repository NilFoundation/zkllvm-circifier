#include <cassert>
#include "storage_iter_map_gen.h"

using namespace evm;

static constexpr const char* STORAGE_YAML = R"(
types:
- name: Foo
  flags: [storage]
  fields:
  - a: u256
  - b: u256
  - c: u256
  - map: IterableMap<i32>
variables:
- dummy: Foo
- map: IterableMap<i32>
- map_foo: IterableMap<Foo>
- flag: bool
)";

//! DEPLOY

//! let :map_values_slot, 8
//! let :value, 123456
//! CALL function: :set_data, input: [value]
[[evm]] void set_data(int v) {
  auto map = stor::map;
  assert(map.empty());

  assert(!map.contains(123));
  //! CHECK_STOR  key: "sha3(map_values_slot, 123)", result: value + 0
  map[123] = v;
  assert(map.contains(123));
  assert(map.size() == 1);

  //! CHECK_STOR  key: "sha3(map_values_slot, 124)", result: value + value + 1
  map[124] = map[123] + v + 1;
  assert(map.contains(124));
  assert(map.size() == 2);

  //! let :map_foo_values_slot, 12
  auto map_foo = stor::map_foo;
  assert(map_foo.empty());

  assert(!map_foo.contains(123));
  auto foo = map_foo[123];
  //! CHECK_STOR  key: "sha3(map_foo_values_slot, 123) + 0", result: value + 2
  //! CHECK_STOR  key: "sha3(map_foo_values_slot, 123) + 1", result: value + 3
  //! CHECK_STOR  key: "sha3(map_foo_values_slot, 123) + 2", result: value + 4
  foo.a = v + 2;
  foo.b = v + 3;
  foo.c = v + 4;
  foo.map[100] = v + 5;
  assert(map_foo.contains(123));
  assert(map_foo.size() == 1);
  assert(foo.map.contains(100));
  assert(foo.map.size() == 1);
}

//! CALL function: :remove_data
[[evm]] void remove_data() {
  auto map = stor::map;
  assert(map.size() == 2);

  assert(map.contains(123));
  //! CHECK_STOR  key: "sha3(map_values_slot, 123)", result: 0
  map.remove(123);
  assert(!map.contains(123));
  assert(map.size() == 1);

  assert(map.contains(124));
  //! CHECK_STOR  key: "sha3(map_values_slot, 124)", result: 0
  map.remove(124);
  assert(!map.contains(124));
  assert(map.size() == 0);

  auto map_foo = stor::map_foo;
  assert(map_foo.contains(123));
  assert(map_foo.size() == 1);
  map_foo.remove(123);
  //! CHECK_STOR  key: "sha3(map_foo_values_slot, 123) + 0", result: 0
  //! CHECK_STOR  key: "sha3(map_foo_values_slot, 123) + 1", result: 0
  //! CHECK_STOR  key: "sha3(map_foo_values_slot, 123) + 2", result: 0
  assert(!map_foo.contains(123));
  assert(map_foo.size() == 0);
}

//! CALL function: :iterate_insert
[[evm]] void iterate_insert() {
  auto map = stor::map;
  assert(map.empty());

  for (unsigned i = 0; i < 50; i++) {
    auto key = 123X << i;
    assert(!map.contains(key));
    map[key] = i;
    assert(map.contains(key));
    assert(map.size() == i + 1);
  }
}

//! CALL function: :iterate_test
[[evm]] void iterate_test() {
  auto map = stor::map;
  assert(map.size() == 50);

  for (unsigned i = 0; i < 50; i++) {
    auto key = map.get_key_at(i);
    assert(key == 123X << i);
    assert(map.contains(key));
    assert(map[key] == i);
  }
}
