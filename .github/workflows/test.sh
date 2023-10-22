#!/bin/bash

set -ev

if [[ $TARGET_OS_NAME == 'Linux' ]]; then
    CUR=`pwd`

    if [[ "$BUILD_TARGET" != "" ]]; then
        docker run -t --rm -e CI=${CI} -v ${CUR}:/fibjs fibjs/${BUILD_TARGET}-test-env:${TARGET_ARCH} bash -c "cd /fibjs; ./bin/${DIST_DIR}/fibjs test/main.js"
    else
        if [[ $TARGET_ARCH == 'x64' ]]; then
            docker run -t --rm -e CI=${CI} -v ${CUR}:/fibjs ubuntu:12.04 bash -c "cd /fibjs; ./bin/${DIST_DIR}/fibjs test/main.js"
        else
            docker run -t --rm -e CI=${CI} -v ${CUR}:/fibjs fibjs/linux-build-env:${TARGET_ARCH} bash -c "cd /fibjs; ./bin/${DIST_DIR}/fibjs test/main.js"
        fi
    fi
else # Windows/Darwin
    ./bin/$DIST_DIR/fibjs test
fi

exit 0;
