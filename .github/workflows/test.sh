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
        if [[ $BUILD_ARCH == "x64" ]]; then
            # A fatal signal must leave the faulting thread's registers and maps
            # in the job log: bionic's handler prints only the fault address and
            # the core written inside the container does not carry the crashing
            # thread, which made the android/x64 SIGSEGV undebuggable.  The
            # reporter is built by the image's own $CC and clears LD_PRELOAD
            # from the environment, so the suite's glibc children (sh, node) are
            # not affected by it.
            CRASH_REPORT="\$CC -O1 -fPIC -shared -o /tmp/crash_report.so ${CUR}/tools/ci/crash_report.c && LD_PRELOAD=/tmp/crash_report.so "
        else
            CRASH_REPORT=""
        fi
        TEST_CMD="mkdir -p /data/local/tmp /system/etc /system/bin && cp -f /etc/hosts /system/etc/hosts && ln -sf /bin/sh /system/bin/sh && ${CRASH_REPORT}${TEST_CMD}"
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
    # The two budgets below are wall-clock limits for the whole suite.  They
    # exist only to catch a livelocked/hung run, so they are deliberately much
    # larger than the slowest observed pass.  Raise TEST_TIMEOUT to override.
    #
    # Native targets finished in 8-12 minutes in the 2026-10/2026-11 runs
    # (ia32 664s, alpine/x64 648s, android/x64 492s), but the glibc ia32 job
    # was killed at the old 1800s cap in runs 45854076014 and 45910300007
    # after printing roughly half of the suite, while alpine/ia32 (the same
    # 32-bit arch) finished in 703s in the latter run - so 1800s has too
    # little headroom (and may be hiding a hang).  Native gets 7200s (2h).
    #
    # qemu-emulated targets normally need 40-50 minutes (2026-11-22: arm64
    # 2978s, loong64 2562s, arm 2501s, android/arm64 2466s, ppc64 2376s;
    # 2026-10-08: riscv64 2186s, ppc64 2198s, loong64 2071s; older runners
    # needed 849-1434s), but ppc64 was already killed at the previous 6300s
    # cap (run 45854076014), several emulated jobs of run 45910300007 were
    # still running after 40+ minutes, and the suite keeps growing.  They get
    # 12600s (3.5h); 3.5h of tests plus the longest observed build (~50 min)
    # stays below GitHub's 6h job limit.
    if [[ $BUILD_ARCH == 'x64' || $BUILD_ARCH == 'ia32' ]]; then
        TEST_TIMEOUT=${TEST_TIMEOUT:-7200}
    else
        TEST_TIMEOUT=${TEST_TIMEOUT:-12600}
    fi
    # qemu 10, which docker/setup-qemu-action registers since the ubuntu 26.04
    # migration, defaults the riscv64 CPU to the "max" model.  The TCG code
    # behind the extensions that model enables is about four times slower than
    # the plain rv64 model for compute-heavy suites: on the same binary,
    # zlib needs 72s instead of 17s, xml 332s instead of 83s and the
    # worker_threads suite 79s instead of 33s.  With the default the full
    # suite no longer fitted the budget above and the riscv64 job of run
    # 45002444438 was killed at 5400s in the middle of
    # worker_threads_fibjs_test; with rv64 the target runs in line with the
    # other emulated targets.  QEMU_CPU is read by qemu itself, so it also
    # applies to every exec binfmt performs for a child process.
    TEST_ENV=""
    if [[ $BUILD_ARCH == 'riscv64' ]]; then
        TEST_ENV="-e QEMU_CPU=rv64"
    fi

    TEST_CONTAINER=fibjs-test-${BUILD_OS}-${BUILD_ARCH}

    TEST_EXIT=0
    timeout --preserve-status -s KILL ${TEST_TIMEOUT} docker run -t --rm --name ${TEST_CONTAINER} --privileged \
        -e CI=${CI} ${TEST_ENV} -v ${CUR}:${CUR} ${TEST_IMAGE} bash -c "cd ${CUR}; ${TEST_CMD}" || TEST_EXIT=$?

    # --preserve-status keeps the SIGKILL exit code (137) visible: without it
    # GNU coreutils <= 9.4 already reported 137 but the uutils implementation
    # that ubuntu 26.04 (and therefore ubuntu-latest) ships reports 124, which
    # is also the code the test runner's own watchdog uses when a leaked handle
    # keeps the process alive after the run.  With --preserve-status the two
    # are distinguishable again.
    if [[ ${TEST_EXIT} == 137 ]]; then
        # timeout killed the client, not the container; drop the stuck one so it
        # cannot keep burning a core for the rest of the job
        docker rm -f ${TEST_CONTAINER} >/dev/null 2>&1 || true

        echo "::error::the test run did not finish within ${TEST_TIMEOUT}s and was killed: if the log was still advancing it was slow rather than stuck (raise TEST_TIMEOUT); if it stopped, treat it as a hang."
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
