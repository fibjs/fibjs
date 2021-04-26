/*
 * SslSocket.cpp
 *
 *  Created on: Apr 6, 2014
 *      Author: lion
 */

#include "object.h"
#include "ifs/io.h"
#include "ifs/console.h"
#include "SslSocket.h"
#include "PKey.h"
#include <string.h>
#include "options.h"

namespace fibjs {

result_t SslSocket_base::_new(X509Cert_base* crt, PKey_base* key,
    obj_ptr<SslSocket_base>& retVal,
    v8::Local<v8::Object> This)
{
    obj_ptr<SslSocket> ss = new SslSocket();
    result_t hr = ss->setCert("", crt, key);
    if (hr < 0)
        return hr;

    retVal = ss;
    return 0;
}

result_t SslSocket_base::_new(v8::Local<v8::Array> certs,
    obj_ptr<SslSocket_base>& retVal,
    v8::Local<v8::Object> This)
{
    result_t hr;
    obj_ptr<SslSocket> ss = new SslSocket();

    Isolate* isolate = ss->holder();
    int32_t i;
    int32_t sz = certs->Length();

    for (i = 0; i < sz; i++) {
        v8::Local<v8::Object> o;
        exlib::string name;
        obj_ptr<X509Cert_base> crt;
        obj_ptr<PKey_base> key;

        hr = GetArgumentValue(certs->Get(i), o, true);
        if (hr < 0)
            return hr;

        GetConfigValue(isolate->m_isolate, o, "name", name);

        hr = GetConfigValue(isolate->m_isolate, o, "crt", crt);
        if (hr < 0)
            return hr;

        hr = GetConfigValue(isolate->m_isolate, o, "key", key);
        if (hr < 0)
            return hr;

        result_t hr = ss->setCert(name, crt, key);
        if (hr < 0)
            return hr;
    }

    retVal = ss;
    return 0;
}

SslSocket::SslSocket()
{
    mbedtls_ssl_init(&m_ssl);
    mbedtls_ssl_config_init(&m_ssl_conf);
    mbedtls_ssl_config_defaults(&m_ssl_conf,
        MBEDTLS_SSL_IS_CLIENT,
        MBEDTLS_SSL_TRANSPORT_STREAM,
        MBEDTLS_SSL_PRESET_DEFAULT);
    mbedtls_ssl_conf_authmode(&m_ssl_conf, g_ssl.m_authmode);
    mbedtls_ssl_conf_rng(&m_ssl_conf, mbedtls_ctr_drbg_random, &g_ssl.ctr_drbg);

    mbedtls_ssl_conf_min_version(&m_ssl_conf, MBEDTLS_SSL_MAJOR_VERSION_3, g_ssl.m_min_version);
    mbedtls_ssl_conf_max_version(&m_ssl_conf, MBEDTLS_SSL_MAJOR_VERSION_3, g_ssl.m_max_version);

    m_recv_pos = 0;
}

SslSocket::~SslSocket()
{
    mbedtls_ssl_config_free(&m_ssl_conf);
    mbedtls_ssl_free(&m_ssl);
    memset(&m_ssl, 0, sizeof(m_ssl));
}

SslSocket::Cert::Cert(exlib::string name, X509Cert_base* crt, PKey_base* key)
    : m_name(name)
    , m_crt((X509Cert*)crt)
    , m_key((PKey*)key)
{
    int32_t opt = PCRE_JAVASCRIPT_COMPAT | PCRE_NEWLINE_ANYCRLF | PCRE_UCP | PCRE_CASELESS;
    const char* error;
    int32_t erroffset;

    m_re = pcre_compile(Routing::host2RegExp(name).c_str(), opt,
        &error, &erroffset, NULL);
}

result_t SslSocket::setCert(Cert* crt)
{
    result_t hr;
    bool priv;

    hr = crt->m_key->isPrivate(priv);
    if (hr < 0)
        return hr;

    if (!priv)
        return CHECK_ERROR(CALL_E_INVALIDARG);

    int32_t ret = mbedtls_ssl_conf_own_cert(&m_ssl_conf, &crt->m_crt->m_crt,
        &crt->m_key->m_key);
    if (ret != 0)
        return CHECK_ERROR(_ssl::setError(ret));

    m_crts.push_back(crt);

    return 0;
}

int32_t SslSocket::my_recv(unsigned char* buf, size_t len)
{
    if (!len)
        return 0;

    if (m_recv_pos < 0)
        return MBEDTLS_ERR_NET_CONN_RESET;

    if (m_recv_pos == (int32_t)m_recv.length()) {
        m_recv_pos = 0;
        m_recv.resize(0);
        return MBEDTLS_ERR_SSL_WANT_READ;
    }

    if (len > m_recv.length() - m_recv_pos)
        len = m_recv.length() - m_recv_pos;

    memcpy(buf, m_recv.c_str() + m_recv_pos, len);
    m_recv_pos += (int32_t)len;

    return (int32_t)len;
}

int32_t SslSocket::my_recv(void* ctx, unsigned char* buf, size_t len)
{
    return ((SslSocket*)ctx)->my_recv(buf, len);
}

int32_t SslSocket::my_send(const unsigned char* buf, size_t len)
{
    if (!len)
        return 0;

    if (m_send.length() > 8192)
        return MBEDTLS_ERR_SSL_WANT_WRITE;

    m_send.append((const char*)buf, len);
    return (int32_t)len;
}

int32_t SslSocket::my_send(void* ctx, const unsigned char* buf, size_t len)
{
    return ((SslSocket*)ctx)->my_send(buf, len);
}

result_t SslSocket::get_fd(int32_t& retVal)
{
    if (!m_s)
        return CHECK_ERROR(CALL_E_CLOSED);

    return m_s->get_fd(retVal);
}

result_t SslSocket::read(int32_t bytes, obj_ptr<Buffer_base>& retVal,
    AsyncEvent* ac)
{
    class asyncRead : public asyncSsl {
    public:
        asyncRead(SslSocket* pThis, int32_t bytes, obj_ptr<Buffer_base>& retVal,
            AsyncEvent* ac)
            : asyncSsl(pThis, ac)
            , m_bytes(bytes)
            , m_retVal(retVal)
        {
            if (m_bytes == -1)
                m_bytes = 8192;

            m_buf.resize(m_bytes);
        }

    public:
        virtual int32_t process()
        {
            int32_t ret = mbedtls_ssl_read(&m_pThis->m_ssl, (unsigned char*)m_buf.c_buffer(), m_bytes);
            if (ret > 0) {
                m_buf.resize(ret);
                m_retVal = new Buffer(m_buf);
                if (g_ssldump)
                    outLog(console_base::C_NOTICE, clean_string(m_buf));
                return 0;
            }

            if (ret == MBEDTLS_ERR_SSL_WANT_READ || ret == MBEDTLS_ERR_SSL_WANT_WRITE)
                return ret;

            m_pThis->m_send.resize(0);
            return 0;
        }

        virtual int32_t finally()
        {
            return next(m_retVal ? 0 : CALL_RETURN_NULL);
        }

    private:
        int32_t m_bytes;
        exlib::string m_buf;
        obj_ptr<Buffer_base>& m_retVal;
    };

    if (!m_s)
        return CHECK_ERROR(CALL_E_CLOSED);

    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    return (new asyncRead(this, bytes, retVal, ac))->post(0);
}

result_t SslSocket::write(Buffer_base* data, AsyncEvent* ac)
{
    class asyncWrite : public asyncSsl {
    public:
        asyncWrite(SslSocket* pThis, Buffer_base* data, AsyncEvent* ac)
            : asyncSsl(pThis, ac)
        {
            data->toString(m_buf);
            if (g_ssldump)
                outLog(console_base::C_WARN, clean_string(m_buf));
            m_pos = 0;
        }

    public:
        virtual int32_t process()
        {
            int32_t ret;

            while ((ret = mbedtls_ssl_write(&m_pThis->m_ssl,
                        (const unsigned char*)m_buf.c_str() + m_pos,
                        m_buf.length() - m_pos))
                > 0) {
                m_pos += ret;
                if (m_pos == (int32_t)m_buf.length())
                    return 0;
            }

            return ret;
        }

    private:
        exlib::string m_buf;
        int32_t m_pos;
    };

    if (!m_s)
        return CHECK_ERROR(CALL_E_CLOSED);

    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    return (new asyncWrite(this, data, ac))->post(0);
}

result_t SslSocket::flush(AsyncEvent* ac)
{
    return 0;
}

result_t SslSocket::close(AsyncEvent* ac)
{
    class asyncClose : public asyncSsl {
    public:
        asyncClose(SslSocket* pThis, AsyncEvent* ac)
            : asyncSsl(pThis, ac)
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

        virtual int32_t finally()
        {
            return m_pThis->m_s->close(next());
        }
    };

    if (!m_s)
        return CHECK_ERROR(CALL_E_CLOSED);

    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    return (new asyncClose(this, ac))->post(0);
}

result_t SslSocket::copyTo(Stream_base* stm, int64_t bytes,
    int64_t& retVal, AsyncEvent* ac)
{
    if (!m_s)
        return CHECK_ERROR(CALL_E_CLOSED);

    return io_base::copyStream(this, stm, bytes, retVal, ac);
}

result_t SslSocket::get_verification(int32_t& retVal)
{
    retVal = m_ssl_conf.authmode;
    return 0;
}

result_t SslSocket::set_verification(int32_t newVal)
{
    if (newVal < ssl_base::C_VERIFY_NONE || newVal > ssl_base::C_VERIFY_REQUIRED)
        return CHECK_ERROR(CALL_E_INVALIDARG);

    mbedtls_ssl_conf_authmode(&m_ssl_conf, newVal);
    return 0;
}

result_t SslSocket::get_ca(obj_ptr<X509Cert_base>& retVal)
{
    if (!m_ca)
        m_ca = new X509Cert();

    retVal = m_ca;
    return 0;
}

result_t SslSocket::get_peerCert(obj_ptr<X509Cert_base>& retVal)
{
    const mbedtls_x509_crt* crt = mbedtls_ssl_get_peer_cert(&m_ssl);

    if (!crt)
        return CALL_RETURN_NULL;

    obj_ptr<X509Cert> crtObject = new X509Cert();

    result_t hr = crtObject->load(crt);
    if (hr < 0)
        return hr;

    retVal = crtObject;

    return 0;
}

result_t SslSocket::get_hostname(exlib::string& retVal)
{
    if (m_ssl.hostname)
        retVal = m_ssl.hostname;
    return 0;
}

result_t SslSocket::get_stream(obj_ptr<Stream_base>& retVal)
{
    if (!m_s)
        return CALL_RETURN_NULL;

    retVal = m_s;
    return 0;
}

result_t SslSocket::handshake(int32_t* retVal, AsyncEvent* ac)
{
    class asyncHandshake : public asyncSsl {
    public:
        asyncHandshake(SslSocket* pThis, int32_t* retVal, AsyncEvent* ac)
            : asyncSsl(pThis, ac)
            , m_retVal(retVal)
        {
        }

    public:
        virtual int32_t process()
        {
            return mbedtls_ssl_handshake(&m_pThis->m_ssl);
        }

        virtual int32_t finally()
        {
            if (m_retVal)
                *m_retVal = mbedtls_ssl_get_verify_result(&m_pThis->m_ssl);

            return next();
        }

    private:
        int32_t* m_retVal;
    };

    return (new asyncHandshake(this, retVal, ac))->post(0);
}

result_t SslSocket::connect(Stream_base* s, exlib::string server_name,
    int32_t& retVal, AsyncEvent* ac)
{
    if (m_s)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    int32_t ret;
    m_s = s;

    X509Cert_base* ca = ac->isolate()->m_ca;
    if (!m_ca)
        m_ca = (X509Cert*)ca;

    mbedtls_ssl_conf_ca_chain(&m_ssl_conf, &m_ca->m_crt, NULL);

    ret = mbedtls_ssl_setup(&m_ssl, &m_ssl_conf);
    if (ret != 0)
        return CHECK_ERROR(_ssl::setError(ret));

    mbedtls_ssl_set_bio(&m_ssl, this, my_send, my_recv, NULL);

    if (!server_name.empty())
        mbedtls_ssl_set_hostname(&m_ssl, server_name.c_str());

    return handshake(&retVal, ac);
}

#define RE_SIZE 64
int SslSocket::sni_callback(void* p_info, mbedtls_ssl_context* ssl,
    const unsigned char* name, size_t name_len)
{
    SslSocket* ss = (SslSocket*)p_info;
    int32_t sz = (int32_t)ss->m_crts.size();
    int32_t i;
    Cert* def_crt = NULL;
    int32_t rc = 0;
    int32_t ovector[RE_SIZE];

    if (sz == 0)
        return -1;

    for (i = 0; i < sz; i++) {
        Cert* crt = ss->m_crts[i];

        if (crt->m_name.empty()) {
            if (def_crt == NULL)
                def_crt = crt;
        } else {
            rc = pcre_exec(crt->m_re, NULL, (const char*)name, (int32_t)name_len,
                0, 0, ovector, RE_SIZE);
            if (rc > 0) {
                mbedtls_ssl_set_hostname(&ss->m_ssl, crt->m_name.c_str());
                return mbedtls_ssl_set_hs_own_cert(ssl, &crt->m_crt->m_crt,
                    &crt->m_key->m_key);
            }
        }
    }

    if (!def_crt)
        return -1;

    return mbedtls_ssl_set_hs_own_cert(ssl, &def_crt->m_crt->m_crt,
        &def_crt->m_key->m_key);
}

result_t SslSocket::accept(Stream_base* s, obj_ptr<SslSocket_base>& retVal,
    AsyncEvent* ac)
{
    if (m_s)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    obj_ptr<SslSocket> ss = new SslSocket();
    retVal = ss;
    int32_t sz = (int32_t)m_crts.size();
    int32_t i;
    result_t hr;
    int32_t ret;

    for (i = 0; i < sz; i++) {
        hr = ss->setCert(m_crts[i]);
        if (hr < 0)
            return hr;
    }

    ss->m_s = s;
    mbedtls_ssl_conf_authmode(&ss->m_ssl_conf, m_ssl_conf.authmode);
    mbedtls_ssl_conf_endpoint(&ss->m_ssl_conf, MBEDTLS_SSL_IS_SERVER);
    mbedtls_ssl_conf_sni(&ss->m_ssl_conf, sni_callback, ss);

    ret = mbedtls_ssl_conf_dh_param(&ss->m_ssl_conf,
        MBEDTLS_DHM_RFC5114_MODP_2048_P,
        MBEDTLS_DHM_RFC5114_MODP_2048_G);
    if (ret != 0)
        return CHECK_ERROR(_ssl::setError(ret));

    if (m_ca) {
        ss->m_ca = m_ca;
        mbedtls_ssl_conf_ca_chain(&ss->m_ssl_conf, &m_ca->m_crt, NULL);
    }

    mbedtls_ssl_conf_session_cache(&ss->m_ssl_conf, &g_ssl.m_cache,
        mbedtls_ssl_cache_get, mbedtls_ssl_cache_set);

    ret = mbedtls_ssl_setup(&ss->m_ssl, &ss->m_ssl_conf);
    if (ret != 0)
        return CHECK_ERROR(_ssl::setError(ret));

    mbedtls_ssl_set_bio(&ss->m_ssl, ss, my_send, my_recv, NULL);

    return ss->handshake(NULL, ac);
}
}
