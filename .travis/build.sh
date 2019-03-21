#!/bin/bash

set -ev

if [[ $TRAVIS_OS_NAME == 'linux' ]]; then
    echo "start docker build..."
    DIR=`pwd`;sudo docker run -it -v ${DIR}:/home/ci zhixiangxiao/build-env:xenial /bin/sh -c "cd /home/ci; sh build -j2 ${ARCH} ci"
else
    sh build -j2 ${ARCH} ci
fi

exit 0;