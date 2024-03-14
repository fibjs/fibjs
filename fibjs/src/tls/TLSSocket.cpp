/*
 * TLSSocket.cpp
 *
 *  Created on: Mar 4, 2024
 *      Author: lion
 */

#include "ifs/io.h"
#include "ifs/tls.h"
#include "ifs/Socket.h"
#include "ifs/console.h"
#include "TLSSocket.h"
#include "SecureContext.h"
#include "X509Certificate.h"
#include "Buffer.h"
#include "options.h"

namespace fibjs {

static const BIO_METHOD* s_method = []() {
    BIO_METHOD* method = BIO_meth_new(BIO_TYPE_MEM, "fibjs stream bio");
    BIO_meth_set_write(method, [](BIO* bio, const char* data, int len) {
        return TLSSocket::FromBIO(bio)->Write(data, len);
    });
    BIO_meth_set_read(method, [](BIO* bio, char* out, int len) {
        return TLSSocket::FromBIO(bio)->Read(out, len);
    });
    BIO_meth_set_ctrl(method, [](BIO* bio, int cmd, long num, void* ptr) {
        switch (cmd) {
        case BIO_CTRL_FLUSH:
            return (long)1;
        case BIO_CTRL_EOF:
            return TLSSocket::FromBIO(bio)->m_eof;
        case BIO_CTRL_PUSH:
        case BIO_CTRL_POP:
            return (long)0;
        }

        return (long)0;
    });
    BIO_meth_set_create(method, [](BIO* bio) {
        BIO_set_init(bio, 1);
        return 1;
    });
    BIO_meth_set_destroy(method, [](BIO* bio) {
        if (bio == nullptr)
            return 0;

        if (BIO_get_shutdown(bio)) {
            if (BIO_get_init(bio) && BIO_get_data(bio) != nullptr)
                BIO_set_data(bio, nullptr);
        }

        return 1;
    });

    return method;
}();

result_t TLSSocket_base::_new(obj_ptr<TLSSocket_base>& retVal, v8::Local<v8::Object> This)
{
    Isolate* isolate = Isolate::current(This);

    obj_ptr<TLSSocket> sock = new TLSSocket();
    sock->wrap(This);
    sock->init(isolate->m_ctx);

    retVal = sock;

    return 0;
}

result_t TLSSocket_base::_new(SecureContext_base* context, obj_ptr<TLSSocket_base>& retVal, v8::Local<v8::Object> This)
{
    obj_ptr<TLSSocket> sock = new TLSSocket();
    sock->wrap(This);
    sock->init(context);

    retVal = sock;

    return 0;
}

result_t TLSSocket_base::_new(v8::Local<v8::Object> options, bool isServer, obj_ptr<TLSSocket_base>& retVal, v8::Local<v8::Object> This)
{
    obj_ptr<SecureContext_base> context;
    result_t hr = tls_base::createSecureContext(options, isServer, context);
    if (hr < 0)
        return hr;

    return _new(context, retVal, This);
}

result_t TLSSocket::init(SecureContext_base* context)
{
    m_ctx = context;
    m_tls = SSL_new(m_ctx.As<SecureContext>()->ctx());

    return 0;
}

class AsyncHandshake : public AsyncState {
public:
    AsyncHandshake(TLSSocket* sock, Stream_base* socket, bool is_server, exlib::string server_name, AsyncEvent* ac)
        : AsyncState(ac)
        , m_sock(sock)
    {
        m_sock->m_read_lock.lock(this);
        m_sock->m_write_lock.lock(this);

        m_sock->m_stream = socket;

        m_sock->m_bio_in = BIO_new(s_method);
        BIO_set_data(m_sock->m_bio_in, m_sock);

        m_sock->m_bio_out = BIO_new(s_method);
        BIO_set_data(m_sock->m_bio_out, m_sock);

        SSL_set_bio(m_sock->m_tls, m_sock->m_bio_in, m_sock->m_bio_out);

        if (is_server)
            SSL_set_accept_state(m_sock->m_tls);
        else {
            SSL_set_connect_state(m_sock->m_tls);
            if (!server_name.empty())
                SSL_set1_host(m_sock->m_tls, server_name.c_str());
        }

        next(handshake);
    }

