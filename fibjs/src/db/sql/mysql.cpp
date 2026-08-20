/*
 * mysql.cpp
 *
 *  Created on: Jul 20, 2012
 *      Author: lion
 */

#include "object.h"
#include "mysql.h"
#include "Socket_api.h"
#include "Buffer.h"
#include "Variant.h"
#include "ifs/db.h"
#include "DBResult.h"
#include "Url.h"

namespace fibjs {

void* API_getSocket()
{
    return socket::create();
}

void API_deleteSocket(void* sock)
{
    socket::destroy(sock);
}

void API_closeSocket(void* sock)
{
}

int32_t API_connectSocket(void* sock, const char* host, int32_t port)
{
    return socket::c_connect(sock, host, port);
}

int32_t API_setTimeout(void* sock, int32_t timeoutSec)
{
    return 1;
}

void API_clearException(void)
{
}

int32_t API_recvSocket(void* sock, char* buffer, int32_t cbBuffer)
{
    return socket::c_recv(sock, buffer, cbBuffer);
}

int32_t API_sendSocket(void* sock, const char* buffer, int32_t cbBuffer)
{
    return socket::c_send(sock, buffer, cbBuffer);
}

void* API_createResult(int32_t columns)
{
    DBResult* res = new DBResult(columns);
    res->Ref();
    return res;
}

void API_resultSetField(void* result, int32_t ifield, UMTypeInfo* ti, void* name,
    size_t cbName)
{
    exlib::string s((char*)name, cbName);
    ((DBResult*)result)->setField(ifield, s);
}

void API_resultRowBegin(void* result)
{
    ((DBResult*)result)->beginRow();
}

// 列类型转换（execute 回调与 Statement 游标共用）
void mysql::columnValue(const UMTypeInfo* ti, const UINT8* value,
    size_t cbValue, Variant& v)
{
    if (value) {
        switch (ti->type) {
        case MFTYPE_NULL:
            v.setNull();
            break;

        case MFTYPE_BIT:
            v = new Buffer(value, cbValue);
            break;

        case MFTYPE_TINY:
        case MFTYPE_SHORT:
        case MFTYPE_LONG:
        case MFTYPE_INT24:
        case MFTYPE_LONGLONG:
        case MFTYPE_FLOAT:
        case MFTYPE_DOUBLE:
        case MFTYPE_DECIMAL:
        case MFTYPE_NEWDECIMAL:
        case MFTYPE_YEAR:
            v.parseNumber((const char*)value, (int32_t)cbValue);
            break;

        case MFTYPE_DATE:
        case MFTYPE_DATETIME:
        case MFTYPE_TIMESTAMP:
            v.parseDate((const char*)value, (int32_t)cbValue);
            break;

        case MFTYPE_TIME:
            v = exlib::string((const char*)value, cbValue);
            break;

        case MFTYPE_TINY_BLOB:
        case MFTYPE_MEDIUM_BLOB:
        case MFTYPE_LONG_BLOB:
        case MFTYPE_BLOB:
        case MFTYPE_STRING:
        case MFTYPE_VAR_STRING:
            if ((ti->flags & MFFLAG_BINARY_FLAG) && (ti->charset == MCS_binary)) {
                v = new Buffer(value, cbValue);
                break;
            }
        default:
            v = exlib::string((const char*)value, cbValue);
            break;
        }
    } else {
        v.setNull();
    }
}

int32_t API_resultRowValue(void* result, int32_t icolumn, UMTypeInfo* ti, void* value,
    size_t cbValue)
{
    Variant v;

    mysql::columnValue(ti, (const UINT8*)value, cbValue, v);
    ((DBResult*)result)->rowValue(icolumn, v);
    return true;
}

void API_resultRowEnd(void* result)
{
    ((DBResult*)result)->endRow();
}

void API_destroyResult(void* result)
{
    ((DBResult*)result)->Unref();
}

void* API_resultOK(UINT64 affected, UINT64 insertId, int32_t serverStatus,
    const char* message, size_t len)
{
    DBResult* res = new DBResult(0, affected, insertId);
    res->Ref();
    return res;
}

UMConnectionCAPI capi = {
    API_getSocket, API_deleteSocket, API_closeSocket, API_connectSocket,
    API_setTimeout, API_clearException, API_recvSocket, API_sendSocket,
    API_createResult, API_resultSetField, API_resultRowBegin,
    API_resultRowValue, API_resultRowEnd, API_destroyResult, API_resultOK
};

// ---------------------------------------------------------------------------
// MySQL Statement 游标实现（文本协议 + 客户端转义绑定 + 驱动流式状态机）
// ---------------------------------------------------------------------------

// 把 Variant 参数按 MySQL 字面量语义转义拼入 SQL（与 db_format 对齐）：
//   number → 裸数字；boolean → true/false；null/undefined → NULL；
//   Buffer → 0x hex；其他 → '...'（' 与 \ 转义）
static void appendMySQLValue(exlib::string& str, Variant& v)
{
    switch (v.type()) {
    case Variant::VT_Null:
    case Variant::VT_Undefined:
        str.append("NULL", 4);
        break;

    case Variant::VT_Boolean:
        str.append(v.boolVal() ? "true" : "false",
            v.boolVal() ? 4 : 5);
        break;

    case Variant::VT_Integer:
    case Variant::VT_Long:
    case Variant::VT_Number: {
        exlib::string s;
        v.toString(s);
        str.append(s);
        break;
    }

    case Variant::VT_Object: {
        object_base* obj = v.object();
        if (obj && obj->class_info().isInstance(Buffer_base::class_info())) {
            obj_ptr<Buffer> buf = (Buffer*)obj;
            exlib::string hex;
            buf->hex(hex);
            str.append("0x", 2);
            str.append(hex);
        } else {
            exlib::string s;
            v.toString(s);
            str.append(mysql::escape_string(s));
        }
        break;
    }

    default: {
        exlib::string s;
        v.toString(s);
        str.append(mysql::escape_string(s));
        break;
    }
    }
}

// 按 ? 占位符顺序替换参数（与 db_format::format 语义一致）
static result_t formatMySQL(const char* sql, std::vector<Variant>& args,
    exlib::string& retVal)
{
    exlib::string str;
    const char *p, *p1;
    size_t cnt = 0;

    while (*sql) {
        p = p1 = sql;
        while (*p1 && *p1 != '?')
            p1++;

        str.append(p, p1 - p);

        if (*p1) {
            p1++;

            if (cnt < args.size())
                appendMySQLValue(str, args[cnt]);
            else
                str.append(1, '?');

            cnt++;
        }

        sql = p1;
    }

    retVal = str;
    return 0;
}

class MySQLStmtImpl : public Statement::impl {
public:
    MySQLStmtImpl(mysql* db, exlib::string sql)
        : m_db(db)
        , m_sql(sql)
        , m_conn(NULL)
        , m_fieldCount(0)
        , m_open(false)
        , m_okResult(NULL)
    {
    }

