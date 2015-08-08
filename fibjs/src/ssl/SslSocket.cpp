/*
 * SslSocket.cpp
 *
 *  Created on: Apr 6, 2014
 *      Author: lion
 */

#include "SslSocket.h"
#include "Stream.h"
#include "PKey.h"
#include <string.h>

namespace fibjs
{

result_t SslSocket_base::_new(X509Cert_base *crt, PKey_base *key,
                              obj_ptr<SslSocket_base> &retVal,
                              v8::Local<v8::Object> This)
{
    obj_ptr<SslSocket> ss = new SslSocket();
    result_t hr = ss->setCert(crt, key);
    if (hr < 0)
        return hr;

    retVal = ss;
    return 0;
}

result_t SslSocket_base::_new(v8::Local<v8::Array> certs,
                              obj_ptr<SslSocket_base> &retVal,
                              v8::Local<v8::Object> This)
{
    obj_ptr<SslSocket> ss = new SslSocket();

    int32_t sz = certs->Length();

    if (sz)
    {
        Isolate* isolate = Isolate::now();
        v8::Local<v8::Value> sCrt = v8::String::NewFromUtf8(isolate->m_isolate, "crt",
                                    v8::String::kNormalString, 3);
        v8::Local<v8::Value> sKey = v8::String::NewFromUtf8(isolate->m_isolate, "key",
                                    v8::String::kNormalString, 3);
        int32_t i;

        for (i = 0; i < sz; i ++)
        {
            v8::Local<v8::Value> v = certs->Get(i);

            if (v->IsObject())
            {
                v8::Local<v8::Object> o = v->ToObject();

                obj_ptr<X509Cert_base> crt = X509Cert_base::getInstance(o->Get(sCrt));
                if (!crt)
                    return CHECK_ERROR(CALL_E_INVALIDARG);

                obj_ptr<PKey_base> key = PKey_base::getInstance(o->Get(sKey));
                if (!key)
                    return CHECK_ERROR(CALL_E_INVALIDARG);


                result_t hr = ss->setCert(crt, key);
                if (hr < 0)
                    return hr;
            }
            else
                return CHECK_ERROR(CALL_E_INVALIDARG);

        }
    }

    retVal = ss;
    return 0;
}

SslSocket::SslSocket()
{
    ssl_init(&m_ssl);

    ssl_set_authmode(&m_ssl, g_ssl.m_authmode);
    ssl_set_rng(&m_ssl, ctr_drbg_random, &g_ssl.ctr_drbg);
    ssl_set_bio(&m_ssl, my_recv, this, my_send, this);

    m_recv_pos = 0;
}

SslSocket::~SslSocket()
{
    ssl_free(&m_ssl);
    memset(&m_ssl, 0, sizeof(m_ssl));
}

result_t SslSocket::setCert(X509Cert_base *crt, PKey_base *key)
{
    result_t hr;
    bool priv;

    hr = key->isPrivate(priv);
    if (hr < 0)
        return hr;

    if (!priv)
        return CHECK_ERROR(CALL_E_INVALIDARG);

    int32_t ret = ssl_set_own_cert(&m_ssl, &((X509Cert *)crt)->m_crt,
                                   &((PKey *)key)->m_key);
    if (ret != 0)
        return CHECK_ERROR(_ssl::setError(ret));

    m_crts.push_back(crt);
    m_keys.push_back(key);

    return 0;
}

int32_t SslSocket::my_recv(unsigned char *buf, size_t len)
{
    if (!len)
        return 0;

    if (m_recv_pos < 0)
        return POLARSSL_ERR_NET_CONN_RESET;

    if (m_recv_pos == (int32_t)m_recv.length())
    {
        m_recv_pos = 0;
        m_recv.resize(0);
        return POLARSSL_ERR_NET_WANT_READ;
    }

    if (len > m_recv.length() - m_recv_pos)
        len = m_recv.length() - m_recv_pos;

    memcpy(buf, m_recv.c_str() + m_recv_pos, len);
    m_recv_pos += (int32_t)len;

    return (int32_t)len;
}

int32_t SslSocket::my_recv(void *ctx, unsigned char *buf, size_t len)
{
    return ((SslSocket *)ctx)->my_recv(buf, len);
}

int32_t SslSocket::my_send(const unsigned char *buf, size_t len)
{
    if (!len)
        return 0;

    if (m_send.length() > 8192)
        return POLARSSL_ERR_NET_WANT_WRITE;

    m_send.append((const char *)buf, len);
    return (int32_t)len;
}

int32_t SslSocket::my_send(void *ctx, const unsigned char *buf, size_t len)
{
    return ((SslSocket *)ctx)->my_send(buf, len);
}

result_t SslSocket::read(int32_t bytes, obj_ptr<Buffer_base> &retVal,
                         AsyncEvent *ac)
{
    class asyncRead: public asyncSsl
    {
    public:
        asyncRead(SslSocket *pThis, int32_t bytes, obj_ptr<Buffer_base> &retVal,
                  AsyncEvent *ac) :
            asyncSsl(pThis, ac), m_bytes(bytes), m_retVal(retVal)
        {
            if (m_bytes == -1)
                m_bytes = 8192;

            m_buf.resize(m_bytes);
        }

    public:
        virtual int32_t process()
        {
            int32_t ret = ssl_read(&m_pThis->m_ssl, (unsigned char *)&m_buf[0], m_bytes);
            if (ret > 0)
            {
                m_buf.resize(ret);
                m_retVal = new Buffer(m_buf);
                return 0;
            }

            if (ret == POLARSSL_ERR_NET_WANT_READ ||
                    ret == POLARSSL_ERR_NET_WANT_WRITE)
                return ret;

            m_pThis->m_send.resize(0);
            return 0;
        }

        virtual int32_t finally()
        {
            return m_retVal ? 0 : CALL_RETURN_NULL;
        }

    private:
        int32_t m_bytes;
        std::string m_buf;
        obj_ptr<Buffer_base> &m_retVal;
    };

    if (!m_s)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    if (!ac)
        return CHECK_ERROR(CALL_E_NOSYNC);

    return (new asyncRead(this, bytes, retVal, ac))->post(0);
}

result_t SslSocket::write(Buffer_base *data, AsyncEvent *ac)
{
    class asyncWrite: public asyncSsl
    {
    public:
        asyncWrite(SslSocket *pThis, Buffer_base *data, AsyncEvent *ac) :
            asyncSsl(pThis, ac)
        {
            data->toString(m_buf);
            m_pos = 0;
        }

    public:
        virtual int32_t process()
        {
            int32_t ret;

            while ((ret = ssl_write(&m_pThis->m_ssl, (const unsigned char *)m_buf.c_str() + m_pos,
                                    m_buf.length() - m_pos)) > 0)
            {
                m_pos += ret;
                if (m_pos == (int32_t)m_buf.length())
                    return 0;
            }

            return ret;
        }

    private:
        std::string m_buf;
        int32_t m_pos;
    };

    if (!m_s)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    if (!ac)
        return CHECK_ERROR(CALL_E_NOSYNC);

    return (new asyncWrite(this, data, ac))->post(0);
}

result_t SslSocket::close(AsyncEvent *ac)
{
    class asyncClose: public asyncSsl
    {
    public:
        asyncClose(SslSocket *pThis, AsyncEvent *ac) :
            asyncSsl(pThis, ac)
        {
        }

        ~asyncClose()
        {
            m_pThis->m_s.Release();
        }

    public:
        virtual int32_t process()
        {
            return ssl_close_notify(&m_pThis->m_ssl);
        }
    };

    if (!m_s)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    if (!ac)
        return CHECK_ERROR(CALL_E_NOSYNC);

    return (new asyncClose(this, ac))->post(0);
}

result_t SslSocket::copyTo(Stream_base *stm, int64_t bytes,
                           int64_t &retVal, AsyncEvent *ac)
{
    return copyStream(this, stm, bytes, retVal, ac);
}

result_t SslSocket::get_verification(int32_t &retVal)
{
    retVal = m_ssl.authmode;
    return 0;
}

result_t SslSocket::set_verification(int32_t newVal)
{
    if (newVal < ssl_base::_VERIFY_NONE || newVal > ssl_base::_VERIFY_REQUIRED)
        return CHECK_ERROR(CALL_E_INVALIDARG);

    ssl_set_authmode(&m_ssl, newVal);
    return 0;
}

result_t SslSocket::get_ca(obj_ptr<X509Cert_base> &retVal)
{
    if (!m_ca)
        m_ca = new X509Cert();

    retVal = m_ca;
    return 0;
}

result_t SslSocket::get_peerCert(obj_ptr<X509Cert_base> &retVal)
{
    const x509_crt *crt = ssl_get_peer_cert(&m_ssl);

    if (!crt)
        return CALL_RETURN_NULL;

    obj_ptr<X509Cert> crtObject = new X509Cert();

    result_t hr = crtObject->load(crt);
    if (hr < 0)
        return hr;

    retVal = crtObject;

    return 0;
}

result_t SslSocket::handshake(int32_t *retVal, AsyncEvent *ac)
{
    class asyncHandshake: public asyncSsl
    {
    public:
        asyncHandshake(SslSocket *pThis, int32_t *retVal, AsyncEvent *ac) :
            asyncSsl(pThis, ac), m_retVal(retVal)
        {
        }

    public:
        virtual int32_t process()
        {
            return ssl_handshake(&m_pThis->m_ssl);
        }

        virtual int32_t finally()
        {
            if (m_retVal)
                *m_retVal = ssl_get_verify_result(&m_pThis->m_ssl);

            return 0;
        }

    private:
        int32_t *m_retVal;
    };

    return (new asyncHandshake(this, retVal, ac))->post(0);
}

result_t SslSocket::connect(Stream_base *s, const char *server_name,
                            int32_t &retVal, AsyncEvent *ac)
{
    if (m_s)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    if (!ac)
        return CHECK_ERROR(CALL_E_NOSYNC);

    m_s = s;

    ssl_set_endpoint(&m_ssl, SSL_IS_CLIENT);

    if (!m_ca)
        m_ca = g_ssl.ca();

    ssl_set_ca_chain(&m_ssl, &m_ca->m_crt, NULL, NULL);

    if (server_name && *server_name)
        ssl_set_hostname(&m_ssl, server_name);

    return handshake(&retVal, ac);
}

result_t SslSocket::accept(Stream_base *s, obj_ptr<SslSocket_base> &retVal,
                           AsyncEvent *ac)
{
    if (m_s)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    if (!ac)
        return CHECK_ERROR(CALL_E_NOSYNC);

    obj_ptr<SslSocket> ss = new SslSocket();
    retVal = ss;
    int32_t sz = (int32_t)m_crts.size();
    int32_t i;
    result_t hr;

    for (i = 0; i < sz; i ++)
    {
        hr = ss->setCert(m_crts[i], m_keys[i]);
        if (hr < 0)
            return hr;
    }

    ss->m_s = s;

    ssl_set_authmode(&ss->m_ssl, m_ssl.authmode);
    ssl_set_endpoint(&ss->m_ssl, SSL_IS_SERVER);

    if (m_ca)
    {
        ss->m_ca = m_ca;
        ssl_set_ca_chain(&ss->m_ssl, &m_ca->m_crt, NULL, NULL);
    }

    ssl_set_session_cache(&ss->m_ssl, ssl_cache_get, &g_ssl.m_cache,
                          ssl_cache_set, &g_ssl.m_cache);

    return ss->handshake(NULL, ac);
}

}
