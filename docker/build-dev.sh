#!/bin/bash

# Creates a multi-architecture fibjs docker image
# Usage: ./build-dev.sh [OPTIONS]
# Options:
#   --push            Push image to registry after build
#   --skip-build      Skip compilation, use existing binaries
#   --help            Show this help message

set -e

# Disable BuildKit: the classic builder uses dockerd directly for pulling
# base images, which respects the proxy configured in daemon.json.
# BuildKit's docker-container driver does not honor proxy env vars for
# registry resolution, causing failures when Docker Hub is blocked.
export DOCKER_BUILDKIT=0

# Read proxy from Docker daemon config for CLI-side commands (docker manifest).
# dockerd itself reads daemon.json directly, but docker manifest does not.
DAEMON_CONFIG="/etc/docker/daemon.json"
CLI_HTTP_PROXY=""
CLI_HTTPS_PROXY=""
if [ -f "$DAEMON_CONFIG" ]; then
    CLI_HTTP_PROXY=$(grep '"http-proxy"' "$DAEMON_CONFIG" | sed 's/.*"http-proxy"[[:space:]]*:[[:space:]]*"\([^"]*\)".*/\1/')
    CLI_HTTPS_PROXY=$(grep '"https-proxy"' "$DAEMON_CONFIG" | sed 's/.*"https-proxy"[[:space:]]*:[[:space:]]*"\([^"]*\)".*/\1/')
fi

# Configuration
REGISTRY="fibjs"
IMAGE_NAME="dev"
TAG="latest"
PUSH="false"
SKIP_BUILD="false"

# Parse command line arguments
while [[ $# -gt 0 ]]; do
    case $1 in
        --push)
            PUSH="true"
            shift
            ;;
        --skip-build)
            SKIP_BUILD="true"
            shift
            ;;
        --help|-h)
            echo "Usage: $0 [OPTIONS]"
            echo ""
            echo "Creates a multi-architecture fibjs Docker image: fibjs/dev:latest"
            echo ""
            echo "Options:"
            echo "  --push            Push image to registry after build"
            echo "  --skip-build      Skip fibjs compilation, use existing binaries (useful for testing)"
            echo "  --help, -h        Show this help message"
            echo ""
            echo "Examples:"
            echo "  $0                     # Build fibjs binaries and create fibjs/dev:latest locally"
            echo "  $0 --push             # Build fibjs binaries and push fibjs/dev:latest to registry"
            echo "  $0 --skip-build       # Skip compilation, build Docker image using existing binaries"
            echo "  $0 --skip-build --push # Skip compilation and push using existing binaries"
            exit 0
            ;;
        *)
            echo "Unknown option: $1"
            echo "Use --help for usage information"
            exit 1
            ;;
    esac
done

# Supported architectures: Docker platform -> (fibjs build arch, arch suffix for tag)
# Note: Only including architectures supported by Ubuntu 24.04
declare -A ARCH_MAP=(
    ["linux/amd64"]="x64"
    ["linux/arm64"]="arm64"
    ["linux/arm/v7"]="arm"
    ["linux/ppc64le"]="ppc64"
    ["linux/riscv64"]="riscv64"
)

# Map platform to arch suffix used in intermediate tags
platform_suffix() {
    case "$1" in
        "linux/amd64")   echo "amd64" ;;
        "linux/arm64")   echo "arm64" ;;
        "linux/arm/v7")  echo "armv7" ;;
        "linux/ppc64le") echo "ppc64le" ;;
        "linux/riscv64") echo "riscv64" ;;
        *)               echo "unknown" ;;
    esac
}

echo "🏗️  Building multi-architecture fibjs Docker image..."
echo "Image: ${REGISTRY}/${IMAGE_NAME}:${TAG}"
echo "Push: ${PUSH}"
echo "Skip Build: ${SKIP_BUILD}"

# Build binaries for all architectures
if [ "${SKIP_BUILD}" = "true" ]; then
    echo "⏭️  Skipping binary build, using existing binaries..."
else
    echo "🔨 Building fibjs binaries for all architectures..."
    cd ..
    BUILD_FAILED=false
    for arch in "${ARCH_MAP[@]}"; do
        echo "Building for ${arch}..."
        if bash build dev linux "${arch}"; then
            echo "✅ Built ${arch}"
        else
            echo "❌ Failed to build ${arch}"
            BUILD_FAILED=true
        fi
    done
    cd docker

    if [ "$BUILD_FAILED" = "true" ]; then
        echo "❌ One or more builds failed! Exiting..."
        exit 1
    fi
    echo "✅ All binary builds completed successfully!"
fi

# Copy binaries to build context
echo "📦 Copying binaries..."
rm -rf binaries
mkdir -p binaries
BUILT_PLATFORMS=()

for platform in "${!ARCH_MAP[@]}"; do
    arch="${ARCH_MAP[$platform]}"
    binary_path="../bin/Linux_${arch}_release/fibjs"

    # Map architecture names to Docker platform names for file naming
    case "$platform" in
        "linux/amd64") binary_name="fibjs-amd64" ;;
        "linux/arm64") binary_name="fibjs-arm64" ;;
        "linux/arm/v7") binary_name="fibjs-arm" ;;
        "linux/ppc64le") binary_name="fibjs-ppc64le" ;;
        "linux/riscv64") binary_name="fibjs-riscv64" ;;
        *) binary_name="fibjs-unknown" ;;
    esac

    if [ -f "${binary_path}" ]; then
        echo "✅ Found ${arch}: ${binary_path}"
        cp "${binary_path}" "binaries/${binary_name}"
        BUILT_PLATFORMS+=("${platform}")
    else
        echo "⚠️  Missing ${arch}: ${binary_path}"
    fi