    ~AsyncHandshake()
    {
        m_sock->m_write_lock.unlock(this);
        m_sock->m_read_lock.unlock(this);
    }

public:
    ON_STATE(AsyncHandshake, handshake)
    {
        m_state = SSL_get_error(m_sock->m_tls, SSL_do_handshake(m_sock->m_tls));
        if (m_sock->m_out)
            return m_sock->m_stream->write(m_sock->m_out, next(read));

        return next(read);
    }

    ON_STATE(AsyncHandshake, read)
    {
        m_sock->m_out.Release();

        switch (m_state) {
        case SSL_ERROR_NONE:
            return next();
        case SSL_ERROR_WANT_READ:
            return m_sock->m_stream->read(0, m_sock->m_in, next(read_ok));
        case SSL_ERROR_WANT_WRITE:
            return next(handshake);
        case SSL_ERROR_SSL:
            return openssl_error();
        }

        return Runtime::setError("handshake failed");
    }

    ON_STATE(AsyncHandshake, read_ok)
    {
        if (m_sock->m_in) {
            m_sock->m_inpos = 0;
            return next(handshake);
        }

        return Runtime::setError("socket closed");
    }

public:
    obj_ptr<TLSSocket> m_sock;
    int32_t m_state;
};

result_t TLSSocket::connect(Stream_base* socket, exlib::string server_name, AsyncEvent* ac)
{
    result_t hr = is_not_connected();
    if (hr < 0)
        return hr;

    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    return (new AsyncHandshake(this, socket, false, server_name, ac))->post(0);
}

result_t TLSSocket::accept(Stream_base* socket, AsyncEvent* ac)
{
    result_t hr = is_not_connected();
    if (hr < 0)
        return hr;

    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    return (new AsyncHandshake(this, socket, true, "", ac))->post(0);
}

result_t TLSSocket::getProtocol(exlib::string& retVal)
{
    const char* protocol = SSL_get_version(m_tls);
    if (protocol == nullptr)
        return CALL_RETURN_UNDEFINED;

    retVal = protocol;

    return 0;
}

result_t TLSSocket::getX509Certificate(obj_ptr<X509Certificate_base>& retVal)
{
    if (!m_cert) {
        X509* cert = SSL_get_certificate(m_tls);
        if (cert == nullptr)
            return CALL_RETURN_UNDEFINED;

        X509_up_ref(cert);
        m_cert = new X509Certificate(cert);
    }

    retVal = m_cert;
    return 0;
}

result_t TLSSocket::getPeerX509Certificate(obj_ptr<X509Certificate_base>& retVal)
{
    if (!m_peer_cert) {
        X509* cert = SSL_get_peer_certificate(m_tls);
        if (cert == nullptr)
            return CALL_RETURN_UNDEFINED;

        m_peer_cert = new X509Certificate(cert);
    }

    retVal = m_peer_cert;
    return 0;
}

result_t TLSSocket::get_secureContext(obj_ptr<SecureContext_base>& retVal)
{
    retVal = m_ctx;
    return 0;
}

result_t TLSSocket::get_remoteAddress(exlib::string& retVal)
{
    obj_ptr<Socket_base> sock = Socket_base::getInstance(m_stream);
    if (!sock)
        return CALL_E_INVALID_CALL;

    return sock->get_remoteAddress(retVal);
}

result_t TLSSocket::get_remotePort(int32_t& retVal)
{
    obj_ptr<Socket_base> sock = Socket_base::getInstance(m_stream);
    if (!sock)
        return CALL_E_INVALID_CALL;

    return sock->get_remotePort(retVal);
}

result_t TLSSocket::get_localAddress(exlib::string& retVal)
{
    obj_ptr<Socket_base> sock = Socket_base::getInstance(m_stream);
    if (!sock)
        return CALL_E_INVALID_CALL;

    return sock->get_localAddress(retVal);
}

result_t TLSSocket::get_localPort(int32_t& retVal)
{
    obj_ptr<Socket_base> sock = Socket_base::getInstance(m_stream);
    if (!sock)
        return CALL_E_INVALID_CALL;

    return sock->get_localPort(retVal);
}

result_t TLSSocket::get_fd(int32_t& retVal)
{
    obj_ptr<Socket_base> sock = Socket_base::getInstance(m_stream);
    if (!sock)
        return CALL_E_INVALID_CALL;

    return sock->get_fd(retVal);
}

result_t TLSSocket::read(int32_t bytes, obj_ptr<Buffer_base>& retVal, AsyncEvent* ac)
{
    class AsyncRead : public AsyncState {
    public:
        AsyncRead(TLSSocket* sock, int32_t bytes, obj_ptr<Buffer_base>& retVal, AsyncEvent* ac)
            : AsyncState(ac)
            , m_sock(sock)
            , m_bytes(bytes)
            , m_retVal(retVal)
        {
            m_data = new Buffer(nullptr, bytes < 0 ? 8192 : bytes);
            next(try_lock);
        }

        ~AsyncRead()
        {
            m_sock->m_read_lock.unlock(this);
        }

    public:
        ON_STATE(AsyncRead, try_lock)
        {
            return lock(m_sock->m_read_lock, next(read));
        }

        ON_STATE(AsyncRead, read)
        {
            if (n == CALL_RETURN_NULL)
                m_sock->m_eof = 1;

            if (!m_sock->m_eof) {
                int32_t size = SSL_read(m_sock->m_tls, m_data->data() + m_pos, m_data->length() - m_pos);
                if (size < 0 && m_sock->m_in)
                    return openssl_error();

                if (size > 0)
                    m_pos += size;
                if ((size <= 0 && SSL_get_error(m_sock->m_tls, size) != SSL_ERROR_ZERO_RETURN)
                    || (m_bytes > 0 && size > 0 && m_pos < m_bytes))
                    return m_sock->m_in ? next(read) : m_sock->m_stream->read(0, m_sock->m_in, next(read));
            }

            if (m_pos == 0)
                return next(CALL_RETURN_NULL);

            m_data->resize(m_pos);
            m_retVal = m_data;

            if (g_ssldump)
                outLog(console_base::C_NOTICE, clean_string((const char*)m_data->data(), m_data->length()));

            return next();
        }

        result_t lock(exlib::Locker& l, AsyncState* pThis)
        {
            return l.lock(pThis) ? 0 : CALL_E_PENDDING;
        }

    public:
        obj_ptr<TLSSocket> m_sock;
        int32_t m_bytes;
        obj_ptr<Buffer_base>& m_retVal;
        obj_ptr<Buffer> m_data;
        int32_t m_pos = 0;
    };

    result_t hr = is_ready();
    if (hr < 0)
        return hr;

    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    return (new AsyncRead(this, bytes, retVal, ac))->post(0);
}

result_t TLSSocket::write(Buffer_base* data, AsyncEvent* ac)
{
    class AsyncWrite : public AsyncState {
    public:
        AsyncWrite(TLSSocket* sock, Buffer_base* data, AsyncEvent* ac)
            : AsyncState(ac)
            , m_sock(sock)
            , m_data(data)
        {
            next(try_lock);
        }

        ~AsyncWrite()
        {
            m_sock->m_write_lock.unlock(this);
        }

    public:
        ON_STATE(AsyncWrite, try_lock)
        {
            return lock(m_sock->m_write_lock, next(write));
        }

        ON_STATE(AsyncWrite, write)
        {
            if (m_state == SSL_ERROR_SSL)
                return openssl_error();

            if (m_state == SSL_ERROR_NONE) {
                if (g_ssldump)
                    outLog(console_base::C_WARN, clean_string((const char*)m_data.As<Buffer>()->data(), m_data.As<Buffer>()->length()));
                return next();
            }

            m_sock->m_out.Release();
            m_state = SSL_get_error(m_sock->m_tls,
                SSL_write(m_sock->m_tls, m_data.As<Buffer>()->data(), m_data.As<Buffer>()->length()));
            if (m_sock->m_out)
                return m_sock->m_stream->write(m_sock->m_out, next(write));

            if (m_state == SSL_ERROR_NONE)
                return next(write);

            return Runtime::setError("write failed");
        }

        result_t lock(exlib::Locker& l, AsyncState* pThis)
        {
            return l.lock(pThis) ? 0 : CALL_E_PENDDING;
        }

    public:
        obj_ptr<TLSSocket> m_sock;
        obj_ptr<Buffer_base> m_data;
        int32_t m_state = SSL_ERROR_WANT_WRITE;
    };

    result_t hr = is_ready();
    if (hr < 0)
        return hr;

    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    return (new AsyncWrite(this, data, ac))->post(0);
}

result_t TLSSocket::flush(AsyncEvent* ac)
{
    return 0;
}

result_t TLSSocket::close(AsyncEvent* ac)
{
    class AsyncClose : public AsyncState {
    public:
        AsyncClose(TLSSocket* sock, AsyncEvent* ac)
            : AsyncState(ac)
            , m_sock(sock)
        {
            next(try_lock);
        }

        ~AsyncClose()
        {
            m_sock->m_write_lock.unlock(this);
        }

    public:
        ON_STATE(AsyncClose, try_lock)
        {
            return lock(m_sock->m_write_lock, next(write));
        }

        ON_STATE(AsyncClose, write)
        {
            m_sock->m_out.Release();
            if (m_state != SSL_ERROR_WANT_WRITE)
                return next();

            m_state = SSL_get_error(m_sock->m_tls, SSL_shutdown(m_sock->m_tls));
            if (m_sock->m_out)
                return m_sock->m_stream->write(m_sock->m_out, next(write));

            return next();
        }

        result_t lock(exlib::Locker& l, AsyncState* pThis)
        {
            return l.lock(pThis) ? 0 : CALL_E_PENDDING;
        }

    public:
        obj_ptr<TLSSocket> m_sock;
        int32_t m_state = SSL_ERROR_WANT_WRITE;
    };

    result_t hr = is_ready();
    if (hr < 0)
        return hr;

    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    return (new AsyncClose(this, ac))->post(0);
}

result_t TLSSocket::copyTo(Stream_base* stm, int64_t bytes, int64_t& retVal, AsyncEvent* ac)
{
    result_t hr = is_ready();
    if (hr < 0)
        return hr;

    return io_base::copyStream(this, stm, bytes, retVal, ac);
}

int TLSSocket::Write(const char* data, int len)
{
    BIO_clear_retry_flags(m_bio_out);
    if (m_out) {
        BIO_set_retry_write(m_bio_out);
        return 0;
    }

    m_out = new Buffer((const unsigned char*)data, len);
    return len;
}

int TLSSocket::Read(char* out, int len)
{
    BIO_clear_retry_flags(m_bio_in);
    if (!m_in) {
        BIO_set_retry_read(m_bio_in);
        return 0;
    }

    const unsigned char* data = m_in.As<Buffer>()->data();
    int n = m_in.As<Buffer>()->length();
    if (len > n - m_inpos)
        len = n - m_inpos;

    memcpy(out, data + m_inpos, len);

    m_inpos += len;
    if (m_inpos >= n) {
        m_in.Release();
        m_inpos = 0;
    }

    return len;
}

}
