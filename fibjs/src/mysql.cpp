/*
 * mysql.cpp
 *
 *  Created on: Jul 20, 2012
 *      Author: lion
 */

#include "mysql.h"
#include "Socket_api.h"
#include "Buffer.h"
#include "ifs/db.h"
#include "DBResult.h"
#include "Url.h"

namespace fibjs
{

void *API_getSocket()
{
	return fibjs::socket::create();
}

void API_deleteSocket(void *sock)
{
	fibjs::socket::destroy(sock);
}

void API_closeSocket(void *sock)
{
	fibjs::socket::close(sock);
}

int API_connectSocket(void *sock, const char *host, int port)
{
	return fibjs::socket::connect(sock, host, port);
}

int API_setTimeout(void *sock, int timeoutSec)
{
	return 1;
}

void API_clearException(void)
{
}

int API_recvSocket(void *sock, char *buffer, int cbBuffer)
{
	return fibjs::socket::recv(sock, buffer, cbBuffer);
}

int API_sendSocket(void *sock, const char *buffer, int cbBuffer)
{
	return fibjs::socket::send(sock, buffer, cbBuffer);
}

void *API_createResult(int columns, void* opt)
{
	DBResult* res = new DBResult(columns);
	res->Ref();
	return res;
}

void API_resultSetField(void *result, int ifield, UMTypeInfo *ti, void *name,
		size_t cbName, void* opt)
{
	std::string s((char*) name, cbName);
	((DBResult*) result)->setField(ifield, s);
}

void API_resultRowBegin(void *result, void* opt)
{
	((DBResult*) result)->beginRow();
}

int API_resultRowValue(void *result, int icolumn, UMTypeInfo *ti, void *value,
		size_t cbValue, void* opt)
{
	Variant v;

	if (value)
		switch (ti->type)
		{
		case MFTYPE_NULL:
			break;

		case MFTYPE_TINY:
		case MFTYPE_SHORT:
		case MFTYPE_LONG:
		case MFTYPE_INT24:
		case MFTYPE_LONGLONG:
		case MFTYPE_FLOAT:
		case MFTYPE_DOUBLE:
			v.parseNumber((const char*) value, (int) cbValue);
			break;

		case MFTYPE_DATE:
		case MFTYPE_TIME:
		case MFTYPE_DATETIME:
			v.parseDate((const char*) value, (int) cbValue);
			break;

		case MFTYPE_TINY_BLOB:
		case MFTYPE_MEDIUM_BLOB:
		case MFTYPE_LONG_BLOB:
		case MFTYPE_BLOB:
			v = new Buffer(std::string((const char*) value, cbValue));
			break;

		default:
			v = std::string((const char*) value, cbValue);
			break;
		}

	((DBResult*) result)->rowValue(icolumn, v);
	return true;
}

int API_resultRowEnd(void *result, void* opt)
{
	DBResult* res = (DBResult*) result;
	mysql* db = (mysql*)opt;

	res->endRow();

	if (!db->m_func.IsEmpty())
	{
		Variant val;

		res->_indexed_getter(0, val);
		res->resize(0);

		v8::Handle < v8::Value > v;
		v = val;
		v8::Handle < v8::Value > r = db->m_func->Call(db->wrap(), 1,
				&v);
		if (r.IsEmpty())
		{
			Runtime::setError(CALL_E_JAVASCRIPT);
			return 0;
		}
	}

	return 1;
}

void API_destroyResult(void *result, void* opt)
{
	((DBResult*) result)->Unref();
}

void *API_resultOK(UINT64 affected, UINT64 insertId, int serverStatus,
		const char *message, size_t len, void* opt)
{
	DBResult* res = new DBResult(affected, insertId);
	res->Ref();
	return res;
}

UMConnectionCAPI capi =
{ API_getSocket, API_deleteSocket, API_closeSocket, API_connectSocket,
		API_setTimeout, API_clearException, API_recvSocket, API_sendSocket,
		API_createResult, API_resultSetField, API_resultRowBegin,
		API_resultRowValue, API_resultRowEnd, API_destroyResult, API_resultOK };

// ----------------------------------------------------------------------------------

result_t db_base::openMySQL(const char* connString, obj_ptr<MySQL_base>& retVal)
{
	if (qstrcmp(connString, "mysql:", 6))
		return CALL_E_INVALIDARG;

	obj_ptr < Url > u = new Url();

	result_t hr = u->parse(connString);
	if (hr < 0)
		return hr;

	int nPort = 3306;
	if (u->m_port.length() > 0)
		nPort = atoi(u->m_port.c_str());

	obj_ptr < mysql > conn = new mysql();

	hr = conn->connect(u->m_hostname.c_str(), nPort, u->m_username.c_str(),
			u->m_password.c_str(),
			u->m_pathname.length() > 0 ? u->m_pathname.c_str() + 1 : "");
	if (hr < 0)
		return hr;

	retVal = conn;

	return 0;
}

result_t mysql::connect(const char *host, int port, const char *username,
		const char *password, const char *dbName)
{
	if (m_conn)
		return CALL_E_INVALID_CALL;

	m_conn = UMConnection_Create(&capi);
	if (!UMConnection_Connect(m_conn, host, port, username, password, dbName,
			NULL, MCS_utf8mb4_bin))
	{
		result_t hr = error();

		UMConnection_Destroy(m_conn);
		m_conn = NULL;

		return hr;
	}

	m_rxBufferSize = UMConnection_GetRxBufferSize(m_conn);
	m_txBufferSize = UMConnection_GetTxBufferSize(m_conn);

	return 0;
}

result_t mysql::close()
{
	if (m_conn)
	{
		UMConnection_Close(m_conn);
		UMConnection_Destroy(m_conn);
		m_conn = NULL;
	}

	return 0;
}

result_t mysql::use(const char* dbName)
{
	obj_ptr < DBResult_base > retVal;
	std::string s("USE ", 4);
	s.append(dbName);
	return execute(s.c_str(), (int) s.length(), retVal);
}

result_t mysql::begin()
{
	obj_ptr < DBResult_base > retVal;
	return execute("BEGIN", 5, retVal);
}

result_t mysql::commit()
{
	obj_ptr < DBResult_base > retVal;
	return execute("COMMIT", 6, retVal);
}

result_t mysql::rollback()
{
	obj_ptr < DBResult_base > retVal;
	return execute("ROLLBACK", 8, retVal);
}

result_t mysql::execute(const char* sql, int sLen,
		obj_ptr<DBResult_base>& retVal)
{
	if (!m_conn)
		return CALL_E_INVALID_CALL;

	DBResult* res = (DBResult*) UMConnection_Query(m_conn, sql, sLen, this);
	if (!res)
		return error();

	retVal = res;
	res->Unref();

	return 0;
}

result_t mysql::execute(const char* sql, const v8::Arguments& args,
		obj_ptr<DBResult_base>& retVal)
{
	std::string str;
	result_t hr = db_base::formatMySQL(sql, args, str);
	if (hr < 0)
		return hr;

	v8::Handle < v8::Value > v = args[args.Length() - 1];
	if (v->IsFunction())
		m_func = v8::Handle < v8::Function > ::Cast(v);

	hr = execute(str.c_str(), (int) str.length(), retVal);
	m_func.Clear();

	return hr;
}

result_t mysql::get_rxBufferSize(int32_t& retVal)
{
	retVal = m_rxBufferSize;
	return 0;
}

result_t mysql::get_txBufferSize(int32_t& retVal)
{
	retVal = m_txBufferSize;
	return 0;
}

} /* namespace fibjs */
