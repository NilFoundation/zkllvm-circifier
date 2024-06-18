add_compile_options(-fexceptions)

add_llvm_component_library(LLVMZK
  FieldArithmetics.cpp
  ADDITIONAL_HEADER_DIRS
  ${LLVM_MAIN_INCLUDE_DIR}/llvm/ZK

  LINK_LIBS
  crypto3::all

  LINK_COMPONENTS
  Support
)
