/*
 * RTCListener.h
 *
 *  Created on: Nov 18, 2026
 */

#pragma once

#ifndef RTC_STATIC
#define RTC_STATIC
#endif

#include "ifs/RTCListener.h"
#include "datachannel/include/rtc/rtc.hpp"

#include <atomic>
#include <memory>
#include <mutex>
#include <string>
#include <unordered_map>

namespace fibjs {

class RTCListener : public RTCListener_base {
public:
    ~RTCListener();

public:
    // RTCListener_base
    virtual result_t listen(int32_t port, exlib::string addr, obj_ptr<RTCListener_base>& retVal);
    virtual result_t address(obj_ptr<AddressType>& retVal);
    virtual result_t close(obj_ptr<RTCListener_base>& retVal);
    virtual result_t stop(obj_ptr<RTCListener_base>& retVal);
    virtual result_t get_certificate(v8::Local<v8::Object>& retVal);

    virtual result_t stop();

public:
    result_t create(RTCListenerOptions* options);

private:
    struct ParsedRequest {
        int32_t version = 1;
        std::string serverUfrag;
        std::string clientUfrag;
        std::string clientPwd;
        std::string remoteAddress;
        uint16_t remotePort = 0;
        std::string pendingKey;
    };

    struct ListenerState {
        std::mutex lock;
        int32_t active = 0;
        int32_t maxConnections = 1024;
        int32_t maxConnectionsPerAddress = 16;
        std::unordered_map<std::string, int32_t> perAddress;
    };

    struct SessionToken {
        std::shared_ptr<ListenerState> state;
        std::string address;
        std::atomic<bool> released { false };

        void release();
    };

private:
    result_t ensureCertificate();
    bool parseRequest(const rtc::IceUdpMuxRequest& request, ParsedRequest& parsed);
    obj_ptr<NObject> request_to_object(const ParsedRequest& request) const;
    std::string buildOffer(const ParsedRequest& request) const;
    std::shared_ptr<SessionToken> tryAccept(const std::string& address);
    bool authorize(const obj_ptr<NObject>& requestObject);
    void onStunRequest(const rtc::IceUdpMuxRequest& request);
    void handleRequest(ParsedRequest request);
    void emitError(const std::string& message);

    result_t closeInternal();
    void releaseMux();

private:
    std::shared_ptr<rtc::IceUdpMuxListener> m_mux;
    std::shared_ptr<ListenerState> m_state;
    v8::Global<v8::Function> m_authorize;

    int32_t m_port = 0;
    exlib::string m_address;
    std::atomic<bool> m_listening { false };
    bool m_holding = false;

    // A bound listener holds itself until close(): queued accept tasks capture
    // this, so the object must outlive them even when JavaScript drops its
    // reference to the listener.
    obj_ptr<RTCListener> m_self;

    std::string m_certPem;
    std::string m_keyPem;
    std::string m_keyPass;
    std::string m_fingerprint;
    int32_t m_maxMessageSize = 256 * 1024;
    int32_t m_maxConnections = 1024;
    int32_t m_maxConnectionsPerAddress = 16;

    // Deduplication of retransmitted binding requests. The mux callback runs on
    // the juice thread and posts the real work to the isolate; a request that
    // arrives while the connection is being built must not create a second PC.
    std::mutex m_pendingLock;
    std::unordered_map<std::string, int64_t> m_pending;
};

} /* namespace fibjs */
