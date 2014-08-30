#!/bin/bash

V8_PATH=~/Downloads/source/js/v8

cp $V8_PATH/include/*.h include/
cp $V8_PATH/include/*.h ../../include/v8/

rm src/*.h
rm src/*.cc
cp $V8_PATH/src/*.h src/
cp $V8_PATH/src/*.cc src/

rm src/base/*.h
rm src/base/*.cc
cp $V8_PATH/src/base/*.h src/base/
cp $V8_PATH/src/base/*.cc src/base/

cp $V8_PATH/src/base/platform/elapsed-timer.h src/base/platform/
cp $V8_PATH/src/base/platform/time* src/base/platform/
cp $V8_PATH/src/base/platform/platform* src/base/platform/

rm src/base/utils/*.h
rm src/base/utils/*.cc
cp $V8_PATH/src/base/utils/*.h src/base/utils/
cp $V8_PATH/src/base/utils/*.cc src/base/utils/

cp $V8_PATH/out/ia32.release/obj/gen/libraries.cc src/
cp $V8_PATH/out/ia32.release/obj/gen/experimental-libraries.cc src/

rm src/arm/*.h
rm src/arm/*.cc
cp $V8_PATH/src/arm/*.h src/arm/
cp $V8_PATH/src/arm/*.cc src/arm/

rm src/arm64/*.h
rm src/arm64/*.cc
cp $V8_PATH/src/arm64/*.h src/arm64/
cp $V8_PATH/src/arm64/*.cc src/arm64/

rm src/ia32/*.h
rm src/ia32/*.cc
cp $V8_PATH/src/ia32/*.h src/ia32/
cp $V8_PATH/src/ia32/*.cc src/ia32/

rm src/compiler/*.h
rm src/compiler/*.cc
cp $V8_PATH/src/compiler/*.h src/compiler/
cp $V8_PATH/src/compiler/*.cc src/compiler/

rm src/compiler/arm/*.h
rm src/compiler/arm/*.cc
cp $V8_PATH/src/compiler/arm/*.h src/compiler/arm/
cp $V8_PATH/src/compiler/arm/*.cc src/compiler/arm/

rm src/compiler/arm64/*.h
rm src/compiler/arm64/*.cc
cp $V8_PATH/src/compiler/arm64/*.h src/compiler/arm64/
cp $V8_PATH/src/compiler/arm64/*.cc src/compiler/arm64/

rm src/compiler/ia32/*.h
rm src/compiler/ia32/*.cc
cp $V8_PATH/src/compiler/ia32/*.h src/compiler/ia32/
cp $V8_PATH/src/compiler/ia32/*.cc src/compiler/ia32/

rm src/compiler/x64/*.h
rm src/compiler/x64/*.cc
cp $V8_PATH/src/compiler/x64/*.h src/compiler/x64/
cp $V8_PATH/src/compiler/x64/*.cc src/compiler/x64/

rm src/x64/*.h
rm src/x64/*.cc
cp $V8_PATH/src/x64/*.h src/x64/
cp $V8_PATH/src/x64/*.cc src/x64/

rm src/extensions/*.h
rm src/extensions/*.cc
cp $V8_PATH/src/extensions/*.h src/extensions/
cp $V8_PATH/src/extensions/*.cc src/extensions/

rm src/libplatform/*.h
rm src/libplatform/*.cc
cp $V8_PATH/src/libplatform/*.h src/libplatform/
cp $V8_PATH/src/libplatform/*.cc src/libplatform/

rm src/heap/*.h
rm src/heap/*.cc
cp $V8_PATH/src/heap/*.h src/heap/
cp $V8_PATH/src/heap/*.cc src/heap/

cp $V8_PATH/src/third_party/valgrind/valgrind.h src/third_party/valgrind/
cp $V8_PATH/src/third_party/kernel/tools/perf/util/jitdump.h src/third_party/kernel/tools/perf/util/
cp $V8_PATH/third_party/fdlibm/*.h src/third_party/fdlibm/
cp $V8_PATH/third_party/fdlibm/*.cc src/third_party/fdlibm/

rm src/d8*
rm src/i18n.*
rm src/natives-external.cc
rm src/snapshot-external.cc
rm src/mksnapshot.cc
rm src/v8dll-main.cc
rm src/base/platform/platform-cygwin.cc

patch -p0 --no-backup-if-mismatch < v8.patch
