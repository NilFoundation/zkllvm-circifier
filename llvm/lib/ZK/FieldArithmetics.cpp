#include <nil/marshalling/algorithms/pack.hpp>
#include <nil/crypto3/marshalling/algebra/types/field_element.hpp>

#include <nil/crypto3/algebra/random_element.hpp>
#include <nil/crypto3/algebra/curves/bls12.hpp>
#include <nil/crypto3/algebra/curves/detail/marshalling.hpp>
#include <nil/crypto3/algebra/fields/pallas/base_field.hpp>
#include <nil/crypto3/algebra/fields/curve25519/base_field.hpp>

#include "llvm/ADT/ArrayRef.h"
#include "llvm/ADT/SmallString.h"
#include "llvm/ZK/FieldArithmetics.h"

namespace llvm {

template <typename FieldType>
static FieldElem FieldBinOpImpl(FieldOperation Op, const FieldElem &LHS,
                                const FieldElem &RHS) {
  auto APIntData = reinterpret_cast<char *>(LHS.getData());
  // TODO(maksenov): avoid copying here
  std::vector<char> bytes(APIntData, APIntData + LHS.getNumWords() * 8);
  nil::marshalling::status_type status;
  typename FieldType::value_type lhsVal = nil::marshalling::pack<nil::marshalling::option::little_endian>(bytes, status);
  APIntData = reinterpret_cast<char *>(RHS.getData());
  bytes = std::vector(APIntData, APIntData + RHS.getNumWords() * 8);
  typename FieldType::value_type rhsVal = nil::marshalling::pack<nil::marshalling::option::little_endian>(bytes, status);
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
  std::vector<char> res_bytes = nil::marshalling::pack<nil::marshalling::option::little_endian>(op_res, status);
  int padding = res_bytes.size() % 8;
  for (int i = 0; i < padding; ++i)
    res_bytes.push_back(0);
  APInt res = APInt(FieldType::number_bits, makeArrayRef(reinterpret_cast<const uint64_t *>(res_bytes.data()), res_bytes.size() / 8));
  return FieldElem(LHS.getKind(), res);
}


FieldElem FieldBinOp(FieldOperation Op, const FieldElem &LHS, const FieldElem &RHS) {
    assert(LHS.getKind() == RHS.getKind());
    switch (LHS.getKind()) {
    case GALOIS_FIELD_BLS12_381_BASE:
        return FieldBinOpImpl<nil::crypto3::algebra::fields::bls12_base_field<381>>
        (Op, LHS, RHS);
    case GALOIS_FIELD_PALLAS_BASE: {
        return FieldBinOpImpl<nil::crypto3::algebra::fields::pallas_base_field>
            (Op, LHS, RHS);
    case GALOIS_FIELD_CURVE_25519_BASE:
        return FieldBinOpImpl<nil::crypto3::algebra::fields::curve25519_base_field>
            (Op, LHS, RHS);
      }
    }
}
}
