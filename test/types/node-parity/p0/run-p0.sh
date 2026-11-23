#!/bin/sh
# P0 type checks. `TSC` may point at any TypeScript compiler:
#
#   TSC=/path/to/tsc sh test/types/node-parity/p0/run-p0.sh
#
# The repo does not vendor tsc; any 5.x compiler works. The package-install
# variant (npm pack + install into a temp project) is documented in README.md.
set -eu

TSC=${TSC:-tsc}
HERE=$(CDPATH= cd -- "$(dirname -- "$0")" && pwd)
cd "$HERE"

"$TSC" -p tsconfig.node10.json
"$TSC" -p tsconfig.node16.json

# The corpus has a few pre-existing diagnostics; P0 only owns index.d.ts.
# Fail if index.d.ts itself reports anything.
if "$TSC" -p tsconfig.index.json 2>&1 | grep -q 'npm/types/index.d.ts'; then
    echo "index.d.ts self-check failed" >&2
    exit 1
fi

echo "P0 type checks passed"
