add_llvm_library(
  LibcTableGenUtil
  APIIndexer.cpp
  APIIndexer.h
  LINK_COMPONENTS Support TableGen
)
target_include_directories(LibcTableGenUtil PUBLIC ${LLVM_LIBC_INCLUDE_DIRS})
