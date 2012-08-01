/*
 * SQLite.cpp
 *
 *  Created on: Aug 1, 2012
 *      Author: lion
 */

#include "SQLite.h"
#include "ifs/db.h"
#include "DBResult.h"

namespace fibjs
{

result_t SQLite::open(const char* file)
{
	if (sqlite3_open(file, &m_db))
	{
		result_t hr = Runtime::setError(sqlite3_errmsg(m_db));
		sqlite3_close(m_db);
		m_db = NULL;
		return hr;
	}

	m_file = file;

	return 0;
}

result_t SQLite::close()
{
	if (!m_db)
		return CALL_E_INVALID_CALL;

	sqlite3_close(m_db);
	m_db = NULL;

	return 0;
}

result_t SQLite::begin()
{
	obj_ptr<DBResult_base> retVal;
	return execute("BEGIN", retVal);
}

result_t SQLite::commit()
{
	obj_ptr<DBResult_base> retVal;
	return execute("COMMIT", retVal);
}

result_t SQLite::rollback()
{
	obj_ptr<DBResult_base> retVal;
	return execute("ROLLBACK", retVal);
}

result_t SQLite::execute(const char* sql, obj_ptr<DBResult_base>& retVal)
{
	if (!m_db)
		return CALL_E_INVALID_CALL;

	sqlite3_stmt *stmt = 0;
	const char *pStr1;
	int sLen = (int)qstrlen(sql);

	if (sqlite3_prepare_v2(m_db, sql, sLen, &stmt, &pStr1))
	{
		result_t hr = Runtime::setError(sqlite3_errmsg(m_db));
		if (stmt)
			sqlite3_finalize(stmt);
		return hr;
	}

	if (stmt)
	{
		int columns = sqlite3_column_count(stmt);
		obj_ptr<DBResult> res;

		if (columns > 0)
		{
			int i;
			res = new DBResult(columns);

			for (i = 0; i < columns; i++)
			{
				std::string s = sqlite3_column_name(stmt, i);
				res->setField(i, s);
			}

			while (true)
			{
				int r = sqlite3_step(stmt);
				if (r == SQLITE_ROW)
				{
					res->beginRow();
					for (i = 0; i < columns; i++)
					{
						Variant v;

						switch (sqlite3_column_type(stmt, i))
						{
						case SQLITE_INTEGER:
							v = sqlite3_column_int64(stmt, i);
							break;
						case SQLITE_FLOAT:
							v = sqlite3_column_double(stmt, i);
							break;
						case SQLITE_NULL:
							break;
						default:
							v = (const char *) sqlite3_column_text(stmt, i);
							break;
						}

						res->rowValue(i, v);
					}
					res->endRow();
				}
				else if (r == SQLITE_DONE)
					break;
				else
				{
					sqlite3_finalize(stmt);
					return Runtime::setError(sqlite3_errmsg(m_db));
				}
			}
		}
		else
		{
			int r = sqlite3_step(stmt);
			if (r == SQLITE_DONE)
				res = new DBResult(sqlite3_changes(m_db),
						sqlite3_last_insert_rowid(m_db));
			else
			{
				sqlite3_finalize(stmt);
				return Runtime::setError(sqlite3_errmsg(m_db));
			}
		}

		sqlite3_finalize(stmt);

		retVal = res;
	}

	return 0;
}

result_t SQLite::execute(const char* sql, const v8::Arguments& args,
		obj_ptr<DBResult_base>& retVal)
{
	std::string str;
	result_t hr = db_base::format(sql, args, str);
	if (hr < 0)
		return hr;

	return execute(str.c_str(), retVal);
}

result_t SQLite::get_fileName(std::string& retVal)
{
	if (!m_db)
		return CALL_E_INVALID_CALL;

	retVal = m_file;
	return 0;
}

result_t SQLite::get_timeout(int32_t& retVal)
{
	if (!m_db)
		return CALL_E_INVALID_CALL;

	retVal = m_nCmdTimeout;
	return 0;
}

result_t SQLite::set_timeout(int32_t newVal)
{
	if (!m_db)
		return CALL_E_INVALID_CALL;

	m_nCmdTimeout = newVal;
	sqlite3_busy_timeout(m_db, m_nCmdTimeout);
	return 0;
}

result_t SQLite::backup(const char* fileName)
{
	if (!m_db)
		return CALL_E_INVALID_CALL;

	int rc;
	struct sqlite3 *db2 = NULL;
	sqlite3_backup *pBackup;

	if (sqlite3_open(fileName, &db2))
	{
		result_t hr = Runtime::setError(sqlite3_errmsg(db2));
		sqlite3_close(m_db);
		return hr;
	}

	pBackup = sqlite3_backup_init(db2, "main", m_db, "main");
	if (pBackup)
	{
		do
		{
			rc = sqlite3_backup_step(pBackup, 5);
			if (rc == SQLITE_LOCKED)
				sqlite3_sleep(1);
		} while (rc == SQLITE_OK || rc == SQLITE_BUSY || rc == SQLITE_LOCKED);

		sqlite3_backup_finish(pBackup);
	}
	else
	{
		result_t hr = Runtime::setError(sqlite3_errmsg(db2));
		sqlite3_close(m_db);
		return hr;
	}

	sqlite3_close(m_db);

	return 0;
}

} /* namespace fibjs */
