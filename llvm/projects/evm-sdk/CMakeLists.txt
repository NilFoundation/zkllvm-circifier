set(ZKLLVM_STDLIB_DIR evm)
set(LIBC_INSTALL_HEADERS ON)
set(LIBCXX_INSTALL_HEADERS ON)

add_subdirectory(evm-stdlib)
add_subdirectory(cpp-stdlib)

add_custom_target(evmlib ALL DEPENDS evm-stdlib zkllvm-cxx-headers zkllvm-c-headers)
