#!/bin/bash

set -ev

DIST_EXEC="bin/${BUILD_OS}_${BUILD_ARCH}_${BUILD_TYPE}/fibjs"
if [[ $HOST_OS == 'Linux' ]]; then
    CUR=$(pwd)

    if [[ $BUILD_TARGET == "linux" && $BUILD_ARCH == 'x64' ]]; then
        docker run -t --rm --privileged -e CI=${CI} -v ${CUR}:${CUR} fibjs/ubuntu:10.04 bash -c "cd ${CUR}; ${DIST_EXEC} test"
    else
        docker run -t --rm --privileged -e CI=${CI} -v ${CUR}:${CUR} fibjs/${BUILD_TARGET}-build-env:${BUILD_ARCH} bash -c "cd ${CUR}; ${DIST_EXEC} test"
    fi
elif [[ $HOST_OS == 'Darwin' && $BUILD_OS == 'iPhoneSimulator' ]]; then
    # A simulator binary cannot be executed by the host directly: dyld aborts
    # with "DYLD_ROOT_PATH not set for simulator program", so the tests have to
    # run inside a booted simulator.  Three settings are needed there:
    #   * DYLD_ROOT_PATH has to be re-exported, because the host /bin/sh is a
    #     macOS binary and does not pass DYLD_* on to its children;
    #   * ulimit -n lifts the 256 fd soft limit of the simulator environment
    #     (the suite holds sockets until each test file's after hook);
    #   * PATH has to include the host tools the tests use (cat/sleep/echo/npm).
    CUR=$(pwd)
    UDID=$(xcrun simctl list devices available | grep -m1 "iPhone" | sed -E 's/.*\(([0-9A-F-]+)\).*/\1/')

    if [[ -z "${UDID}" ]]; then
        echo "no available iPhone simulator"
        exit 1
    fi

    xcrun simctl boot ${UDID} || true
    xcrun simctl bootstatus ${UDID} -b

    TEST_EXIT=0
    xcrun simctl spawn ${UDID} /bin/sh -c "
        export DYLD_ROOT_PATH=\"\$SIMULATOR_ROOT\"
        ulimit -n 10240
        export PATH=\"${PATH}\"
        cd ${CUR} && exec ./${DIST_EXEC} test" || TEST_EXIT=$?

    xcrun simctl shutdown ${UDID} || true

    exit ${TEST_EXIT}
else # Windows/Darwin
    ${DIST_EXEC} test
fi

exit 0
