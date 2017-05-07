#!/bin/bash

set -ev

if [[ $TRAVIS_OS_NAME == 'linux' ]]; then
    echo "start docker build..."
    DIR=`pwd`;sudo  docker run -it -v ${DIR}:/home/ci fibjs/fibjs:build-env /bin/sh -c "cd /home/ci; sh build -j2 ${ARCH}"
else
    sh build -j2 ${ARCH}
fi

exit 0;