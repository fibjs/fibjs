#!/bin/bash

# generate snapshot-<arch>-<os>.cc in repo's root

set -ev

if [[ $TARGET_OS_NAME == 'Linux' ]]; then
    if [[ $TARGET_ARCH == "arm" ]]; then
        # generate v8-snapshot on arm using qemu
        sudo docker run --rm --privileged multiarch/qemu-user-static:register
        DIR=`pwd`;sudo docker run --privileged=true -t --env CI=$CI -v ${DIR}:/home/ci fibjs/build-env:clang /bin/sh -c "
        cd /home;
        sh init_armhf.sh;
        cp -f ./ci/.dist/bin/Linux_arm_release/v8_test ./arm_root_fs/bin/v8_test;
        chroot ./arm_root_fs v8_test
        mv ./arm_root_fs/snapshot-arm-Linux.cc /home/ci/"
    elif [[ $TARGET_ARCH == "arm64" ]]; then
        # generate v8-snapshot on arm64 using qemu
        sudo docker run --rm --privileged multiarch/qemu-user-static:register
        DIR=`pwd`;sudo docker run --privileged=true -t --env CI=$CI -v ${DIR}:/home/ci fibjs/build-env:clang /bin/sh -c "
        cd /home;
        sh init_arm64.sh;
        cp -f ./ci/.dist/bin/Linux_arm64_release/v8_test ./arm64_root_fs/bin/v8_test;
        chroot ./arm64_root_fs v8_test
        mv ./arm64_root_fs/snapshot-arm64-Linux.cc /home/ci/"
    else
        .dist/bin/$DIST_DIR/v8_test
    fi
else # Darwin/Windows
    .dist/bin/$DIST_DIR/v8_test
fi

exit 0;