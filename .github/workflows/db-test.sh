#!/bin/bash
# fibjs 全数据库覆盖测试脚本
# 支持 Linux (Docker Compose 6 库) / macOS (brew PG+MySQL) / Windows (预装 PG+MySQL, Docker MSSQL)
# 每个平台在 fibjs 驱动测试后还会 clone fib-orm 并运行跨库 ORM 冒烟测试。
# 
# 环境变量:
#   BUILD_OS, BUILD_ARCH, BUILD_TYPE - 由 actions-env.sh 设置
#   FIBJS_SKIP_ORM_TESTS=1           - 跳过 fib-orm 部分（本地调试用）
#   FIB_ORM_REPO / FIB_ORM_REF       - 指定 fib-orm 仓库与分支/标签/SHA

set -e

detect_os() {
    case "$(uname -s)" in
        Linux*)                      echo "linux" ;;
        Darwin*)                     echo "darwin" ;;
        MINGW*|MSYS*|CYGWIN*)        echo "windows" ;;
        *)                           echo "unknown" ;;
    esac
}

OS=$(detect_os)
BIN="bin/${BUILD_OS}_${BUILD_ARCH}_${BUILD_TYPE}/fibjs"

# Windows Git Bash: 尝试 .exe 扩展名
if [ "$OS" = "windows" ] && [ ! -f "$BIN" ]; then
    BIN="${BIN}.exe"
fi

if [ ! -f "$BIN" ]; then
    echo "fibjs binary not found at $BIN"
    echo "Please build first: bash build -j2 ${BUILD_ARCH} ${BUILD_TYPE} ci"
    exit 1
fi

echo "=== Platform: $OS ==="
echo "=== Binary: $BIN ==="

