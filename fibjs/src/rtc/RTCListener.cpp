/*
 * RTCListener.cpp
 *
 *  Created on: Nov 18, 2026
 */

#include "object.h"
#include "RTCListener.h"
#include "RTCPeerConnection.h"
#include "SimpleObject.h"
#include "utils.h"

#include <openssl/bio.h>
#include <openssl/ec.h>
#include <openssl/evp.h>
#include <openssl/pem.h>
#include <openssl/x509.h>

#include <chrono>
#include <cstdio>
#include <cstring>
#include <sstream>

namespace fibjs {

static const char* V2_UFRAG_PREFIX = "libp2p+webrtc+v2/";

static int64_t now_ms()
{
    return std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::steady_clock::now().time_since_epoch())
        .count();
}

static bool is_ice_char(char c)
{
    return (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') || (c >= '0' && c <= '9')
        || c == '+' || c == '/';
}

// RFC 8839 section 5.4: ufrag = 4*256ice-char, password = 22*256ice-char.
static bool is_ice_string(const std::string& value, size_t min_len)
{
    if (value.size() < min_len || value.size() > 256)
        return false;

    for (char c : value)
        if (!is_ice_char(c))
            return false;

    return true;
}

// The address comes from getnameinfo(NI_NUMERICHOST), but it is still checked
// before it is interpolated into an SDP that libdatachannel parses.
static bool is_numeric_address(const std::string& value)
{
    if (value.empty() || value.size() > 64)
        return false;

    for (char c : value)
        if (!((c >= '0' && c <= '9') || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F')
                || c == ':' || c == '.'))
            return false;

    return value.find(':') != std::string::npos || value.find('.') != std::string::npos;
}

result_t RTCListener_base::_new(RTCListenerOptions* options,
    obj_ptr<RTCListener_base>& retVal, v8::Local<v8::Object> This)
{
    obj_ptr<RTCListener> listener = new RTCListener();
    listener->wrap(This);
    result_t hr = listener->create(options);
    if (hr.failed())
        return hr;
    retVal = listener;

    return result_t();
}

RTCListener::~RTCListener()
{
    if (m_mux) {
        m_mux->stop();
        m_mux.reset();
    }

    Isolate* isolate = get_holder();
    if (m_holding && isolate && !isolate->is_terminating()) {
        m_holding = false;
        isolate_unref();
    } else {
        m_holding = false;
    }

    if (!m_authorize.IsEmpty() && isolate && !isolate->is_terminating())
        m_authorize.Reset();
}

void RTCListener::SessionToken::release()
{
    bool expected = false;
    if (!released.compare_exchange_strong(expected, true))
        return;

    std::lock_guard lock(state->lock);
    if (state->active > 0)
        --state->active;

    auto it = state->perAddress.find(address);
    if (it != state->perAddress.end()) {
        if (--it->second <= 0)
            state->perAddress.erase(it);
    }
}

result_t RTCListener::create(RTCListenerOptions* options)
{
    if (options) {
        m_certPem = std::string(options->certPem.value_or(""));
        m_keyPem = std::string(options->keyPem.value_or(""));
        m_keyPass = std::string(options->keyPass.value_or(""));
        m_maxMessageSize = options->maxMessageSize.value_or(256 * 1024);
        m_maxConnections = options->maxConnections.value_or(1024);
        m_maxConnectionsPerAddress = options->maxConnectionsPerAddress.value_or(16);

        // The struct field has already enforced that the value is a function.
        if (options->authorize.has_value())
            m_authorize.Reset(holder()->m_isolate, options->authorize.value());
    }

    if (m_maxMessageSize <= 0)
        return result_t::make(CALL_E_INVALID_DATA).with_type_name(errtype::kTypeError).with_code("ERR_INVALID_ARG_VALUE").with_message("maxMessageSize must be positive");

    if (m_maxConnections < 0)
        return result_t::make(CALL_E_INVALID_DATA).with_type_name(errtype::kTypeError).with_code("ERR_INVALID_ARG_VALUE").with_message("maxConnections must not be negative");

    if (m_maxConnectionsPerAddress < 0)
        return result_t::make(CALL_E_INVALID_DATA).with_type_name(errtype::kTypeError).with_code("ERR_INVALID_ARG_VALUE").with_message("maxConnectionsPerAddress must not be negative");

    result_t hr = ensureCertificate();
    if (hr.failed())
        return hr;

    m_state = std::make_shared<ListenerState>();
    m_state->maxConnections = m_maxConnections;
    m_state->maxConnectionsPerAddress = m_maxConnectionsPerAddress;

    return result_t();
}

static std::string fingerprint_from_x509(X509* x509)
{
    unsigned char digest[EVP_MAX_MD_SIZE];
    unsigned int len = 0;
    if (!X509_digest(x509, EVP_sha256(), digest, &len))
        return std::string();

    std::string fingerprint;
    char buf[4];
    for (unsigned int i = 0; i < len; ++i) {
        if (i)
            fingerprint += ':';
        snprintf(buf, sizeof(buf), "%02X", digest[i]);
        fingerprint += buf;
    }

    return fingerprint;
}

static std::string x509_to_pem(X509* x509)
{
    BIO* bio = BIO_new(BIO_s_mem());
    if (!bio)
        return std::string();

    std::string pem;
    if (PEM_write_bio_X509(bio, x509)) {
        char* data = nullptr;
        long len = BIO_get_mem_data(bio, &data);
        if (data && len > 0)
            pem.assign(data, len);
    }

    BIO_free(bio);
    return pem;
}

static std::string pkey_to_pem(EVP_PKEY* pkey)
{
    BIO* bio = BIO_new(BIO_s_mem());
    if (!bio)
        return std::string();

    std::string pem;
    if (PEM_write_bio_PrivateKey(bio, pkey, nullptr, nullptr, 0, nullptr, nullptr)) {
        char* data = nullptr;
        long len = BIO_get_mem_data(bio, &data);
        if (data && len > 0)
            pem.assign(data, len);
    }

    BIO_free(bio);
    return pem;
}

result_t RTCListener::ensureCertificate()
{
    if (m_certPem.empty() != m_keyPem.empty())
        return result_t::make(CALL_E_INVALID_DATA).with_type_name(errtype::kTypeError).with_code("ERR_INVALID_ARG_VALUE").with_message("certPem and keyPem must be given together");

    if (!m_certPem.empty()) {
        BIO* bio = BIO_new_mem_buf(m_certPem.data(), (int)m_certPem.size());
        X509* x509 = bio ? PEM_read_bio_X509(bio, nullptr, nullptr, nullptr) : nullptr;
        if (x509) {
            m_fingerprint = fingerprint_from_x509(x509);
            X509_free(x509);
        }
        if (bio)
            BIO_free(bio);

        return m_fingerprint.empty()
            ? result_t::make(CALL_E_INVALID_DATA).with_type_name(errtype::kError).with_code("ERR_RTC_ERROR").with_message("Unable to read the listener certificate")
            : result_t();
    }

    EVP_PKEY_CTX* key_ctx = EVP_PKEY_CTX_new_id(EVP_PKEY_EC, nullptr);
    if (!key_ctx)
        return result_t::make(CALL_E_INTERNAL).with_type_name(errtype::kError).with_code("ERR_RTC_ERROR").with_message("Unable to generate the listener key");

    EVP_PKEY* pkey = nullptr;
    X509* x509 = nullptr;
    result_t hr;

    if (EVP_PKEY_keygen_init(key_ctx) <= 0
        || EVP_PKEY_CTX_set_ec_paramgen_curve_nid(key_ctx, NID_X9_62_prime256v1) <= 0
        || EVP_PKEY_keygen(key_ctx, &pkey) <= 0) {
        hr = result_t::make(CALL_E_INTERNAL).with_type_name(errtype::kError).with_code("ERR_RTC_ERROR").with_message("Unable to generate the listener key");
        goto done;
    }

    x509 = X509_new();
    if (!x509) {
        hr = result_t::make(CALL_E_INTERNAL).with_type_name(errtype::kError).with_code("ERR_RTC_ERROR").with_message("Unable to allocate the listener certificate");
        goto done;
    }

    if (!X509_set_version(x509, 2)
        || !ASN1_INTEGER_set(X509_get_serialNumber(x509), 1)
        || !X509_gmtime_adj(X509_getm_notBefore(x509), 0)
        || !X509_gmtime_adj(X509_getm_notAfter(x509), 365L * 24 * 3600)
        || !X509_set_pubkey(x509, pkey)) {
        hr = result_t::make(CALL_E_INTERNAL).with_type_name(errtype::kError).with_code("ERR_RTC_ERROR").with_message("Unable to build the listener certificate");
        goto done;
    }

    {
        X509_NAME* name = X509_get_subject_name(x509);
        if (!name
            || !X509_NAME_add_entry_by_txt(name, "CN", MBSTRING_ASC,
                (const unsigned char*)"fibjs-webrtc", -1, -1, 0)
            || !X509_set_issuer_name(x509, name)
            || !X509_sign(x509, pkey, EVP_sha256())) {
            hr = result_t::make(CALL_E_INTERNAL).with_type_name(errtype::kError).with_code("ERR_RTC_ERROR").with_message("Unable to sign the listener certificate");
            goto done;
        }
    }

    m_certPem = x509_to_pem(x509);
    m_keyPem = pkey_to_pem(pkey);
    m_fingerprint = fingerprint_from_x509(x509);

    hr = (m_certPem.empty() || m_keyPem.empty() || m_fingerprint.empty())
        ? result_t::make(CALL_E_INTERNAL).with_type_name(errtype::kError).with_code("ERR_RTC_ERROR").with_message("Unable to serialize the listener certificate")
        : result_t();

done:
    if (x509)
        X509_free(x509);
    if (pkey)
        EVP_PKEY_free(pkey);
    EVP_PKEY_CTX_free(key_ctx);

    return hr;
}

bool RTCListener::parseRequest(const rtc::IceUdpMuxRequest& request, ParsedRequest& parsed)
{
    if (request.localUfrag.empty() || request.remoteUfrag.empty() || request.remoteAddress.empty())
        return false;

    std::string server_ufrag = request.localUfrag;
    std::string client_ufrag = request.remoteUfrag;

    if (!is_ice_string(server_ufrag, 4) || !is_ice_string(client_ufrag, 4))
        return false;

    if (!is_numeric_address(request.remoteAddress) || request.remotePort == 0)
        return false;

    if (server_ufrag.compare(0, strlen(V2_UFRAG_PREFIX), V2_UFRAG_PREFIX) == 0) {
        std::string client_pwd = server_ufrag.substr(strlen(V2_UFRAG_PREFIX));
        if (!is_ice_string(client_pwd, 22) || !is_ice_string(server_ufrag, 22))
            return false;

        parsed.version = 2;
        parsed.clientPwd = std::move(client_pwd);
    } else {
        // v1 reuses both ufrags as ICE passwords, so they must also satisfy the
        // password length bound. An unknown prefix is treated as v1.
        if (!is_ice_string(server_ufrag, 22) || !is_ice_string(client_ufrag, 22))
            return false;

        parsed.version = 1;
        parsed.clientPwd = client_ufrag;
    }

    parsed.serverUfrag = std::move(server_ufrag);
    parsed.clientUfrag = std::move(client_ufrag);
    parsed.remoteAddress = request.remoteAddress;
    parsed.remotePort = request.remotePort;

    return true;
}

std::string RTCListener::buildOffer(const ParsedRequest& request) const
{
    bool ipv6 = request.remoteAddress.find(':') != std::string::npos;
    const char* family = ipv6 ? "IP6" : "IP4";

    std::string fingerprint;
    for (int i = 0; i < 32; ++i) {
        if (i)
            fingerprint += ':';
        fingerprint += "00";
    }

    std::ostringstream sdp;
    sdp << "v=0\r\n"
        << "o=- 0 0 IN " << family << " " << request.remoteAddress << "\r\n"
        << "s=-\r\n"
        << "c=IN " << family << " " << request.remoteAddress << "\r\n"
        << "t=0 0\r\n"
        << "a=ice-options:ice2,trickle\r\n"
        << "m=application " << request.remotePort << " UDP/DTLS/SCTP webrtc-datachannel\r\n"
        << "a=mid:0\r\n"
        << "a=setup:active\r\n"
        << "a=ice-ufrag:" << request.clientUfrag << "\r\n"
        << "a=ice-pwd:" << request.clientPwd << "\r\n"
        << "a=fingerprint:sha-256 " << fingerprint << "\r\n"
        << "a=sctp-port:5000\r\n"
        << "a=max-message-size:" << m_maxMessageSize << "\r\n"
        << "a=candidate:1467250027 1 UDP 1467250027 " << request.remoteAddress
        << " " << request.remotePort << " typ host\r\n"
        << "a=end-of-candidates\r\n";

    return sdp.str();
}

std::shared_ptr<RTCListener::SessionToken> RTCListener::tryAccept(const std::string& address)
{
    std::lock_guard lock(m_state->lock);

    if (m_state->maxConnections > 0 && m_state->active >= m_state->maxConnections)
        return nullptr;

    if (m_state->maxConnectionsPerAddress > 0) {
        auto it = m_state->perAddress.find(address);
        if (it != m_state->perAddress.end()
            && it->second >= m_state->maxConnectionsPerAddress)
            return nullptr;
    }

    ++m_state->active;
    ++m_state->perAddress[address];

    auto token = std::make_shared<SessionToken>();
    token->state = m_state;
    token->address = address;

    return token;
}

void RTCListener::emitError(const std::string& message)
{
    obj_ptr<NObject> ev = new NObject();
    ev->add("message", exlib::string(message));
    _emit("error", ev);
}

bool RTCListener::authorize(const obj_ptr<NObject>& requestObject)
{
    if (m_authorize.IsEmpty())
        return true;

    Isolate* isolate = holder();
    v8::Local<v8::Function> fn = v8::Local<v8::Function>::New(isolate->m_isolate, m_authorize);
    if (fn.IsEmpty())
        return false;

    v8::Local<v8::Value> requestValue;
    requestObject->valueOf(requestValue);

    v8::Local<v8::Value> argv[] = { requestValue };
    v8::TryCatch try_catch(isolate->m_isolate);
    v8::MaybeLocal<v8::Value> ret = fn->Call(isolate->context(),
        v8::Undefined(isolate->m_isolate), 1, argv);

    if (try_catch.HasCaught()) {
        emitError(std::string("authorize: ") + std::string(GetException(try_catch.Exception(), false, false)));
        return false;
    }

    v8::Local<v8::Value> value;
    if (!ret.ToLocal(&value) || value.IsEmpty())
        return false;

    // authorize is synchronous: a promise is rejected instead of being treated
    // as a truthy acceptance.
    if (value->IsPromise() || !value->BooleanValue(isolate->m_isolate))
        return false;

    return true;
}

obj_ptr<NObject> RTCListener::request_to_object(const ParsedRequest& request) const
{
    obj_ptr<NObject> obj = new NObject();
    obj->add("version", request.version);
    obj->add("serverUfrag", exlib::string(request.serverUfrag));
    obj->add("clientUfrag", exlib::string(request.clientUfrag));
    obj->add("clientPwd", exlib::string(request.clientPwd));
    obj->add("remoteAddress", exlib::string(request.remoteAddress));
    obj->add("remotePort", (int32_t)request.remotePort);
    obj->add("localPort", m_port);
    return obj;
}

void RTCListener::onStunRequest(const rtc::IceUdpMuxRequest& request)
{
    if (!m_listening)
        return;

    ParsedRequest parsed;
    if (!parseRequest(request, parsed))
        return;

    std::string key = parsed.serverUfrag;
    key += '\x1f';
    key += parsed.clientUfrag;
    key += '\x1f';
    key += parsed.remoteAddress;
    key += ':';
    key += std::to_string(parsed.remotePort);

    int64_t now = now_ms();
    {
        std::lock_guard lock(m_pendingLock);

        for (auto it = m_pending.begin(); it != m_pending.end();) {
            if (it->second <= now)
                it = m_pending.erase(it);
            else
                ++it;
        }

        auto it = m_pending.find(key);
        if (it != m_pending.end() && it->second > now)
            return;

        if (m_pending.size() >= 1024)
            return;

        m_pending[key] = now + 5000;
    }

    parsed.pendingKey = key;

    Isolate* isolate = get_holder();
    if (!isolate || isolate->is_terminating())
        return;

    isolate->sync([this, parsed = std::move(parsed)]() mutable {
        handleRequest(std::move(parsed));
    });
}

void RTCListener::handleRequest(ParsedRequest request)
{
    if (!m_listening || !m_state)
        return;

    auto erasePending = [this](const std::string& key) {
        if (key.empty())
            return;
        std::lock_guard lock(m_pendingLock);
        m_pending.erase(key);
    };

    obj_ptr<NObject> requestObject = request_to_object(request);

    if (!authorize(requestObject))
        return;

    auto token = tryAccept(request.remoteAddress);
    if (!token) {
        // A rejected request must not stay in the deduplication map: the peer
        // retries and is accepted as soon as a slot frees.
        erasePending(request.pendingKey);
        return;
    }

    std::string offer;
    try {
        offer = buildOffer(request);
    } catch (std::exception& e) {
        token->release();
        erasePending(request.pendingKey);
        emitError(e.what());
        return;
    }

    rtc::Configuration config;
    config.enableIceUdpMux = true;
    config.disableFingerprintVerification = true;
    config.disableAutoNegotiation = true;
    config.portRangeBegin = config.portRangeEnd = (uint16_t)m_port;
    config.maxMessageSize = m_maxMessageSize;
    config.certificatePemFile = m_certPem;
    config.keyPemFile = m_keyPem;
    if (!m_keyPass.empty())
        config.keyPemPass = m_keyPass;

    rtc::LocalDescriptionInit localInit;
    localInit.iceUfrag = request.serverUfrag;
    localInit.icePwd = request.serverUfrag;

    obj_ptr<RTCPeerConnection> pc;
    try {
        auto peer = std::make_shared<rtc::PeerConnection>(config);
        pc = new RTCPeerConnection();
        pc->holder(holder());

        result_t hr = pc->accept(std::move(peer), std::move(localInit), offer,
            [token]() { token->release(); });
        if (hr.failed()) {
            token->release();
            erasePending(request.pendingKey);
            std::string message = "Unable to accept the incoming connection";
            if (hr.payload() && !hr.payload()->message.empty())
                message += ": " + std::string(hr.payload()->message);
            emitError(message);
            return;
        }
    } catch (std::exception& e) {
        token->release();
        erasePending(request.pendingKey);
        emitError(e.what());
        return;
    }

    obj_ptr<NObject> ev = new NObject();
    ev->add("peerConnection", pc);
    ev->add("request", requestObject);

    _emit("connection", ev);
}

result_t RTCListener::listen(int32_t port, exlib::string addr, obj_ptr<RTCListener_base>& retVal)
{
    if (m_listening)
        return result_t::make(CALL_E_INVALID_CALL).with_type_name(errtype::kError).with_code("ERR_INVALID_STATE").with_message("RTCListener is already listening");

    if (port < 1 || port > 65535)
        return result_t::make(CALL_E_INVALID_DATA).with_type_name(errtype::kTypeError).with_code("ERR_INVALID_ARG_VALUE").with_message("port must be between 1 and 65535");

    try {
        m_mux = std::make_shared<rtc::IceUdpMuxListener>((uint16_t)port,
            addr.empty() ? std::optional<std::string>() : std::optional<std::string>(std::string(addr)));
    } catch (std::exception& e) {
        m_mux.reset();
        return result_t::make(CALL_E_INVALID_CALL).with_type_name(errtype::kError).with_code("ERR_SOCKET_BIND_FAILED").with_message(e.what());
    }

    m_port = port;
    m_address = addr;
    m_listening = true;

    Isolate* isolate = holder();
    m_mux->OnUnhandledStunRequest([this](rtc::IceUdpMuxRequest request) {
        onStunRequest(request);
    });

    if (!m_holding) {
        m_holding = true;

        // The released hold may still be pending in the isolate queue after a
        // close(), so only take the self-reference when it is really cleared.
        if (!m_self)
            m_self = this;

        isolate_ref();
    }

    retVal = this;

    return result_t();
}

result_t RTCListener::address(obj_ptr<AddressType>& retVal)
{
    if (!m_listening)
        return result_t::make(CALL_E_INVALID_CALL).with_type_name(errtype::kError).with_code("ERR_INVALID_STATE").with_message("RTCListener is not listening");

    obj_ptr<AddressType> addr = new AddressType();
    addr->address = m_address.empty() ? "::" : m_address;
    addr->family = (m_address.empty() || m_address.find(':') != exlib::string::npos) ? "IPv6" : "IPv4";
    addr->port = m_port;
    retVal = addr;

    return result_t();
}

result_t RTCListener::closeInternal()
{
    if (m_listening) {
        m_listening = false;

        if (m_mux) {
            m_mux->stop();
            m_mux.reset();
        }

        std::lock_guard lock(m_pendingLock);
        m_pending.clear();
    }

    if (m_holding) {
        m_holding = false;

        Isolate* isolate = holder();
        if (isolate && !isolate->is_terminating()) {
            // Queue the release behind already posted accept tasks: they observe
            // m_listening == false and return before the listener is freed.
            // A listen() made before this task runs keeps the hold alive.
            isolate->sync([this]() {
                if (m_listening || m_holding)
                    return;

                obj_ptr<RTCListener> hold = this;
                m_self.Release();
                isolate_unref();
            });
        } else {
            obj_ptr<RTCListener> hold = this;
            m_self.Release();
            isolate_unref();
        }
    }

    return result_t();
}

result_t RTCListener::close(obj_ptr<RTCListener_base>& retVal)
{
    result_t hr = closeInternal();
    if (hr.failed())
        return hr;

    retVal = this;
    return result_t();
}

result_t RTCListener::stop(obj_ptr<RTCListener_base>& retVal)
{
    return close(retVal);
}

result_t RTCListener::stop()
{
    return closeInternal();
}

result_t RTCListener::get_certificate(v8::Local<v8::Object>& retVal)
{
    Isolate* isolate = holder();
    v8::Local<v8::Context> context = isolate->context();
    v8::Local<v8::Object> obj = v8::Object::New(isolate->m_isolate);

    obj->Set(context, isolate->NewString("certPem"),
                isolate->NewString(m_certPem.c_str(), (int)m_certPem.size()))
        .FromMaybe(false);
    obj->Set(context, isolate->NewString("keyPem"),
                isolate->NewString(m_keyPem.c_str(), (int)m_keyPem.size()))
        .FromMaybe(false);
    obj->Set(context, isolate->NewString("fingerprint"),
                isolate->NewString(m_fingerprint.c_str(), (int)m_fingerprint.size()))
        .FromMaybe(false);

    retVal = obj;
    return result_t();
}

} /* namespace fibjs */
