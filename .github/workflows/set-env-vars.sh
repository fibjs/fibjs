. ./vender/scripts/get_env.sh --source-only
get_build_env;

echo "::set-output name=TARGET_OS::$TARGET_OS"

export GIT_BRANCH=${GITHUB_REF#refs/heads/}
echo "::set-output name=GIT_BRANCH::$GIT_BRANCH"
export GIT_TAG=$(git tag | grep $(git describe --tags HEAD))
echo "::set-output name=GIT_TAG::$GIT_TAG"
export GIT_COMMIT_HEAD_MSG=$(git log --format=%b -1)
echo "::set-output name=GIT_COMMIT_HEAD_MSG::$GIT_COMMIT_HEAD_MSG"
export GIT_COMMIT_SHORTCUTS=$(git log --format=%h -1)
echo "::set-output name=GIT_COMMIT_SHORTCUTS::$GIT_COMMIT_SHORTCUTS"
export GIT_COMMIT_TIME=$(git show -s --format="%cd" --date=format:%Y%m%d%H%M%S HEAD)
echo "::set-output name=GIT_COMMIT_TIME::$GIT_COMMIT_TIME"

if [[ "$GIT_TAG" =~ ^v?[012]\.[0-9]+\.[0-9]+$ ]]; then
    export IS_GIT_TAG_MATCH_SEMVER="true"
    echo "::set-output name=IS_GIT_TAG_MATCH_SEMVER::$IS_GIT_TAG_MATCH_SEMVER"
fi

if [ "$GITHUB_REPOSITORY" == "fibjs/fibjs" ]; then
    export IS_ORIGIN_REPO=1
fi

if [[ -z "$IS_ORIGIN_REPO" || ! -z "$GIT_TAG" ]]; then
    echo "::set-output name=IS_UPLOAD_ASSETS::1"
fi

if [ -z "$GIT_TAG" ]; then
    export RELEASE_TAG="$GIT_COMMIT_TIME-$GIT_COMMIT_SHORTCUTS";
else
    export RELEASE_TAG="$GIT_TAG";
fi
if [ -z "$IS_GIT_TAG_MATCH_SEMVER" ]; then
    SUFFIX=${GIT_BRANCH//\//'-'}
    RELEASE_TAG="$RELEASE_TAG-$SUFFIX"
fi
echo "::set-output name=RELEASE_TAG::$RELEASE_TAG"

git fetch;
if [ $(git tag --list | egrep "^$RELEASE_TAG$") ]; then
    echo "tag $RELEASE_TAG existed";
    export RELEASE_TAG_FROM_GIT_ORIGIN="YES"
else
    export RELEASE_TAG_FROM_GIT_ORIGIN=""
fi
echo "::set-output name=RELEASE_TAG_FROM_GIT_ORIGIN::$RELEASE_TAG_FROM_GIT_ORIGIN"

case "${TARGET_ARCH}" in
    i386)
        DIST_ARCH=x86
        SNAPSHOT_ARCH=ia32
        ;;
    amd64)
        DIST_ARCH=x64
        SNAPSHOT_ARCH=x64
        ;;
    *)
        DIST_ARCH=$TARGET_ARCH
        SNAPSHOT_ARCH=$TARGET_ARCH
        ;;
esac

if [[ "$RUNNER_OS" == "Linux" ]]; then
    export TARGET_OS_NAME="Linux";
    export DIST_DIR="${TARGET_OS_NAME}_${TARGET_ARCH}_$BUILD_TYPE"
fi

if [[ "$RUNNER_OS" == "macOS" ]]; then
    export TARGET_OS_NAME="Darwin";
    export DIST_DIR="${TARGET_OS_NAME}_${TARGET_ARCH}_$BUILD_TYPE"
fi

if [[ "$RUNNER_OS" == "Windows" ]]; then
    export TARGET_OS_NAME="Windows";
    export DIST_DIR="${TARGET_OS_NAME}_${TARGET_ARCH}_$BUILD_TYPE"
fi

echo "::set-output name=TARGET_OS_NAME::$TARGET_OS_NAME"
echo "::set-output name=DIST_DIR::$DIST_DIR"
echo "::set-output name=DIST_ARCH::$DIST_ARCH"
echo "::set-output name=HOST_ARCH::$HOST_ARCH"