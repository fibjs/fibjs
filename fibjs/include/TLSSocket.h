/*
 * TLSSocket.h
 *
 *  Created on: Mar 4, 2024
 *      Author: lion
 */

#pragma once

#include "ifs/TLSSocket.h"
#include "ifs/SecureContext.h"
#include "crypto_util.h"

namespace fibjs {

class TLSSocket : public TLSSocket_base {
    FIBER_FREE();

public:
    // TLSSocket_base
    virtual result_t connect(Stream_base* socket, exlib::string server_name, AsyncEvent* ac);
    virtual result_t accept(Stream_base* socket, AsyncEvent* ac);
    virtual result_t get_stream(obj_ptr<Stream_base>& retVal);
    virtual result_t getProtocol(exlib::string& retVal);
    virtual result_t getX509Certificate(obj_ptr<X509Certificate_base>& retVal);
    virtual result_t getPeerX509Certificate(obj_ptr<X509Certificate_base>& retVal);
    virtual result_t get_secureContext(obj_ptr<SecureContext_base>& retVal);
    virtual result_t get_remoteAddress(exlib::string& retVal);
    virtual result_t get_remotePort(int32_t& retVal);
    virtual result_t get_localAddress(exlib::string& retVal);
    virtual result_t get_localPort(int32_t& retVal);

public:
    // Stream_base
    virtual result_t get_fd(int32_t& retVal);
    virtual result_t read(int32_t bytes, obj_ptr<Buffer_base>& retVal, AsyncEvent* ac);
    virtual result_t write(Buffer_base* data, AsyncEvent* ac);
    virtual result_t flush(AsyncEvent* ac);
    virtual result_t close(AsyncEvent* ac);
    virtual result_t copyTo(Stream_base* stm, int64_t bytes, int64_t& retVal, AsyncEvent* ac);

public:
    int Write(const char* data, int len);
    int Read(char* out, int len);

public:
    result_t init(SecureContext_base* context);

    static TLSSocket* FromBIO(BIO* bio)
    {
        return static_cast<TLSSocket*>(BIO_get_data(bio));
    }

private:
    result_t is_ready()
    {
        if (!m_stream)
            return Runtime::setError("socket not connected.");

        if (m_closed)
            return Runtime::setError("socket closed.");

        return 0;
    }

    result_t is_not_connected()
    {
        if (m_stream)
            return Runtime::setError("socket already connected.");

        return 0;
    }

public:
    obj_ptr<Stream_base> m_stream;
    obj_ptr<SecureContext_base> m_ctx;
    BIO *m_bio_in, *m_bio_out;
    SSLPointer m_tls;

public:
    obj_ptr<Buffer_base> m_out;
    obj_ptr<Buffer_base> m_in;
    int32_t m_inpos = 0;
    long m_eof = 0;
    exlib::atomic m_closed;

public:
    exlib::Locker m_read_lock;
    exlib::Locker m_write_lock;

private:
    obj_ptr<X509Certificate_base> m_cert;
    obj_ptr<X509Certificate_base> m_peer_cert;
};
}
