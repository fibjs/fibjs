/*
 * db.cpp
 *
 *  Created on: Jul 20, 2012
 *      Author: lion
 */

#include "ifs/db.h"
#include "Url.h"
#include "SQLite.h"

namespace fibjs
{

result_t db_base::open(const char* connString,
		obj_ptr<DbConnection_base>& retVal)
{
	if (!qstrcmp(connString, "mysql:", 6))
	{
		obj_ptr<MySQL_base> db;
		result_t hr;

		hr = openMySQL(connString, db);
		if (hr < 0)
			return hr;

		retVal = db;
		return 0;
	}

	if (!qstrcmp(connString, "sqlite:", 7))
		return openSQLite(connString, retVal);

	return CALL_E_INVALIDARG;
}

result_t db_base::openMySQL(const char* connString, obj_ptr<MySQL_base>& retVal)
{
	if (qstrcmp(connString, "mysql:", 6))
		return CALL_E_INVALIDARG;

	obj_ptr<Url> u = new Url();

	result_t hr = u->parse(connString);
	if (hr < 0)
		return hr;

	int nPort = 3306;
	if (u->m_port.length() > 0)
		nPort = atoi(u->m_port.c_str());

	return openMySQL(u->m_hostname.c_str(), nPort, u->m_username.c_str(),
			u->m_password.c_str(),
			u->m_pathname.length() > 0 ? u->m_pathname.c_str() + 1 : "", retVal);
}

result_t db_base::openSQLite(const char* connString,
		obj_ptr<DbConnection_base>& retVal)
{
	result_t hr;

	if (!qstrcmp(connString, "sqlite:", 7))
		connString += 7;

	obj_ptr<SQLite> db = new SQLite();
	hr = db->open(connString);
	if (hr < 0)
		return hr;

	retVal = db;

	return 0;
}

result_t db_base::format(const char* sql, const v8::Arguments& args,
		std::string& retVal)
{
	retVal = sql;
	return 0;
}

}
