#include <nil/crypto3/algebra/curves/bls12.hpp>
#include <nil/crypto3/algebra/curves/curve25519.hpp>
#include <nil/crypto3/algebra/curves/pallas.hpp>
#include <nil/crypto3/algebra/curves/vesta.hpp>
#include <nil/crypto3/algebra/fields/pallas/base_field.hpp>

#include <nil/crypto3/marshalling/algebra/types/field_element.hpp>
#include <nil/marshalling/algorithms/pack.hpp>

#include "llvm/ADT/ArrayRef.h"
#include "llvm/ADT/SmallString.h"
#include "llvm/ZK/FieldArithmetics.h"

namespace llvm {

unsigned GetNumberBits(GaloisFieldKind Kind) {
  switch (Kind) {
  case GALOIS_FIELD_BLS12381_BASE:
    return nil::crypto3::algebra::curves::bls12_381::base_field_type::number_bits;
  case GALOIS_FIELD_BLS12381_SCALAR:
    return nil::crypto3::algebra::curves::bls12_381::scalar_field_type::number_bits;
  case GALOIS_FIELD_PALLAS_BASE:
    return nil::crypto3::algebra::curves::pallas::base_field_type::number_bits;
  case GALOIS_FIELD_PALLAS_SCALAR:
    return nil::crypto3::algebra::curves::pallas::scalar_field_type::number_bits;
  case GALOIS_FIELD_VESTA_BASE:
    return nil::crypto3::algebra::curves::vesta::base_field_type::number_bits;
  case GALOIS_FIELD_VESTA_SCALAR:
    return nil::crypto3::algebra::curves::vesta::scalar_field_type::number_bits;
  case GALOIS_FIELD_CURVE25519_BASE:
    return nil::crypto3::algebra::curves::curve25519::base_field_type::number_bits;
  case GALOIS_FIELD_CURVE25519_SCALAR:
    return nil::crypto3::algebra::curves::curve25519::scalar_field_type::number_bits;
  }
  llvm_unreachable("Unspecified field kind");
}

unsigned GetNumberBits(EllipticCurveKind Kind) {
  return 2 * GetNumberBits(GetBaseFieldKind(Kind));
}

template <typename FieldType>
static FieldElem FieldBinOpImpl(FieldOperation Op, const FieldElem &LHS,
                                const FieldElem &RHS) {
  auto APIntData = reinterpret_cast<char *>(LHS.getData());
  // TODO(maksenov): avoid copying here
  std::vector<char> bytes(APIntData, APIntData + LHS.getNumWords() * 8);
  nil::marshalling::status_type status;
  typename FieldType::value_type lhsVal =
      nil::marshalling::pack<nil::marshalling::option::little_endian>(bytes,
                                                                      status);
  APIntData = reinterpret_cast<char *>(RHS.getData());
  bytes = std::vector(APIntData, APIntData + RHS.getNumWords() * 8);
  typename FieldType::value_type rhsVal =
      nil::marshalling::pack<nil::marshalling::option::little_endian>(bytes,
                                                                      status);
  typename FieldType::value_type op_res;
  switch (Op) {
  default:
    llvm_unreachable("Unexpected field binary operation");
  case F_Add:
    op_res = lhsVal + rhsVal;
    break;
  case F_Sub:
    op_res = lhsVal - rhsVal;
    break;
  case F_Mul:
    op_res = lhsVal * rhsVal;
    break;
  case F_Div:
    op_res = lhsVal / rhsVal;
    break;
  }
  std::vector<char> res_bytes =
      nil::marshalling::pack<nil::marshalling::option::little_endian>(op_res,
                                                                      status);
  int padding = res_bytes.size() % 8;
  for (int i = 0; i < padding; ++i)
    res_bytes.push_back(0);
  APInt res =
      APInt(FieldType::number_bits,
            ArrayRef(reinterpret_cast<const uint64_t *>(res_bytes.data()),
                         res_bytes.size() / 8));
  return FieldElem(LHS.getKind(), res);
}

FieldElem FieldBinOp(FieldOperation Op, const FieldElem &LHS,
                     const FieldElem &RHS) {
  assert(LHS.getKind() == RHS.getKind());
  switch (LHS.getKind()) {
  case GALOIS_FIELD_BLS12381_BASE:
    return FieldBinOpImpl<
        nil::crypto3::algebra::curves::bls12<381>::base_field_type>(Op, LHS,
                                                                    RHS);
  case GALOIS_FIELD_BLS12381_SCALAR:
    return FieldBinOpImpl<
        nil::crypto3::algebra::curves::bls12<381>::scalar_field_type>(Op, LHS,
                                                                      RHS);
  case GALOIS_FIELD_PALLAS_BASE:
    return FieldBinOpImpl<
        nil::crypto3::algebra::curves::pallas::base_field_type>(Op, LHS, RHS);
  case GALOIS_FIELD_PALLAS_SCALAR:
    return FieldBinOpImpl<
        nil::crypto3::algebra::curves::pallas::scalar_field_type>(Op, LHS, RHS);
  case GALOIS_FIELD_VESTA_BASE:
    return FieldBinOpImpl<
        nil::crypto3::algebra::curves::vesta::base_field_type>(Op, LHS, RHS);
  case GALOIS_FIELD_VESTA_SCALAR:
    return FieldBinOpImpl<
        nil::crypto3::algebra::curves::vesta::scalar_field_type>(Op, LHS, RHS);
  case GALOIS_FIELD_CURVE25519_BASE:
    return FieldBinOpImpl<
        nil::crypto3::algebra::curves::curve25519::base_field_type>(Op, LHS,
                                                                    RHS);
  case GALOIS_FIELD_CURVE25519_SCALAR:
    return FieldBinOpImpl<
        nil::crypto3::algebra::curves::curve25519::scalar_field_type>(Op, LHS,
                                                                      RHS);
  }
}

bool FieldElemFromStr(GaloisFieldKind Kind, StringRef StrRef,
                      FieldElem &Result) {
  int radix = 10;
  SmallString<10> Str(StrRef);
  if (Str[0] == '0') {
    Str.erase(Str.begin());
    if (Str[0] == 'x') {
      Str.erase(Str.begin());
      radix = 16;
    } else if (Str[0] == 'b') {
      Str.erase(Str.begin());
      radix = 2;
    } else {
      radix = 8;
    }
  }
  // TODO(maksenov): deal with negative constants
  unsigned StorageBits = GetNumberBits(Kind) + 1;  // +1 for sign
  APInt ParsedInt(StorageBits, 0);
  if (Str.str().getAsInteger(radix, ParsedInt))
    return false;
  if (ParsedInt.getSignificantBits() > StorageBits) {
    // Value was extended during parsing, so field bit width is insufficient
    return false;
  }
  Result = FieldElem(Kind, ParsedInt);
  return true;
}
} // namespace llvm
