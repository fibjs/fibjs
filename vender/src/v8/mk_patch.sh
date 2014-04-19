#!/bin/bash

V8_PATH=~/Downloads/source/js/v8

diff -u $V8_PATH/src/arm/deoptimizer-arm.cc src/arm/deoptimizer-arm.cc > v8.patch
diff -u $V8_PATH/src/arm/lithium-arm.cc src/arm/lithium-arm.cc >> v8.patch
diff -u $V8_PATH/src/arm/lithium-codegen-arm.cc src/arm/lithium-codegen-arm.cc >> v8.patch
diff -u $V8_PATH/src/arm/lithium-gap-resolver-arm.cc src/arm/lithium-gap-resolver-arm.cc >> v8.patch
diff -u $V8_PATH/src/arm64/deoptimizer-arm64.cc src/arm64/deoptimizer-arm64.cc >> v8.patch
diff -u $V8_PATH/src/arm64/instrument-arm64.cc src/arm64/instrument-arm64.cc >> v8.patch
diff -u $V8_PATH/src/arm64/lithium-arm64.cc src/arm64/lithium-arm64.cc >> v8.patch
diff -u $V8_PATH/src/arm64/lithium-codegen-arm64.cc src/arm64/lithium-codegen-arm64.cc >> v8.patch
diff -u $V8_PATH/src/arm64/lithium-gap-resolver-arm64.cc src/arm64/lithium-gap-resolver-arm64.cc >> v8.patch
diff -u $V8_PATH/src/platform-freebsd.cc src/platform-freebsd.cc >> v8.patch
diff -u $V8_PATH/src/platform-linux.cc src/platform-linux.cc >> v8.patch
diff -u $V8_PATH/src/platform-macos.cc src/platform-macos.cc >> v8.patch
diff -u $V8_PATH/src/platform-openbsd.cc src/platform-openbsd.cc >> v8.patch
diff -u $V8_PATH/src/platform-posix.cc src/platform-posix.cc >> v8.patch
diff -u $V8_PATH/src/platform-qnx.cc src/platform-qnx.cc >> v8.patch
diff -u $V8_PATH/src/platform-solaris.cc src/platform-solaris.cc >> v8.patch
diff -u $V8_PATH/src/platform-win32.cc src/platform-win32.cc >> v8.patch
diff -u $V8_PATH/src/platform.h src/platform.h >> v8.patch