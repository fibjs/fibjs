/*
 * DgramSocket.cpp
 *
 *  Created on: Jan 1, 2018
 *      Author: lion
 */

#include "object.h"
#include "ifs/dgram.h"
#include "ifs/net.h"
#include "inetAddr.h"
#include "DgramSocket.h"
#include "Buffer.h"
#include "SimpleObject.h"
#include "EventInfo.h"
#include <fcntl.h>

namespace fibjs {

DECLARE_MODULE(dgram);

int32_t get_family(exlib::string type)
{
    if (type == "udp4")
        return net_base::C_AF_INET;
    else if (type == "udp6")
        return net_base::C_AF_INET6;
    return -1;
}

result_t dgram_base::createSocket(exlib::string type, obj_ptr<DgramSocket_base>& retVal)
{
    int32_t family = get_family(type);
    if (family < 0)
        return CHECK_ERROR(CALL_E_INVALIDARG);

    obj_ptr<DgramSocket> s = new DgramSocket();
    result_t hr = s->create(family, 0);
    if (hr < 0)
        return hr;

    retVal = s;
    return 0;
}

result_t dgram_base::createSocket(exlib::string type, v8::Local<v8::Function> callback,
    obj_ptr<DgramSocket_base>& retVal)
{
    result_t hr = createSocket(type, retVal);
    if (hr < 0)
        return hr;

    v8::Local<v8::Object> r;
    retVal->on("message", callback, r);

    return 0;
}

result_t dgram_base::createSocket(v8::Local<v8::Object> opts, obj_ptr<DgramSocket_base>& retVal)
{
    Isolate* isolate = Isolate::current();
    result_t hr;

    bool reuseAddr = false;
    hr = GetConfigValue(isolate->m_isolate, opts, "reuseAddr", reuseAddr);
    if (hr < 0 && hr != CALL_E_PARAMNOTOPTIONAL)
        return hr;

    bool ipv6Only = false;
    hr = GetConfigValue(isolate->m_isolate, opts, "ipv6Only", ipv6Only);
    if (hr < 0 && hr != CALL_E_PARAMNOTOPTIONAL)
        return hr;

    exlib::string type;
    hr = GetConfigValue(isolate->m_isolate, opts, "type", type);
    if (hr < 0)
        return hr;

    int32_t family = get_family(type);
    if (family < 0)
        return CHECK_ERROR(CALL_E_INVALIDARG);

    obj_ptr<DgramSocket> s = new DgramSocket();
    hr = s->create(family, (reuseAddr ? UV_UDP_REUSEADDR : 0) | (ipv6Only ? UV_UDP_IPV6ONLY : 0));
    if (hr < 0)
        return hr;

    hr = GetConfigValue(isolate->m_isolate, opts, "recvBufferSize", s->m_recvbuf_size);
    if (hr < 0 && hr != CALL_E_PARAMNOTOPTIONAL)
        return hr;

    hr = GetConfigValue(isolate->m_isolate, opts, "sendBufferSize", s->m_sendbuf_size);
    if (hr < 0 && hr != CALL_E_PARAMNOTOPTIONAL)
        return hr;

    retVal = s;

    return 0;
}

result_t dgram_base::createSocket(v8::Local<v8::Object> opts, v8::Local<v8::Function> callback,
    obj_ptr<DgramSocket_base>& retVal)
{
    result_t hr = createSocket(opts, retVal);
    if (hr < 0)
        return hr;

    v8::Local<v8::Object> r;
    retVal->on("message", callback, r);

    return 0;
}

result_t DgramSocket::create(int32_t family, int32_t flags)
{
    m_flags = flags;
    m_family = family;

    return uv_call([&] {
        return uv_udp_init_ex(s_uv_loop, &m_udp, 0);
    });
}

void DgramSocket::on_alloc(uv_handle_t* handle, size_t suggested_size, uv_buf_t* buf)
{
    DgramSocket* pThis = container_of(handle, DgramSocket, m_handle);

    pThis->m_buf.resize(suggested_size);
    *buf = uv_buf_init(pThis->m_buf.c_buffer(), (int32_t)pThis->m_buf.length());
}

void DgramSocket::on_recv(uv_udp_t* handle, ssize_t nread, const uv_buf_t* buf, const struct sockaddr* addr, unsigned flags)
{
    DgramSocket* pThis = container_of(handle, DgramSocket, m_udp);

    pThis->m_buf.resize(nread);
    if (addr) {
        Variant v[2];

        obj_ptr<Buffer> _buf = new Buffer(pThis->m_buf.c_str(), pThis->m_buf.length());
        v[0] = _buf;

        inetAddr& _addr = *(inetAddr*)addr;
        obj_ptr<NObject> msg = new NObject();
        msg->add("address", _addr.str());
        msg->add("family", _addr.family() == net_base::C_AF_INET6 ? "IPv6" : "IPv4");
        msg->add("port", _addr.port());
        msg->add("size", (int32_t)nread);
        v[1] = msg;

        pThis->_emit("message", v, 2);
    }
}

void DgramSocket::stop_bind()
{
    m_bound = false;
    uv_udp_recv_stop(&m_udp);
    m_holder.Release();
    isolate_unref();
}

result_t DgramSocket::bind(int32_t port, exlib::string addr, AsyncEvent* ac)
{
    if (m_bound)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    if (ac->isSync()) {
        m_holder = new ValueHolder(wrap());
        return CHECK_ERROR(CALL_E_NOSYNC);
    }

    inetAddr addr_info;

    addr_info.init(m_family);
    addr_info.setPort(port);

    if (addr_info.addr(addr) < 0) {
        m_holder.Release();
        return CHECK_ERROR(CALL_E_INVALIDARG);
    }

    int32_t ret = uv_udp_bind(&m_udp, (sockaddr*)&addr_info, m_flags & (UV_UDP_IPV6ONLY | UV_UDP_REUSEADDR));
    if (ret) {
        m_holder.Release();
        return CHECK_ERROR(ret);
    }

    if (m_recvbuf_size > 0)
        uv_recv_buffer_size(&m_handle, &m_recvbuf_size);

    if (m_sendbuf_size > 0)
        uv_send_buffer_size(&m_handle, &m_sendbuf_size);

    _emit("listening");

    m_bound = true;
    isolate_ref();

    return uv_call([&] {
        return uv_udp_recv_start(&m_udp, on_alloc, on_recv);
    });
}

result_t DgramSocket::bind(v8::Local<v8::Object> opts, AsyncEvent* ac)
{
    if (m_bound)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    if (ac->isSync()) {
        Isolate* isolate = holder();

        m_holder = new ValueHolder(wrap());

        result_t hr;

        int32_t port;
        hr = GetConfigValue(isolate->m_isolate, opts, "port", port);
        if (hr < 0)
            return hr;

        exlib::string addr;
        hr = GetConfigValue(isolate->m_isolate, opts, "address", addr);
        if (hr < 0)
            return hr;

        ac->m_ctx.resize(2);
        ac->m_ctx[0] = port;
        ac->m_ctx[1] = addr;
    }

    int32_t port = ac->m_ctx[0].intVal();
    exlib::string addr = ac->m_ctx[1].string();

    return bind(port, addr, ac);
}

result_t DgramSocket::send(Buffer_base* msg, int32_t port, exlib::string address,
    int32_t& retVal, AsyncEvent* ac)
{
    class AsyncSend : public uv_udp_send_t {
    public:
        AsyncSend(Buffer_base* msg, int32_t port, int32_t& retVal, AsyncEvent* ac)
            : m_ac(ac)
            , m_retVal(retVal)
            , m_port(port)
        {
            m_msg = Buffer::Cast(msg);
            m_buf = uv_buf_init((char*)m_msg->data(), (int32_t)m_msg->length());
        }

        static void callback(uv_udp_send_t* req, int status)
        {
            AsyncSend* pThis = (AsyncSend*)req;

            if (status < 0)
                pThis->m_ac->apost(status);
            else {
                pThis->m_retVal = status;
                pThis->m_ac->apost(0);
            }

            delete pThis;
        }

    public:
        AsyncEvent* m_ac;
        int32_t& m_retVal;
        obj_ptr<Buffer> m_msg;
        uv_buf_t m_buf;
        int32_t m_port;
    };

    result_t hr;
    if (!m_bound) {
        hr = bind(0, "", ac);
        if (hr < 0)
            return hr;
    }

    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    inetAddr addr_info;

    addr_info.init(m_family);
    addr_info.setPort(port);

    if (address.empty())
        address = m_family == net_base::C_AF_INET6 ? "::1" : "127.0.0.1";

    if (addr_info.addr(address.c_str()) < 0) {
        exlib::string strAddr;
        hr = net_base::cc_resolve(address, m_family, strAddr);
        if (hr < 0)
            return hr;

        if (addr_info.addr(strAddr.c_str()) < 0)
            return CHECK_ERROR(CALL_E_INVALIDARG);
    }

    AsyncSend* _send = new AsyncSend(msg, port, retVal, ac);
    int32_t status = uv_udp_try_send(&m_udp, &_send->m_buf, 1, (sockaddr*)&addr_info);
    if (status >= 0) {
        delete _send;
        retVal = status;
        return 0;
    } else if (status != UV_ENOSYS && status != UV_EAGAIN)
        return CHECK_ERROR(status);

    return uv_async([&] {
        return uv_udp_send(_send, &m_udp, &_send->m_buf, 1, (sockaddr*)&addr_info, AsyncSend::callback);
    });
}

result_t DgramSocket::send(Buffer_base* msg, int32_t offset, int32_t length, int32_t port,
    exlib::string address, int32_t& retVal, AsyncEvent* ac)
{
    if (offset < 0 || length <= 0)
        return CHECK_ERROR(CALL_E_INVALIDARG);

    result_t hr;
    if (!m_bound) {
        hr = bind(0, "", ac);
        if (hr < 0)
            return hr;
    }

    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    obj_ptr<Buffer_base> msg1;
    msg->slice(offset, offset + length, msg1);

    return send(msg1, port, address, retVal, ac);
}

result_t DgramSocket::address(obj_ptr<NObject>& retVal)
{
    inetAddr addr_info;
    int32_t sz = (int32_t)sizeof(addr_info);

    int32_t ret = uv_udp_getsockname(&m_udp, (sockaddr*)&addr_info, &sz);
    if (ret < 0)
        return CHECK_ERROR(ret);

    retVal = new NObject();

    retVal->add("family", addr_info.family() == net_base::C_AF_INET6 ? "IPv6" : "IPv4");
    retVal->add("address", addr_info.str());
    retVal->add("port", addr_info.port());

    return 0;
}

static void on_close(uv_handle_t* handle)
{
    DgramSocket* pThis = container_of(handle, DgramSocket, m_handle);
    pThis->_emit("close");
    if (pThis->m_bound)
        pThis->stop_bind();
}

result_t DgramSocket::close()
{
    if (uv_is_closing(&m_handle))
        return CALL_E_INVALID_CALL;

    return uv_call([&] {
        uv_close(&m_handle, on_close);
        return 0;
    });
}

result_t DgramSocket::close(v8::Local<v8::Function> callback)
{
    v8::Local<v8::Object> r;
    on("close", callback, r);
    return close();
}

result_t DgramSocket::getRecvBufferSize(int32_t& retVal)
{
    retVal = 0;
    return uv_recv_buffer_size(&m_handle, &retVal);
}

result_t DgramSocket::getSendBufferSize(int32_t& retVal)
{
    retVal = 0;
    return uv_send_buffer_size(&m_handle, &retVal);
}

result_t DgramSocket::setRecvBufferSize(int32_t size)
{
    return uv_recv_buffer_size(&m_handle, &size);
}

result_t DgramSocket::setSendBufferSize(int32_t size)
{
    return uv_send_buffer_size(&m_handle, &size);
}

result_t DgramSocket::setBroadcast(bool flag)
{
    return uv_udp_set_broadcast(&m_udp, flag ? 1 : 0);
}

result_t DgramSocket::ref(obj_ptr<DgramSocket_base>& retVal)
{
    isolate_ref();
    retVal = this;
    return 0;
}

result_t DgramSocket::unref(obj_ptr<DgramSocket_base>& retVal)
{
    isolate_unref();
    retVal = this;
    return 0;
}
}