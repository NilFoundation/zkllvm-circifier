set(EVM_STDLIB_DIR evm)
add_subdirectory(evm-stdlib)
add_subdirectory(cpp-stdlib)

add_custom_target(evmlib ALL DEPENDS evm-stdlib evm-cxx-headers evm-c-headers)
