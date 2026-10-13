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

    // The per-thread failure slot of the C ABI (replaces the retired Runtime
    // thread state): a callback reports the failure code and the driver reads
    // it back on the same thread. A non-consuming read mirrors the old m_code
    // semantics (the code stays until the next failure overwrites it).
    static thread_local result_t s_last_socket_error;

    void set_last_error(result_t hr)
    {
        s_last_socket_error = hr;
    }

    result_t last_error()
    {
        return s_last_socket_error;
    }

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
            set_last_error(result_t::invalid_call());
            return 0;
        }

        ex_assert(!Runtime::check());

        obj_ptr<Stream_base> retVal;
        result_t hr = ((Socket_base*)sock)->cc_connect(port, host, 0, retVal);
        if (hr.failed()) {
            set_last_error(hr);
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
            set_last_error(result_t::invalid_call());
            return -1;
        }

        ex_assert(!Runtime::check());

        obj_ptr<Buffer_base> retVal;

        result_t hr = ((Socket_base*)sock)->cc_recv(cbBuffer, retVal);
        if (hr.failed()) {
            set_last_error(hr);
            return -1;
        }

        if (hr.is_return_null())
            return 0;

        Buffer* buf = Buffer::Cast(retVal);
        memcpy(buffer, buf->data(), buf->length());

        return (int32_t)buf->length();
    }

    int32_t c_read(void* sock, void* buffer, int32_t cbBuffer)
    {
        if (!sock) {
            set_last_error(result_t::invalid_call());
            return -1;
        }

        if (cbBuffer <= 0)
            return 0;

        ex_assert(!Runtime::check());

        obj_ptr<Buffer_base> retVal;

        result_t hr = ((Socket_base*)sock)->cc_readBuffer(cbBuffer, retVal);
        if (hr.failed()) {
            set_last_error(hr);
            return -1;
        }

        if (hr.is_return_null())
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
            set_last_error(result_t::invalid_call());
            return -1;
        }

        if (cbBuffer <= 0)
            return 0;

        ex_assert(!Runtime::check());

        obj_ptr<Buffer_base> buf = new Buffer((const char*)buffer, cbBuffer);

        int32_t len;
        result_t hr = ((Socket_base*)sock)->cc_send(buf, len);
        if (hr.failed()) {
            set_last_error(hr);
            return -1;
        }

        return cbBuffer;
    }
}
}
