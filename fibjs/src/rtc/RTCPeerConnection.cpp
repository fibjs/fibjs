/*
 * RTCPeerConnection.cpp
 *
 *  Created on: Apr 27, 2024
 */

#include "object.h"
#include "RTCPeerConnection.h"
#include "RTCDataChannel.h"
#include "RTCSessionDescription.h"
#include "RTCIceCandidate.h"
#include "SimpleObject.h"
#include "Fiber.h"
#include "ifs/rtc.h"

#ifndef JUICE_STATIC
#define JUICE_STATIC
#endif
#include "juice/include/juice/juice.h"

namespace fibjs {
DECLARE_MODULE(rtc);

// class _init_rtc {
// public:
//     _init_rtc()
//     {
//         rtc::InitLogger(rtc::LogLevel::Verbose);
//     }
// } s_init_rtc;

result_t rtc_base::listen(exlib::string bind_address, int32_t local_port, v8::Local<v8::Function> cb)
{
    struct cb_data {
        exlib::string local_ufrag;
        exlib::string remote_ufrag;
        exlib::string address;
        uint16_t port;
    };

    Isolate* isolate = Isolate::current(cb);
    if (isolate->m_id != 1)
        return Runtime::setError("rtc.bind() can only be called in the main isolate");

    static v8::Global<v8::Function> s_cb_global;
    s_cb_global.Reset(isolate->m_isolate, cb);
    const char* bind_address_ = bind_address.empty() ? nullptr : bind_address.c_str();

    int ret = juice_mux_listen(bind_address_, local_port, [](const juice_mux_incoming_t* incoming_info, void* user_data) {
            cb_data* data_ = new cb_data();

            data_->local_ufrag = incoming_info->local_ufrag;
            data_->remote_ufrag = incoming_info->remote_ufrag;
            data_->address = incoming_info->address;
            data_->port = incoming_info->port;

            syncCall((Isolate*)user_data, [](cb_data* data_) {
                Isolate* isolate = Isolate::current();
                JSFiber::EnterJsScope s(NULL, true);

                v8::Local<v8::Function> cb = v8::Local<v8::Function>::New(isolate->m_isolate, s_cb_global);
                v8::Local<v8::Object> data = v8::Object::New(isolate->m_isolate);

                data->Set(isolate->context(), isolate->NewString("local_ufrag"), isolate->NewString(data_->local_ufrag)).Check();
                data->Set(isolate->context(), isolate->NewString("remote_ufrag"), isolate->NewString(data_->remote_ufrag)).Check();
                data->Set(isolate->context(), isolate->NewString("address"), isolate->NewString(data_->address)).Check();
                data->Set(isolate->context(), isolate->NewString("port"), v8::Number::New(isolate->m_isolate, data_->port)).Check();
                delete data_;

                v8::Local<v8::Value> argv[] = { data };

                v8::Local<v8::Value> result = cb->Call(isolate->context(), v8::Undefined(isolate->m_isolate), 1, argv).FromMaybe(v8::Local<v8::Value>());

                return 0; }, data_); }, isolate);

    if (ret == -1)
        return Runtime::setError("rtc.listen() need to be called before RTCPeerConnection object is created");
    else if (ret < 0)
        return Runtime::setError("rtc.listen() failed to bind to the specified port");

    isolate->Ref();

    return 0;
}

result_t rtc_base::listen(int32_t local_port, v8::Local<v8::Function> cb)
{
    return listen("", local_port, cb);
}

result_t rtc_base::stopListen(exlib::string bind_address, int32_t local_port)
{
    Isolate* isolate = Isolate::current();
    if (isolate->m_id != 1)
        return Runtime::setError("rtc.stopListen() can only be called in the main isolate");

    int ret = juice_mux_stop_listen(bind_address.empty() ? nullptr : bind_address.c_str(), local_port);
    if (ret == 0)
        isolate->Unref();

    return 0;
}

result_t rtc_base::stopListen(int32_t local_port)
{
    return stopListen("", local_port);
}

result_t RTCPeerConnection_base::_new(v8::Local<v8::Object> options,
    obj_ptr<RTCPeerConnection_base>& retVal, v8::Local<v8::Object> This)
{
    obj_ptr<RTCPeerConnection> peer = new RTCPeerConnection();
    peer->wrap(This);
    result_t hr = peer->create(options);
    if (hr < 0)
        return hr;
    retVal = peer;

    return 0;
}

result_t RTCSessionDescription_base::_new(v8::Local<v8::Object> description,
    obj_ptr<RTCSessionDescription_base>& retVal, v8::Local<v8::Object> This)
{
    Isolate* isolate = Isolate::current(description);

    result_t hr;
    exlib::string type, sdp;

    hr = GetConfigValue(isolate, description, "type", type, true);
    if (hr < 0)
        return hr;

    hr = GetConfigValue(isolate, description, "sdp", sdp, true);
    if (hr < 0)
        return hr;

    try {
        retVal = new RTCSessionDescription(sdp, type);
    } catch (std::exception& e) {
        return Runtime::setError(e.what());
    }

    return 0;
}

inline const char* connection_state_str(rtc::PeerConnection::State state)
{
    switch (state) {
    case rtc::PeerConnection::State::New:
        return "new";
    case rtc::PeerConnection::State::Connecting:
        return "connecting";
    case rtc::PeerConnection::State::Connected:
        return "connected";
    case rtc::PeerConnection::State::Disconnected:
        return "disconnected";
    case rtc::PeerConnection::State::Failed:
        return "failed";
    case rtc::PeerConnection::State::Closed:
        return "closed";
    default:
        return "unknown";
    }
}

inline const char* ice_connection_state_str(rtc::PeerConnection::IceState state)
{
    switch (state) {
    case rtc::PeerConnection::IceState::New:
        return "new";
    case rtc::PeerConnection::IceState::Checking:
        return "checking";
    case rtc::PeerConnection::IceState::Connected:
        return "connected";
    case rtc::PeerConnection::IceState::Completed:
        return "completed";
    case rtc::PeerConnection::IceState::Failed:
        return "failed";
    case rtc::PeerConnection::IceState::Disconnected:
        return "disconnected";
    case rtc::PeerConnection::IceState::Closed:
        return "closed";
    default:
        return "unknown";
    }
}

inline const char* ice_gathering_state_str(rtc::PeerConnection::GatheringState state)
{
    switch (state) {
    case rtc::PeerConnection::GatheringState::New:
        return "new";
    case rtc::PeerConnection::GatheringState::InProgress:
        return "in-progress";
    case rtc::PeerConnection::GatheringState::Complete:
        return "complete";
    default:
        return "unknown";
    }
}

inline const char* signaling_state_str(rtc::PeerConnection::SignalingState state)
{
    switch (state) {
    case rtc::PeerConnection::SignalingState::Stable:
        return "stable";
    case rtc::PeerConnection::SignalingState::HaveLocalOffer:
        return "have-local-offer";
    case rtc::PeerConnection::SignalingState::HaveRemoteOffer:
        return "have-remote-offer";
    case rtc::PeerConnection::SignalingState::HaveLocalPranswer:
        return "have-local-pranswer";
    case rtc::PeerConnection::SignalingState::HaveRemotePranswer:
        return "have-remote-pranswer";
    default:
        return "unknown";
    }
}

inline exlib::string candidate_type_str(rtc::Candidate::Type type)
{
    switch (type) {
    case rtc::Candidate::Type::Host:
        return "host";
    case rtc::Candidate::Type::ServerReflexive:
        return "srflx";
    case rtc::Candidate::Type::PeerReflexive:
        return "prflx";
    case rtc::Candidate::Type::Relayed:
        return "relay";
    default:
        return "unknown";
    }
}

obj_ptr<NObject> RTCPeerConnection::description_to_object(rtc::Description description)
{
    NObject* obj_description = new NObject();

    switch (description.type()) {
    case rtc::Description::Type::Unspec:
        obj_description->add("type", "unspec");
        break;
    case rtc::Description::Type::Offer:
        obj_description->add("type", "offer");
        break;
    case rtc::Description::Type::Answer:
        obj_description->add("type", "answer");
        break;
    case rtc::Description::Type::Pranswer:
        obj_description->add("type", "pranswer");
        break;
    case rtc::Description::Type::Rollback:
        obj_description->add("type", "rollback");
        break;
    default:
        obj_description->add("type", "unknown");
        break;
    }

    obj_description->add("sdp", description.generateSdp());

    auto ufrag = description.iceUfrag();
    if (ufrag.has_value())
        obj_description->add("usernameFragment", ufrag.value());

    return obj_description;
}

RTCPeerConnection::~RTCPeerConnection()
{
    close();
}

result_t RTCPeerConnection::createDataChannel(exlib::string label, v8::Local<v8::Object> options, obj_ptr<RTCDataChannel_base>& retVal)
{
    Isolate* isolate = holder();
    result_t hr;
    rtc::DataChannelInit init;

    bool ordered;
    hr = GetConfigValue(isolate, options, "ordered", ordered, true);
    if (hr != CALL_E_PARAMNOTOPTIONAL) {
        if (hr < 0)
            return hr;
        init.reliability.unordered = !ordered;
    }

    int32_t maxPacketLifeTime;
    hr = GetConfigValue(isolate, options, "maxPacketLifeTime", maxPacketLifeTime, true);
    if (hr != CALL_E_PARAMNOTOPTIONAL) {
        if (hr < 0)
            return hr;
        init.reliability.maxPacketLifeTime = std::chrono::milliseconds(maxPacketLifeTime);
    }

    int32_t maxRetransmits;
    hr = GetConfigValue(isolate, options, "maxRetransmits", maxRetransmits, true);
    if (hr != CALL_E_PARAMNOTOPTIONAL) {
        if (hr < 0)
            return hr;
        init.reliability.maxRetransmits = maxRetransmits;
    }

    exlib::string protocol;
    hr = GetConfigValue(isolate, options, "protocol", protocol, true);
    if (hr != CALL_E_PARAMNOTOPTIONAL) {
        if (hr < 0)
            return hr;
        init.protocol = protocol;
    }

    bool negotiated;
    hr = GetConfigValue(isolate, options, "negotiated", negotiated, true);
    if (hr != CALL_E_PARAMNOTOPTIONAL) {
        if (hr < 0)
            return hr;
        init.negotiated = negotiated;
    }

    int32_t id;
    hr = GetConfigValue(isolate, options, "id", id, true);
    if (hr != CALL_E_PARAMNOTOPTIONAL) {
        if (hr < 0)
            return hr;
        init.id = id;
    }

    obj_ptr<RTCDataChannel> dc;
    try {
        dc = new RTCDataChannel(m_peerConnection->createDataChannel(label, init));
        retVal = dc;
    } catch (std::exception& e) {
        return Runtime::setError(e.what());
    }

    m_lock.lock();
    if (m_peerConnection->state() != rtc::PeerConnection::State::Connected)
        m_dataChannels.emplace_back(dc);
    m_lock.unlock();

    return 0;
}

result_t RTCPeerConnection::setLocalDescription(AsyncEvent* ac)
{
    return 0;
}

result_t RTCPeerConnection::setLocalDescription(RTCSessionDescription_base* description, AsyncEvent* ac)
{
    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    RTCSessionDescription* desc = static_cast<RTCSessionDescription*>(description);

    if (desc->m_desc.type() == rtc::Description::Type::Offer) {
        try {
            m_peerConnection->setLocalDescription(rtc::Description::Type::Offer);
        } catch (std::exception& e) {
            return Runtime::setError(e.what());
        }
    }

    return 0;
}

result_t RTCPeerConnection::setRemoteDescription(RTCSessionDescription_base* description, AsyncEvent* ac)
{
    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);
    else if (ac->isPost()) {
        m_self = new ValueHolder(wrap());
        isolate_ref();

        return 0;
    }

