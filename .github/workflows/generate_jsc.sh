#!/bin/bash

# generate jsc_test-<arch>.jsc, jsc_test-<arch>-<os>.jsc

set -ev

if [[ $TARGET_OS_NAME == 'Linux' ]]; then
    if [[ $TARGET_ARCH == "arm" ]]; then # generate jsc on arm using qemu
        docker run --rm --privileged multiarch/qemu-user-static:register
        DIR=`pwd`;docker run --privileged=true -t --env CI=$CI -v ${DIR}:/home/ci fibjs/build-env:clang /bin/sh -c "
        cd /home;
        sh init_armhf.sh;
        cp -f ./ci/bin/Linux_arm_release/fibjs ./arm_root_fs/bin/fibjs;
        cp -f ./ci/test/vm_test/build_jsc.js ./arm_root_fs/build_jsc.js;
        chroot ./arm_root_fs fibjs ./build_jsc.js;
        ls -la /home/arm_root_fs | grep jsc_test;
        cp -f /home/arm_root_fs/jsc_test*.jsc /home/ci/test/vm_test/;"
    elif [[ $TARGET_ARCH == "arm64" ]]; then # generate jsc on arm64 using qemu
        docker run --rm --privileged multiarch/qemu-user-static:register
        DIR=`pwd`;docker run --privileged=true -t --env CI=$CI -v ${DIR}:/home/ci fibjs/build-env:clang /bin/sh -c "
        cd /home;
        sh init_arm64.sh;
        cp -f ./ci/bin/Linux_arm64_release/fibjs ./arm64_root_fs/bin/fibjs;
        cp -f ./ci/test/vm_test/build_jsc.js ./arm64_root_fs/build_jsc.js;
        chroot ./arm64_root_fs fibjs ./build_jsc.js;
        ls -la /home/arm64_root_fs | grep jsc_test;
        cp -f /home/arm64_root_fs/jsc_test*.jsc /home/ci/test/vm_test/;"
    else # Other Linux
        ./bin/$DIST_DIR/fibjs ./test/vm_test/build_jsc.js
    fi
else # Darwin/Windows
    ./bin/$DIST_DIR/fibjs ./test/vm_test/build_jsc.js
fi

exit 0;