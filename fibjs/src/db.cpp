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

inline void _escape(const char* str, int sz, std::string& retVal)
{
	int len, l;
	const char* src;
	char* bstr;
	char ch;

	for (len = 0, src = str, l = sz; l > 0; len++, l--)
	{
		ch = (unsigned char) *src++;

		if (ch == 0 || ch == 0x1a)
			len += 3;
		else if (ch == '\r' || ch == '\n' || ch == '\\' || ch == '\''
				|| ch == '\"')
			len++;
	}

	retVal.resize(len);

	bstr = &retVal[0];

	for (src = str, l = sz; l > 0; l--)
	{
		ch = (unsigned char) *src++;

		switch (ch)
		{
		case 0:
			*bstr++ = '\\';
			*bstr++ = 'x';
			*bstr++ = '0';
			*bstr++ = '0';
			break;
		case 0x1a:
			*bstr++ = '\\';
			*bstr++ = 'x';
			*bstr++ = '1';
			*bstr++ = 'a';
			break;

		case '\r':
			*bstr++ = '\\';
			*bstr++ = 'r';
			break;

		case '\n':
			*bstr++ = '\\';
			*bstr++ = 'n';
			break;

		case '\\':
			*bstr++ = '\\';
			*bstr++ = '\\';
			break;

		case '\'':
			*bstr++ = '\\';
			*bstr++ = '\'';
			break;

		case '\"':
			*bstr++ = '\\';
			*bstr++ = '\"';
			break;

		default:
			*bstr++ = ch;
			break;
		}
	}
}

result_t db_base::format(const char* sql, const v8::Arguments& args,
		std::string& retVal)
{
	std::string str;
	const char *p, *p1;
	int cnt = 1;

	while (*sql)
	{
		p = p1 = sql;
		while (*p1 && *p1 != '?')
			p1++;

		str.append(p, p1 - p);

		if (*p1)
		{
			p1++;

			str += '\'';
			if (cnt < args.Length())
			{
				v8::Handle<v8::Value> v = args[cnt];
				std::string s;

				if (v->IsDate())
				{
					date_t d = v;
					d.sqlString(s);
				}
				else
				{
					v8::String::Utf8Value s1(v);
					_escape(*s1, s1.length(), s);
				}

				str.append(s);
			}
			str += '\'';

			cnt++;
		}

		sql = p1;
	}

	retVal = str;
	return 0;
}

result_t db_base::escape(const char* str, std::string& retVal)
{
	_escape(str, (int)qstrlen(str), retVal);
	return 0;
}

result_t db_base::escape(Buffer_base* data, std::string& retVal)
{
	std::string str;
	data->toString(str);
	_escape(str.c_str(), (int)str.length(), retVal);

	return 0;
}

}
