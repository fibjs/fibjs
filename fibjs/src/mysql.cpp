/*
 * mysql.cpp
 *
 *  Created on: Jul 20, 2012
 *      Author: lion
 */

#include "mysql.h"
#include "Socket.h"
#include "Buffer.h"
#include "ifs/db.h"
#include "DBResult.h"

namespace fibjs
{

void *API_getSocket()
{
	Socket* s = new Socket();
	s->Ref();

	result_t hr = s->create(net_base::_AF_INET, net_base::_SOCK_STREAM);
	if (hr < 0)
	{
		s->Unref();
		Runtime::setError(hr);
		return NULL;
	}

	return s;
}

void API_deleteSocket(void *sock)
{
	((Socket*) sock)->Unref();
}

void API_closeSocket(void *sock)
{
	((Socket*) sock)->ac_close();
}

int API_connectSocket(void *sock, const char *host, int port)
{
	result_t hr = ((Socket*) sock)->ac_connect(host, port);
	if(hr < 0)
	{
		Runtime::setError(hr);
		return 0;
	}

	return 1;
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
	obj_ptr<Buffer_base> retVal;

	result_t hr = ((Socket*) sock)->ac_read(cbBuffer, retVal);
	if(hr < 0)
	{
		Runtime::setError(hr);
		return -1;
	}

	std::string strBuf;

	if (retVal != NULL)
	{
		retVal->toString(strBuf);
		memcpy(buffer, strBuf.c_str(), strBuf.length());
	}

	return (int) strBuf.length();
}

int API_sendSocket(void *sock, const char *buffer, int cbBuffer)
{
	std::string strBuf(buffer, cbBuffer);
	obj_ptr<Buffer_base> buf;

	buf = new Buffer(strBuf);

	result_t hr = ((Socket*) sock)->ac_write(buf);
	if(hr < 0)
	{
		Runtime::setError(hr);
		return -1;
	}

	return (int) strBuf.length();
}

void *API_createResult(int columns)
{
	DBResult* res = new DBResult(columns);
	res->Ref();
	return res;
}

void API_resultSetField(void *result, int ifield, UMTypeInfo *ti, void *name,
		size_t cbName)
{
	std::string s((char*) name, cbName);
	((DBResult*) result)->setField(ifield, ti->type, s);
}

void API_resultRowBegin(void *result)
{
	((DBResult*) result)->beginRow();
}

int API_resultRowValue(void *result, int icolumn, UMTypeInfo *ti, void *value,
		size_t cbValue)
{
	std::string s((const char*)value, cbValue);
	Variant v;
	v = s;

	((DBResult*) result)->rowValue(icolumn, v);
	return true;
}

void API_resultRowEnd(void *result)
{
	((DBResult*) result)->endRow();
}

void API_destroyResult(void *result)
{
	DBResult* res = (DBResult*) result;
	res->Unref();
}

void *API_resultOK(UINT64 affected, UINT64 insertId, int serverStatus,
		const char *message, size_t len)
{
	puts("API_resultOK");
	return NULL;
}

UMConnectionCAPI capi =
{ API_getSocket, API_deleteSocket, API_closeSocket, API_connectSocket,
		API_setTimeout, API_clearException, API_recvSocket, API_sendSocket,
		API_createResult, API_resultSetField, API_resultRowBegin,
		API_resultRowValue, API_resultRowEnd, API_destroyResult, API_resultOK };

// ----------------------------------------------------------------------------------

result_t db_base::openMySQL(const char* host, int32_t port,
		const char* username, const char* password, const char* dbName,
		obj_ptr<MySQL_base>& retVal)
{
	obj_ptr<mysql> conn = new mysql();

	result_t hr = conn->connect(host, port, username, password, dbName);
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
			NULL, MCS_utf8_bin))
	{
		result_t hr = error();

		UMConnection_Destroy(m_conn);
		m_conn = NULL;

		return hr;
	}

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
	return 0;
}

result_t mysql::beginTrans()
{
	return 0;
}

result_t mysql::commitTrans()
{
	return 0;
}

result_t mysql::rollBack()
{
	return 0;
}

result_t mysql::execute(const char* sql, obj_ptr<DBResult_base>& retVal)
{
	if (!m_conn)
		return CALL_E_INVALID_CALL;

	DBResult* res = (DBResult*) UMConnection_Query(m_conn, sql, qstrlen(sql));
	if (!res)
		return error();

	retVal = res;
	res->Unref();

	return 0;
}

result_t mysql::get_rxBufferSize(int32_t& retVal)
{
	return 0;
}

result_t mysql::get_txBufferSize(int32_t& retVal)
{
	return 0;
}

} /* namespace fibjs */
