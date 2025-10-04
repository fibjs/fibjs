#!/bin/bash

# Creates a multi-architecture fibjs docker image
# Usage: ./build-dev.sh [OPTIONS]
# Options:
#   --push            Push image to registry after build
#   --skip-build      Skip compilation, use existing binaries
#   --help            Show this help message

set -e

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
            echo "Creates a multi-architecture fibjs docker image: fibjs/dev:latest"
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

# Supported architectures: Docker platform -> fibjs build arch
# Note: Only including architectures supported by Ubuntu 24.04
declare -A ARCH_MAP=(
    ["linux/amd64"]="x64"
    ["linux/arm64"]="arm64"
    ["linux/arm/v7"]="arm"
    ["linux/ppc64le"]="ppc64"
    ["linux/riscv64"]="riscv64"
)

echo "🏗️  Building multi-architecture fibjs Docker image..."
echo "Image: ${REGISTRY}/${IMAGE_NAME}:${TAG}"
echo "Push: ${PUSH}"
echo "Skip Build: ${SKIP_BUILD}"

# Create buildx builder if needed
BUILDER_NAME="fibjs-builder"
if ! docker buildx ls | grep -q "${BUILDER_NAME}"; then
    echo "📦 Creating buildx builder with Docker daemon proxy configuration..."
    
    # Read proxy settings from Docker daemon configuration
    PROXY_OPTS=""
    DAEMON_CONFIG="/etc/docker/daemon.json"
    
    if [ -f "$DAEMON_CONFIG" ]; then
        # Simple grep/sed parsing for proxy settings
        HTTP_PROXY=$(cat "$DAEMON_CONFIG" | grep '"http-proxy"' | sed 's/.*"http-proxy"[[:space:]]*:[[:space:]]*"\([^"]*\)".*/\1/')
        HTTPS_PROXY=$(cat "$DAEMON_CONFIG" | grep '"https-proxy"' | sed 's/.*"https-proxy"[[:space:]]*:[[:space:]]*"\([^"]*\)".*/\1/')
        NO_PROXY=$(cat "$DAEMON_CONFIG" | grep '"no-proxy"' | sed 's/.*"no-proxy"[[:space:]]*:[[:space:]]*"\([^"]*\)".*/\1/')
        
        # Build proxy options for buildx
        if [ -n "$HTTP_PROXY" ]; then
            PROXY_OPTS="$PROXY_OPTS --driver-opt env.HTTP_PROXY=$HTTP_PROXY"
            echo "📋 Using HTTP proxy from Docker daemon: $HTTP_PROXY"
        fi
        if [ -n "$HTTPS_PROXY" ]; then
            PROXY_OPTS="$PROXY_OPTS --driver-opt env.HTTPS_PROXY=$HTTPS_PROXY"
            echo "📋 Using HTTPS proxy from Docker daemon: $HTTPS_PROXY"
        fi
        if [ -n "$NO_PROXY" ]; then
            # Clean up NO_PROXY value - remove CIDR notation which buildx doesn't support
            NO_PROXY_CLEAN=$(echo "$NO_PROXY" | sed 's/[0-9]*\.[0-9]*\.[0-9]*\.[0-9]*\/[0-9]*//g' | sed 's/,,*/,/g' | sed 's/^,//;s/,$//')
            if [ -n "$NO_PROXY_CLEAN" ]; then
                PROXY_OPTS="$PROXY_OPTS --driver-opt env.NO_PROXY=$NO_PROXY_CLEAN"
                echo "📋 Using NO_PROXY from Docker daemon (cleaned): $NO_PROXY_CLEAN"
            fi
        fi
        
        if [ -z "$PROXY_OPTS" ]; then
            echo "📋 No proxy configuration found in Docker daemon"
        fi
    else
        echo "📋 Docker daemon config not found: $DAEMON_CONFIG"
    fi
    
    # Create builder with proxy configuration
    eval "docker buildx create --name ${BUILDER_NAME} --driver docker-container --driver-opt network=host $PROXY_OPTS --bootstrap"
fi
docker buildx use "${BUILDER_NAME}"

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
echo "� Copying binaries..."
rm -rf binaries
mkdir -p binaries
PLATFORMS=""

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
        
        if [ -n "${PLATFORMS}" ]; then
            PLATFORMS="${PLATFORMS},"
        fi
        PLATFORMS="${PLATFORMS}${platform}"
    else
        echo "⚠️  Missing ${arch}: ${binary_path}"
    fi
done

if [ -z "${PLATFORMS}" ]; then
    echo "❌ No binaries found!"
    exit 1
fi

echo "🚀 Building for platforms: ${PLATFORMS}"

# Build multi-arch image
if [ "${PUSH}" = "true" ]; then
    echo "🚀 Building and pushing multi-arch image..."
    echo "📤 This may take several minutes, please wait..."
    echo "Executing: docker buildx build --platform ${PLATFORMS} -t ${REGISTRY}/${IMAGE_NAME}:${TAG} -f Dockerfile.dev --push ."
    
    if docker buildx build --platform ${PLATFORMS} -t ${REGISTRY}/${IMAGE_NAME}:${TAG} -f Dockerfile.dev --push .; then
        echo "✅ Build and push completed successfully!"
        echo "🚀 Multi-arch image available at: ${REGISTRY}/${IMAGE_NAME}:${TAG}"
        echo "🏗️  Supported platforms: ${PLATFORMS}"
        echo "🧹 Cleaning up..."
        rm -rf binaries
        echo "✅ Multi-arch image ${REGISTRY}/${IMAGE_NAME}:${TAG} built and pushed!"
    else
        echo "❌ Build and push failed!"
        echo "💡 Check Docker login status: docker login"
        echo "🧹 Cleaning up..."
        rm -rf binaries
        exit 1
    fi
else
    echo "🚀 Building multi-arch image..."
    echo "Executing: docker buildx build --platform ${PLATFORMS} -t ${REGISTRY}/${IMAGE_NAME}:${TAG} -f Dockerfile.dev ."
    
    if docker buildx build --platform ${PLATFORMS} -t ${REGISTRY}/${IMAGE_NAME}:${TAG} -f Dockerfile.dev .; then
        echo "✅ Build completed successfully!"
        echo "💡 Multi-arch image built in buildx cache (not available in 'docker images')"
        echo "💡 Use --push to push to registry"
        echo "🧹 Cleaning up..."
        rm -rf binaries
        echo "✅ Multi-arch image ${REGISTRY}/${IMAGE_NAME}:${TAG} built successfully!"
    else
        echo "❌ Build failed!"
        echo "🧹 Cleaning up..."
        rm -rf binaries
        exit 1
    fi
fi

echo ""
echo "🧪 Test commands:"
echo "  docker run --rm ${REGISTRY}/${IMAGE_NAME}:${TAG} --version"
echo "  docker run --rm --platform linux/amd64 ${REGISTRY}/${IMAGE_NAME}:${TAG} --version"
echo "  docker run --rm --platform linux/arm64 ${REGISTRY}/${IMAGE_NAME}:${TAG} --version"
echo "  docker run --rm --platform linux/arm/v7 ${REGISTRY}/${IMAGE_NAME}:${TAG} --version"
echo "  docker run --rm --platform linux/riscv64 ${REGISTRY}/${IMAGE_NAME}:${TAG} --version"
