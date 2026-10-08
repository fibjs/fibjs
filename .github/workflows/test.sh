#!/bin/bash

set -ev

DIST_EXEC="bin/${BUILD_OS}_${BUILD_ARCH}_${BUILD_TYPE}/fibjs"
if [[ $HOST_OS == 'Linux' ]]; then
    CUR=$(pwd)

    # Launch the binary through its absolute path.  The qemu-user emulation
    # behind the non-x64 architectures resolves /proc/self/exe from argv[0], so
    # a relative path stops resolving as soon as a test calls process.chdir()
    # and process.execPath (plus everything built on it, e.g. spawning child
    # fibjs processes) starts failing with ENOENT.
    DIST_EXEC="${CUR}/${DIST_EXEC}"

    TEST_CMD="${DIST_EXEC} test"

    if [[ $BUILD_TARGET == "android" ]]; then
        # The Android userland of the build environment image is not complete:
        # os.tmpdir() reports /data/local/tmp even though the directory does not
        # exist (so every temp file fails with ENOENT), bionic resolves names
        # through /system/etc/hosts, which is missing as well (so even
        # "localhost" fails with EAI_NODATA), and fibjs runs shell commands
        # through /system/bin/sh, which is not there either.  Provide all three,
        # otherwise the file, network and child_process suites cannot run.
        TEST_CMD="mkdir -p /data/local/tmp /system/etc /system/bin && cp -f /etc/hosts /system/etc/hosts && ln -sf /bin/sh /system/bin/sh && ${TEST_CMD}"
    fi

    if [[ $BUILD_TARGET == "linux" && $BUILD_ARCH == 'x64' ]]; then
        TEST_IMAGE=fibjs/ubuntu:10.04
    else
        TEST_IMAGE=fibjs/${BUILD_TARGET}-build-env:${BUILD_ARCH}
    fi

    # A stuck test process must not keep the job alive until GitHub's 6 hour
    # limit.  A crashed guest whose crash handler cannot run (bionic's
    # debuggerd dispatch thread has nothing to talk to inside the container) or
    # a livelocked emulated thread leaves the process spinning forever with no
    # output at all - that is what the android/arm64 job did, hanging for hours
    # right after the "path" suite header.
    #
    # The two budgets below come from the job logs of the 2026-10-08 runs.  The
    # native targets finish the suite in 8-12 minutes (ia32 664s, alpine/x64
    # 648s, android/x64 492s), so a short leash is enough to catch a hang.  The
    # qemu-emulated targets need 20-40 minutes for the same suite and swing by
    # +-50% from one runner to the next (same run: riscv64 2186s, ppc64 2198s,
    # loong64 2071s, arm64 2400s killed, arm 2400s killed, while a 2026-09-22
    # runner needed 849-1434s for the same jobs), so their budget has to cover
    # the slow end of that range: the previous 2400s cap killed runs that had
    # already printed 96% of the suite output.  Raise TEST_TIMEOUT to override.
    if [[ $BUILD_ARCH == 'x64' || $BUILD_ARCH == 'ia32' ]]; then
        TEST_TIMEOUT=${TEST_TIMEOUT:-1800}
    else
        TEST_TIMEOUT=${TEST_TIMEOUT:-5400}
    fi
    TEST_CONTAINER=fibjs-test-${BUILD_OS}-${BUILD_ARCH}

    TEST_EXIT=0
    timeout -s KILL ${TEST_TIMEOUT} docker run -t --rm --name ${TEST_CONTAINER} --privileged \
        -e CI=${CI} -v ${CUR}:${CUR} ${TEST_IMAGE} bash -c "cd ${CUR}; ${TEST_CMD}" || TEST_EXIT=$?

    if [[ ${TEST_EXIT} == 137 ]]; then
        # timeout killed the client, not the container; drop the stuck one so it
        # cannot keep burning a core for the rest of the job
        docker rm -f ${TEST_CONTAINER} >/dev/null 2>&1 || true

        echo "::error::the test run did not finish within ${TEST_TIMEOUT}s and was killed: the process is stuck, not merely slow (no known run needs more than two thirds of that)."
        exit 1
    fi

    exit ${TEST_EXIT}
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
    # On Windows the build produces fibjs.exe; without the extension the shell
    # cannot find it and the whole step fails with exit code 127.
    if [[ $HOST_OS == 'Windows' && ! -f "${DIST_EXEC}" ]]; then
        DIST_EXEC="${DIST_EXEC}.exe"
    fi

    ${DIST_EXEC} test
fi

exit 0
