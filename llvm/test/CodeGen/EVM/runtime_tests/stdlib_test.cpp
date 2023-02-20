#include <cstring>

// EVM_RUN: function: test_memset, input: [0xab], result: 0

[[evm]] int test_memset(int a) {
  __uint256_t val;
  memset(&val, 0xab, sizeof(val));
  if (val != 0xababababababababababababababababababababababababababababababababX)
    return 1;
  return 0;
}

// EVM_RUN: function: test_memcpy, input: [123], result: 0
[[evm]] int test_memcpy(int a) {
  static constexpr unsigned count = 7;
  int src[count];
  int dst[count];
  static_assert(sizeof(src) == count * 32);

  for (int i = 0; i < count; i++) {
    src[i] = a + i;
    dst[i] = -1;
  }
  memcpy(dst, src, sizeof(src));
  for (int i = 0; i < count; i++) {
    if (dst[i] != a + i)
      return 1;
  }
  return 0;
}

// EVM_RUN: function: test_struct_copy, input: [], result: 0

struct Foo {
  int n{1};
  char c{2};
};

[[evm]] int test_struct_copy(int n) {
  Foo f1{0x123, 45};
  Foo f2;
  if (f2.n != 1 || f2.c != 2)
    return 1;
  memcpy(&f2, &f1, sizeof(Foo));
  if (f2.n != 0x123 || f2.c != 45)
    return 2;
  return 0;
}
