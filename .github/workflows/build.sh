#!/bin/bash

set -ev

if [[ ! -z $USE_VENDER_DIST ]]; then
    ./scripts/download_vender.sh;
fi

if [[ "$TARGET_OS" == 'Linux' ]]; then
    echo "start docker build..."
    DIR=`pwd`; docker run -t --env GIT_BRANCH=${GIT_BRANCH} --env USE_VENDER_DIST=${USE_VENDER_DIST} --env CI=ci -v ${DIR}:/home/ci fibjs/build-env:clang /bin/sh -c "cd /home/ci; sh build -j2 ${TARGET_ARCH} ${BUILD_TYPE} ci"
elif [[ "$TARGET_OS" == 'Darwin' ]]; then
    sh build -j2 ${TARGET_ARCH} ${BUILD_TYPE} ci
else # Window
    exit 1;
fi

exit 0;