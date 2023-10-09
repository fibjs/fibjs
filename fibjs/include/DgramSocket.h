/*
 * DgramSocket.h
 *
 *  Created on: Jan 1, 2018
 *      Author: lion
 */

#pragma once

#include "ifs/DgramSocket.h"
#include "AsyncUV.h"
#include "Buffer.h"

namespace fibjs {

class DgramSocket : public DgramSocket_base {
public:
    DgramSocket()
        : m_flags(0)
        , m_bound(false)
    {
    }

    static void on_delete(uv_handle_t* handle)
    {
        container_of(handle, DgramSocket, m_handle)->safe_release();
    }

    virtual void Delete()
    {
        if (uv_is_closing(&m_handle)) {
            delete this;
            return;
        }

        uv_post([&] {
            uv_close(&m_handle, on_delete);
            return 0;
        });
    }

    result_t create(int32_t family, int32_t flags);

public:
    EVENT_SUPPORT();

public:
    // DgramSocket_base
    virtual result_t bind(int32_t port, exlib::string addr, AsyncEvent* ac);
    virtual result_t bind(v8::Local<v8::Object> opts, AsyncEvent* ac);
    virtual result_t send(Buffer_base* msg, int32_t port, exlib::string address, int32_t& retVal, AsyncEvent* ac);
    virtual result_t send(Buffer_base* msg, int32_t offset, int32_t length, int32_t port, exlib::string address, int32_t& retVal, AsyncEvent* ac);
    virtual result_t address(obj_ptr<NObject>& retVal);
    virtual result_t close();
    virtual result_t close(v8::Local<v8::Function> callback);
    virtual result_t getRecvBufferSize(int32_t& retVal);
    virtual result_t getSendBufferSize(int32_t& retVal);
    virtual result_t addMembership(exlib::string multicastAddress, exlib::string multicastInterface);
    virtual result_t dropMembership(exlib::string multicastAddress, exlib::string multicastInterface);
    virtual result_t setMulticastTTL(int32_t ttl);
    virtual result_t setRecvBufferSize(int32_t size);
    virtual result_t setSendBufferSize(int32_t size);
    virtual result_t setBroadcast(bool flag);
    virtual result_t ref(obj_ptr<DgramSocket_base>& retVal);
    virtual result_t unref(obj_ptr<DgramSocket_base>& retVal);

public:
    void stop_bind();

private:
    static void on_alloc(uv_handle_t* handle, size_t suggested_size, uv_buf_t* buf);
    static void on_recv(uv_udp_t* handle, ssize_t nread, const uv_buf_t* buf, const struct sockaddr* addr, unsigned flags);

public:
    union {
        uv_handle_t m_handle;
        uv_udp_t m_udp;
    };
    int32_t m_family;
    int32_t m_flags;
    bool m_bound;
    int32_t m_recvbuf_size = -1;
    int32_t m_sendbuf_size = -1;

    exlib::string m_buf;

    obj_ptr<ValueHolder> m_holder;
};
}
