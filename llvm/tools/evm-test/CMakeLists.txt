
add_custom_target(evm-tests
  COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/evm_test.rb --bin-dir ${CMAKE_BINARY_DIR}/bin --glob \"${CMAKE_SOURCE_DIR}/test/CodeGen/EVM/runtime_tests/**/*.{c,cpp}\"
  DEPENDS clang evm-linker clang evm_test.rb
)