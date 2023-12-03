
set(CLANG_ARGS --sysroot=${CMAKE_BINARY_DIR})
set(CLANG_ARGS "${CLANG_ARGS} -I${CMAKE_SOURCE_DIR}/projects/evm-sdk/cpp-stdlib/libcpp")
set(CLANG_ARGS "${CLANG_ARGS} -I${CMAKE_SOURCE_DIR}/projects/evm-sdk/cpp-stdlib/libc/include")
set(CLANG_ARGS "${CLANG_ARGS} -I${CMAKE_SOURCE_DIR}/projects/evm-sdk/evm-stdlib/include")

add_custom_target(evm-tests
  COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/evm_test.rb --bin-dir ${CMAKE_BINARY_DIR}/bin --glob \"${CMAKE_SOURCE_DIR}/test/CodeGen/EVM/runtime_tests/**/*.{c,cpp}\" --no-ctor --clang-args ${CLANG_ARGS}
  DEPENDS clang evm-linker evm_test.rb evm-stdlib
)
