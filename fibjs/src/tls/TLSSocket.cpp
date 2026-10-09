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
#include "EventInfo.h"
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

// Node.js exposes the X509_V_ERR_* enumeration name (without the prefix) as
// the error code for certificate verification failures.
static const char* tls_verify_code(long vr)
{
    switch (vr) {
    case X509_V_ERR_UNSPECIFIED: return "UNSPECIFIED";
    case X509_V_ERR_UNABLE_TO_GET_ISSUER_CERT: return "UNABLE_TO_GET_ISSUER_CERT";
    case X509_V_ERR_UNABLE_TO_GET_CRL: return "UNABLE_TO_GET_CRL";
    case X509_V_ERR_UNABLE_TO_DECRYPT_CERT_SIGNATURE: return "UNABLE_TO_DECRYPT_CERT_SIGNATURE";
    case X509_V_ERR_UNABLE_TO_DECRYPT_CRL_SIGNATURE: return "UNABLE_TO_DECRYPT_CRL_SIGNATURE";
    case X509_V_ERR_UNABLE_TO_DECODE_ISSUER_PUBLIC_KEY: return "UNABLE_TO_DECODE_ISSUER_PUBLIC_KEY";
    case X509_V_ERR_CERT_SIGNATURE_FAILURE: return "CERT_SIGNATURE_FAILURE";
    case X509_V_ERR_CRL_SIGNATURE_FAILURE: return "CRL_SIGNATURE_FAILURE";
    case X509_V_ERR_CERT_NOT_YET_VALID: return "CERT_NOT_YET_VALID";
    case X509_V_ERR_CERT_HAS_EXPIRED: return "CERT_HAS_EXPIRED";
    case X509_V_ERR_CRL_NOT_YET_VALID: return "CRL_NOT_YET_VALID";
    case X509_V_ERR_CRL_HAS_EXPIRED: return "CRL_HAS_EXPIRED";
    case X509_V_ERR_ERROR_IN_CERT_NOT_BEFORE_FIELD: return "ERROR_IN_CERT_NOT_BEFORE_FIELD";
    case X509_V_ERR_ERROR_IN_CERT_NOT_AFTER_FIELD: return "ERROR_IN_CERT_NOT_AFTER_FIELD";
    case X509_V_ERR_ERROR_IN_CRL_LAST_UPDATE_FIELD: return "ERROR_IN_CRL_LAST_UPDATE_FIELD";
    case X509_V_ERR_ERROR_IN_CRL_NEXT_UPDATE_FIELD: return "ERROR_IN_CRL_NEXT_UPDATE_FIELD";
    case X509_V_ERR_OUT_OF_MEM: return "OUT_OF_MEM";
    case X509_V_ERR_DEPTH_ZERO_SELF_SIGNED_CERT: return "DEPTH_ZERO_SELF_SIGNED_CERT";
    case X509_V_ERR_SELF_SIGNED_CERT_IN_CHAIN: return "SELF_SIGNED_CERT_IN_CHAIN";
    case X509_V_ERR_UNABLE_TO_GET_ISSUER_CERT_LOCALLY: return "UNABLE_TO_GET_ISSUER_CERT_LOCALLY";
    case X509_V_ERR_UNABLE_TO_VERIFY_LEAF_SIGNATURE: return "UNABLE_TO_VERIFY_LEAF_SIGNATURE";
    case X509_V_ERR_CERT_CHAIN_TOO_LONG: return "CERT_CHAIN_TOO_LONG";
    case X509_V_ERR_CERT_REVOKED: return "CERT_REVOKED";
    case X509_V_ERR_INVALID_CA: return "INVALID_CA";
    case X509_V_ERR_PATH_LENGTH_EXCEEDED: return "PATH_LENGTH_EXCEEDED";
    case X509_V_ERR_INVALID_PURPOSE: return "INVALID_PURPOSE";
    case X509_V_ERR_CERT_UNTRUSTED: return "CERT_UNTRUSTED";
    case X509_V_ERR_CERT_REJECTED: return "CERT_REJECTED";
    case X509_V_ERR_SUBJECT_ISSUER_MISMATCH: return "SUBJECT_ISSUER_MISMATCH";
    case X509_V_ERR_AKID_SKID_MISMATCH: return "AKID_SKID_MISMATCH";
    case X509_V_ERR_AKID_ISSUER_SERIAL_MISMATCH: return "AKID_ISSUER_SERIAL_MISMATCH";
    case X509_V_ERR_KEYUSAGE_NO_CERTSIGN: return "KEYUSAGE_NO_CERTSIGN";
#ifdef X509_V_ERR_HOSTNAME_MISMATCH
    case X509_V_ERR_HOSTNAME_MISMATCH: return "HOSTNAME_MISMATCH";
#endif
    }
    return nullptr;
}

