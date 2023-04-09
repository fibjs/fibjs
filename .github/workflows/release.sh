#!/bin/bash

set -ev

mkdir -p ${RELEASE_TAG}

DIST_DIRPATH="bin/${DIST_DIR}"
echo "DIST_DIRPATH is $DIST_DIRPATH"

if [[ $TARGET_OS_NAME == 'Linux' ]]; then # Linux
  FIBJS_FILE=${DIST_DIRPATH}/fibjs
  INSTALLER_FILE=${DIST_DIRPATH}/installer.sh
  XZ_FILE=${DIST_DIRPATH}/fibjs.xz
  GZ_FILE=${DIST_DIRPATH}/fibjs.tar.gz

  if [[ "$BUILD_TARGET" == "" ]]; then
    BUILD_TARGET="linux"
  fi

  cp ${FIBJS_FILE} ${RELEASE_TAG}/fibjs-${RELEASE_TAG}-${BUILD_TARGET}-${DIST_ARCH}
  cp ${INSTALLER_FILE} ${RELEASE_TAG}/installer-${RELEASE_TAG}-${BUILD_TARGET}-${DIST_ARCH}.sh
  cp ${XZ_FILE} ${RELEASE_TAG}/fibjs-${RELEASE_TAG}-${BUILD_TARGET}-${DIST_ARCH}.xz
  cp ${GZ_FILE} ${RELEASE_TAG}/fibjs-${RELEASE_TAG}-${BUILD_TARGET}-${DIST_ARCH}.tar.gz

elif [[ $TARGET_OS_NAME == 'Darwin' ]]; then # Darwin
  FIBJS_FILE=${DIST_DIRPATH}/fibjs
  INSTALLER_FILE=${DIST_DIRPATH}/installer.sh
  XZ_FILE=${DIST_DIRPATH}/fibjs.xz
  GZ_FILE=${DIST_DIRPATH}/fibjs.tar.gz

  if [[ "$BUILD_TARGET" == "" ]]; then
    BUILD_TARGET="darwin"
  fi

  cp ${FIBJS_FILE} ${RELEASE_TAG}/fibjs-${RELEASE_TAG}-${BUILD_TARGET}-${DIST_ARCH}
  cp ${INSTALLER_FILE} ${RELEASE_TAG}/installer-${RELEASE_TAG}-${BUILD_TARGET}-${DIST_ARCH}.sh
  cp ${XZ_FILE} ${RELEASE_TAG}/fibjs-${RELEASE_TAG}-${BUILD_TARGET}-${DIST_ARCH}.xz
  cp ${GZ_FILE} ${RELEASE_TAG}/fibjs-${RELEASE_TAG}-${BUILD_TARGET}-${DIST_ARCH}.tar.gz

  if [[ $DIST_ARCH == 'x64' ]]; then
    echo "zip fullsrc..."
    # sudo sh build clean
    zip -r ./${RELEASE_TAG}/fullsrc.zip ./ -x *.git* -x out/* -x bin/* ${RELEASE_TAG} ${RELEASE_TAG}/*
  fi
fi

echo "list files in '${RELEASE_TAG}'";
ls -la ./${RELEASE_TAG};

exit 0;
