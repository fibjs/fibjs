/*
 * mysql.h
 *
 *  Created on: Jul 20, 2012
 *      Author: lion
 */

#ifndef MYSQL_H_
#define MYSQL_H_

#include "ifs/DbConnection.h"
#include <umysql/umysql.h>

namespace fibjs
{

class mysql: public DbConnection_base
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
	virtual result_t execute(const char* sql);

public:
	result_t connect(const char *host, int port, const char *username,
			const char *password, const char *dbName);

private:
	UMConnection m_conn;
};

} /* namespace fibjs */
#endif /* MYSQL_H_ */
