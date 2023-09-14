#!/bin/bash -e

if [[ $# != 1 ]]; then
    echo "Usage: $0 output_dir"
    exit 1
fi
OUTPUT_DIR=$1
ROOT_DIR=`dirname -- "$0"`/../

PACKAGE_VERSION=`grep -Po 'Version: \K.*' $ROOT_DIR/packaging/control`
PACKAGE_NAME="zkllvm-stdlib_${PACKAGE_VERSION}_all"
INSTALL_PATH=/usr/include/zkllvm
PACKAGE_DIR=$OUTPUT_DIR/$PACKAGE_NAME
mkdir -p $PACKAGE_DIR/$INSTALL_PATH/c++
mkdir -p $PACKAGE_DIR/DEBIAN

cp $ROOT_DIR/packaging/control $PACKAGE_DIR/DEBIAN
cp -R $ROOT_DIR/libc/include/* $PACKAGE_DIR/$INSTALL_PATH/
cp -R $ROOT_DIR/libcpp/* $PACKAGE_DIR/$INSTALL_PATH/c++/

dpkg-deb --root-owner-group --build $PACKAGE_DIR