static result_t tls_handshake_error(SSL* ssl)
{
    long vr = SSL_get_verify_result(ssl);
    if (vr != X509_V_OK) {
        const char* code = tls_verify_code(vr);
        exlib::string reason = X509_verify_cert_error_string(vr);

        if (code)
            return Runtime::setError(ErrorPayload::make(errtype::kError)
                    .with_code(code)
                    .with_message(reason));

        // Unknown verification result: fall back to the upper-snake form.
        exlib::string c = reason;
        for (size_t i = 0; i < c.length(); i++) {
            char ch = c[i];
            if (ch == ' ')
                c[i] = '_';
            else if (ch >= 'a' && ch <= 'z')
                c[i] = (char)(ch - 'a' + 'A');
        }

        return Runtime::setError(ErrorPayload::make(errtype::kError)
                .with_code(c)
                .with_message(reason));
    }

    return openssl_error();
}

class AsyncHandshake : public AsyncState {
public:
    AsyncHandshake(TLSSocket* sock, Stream_base* socket, bool is_server, exlib::string server_name, AsyncHandle ac)
        : AsyncState(ac)
        , m_sock(sock)
        , m_isolate(nullptr)
    {
        init(socket, is_server, server_name);
    }

    AsyncHandshake(TLSSocket* sock, Stream_base* socket, bool is_server, exlib::string server_name, Isolate* isolate)
        : AsyncState(nullptr)
        , m_sock(sock)
        , m_isolate(isolate)
    {
        m_isolate->Ref();
        init(socket, is_server, server_name);
    }

    void init(Stream_base* socket, bool is_server, exlib::string server_name)
    {
        m_server_name = server_name;
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
            if (!server_name.empty()) {
                SSL_set1_host(m_sock->m_tls, server_name.c_str());
                SSL_set_tlsext_host_name(m_sock->m_tls, server_name.c_str());
            }
        }

        AsyncState::init(handshake);
    }

    ~AsyncHandshake()
    {
        if (m_locked) {
            m_sock->m_write_lock.unlock(this);
            m_sock->m_read_lock.unlock(this);
        }
        if (m_isolate)
            m_isolate->Unref();
    }

public:
    ON_STATE(AsyncHandshake, handshake)
    {
        ERR_clear_error();
        m_state = SSL_get_error(m_sock->m_tls, SSL_do_handshake(m_sock->m_tls));
        if (m_state == SSL_ERROR_SSL)
            return tls_handshake_error(m_sock->m_tls);
        if (m_sock->m_out)
            return m_sock->m_stream->writeBuffer(m_sock->m_out, next(read));

        return next(read);
    }

    ON_STATE(AsyncHandshake, read)
    {
        m_sock->m_out.Release();

        switch (m_state) {
        case SSL_ERROR_NONE:
            m_sock->on_connected();
            if (m_isolate)
                (new EventInfo(m_sock, "connect"))->emit();
            // Release locks before completing, since m_ac->post() runs
            // synchronously before ~AsyncHandshake, and the next state
            // may need to acquire these locks.
            m_sock->m_write_lock.unlock(this);
            m_sock->m_read_lock.unlock(this);
            m_locked = false;
            return next();
        case SSL_ERROR_WANT_READ:
            return m_sock->m_stream->readBuffer(-1, m_sock->m_in, next(read_ok));
        case SSL_ERROR_WANT_WRITE:
            return next(handshake);
        case SSL_ERROR_SSL:
            return tls_handshake_error(m_sock->m_tls);
        }

        return Runtime::setError("handshake failed");
    }

    virtual int32_t error(int32_t v)
    {
        m_sock->on_connected(v);

        // The SNI / verification host is the key context for a failed handshake.
        // Keep the payload on this thread for the sync/callback path and pass a
        // copy into the emit fiber, which may run on another thread. The error
        // description travels the same way: a handshake failure is a
        // CALL_E_EXCEPTION whose name/message live in per-thread state.
        Runtime::ErrorDescription desc = Runtime::captureErrorDescription(v);
        ErrorPayload payload = takeErrorPayload();

        if (!m_server_name.empty())
            payload.arg("servername", m_server_name);
        setErrorPayload(payload);

        if (m_isolate) {
            obj_ptr<TLSSocket> sock = m_sock;
            m_isolate->sync([sock, v, desc, payload]() -> int32_t {
                JSFiber::EnterJsScope s;

                Runtime::applyErrorDescription(desc, payload);

                v8::Local<v8::Value> err = FillError(v);
                bool retVal;
                sock->_emit("error", &err, 1, retVal);
                return 0;
            });
        }
        return v;
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
    Isolate* m_isolate;
    int32_t m_state;
    bool m_locked = true;
    exlib::string m_server_name; // client SNI / verification host, for diagnostics
};

