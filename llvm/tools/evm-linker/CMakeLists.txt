set(LLVM_LINK_COMPONENTS
  Object
  Support
)

add_definitions("-Wno-gnu-zero-variadic-macro-arguments")

add_llvm_tool(evm-linker
  evm_builder.cpp
  evm_file.cpp
  evm_linker.cpp
  DEPENDS EVMCommonTableGen
)
target_include_directories(evm-linker PRIVATE ${CMAKE_BINARY_DIR} ${CMAKE_SOURCE_DIR}
  ${CMAKE_SOURCE_DIR}/utils/crypto/hash/include)