    RTCSessionDescription* desc = static_cast<RTCSessionDescription*>(description);
    try {
        m_peerConnection->setRemoteDescription(desc->m_desc);
        ac->setPost();
    } catch (std::exception& e) {
        return Runtime::setError(e.what());
    }

    return 0;
}

result_t RTCPeerConnection::addIceCandidate(RTCIceCandidate_base* candidate, AsyncEvent* ac)
{
    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    RTCIceCandidate* cand = static_cast<RTCIceCandidate*>(candidate);
    try {
        m_peerConnection->addRemoteCandidate(cand->m_candidate);
    } catch (std::exception& e) {
        return Runtime::setError(e.what());
    }

    return 0;
}

result_t RTCPeerConnection::createOffer(v8::Local<v8::Object> options, Variant& retVal, AsyncEvent* ac)
{
    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    m_lock.lock();
    if (m_offer_description) {
        retVal = m_offer_description;
        m_lock.unlock();
        return 0;
    }
    m_offers.emplace_back(retVal, ac);
    m_lock.unlock();

    return CALL_E_PENDDING;
}

result_t RTCPeerConnection::createAnswer(v8::Local<v8::Object> options, Variant& retVal, AsyncEvent* ac)
{
    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    m_lock.lock();
    if (m_answer_description) {
        retVal = m_answer_description;
        m_lock.unlock();
        return 0;
    }
    m_answers.emplace_back(retVal, ac);
    m_lock.unlock();

    return CALL_E_PENDDING;
}

