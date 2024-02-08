
struct Bar {
  short s;
  long long ll;
} __attribute__ ((packed));

struct Foo {
  int i;
  unsigned char c;
  Bar b;
  unsigned u;
} __attribute__ ((packed));

[[storage]] Foo foo;

// EVM_RUN: function: set, input: '11111111223333444444444444444455555555'
[[evm]] void set(Foo f) {
  foo.i = f.i;
  foo.c = f.c;
  foo.b.s = f.b.s;
  foo.b.ll = f.b.ll;
  foo.u = f.u;
}

// EVM_RUN: function: sum, result: 4444444511114465
[[evm]] long long sum() {
  return foo.i + foo.c + foo.b.ll + foo.b.s + foo.u;
}