result_t TLSSocket::connect(Stream_base* socket, exlib::string server_name, AsyncHandle ac)
{
    result_t hr = is_not_connected();
    if (hr < 0)
        return hr;

    if (ac.isSync()) {
        startConnectEvent();
        return CHECK_ERROR(CALL_E_NOSYNC);
    }

    if (!m_connect_event)
        return (new AsyncHandshake(this, socket, false, server_name, std::move(ac)))->post(0);

    (new AsyncHandshake(this, socket, false, server_name, holder()))->post(0);
    return 0;
}

result_t TLSSocket::connect(Stream_base* socket, v8::Local<v8::Function> connectListener, AsyncHandle ac)
{
    return connect(socket, "", connectListener, std::move(ac));
}

result_t TLSSocket::connect(Stream_base* socket, exlib::string server_name, v8::Local<v8::Function> connectListener, AsyncHandle ac)
{
    if (ac.isSync()) {
        v8::Local<v8::Object> _retVal;
        once(holder()->NewString("connect"), connectListener, _retVal);
    }

    return connect(socket, server_name, std::move(ac));
}

result_t TLSSocket::accept(Stream_base* socket, AsyncHandle ac)
{
    result_t hr = is_not_connected();
    if (hr < 0)
        return hr;

    if (ac.isSync()) {
        startConnectEvent();
        return CHECK_ERROR(CALL_E_NOSYNC);
    }

    return (new AsyncHandshake(this, socket, true, "", std::move(ac)))->post(0);
}

