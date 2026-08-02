#!/bin/bash
# fibjs 全数据库覆盖测试脚本
# 支持 Linux (Docker Compose 4 库) / macOS (brew PG+MySQL) / Windows (预装 PG+MySQL, Docker MSSQL)
# 
# 环境变量:
#   BUILD_OS, BUILD_ARCH, BUILD_TYPE - 由 actions-env.sh 设置

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
# Linux: Docker Compose 启动全部 4 个数据库
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

    # 全部 4 个数据库
    export FIBJS_TEST_PSQL="psql://fibjs:fibjs_test@localhost:5432/test"
    export FIBJS_TEST_MYSQL="mysql://root:root_test@localhost:3306/test"
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
    /opt/homebrew/opt/postgresql@16/bin/psql -c "ALTER USER fibjs PASSWORD 'fibjs_test';" 2>/dev/null || \
    /usr/local/opt/postgresql@16/bin/psql -c "ALTER USER fibjs PASSWORD 'fibjs_test';" 2>/dev/null || true

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
    # 创建测试数据库
    "$PG_BIN/psql" -U postgres -c "CREATE DATABASE test;" 2>/dev/null || true
    "$PG_BIN/psql" -U postgres -c "ALTER USER postgres PASSWORD 'root';" 2>/dev/null || true

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

    # Windows ODBC 驱动由系统管理
    export FIBJS_TEST_PSQL="psql://postgres:root@localhost:5432/test"
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
echo "FIBJS_TEST_MYSQL=${FIBJS_TEST_MYSQL:-'(not set)'}"
echo "FIBJS_TEST_MSSQL=${FIBJS_TEST_MSSQL:-'(not set)'}"
echo "FIBJS_TEST_DM=${FIBJS_TEST_DM:-'(not set)'}"

echo ""
echo "=== Running database tests ==="
$BIN test/db_test.js

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
