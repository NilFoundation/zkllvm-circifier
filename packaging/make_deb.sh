#!/bin/bash -e

if [[ $# != 1 ]]; then
    echo "Usage: $0 binary_dir"
    exit 1
fi
BINARY_DIR=$1
ROOT_DIR=`dirname -- "$0"`/../

PACKAGE_VERSION=`grep -Po 'Version: \K.*' $ROOT_DIR/packaging/control`
PACKAGE_NAME="zkllvm-stdlib_${PACKAGE_VERSION}_all"
INCLUDE_PATH=/usr/include/zkllvm
LIB_PATH=/usr/lib/zkllvm
PACKAGE_DIR=$BINARY_DIR/$PACKAGE_NAME
mkdir -p $PACKAGE_DIR/$INCLUDE_PATH/c++
mkdir -p $PACKAGE_DIR/$LIB_PATH
mkdir -p $PACKAGE_DIR/DEBIAN

cp $ROOT_DIR/packaging/control $PACKAGE_DIR/DEBIAN
cp -R $ROOT_DIR/libc/include/* $PACKAGE_DIR/$INCLUDE_PATH/
cp -R $ROOT_DIR/libcpp/* $PACKAGE_DIR/$INCLUDE_PATH/c++/
cp $BINARY_DIR/libs/stdlib/libc/zkllvm-libc.ll $PACKAGE_DIR/$LIB_PATH/

dpkg-deb --root-owner-group --build $PACKAGE_DIR
