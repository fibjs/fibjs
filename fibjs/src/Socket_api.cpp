/*
 * Socket_api.cpp
 *
 *  Created on: Oct 24, 2012
 *      Author: lion
 */

#include "Socket_api.h"
#include "Socket.h"
#include "Buffer.h"

namespace fibjs
{
namespace socket
{

void *create()
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

void destroy(void *sock)
{
	((Socket*) sock)->Unref();
}

void close(void *sock)
{
	((Socket*) sock)->ac_close();
}

int connect(void *sock, const char *host, int port)
{
	result_t hr = ((Socket*) sock)->ac_connect(host, port);
	if (hr < 0)
	{
		Runtime::setError(hr);
		return 0;
	}

	return 1;
}

int recv(void *sock, char *buffer, int cbBuffer)
{
	obj_ptr<Buffer_base> retVal;

	result_t hr = ((Socket*) sock)->ac_recv(cbBuffer, retVal);
	if (hr < 0)
	{
		Runtime::setError(hr);
		return -1;
	}

	std::string strBuf;

	if (hr != CALL_RETURN_NULL)
	{
		retVal->toString(strBuf);
		memcpy(buffer, strBuf.c_str(), strBuf.length());
	}

	return (int) strBuf.length();
}

int send(void *sock, const char *buffer, int cbBuffer)
{
	std::string strBuf(buffer, cbBuffer);
	obj_ptr<Buffer_base> buf;

	buf = new Buffer(strBuf);

	result_t hr = ((Socket*) sock)->ac_send(buf);
	if (hr < 0)
	{
		Runtime::setError(hr);
		return -1;
	}

	return (int) strBuf.length();
}

}

}
