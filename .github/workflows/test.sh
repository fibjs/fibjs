#!/bin/bash

set -ev

if [[ $TARGET_OS_NAME == 'Linux' ]]; then
    CUR=`pwd`

    if [[ "$BUILD_TARGET" == "" ]]; then
        docker run -t --rm -v ${CUR}:/fibjs fibjs/linux-build-env:${TARGET_ARCH} bash -c "cd /fibjs; ./bin/Linux_${TARGET_ARCH}_${BUILD_TYPE}/fibjs test/main.js"
    else
        docker run -t --rm -v ${CUR}:/fibjs fibjs/${BUILD_TARGET}-test-env:${TARGET_ARCH} bash -c "cd /fibjs; ./bin/Linux_${TARGET_ARCH}_${BUILD_TYPE}/fibjs test/main.js"
    fi
else # Windows/Darwin
    ./bin/$DIST_DIR/fibjs test
fi

exit 0;
