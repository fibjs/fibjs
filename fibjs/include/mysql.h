/*
 * mysql.h
 *
 *  Created on: Jul 20, 2012
 *      Author: lion
 */

#ifndef MYSQL_H_
#define MYSQL_H_

#include "ifs/MySQL.h"
#include <umysql/umysql.h>

namespace fibjs
{

class mysql: public MySQL_base
{
public:
	mysql() :
			m_conn(NULL)
	{
	}

	virtual ~mysql()
	{
		close();
	}

public:
	// DbConnection_base
	virtual result_t close();
	virtual result_t use(const char* dbName);
	virtual result_t beginTrans();
	virtual result_t commitTrans();
	virtual result_t rollBack();
	virtual result_t execute(const char* sql, obj_ptr<DBResult_base>& retVal);

public:
	// MySQL_base
	virtual result_t get_rxBufferSize(int32_t& retVal);
	virtual result_t get_txBufferSize(int32_t& retVal);

public:
	result_t connect(const char *host, int port, const char *username,
			const char *password, const char *dbName);

private:
	inline result_t error()
	{
		const char* errorMessage = NULL;
		int errCode;
		int errType;

		if (UMConnection_GetLastError(m_conn, &errorMessage, &errCode,
				&errType))
			return Runtime::setError(errorMessage);
		return Runtime::errNumber();
	}

private:
	UMConnection m_conn;
};

} /* namespace fibjs */
#endif /* MYSQL_H_ */
