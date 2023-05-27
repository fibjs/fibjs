/*
 * Socket_api.cpp
 *
 *  Created on: Oct 24, 2012
 *      Author: lion
 */

#include "object.h"
#include "Socket_api.h"
#include "ifs/net.h"
#include "Buffer.h"

namespace fibjs {
namespace socket {

    void* create()
    {
        obj_ptr<Socket_base> s;
        Socket_base::_new(net_base::C_AF_INET, s);
        s->Ref();

        return s;
    }

    void destroy(void* sock)
    {
        if (sock)
            ((Socket_base*)sock)->Unref();
    }

    int32_t c_connect(void* sock, const char* host, int32_t port)
    {
        if (!sock) {
            Runtime::setError(CALL_E_INVALID_CALL);
            return 0;
        }

        assert(!Runtime::check());

        result_t hr = ((Socket_base*)sock)->cc_connect(host, port);
        if (hr < 0) {
            Runtime::setError(hr);
            return 0;
        }

        return 1;
    }

    void* c_connect(const char* host, int32_t port)
    {
        void* socket;

        socket = create();
        if (!c_connect(socket, host, port)) {
            destroy(socket);
            return NULL;
        }

        return socket;
    }

    int32_t c_recv(void* sock, void* buffer, int32_t cbBuffer)
    {
        if (!sock) {
            Runtime::setError(CALL_E_INVALID_CALL);
            return -1;
        }

        assert(!Runtime::check());

        obj_ptr<Buffer_base> retVal;

        result_t hr = ((Socket_base*)sock)->cc_recv(cbBuffer, retVal);
        if (hr < 0) {
            Runtime::setError(hr);
            return -1;
        }

        if (hr == CALL_RETURN_NULL)
            return 0;

        Buffer* buf = Buffer::Cast(retVal);
        memcpy(buffer, buf->data(), buf->length());

        return (int32_t)buf->length();
    }

    int32_t c_read(void* sock, void* buffer, int32_t cbBuffer)
    {
        if (!sock) {
            Runtime::setError(CALL_E_INVALID_CALL);
            return -1;
        }

        if (cbBuffer <= 0)
            return 0;

        assert(!Runtime::check());

        obj_ptr<Buffer_base> retVal;

        result_t hr = ((Socket_base*)sock)->cc_read(cbBuffer, retVal);
        if (hr < 0) {
            Runtime::setError(hr);
            return -1;
        }

        if (hr == CALL_RETURN_NULL)
            return 0;

        Buffer* buf = Buffer::Cast(retVal);
        if ((int32_t)buf->length() < cbBuffer)
            return -1;

        memcpy(buffer, buf->data(), buf->length());

        return (int32_t)buf->length();
    }

    int32_t c_send(void* sock, const void* buffer, int32_t cbBuffer)
    {
        if (!sock) {
            Runtime::setError(CALL_E_INVALID_CALL);
            return -1;
        }

        if (cbBuffer <= 0)
            return 0;

        assert(!Runtime::check());

        obj_ptr<Buffer_base> buf = new Buffer((const char*)buffer, cbBuffer);

        result_t hr = ((Socket_base*)sock)->cc_send(buf);
        if (hr < 0) {
            Runtime::setError(hr);
            return -1;
        }

        return cbBuffer;
    }
}
}
