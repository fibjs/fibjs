#!/bin/bash

set -ev

if [[ $TRAVIS_OS_NAME == 'linux' ]]; then
    if [[ $ARCH == "amd64" ]]; then
        # Test on ubuntu:18.04
        DIR=`pwd`;sudo docker run -it -v ${DIR}:/home/ci ubuntu:18.04 /bin/sh -c "cd /home/ci; ./bin/Linux_amd64_release/fibjs test/main.js"

        # Test on ubuntu:16.04
        DIR=`pwd`;sudo docker run -it -v ${DIR}:/home/ci ubuntu:16.04 /bin/sh -c "cd /home/ci; ./bin/Linux_amd64_release/fibjs test/main.js"

        # Test on ubuntu:14.04
        DIR=`pwd`;sudo docker run -it -v ${DIR}:/home/ci ubuntu:14.04 /bin/sh -c "cd /home/ci; ./bin/Linux_amd64_release/fibjs test/main.js"
    fi

    if [[ $ARCH == "arm" ]]; then
        # Test on arm using qemu
        DIR=`pwd`;sudo docker run --privileged=true -it -v ${DIR}:/home/ci fibjs/build-env:clang /bin/sh -c "
        cd /home;
        sh init_armhf.sh;
        cp -f ./ci/bin/Linux_arm_release/fibjs ./arm_root_fs/bin/fibjs;
        cp -rf ./ci/test ./arm_root_fs/home/test
        chroot ./arm_root_fs fibjs /home/test/main.js"
    fi

    if [[ $ARCH == "arm64" ]]; then
        # Test on arm64 using qemu
        DIR=`pwd`;sudo docker run -it -v ${DIR}:/home/ci fibjs/build-env:clang /bin/sh -c "cd /home/ci; qemu-aarch64 ./bin/Linux_arm64_release/fibjs test/main.js"
    fi
else # darwin
    if [[ $ARCH == "amd64" ]]; then
        ./bin/Darwin_amd64_release/fibjs test
    else
        ./bin/Darwin_i386_release/fibjs test
    fi
fi

exit 0;