result_t RTCPeerConnection::getStats(obj_ptr<NMap>& retVal, AsyncEvent* ac)
{
    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    obj_ptr<NMap> stats = new NMap();

    rtc::Candidate local, remote;
    size_t bytesSent, bytesReceived;
    std::chrono::milliseconds rtt;
    std::chrono::milliseconds timestamp;

    m_peerConnection->getSelectedCandidatePair(&local, &remote);

    bytesSent = m_peerConnection->bytesSent();
    bytesReceived = m_peerConnection->bytesReceived();
    rtt = m_peerConnection->rtt().value_or(std::chrono::milliseconds(0));
    timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch());

    obj_ptr<NObject> stat;

    exlib::string local_id = "RTCIceCandidate_" + m_local_id;
    stat = new NObject();
    stats->add(local_id, stat);

    stat->add("id", local_id);
    stat->add("type", "localcandidate");
    stat->add("timestamp", (double)timestamp.count());
    stat->add("candidateType", candidate_type_str(local.type()));
    stat->add("ip", local.address().value());
    stat->add("port", local.port().value());

    exlib::string remote_id = "RTCIceCandidate_" + m_remote_id;
    stat = new NObject();
    stats->add(remote_id, stat);

    stat->add("id", remote_id);
    stat->add("type", "remotecandidate");
    stat->add("timestamp", (double)timestamp.count());
    stat->add("candidateType", candidate_type_str(remote.type()));
    stat->add("ip", remote.address().value());
    stat->add("port", remote.port().value());

    exlib::string peer_id = "RTCIceCandidatePair_" + m_local_id + "_" + m_remote_id;
    stat = new NObject();
    stats->add(peer_id, stat);

    stat->add("id", peer_id);
    stat->add("type", "candidate-pair");
    stat->add("timestamp", (double)timestamp.count());
    stat->add("localCandidateId", local_id);
    stat->add("remoteCandidateId", remote_id);
    stat->add("state", "succeeded");
    stat->add("nominated", true);
    stat->add("writable", true);
    stat->add("bytesSent", (double)bytesSent);
    stat->add("bytesReceived", (double)bytesReceived);
    stat->add("totalRoundTripTime", (double)rtt.count());
    stat->add("currentRoundTripTime", (double)rtt.count());

    exlib::string transport_id = "RTCTransport_0_1";
    stat = new NObject();
    stats->add(transport_id, stat);

    stat->add("id", transport_id);
    stat->add("type", "transport");
    stat->add("timestamp", (double)timestamp.count());
    stat->add("bytesSent", (double)bytesSent);
    stat->add("bytesReceived", (double)bytesReceived);
    stat->add("dtlsState", "connected");
    stat->add("selectedCandidatePairId", peer_id);
    stat->add("selectedCandidatePairChanges", 1);

    retVal = stats;

    return 0;
}

