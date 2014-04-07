/*
 * File.cpp
 *
 *  Created on: Apr 6, 2014
 *      Author: lion
 */

#include "ifs/ssl.h"
#include "SslSocket.h"

#include <string.h>
#include <stdio.h>

#include "polarssl/ssl_cache.h"
#include "polarssl/error.h"

namespace fibjs
{

static class _ssl
{
public:
    _ssl()
    {
        ssl_cache_init(&m_cache);
    }

public:
    ssl_cache_context m_cache;
} s_ssl;

result_t ssl_base::loadRsaKeys()
{

    return 0;
}

result_t ssl_base::connect(Stream_base *s, obj_ptr<SslSocket_base> &retVal,
                           exlib::AsyncEvent *ac)
{
    if (!ac)
        return CALL_E_NOSYNC;

    retVal = new SslSocket();

    return retVal->connect(s, ac);
}

result_t ssl_base::accept(Stream_base *s, obj_ptr<SslSocket_base> &retVal,
                          exlib::AsyncEvent *ac)
{
    if (!ac)
        return CALL_E_NOSYNC;

    retVal = new SslSocket();

    return retVal->accept(s, ac);
}

result_t SslSocket_base::_new(obj_ptr<SslSocket_base> &retVal)
{
    retVal = new SslSocket();
    return 0;
}

SslSocket::SslSocket()
{
    const char *pers = "fibjs";

    ssl_init(&m_ssl);
    entropy_init(&m_entropy);
    ctr_drbg_init( &m_ctr_drbg, entropy_func, &m_entropy,
                   (const unsigned char *) pers,
                   strlen(pers));
}

SslSocket::~SslSocket()
{
    ssl_free(&m_ssl);
    entropy_free(&m_entropy);

    memset(&m_ssl, 0, sizeof(m_ssl));
}

int SslSocket::my_recv(unsigned char *buf, size_t len)
{
    if (!len)
        return 0;

    puts("SslSocket: my_recv");
    if (m_recv.empty())
        return POLARSSL_ERR_NET_WANT_READ;

    if (len > m_recv.length())
        len = m_recv.length();

    memcpy(buf, m_recv.c_str(), len);
    m_recv = m_recv.substr(len);

    return (int)len;
}

int SslSocket::my_recv(void *ctx, unsigned char *buf, size_t len)
{
    return ((SslSocket *)ctx)->my_recv(buf, len);
}

int SslSocket::my_send(const unsigned char *buf, size_t len)
{
    if (!len)
        return 0;

    puts("SslSocket: my_send");
    if (!m_send.empty())
    {
        puts("SslSocket: my_send  POLARSSL_ERR_NET_WANT_WRITE");
        return POLARSSL_ERR_NET_WANT_WRITE;
    }

    m_send.resize(len);
    memcpy(&m_send[0], buf, len);

    return (int)len;
}

int SslSocket::my_send(void *ctx, const unsigned char *buf, size_t len)
{
    return ((SslSocket *)ctx)->my_send(buf, len);
}

result_t SslSocket::read(int32_t bytes, obj_ptr<Buffer_base> &retVal,
                         exlib::AsyncEvent *ac)
{
    class asyncRead: public asyncSsl
    {
    public:
        asyncRead(SslSocket *pThis, int32_t bytes, obj_ptr<Buffer_base> &retVal,
                  exlib::AsyncEvent *ac) :
            asyncSsl(pThis, ac), m_bytes(bytes), m_retVal(retVal)
        {
            m_buf.resize(bytes);
        }

    public:
        virtual int process()
        {
            int ret = ssl_read(&m_pThis->m_ssl, (unsigned char *)&m_buf[0], m_bytes);

            if (ret >= 0)
            {
                m_buf.resize(ret);
                m_retVal = new Buffer(m_buf);
            }

            return ret;
        }

    private:
        int32_t m_bytes;
        std::string m_buf;
        obj_ptr<Buffer_base> &m_retVal;
    };

    if (!ac)
        return CALL_E_NOSYNC;

    puts("================  read");
    return (new asyncRead(this, bytes, retVal, ac))->post(0);
}

result_t SslSocket::write(Buffer_base *data, exlib::AsyncEvent *ac)
{
    class asyncWrite: public asyncSsl
    {
    public:
        asyncWrite(SslSocket *pThis, Buffer_base *data, exlib::AsyncEvent *ac) :
            asyncSsl(pThis, ac)
        {
            data->toString(m_buf);
        }

    public:
        virtual int process()
        {
            return ssl_write(&m_pThis->m_ssl, (const unsigned char *)m_buf.c_str(),
                             m_buf.length());
        }

    private:
        std::string m_buf;
    };

    if (!ac)
        return CALL_E_NOSYNC;

    puts("================  send");
    return (new asyncWrite(this, data, ac))->post(0);
}

result_t SslSocket::close(exlib::AsyncEvent *ac)
{
    if (!ac)
        return CALL_E_NOSYNC;

    return 0;
}

result_t SslSocket::copyTo(Stream_base *stm, int64_t bytes,
                           int64_t &retVal, exlib::AsyncEvent *ac)
{
    if (!ac)
        return CALL_E_NOSYNC;

    return 0;
}

#define DEBUG_LEVEL 1
static void my_debug( void *ctx, int level, const char *str )
{
    if ( level < DEBUG_LEVEL )
    {
        fprintf( (FILE *) ctx, "%s", str );
        fflush(  (FILE *) ctx  );
    }
}

result_t SslSocket::connect(Stream_base *s, exlib::AsyncEvent *ac)
{
    class asyncConnect: public asyncSsl
    {
    public:
        asyncConnect(SslSocket *pThis, exlib::AsyncEvent *ac) :
            asyncSsl(pThis, ac)
        {
        }

    public:
        virtual int process()
        {
            return ssl_handshake(&m_pThis->m_ssl);
        }
    };

    if (!ac)
        return CALL_E_NOSYNC;

    if (!s)
        return CALL_E_INVALIDARG;
    m_s = s;

    ssl_set_endpoint(&m_ssl, SSL_IS_CLIENT);
    ssl_set_rng(&m_ssl, ctr_drbg_random, &m_ctr_drbg);
    ssl_set_dbg(&m_ssl, my_debug, stdout);
    ssl_set_bio(&m_ssl, my_recv, this, my_send, this);

    puts("================  connect");
    return (new asyncConnect(this, ac))->post(0);
}

result_t SslSocket::accept(Stream_base *s, exlib::AsyncEvent *ac)
{
    if (!ac)
        return CALL_E_NOSYNC;

    if (!s)
        return CALL_E_INVALIDARG;
    m_s = s;

    return 0;
}

struct
{
    int no;
    const char *msg;
} s_msgs[] =
{
    {10, ""}

};

result_t SslSocket::setError(int ret)
{
    char msg[128];

    polarssl_strerror(ret, msg, sizeof(msg));
    return Runtime::setError(msg);
}

}
