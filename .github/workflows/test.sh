#!/bin/bash

set -ev

if [[ $TARGET_OS_NAME == 'Linux' ]]; then
    if [[ $TARGET_ARCH == "amd64" ]]; then
        # Test on ubuntu:18.04
        DIR=`pwd`;sudo docker run -t --env CI=$CI -v ${DIR}:/home/ci ubuntu:18.04 /bin/sh -c "cd /home/ci; ./bin/Linux_amd64_release/fibjs test/main.js"

        # Test on ubuntu:16.04
        DIR=`pwd`;sudo docker run -t --env CI=$CI -v ${DIR}:/home/ci ubuntu:16.04 /bin/sh -c "cd /home/ci; ./bin/Linux_amd64_release/fibjs test/main.js"

        # Test on ubuntu:14.04
        DIR=`pwd`;sudo docker run -t --env CI=$CI -v ${DIR}:/home/ci ubuntu:14.04 /bin/sh -c "cd /home/ci; ./bin/Linux_amd64_release/fibjs test/main.js"
    fi

    if [[ $TARGET_ARCH == "arm" ]]; then
        # Test on arm using qemu
        sudo docker run --rm --privileged multiarch/qemu-user-static:register
        DIR=`pwd`;sudo docker run --privileged=true -t --env CI=$CI -v ${DIR}:/home/ci fibjs/build-env:clang /bin/sh -c "
        cd /home;
        sh init_armhf.sh;
        cp -f ./ci/bin/$DIST_DIR/fibjs ./arm_root_fs/bin/fibjs;
        cp -rf ./ci/test ./arm_root_fs/home/test
        chroot ./arm_root_fs fibjs /home/test/main.js"
    fi

    if [[ $TARGET_ARCH == "arm64" ]]; then
        # Test on arm64 using qemu
        sudo docker run --rm --privileged multiarch/qemu-user-static:register
        DIR=`pwd`;sudo docker run --privileged=true -t --env CI=$CI -v ${DIR}:/home/ci fibjs/build-env:clang /bin/sh -c "
        cd /home;
        sh init_arm64.sh;
        cp -f ./ci/bin/$DIST_DIR/fibjs ./arm64_root_fs/bin/fibjs;
        cp -rf ./ci/test ./arm64_root_fs/home/test
        chroot ./arm64_root_fs fibjs /home/test/main.js"
    fi
else # Windows/Darwin
    ./bin/$DIST_DIR/fibjs test
fi

exit 0;