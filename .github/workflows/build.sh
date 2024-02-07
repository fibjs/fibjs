set -ev

if [[ "${HOST_OS}" == "Linux" ]]; then
    CUR=$(pwd)
    docker run -t --rm -v ${CUR}:${CUR} fibjs/${BUILD_TARGET}-build-env:${BUILD_ARCH} \
        bash -c "cd ${CUR}; git config --global --add safe.directory ${CUR}; bash build -j2 ${BUILD_ARCH} ${BUILD_TYPE} ci"
else
    bash build -j2 ${BUILD_ARCH} ${BUILD_TARGET} ${BUILD_TYPE} ci
fi

if [[ "${BUILD_TAG}" != "" && "${BUILD_TYPE}" == "release" ]]; then
    mkdir release

    if [ "${HOST_OS}" == "Windows" ]; then
        cp bin/${BUILD_OS}_${BUILD_ARCH}_${BUILD_TYPE}/fibjs.exe release/fibjs-${BUILD_TAG}-${BUILD_TARGET}-${BUILD_ARCH}.exe
        cp bin/${BUILD_OS}_${BUILD_ARCH}_${BUILD_TYPE}/fibjs_gui.exe release/fibjs-${BUILD_TAG}-${BUILD_TARGET}-gui-${BUILD_ARCH}.exe
        cp bin/${BUILD_OS}_${BUILD_ARCH}_${BUILD_TYPE}/installer.exe release/installer-${BUILD_TAG}-${BUILD_TARGET}-${BUILD_ARCH}.exe
        cp bin/${BUILD_OS}_${BUILD_ARCH}_${BUILD_TYPE}/fibjs.cab release/fibjs-${BUILD_TAG}-${BUILD_TARGET}-${BUILD_ARCH}.cab
        7z a -tzip release/fibjs-${BUILD_TAG}-${BUILD_TARGET}-${BUILD_ARCH}.zip bin/${BUILD_OS}_${BUILD_ARCH}_${BUILD_TYPE}/fibjs.exe
        7z a -tzip release/fibjs-${BUILD_TAG}-${BUILD_TARGET}-gui-${BUILD_ARCH}.zip bin/${BUILD_OS}_${BUILD_ARCH}_${BUILD_TYPE}/fibjs_gui.exe
    else
        cp bin/${BUILD_OS}_${BUILD_ARCH}_${BUILD_TYPE}/fibjs release/fibjs-${BUILD_TAG}-${BUILD_TARGET}-${BUILD_ARCH}
        cp bin/${BUILD_OS}_${BUILD_ARCH}_${BUILD_TYPE}/installer.sh release/installer-${BUILD_TAG}-${BUILD_TARGET}-${BUILD_ARCH}.sh
        cp bin/${BUILD_OS}_${BUILD_ARCH}_${BUILD_TYPE}/fibjs.tar.gz release/fibjs-${BUILD_TAG}-${BUILD_TARGET}-${BUILD_ARCH}.tar.gz
    fi
fi