    virtual ~MySQLStmtImpl()
    {
        reset();
    }

    virtual result_t open(std::vector<Variant>& args, bool& hasResult)
    {
        reset(); // 防御：上次游标未释放

        exlib::string full;
        result_t hr = formatMySQL(m_sql.c_str(), args, full);
        if (hr < 0)
            return hr;

        m_conn = (Connection*)m_db->m_conn;
        if (!m_conn)
            return CHECK_ERROR(CALL_E_CLOSED);

        int st = m_conn->beginQuery(full.c_str(), full.length());
        if (st < 0)
            return m_db->error();

        if (st == 0) {
            // OK 包：无结果集；affected/insertId 经 takeResult 读取
            m_okResult = m_conn->takeResult();
            hasResult = false;
            m_db->m_activeStmt = 1;
            return 0;
        }

        m_fieldCount = m_conn->fieldCount();
        hasResult = true;
        m_open = true;
        m_db->m_activeStmt = 1;
        return 0;
    }

    virtual result_t fetchRow(NObject* row, bool& done)
    {
        if (!m_conn || !m_open)
            return CHECK_ERROR(CALL_E_CLOSED);

        int r = m_conn->nextRow();
        if (r < 0)
            return m_db->error();

        if (r == 0) {
            done = true;
            m_open = false;
            return 0;
        }

        for (int32_t i = 0; i < m_fieldCount; i++) {
            size_t len = 0;
            const UINT8* value = m_conn->columnValue(i, &len);
            Variant v;
            mysql::columnValue(&m_conn->fieldInfo(i), value, len, v);
            row->add(m_conn->fieldName(i), v);
        }
        return 0;
    }

