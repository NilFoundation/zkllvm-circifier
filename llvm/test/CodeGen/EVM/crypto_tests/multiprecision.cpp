
#include <nil/crypto3/detail/literals.hpp>

using namespace nil::crypto3::multiprecision;

// EVM_RUN: function: test_arith_256, input: [0], result: 0
// EVM_RUN: function: test_arith_256, input: [1], result: 0
// EVM_RUN: function: test_arith_256, input: [2], result: 1
[[evm]] int test_arith_256(int action) {
    auto a = 0x1101010123232323454545456767676789898989ababababcdcdcdcdefefefef_cppui256;
    auto b = 0x12345678123456781234567812345678_cppui256;
    decltype (a) arith;
    decltype (a) res;
    switch (action) {
    case 0:
      arith = a + b;
      res = 0x110101012323232345454545676767679bbde001bde00223e002244602244667_cppui256;
      break;
    case 1:
      arith = a - b;
      res = 0x110101012323232345454545676767677755331199775533bb997755ddbb9977_cppui256;
      break;
    default:
      arith = 0;
      res = 1;
      break;
    }
    if (arith != res)
        return 1;
    return 0;
}

// EVM_RUN: function: test_arith_381, input: [], result: 0
[[evm]] int test_arith_381() {
    auto a = 0x12345678123456781234567812345678ababababcdcdcdcdefefefef010101012323232345454545_cppui381;
    auto b = 0xabcdef44abcdef44abcdef44abcdef44abcdef44abcdef44abcdef44abcdef44abcdef44abcdef44_cppui381;
    auto c = 0xbe0245bcbe0245bcbe0245bcbe0245bd57799af0799bbd129bbddf33accef045cef11267f1133489_cppui381;
    auto d = a + b;
    if (d != c)
        return 1;
    if (d == 0x0_cppui381)
        return 1;
    return 0;
}