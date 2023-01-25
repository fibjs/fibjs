#!/bin/bash

set -ev

if [[ ! -z $USE_VENDER_DIST ]]; then
    bash ./scripts/download_vender.sh;
fi

if [[ "$TARGET_OS" != 'Windows' ]]; then
    TARGET_OS=`uname`
    if [[ "$TARGET_OS" == "Linux" ]]; then
        if [[ "$BUILD_TARGET" == "" ]]; then
            BUILD_TARGET="linux"
        fi

        CUR=`pwd`
        docker run -t --rm -e USE_VENDER_DIST -v ${CUR}:/fibjs fibjs/${BUILD_TARGET}-build-env:${TARGET_ARCH} bash -c "cd /fibjs; bash build -j2 ${TARGET_ARCH} ${BUILD_TYPE} ci"
    else
        bash build -j2 ${TARGET_ARCH} ${BUILD_TYPE} ${BUILD_TARGET} ci
    fi
else # Window
    exit 1;
fi

exit 0;