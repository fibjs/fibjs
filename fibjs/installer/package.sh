#!/bin/bash

# Runtime library requirements of the packaged binary (informational).
#
# Called by the `package` target after fibjs.tar.gz and installer.sh have been
# produced (see installer/CMakeLists.txt, which declares them as build outputs).
# The leading blank line keeps the report visually separate from the build
# output above it.

set -e

: "${BIN_PATH:?BIN_PATH must be set}"

cd "${BIN_PATH}"

echo ""
echo "==== runtime library requirements ===="
echo ""
if [[ "$(uname)" == "Linux" ]]; then
    echo "==== GLIBC ===="
    objdump fibjs -p | grep -o "GLIBCX*_[0-9.]*" | sort -V | uniq || true
elif [[ "$(uname)" == "Darwin" ]]; then
    otool -L fibjs
fi

# Always list what the packaging produced: the archives are build outputs, so an
# up-to-date incremental build prints nothing about them otherwise.
echo ""
echo "==== packaged artifacts ===="
for f in fibjs.tar.gz installer.sh; do
    if [[ -f "${f}" ]]; then
        echo "  ${f}  $(wc -c < "${f}") bytes"
    else
        echo "  ${f}  MISSING"
    fi
done
echo ""