result_t RTCPeerConnection::close()
{
    m_peerConnection->resetCallbacks();
    m_peerConnection->close();

    return 0;
}

result_t RTCPeerConnection::get_connectionState(exlib::string& retVal)
{
    retVal = connection_state_str(m_peerConnection->state());
    return 0;
}

result_t RTCPeerConnection::get_iceConnectionState(exlib::string& retVal)
{
    retVal = ice_connection_state_str(m_peerConnection->iceState());
    return 0;
}

result_t RTCPeerConnection::get_iceGatheringState(exlib::string& retVal)
{
    retVal = ice_gathering_state_str(m_peerConnection->gatheringState());
    return 0;
}

result_t RTCPeerConnection::get_localDescription(v8::Local<v8::Object>& retVal)
{
    auto desc = m_peerConnection->localDescription();
    if (desc.has_value())
        return description_to_object(desc.value())->valueOf(retVal);

    return 0;
}

result_t RTCPeerConnection::get_remoteDescription(v8::Local<v8::Object>& retVal)
{
    auto desc = m_peerConnection->remoteDescription();
    if (desc.has_value())
        return description_to_object(desc.value())->valueOf(retVal);

    return 0;
}

result_t RTCPeerConnection::get_remoteFingerprint(v8::Local<v8::Object>& retVal)
{
    Isolate* isolate = holder();
    v8::Local<v8::Context> context = isolate->context();
    v8::Local<v8::Object> obj = v8::Object::New(isolate->m_isolate);

    rtc::CertificateFingerprint fingerprint = m_peerConnection->remoteFingerprint();

    switch (fingerprint.algorithm) {
    case rtc::CertificateFingerprint::Algorithm::Sha1:
        obj->Set(context, isolate->NewString("algorithm"), isolate->NewString("SHA-1")).Check();
        break;
    case rtc::CertificateFingerprint::Algorithm::Sha224:
        obj->Set(context, isolate->NewString("algorithm"), isolate->NewString("SHA-224")).Check();
        break;
    case rtc::CertificateFingerprint::Algorithm::Sha256:
        obj->Set(context, isolate->NewString("algorithm"), isolate->NewString("SHA-256")).Check();
        break;
    case rtc::CertificateFingerprint::Algorithm::Sha384:
        obj->Set(context, isolate->NewString("algorithm"), isolate->NewString("SHA-384")).Check();
        break;
    case rtc::CertificateFingerprint::Algorithm::Sha512:
        obj->Set(context, isolate->NewString("algorithm"), isolate->NewString("SHA-512")).Check();
        break;
    }

    obj->Set(context, isolate->NewString("fingerprint"), isolate->NewString(fingerprint.value)).Check();

    retVal = obj;

    return 0;
}

