#!/bin/bash

set -ev

if [[ ! -z $USE_VENDER_DIST ]]; then
    bash ./scripts/download_vender.sh;
fi

if [[ "$TARGET_OS" != 'Windows' ]]; then
    TARGET_OS=`uname`
    if [[ "$BUILD_TARGET" != "" ]]; then
        CUR=`pwd`
        docker run -t --rm -e USE_VENDER_DIST -e CI=${CI} -v ${CUR}:/fibjs fibjs/${BUILD_TARGET}-build-env:${TARGET_ARCH} bash -c "cd /fibjs; bash build -j2 ${TARGET_ARCH} ${BUILD_TYPE} ci"
    elif [[ "$TARGET_OS" == "Linux" ]]; then
        CUR=`pwd`
        docker run -t --rm -e USE_VENDER_DIST -e CI=${CI} -v ${CUR}:/fibjs fibjs/linux-build-env:${TARGET_ARCH} bash -c "cd /fibjs; bash build -j2 ${TARGET_ARCH} ${BUILD_TYPE} ci"
    else
        bash build -j2 ${TARGET_ARCH} ${BUILD_TYPE} ci
    fi
else # Window
    exit 1;
fi

exit 0;