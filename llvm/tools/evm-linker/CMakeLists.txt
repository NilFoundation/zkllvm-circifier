set(LLVM_LINK_COMPONENTS
  MC
  MCDisassembler
  Target
  EVMAsmParser
  EVMInfo
  EVMDesc
)

add_llvm_tool(evm-linker
  evm_linker.cpp
  DEPENDS EVMCommonTableGen
)
target_include_directories(evm-linker PRIVATE ${CMAKE_BINARY_DIR} ${CMAKE_SOURCE_DIR}
  ${CMAKE_SOURCE_DIR}/utils/crypto/hash/include)
