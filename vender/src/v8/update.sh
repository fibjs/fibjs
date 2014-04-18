V8_PATH=~/Downloads/source/js/v8

WORK_PATH=`pwd`
cd $V8_PATH

svn revert -R .
svn up
make ia32.release

cd $WORK_PATH

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