// RUN: %clang -triple evm -target-feature +v -ast-print %s \
// RUN:    | FileCheck %s

void bar(void) {
  // CHECK: __evm_uint256_t x0;
  __evm_uint256_t x0;

  // CHECK: __evm_int256_t x1;
  __evm_int256_t x1;
}
