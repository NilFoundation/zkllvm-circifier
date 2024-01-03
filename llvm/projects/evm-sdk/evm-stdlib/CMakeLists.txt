
set(CLANG_OPTIONS -fno-rtti -std=c++17 -O3)

set(BUILTINS_OUPTUT builtins.o)
add_custom_command(OUTPUT ${BUILTINS_OUPTUT}
  COMMAND $<TARGET_FILE:clang> -target evm ${CLANG_OPTIONS} ${CLANG_CPP_OPTIONS} ${CMAKE_CURRENT_SOURCE_DIR}/builtins.cpp -c -o ${BUILTINS_OUPTUT}
  DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/builtins.cpp
  DEPENDS clang
  COMMENT "Build stdlib: builtins.cpp"
)

set(STDLIB_OUPTUT stdlib.o)
add_custom_command(OUTPUT ${STDLIB_OUPTUT}
  COMMAND $<TARGET_FILE:clang> -target evm ${CLANG_OPTIONS} ${CMAKE_CURRENT_SOURCE_DIR}/stdlib.s -c -o ${STDLIB_OUPTUT}
  DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/stdlib.s
  DEPENDS clang
  COMMENT "Build stdlib: stdlib.s"
)

set(LINKER_OUPTUT ${CMAKE_BINARY_DIR}/lib/libevm.a)
add_custom_command(OUTPUT ${LINKER_OUPTUT}
  COMMAND $<TARGET_FILE:evm-linker> ${STDLIB_OUPTUT} ${BUILTINS_OUPTUT} --no-ctor --make-archive -o ${LINKER_OUPTUT}
  DEPENDS ${STDLIB_OUPTUT} ${BUILTINS_OUPTUT} evm-linker
  COMMENT "Build stdlib: linking"
)

add_custom_target(evm-stdlib ALL
  DEPENDS ${BUILTINS_OUPTUT} ${STDLIB_OUPTUT} ${LINKER_OUPTUT}
)

install(DIRECTORY include/
  DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${ZKLLVM_STDLIB_DIR}/evm-sdk
  COMPONENT evm-stdlib
)

install(FILES ${LINKER_OUPTUT}
  DESTINATION ${CMAKE_INSTALL_LIBDIR}
  COMPONENT evm-stdlib
)

add_custom_target(install-evm-stdlib
  DEPENDS evm-stdlib
  COMMAND "${CMAKE_COMMAND}"
          -DCMAKE_INSTALL_COMPONENT=evm-stdlib
          -P "${CMAKE_BINARY_DIR}/cmake_install.cmake"
)
add_custom_target(install-evm-stdlib-stripped DEPENDS install-evm-stdlib)
