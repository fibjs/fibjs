/*
 * MongoDB.cpp
 *
 *  Created on: Oct 25, 2012
 *      Author: lion
 */

#include "ifs/db.h"
#include "MongoDB.h"
#include "MongoCollection.h"
#include "GridFS.h"
#include "Socket_api.h"
#include <mongo/env.h>
#include "Url.h"
#include "encoding_bson.h"
#include "MongoID.h"

int mongo_env_set_socket_op_timeout(mongo *conn, int millis)
{
	return MONGO_OK;
}

int mongo_env_read_socket(mongo *conn, void *buf, int len)
{
	if (fibjs::socket::read(conn->sock, buf, len) < 0)
	{
		__mongo_set_error(conn, MONGO_IO_ERROR, NULL,
				fibjs::Runtime::errNumber());
		return MONGO_ERROR;
	}

	return MONGO_OK;
}

int mongo_env_write_socket(mongo *conn, const void *buf, int len)
{
	if (fibjs::socket::send(conn->sock, buf, len) < 0)
	{
		__mongo_set_error(conn, MONGO_IO_ERROR, NULL,
				fibjs::Runtime::errNumber());
		return MONGO_ERROR;
	}

	return MONGO_OK;
}

int mongo_env_socket_connect(mongo *conn, const char *host, int port)
{
	if (!(conn->sock = fibjs::socket::connect(host, port)))
	{
		__mongo_set_error(conn, MONGO_IO_ERROR, NULL,
				fibjs::Runtime::errNumber());
		return MONGO_ERROR;
	}

	return MONGO_OK;
}

int mongo_env_sock_init(void)
{
	return 0;
}

int mongo_env_close_socket(void* socket)
{
	fibjs::socket::close(socket);
	return 0;
}

int mongo_run_command(mongo *conn, const char *db, const bson *command,
		bson *out)
{
	int ret = MONGO_OK;
	bson response =
	{ NULL, 0 };
	bson fields;
	int sl = (int) strlen(db);
	char *ns = (char*) bson_malloc(sl + 5 + 1); /* ".$cmd" + nul */
	int res, success = 0;

	strcpy(ns, db);
	strcpy(ns + sl, ".$cmd");

	res = mongo_find_one(conn, ns, command, bson_empty(&fields), &response);
	bson_free(ns);

	if (res != MONGO_OK)
		ret = MONGO_ERROR;
	else
	{
		bson_iterator it;
		if (bson_find(&it, &response, "ok"))
			success = bson_iterator_bool(&it);

		if (!success)
		{
			if (bson_find(&it, &response, "errmsg"))
			{
				int result_len = bson_iterator_string_len(&it);
				const char *result_string = bson_iterator_string(&it);
				int len =
						result_len < MONGO_ERR_LEN ? result_len : MONGO_ERR_LEN;
				memcpy(conn->lasterrstr, result_string, len);
				conn->lasterrcode = -1;
			}
			else
				conn->err = MONGO_COMMAND_FAILED;

			bson_destroy(&response);
			ret = MONGO_ERROR;
		}
		else
		{
			if (out)
				*out = response;
			else
				bson_destroy(&response);
		}
	}
	return ret;
}

