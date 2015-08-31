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
    printf("%s\n", "~~~~~~SslSocket::Construct~~~~~~");
    mbedtls_ssl_init(&m_ssl);
    mbedtls_ssl_config_init( &m_ssl_conf );

    m_recv_pos = 0;
}

SslSocket::~SslSocket()
{
    mbedtls_ssl_free(&m_ssl);
    mbedtls_ssl_config_free( &m_ssl_conf );
    memset(&m_ssl, 0, sizeof(m_ssl));
}

result_t SslSocket::setCert(X509Cert_base *crt, PKey_base *key)
{
    printf("%s\n", "SslSocket::setCert");
    result_t hr;
    bool priv;

    hr = key->isPrivate(priv);
    if (hr < 0)
        return hr;

    if (!priv)
        return CHECK_ERROR(CALL_E_INVALIDARG);

    // int32_t ret = mbedtls_ssl_conf_own_cert(&m_ssl_conf, &((X509Cert *)crt)->m_crt,
    //                                         &((PKey *)key)->m_key);
    // if (ret != 0)
    //     return CHECK_ERROR(_ssl::setError(ret));

    m_crts.push_back(crt);
    m_keys.push_back(key);

    return 0;
}

int32_t SslSocket::my_recv(unsigned char *buf, size_t len)
{
    printf("SslSocket::my_recv begin\n");
    if (!len)
        return 0;

    if (m_recv_pos < 0) {
        printf("SslSocket::my_recv pos<0,%d\n", m_recv_pos);
        return MBEDTLS_ERR_NET_CONN_RESET;
    }


    if (m_recv_pos == (int32_t)m_recv.length() )
    {
        printf("SslSocket::my_recv length:%d,%d\n", m_recv_pos, (int32_t)m_recv.length());
        m_recv_pos = 0;
        m_recv.resize(0);
        return MBEDTLS_ERR_SSL_WANT_READ;
    }

    if (len > m_recv.length() - m_recv_pos)
        len = m_recv.length() - m_recv_pos;

    memcpy(buf, m_recv.c_str() + m_recv_pos, len);
    m_recv_pos += (int32_t)len;
    printf("SslSocket::my_recv length:%zu\n", len);
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
    printf("SslSocket::my_send\n");
    if (m_send.length() > 8192)
        return MBEDTLS_ERR_SSL_WANT_WRITE;

    m_send.append((const char *)buf, len);
    return (int32_t)len;
}

int32_t SslSocket::my_send(void *ctx, const unsigned char *buf, size_t len)
{
    return ((SslSocket *)ctx)->my_send(buf, len);
}

