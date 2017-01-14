/*
 * Socket_api.cpp
 *
 *  Created on: Oct 24, 2012
 *      Author: lion
 */

#include "object.h"
#include "Socket_api.h"
#include "Socket.h"
#include "Buffer.h"

namespace fibjs
{
namespace socket
{

void *create()
{
    Socket *s = new Socket();
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
    if (sock)
        ((Socket *) sock)->Unref();
}

int32_t c_connect(void *sock, const char *host, int32_t port)
{
    if (!sock)
    {
        Runtime::setError(CALL_E_INVALID_CALL);
        return 0;
    }

    assert(!Isolate::check());

    result_t hr = ((Socket *) sock)->cc_connect(host, port);
    if (hr < 0)
    {
        Runtime::setError(hr);
        return 0;
    }

    return 1;
}

void *c_connect(const char *host, int32_t port)
{
    void *socket;

    socket = create();
    if (!c_connect(socket, host, port))
    {
        destroy(socket);
        return NULL;
    }

    return socket;
}

int32_t c_recv(void *sock, void *buffer, int32_t cbBuffer)
{
    if (!sock)
    {
        Runtime::setError(CALL_E_INVALID_CALL);
        return -1;
    }

    assert(!Isolate::check());

    obj_ptr<Buffer_base> retVal;

    result_t hr = ((Socket *) sock)->cc_recv(cbBuffer, retVal);
    if (hr < 0)
    {
        Runtime::setError(hr);
        return -1;
    }

    exlib::string strBuf;

    if (hr != CALL_RETURN_NULL)
    {
        retVal->toString(strBuf);
        memcpy(buffer, strBuf.c_str(), strBuf.length());
    }

    return (int32_t) strBuf.length();
}

int32_t c_read(void *sock, void *buffer, int32_t cbBuffer)
{
    if (!sock)
    {
        Runtime::setError(CALL_E_INVALID_CALL);
        return -1;
    }

    if (cbBuffer <= 0)
        return 0;

    assert(!Isolate::check());

    obj_ptr<Buffer_base> retVal;

    result_t hr = ((Socket *) sock)->cc_read(cbBuffer, retVal);
    if (hr < 0)
    {
        Runtime::setError(hr);
        return -1;
    }

    exlib::string strBuf;

    if (hr != CALL_RETURN_NULL)
    {
        retVal->toString(strBuf);
        if ((int32_t) strBuf.length() < cbBuffer)
            return -1;
        memcpy(buffer, strBuf.c_str(), strBuf.length());
    }

    return (int32_t) strBuf.length();
}

int32_t c_send(void *sock, const void *buffer, int32_t cbBuffer)
{
    if (!sock)
    {
        Runtime::setError(CALL_E_INVALID_CALL);
        return -1;
    }

    if (cbBuffer <= 0)
        return 0;

    assert(!Isolate::check());

    exlib::string strBuf((const char *) buffer, cbBuffer);
    obj_ptr<Buffer_base> buf;

    buf = new Buffer(strBuf);

    result_t hr = ((Socket *) sock)->cc_send(buf);
    if (hr < 0)
    {
        Runtime::setError(hr);
        return -1;
    }

    return (int32_t) strBuf.length();
}

}

}