result_t RTCPeerConnection::get_signalingState(exlib::string& retVal)
{
    retVal = signaling_state_str(m_peerConnection->signalingState());
    return 0;
}

void RTCPeerConnection::onStateChange(rtc::PeerConnection::State state)
{
    if (state == rtc::PeerConnection::State::Connected) {
        m_lock.lock();
        m_dataChannels.clear();
        m_lock.unlock();
    } else if (state == rtc::PeerConnection::State::Closed) {
        m_lock.lock();
        while (!m_dataChannels.empty()) {
            m_dataChannels.front()->close();
            m_dataChannels.pop_front();
        }
        m_lock.unlock();

        m_self.Release();
        isolate_unref();
    }

    obj_ptr<NObject> ev = new NObject();

    ev->add("state", connection_state_str(state));

    _emit("connectionstatechange", ev);
}

void RTCPeerConnection::onDataChannel(std::shared_ptr<rtc::DataChannel> dataChannel)
{
    obj_ptr<NObject> ev = new NObject();

    ev->add("channel", new RTCDataChannel(dataChannel));

    _emit("datachannel", ev);
}

void RTCPeerConnection::onLocalCandidate(rtc::Candidate candidate)
{
    obj_ptr<NObject> ev = new NObject();
    obj_ptr<NObject> obj_candidate = new NObject();

    ev->add("candidate", obj_candidate);
    obj_candidate->add("candidate", candidate.candidate());
    obj_candidate->add("sdpMid", candidate.mid());

    obj_candidate->add("priority", (int32_t)candidate.priority());

    if (candidate.isResolved()) {
        switch (candidate.transportType()) {
        case rtc::Candidate::TransportType::Udp:
            obj_candidate->add("transport", "udp");
            break;
        case rtc::Candidate::TransportType::TcpActive:
            obj_candidate->add("transport", "tcp-active");
            break;
        case rtc::Candidate::TransportType::TcpPassive:
            obj_candidate->add("transport", "tcp-passive");
            break;
        case rtc::Candidate::TransportType::TcpSo:
            obj_candidate->add("transport", "tcp-so");
            break;
        case rtc::Candidate::TransportType::TcpUnknown:
            obj_candidate->add("transport", "tcp-unknown");
            break;
        default:
            obj_candidate->add("transport", "unknown");
            break;
        }

        obj_candidate->add("address", candidate.address().value());
        obj_candidate->add("port", candidate.port().value());
    }

    obj_candidate->add("type", candidate_type_str(candidate.type()));

    _emit("icecandidate", ev);
}

