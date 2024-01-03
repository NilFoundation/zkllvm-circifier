set(LLVM_LINK_COMPONENTS
  Object
  Support
)

add_definitions("-Wno-gnu-zero-variadic-macro-arguments")

add_llvm_tool(evm-linker
  main.cpp
  evm_builder.cpp
  evm_file.cpp
  evm_linker.cpp
  DEPENDS EVMCommonTableGen
)
target_include_directories(evm-linker PRIVATE ${CMAKE_BINARY_DIR} ${CMAKE_SOURCE_DIR}
  ${CMAKE_SOURCE_DIR}/utils/crypto/hash/include)

install(TARGETS evm-linker RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}" COMPONENT evm-linker)

# We must install evm-linker even if LLVM_INSTALL_TOOLCHAIN_ONLY is set
if (NOT TARGET install-evm-linker)
  add_custom_target(install-evm-linker
    DEPENDS evm-linker
    COMMAND "${CMAKE_COMMAND}"
             -DCMAKE_INSTALL_COMPONENT=evm-linker
             -P "${CMAKE_BINARY_DIR}/cmake_install.cmake"
  )
  add_custom_target(install-evm-linker-stripped DEPENDS install-evm-linker)
endif()
