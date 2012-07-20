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
	return ((Socket*) sock)->ac_connect(host, port) >= 0;
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

	if (((Socket*) sock)->ac_read(cbBuffer, retVal) < 0)
		return -1;

	std::string strBuf;

	if (retVal != NULL)
	{
		retVal->toString(strBuf);
		memcpy(buffer, strBuf.c_str(), strBuf.length());
	}

	return (int)strBuf.length();
}

int API_sendSocket(void *sock, const char *buffer, int cbBuffer)
{
	std::string strBuf;
	obj_ptr<Buffer_base> buf;

	strBuf.assign(buffer, cbBuffer);
	buf = new Buffer(strBuf);

	if (((Socket*) sock)->ac_write(buf) < 0)
		return -1;

	return (int)strBuf.length();
}

void *API_createResult(int columns)
{
	puts("API_createResult");
	return NULL;
}

void API_resultSetField(void *result, int ifield, UMTypeInfo *ti, void *name,
		size_t cbName)
{
	puts("resultSetField");

}

void API_resultRowBegin(void *result)
{
	puts("API_resultRowBegin");

}

int API_resultRowValue(void *result, int icolumn, UMTypeInfo *ti, void *value,
		size_t cbValue)
{
	puts("API_resultRowValue");
	return 0;
}

void API_resultRowEnd(void *result)
{
	puts("API_resultRowEnd");

}

void API_destroyResult(void *result)
{
	puts("API_destroyResult");

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
		obj_ptr<DbConnection_base>& retVal)
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
	m_conn = UMConnection_Create(&capi);
	if (!UMConnection_Connect(m_conn, host, port, username, password, dbName,
			NULL, MCS_utf8_bin))
	{
		const char* errorMessage;
		int errCode;
		int errType;

		UMConnection_GetLastError(m_conn, &errorMessage, &errCode, &errType);
		printf("ERROR: [%d:%d] %s\n", errCode, errType, errorMessage);
		return CALL_E_INVALID_CALL;
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

result_t mysql::execute(const char* sql)
{
	return 0;
}

} /* namespace fibjs */
