#!/bin/bash

set -ev

if [[ $TARGET_OS_NAME == 'Linux' ]]; then
    if [[ "$BUILD_TARGET" != "" ]]; then
        CUR=`pwd`
        docker run --rm -v ${CUR}:/fibjs fibjs/${BUILD_TARGET}-test-env:${TARGET_ARCH} bash -c "cd /fibjs; ./bin/Linux_${TARGET_ARCH}_${BUILD_TYPE}/fibjs test/main.js"
    else
        case $TARGET_ARCH in
            arm) export QEMU_LD_PREFIX="/usr/arm-linux-gnueabihf"
                ;;
            arm64) export QEMU_LD_PREFIX="/usr/aarch64-linux-gnu"
                ;;
            mips64) export QEMU_LD_PREFIX="/usr/mips64el-linux-gnuabi64"
                ;;
            ppc64) export QEMU_LD_PREFIX="/usr/powerpc64le-linux-gnu"
                ;;
            i386|amd64)
                ;;
        esac

        ./bin/Linux_${TARGET_ARCH}_${BUILD_TYPE}/fibjs test/main.js
    fi
else # Windows/Darwin
    ./bin/$DIST_DIR/fibjs test
fi

exit 0;
