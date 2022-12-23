add_compile_options(-fexceptions)

add_llvm_component_library(LLVMZK
  FieldArithmetics.cpp
  ADDITIONAL_HEADER_DIRS
  ${LLVM_MAIN_INCLUDE_DIR}/llvm/ZK

  LINK_LIBS
  crypto3::algebra
  crypto3::block
  crypto3::blueprint
  crypto3::codec
  crypto3::math
  crypto3::multiprecision
  crypto3::pkpad
  crypto3::pubkey
  crypto3::random
  crypto3::zk
  marshalling::core
  marshalling::crypto3_algebra
  marshalling::crypto3_multiprecision
  marshalling::crypto3_zk

  LINK_COMPONENTS
  Support
)
