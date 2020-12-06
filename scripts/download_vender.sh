#!/usr/bin/env sh

. vender/scripts/get_env.sh --source-only

get_build_env;

if [ ! -z $TARGET_ARCH ]; then
  VENDER_ARCH=${TARGET_ARCH}
else
  VENDER_ARCH=${HOST_ARCH}
fi

echo "VENDER_ARCH to downloaed is $VENDER_ARCH";
VENDER_OS=`echo "$HOST_OS" | tr "[:upper:]" "[:lower:]"`

case "${VENDER_ARCH}" in
  i386)
    VENDER_ARCH=x86
      ;;
  amd64)
    VENDER_ARCH=x64
      ;;
esac

VENDER_ASSETS_FILE="vender-$VENDER_OS-$VENDER_ARCH-$BUILD_TYPE.zip"

# get vender name

FIBJS_DIR=`pwd`
FIBJS_VENDER_DIR="$FIBJS_DIR/vender"
BIN_DIR="$FIBJS_DIR/bin"

cd $FIBJS_VENDER_DIR

LATEST_REV_CROSS_ALL_BRANCHES=`git rev-list --tags --max-count=1`
LATEST_VENDER_TAG=`git describe --tags $LATEST_REV_CROSS_ALL_BRANCHES`

echo "LATEST_VENDER_TAG is $LATEST_VENDER_TAG"

cd $FIBJS_DIR

# ensure bin directory exsited
mkdir -p $BIN_DIR
cd $BIN_DIR;
if [ ! -e $BIN_DIR/$VENDER_ASSETS_FILE ]; then
    ASSET_URL=https://github.com/fibjs/fibjs_vender/releases/download/$LATEST_VENDER_TAG/$VENDER_ASSETS_FILE
    echo "starting download vender from $ASSET_URL"
    curl -sL $ASSET_URL -o $BIN_DIR/$VENDER_ASSETS_FILE
    ls -la ./;
    echo "download vender success!"
fi

if [ ! -e $BIN_DIR/$VENDER_ASSETS_FILE ]; then
    echo "no built vender found"
    unset USE_VENDER_DIST
else
    echo "built vender found, would extract it";
    unzip $BIN_DIR/$VENDER_ASSETS_FILE -d $BIN_DIR/;
fi

# go back to root
cd $FIBJS_DIR;
