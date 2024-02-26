#include <cassert>
#include "storage_struct_gen.h"

using namespace evm;

static constexpr const char* STORAGE_YAML = R"(
types:
- name: Foo
  fields:
  - { name: a, type: uint256_t }
  - { name: b, type: uint256_t }
  - { name: c, type: uint256_t }
- name: Bar
  fields:
  - { name: a, type: uint256_t }
  - { name: b, type: uint256_t }
  - { name: c, type: uint256_t }
variables:
- { name: foo, type: Foo }
- { name: bar, type: Bar }
)";

//! DEPLOY
//! CALL        function: :test, input: [123456]
// Test that static fields won't overlap

[[evm]] void test(int v) {
  //! CHECK_STOR  key: 0, result: 123456 + 0
  stor::foo.a = v++;
  //! CHECK_STOR  key: 1, result: 123456 + 1
  stor::foo.b = v++;
  //! CHECK_STOR  key: 2, result: 123456 + 2
  stor::foo.c = v++;
  //! CHECK_STOR  key: 3, result: 123456 + 3
  stor::bar.a = v++;
  //! CHECK_STOR  key: 4, result: 123456 + 4
  stor::bar.b = v++;
  //! CHECK_STOR  key: 5, result: 123456 + 5
  stor::bar.c = v++;
}