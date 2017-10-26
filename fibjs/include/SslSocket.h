/*
 * Socket.h
 *
 *  Created on: Apr 6, 2014
 *      Author: lion
 */

#include "ssl.h"
#include "ifs/SslSocket.h"
#include "Buffer.h"
#include "Cipher.h"
#include "ssl.h"
#include <mbedtls/mbedtls/net.h>

#ifndef SSLSOCKET_H_
#define SSLSOCKET_H_

namespace fibjs {

class SslSocket : public SslSocket_base {
private:
    class asyncSsl : public AsyncState {
    public:
        asyncSsl(SslSocket* pThis, AsyncEvent* ac)
            : AsyncState(ac)
            , m_pThis(pThis)
            , m_ret(0)
        {
            set(process);
        }

    public:
        virtual int32_t process() = 0;
        virtual int32_t finally()
        {
            return 0;
        }

        static int32_t process(AsyncState* pState, int32_t n)
        {
            asyncSsl* pThis = (asyncSsl*)pState;

            if (pThis->m_ret == MBEDTLS_ERR_SSL_WANT_READ) {
                if (pThis->m_buf) {
                    pThis->m_pThis->m_recv_pos = 0;
                    pThis->m_buf->toString(pThis->m_pThis->m_recv);
                    pThis->m_buf.Release();
                } else
                    pThis->m_pThis->m_recv_pos = -1;
            }

            pThis->m_ret = pThis->process();
            if (pThis->m_ret == 0) {
                if (pThis->m_pThis->m_send.length() > 0) {
                    pThis->set(flush);
                    return 0;
                } else
                    return pThis->done(pThis->finally());
            }

            pThis->set(send);
            if (pThis->m_ret == MBEDTLS_ERR_SSL_WANT_READ || pThis->m_ret == MBEDTLS_ERR_SSL_WANT_WRITE)
                return 0;

            return CHECK_ERROR(_ssl::setError(pThis->m_ret));
        }

        static int32_t send(AsyncState* pState, int32_t n)
        {
            asyncSsl* pThis = (asyncSsl*)pState;

            exlib::string& m_send = pThis->m_pThis->m_send;

            pThis->set(recv);
            if (m_send.empty())
                return 0;

            pThis->m_buf = new Buffer(m_send);
            m_send.resize(0);

            return pThis->m_pThis->m_s->write(pThis->m_buf, pThis);
        }

        static int32_t recv(AsyncState* pState, int32_t n)
        {
            asyncSsl* pThis = (asyncSsl*)pState;

            pThis->m_buf.Release();

            pThis->set(process);
            if (pThis->m_ret == MBEDTLS_ERR_SSL_WANT_WRITE)
                return 0;

            return pThis->m_pThis->m_s->read(-1, pThis->m_buf, pThis);
        }

        static int32_t flush(AsyncState* pState, int32_t n)
        {
            asyncSsl* pThis = (asyncSsl*)pState;

            exlib::string& m_send = pThis->m_pThis->m_send;

            pThis->m_buf = new Buffer(m_send);
            m_send.resize(0);

            pThis->set(end);
            return pThis->m_pThis->m_s->write(pThis->m_buf, pThis);
        }

        static int32_t end(AsyncState* pState, int32_t n)
        {
            asyncSsl* pThis = (asyncSsl*)pState;
            return pThis->done(pThis->finally());
        }

    protected:
        obj_ptr<SslSocket> m_pThis;
        obj_ptr<Buffer_base> m_buf;
        int32_t m_ret;
    };

public:
    SslSocket();
    ~SslSocket();

public:
    virtual bool enterTask(exlib::Task_base* current)
    {
        if (!m_s)
            return true;

        return object_base::enterTask(current);
    }

    virtual void enter()
    {
        if (!m_s)
            return;

        object_base::enter();
    }

    virtual void leave(exlib::Task_base* current = NULL)
    {
        if (!m_s)
            return;

        if (m_lock.owned(current))
            m_lock.unlock(current);
    }

public:
    // Stream_base
    virtual result_t read(int32_t bytes, obj_ptr<Buffer_base>& retVal,
        AsyncEvent* ac);
    virtual result_t write(Buffer_base* data, AsyncEvent* ac);
    virtual result_t flush(AsyncEvent* ac);
    virtual result_t close(AsyncEvent* ac);
    virtual result_t copyTo(Stream_base* stm, int64_t bytes, int64_t& retVal, AsyncEvent* ac);

public:
    // SslSocket_base
    virtual result_t get_verification(int32_t& retVal);
    virtual result_t set_verification(int32_t newVal);
    virtual result_t get_ca(obj_ptr<X509Cert_base>& retVal);
    virtual result_t get_peerCert(obj_ptr<X509Cert_base>& retVal);
    virtual result_t get_stream(obj_ptr<Stream_base>& retVal);
    virtual result_t connect(Stream_base* s, exlib::string server_name, int32_t& retVal, AsyncEvent* ac);
    virtual result_t accept(Stream_base* s, obj_ptr<SslSocket_base>& retVal, AsyncEvent* ac);

private:
    int32_t my_recv(unsigned char* buf, size_t len);
    static int32_t my_recv(void* ctx, unsigned char* buf, size_t len);

    int32_t my_send(const unsigned char* buf, size_t len);
    static int32_t my_send(void* ctx, const unsigned char* buf, size_t len);

    result_t handshake(int32_t* retVal, AsyncEvent* ac);

public:
    result_t setCert(X509Cert_base* crt, PKey_base* key);

public:
    mbedtls_ssl_context m_ssl;
    mbedtls_ssl_config m_ssl_conf;

private:
    obj_ptr<X509Cert> m_ca;
    std::vector<obj_ptr<X509Cert_base>> m_crts;
    std::vector<obj_ptr<PKey_base>> m_keys;
    obj_ptr<Stream_base> m_s;
    exlib::string m_recv;
    int32_t m_recv_pos;
    exlib::string m_send;
};
}

#endif /* SSLSOCKET_H_ */
