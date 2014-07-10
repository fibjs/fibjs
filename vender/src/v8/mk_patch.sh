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
diff -u $V8_PATH/src/base/platform/platform-freebsd.cc src/base/platform/platform-freebsd.cc >> v8.patch
diff -u $V8_PATH/src/base/platform/platform-linux.cc src/base/platform/platform-linux.cc >> v8.patch
diff -u $V8_PATH/src/base/platform/platform-macos.cc src/base/platform/platform-macos.cc >> v8.patch
diff -u $V8_PATH/src/base/platform/platform-openbsd.cc src/base/platform/platform-openbsd.cc >> v8.patch
diff -u $V8_PATH/src/base/platform/platform-posix.cc src/base/platform/platform-posix.cc >> v8.patch
diff -u $V8_PATH/src/base/platform/platform-qnx.cc src/base/platform/platform-qnx.cc >> v8.patch
diff -u $V8_PATH/src/base/platform/platform-solaris.cc src/base/platform/platform-solaris.cc >> v8.patch
diff -u $V8_PATH/src/base/platform/platform-win32.cc src/base/platform/platform-win32.cc >> v8.patch
diff -u $V8_PATH/src/base/platform/platform.h src/base/platform/platform.h >> v8.patch