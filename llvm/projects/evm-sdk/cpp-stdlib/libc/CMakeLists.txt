
file(GLOB_RECURSE C_HEADERS "${CMAKE_CURRENT_LIST_DIR}/include/*.h")
add_custom_target(zkllvm-c-headers ALL DEPENDS ${C_HEADERS})

if (LIBC_INSTALL_HEADERS)
  install(DIRECTORY include/
      DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${ZKLLVM_STDLIB_DIR}/libc
      COMPONENT zkllvm-c-headers
  )

  add_custom_target(install-zkllvm-c-headers
    DEPENDS zkllvm-c-headers
    COMMAND "${CMAKE_COMMAND}"
            -DCMAKE_INSTALL_COMPONENT=zkllvm-c-headers
            -P "${CMAKE_BINARY_DIR}/cmake_install.cmake"
  )
  add_custom_target(install-zkllvm-c-headers-stripped DEPENDS install-zkllvm-c-headers)
endif()

if (COMMAND add_circuit)
  add_circuit(zkllvm-libc SOURCES zk/malloc.c)
endif()
