#!/bin/bash

set -ev

TARGET_ARCH=${ARCH}

case "${ARCH}" in
  i386)
    TARGET_ARCH=x86
      ;;
  amd64)
    TARGET_ARCH=x64
      ;;
esac

mkdir -p ${TRAVIS_TAG}

DIST_FILE=""

if [[ $TRAVIS_OS_NAME == 'linux' ]]; then # linux
  DIST_FILE=bin/Linux_${ARCH}_release

  FIBJS_FILE=${DIST_FILE}/fibjs
  INSTALLER_FILE=${DIST_FILE}/installer.sh
  XZ_FILE=${DIST_FILE}/fibjs.xz
  GZ_FILE=${DIST_FILE}/fibjs.tar.gz

  cp ${FIBJS_FILE} ${TRAVIS_TAG}/fibjs-${TRAVIS_TAG}-linux-${TARGET_ARCH}
  cp ${INSTALLER_FILE} ${TRAVIS_TAG}/installer-${TRAVIS_TAG}-linux-${TARGET_ARCH}.sh
  cp ${XZ_FILE} ${TRAVIS_TAG}/fibjs-${TRAVIS_TAG}-linux-${TARGET_ARCH}.xz
  cp ${GZ_FILE} ${TRAVIS_TAG}/fibjs-${TRAVIS_TAG}-linux-${TARGET_ARCH}.tar.gz

  if [[ $TARGET_ARCH == 'x64' ]]; then
    echo "zip fullsrc..."

    mkdir fullsrc
    cp -rf vender fibjs build fibjs.sln README.md LICENSE.md CHANGELOG.md ./fullsrc
    zip -r ./${TRAVIS_TAG}/fullsrc.zip ./fullsrc
  fi

else # darwin
  DIST_FILE=bin/Darwin_amd64_release

  FIBJS_FILE=${DIST_FILE}/fibjs
  INSTALLER_FILE=${DIST_FILE}/installer.sh
  XZ_FILE=${DIST_FILE}/fibjs.xz
  GZ_FILE=${DIST_FILE}/fibjs.tar.gz

  cp ${FIBJS_FILE} ${TRAVIS_TAG}/fibjs-${TRAVIS_TAG}-darwin-x64
  cp ${INSTALLER_FILE} ${TRAVIS_TAG}/installer-${TRAVIS_TAG}-darwin-x64.sh
  cp ${XZ_FILE} ${TRAVIS_TAG}/fibjs-${TRAVIS_TAG}-darwin-x64.xz
  cp ${GZ_FILE} ${TRAVIS_TAG}/fibjs-${TRAVIS_TAG}-darwin-x64.tar.gz
fi

exit 0;
