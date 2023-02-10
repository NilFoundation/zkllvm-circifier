// EVM_RUN: function: test, input: [], result: 123

// Test for empty structs. Due to 256-bits alignment frontend should properly
// determine size of empty struct.
struct Empty {};

struct Empty2 {};

class EmptyDerivesEmpty : Empty {
};

struct EmptyContainsEmpty {
  Empty2 empty;
};

// Problem was in assert(TypeSizeInBits == getDataLayout().getTypeAllocSizeInBits(Ty))
// due to wrong alignment of empty struct, which was 1.
struct EmptyDerivesAndContainsEmpty : EmptyDerivesEmpty {
  Empty2 n;
};

struct Bar {
  int a;
  int b[16];
};

struct StructArrayOneChar {
  char b[1];
};

static_assert(sizeof(char (&)[1]) != sizeof(char (&)[2]), "FAIL");

[[evm]] int test() {
  Empty e;
  EmptyDerivesEmpty ede;
  EmptyContainsEmpty ece;
  EmptyDerivesAndContainsEmpty edce;
  Bar b;
  StructArrayOneChar s;
  return 123;
}