namespace fibjs
{

result_t MongoDB::error()
{
	static const char* s_msgs[] =
	{ "Connection success!", "Could not create a socket.",
			"An error occured while calling connect().",
			"An error occured while calling getaddrinfo().",
			"Warning: connected to a non-master node (read-only).",
			"Given rs name doesn't match this replica set.",
			"Can't find primary in replica set. Connection closed.",
			"An error occurred while reading or writing on the socket.",
			"Other socket error.", "The response is not the expected length.",
			"The command returned with 'ok' value of 0.",
			"Write with given write_concern returned an error.",
			"The name for the ns (database or collection) is invalid.",
			"BSON not valid for the specified op.",
			"BSON object has not been finished.",
			"BSON object exceeds max BSON size.",
			"Supplied write concern object is invalid." };

	result_t hr = 0;

	if (m_conn.err == MONGO_IO_ERROR)
		hr = m_conn.errcode;
	else if (m_conn.err > 0 && m_conn.err <= MONGO_WRITE_CONCERN_INVALID)
		hr = Runtime::setError(s_msgs[m_conn.err]);
	else if (m_conn.lasterrcode != 0)
		hr = Runtime::setError(m_conn.lasterrstr);

	mongo_clear_errors(&m_conn);
	return hr;
}

result_t db_base::openMongoDB(const char* connString,
		obj_ptr<MongoDB_base>& retVal)
{
	if (qstrcmp(connString, "mongodb:", 8))
		return CALL_E_INVALIDARG;

	obj_ptr < MongoDB > db = new MongoDB();
	result_t hr = db->open(connString);
	if (hr < 0)
		return hr;

	retVal = db;

	return 0;
}

result_t MongoDB::open(const char* connString)
{
	obj_ptr < Url > u = new Url();
	int result;
	int nPort;

	result_t hr = u->parse(connString);
	if (hr < 0)
		return hr;

	if (u->m_host.find(',', 0) != std::string::npos)
	{
		const char* host = u->m_host.c_str();

		mongo_replset_init(&m_conn, "");

		while (true)
		{
			std::string hostname;
			std::string port;

			Url::parseHost(host, hostname, port);

			nPort = 27017;
			if (!port.empty())
				nPort = atoi(port.c_str());

			mongo_replset_add_seed(&m_conn, hostname.c_str(), nPort);

			if (*host != ',')
				break;

			host++;
		}

		result = mongo_replset_connect(&m_conn);
	}
	else
	{
		nPort = 27017;
		if (!u->m_port.empty())
			nPort = atoi(u->m_port.c_str());

		mongo_init(&m_conn);
		result = mongo_connect(&m_conn, u->m_hostname.c_str(), nPort);
	}

	if (result != MONGO_OK)
		return error();

	if (!u->m_pathname.empty())
		m_ns = u->m_pathname.substr(1);

	if (!u->m_username.empty())
		if (mongo_cmd_authenticate(&m_conn, m_ns.c_str(), u->m_username.c_str(),
				u->m_password.c_str()) != MONGO_OK)
			return error();

	return 0;
}

result_t MongoDB::getCollection(const char* name,
		obj_ptr<MongoCollection_base>& retVal)
{
	std::string nsStr;
	const char* ns = name;

	if (!m_ns.empty())
	{
		nsStr = m_ns;
		nsStr += '.';
		nsStr.append(name);
		ns = nsStr.c_str();
	}

	retVal = new MongoCollection(this, ns, name);

	return 0;
}

result_t MongoDB::run_command(bson *command, v8::Handle<v8::Object>& retVal)
{
	bson out;

	if (mongo_run_command(&m_conn, m_ns.c_str(), command, &out) != MONGO_OK)
	{
		bson_destroy(command);
		return error();
	}

	retVal = decodeObject(&out);
	bson_destroy(&out);
	bson_destroy(command);

	return 0;
}

result_t MongoDB::runCommand(v8::Handle<v8::Object> cmd,
		v8::Handle<v8::Object>& retVal)
{
	bson bbq;
	result_t hr;

	hr = encodeObject(&bbq, cmd);
	if (hr < 0)
		return hr;

	return run_command(&bbq, retVal);
}

result_t MongoDB::runCommand(const char* cmd, v8::Handle<v8::Value> arg,
		v8::Handle<v8::Object>& retVal)
{
	bson bbq;

	bson_init(&bbq);
	encodeValue(&bbq, cmd, arg);
	bson_finish(&bbq);

	return run_command(&bbq, retVal);
}

result_t MongoDB::_named_getter(const char* property,
		obj_ptr<MongoCollection_base>& retVal)
{
	return getCollection(property, retVal);
}

result_t MongoDB::get_fs(obj_ptr<GridFS_base>& retVal)
{
	retVal = new GridFS(this);
	return 0;
}

result_t MongoDB::oid(const char* hexStr, obj_ptr<MongoID_base>& retVal)
{
	retVal = new MongoID(hexStr);
	return 0;
}

result_t MongoDB::close()
{
	if(mongo_is_connected(&m_conn))
		mongo_destroy(&m_conn);

	return 0;
}

} /* namespace fibjs */
