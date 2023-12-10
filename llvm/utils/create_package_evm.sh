#!/bin/bash -e

SOURCE_DIR=$(realpath `dirname -- "$0"`/../)

set -x

cmake $SOURCE_DIR -G Ninja \
  -DCMAKE_BUILD_TYPE=Release \
  -DLLVM_ENABLE_PROJECTS="clang" \
  -DLLVM_TARGETS_TO_BUILD="EVM" \
  -DLLVM_EXPERIMENTAL_TARGETS_TO_BUILD=EVM \
  -DLLVM_CCACHE_BUILD=ON \
  -DLLVM_ENABLE_LLD=ON \
  -DCMAKE_INSTALL_PREFIX=./install \
  -DLLVM_INSTALL_TOOLCHAIN_ONLY=ON \
  -DDEFAULT_SYSROOT=/usr \
  -DEVM_HOST=ON \
  -DLLVM_DISTRIBUTION_COMPONENTS="clang;evm-stdlib;evm-linker;zkllvm-cxx-headers;zkllvm-c-headers"

ninja install-distribution

PACKAGE_DIR=ecc-deb

rm -rf $PACKAGE_DIR

mkdir -p $PACKAGE_DIR/DEBIAN
mkdir -p $PACKAGE_DIR/usr/bin
mkdir -p $PACKAGE_DIR/usr/lib
mkdir -p $PACKAGE_DIR/usr/include

cp ./install/bin/ecc-16 $PACKAGE_DIR/usr/bin/ecc
cp ./install/bin/evm-linker $PACKAGE_DIR/usr/bin/
cp ./install/lib/libevm.a $PACKAGE_DIR/usr/lib/
cp -r ./install/include/evm $PACKAGE_DIR/usr/include/

cat > $PACKAGE_DIR/DEBIAN/control <<-END
Architecture: amd64
Description: C++ compiler for EVM
Maintainer: =nil; Foundation
Package: ecc
Priority: optional
Section: devel
Version: 0.0.1
END

dpkg-deb --build $PACKAGE_DIR
