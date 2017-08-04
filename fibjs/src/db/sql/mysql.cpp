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
#include "ifs/db.h"
#include "DBResult.h"
#include "Url.h"
#include "trans.h"

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

int32_t API_resultRowValue(void* result, int32_t icolumn, UMTypeInfo* ti, void* value,
    size_t cbValue)
{
    Variant v;

    if (value)
        switch (ti->type) {
        case MFTYPE_NULL:
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
            v.parseNumber((const char*)value, (int32_t)cbValue);
            break;

        case MFTYPE_DATE:
        case MFTYPE_TIME:
        case MFTYPE_DATETIME:
            v.parseDate((const char*)value, (int32_t)cbValue);
            break;

        case MFTYPE_TINY_BLOB:
        case MFTYPE_MEDIUM_BLOB:
        case MFTYPE_LONG_BLOB:
        case MFTYPE_BLOB:
            v = new Buffer((const char*)value, cbValue);
            break;

        default:
            v = exlib::string((const char*)value, cbValue);
            break;
        }

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

// ----------------------------------------------------------------------------------

result_t db_base::openMySQL(exlib::string connString, obj_ptr<MySQL_base>& retVal,
    AsyncEvent* ac)
{
    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    if (qstrcmp(connString.c_str(), "mysql:", 6))
        return CHECK_ERROR(CALL_E_INVALIDARG);

    obj_ptr<Url> u = new Url();

    result_t hr = u->parse(connString);
    if (hr < 0)
        return hr;

    int32_t nPort = 3306;
    if (u->m_port.length() > 0)
        nPort = atoi(u->m_port.c_str());

    obj_ptr<mysql> conn = new mysql();

    hr = conn->connect(u->m_hostname.c_str(), nPort, u->m_username.c_str(),
        u->m_password.c_str(),
        u->m_pathname.length() > 0 ? u->m_pathname.c_str() + 1 : "");
    if (hr < 0)
        return hr;

    retVal = conn;

    return 0;
}

static result_t close_conn(UMConnection conn)
{
    UMConnection_Close(conn);
    UMConnection_Destroy(conn);
    return 0;
}

mysql::~mysql()
{
    if (m_conn) {
        asyncCall(close_conn, m_conn);
        m_conn = NULL;
    }
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
        return CHECK_ERROR(CALL_E_NOSYNC);

    if (m_conn) {
        UMConnection_Close(m_conn);
        UMConnection_Destroy(m_conn);
        m_conn = NULL;
    }

    return 0;
}

result_t mysql::use(exlib::string dbName, AsyncEvent* ac)
{
    if (!m_conn)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    obj_ptr<DBResult_base> retVal;
    exlib::string s("USE ", 4);
    s.append(dbName);
    return execute(s.c_str(), (int32_t)s.length(), retVal);
}

result_t mysql::begin(AsyncEvent* ac)
{
    if (!m_conn)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    obj_ptr<DBResult_base> retVal;
    return execute("BEGIN", 5, retVal);
}

result_t mysql::commit(AsyncEvent* ac)
{
    if (!m_conn)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    obj_ptr<DBResult_base> retVal;
    return execute("COMMIT", 6, retVal);
}

result_t mysql::rollback(AsyncEvent* ac)
{
    if (!m_conn)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    obj_ptr<DBResult_base> retVal;
    return execute("ROLLBACK", 8, retVal);
}

result_t mysql::trans(v8::Local<v8::Function> func)
{
    return _trans(this, func);
}

result_t mysql::execute(const char* sql, int32_t sLen,
    obj_ptr<DBResult_base>& retVal)
{
    if (!m_conn)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    DBResult* res = (DBResult*)UMConnection_Query(m_conn, sql, sLen);
    if (!res)
        return CHECK_ERROR(error());

    res->freeze();
    retVal = res;
    res->Unref();

    return 0;
}

result_t mysql::execute(exlib::string sql, obj_ptr<DBResult_base>& retVal, AsyncEvent* ac)
{
    if (!m_conn)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    return execute(sql.c_str(), (int32_t)sql.length(), retVal);
}

result_t mysql::execute(exlib::string sql, OptArgs args, obj_ptr<DBResult_base>& retVal,
    AsyncEvent* ac)
{
    if (!m_conn)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    if (ac->isSync()) {
        exlib::string str;
        result_t hr = format(sql, args, str);
        if (hr < 0)
            return hr;

        ac->m_ctx.resize(1);
        ac->m_ctx[0] = str;

        return CHECK_ERROR(CALL_E_NOSYNC);
    }

    exlib::string str = ac->m_ctx[0].string();
    return execute(str, retVal, ac);
}

result_t mysql::format(exlib::string sql, OptArgs args,
    exlib::string& retVal)
{
    return db_base::formatMySQL(sql, args, retVal);
}

result_t mysql::get_rxBufferSize(int32_t& retVal)
{
    if (!m_conn)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    retVal = UMConnection_GetRxBufferSize(m_conn);
    return 0;
}

result_t mysql::set_rxBufferSize(int32_t newVal)
{
    if (!m_conn)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    UMConnection_SetRxBufferSize(m_conn, newVal);
    return 0;
}

result_t mysql::get_txBufferSize(int32_t& retVal)
{
    if (!m_conn)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    retVal = UMConnection_GetTxBufferSize(m_conn);
    return 0;
}

result_t mysql::set_txBufferSize(int32_t newVal)
{
    if (!m_conn)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    UMConnection_SetTxBufferSize(m_conn, newVal);
    return 0;
}

} /* namespace fibjs */
