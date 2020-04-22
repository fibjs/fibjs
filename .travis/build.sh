#!/bin/bash

set -ev

if [[ ! -z $USE_VENDER_DIST ]]; then
    ./scripts/download_vender.sh;
fi

if [[ $TRAVIS_OS_NAME == 'linux' ]]; then
    echo "start docker build..."
    DIR=`pwd`;
    sudo docker run \
        --env USE_VENDER_DIST="$USE_VENDER_DIST" \
        -it -v ${DIR}:/home/ci \
        fibjs/build-env:clang /bin/sh -c "cd /home/ci; sh build -j2 ${ARCH} ci"
else
    sh build -j2 ${ARCH} ci
fi

exit 0;