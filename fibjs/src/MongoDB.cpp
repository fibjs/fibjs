/*
 * MongoDB.cpp
 *
 *  Created on: Oct 25, 2012
 *      Author: lion
 */

#include "ifs/db.h"
#include "MongoDB.h"
#include "MongoCollection.h"
#include "Socket_api.h"
#include <mongo/env.h>

int mongo_env_set_socket_op_timeout(mongo *conn, int millis)
{
	return MONGO_OK;
}

int mongo_env_read_socket(mongo *conn, void *buf, int len)
{
	return fibjs::socket::read(conn->sock, buf, len);
}

int mongo_env_write_socket(mongo *conn, const void *buf, int len)
{
	return fibjs::socket::send(conn->sock, buf, len);
}

int mongo_env_socket_connect(mongo *conn, const char *host, int port)
{
	conn->sock = fibjs::socket::connect(host, port);
	return conn->sock ? 0 : -1;
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

namespace fibjs
{

result_t db_base::openMongoDB(const char* connString,
		obj_ptr<MongoDB_base>& retVal)
{
	retVal = new MongoDB();
	return 0;
}

result_t MongoDB::getCollection(const char* name,
		obj_ptr<MongoCollection_base>& retVal)
{
	retVal = new MongoCollection();

	return 0;
}

} /* namespace fibjs */