result_t TLSSocket::get_stream(obj_ptr<Stream_base>& retVal)
{
    retVal = m_stream;
    return 0;
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

result_t TLSSocket::get_alpnProtocol(exlib::string& retVal)
{
    const unsigned char* proto = nullptr;
    unsigned int proto_len = 0;

    SSL_get0_alpn_selected(m_tls, &proto, &proto_len);
    if (proto == nullptr || proto_len == 0)
        return CALL_RETURN_UNDEFINED;

    retVal.assign(reinterpret_cast<const char*>(proto), proto_len);
    return 0;
}

result_t TLSSocket::get_fd(int32_t& retVal)
{
    obj_ptr<Socket_base> sock = Socket_base::getInstance(m_stream);
    if (!sock)
        return CALL_E_INVALID_CALL;

    return sock->get_fd(retVal);
}

result_t TLSSocket::readBuffer(int32_t bytes, obj_ptr<Buffer_base>& retVal, AsyncHandle ac)
{
    class AsyncRead : public AsyncState {
    public:
        AsyncRead(TLSSocket* sock, int32_t bytes, obj_ptr<Buffer_base>& retVal, AsyncHandle ac)
            : AsyncState(ac)
            , m_sock(sock)
            , m_bytes(bytes)
            , m_retVal(retVal)
        {
            m_data = new Buffer(nullptr, bytes < 0 ? 8192 : bytes);
            init(try_lock);
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
                m_sock->m_write_lock.lock();

                ERR_clear_error();
                m_sock->m_out.Release();
                int32_t size = SSL_read(m_sock->m_tls, m_data->data() + m_pos, m_data->length() - m_pos);
                int32_t ssl_err = size <= 0 ? SSL_get_error(m_sock->m_tls, size) : SSL_ERROR_NONE;

                if (size < 0 && m_sock->m_in && ssl_err == SSL_ERROR_SSL) {
                    m_sock->m_write_lock.unlock();
                    return openssl_error();
                }

                if (size > 0)
                    m_pos += size;
                if (m_sock->m_out)
                    m_out = m_sock->m_out;

                m_sock->m_write_lock.unlock();

                if ((size <= 0 && ssl_err != SSL_ERROR_ZERO_RETURN)
                    || (m_bytes > 0 && size > 0 && m_pos < m_bytes)) {
                    if (m_sock->m_in)
                        return next(read);

                    if (m_out)
                        return m_sock->m_stream->writeBuffer(m_out, next(readBuffer));

                    return next(readBuffer);
                }
            }

            if (m_pos == 0)
                return next(CALL_RETURN_NULL);

            m_data->resize(m_pos);
            m_retVal = m_data;

            if (g_ssldump)
                outLog(console_base::C_NOTICE, clean_string((const char*)m_data->data(), m_data->length()));

            if (m_out)
                return m_sock->m_stream->writeBuffer(m_out, next());
            else
                return next();
        }

        ON_STATE(AsyncRead, readBuffer)
        {
            m_out.Release();
            return m_sock->m_stream->readBuffer(-1, m_sock->m_in, next(read));
        }

        result_t lock(exlib::Locker& l, AsyncEvent* pThis)
        {
            return l.lock(pThis) ? 0 : CALL_E_PENDDING;
        }

    public:
        obj_ptr<TLSSocket> m_sock;
        int32_t m_bytes;
        obj_ptr<Buffer_base>& m_retVal;
        obj_ptr<Buffer> m_data;
        obj_ptr<Buffer_base> m_out;
        int32_t m_pos = 0;
    };

    result_t hr = is_ready();
    if (hr < 0)
        return hr;

    if (ac.isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    return (new AsyncRead(this, bytes, retVal, std::move(ac)))->post(0);
}

result_t TLSSocket::writeBuffer(Buffer_base* data, AsyncHandle ac)
{
    class AsyncWrite : public AsyncState {
    public:
        AsyncWrite(TLSSocket* sock, Buffer_base* data, AsyncHandle ac)
            : AsyncState(ac)
            , m_sock(sock)
            , m_data(data)
        {
            init(write);
        }

    public:
        ON_STATE(AsyncWrite, write)
        {
            int32_t len = m_data.As<Buffer>()->length();
            if (len == 0)
                return next();

            if (m_state == SSL_ERROR_NONE) {
                if (g_ssldump)
                    outLog(console_base::C_WARN, clean_string((const char*)m_data.As<Buffer>()->data(), len));
                return next();
            }

            m_sock->m_write_lock.lock();

            m_sock->m_out.Release();
            ERR_clear_error();
            m_state = SSL_get_error(m_sock->m_tls,
                SSL_write(m_sock->m_tls, m_data.As<Buffer>()->data(), len));
            if (m_state == SSL_ERROR_SSL) {
                m_sock->m_write_lock.unlock();
                return openssl_error();
            }

            if (m_sock->m_out) {
                m_out = m_sock->m_out;
                m_sock->m_write_lock.unlock();
                return m_sock->m_stream->writeBuffer(m_out, next(write));
            } else
                m_sock->m_write_lock.unlock();

            if (m_state == SSL_ERROR_NONE)
                return next(write);

            return Runtime::setError("write failed");
        }

        result_t lock(exlib::Locker& l, AsyncEvent* pThis)
        {
            return l.lock(pThis) ? 0 : CALL_E_PENDDING;
        }

    public:
        obj_ptr<TLSSocket> m_sock;
        obj_ptr<Buffer_base> m_data;
        obj_ptr<Buffer_base> m_out;
        int32_t m_state = SSL_ERROR_WANT_WRITE;
    };

    result_t hr = is_ready();
    if (hr < 0)
        return hr;

    if (ac.isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    return (new AsyncWrite(this, data, std::move(ac)))->post(0);
}

result_t TLSSocket::flush(AsyncHandle ac)
{
    return 0;
}

result_t TLSSocket::close(AsyncHandle ac)
{
    class AsyncClose : public AsyncState {
    public:
        AsyncClose(TLSSocket* sock, AsyncHandle ac)
            : AsyncState(ac)
            , m_sock(sock)
        {
            init(try_lock);
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

            ERR_clear_error();
            m_state = SSL_get_error(m_sock->m_tls, SSL_shutdown(m_sock->m_tls));
            if (m_sock->m_out)
                return m_sock->m_stream->writeBuffer(m_sock->m_out, next(write));

            return next();
        }

        result_t lock(exlib::Locker& l, AsyncEvent* pThis)
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

    if (ac.isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    return (new AsyncClose(this, std::move(ac)))->post(0);
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