void RTCPeerConnection::onIceStateChange(rtc::PeerConnection::IceState state)
{
    obj_ptr<NObject> ev = new NObject();

    ev->add("state", ice_connection_state_str(state));

    _emit("iceconnectionstatechange", ev);
}

void RTCPeerConnection::onGatheringStateChange(rtc::PeerConnection::GatheringState state)
{
    obj_ptr<NObject> ev = new NObject();

    ev->add("state", ice_gathering_state_str(state));

    _emit("icegatheringstatechange", ev);
}

void RTCPeerConnection::onLocalDescription(rtc::Description description)
{
    obj_ptr<NObject> ev = new NObject();
    obj_ptr<NObject> obj_description = description_to_object(description);

    ev->add("description", obj_description);

    m_lock.lock();
    if (description.type() == rtc::Description::Type::Offer) {
        m_offer_description = obj_description;
        for (auto& [retVal, ac] : m_offers) {
            retVal = obj_description;
            ac->post(0);
        }
        m_offers.clear();
    } else if (description.type() == rtc::Description::Type::Answer) {
        m_answer_description = obj_description;
        for (auto& [retVal, ac] : m_answers) {
            retVal = obj_description;
            ac->post(0);
        }
        m_answers.clear();
    }
    m_lock.unlock();

    _emit("localdescription", ev);
}

void RTCPeerConnection::onSignalingStateChange(rtc::PeerConnection::SignalingState state)
{
    obj_ptr<NObject> ev = new NObject();

    ev->add("state", signaling_state_str(state));

    _emit("signalingstatechange", ev);
}

void RTCPeerConnection::onTrack(std::shared_ptr<rtc::Track> track)
{
    _emit("track");
}

