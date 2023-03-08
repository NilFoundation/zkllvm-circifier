// EVM_RUN: function: same_uses, input: [], result: 1
[[evm]] long same_uses() {
  long a[1];
  a[0] = 1;
  if (a[0] == 1) {
    return 1;
  }
  return 0;
}

// Problem was in StackAlloc: after bar returns, POP instruction, that clears
// unused bar's result, was inserted in wrong place. Error was appeared in test
// with addition of two _cppui381 numbers.
//
// EVM_RUN: function: test, input: [], result: 0
void* bar(const void* a, const void* b) {
  return nullptr;
}

struct Foo {
  void Init(const Foo& f) {
    bar(arr, f.arr);
    size = f.size;
  }
  __int128_t arr[3];
  __int128_t size;
};

[[evm]] int test() {
  Foo f1, f2;
  f1.size = 0x123456;
  f2.Init(f1);
  if (f2.size != 0x123456)
    return 1;
  return 0;
}

// Problem: wrong lowering of SELECT_CC instruction.
//
// EVM_RUN: function: test_selectcc, input: [0], result: 21
// EVM_RUN: function: test_selectcc, input: [1], result: 11
[[evm]] int test_selectcc(int v) {
  int a = v ? 10 : 20;
  return a + 1;
}

struct StackAllocTest {
  StackAllocTest& operator=(const int& v) {
    return *this;
  }
};

// Problem in Stack Allocation: wrong POP and SWAP order after calling
// `operator=`, that leads to "error: stack underflow (0 <=> 2)"
//
// EVM_RUN: function: test_stack_alloc, input: [1], result: 0
[[evm]] int test_stack_alloc(int flag) {
  StackAllocTest f;
  if (flag) {
    f = 1234;
  }

  return 0;
}

// Resolving reference to non-first struct's field. Due to 32 bytes alignment for i8 in EVM, error appeared in
// a offset calculation for `Foo2::b`, it becomes 32 times bigger.
//
// EVM_RUN: function: test_struct_field_resolve, input: [], result: 2
struct Foo2 {
  int a = 1;
  int b = 2;
};

Foo2 gf;
int* gp = &gf.b;

[[evm]] int test_struct_field_resolve() {
  return *gp;
}

// Stack allocator issue: one vreg is used twice within a single instruction with
// arbitrary operands count.
//
// EVM_RUN: function: same_uses_in_func_call, input: [1], result: 3
// EVM_RUN: function: same_uses_in_func_call, input: [7], result: 21

int __attribute__ ((noinline)) __add__(int a, int b)
{
  return a + b;
}

[[evm]] int same_uses_in_func_call(int n) {
  int a = __add__(n, n);
  return n + a;
}