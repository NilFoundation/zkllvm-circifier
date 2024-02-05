
#pragma pack(push, 1)
struct Foo {
  unsigned u32;
  long long u64;
};
#pragma pack(pop)

int __attribute__ ((noinline)) sum(Foo f) {
  return f.u32 + f.u64;
}

// EVM_RUN: function: test1, input: [123, 456], result: 579
[[evm]] int test1(unsigned a, long long b)
{
  Foo f{a, b};
  return sum(f);
}

// EVM_RUN: function: test2, input: '000000010000000000000002', result: 3
// EVM_RUN: function: test2, input: '800000018000000000000002', result: 0x8000000080000003
[[evm]] int test2(Foo f)
{
  return f.u32 + f.u64;
}

#pragma pack(push, 1)
struct Foo2 {
  unsigned char u8;
  long long u64;
  unsigned short u16;
};
#pragma pack(pop)

// EVM_RUN: function: test3, input: '1180000000000000021234', result: 0x8000000000001247
[[evm]] int test3(Foo2 f)
{
  return f.u8 + f.u64 + f.u16;
}