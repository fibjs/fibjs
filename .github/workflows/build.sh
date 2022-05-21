#!/bin/bash

set -ev

if [[ ! -z $USE_VENDER_DIST ]]; then
    ./scripts/download_vender.sh;
fi

if [[ "$TARGET_OS" != 'Windows' ]]; then
    sh build -j2 ${TARGET_ARCH} ${BUILD_TYPE} ci
else # Window
    exit 1;
fi

exit 0;
