#!/bin/bash

set -ev

if [[ $TRAVIS_OS_NAME == 'linux' ]]; then
    echo "start docker build..."
    DIR=`pwd`;sudo  docker run -it -v ${DIR}:/home/ci fibjs/build-env:latest /bin/sh -c "cd /home/ci; sh build -j2 ${ARCH} ci"
else
    sh build -j2 ${ARCH}
fi

exit 0;