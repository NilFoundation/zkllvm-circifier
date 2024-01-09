// EVM_RUN: function: test, input: [10], result: 2
// EVM_RUN: function: test, input: [1], result: 1

struct Bar {
  virtual int func() {
    return 1;
  }
};

struct Foo : public Bar {
  int func() {
    return 2;
  }
};

[[evm]] int test(int n) {
  Foo foo;
  Bar bar;
  Bar* obj;
  if (n > 5) {
    obj = &foo;
  } else {
    obj = &bar;
  }
  return obj->func();
}