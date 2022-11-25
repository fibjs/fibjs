#!/bin/bash

set -ev

if [[ ! -z $USE_VENDER_DIST ]]; then
    bash ./scripts/download_vender.sh;
fi

if [[ "$TARGET_OS" != 'Windows' ]]; then
    if [[ "$BUILD_ALPINE" == "alpine" ]]; then
        CUR=`pwd`
        docker run --rm -e USE_VENDER_DIST -e CI=${CI} -v ${CUR}:/fibjs fibjs/alpine-build-env:${TARGET_ARCH} bash -c "cd /fibjs; bash build -j2 ${TARGET_ARCH} ${BUILD_TYPE} ci"
    else
        bash build -j2 ${TARGET_ARCH} ${BUILD_TYPE} ci
    fi
else # Window
    exit 1;
fi

exit 0;