# ============================================================
# Linux: Docker Compose 启动全部 6 个数据库
# ============================================================
if [ "$OS" = "linux" ]; then
    echo "=== Starting database containers ==="
    docker compose -f docker/docker-compose.db-test.yml up -d

    echo "=== Waiting for databases to be ready ==="

    echo -n "Waiting for PostgreSQL..."
    for i in $(seq 1 60); do
        if docker exec fibjs-db-test-postgres pg_isready -U fibjs -d test 2>/dev/null; then
            echo " ready!"; break
        fi
        echo -n "."; sleep 2
    done

    echo -n "Waiting for MySQL..."
    for i in $(seq 1 60); do
        if docker exec fibjs-db-test-mysql mysqladmin ping -h localhost -u root -proot_test --silent 2>/dev/null; then
            echo " ready!"; break
        fi
        echo -n "."; sleep 2
    done

    echo -n "Waiting for MySQL 5.7..."
    for i in $(seq 1 60); do
        if docker exec fibjs-db-test-mysql57 mysqladmin ping -h localhost -u root -proot_test --silent 2>/dev/null; then
            echo " ready!"; break
        fi
        echo -n "."; sleep 2
    done

    echo -n "Waiting for MSSQL..."
    for i in $(seq 1 90); do
        if docker logs fibjs-db-test-mssql 2>/dev/null | grep -q "SQL Server is now ready"; then
            echo " ready!"; break
        fi
        echo -n "."; sleep 2
    done

    echo -n "Waiting for DM8..."
    for i in $(seq 1 90); do
        if docker exec fibjs-db-test-dm8 /home/dmdba/dmdbms/bin/disql SYSDBA/123456789@localhost:5236 -e 'select 1' 2>/dev/null | grep -q '1'; then
            echo " ready!"; break
        fi
        echo -n "."; sleep 2
    done

    echo -n "Waiting for KingbaseES V9..."
    for i in $(seq 1 90); do
        if docker exec fibjs-db-test-kingbase bash -c 'export PATH=$PATH:/home/kingbase/install/kingbase/bin; export LD_LIBRARY_PATH=/home/kingbase/install/kingbase/lib; sys_isready -h /tmp -p 54321 -d kingbase' 2>/dev/null | grep -q 'accepting connections'; then
            echo " ready!"; break
        fi
        echo -n "."; sleep 2
    done

    echo ""
    echo "=== Installing ODBC drivers (Linux) ==="
    sudo apt-get update -qq
    sudo apt-get install -y -qq unixodbc unixodbc-dev freetds-bin tdsodbc odbc-postgresql

    echo "=== Extracting DM ODBC driver ==="
    # libdodbc.so 依赖 libdmdpi.so / libdmfldr.so 等 DM 内部库，
    # 需要把整个 bin 目录拷出来，通过 LD_LIBRARY_PATH 让 dlopen 能找到
    DM_DRV_DIR="/usr/local/lib/dm8"
    sudo mkdir -p "$DM_DRV_DIR"
    docker cp fibjs-db-test-dm8:/home/dmdba/dmdbms/bin/. /tmp/dm8_bin/ 2>/dev/null || true
    if [ -d /tmp/dm8_bin ] && [ -f /tmp/dm8_bin/libdodbc.so ]; then
        sudo cp /tmp/dm8_bin/*.so "$DM_DRV_DIR/" 2>/dev/null || true
        sudo cp /tmp/dm8_bin/libdodbc.so "$DM_DRV_DIR/" 2>/dev/null || true
        sudo ldconfig
        echo "DM ODBC driver installed: $(ls "$DM_DRV_DIR"/libdodbc.so)"
    else
        echo "WARNING: DM ODBC driver extraction failed, DM tests will be skipped"
    fi
    export LD_LIBRARY_PATH="$DM_DRV_DIR:${LD_LIBRARY_PATH}"

    echo "=== Extracting KingbaseES V9 ODBC driver ==="
    # KODBC 不随 V9 精简镜像发布；官方下载中心的接口驱动区提供 V9 的
    # X64_Linux_ODBC.tar。OSS 有防盗链，必须带官网 Referer。下载失败时
    # 只跳过 kingbase 引擎用例（psql_kb 仍覆盖同一实例）。
    KB_ODBC_DIR="${RUNNER_TEMP:-/tmp}/kingbase-odbc"
    KB_ODBC_TAR="${RUNNER_TEMP:-/tmp}/kingbase-odbc.tar"
    KB_ODBC_URL="https://kingbase.oss-cn-beijing.aliyuncs.com/upload/KESV9-baseline/allmode/V009R001C010/V009R001C010B0004_interface/ODBC/KingbaseES_V009R001C010B0004_X64_Linux_ODBC.tar"
    if curl -fsSL -H 'Referer: https://www.kingbase.com.cn/' -o "$KB_ODBC_TAR" "$KB_ODBC_URL"; then
        rm -rf "$KB_ODBC_DIR"
        mkdir -p "$KB_ODBC_DIR"
        tar -xf "$KB_ODBC_TAR" -C "$KB_ODBC_DIR"

        # 装进 unixODBC 的默认驱动目录，Driver=kdbodbcw.so 才能按名字直接
        # 解析（与正常安装一致），不需要在连接串里写 ?Driver= 全路径。
        case "$(uname -m)" in
            aarch64|arm64) KB_ODBC_DEST=/usr/lib/aarch64-linux-gnu/odbc ;;
            *)             KB_ODBC_DEST=/usr/lib/x86_64-linux-gnu/odbc ;;
        esac
        if sudo mkdir -p "$KB_ODBC_DEST" && sudo cp -a "$KB_ODBC_DIR/odbc/." "$KB_ODBC_DEST/"; then
            echo "Kingbase ODBC driver installed: $KB_ODBC_DEST/kdbodbcw.so"
            export FIBJS_TEST_KINGBASE="kingbase://SYSTEM:12345678ab@localhost:54321/test"
        else
            echo "WARNING: Kingbase ODBC driver installation failed, kingbase engine tests will be skipped"
        fi
    else
        echo "WARNING: Kingbase ODBC driver download failed, kingbase engine tests will be skipped"
    fi

    # 全部 6 个数据库 (mysql57 验证低版本认证协议 mysql_native_password; psql_kb 为 psql 引擎连 KingbaseES V9,
    # kingbase 为 KODBC 引擎连同一实例)
    export FIBJS_TEST_PSQL="psql://fibjs:fibjs_test@localhost:5432/test"
    export FIBJS_TEST_PSQL_KB="psql://SYSTEM:12345678ab@localhost:54321/test"
    export FIBJS_TEST_MYSQL="mysql://root:root_test@localhost:3306/test"
    export FIBJS_TEST_MYSQL57="mysql://root:root_test@localhost:3307/test"
    export FIBJS_TEST_MSSQL="mssql://sa:Test_1234@localhost:1433/test"
    export FIBJS_TEST_DM="dm://SYSDBA:123456789@localhost:5236/test"

# ============================================================
# macOS: brew 启动 PG + MySQL; MSSQL 跳过 (M1 runner 无嵌套虚拟化); DM 跳过
# ============================================================
elif [ "$OS" = "darwin" ]; then
    echo "=== Installing databases and ODBC drivers (macOS) ==="

    if ! command -v brew &>/dev/null; then
        echo "Homebrew not found, installing..."
        /bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"
        eval "$(/opt/homebrew/bin/brew shellenv)" 2>/dev/null || \
        eval "$(/usr/local/bin/brew shellenv)" 2>/dev/null || true
    fi

    brew install unixodbc psqlodbc freetds postgresql@16 mysql 2>/dev/null || true

    echo "=== Starting PostgreSQL ==="
    brew services start postgresql@16 2>/dev/null || true
    sleep 3
    /opt/homebrew/opt/postgresql@16/bin/createuser -s fibjs 2>/dev/null || \
    /usr/local/opt/postgresql@16/bin/createuser -s fibjs 2>/dev/null || true
    /opt/homebrew/opt/postgresql@16/bin/createdb -O fibjs test 2>/dev/null || \
    /usr/local/opt/postgresql@16/bin/createdb -O fibjs test 2>/dev/null || true
    /opt/homebrew/opt/postgresql@16/bin/psql -d postgres -c "ALTER USER fibjs PASSWORD 'fibjs_test';" 2>/dev/null || \
    /usr/local/opt/postgresql@16/bin/psql -d postgres -c "ALTER USER fibjs PASSWORD 'fibjs_test';" 2>/dev/null || true

    # Homebrew initializes pg_hba.conf with `trust`, so every password is
    # accepted and the credential encoding probe can observe nothing.  Switch
    # the local/loopback rules to scram-sha-256 now that the roles have
    # passwords (the roles were created while trust was still in effect).
    PG_PREFIX=
    for d in /opt/homebrew/opt/postgresql@16 /usr/local/opt/postgresql@16; do
        if [ -x "$d/bin/psql" ]; then PG_PREFIX="$d"; break; fi
    done
    if [ -n "$PG_PREFIX" ]; then
        HBA=$("$PG_PREFIX/bin/psql" -d postgres -tAc 'show hba_file' 2>/dev/null || true)
        if [ -n "$HBA" ] && [ -f "$HBA" ]; then
            sed -i '' -E 's/(^|[[:space:]])trust([[:space:]]*)$/\1scram-sha-256\2/' "$HBA"
            "$PG_PREFIX/bin/psql" -d postgres -c 'select pg_reload_conf();' >/dev/null 2>&1 || true
            if PGPASSWORD=not_the_password "$PG_PREFIX/bin/psql" -w -h 127.0.0.1 -U fibjs -d test -c 'select 1' >/dev/null 2>&1; then
                echo "WARNING: PostgreSQL still accepts a wrong password ($HBA)"
            elif ! PGPASSWORD=fibjs_test "$PG_PREFIX/bin/psql" -w -h 127.0.0.1 -U fibjs -d test -c 'select 1' >/dev/null 2>&1; then
                echo "WARNING: the fibjs role cannot authenticate with its password ($HBA)"
            else
                echo "PostgreSQL password authentication enforced ($HBA)"
            fi
        fi
    fi

    echo "=== Starting MySQL ==="
    brew services start mysql 2>/dev/null || true
    sleep 5
    mysql -u root -e "CREATE DATABASE IF NOT EXISTS test;" 2>/dev/null || true
    mysql -u root -e "ALTER USER 'root'@'localhost' IDENTIFIED BY 'root_test';" 2>/dev/null || true

    # MSSQL 跳过: SQL Server 无 ARM64 原生镜像, GitHub Actions macOS runner (M1) 无嵌套虚拟化
    echo "=== MSSQL skipped on macOS (M1 runner: no nested virt, no arm64 mssql image) ==="

    # DM 跳过: macOS 没有 DM 服务器
    echo "=== Note: DM not tested on macOS (no DM server for macOS) ==="

    export FIBJS_TEST_PSQL="psql://fibjs:fibjs_test@localhost:5432/test"
    export FIBJS_TEST_MYSQL="mysql://root:root_test@localhost:3306/test"

# ============================================================
# Windows: 预装 PG14 (启动即可), choco 装 MySQL, Docker 启动 MSSQL, DM 跳过
# ============================================================
elif [ "$OS" = "windows" ]; then
    echo "=== Setting up databases (Windows) ==="

    # PostgreSQL 14 预装在 windows-2022 runner 上 (服务名 postgresql-x64-14, 密码 root)
    echo "=== Starting PostgreSQL (pre-installed v14) ==="
    if ! NET START postgresql-x64-14 >/dev/null 2>&1 && ! sc start postgresql-x64-14 >/dev/null 2>&1; then
        echo "PostgreSQL 14 service not found, installing via choco..."
        choco install postgresql14 -y --no-progress --params "/Password:root" 2>/dev/null || true
        NET START postgresql-x64-14 >/dev/null 2>&1 || true
    fi
    sleep 3

    PG_BIN="/c/Program Files/PostgreSQL/14/bin"
    export PATH="$PG_BIN:$PATH"
    # 创建测试数据库（PGPASSWORD 对 preinstalled/choco 两种安装都成立）
    PGPASSWORD=root "$PG_BIN/psql" -U postgres -c "CREATE DATABASE test;" 2>/dev/null || true
    PGPASSWORD=root "$PG_BIN/psql" -U postgres -c "ALTER USER postgres PASSWORD 'root';" 2>/dev/null || true

    # EDB/choco 的默认 pg_hba.conf 也是 trust（上面的 psql 不需要密码），
    # 编码探针会假通过。和 macOS 一样把 local/loopback 改成 scram-sha-256。
    PG_HBA=$(PGPASSWORD=root "$PG_BIN/psql" -U postgres -d postgres -tAc 'show hba_file' 2>/dev/null | tr -d '\r')
    if [ -n "$PG_HBA" ]; then
        PG_HBA_U=$(cygpath -u "$PG_HBA" 2>/dev/null || printf '%s' "$PG_HBA")
        if [ -f "$PG_HBA_U" ]; then
            sed -i -E 's/(^|[[:space:]])trust([[:space:]]*)$/\1scram-sha-256\2/' "$PG_HBA_U"
            PGPASSWORD=root "$PG_BIN/psql" -U postgres -d postgres -c 'select pg_reload_conf();' >/dev/null 2>&1 || true
            if PGPASSWORD=not_the_password "$PG_BIN/psql" -w -h 127.0.0.1 -U postgres -d test -c 'select 1' >/dev/null 2>&1; then
                echo "WARNING: PostgreSQL still accepts a wrong password ($PG_HBA)"
            elif ! PGPASSWORD=root "$PG_BIN/psql" -w -h 127.0.0.1 -U postgres -d test -c 'select 1' >/dev/null 2>&1; then
                echo "WARNING: the postgres role cannot authenticate with its password ($PG_HBA)"
            else
                echo "PostgreSQL password authentication enforced ($PG_HBA)"
            fi
        fi
    fi

    # MySQL: runner 只有客户端工具, 需通过 choco 安装服务器
    echo "=== Starting MySQL ==="
    if ! NET START MySQL >/dev/null 2>&1 && ! NET START MySQL80 >/dev/null 2>&1 && \
       ! sc start MySQL >/dev/null 2>&1 && ! sc start MySQL80 >/dev/null 2>&1; then
        echo "MySQL service not found, installing via choco..."
        choco install mysql -y --no-progress 2>/dev/null || true
        NET START MySQL >/dev/null 2>&1 || NET START MySQL80 >/dev/null 2>&1 || true
    fi
    sleep 5

    MYSQL_BIN="/c/Program Files/MySQL/MySQL Server 8.0/bin"
    if [ -d "$MYSQL_BIN" ]; then
        export PATH="$MYSQL_BIN:$PATH"
    fi
    mysql -u root -e "CREATE DATABASE IF NOT EXISTS test;" 2>/dev/null || true
    mysql -u root -e "ALTER USER 'root'@'localhost' IDENTIFIED BY 'root_test';" 2>/dev/null || true

    # MSSQL: Docker 启动 (Docker Desktop 预装在 runner 上, WSL2 后端支持 Linux 容器)
    echo "=== Starting MSSQL (Docker) ==="
    DOCKER_OK=false
    for i in $(seq 1 30); do
        if docker info >/dev/null 2>&1; then
            echo "Docker is ready"; DOCKER_OK=true; break
        fi
        echo -n "."; sleep 2
    done

    MSSQL_READY=false
    if [ "$DOCKER_OK" = true ]; then
        docker rm -f fibjs-db-test-mssql 2>/dev/null || true
        echo "Pulling and starting MSSQL container..."
        # Windows runner 需指定 --platform linux/amd64 (WSL2 会通过 x64 emulation 运行)
        if docker run -d --name fibjs-db-test-mssql \
            -e "ACCEPT_EULA=Y" \
            -e "MSSQL_SA_PASSWORD=Test_1234" \
            -p 1433:1433 \
            --platform linux/amd64 \
            mcr.microsoft.com/mssql/server:2022-latest; then

            echo -n "Waiting for MSSQL to be ready..."
            for i in $(seq 1 90); do
                # 先检查容器是否还在运行 (启动失败会立即退出)
                if ! docker ps --format '{{.Names}}' | grep -q fibjs-db-test-mssql 2>/dev/null; then
                    echo ""
                    echo "ERROR: MSSQL container exited. Logs:"
                    docker logs fibjs-db-test-mssql 2>&1 | tail -30
                    break
                fi
                if docker logs fibjs-db-test-mssql 2>/dev/null | grep -q "SQL Server is now ready"; then
                    echo " ready!"; MSSQL_READY=true; break
                fi
                echo -n "."; sleep 2
            done
        else
            echo "ERROR: docker run failed"
        fi
    else
        echo "ERROR: Docker is not available"
    fi

    if [ "$MSSQL_READY" = true ]; then
        export FIBJS_TEST_MSSQL="mssql://sa:Test_1234@localhost:1433/test"
    else
        echo "WARNING: MSSQL not available, tests will be skipped"
    fi

    # DM 跳过: Windows 上没有 DM 服务器
    echo "=== Note: DM not tested on Windows (no DM server available) ==="

    # psql:// 在 Windows 上走 ODBC, 需要 psqlODBC 驱动; PostgreSQL 安装包不带该驱动
    # ODBC Drivers 键把每个驱动都写成 VALUE（"PostgreSQL Unicode"="Installed"），
    # 不是子键，所以对值路径做 Test-Path 永远不命中。64 位包同时注册了带
    # (x64) 和不带 (x64) 两个名字，两种都要认；两个注册表视图都查。
    psql_odbc_installed() {
        powershell -NoProfile -Command '
            $names = @("PostgreSQL Unicode","PostgreSQL Unicode(x64)","PostgreSQL ANSI","PostgreSQL ANSI(x64)")
            foreach ($view in "Registry64","Registry32") {
                $base = [Microsoft.Win32.RegistryKey]::OpenBaseKey("LocalMachine", $view)
                $drivers = $base.OpenSubKey("SOFTWARE\ODBC\ODBCINST.INI\ODBC Drivers")
                if ($drivers) {
                    foreach ($name in $names) {
                        if ($drivers.GetValue($name)) { "yes"; exit }
                    }
                }
            }' 2>/dev/null | grep -q yes
    }

    if ! psql_odbc_installed; then
        echo "=== Installing PostgreSQL ODBC driver (psqlODBC) ==="
        choco install psqlodbc -y --no-progress 2>/dev/null || true
    fi

    if psql_odbc_installed; then
        export FIBJS_TEST_PSQL="psql://postgres:root@localhost:5432/test"
    else
        echo "WARNING: PostgreSQL ODBC driver not available, psql tests will be skipped"
    fi

    export FIBJS_TEST_MYSQL="mysql://root:root_test@localhost:3306/test"
    # FIBJS_TEST_MSSQL 已在上方 MSSQL_READY=true 时设置

else
    echo "Unsupported OS: $OS"
    exit 1
fi

echo ""
echo "=== ODBC drivers registered ==="
odbcinst -q -d 2>/dev/null || echo "(odbcinst not available, using system ODBC)"

echo ""
echo "=== Database environment variables ==="
echo "FIBJS_TEST_PSQL=${FIBJS_TEST_PSQL:-'(not set)'}"
echo "FIBJS_TEST_PSQL_KB=${FIBJS_TEST_PSQL_KB:-'(not set)'}"
echo "FIBJS_TEST_MYSQL=${FIBJS_TEST_MYSQL:-'(not set)'}"
echo "FIBJS_TEST_MYSQL57=${FIBJS_TEST_MYSQL57:-'(not set)'}"
echo "FIBJS_TEST_MSSQL=${FIBJS_TEST_MSSQL:-'(not set)'}"
echo "FIBJS_TEST_DM=${FIBJS_TEST_DM:-'(not set)'}"
echo "FIBJS_TEST_KINGBASE=${FIBJS_TEST_KINGBASE:-'(not set)'}"

echo ""
echo "=== Running database tests ==="
$BIN test/db_test.js

if [ "${FIBJS_SKIP_ORM_TESTS:-0}" != "1" ]; then
    echo ""
    echo "=== Running fib-orm database smoke tests ==="
    FIBJS_BIN="$(pwd)/$BIN" bash ./.github/workflows/orm-test.sh
fi

echo ""
echo "=== Database tests completed successfully ==="

# 清理
if [ "$OS" = "linux" ]; then
    echo "=== Cleaning up containers ==="
    docker compose -f docker/docker-compose.db-test.yml down
elif [ "$OS" = "darwin" ]; then
    echo "=== Stopping services ==="
    brew services stop postgresql@16 2>/dev/null || true
    brew services stop mysql 2>/dev/null || true
elif [ "$OS" = "windows" ]; then
    echo "=== Cleaning up ==="
    docker rm -f fibjs-db-test-mssql 2>/dev/null || true
    NET STOP postgresql-x64-14 2>/dev/null || true
    NET STOP MySQL 2>/dev/null || NET STOP MySQL80 2>/dev/null || true
fi
