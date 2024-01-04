#include <nil/crypto3/zk/transcript/fiat_shamir.hpp>
#include <nil/crypto3/hash/keccak.hpp>
#include <nil/crypto3/algebra/curves/alt_bn128.hpp>

using namespace nil::crypto3;
using namespace nil::crypto3::zk::transcript;


//[TODO: ENABLE] EVM_RUN: function: fiat_test, input: [], result: 2
[[evm]]
int fiat_test() {
    using field_type = algebra::curves::alt_bn128_254::scalar_field_type;
    std::array<std::uint8_t, 10> init_blob {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    fiat_shamir_heuristic_sequential<hashes::keccak_1600<256>> tr(init_blob);

    auto ch1 = tr.challenge<field_type>();

    if (!ch1.is_one()) {
        return 1;
    }
    return 0;
}

