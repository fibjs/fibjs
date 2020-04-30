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
#include "PKey.h"
#include "Routing.h"

#ifndef SSLSOCKET_H_
#define SSLSOCKET_H_

namespace fibjs {

class SslSocket : public SslSocket_base {
    FIBER_FREE();

private:
    class asyncSsl : public AsyncState {
    public:
        asyncSsl(SslSocket* pThis, AsyncEvent* ac)
            : AsyncState(ac)
            , m_pThis(pThis)
            , m_s(pThis->m_s)
            , m_ret(0)
        {
            next(process);
        }

    public:
        virtual int32_t process() = 0;
        virtual int32_t finally()
        {
            return next();
        }

        ON_STATE(asyncSsl, process)
        {
            if (m_ret == MBEDTLS_ERR_SSL_WANT_READ) {
                if (m_buf) {
                    m_pThis->m_recv_pos = 0;
                    m_buf->toString(m_pThis->m_recv);
                    m_buf.Release();
                } else
                    m_pThis->m_recv_pos = -1;
            }

            m_ret = process();
            if (m_ret == 0) {
                if (m_pThis->m_send.length() > 0)
                    return next(flush);
                else
                    return finally();
            }

            if (m_ret == MBEDTLS_ERR_SSL_WANT_READ || m_ret == MBEDTLS_ERR_SSL_WANT_WRITE)
                return next(send);

            return CHECK_ERROR(_ssl::setError(m_ret));
        }

        ON_STATE(asyncSsl, send)
        {
            exlib::string& m_send = m_pThis->m_send;

            if (m_send.empty())
                return next(recv);

            m_buf = new Buffer(m_send);
            m_send.resize(0);

            return m_pThis->m_s->write(m_buf, next(recv));
        }

        ON_STATE(asyncSsl, recv)
        {
            m_buf.Release();

            if (m_ret == MBEDTLS_ERR_SSL_WANT_WRITE)
                return next(process);

            return m_pThis->m_s->read(-1, m_buf, next(process));
        }

        ON_STATE(asyncSsl, flush)
        {
            exlib::string& m_send = m_pThis->m_send;

            m_buf = new Buffer(m_send);
            m_send.resize(0);

            return m_pThis->m_s->write(m_buf, next(end));
        }

        ON_STATE(asyncSsl, end)
        {
            return finally();
        }

    protected:
        obj_ptr<SslSocket> m_pThis;
        obj_ptr<Buffer_base> m_buf;
        obj_ptr<Stream_base> m_s;
        int32_t m_ret;
    };

public:
    SslSocket();
    ~SslSocket();

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
    virtual result_t get_hostname(exlib::string& retVal);
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
    static int sni_callback(void* p_info, mbedtls_ssl_context* ssl,
        const unsigned char* name, size_t name_len);

    class Cert : public object_base {
    public:
        Cert(exlib::string name, X509Cert_base* crt, PKey_base* key);
        ~Cert()
        {
            if (m_re)
                pcre_free(m_re);
        }

    public:
        exlib::string m_name;
        pcre* m_re;
        obj_ptr<X509Cert> m_crt;
        obj_ptr<PKey> m_key;
    };

    result_t setCert(Cert* crt);
    result_t setCert(exlib::string name, X509Cert_base* crt, PKey_base* key)
    {
        return setCert(new Cert(name, crt, key));
    }

public:
    mbedtls_ssl_context m_ssl;
    mbedtls_ssl_config m_ssl_conf;
    std::vector<obj_ptr<Cert>> m_crts;

private:
    obj_ptr<X509Cert> m_ca;
    obj_ptr<Stream_base> m_s;
    exlib::string m_recv;
    int32_t m_recv_pos;
    exlib::string m_send;
};
}

#endif /* SSLSOCKET_H_ */
