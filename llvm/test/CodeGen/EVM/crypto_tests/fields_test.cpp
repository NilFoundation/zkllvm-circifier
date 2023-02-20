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

    return 0;
}
