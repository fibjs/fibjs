#!/bin/bash

# Creates a fibjs dev docker image

set -e

echo "Building fibjs/dev image..."

# Copy fibjs binary
echo "Copying fibjs binary..."
cp ../bin/Linux_x64_release/fibjs .

# Build docker image
echo "Building Docker image..."
docker build -t fibjs/dev -f Dockerfile.dev .

# Clean up
echo "Cleaning up temporary files..."
rm -f fibjs

echo "✅ fibjs/dev image build completed!"