int32_t my_timeout(void *ctx, unsigned char *buf, size_t len, uint32_t l)
{
    return 0;
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
            int32_t ret = mbedtls_ssl_read(&m_pThis->m_ssl, (unsigned char *)&m_buf[0], m_bytes);
            if (ret > 0)
            {
                m_buf.resize(ret);
                m_retVal = new Buffer(m_buf);
                return 0;
            }

            if (ret == MBEDTLS_ERR_SSL_WANT_READ ||
                    ret == MBEDTLS_ERR_SSL_WANT_WRITE)
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

            while ((ret = mbedtls_ssl_write(&m_pThis->m_ssl,
                                            (const unsigned char *)m_buf.c_str() + m_pos,
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
            return mbedtls_ssl_close_notify(&m_pThis->m_ssl);
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
    retVal = m_ssl_conf.authmode;
    return 0;
}

result_t SslSocket::set_verification(int32_t newVal)
{
    printf("%s\n", "SslSocket::set_verification");
    if (newVal < ssl_base::_VERIFY_NONE || newVal > ssl_base::_VERIFY_REQUIRED)
        return CHECK_ERROR(CALL_E_INVALIDARG);
    verification = newVal;
    // mbedtls_ssl_conf_authmode(&m_ssl_conf, newVal);
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
    const mbedtls_x509_crt *crt = mbedtls_ssl_get_peer_cert(&m_ssl);

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
    printf("%s\n", "~~~~~~SslSocket::handshake~~~~~~");

    // if ( mbedtls_ssl_setup(&m_ssl, &m_ssl_conf) != 0 )
    // {
    //     printf( " failed\n  ! mbedtls_ssl_setup returned \n" );
    //     return 0;
    // }

    // mbedtls_ssl_set_bio(&m_ssl, this, my_send, my_recv, NULL);
    printf("SslSocket::handshake m_recv_pos:%d\n", m_recv_pos );
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
            result_t res = mbedtls_ssl_handshake(&m_pThis->m_ssl);
            printf("%s:%d\n", "SslSocket::handshake res", res);
            return res;
        }

        virtual int32_t finally()
        {
            if (m_retVal)
                *m_retVal = mbedtls_ssl_get_verify_result(&m_pThis->m_ssl);

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

    printf("%s\n", "SslSocket::connect");
    mbedtls_ssl_config_defaults( &m_ssl_conf,
                                 MBEDTLS_SSL_IS_CLIENT,
                                 MBEDTLS_SSL_TRANSPORT_STREAM,
                                 MBEDTLS_SSL_PRESET_DEFAULT );
    mbedtls_ssl_conf_authmode( &m_ssl_conf, MBEDTLS_SSL_VERIFY_OPTIONAL );
    // if (verification)
    //     mbedtls_ssl_conf_authmode(&m_ssl_conf, verification);
    // else
    //     mbedtls_ssl_conf_authmode(&m_ssl_conf, g_ssl.m_authmode);

    mbedtls_ssl_conf_rng(&m_ssl_conf, mbedtls_ctr_drbg_random, &g_ssl.ctr_drbg);

    // mbedtls_ssl_conf_endpoint(&m_ssl_conf, MBEDTLS_SSL_IS_CLIENT);

    if (!m_ca)
        m_ca = g_ssl.ca();

    mbedtls_ssl_conf_ca_chain(&m_ssl_conf, &m_ca->m_crt, NULL);

    if ( mbedtls_ssl_setup(&m_ssl, &m_ssl_conf) != 0 )
    {
        printf( " failed\n  ! mbedtls_ssl_setup returned \n" );
        return 0;
    }
    mbedtls_ssl_set_bio(&m_ssl, this, my_send, my_recv, NULL);

    if (server_name && *server_name)
        mbedtls_ssl_set_hostname(&m_ssl, server_name);

    return handshake(&retVal, ac);
}

result_t SslSocket::accept(Stream_base *s, obj_ptr<SslSocket_base> &retVal,
                           AsyncEvent *ac)
{
    if (m_s)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    if (!ac)
        return CHECK_ERROR(CALL_E_NOSYNC);

    printf("%s\n", "SslSocket::accept");

    // mbedtls_ssl_config_defaults( &m_ssl_conf,
    //                              MBEDTLS_SSL_IS_CLIENT,
    //                              MBEDTLS_SSL_TRANSPORT_DATAGRAM,
    //                              MBEDTLS_SSL_PRESET_DEFAULT );

    // mbedtls_ssl_conf_rng(&m_ssl_conf, mbedtls_ctr_drbg_random, &g_ssl.ctr_drbg);
    // if ( mbedtls_ssl_setup(&m_ssl, &m_ssl_conf) != 0 )
    // {
    //     printf( " failed\n  ! mbedtls_ssl_setup returned \n" );
    //     return 0;
    // }
    // mbedtls_ssl_set_bio(&m_ssl, this, my_send, my_recv, NULL);
    // mbedtls_ssl_conf_min_version(&m_ssl_conf, MBEDTLS_SSL_MINOR_VERSION_3, g_ssl.m_min_version);
    // mbedtls_ssl_conf_max_version(&m_ssl_conf, MBEDTLS_SSL_MINOR_VERSION_3, g_ssl.m_max_version);


    obj_ptr<SslSocket> ss = new SslSocket();
    mbedtls_ssl_config_defaults( &ss->m_ssl_conf,
                                 MBEDTLS_SSL_IS_SERVER,
                                 MBEDTLS_SSL_TRANSPORT_STREAM,
                                 MBEDTLS_SSL_PRESET_DEFAULT );

    mbedtls_ssl_conf_rng(&ss->m_ssl_conf, mbedtls_ctr_drbg_random, &g_ssl.ctr_drbg);
    retVal = ss;
    int32_t sz = (int32_t)m_crts.size();
    int32_t i;
    result_t hr;

    if (m_ca)
    {
        ss->m_ca = m_ca;
        mbedtls_ssl_conf_ca_chain(&ss->m_ssl_conf, &m_ca->m_crt, NULL);
    }

    for (i = 0; i < sz; i ++)
    {
        hr = ss->setCert(m_crts[i], m_keys[i]);
        if (hr != 0)
            return CHECK_ERROR(_ssl::setError(hr));
        hr = mbedtls_ssl_conf_own_cert( &ss->m_ssl_conf,
                                        &((X509Cert *)(X509Cert_base *)m_crts[i])->m_crt,
                                        &((PKey *)(PKey_base *)m_keys[i])->m_key);
        if (hr < 0)
            return hr;
    }

    ss->m_s = s;

    // mbedtls_ssl_conf_authmode(&ss->m_ssl_conf, m_ssl_conf.authmode);
    // mbedtls_ssl_conf_endpoint(&ss->m_ssl_conf, MBEDTLS_SSL_IS_SERVER);



    // mbedtls_ssl_cookie_init( &ss->cookie_ctx );
    // mbedtls_ssl_cookie_setup( &ss->cookie_ctx,
    //                           mbedtls_ctr_drbg_random, &g_ssl.ctr_drbg );
    // mbedtls_ssl_conf_dtls_cookies( &ss->m_ssl_conf, mbedtls_ssl_cookie_write, mbedtls_ssl_cookie_check,
    //                                &ss->cookie_ctx );

    mbedtls_ssl_conf_session_cache(&ss->m_ssl_conf, &g_ssl.m_cache,
                                   mbedtls_ssl_cache_get, mbedtls_ssl_cache_set);
    if ( mbedtls_ssl_setup(&ss->m_ssl, &ss->m_ssl_conf) != 0 )
    {
        printf( " failed\n  ! mbedtls_ssl_setup returned \n" );
        return 0;
    }
    mbedtls_ssl_set_timer_cb( &ss->m_ssl, &timer, mbedtls_timing_set_delay,
                              mbedtls_timing_get_delay );
    mbedtls_ssl_set_bio(&ss->m_ssl, this, my_send, my_recv, NULL);

    return ss->handshake(NULL, ac);
}

}
