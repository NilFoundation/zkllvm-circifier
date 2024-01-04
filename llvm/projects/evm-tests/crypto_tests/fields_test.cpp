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

// EVM_RUN: function: test_bn128_fq, input: [], result: 0
[[evm]] int test_bn128_fq() {
    using T = fields::bn128_fq<254>;

    auto e1 = T::value_type(0x1234567);

    auto e2 = T::value_type(0x1234567812345678123456781234567812345678123456781234567812345678_cppui256);

    {
        auto res = e1 + e2;
        if (res != 0x1234567812345678123456781234567812345678123456781234567813579bdf_cppui256) {
            return __LINE__;
        }
    }

    {
        auto res = e1 - e2;
        if (res != 0x1e2ff7facefd49b1a61bef3e6f4d01e5854d1419563d741529ec359ec76bec36_cppui256) {
            return __LINE__;
        }
    }

    {
        auto res = e2 - e1;
        if (res != 0x1234567812345678123456781234567812345678123456781234567811111111_cppui256) {
            return __LINE__;
        }
    }

    {
        auto res = e1 * e2;
        if (res != 0x1cf0308df5f3f8b02ef5f5dc5dbe5a3e7b412d8aac4cf014fe60e563e90562cc_cppui256) {
            return __LINE__;
        }
    }

    {
        auto res = e1 / e2;
        auto in = e2.inversed();
        if (in != 0x11ffa5dc2201a4d6b4ca1b51bebc1ac8e759f92c7a160f21470243391dacd24a_cppui256) {
            return __LINE__;
        }
        if (res != 0x16205974db8f01ee0c52bf4ce8c6bbc3e47b647ccf0c3cc99f873664f6c95746_cppui256) {
            return __LINE__;
        }
    }

    {
        auto res = e2 / e1;
        auto in = e1.inversed();
        if (in != 0x104584d1c1b2b029c52904af770b9be3031eadee9c5be5c968fa60e295c98ae9_cppui256) {
            return __LINE__;
        }
        if (res != 0xfcf9db43e076ac26360ce7af2eea090be94430f44cd1889f8809430cdf77791_cppui256) {
            return __LINE__;
        }
    }

    {
        auto res = e1.inversed();
        if (res != 0x104584d1c1b2b029c52904af770b9be3031eadee9c5be5c968fa60e295c98ae9_cppui256) {
            return __LINE__;
        }
    }

    {
        auto res = e1.squared();
        if (res != 0x14b66dafaaf71) {
            return __LINE__;
        }
    }

    {
        auto res = e1.pow(0x567);
        if (res != 0x23b9b2d58a15e5a74c5977ff07846ffd9939fcd5f70362383cdb260ece2f7182_cppui256) {
            return __LINE__;
        }
    }

    {
        auto res = e1.pow(0x567_cppui256);
        if (res != 0x23b9b2d58a15e5a74c5977ff07846ffd9939fcd5f70362383cdb260ece2f7182_cppui256) {
            return __LINE__;
        }
    }

    {
        auto res = e1.doubled();
        if (res != 0x2468ace) {
            return __LINE__;
        }
    }

    return 0;
}