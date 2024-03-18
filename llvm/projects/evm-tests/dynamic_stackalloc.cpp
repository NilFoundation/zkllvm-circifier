
struct Array {
  __attribute__ ((noinline)) Array(unsigned char* p, unsigned sz): buf(p), size(sz) {}

  void __attribute__ ((noinline)) fill(unsigned char addend) {
    for (unsigned i = 0; i < size; i++) {
      buf[i] = i + addend;
    }
  }

  unsigned char *buf = nullptr;
  unsigned size = 0;
};

int __attribute__ ((noinline)) nested_alloc(Array& a1, Array& a2, unsigned i) {
  auto size = a1.size;
  unsigned char buf[size];
  Array a3(buf, size);
  for (unsigned i = 0; i < size; i++) {
    a3.buf[i] = a1.buf[i] + a2.buf[i];
  }
  return a3.buf[i];
}

//! DEPLOY
//! def calc(n, i); (0..n).map{ _1 + _1 + 10 }[i]; end
//! CALL function: :test, input: [4, 3], result: calc(4, 3)
//! CALL function: :test, input: [35, 13], result: calc(35, 13)
//! CALL function: :test, input: [150, 48], result: calc(150, 48)
//! CALL function: :test, input: [150, 0], result: calc(150, 0)
[[evm]] int test(unsigned size, unsigned i) {
  unsigned char buf1[size];
  unsigned char buf2[size];
  Array a1(buf1, size);
  Array a2(buf2, size);
  a1.fill(0);
  a2.fill(10);
  return nested_alloc(a1, a2, i);
}

// Test when allocation occurs within local scope, thereby `stacksave` and
// `stackrestore` instructions are generated.
//! CALL function: :test_dyn_stacksave, input: [3], result: 1
//! CALL function: :test_dyn_stacksave, input: [6], result: 0
//! CALL function: :test_dyn_stacksave, input: [600], result: 0
[[evm]] int test_dyn_stacksave(int n) {
  int res= 0;
  if (n > 4) {
    unsigned char arr[n];
    res = arr[4];
  } else {
    res = 1;
  }
  return res;
}