result_t RTCPeerConnection::create(v8::Local<v8::Object> options)
{
    Isolate* isolate = holder();
    result_t hr;
    rtc::Configuration config;

    exlib::string bindAddress;
    hr = GetConfigValue(holder(), options, "bindAddress", bindAddress, true);
    if (hr != CALL_E_PARAMNOTOPTIONAL) {
        if (hr < 0)
            return hr;
        config.bindAddress = bindAddress;
    }

    exlib::string certificateType;
    hr = GetConfigValue(holder(), options, "certificateType", certificateType, true);
    if (hr != CALL_E_PARAMNOTOPTIONAL) {
        if (hr < 0)
            return hr;
        if (certificateType == "default")
            config.certificateType = rtc::CertificateType::Default;
        else if (certificateType == "ecdsa")
            config.certificateType = rtc::CertificateType::Ecdsa;
        else if (certificateType == "rsa")
            config.certificateType = rtc::CertificateType::Rsa;
        else
            return CALL_E_INVALIDARG;
    }

    exlib::string iceTransportPolicy;
    hr = GetConfigValue(holder(), options, "iceTransportPolicy", iceTransportPolicy, true);
    if (hr != CALL_E_PARAMNOTOPTIONAL) {
        if (hr < 0)
            return hr;
        if (iceTransportPolicy == "all")
            config.iceTransportPolicy = rtc::TransportPolicy::All;
        else if (iceTransportPolicy == "relay")
            config.iceTransportPolicy = rtc::TransportPolicy::Relay;
        else
            return CALL_E_INVALIDARG;
    }

    v8::Local<v8::Array> iceServers;
    hr = GetConfigValue(isolate, options, "iceServers", iceServers, true);
    if (hr != CALL_E_PARAMNOTOPTIONAL) {
        if (hr < 0)
            return hr;

        try {
            for (uint32_t i = 0; i < iceServers->Length(); i++) {
                v8::Local<v8::Object> iceServer;

                hr = GetConfigValue(isolate, iceServers, i, iceServer, true);
                if (hr < 0)
                    return hr;

                v8::Local<v8::Array> urls;
                hr = GetConfigValue(isolate, iceServer, "urls", urls, true);
                if (hr < 0)
                    return hr;

                for (uint32_t j = 0; j < urls->Length(); j++) {
                    exlib::string url;
                    hr = GetConfigValue(isolate, urls, j, url, true);
                    if (hr < 0)
                        return hr;

                    config.iceServers.push_back(rtc::IceServer(url));
                }
            }
        } catch (std::exception& e) {
            return Runtime::setError(e.what());
        }
    } else
        config.iceServers.push_back(rtc::IceServer("stun:stun.l.google.com:19302"));

    int32_t port;
    hr = GetConfigValue(isolate, options, "port", port, true);
    if (hr != CALL_E_PARAMNOTOPTIONAL) {
        if (hr < 0)
            return hr;
        config.portRangeBegin = config.portRangeEnd = port;
    }

    int32_t maxMessageSize;
    hr = GetConfigValue(isolate, options, "maxMessageSize", maxMessageSize, true);
    if (hr != CALL_E_PARAMNOTOPTIONAL) {
        if (hr < 0)
            return hr;
        config.maxMessageSize = maxMessageSize;
    }

    bool enableIceUdpMux;
    hr = GetConfigValue(isolate, options, "enableIceUdpMux", enableIceUdpMux, true);
    if (hr != CALL_E_PARAMNOTOPTIONAL) {
        if (hr < 0)
            return hr;
        config.enableIceUdpMux = enableIceUdpMux;
    }

    bool disableFingerprintVerification;
    hr = GetConfigValue(isolate, options, "disableFingerprintVerification", disableFingerprintVerification, true);
    if (hr != CALL_E_PARAMNOTOPTIONAL) {
        if (hr < 0)
            return hr;
        config.disableFingerprintVerification = disableFingerprintVerification;
    }

    exlib::string iceUfrag;
    hr = GetConfigValue(isolate, options, "iceUfrag", iceUfrag, true);
    if (hr != CALL_E_PARAMNOTOPTIONAL) {
        if (hr < 0)
            return hr;
        config.iceUfrag = iceUfrag;
    }

    exlib::string icePwd;
    hr = GetConfigValue(isolate, options, "icePwd", icePwd, true);
    if (hr != CALL_E_PARAMNOTOPTIONAL) {
        if (hr < 0)
            return hr;
        config.icePwd = icePwd;
    }

    exlib::string certPem;
    hr = GetConfigValue(isolate, options, "certPem", certPem, true);
    if (hr != CALL_E_PARAMNOTOPTIONAL) {
        if (hr < 0)
            return hr;
        config.certPem = certPem;
    }

    exlib::string keyPem;
    hr = GetConfigValue(isolate, options, "keyPem", keyPem, true);
    if (hr != CALL_E_PARAMNOTOPTIONAL) {
        if (hr < 0)
            return hr;
        config.keyPem = keyPem;
    }

    m_local_id = std::to_string(rand());
    m_remote_id = std::to_string(rand());

    m_peerConnection = std::make_shared<rtc::PeerConnection>(config);

    m_peerConnection->onStateChange([this](rtc::PeerConnection::State state) {
        onStateChange(state);
    });

    m_peerConnection->onLocalDescription([this](rtc::Description description) {
        onLocalDescription(description);
    });

    m_peerConnection->onLocalCandidate([this](rtc::Candidate candidate) {
        onLocalCandidate(candidate);
    });

    m_peerConnection->onIceStateChange([this](rtc::PeerConnection::IceState state) {
        onIceStateChange(state);
    });

    m_peerConnection->onGatheringStateChange([this](rtc::PeerConnection::GatheringState state) {
        onGatheringStateChange(state);
    });

    m_peerConnection->onSignalingStateChange([this](rtc::PeerConnection::SignalingState state) {
        onSignalingStateChange(state);
    });

    m_peerConnection->onTrack([this](std::shared_ptr<rtc::Track> track) {
        onTrack(track);
    });

    m_peerConnection->onDataChannel([this](std::shared_ptr<rtc::DataChannel> dataChannel) {
        onDataChannel(dataChannel);
    });

    return 0;
}

}