    virtual result_t runResult(int64_t& changes, int64_t& lastInsertId)
    {
        if (m_okResult) {
            DBResult* res = (DBResult*)m_okResult;
            changes = res->m_affected;
            lastInsertId = res->m_insertId;
            capi.destroyResult(m_okResult);
            m_okResult = NULL;
        }
        return 0;
    }

    virtual result_t columns(obj_ptr<NArray>& retVal)
    {
        return CHECK_ERROR(Runtime::setError(CALL_E_INVALID_CALL,
            "MySQL: columns() requires server-side prepared statements (v2)"));
    }

    // 游标复位：drain 剩余结果并释放并发保护（连接保持可用），impl 保留
    virtual void reset()
    {
        if (m_conn) {
            // abortResult 幂等：RS_NONE 时直接返回；RS_ROWSET（含 EOF 后，
            // 状态未复位）释放 beginQuery 持有的并发保护并 drain 剩余
            // 结果集，连接可继续复用。
            m_conn->abortResult();
            if (m_okResult) {
                capi.destroyResult(m_okResult);
                m_okResult = NULL;
            }
            m_conn = NULL;
        }
        m_open = false;
        m_fieldCount = 0;
        m_db->m_activeStmt = 0;
    }

    // 彻底释放（Statement 销毁时）；无服务端句柄，reset 即释放
    virtual void close()
    {
        reset();
    }

private:
    mysql* m_db;
    exlib::string m_sql;
    Connection* m_conn;
    int32_t m_fieldCount;
    bool m_open;
    void* m_okResult;
};

result_t mysql::prepareStmt(db_tmpl<MySQL_base, mysql>* db,
    exlib::string sql, obj_ptr<Statement_base>& retVal)
{
    if (!db->m_conn)
        return CHECK_ERROR(Runtime::setError(CALL_E_INVALID_CALL,
            "MySQL: database is closed."));

    // v1：文本协议 + 客户端转义绑定；编译推迟到 Statement 打开时
    // （无服务端预编译，SQL 每次执行时拼串发送）
    obj_ptr<Statement> stmt = new Statement(sql,
        new MySQLStmtImpl((mysql*)db, sql));
    retVal = stmt;
    return 0;
}

// ----------------------------------------------------------------------------------

result_t db_base::openMySQL(exlib::string connString, obj_ptr<MySQL_base>& retVal,
    AsyncEvent* ac)
{
    if (ac->isSync())
        return CHECK_ERROR(CALL_E_LONGSYNC);

    if (qstrcmp(connString.c_str(), "mysql:", 6))
        return CHECK_ERROR(Runtime::setError(CALL_E_INVALIDARG, "openMySQL: connection string must start with 'mysql:'."));

    obj_ptr<Url> u = new Url();

    result_t hr = u->parse(connString);
    if (hr < 0)
        return hr;

    int32_t nPort = 3306;
    exlib::string port = u->port();
    if (port.length() > 0)
        nPort = atoi(port.c_str());

    obj_ptr<mysql> conn = new mysql();

    exlib::string pathname = u->pathname();

    exlib::string username;
    exlib::string password;
    Url::decodeURI(u->username(), username);
    Url::decodeURI(u->password(), password);

    hr = conn->connect(u->hostname().c_str(), nPort, username.c_str(),
        password.c_str(),
        pathname.length() > 0 ? pathname.c_str() + 1 : "");
    if (hr < 0)
        return hr;

    retVal = conn;

    return 0;
}

mysql::~mysql()
{
    if (m_conn)
        async([conn = m_conn]() {
            UMConnection_Close(conn);
            UMConnection_Destroy(conn);
        });
}

result_t mysql::connect(const char* host, int32_t port, const char* username,
    const char* password, const char* dbName)
{
    if (m_conn)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    m_conn = UMConnection_Create(&capi);
    if (!UMConnection_Connect(m_conn, host, port, username, password, dbName,
            NULL, MCS_utf8mb4_bin)) {
        result_t hr = CHECK_ERROR(error());

        UMConnection_Destroy(m_conn);
        m_conn = NULL;

        return hr;
    }

    return 0;
}

result_t mysql::get_type(exlib::string& retVal)
{
    retVal = "mysql";
    return 0;
}

result_t mysql::close(AsyncEvent* ac)
{
    if (!m_conn)
        return 0;

    if (ac->isSync())
        return CHECK_ERROR(CALL_E_LONGSYNC);

    if (m_conn) {
        UMConnection_Close(m_conn);
        UMConnection_Destroy(m_conn);
        m_conn = NULL;
    }

    return 0;
}

result_t mysql::execute(exlib::string sql, obj_ptr<NArray>& retVal, AsyncEvent* ac)
{
    if (!m_conn)
        return CHECK_ERROR(Runtime::setError(CALL_E_INVALID_CALL, "MySQL: connection is closed."));

    if (m_activeStmt)
        return db_stmt_busy_error();

    if (ac->isSync())
        return CHECK_ERROR(CALL_E_LONGSYNC);

    DBResult* res = (DBResult*)UMConnection_Query(m_conn, sql.c_str(), sql.length());
    if (!res)
        return CHECK_ERROR(error());

    if (UMConnection_HasMoreResult(m_conn)) {
        retVal = new NArray();

        retVal->append(res);
        res->Unref();

        while (UMConnection_HasMoreResult(m_conn)) {
            res = (DBResult*)UMConnection_NextResultSet(m_conn);
            if (!res)
                return CHECK_ERROR(error());

            retVal->append(res);
            res->Unref();
        }
    } else {
        retVal = res;
        res->Unref();
    }

    return 0;
}

result_t mysql::get_rxBufferSize(int32_t& retVal)
{
    if (!m_conn)
        return CHECK_ERROR(Runtime::setError(CALL_E_INVALID_CALL, "MySQL: connection is closed."));

    retVal = UMConnection_GetRxBufferSize(m_conn);
    return 0;
}

result_t mysql::set_rxBufferSize(int32_t newVal)
{
    if (!m_conn)
        return CHECK_ERROR(Runtime::setError(CALL_E_INVALID_CALL, "MySQL: connection is closed."));

    UMConnection_SetRxBufferSize(m_conn, newVal);
    return 0;
}

result_t mysql::get_txBufferSize(int32_t& retVal)
{
    if (!m_conn)
        return CHECK_ERROR(Runtime::setError(CALL_E_INVALID_CALL, "MySQL: connection is closed."));

    retVal = UMConnection_GetTxBufferSize(m_conn);
    return 0;
}

result_t mysql::set_txBufferSize(int32_t newVal)
{
    if (!m_conn)
        return CHECK_ERROR(Runtime::setError(CALL_E_INVALID_CALL, "MySQL: connection is closed."));

    UMConnection_SetTxBufferSize(m_conn, newVal);
    return 0;
}

result_t mysql::getTables(obj_ptr<NArray>& retVal, AsyncEvent* ac)
{
    if (!m_conn)
        return CHECK_ERROR(Runtime::setError(CALL_E_INVALID_CALL, "MySQL: connection is closed."));

    if (ac->isSync())
        return CHECK_ERROR(CALL_E_LONGSYNC);

    // Query MySQL information_schema to get all tables in current database
    exlib::string sql = "SELECT table_name AS name FROM information_schema.tables WHERE table_schema = DATABASE() ORDER BY table_name";
    return execute(sql, retVal, ac);
}

result_t mysql::getTableInfo(exlib::string tableName, obj_ptr<NArray>& retVal, AsyncEvent* ac)
{
    if (!m_conn)
        return CHECK_ERROR(Runtime::setError(CALL_E_INVALID_CALL, "MySQL: connection is closed."));

    if (ac->isSync())
        return CHECK_ERROR(CALL_E_LONGSYNC);

    // Query MySQL information_schema to get table column information
    exlib::string escapedTableName = escape_string(tableName);
    exlib::string sql = "SELECT COLUMN_NAME AS column_name, DATA_TYPE AS data_type, CHARACTER_MAXIMUM_LENGTH AS character_maximum_length, IS_NULLABLE AS is_nullable, COLUMN_DEFAULT AS column_default FROM information_schema.columns WHERE table_schema = DATABASE() AND table_name = " + escapedTableName + " ORDER BY ordinal_position";
    return execute(sql, retVal, ac);
}

} /* namespace fibjs */
