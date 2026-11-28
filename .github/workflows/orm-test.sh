#!/bin/bash
# Clone fib-orm and run its cross-database smoke suite against the fibjs binary
# built by db-test.sh.
#
# Inputs (usually exported by db-test.sh):
#   FIBJS_BIN           absolute path to the fibjs executable (required)
#   FIBJS_TEST_PSQL     psql://...
#   FIBJS_TEST_PSQL_KB  psql://... (KingbaseES through the psql engine)
#   FIBJS_TEST_KINGBASE kingbase://... (KingbaseES through KODBC)
#   FIBJS_TEST_MYSQL    mysql://...
#   FIBJS_TEST_MYSQL57  mysql://...
#   FIBJS_TEST_MSSQL    mssql://...
#   FIBJS_TEST_DM       dm://...
#
# Optional:
#   FIB_ORM_REPO        default: https://github.com/fibjs/fib-orm.git
#   FIB_ORM_REF         default: master (branch, tag or commit SHA)
#   FIB_ORM_DIR         default: ${RUNNER_TEMP:-/tmp}/fib-orm-ci
#   FIB_ORM_SKIP_CLONE  set to 1 to reuse FIB_ORM_DIR as-is

set -e

if [ -z "$FIBJS_BIN" ]; then
    echo "FIBJS_BIN is required (absolute path to the fibjs executable)"
    exit 1
fi

if [ ! -x "$FIBJS_BIN" ]; then
    echo "fibjs binary is not executable: $FIBJS_BIN"
    exit 1
fi

FIB_ORM_REPO="${FIB_ORM_REPO:-https://github.com/fibjs/fib-orm.git}"
FIB_ORM_REF="${FIB_ORM_REF:-master}"
FIB_ORM_DIR="${FIB_ORM_DIR:-${RUNNER_TEMP:-/tmp}/fib-orm-ci}"

echo "=== fib-orm: $FIB_ORM_REPO ($FIB_ORM_REF) ==="

if [ "$FIB_ORM_SKIP_CLONE" = "1" ]; then
    if [ ! -d "$FIB_ORM_DIR/.git" ]; then
        echo "FIB_ORM_SKIP_CLONE=1 but $FIB_ORM_DIR is not a git checkout"
        exit 1
    fi
    echo "Using existing checkout: $FIB_ORM_DIR"
else
    rm -rf "$FIB_ORM_DIR"
    mkdir -p "$FIB_ORM_DIR"
    git -C "$FIB_ORM_DIR" init -q
    git -C "$FIB_ORM_DIR" remote add origin "$FIB_ORM_REPO"
    # A branch or tag can be fetched directly; an arbitrary commit SHA is not a
    # remote ref on every server, so fall back to a full fetch and checkout.
    if git -C "$FIB_ORM_DIR" fetch --depth 1 origin "$FIB_ORM_REF"; then
        git -C "$FIB_ORM_DIR" checkout -q FETCH_HEAD
    else
        git -C "$FIB_ORM_DIR" fetch origin
        git -C "$FIB_ORM_DIR" checkout -q "$FIB_ORM_REF"
    fi
fi

echo "fib-orm commit: $(git -C "$FIB_ORM_DIR" rev-parse --short HEAD)"

cd "$FIB_ORM_DIR"

echo "=== Installing fib-orm dependencies ==="
if ! command -v npm >/dev/null 2>&1; then
    echo "ERROR: npm is required to install fib-orm dependencies"
    exit 1
fi
if [ -f package-lock.json ]; then
    npm ci --no-audit --no-fund || npm install --no-audit --no-fund
else
    npm install --no-audit --no-fund
fi

# fib-orm 2.x and 3.x run the TypeScript sources directly; older revisions
# still need an ftsc build. Detect the entry instead of forcing one workflow
# on both layouts.
if grep -Eq '"main"[[:space:]]*:[[:space:]]*"(src/orm/index\.ts|index\.js)"' package.json; then
    echo "=== Checking fib-orm sources (fibjs --check) ==="
    "$FIBJS_BIN" --check
else
    if [ ! -f node_modules/fib-typify/bin/ftsc.js ]; then
        echo "ERROR: unsupported fib-orm package layout (main is not src/orm/index.ts or index.js)"
        echo "       no node_modules/fib-typify/bin/ftsc.js for the legacy build either"
        exit 1
    fi

    echo "=== Building fib-orm (legacy layout) ==="
    # `npm run build` invokes ftsc through its `#!/usr/bin/env fibjs` shebang.
    # Run it with the binary built by fibjs instead of relying on PATH.
    rm -rf ./lib
    "$FIBJS_BIN" ./node_modules/fib-typify/bin/ftsc.js \
        $(find src -name '*.ts' ! -name '*.d.ts' ! -path '*/plugins/*' | sort) \
        --outDir ./lib
    cp src/codepoints-map.json lib/codepoints-map.json
    cp -r src/knex/deps lib/knex/deps
fi

if [ ! -f test/integration/db-smoke.js ]; then
    echo "ERROR: this fib-orm revision does not provide test/integration/db-smoke.js"
    echo "       merge the fib-orm driver changes or set FIB_ORM_REF to the branch under test"
    exit 1
fi

# Keep the workflow green while the matching fib-orm changes are still on a
# branch: skip only the engines this checkout cannot provide yet.
if [ ! -f src/orm/Drivers/DML/mssql.ts ]; then
    echo "WARNING: this fib-orm revision does not support MSSQL yet; skipping mssql smoke"
    FIBJS_TEST_MSSQL=""
fi

if ! grep -q "openKingbase" src/db-driver/built-ins/driver-postgresql.ts 2>/dev/null; then
    echo "WARNING: this fib-orm revision does not support KingbaseES yet; skipping kingbase-kodbc smoke"
    FIBJS_TEST_KINGBASE=""
fi

if ! grep -q "colInfo.DATA_DEFAULT ?? colInfo.data_default" src/orm-property/transformers/dm.ts 2>/dev/null; then
    echo "WARNING: this fib-orm revision has the DM null-default bug; skipping dm smoke"
    FIBJS_TEST_DM=""
fi
run_smoke () {
    local label="$1"
    local protocol="$2"
    local uri="$3"

    if [ -z "$uri" ]; then
        echo ""
        echo "=== ORM smoke: $label skipped (no connection URL) ==="
        return 0
    fi

    echo ""
    echo "=== ORM smoke: $label ==="
    ORM_PROTOCOL="$protocol" ORM_URI="$uri" "$FIBJS_BIN" test/integration/db-smoke.js
}

# SQLite has no server dependency and is always available.
rm -f ./orm-smoke.db ./orm-smoke.db-shm ./orm-smoke.db-wal
run_smoke "sqlite" "sqlite" "sqlite:orm-smoke.db"

# MySQL 8.x and 5.7 share the same driver/protocol.
run_smoke "mysql" "mysql" "$FIBJS_TEST_MYSQL"
run_smoke "mysql57" "mysql" "$FIBJS_TEST_MYSQL57"

run_smoke "postgresql" "psql" "$FIBJS_TEST_PSQL"

# KingbaseES can be reached both through KODBC and through its PostgreSQL
# protocol mode; run both when the corresponding URL is available.
run_smoke "kingbase-kodbc" "kingbase" "$FIBJS_TEST_KINGBASE"
run_smoke "kingbase-psql" "psql" "$FIBJS_TEST_PSQL_KB"

run_smoke "mssql" "mssql" "$FIBJS_TEST_MSSQL"
run_smoke "dm" "dm" "$FIBJS_TEST_DM"

echo ""
echo "=== fib-orm database smoke tests completed ==="
