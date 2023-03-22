#include <nil/crypto3/algebra/fields/bls12/scalar_field.hpp>
#include <nil/crypto3/algebra/fields/bn128/base_field.hpp>

using namespace nil::crypto3::algebra;
using namespace nil::crypto3::algebra::fields;
using namespace nil::crypto3;

// EVM_RUN: function: test_simple, input: [], result: 0
[[evm]] int test_simple() {
    using T = fields::bls12_fr<381>;

    auto e1 = T::value_type(0x1234567);
    T::value_type e2 = T::value_type::one();

    if (e1 != 0x1234567) {
        return 1;
    }
    if (e2 != 1) {
        return 1;
    }

    return 0;
}

// TODO: enable test. It works with with 'disable-llvm-passes', but doesn't without it.
// !!!EVM_RUN: function: test_inversed, input: [], result: 0
[[evm]] int test_inversed() {
    using T = fields::bn128_fq<254>;

    auto e1 = T::value_type(0x1234567);

    T::value_type e1inv = e1.inversed();

    if (e1inv != 0x104584d1c1b2b029c52904af770b9be3031eadee9c5be5c968fa60e295c98ae9_cppui256) {
        return 1;
    }

    return 0;
}