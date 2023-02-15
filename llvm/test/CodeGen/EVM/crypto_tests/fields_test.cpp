// EVM_RUN: function: test, input: [], result: 0

#include <nil/crypto3/algebra/fields/bls12/scalar_field.hpp>

using namespace nil::crypto3::algebra;
using namespace nil::crypto3::algebra::fields;
using namespace nil::crypto3;

[[evm]] int test() {
    using T = fields::bls12_fr<381>;

    auto e1 = T::value_type(0x1234567);
    T::value_type e2 = T::value_type::one();

    if (e1 != 0x1234567) {
        return 1;
    }
    if (e2 != 1) {
        return 1;
    }

    T::value_type e1inv = e1.inversed();

    if (e1inv != 0x3667dc6f224c235aae3a11459dca90697c112999c0a100bbf14cef709538d2d3_cppui256) {
        return 2;
    }

    return 0;
}
