#!/bin/bash

set -e
set -x

SOURCE_DIR=$(realpath `dirname -- "$0"`)
CWD=`pwd`
BUILD_DIR=${CWD}/build
NPROC=`nproc`

function build() {
  cmake -S ${SOURCE_DIR}/../../ -B ${BUILD_DIR} -DCMAKE_BUILD_TYPE=Debug \
    -DLLVM_ENABLE_PROJECTS="clang" \
    -DLLVM_TARGETS_TO_BUILD="EVM" \
    -DLLVM_EXPERIMENTAL_TARGETS_TO_BUILD=EVM \
    -DCMAKE_INSTALL_PREFIX=${BUILD_DIR}/install \
    -DLLVM_INSTALL_TOOLCHAIN_ONLY=ON \
    -DDEFAULT_SYSROOT=/usr \
    -DEVM_HOST=ON \
    -DLLVM_DISTRIBUTION_COMPONENTS="clang;evm-stdlib;zkllvm-cxx-headers;zkllvm-c-headers"

  cmake --build ${BUILD_DIR} --target evmone install-distribution -j${NPROC}
}

function run_test() {
  # Run without constructor
  ruby ${SOURCE_DIR}/evm_test.rb \
       --bin-dir ${BUILD_DIR}/bin \
       --glob "${SOURCE_DIR}/common_tests/*.{s,cpp}" \
       --no-ctor --clang-args "--sysroot=${BUILD_DIR}/install" --evmone ${BUILD_DIR}/evmone-build
  # Run with constructor
  ruby ${SOURCE_DIR}/evm_test.rb \
        --bin-dir ${BUILD_DIR}/bin \
        --glob "${SOURCE_DIR}/common_tests/*.{s,cpp}" \
        --clang-args "--sysroot=${BUILD_DIR}/install" --evmone ${BUILD_DIR}/evmone-build
}

case $1 in
  build )
    shift
    build
    ;;
  test )
    shift
    run_test
    ;;
  *)
    build
    run_test
    ;;
esac