#!/bin/bash

set -ev

DIST_EXEC="bin/${BUILD_OS}_${BUILD_ARCH}_${BUILD_TYPE}/fibjs"
if [[ $HOST_OS == 'Linux' ]]; then
    CUR=$(pwd)

    if [[ $BUILD_TARGET == "linux" && $BUILD_ARCH == 'x64' ]]; then
        docker run -t --rm -e CI=${CI} -v ${CUR}:${CUR} fibjs/ubuntu:10.04 bash -c "cd ${CUR}; ${DIST_EXEC} test"
    else
        docker run -t --rm -e CI=${CI} -v ${CUR}:${CUR} fibjs/${BUILD_TARGET}-build-env:${BUILD_ARCH} bash -c "cd ${CUR}; ${DIST_EXEC} test"
    fi
else # Windows/Darwin
    ${DIST_EXEC} test
fi

exit 0