done

if [ ${#BUILT_PLATFORMS[@]} -eq 0 ]; then
    echo "❌ No binaries found!"
    exit 1
fi

PLATFORMS_STR=$(IFS=,; echo "${BUILT_PLATFORMS[*]}")
echo "🚀 Building for platforms: ${PLATFORMS_STR}"

FULL_IMAGE="${REGISTRY}/${IMAGE_NAME}:${TAG}"

# Build each platform individually via docker build (uses Docker daemon proxy),
# then assemble multi-arch manifest via docker manifest create.
echo "🔨 Building per-platform images..."
declare -a SUFFIXES=()
for platform in "${BUILT_PLATFORMS[@]}"; do
    suffix=$(platform_suffix "${platform}")
    SUFFIXES+=("${suffix}")
    ARCH_IMAGE="${REGISTRY}/${IMAGE_NAME}:${TAG}-${suffix}"

    # Map platform to the binary filename in binaries/
    case "$platform" in
        "linux/amd64")   fibjs_binary="fibjs-amd64" ;;
        "linux/arm64")   fibjs_binary="fibjs-arm64" ;;
        "linux/arm/v7")  fibjs_binary="fibjs-arm" ;;
        "linux/ppc64le") fibjs_binary="fibjs-ppc64le" ;;
        "linux/riscv64") fibjs_binary="fibjs-riscv64" ;;
        *)               echo "❌ Unknown platform: ${platform}"; exit 1 ;;
    esac

    echo "  📦 Building ${platform} -> ${ARCH_IMAGE}"
    if ! docker build --platform "${platform}" \
        --build-arg "FIBJS_BINARY=${fibjs_binary}" \
        -t "${ARCH_IMAGE}" \
        -f Dockerfile.dev .; then
        echo "❌ Failed to build ${platform}"
        echo "🧹 Cleaning up..."
        rm -rf binaries
        exit 1
    fi
    echo "  ✅ Built ${ARCH_IMAGE}"
done

if [ "${PUSH}" = "true" ]; then
    echo "📤 Pushing per-platform images..."
    for i in "${!BUILT_PLATFORMS[@]}"; do
        suffix="${SUFFIXES[$i]}"
        ARCH_IMAGE="${REGISTRY}/${IMAGE_NAME}:${TAG}-${suffix}"

        echo "  📤 Pushing ${ARCH_IMAGE}"
        if ! docker push "${ARCH_IMAGE}"; then
            echo "❌ Failed to push ${ARCH_IMAGE}"
            echo "💡 Check Docker login status: docker login"
            echo "🧹 Cleaning up..."
            rm -rf binaries
            exit 1
        fi
        echo "  ✅ Pushed ${ARCH_IMAGE}"
    done

    echo "🔗 Creating multi-arch manifest..."
    MANIFEST_ARGS=""
    for suffix in "${SUFFIXES[@]}"; do
        ARCH_IMAGE="${REGISTRY}/${IMAGE_NAME}:${TAG}-${suffix}"
        MANIFEST_ARGS="${MANIFEST_ARGS} ${ARCH_IMAGE}"
    done

    # docker manifest is a CLI-side command (not dockerd), so it needs
    # proxy via environment variables to reach Docker Hub.
    if [ -n "$CLI_HTTP_PROXY" ]; then
        export HTTP_PROXY="$CLI_HTTP_PROXY"
        export HTTPS_PROXY="$CLI_HTTPS_PROXY"
    fi

    docker manifest rm "${FULL_IMAGE}" 2>/dev/null || true
    if ! docker manifest create "${FULL_IMAGE}" ${MANIFEST_ARGS}; then
        echo "❌ Failed to create manifest"
        echo "🧹 Cleaning up..."
        rm -rf binaries
        exit 1
    fi

    if ! docker manifest push "${FULL_IMAGE}"; then
        echo "❌ Failed to push manifest"
        echo "🧹 Cleaning up..."
        rm -rf binaries
        exit 1
    fi

    unset HTTP_PROXY HTTPS_PROXY

    echo "✅ Multi-arch manifest pushed: ${FULL_IMAGE}"

    echo "🧹 Cleaning up local per-platform images..."
    for suffix in "${SUFFIXES[@]}"; do
        ARCH_IMAGE="${REGISTRY}/${IMAGE_NAME}:${TAG}-${suffix}"
        docker rmi "${ARCH_IMAGE}" 2>/dev/null || true
    done
else
    echo "ℹ️  Per-platform images built locally (not pushed)."
    echo "ℹ️  Use --push to push multi-arch image to registry."
fi

echo "🧹 Cleaning up..."
rm -rf binaries

echo ""
echo "✅ Done!"
echo ""
echo "🧪 Test commands:"
echo "  docker run --rm ${FULL_IMAGE} --version"
echo "  docker run --rm --platform linux/amd64 ${FULL_IMAGE} --version"
echo "  docker run --rm --platform linux/arm64 ${FULL_IMAGE} --version"
echo "  docker run --rm --platform linux/arm/v7 ${FULL_IMAGE} --version"
echo "  docker run --rm --platform linux/riscv64 ${FULL_IMAGE} --version"