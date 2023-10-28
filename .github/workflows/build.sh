set -ev

if [[ "${HOST_OS}" == "Linux" ]]; then
    CUR=$(pwd)
    docker run -t --rm -e USE_VENDER_DIST -v ${CUR}:${CUR} fibjs/${BUILD_TARGET}-build-env:${BUILD_ARCH} bash -c "cd ${CUR}; bash build -j2 ${BUILD_ARCH} ${BUILD_TYPE} ci"
else
    bash build -j2 ${BUILD_ARCH} ${BUILD_TARGET} ${BUILD_TYPE} ci
fi

if [[ "${BUILD_TAG}" != "" && "${BUILD_TYPE}" == "release" ]]; then
    cd bin/${BUILD_OS}_${BUILD_ARCH}_${BUILD_TYPE}
    mkdir release

    if [ "${HOST_OS}" == "Windows" ]; then
        cp fibjs.exe release/fibjs-${BUILD_TAG}-${BUILD_TARGET}-${BUILD_ARCH}.exe
        cp fibjs_gui.exe release/fibjs-${BUILD_TAG}-${BUILD_TARGET}-gui-${BUILD_ARCH}.exe
        cp installer.exe release/installer-${BUILD_TAG}-${BUILD_TARGET}-${BUILD_ARCH}.exe
        cp fibjs.cab release/fibjs-${BUILD_TAG}-${BUILD_TARGET}-${BUILD_ARCH}.cab
        7z a -tzip release/fibjs-${BUILD_TAG}-${BUILD_TARGET}-${BUILD_ARCH}.zip fibjs.exe
        7z a -tzip release/fibjs-${BUILD_TAG}-${BUILD_TARGET}-gui-${BUILD_ARCH}.zip fibjs_gui.exe
    else
        cp fibjs release/fibjs-${BUILD_TAG}-${BUILD_TARGET}-${BUILD_ARCH}
        cp installer.sh release/installer-${BUILD_TAG}-${BUILD_TARGET}-${BUILD_ARCH}.sh
        cp fibjs.xz release/fibjs-${BUILD_TAG}-${BUILD_TARGET}-${BUILD_ARCH}.xz
        cp fibjs.tar.gz release/fibjs-${BUILD_TAG}-${BUILD_TARGET}-${BUILD_ARCH}.tar.gz
    fi
fi
