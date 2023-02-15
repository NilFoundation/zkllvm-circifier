
set(CLANG_OPTIONS -fno-rtti -std=c++17 -O3)
set(CLANG_CPP_OPTIONS -Xclang -disable-llvm-passes -fno-exceptions)

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

set(LINKER_OUPTUT evmlib.a)
add_custom_command(OUTPUT ${LINKER_OUPTUT}
  COMMAND $<TARGET_FILE:evm-linker> ${STDLIB_OUPTUT} ${BUILTINS_OUPTUT} --no-ctor --make-archive -o ${LINKER_OUPTUT}
  DEPENDS ${STDLIB_OUPTUT} ${BUILTINS_OUPTUT} evm-linker
  COMMENT "Build stdlib: linking"
  )

add_custom_target(evm-stdlib
  DEPENDS ${BUILTINS_OUPTUT} ${STDLIB_OUPTUT} ${LINKER_OUPTUT}
)
