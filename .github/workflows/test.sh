#!/bin/bash

set -ev

if [[ $TARGET_OS_NAME == 'Linux' ]]; then
    if [[ "$BUILD_TARGET" == "" ]]; then
        BUILD_TARGE="linux"
    fi

    CUR=`pwd`
    docker run --rm -v ${CUR}:/fibjs fibjs/${BUILD_TARGET}-test-env:${TARGET_ARCH} bash -c "cd /fibjs; ./bin/Linux_${TARGET_ARCH}_${BUILD_TYPE}/fibjs test/main.js"
else # Windows/Darwin
    ./bin/$DIST_DIR/fibjs test
fi

exit 0;
