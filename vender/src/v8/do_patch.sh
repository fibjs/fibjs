#!/bin/bash

V8_PATH=~/Downloads/source/js/v8

cp $V8_PATH/include/*.h include/
cp $V8_PATH/include/*.h ../../include/v8/

cp $V8_PATH/src/*.h src/
cp $V8_PATH/src/*.cc src/

cp $V8_PATH/out/ia32.release/obj/gen/libraries.cc src/
cp $V8_PATH/out/ia32.release/obj/gen/trig-table.cc src/
cp $V8_PATH/out/ia32.release/obj/gen/experimental-libraries.cc src/

cp $V8_PATH/src/arm/*.h src/arm/
cp $V8_PATH/src/arm/*.cc src/arm/

cp $V8_PATH/src/arm64/*.h src/arm64/
cp $V8_PATH/src/arm64/*.cc src/arm64/

cp $V8_PATH/src/ia32/*.h src/ia32/
cp $V8_PATH/src/ia32/*.cc src/ia32/

cp $V8_PATH/src/x64/*.h src/x64/
cp $V8_PATH/src/x64/*.cc src/x64/

cp $V8_PATH/src/extensions/*.h src/extensions/
cp $V8_PATH/src/extensions/*.cc src/extensions/

cp $V8_PATH/src/libplatform/*.h src/libplatform/
cp $V8_PATH/src/libplatform/*.cc src/libplatform/

cp $V8_PATH/src/libplatform/*.h src/libplatform/
cp $V8_PATH/src/libplatform/*.cc src/libplatform/

cp $V8_PATH/src/platform/elapsed-timer.h src/platform/
cp $V8_PATH/src/platform/socket* src/platform/
cp $V8_PATH/src/platform/time* src/platform/

cp $V8_PATH/src/third_party/valgrind/valgrind.h src/third_party/valgrind/

cp $V8_PATH/src/utils/*.h src/utils/
cp $V8_PATH/src/utils/*.cc src/utils/

rm src/d8*
rm src/i18n.*
rm src/mksnapshot.cc
rm src/v8dll-main.cc
rm src/platform-cygwin.cc

patch -p0 --no-backup-if-mismatch < v8.patch
