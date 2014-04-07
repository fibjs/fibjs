/*
 * Socket.h
 *
 *  Created on: Apr 6, 2014
 *      Author: lion
 */

#include "ifs/SslSocket.h"
#include "Buffer.h"
#include "polarssl/config.h"
#include "polarssl/ssl.h"
#include "polarssl/entropy.h"
#include "polarssl/ctr_drbg.h"

#ifndef SSLSOCKET_H_
#define SSLSOCKET_H_

namespace fibjs
{

class SslSocket: public SslSocket_base
{
public:
    SslSocket();
    ~SslSocket();

public:
    // Stream_base
    virtual result_t read(int32_t bytes, obj_ptr<Buffer_base> &retVal,
                          exlib::AsyncEvent *ac);
    virtual result_t write(Buffer_base *data, exlib::AsyncEvent *ac);
    virtual result_t close(exlib::AsyncEvent *ac);
    virtual result_t copyTo(Stream_base *stm, int64_t bytes,
                            int64_t &retVal, exlib::AsyncEvent *ac);

public:
    // SslSocket_base
    virtual result_t connect(Stream_base *s, exlib::AsyncEvent *ac);
    virtual result_t accept(Stream_base *s, exlib::AsyncEvent *ac);

private:
    class asyncSsl: public asyncState
    {
    public:
        asyncSsl(SslSocket *pThis, exlib::AsyncEvent *ac) :
            asyncState(ac), m_pThis(pThis), m_ret(0)
        {
            set(process);
        }

    public:
        virtual int process()
        {
            return 0;
        }

        static int process(asyncState *pState, int n)
        {
            asyncSsl *pThis = (asyncSsl *) pState;

            if (pThis->m_buf)
            {
                pThis->m_pThis->m_recv_pos = 0;
                pThis->m_buf->toString(pThis->m_pThis->m_recv);
                pThis->m_buf.Release();
            }

            pThis->m_ret = pThis->process();
            if (pThis->m_ret == 0)
            {
                if (pThis->m_pThis->m_send.length() > 0)
                {
                    pThis->set(flush);
                    return 0;
                }
                else
                    return pThis->done(0);
            }

            pThis->set(send);
            if (pThis->m_ret == POLARSSL_ERR_NET_WANT_READ ||
                    pThis->m_ret == POLARSSL_ERR_NET_WANT_WRITE)
                return 0;

            return SslSocket::setError(pThis->m_ret);
        }

        static int send(asyncState *pState, int n)
        {
            asyncSsl *pThis = (asyncSsl *) pState;

            std::string &m_send =  pThis->m_pThis->m_send;

            pThis->set(recv);
            if (m_send.empty())
                return 0;

            pThis->m_buf = new Buffer(m_send);
            m_send.resize(0);

            return pThis->m_pThis->m_s->write(pThis->m_buf, pThis);
        }

        static int recv(asyncState *pState, int n)
        {
            asyncSsl *pThis = (asyncSsl *) pState;

            pThis->m_buf.Release();

            pThis->set(process);
            if (pThis->m_ret == POLARSSL_ERR_NET_WANT_WRITE)
                return 0;

            return pThis->m_pThis->m_s->read(-1, pThis->m_buf, pThis);
        }

        static int flush(asyncState *pState, int n)
        {
            asyncSsl *pThis = (asyncSsl *) pState;

            std::string &m_send =  pThis->m_pThis->m_send;

            pThis->m_buf = new Buffer(m_send);
            m_send.resize(0);

            pThis->set(end);
            return pThis->m_pThis->m_s->write(pThis->m_buf, pThis);
        }

        static int end(asyncState *pState, int n)
        {
            asyncSsl *pThis = (asyncSsl *) pState;
            return pThis->done(0);
        }

    protected:
        obj_ptr<SslSocket> m_pThis;
        obj_ptr<Buffer_base> m_buf;
        int m_ret;
    };

private:
    int my_recv(unsigned char *buf, size_t len);
    static int my_recv(void *ctx, unsigned char *buf, size_t len);

    int my_send(const unsigned char *buf, size_t len);
    static int my_send(void *ctx, const unsigned char *buf, size_t len);

    static result_t setError(int ret);

public:
    ssl_context m_ssl;

private:
    entropy_context m_entropy;
    ctr_drbg_context m_ctr_drbg;
    obj_ptr<Stream_base> m_s;
    std::string m_recv;
    int m_recv_pos;
    std::string m_send;
};

}

#endif /* SSLSOCKET_H_ */
