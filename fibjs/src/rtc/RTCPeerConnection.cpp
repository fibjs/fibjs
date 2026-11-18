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
#include "union_helpers.h"

namespace fibjs {

result_t RTCPeerConnection_base::_new(RTCPeerConnectionOptions* options,
    obj_ptr<RTCPeerConnection_base>& retVal, v8::Local<v8::Object> This)
{
    obj_ptr<RTCPeerConnection> peer = new RTCPeerConnection();
    peer->wrap(This);
    result_t hr = peer->create(options);
    if (hr.failed())
        return hr;
    retVal = peer;

    return result_t();
}

result_t RTCSessionDescription_base::_new(v8::Local<v8::Object> description,
    obj_ptr<RTCSessionDescription_base>& retVal, v8::Local<v8::Object> This)
{
    result_t hr;
    exlib::string type;

    hr = GetConfigValue(description, "type", type, true);
    if (hr.failed())
        return hr;

    // The standard dictionary defaults `sdp` to an empty string, which is the
    // value a rollback description needs, so the field is optional.
    std::optional<exlib::string> sdp;
    hr = GetConfigValue(description, "sdp", sdp, true);
    if (hr.failed())
        return hr;

    exlib::string sdp_val = sdp.value_or("");

    try {
        retVal = new RTCSessionDescription(sdp_val, type);
    } catch (std::exception& e) {
        return result_t::make(CALL_E_INVALID_DATA).with_type_name(errtype::kError).with_code("ERR_RTC_ERROR").with_message(e.what());
    }

    return result_t();
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

// The library does not expose the DTLS transport state directly; the peer
// connection state is the closest public value (DTLS completes together with
// the connection), and the values are the ones the standard uses.
inline const char* dtls_state_str(rtc::PeerConnection::State state)
{
    switch (state) {
    case rtc::PeerConnection::State::New:
        return "new";
    case rtc::PeerConnection::State::Connecting:
    case rtc::PeerConnection::State::Disconnected:
        return "connecting";
    case rtc::PeerConnection::State::Connected:
        return "connected";
    case rtc::PeerConnection::State::Failed:
        return "failed";
    case rtc::PeerConnection::State::Closed:
        return "closed";
    default:
        return "new";
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
    if (m_peerConnection) {
        m_peerConnection->resetCallbacks();
        close();
    }

    // The listener counts accepted sessions and releases the slot on close. A
    // connection destroyed before libdatachannel reports Closed must release it
    // here, after resetCallbacks() suppressed the state callback.
    auto on_close = std::move(m_on_close);
    m_on_close = nullptr;
    if (on_close)
        on_close();
}

result_t RTCPeerConnection::createDataChannel(exlib::string label, RTCDataChannelOptions* options, obj_ptr<RTCDataChannel_base>& retVal)
{
    Isolate* isolate = holder();
    rtc::DataChannelInit init;

    if (options) {
        // The standard makes the two reliability limits exclusive; the
        // library throws a generic error when both reach it, so the option is
        // rejected here with the argument name instead.
        if (options->maxPacketLifeTime.has_value() && options->maxRetransmits.has_value())
            return result_t::make(CALL_E_INVALID_DATA).with_type_name(errtype::kTypeError).with_code("ERR_INVALID_ARG_VALUE").with_message("maxPacketLifeTime and maxRetransmits are mutually exclusive");

        if (options->ordered.has_value())
            init.reliability.unordered = !options->ordered.value();

        if (options->maxPacketLifeTime.has_value()) {
            if (options->maxPacketLifeTime.value() < 0)
                return result_t::make(CALL_E_INVALID_DATA).with_type_name(errtype::kTypeError).with_code("ERR_INVALID_ARG_VALUE").with_message("maxPacketLifeTime must not be negative");

            init.reliability.maxPacketLifeTime = std::chrono::milliseconds(options->maxPacketLifeTime.value());
        }

        if (options->maxRetransmits.has_value()) {
            if (options->maxRetransmits.value() < 0)
                return result_t::make(CALL_E_INVALID_DATA).with_type_name(errtype::kTypeError).with_code("ERR_INVALID_ARG_VALUE").with_message("maxRetransmits must not be negative");

            init.reliability.maxRetransmits = options->maxRetransmits.value();
        }

        if (options->protocol.has_value())
            init.protocol = options->protocol.value();

        if (options->negotiated.has_value())
            init.negotiated = options->negotiated.value();

        if (options->id.has_value()) {
            // 65535 is the sentinel of an unassigned id, so 65534 is the last
            // usable stream id.
            if (options->id.value() < 0 || options->id.value() > 65534)
                return result_t::make(CALL_E_INVALID_DATA).with_type_name(errtype::kTypeError).with_code("ERR_INVALID_ARG_VALUE").with_message("id must be between 0 and 65534");

            init.id = options->id.value();
        }
    }

    obj_ptr<RTCDataChannel> dc;
    try {
        dc = new RTCDataChannel(isolate, m_peerConnection->createDataChannel(label, init), init.negotiated);
        retVal = dc;
    } catch (std::exception& e) {
        return result_t::make(CALL_E_INVALID_DATA).with_type_name(errtype::kError).with_code("ERR_RTC_ERROR").with_message(e.what());
    }

    m_lock.lock();
    if (m_peerConnection->state() != rtc::PeerConnection::State::Connected)
        m_dataChannels.emplace_back(dc);
    m_lock.unlock();

    return result_t();
}

result_t RTCPeerConnection::setLocalDescription(AsyncHandle ac)
{
    return result_t();
}

result_t RTCPeerConnection::setLocalDescription(Union_setLocalDescription_description description, AsyncHandle ac)
{
    // the async-aware helper keeps the phase logic: the object alternative is
    // built in the sync phase (it reads JS properties), the class alternative
    // is resolved in the async phase, so a cc_ caller can pass it directly
    obj_ptr<RTCSessionDescription_base> description_;
    result_t hr = ctor_object_from_union<RTCSessionDescription_base>(description, description_, ac);
    if (hr.failed())
        return hr;

    RTCSessionDescription* desc = static_cast<RTCSessionDescription*>(description_.get());

    try {
        rtc::Description::Type type = desc->m_desc.type();
        rtc::PeerConnection::SignalingState state = m_peerConnection->signalingState();

        if (type == rtc::Description::Type::Offer) {
            if (state == rtc::PeerConnection::SignalingState::Stable)
                m_peerConnection->setLocalDescription(type,
                    m_local_description_init.value_or(rtc::LocalDescriptionInit {}));
        } else if (type == rtc::Description::Type::Rollback) {
            // The library only rolls back a pending local offer; otherwise
            // the call is a no-op, which is what the standard asks for.
            m_peerConnection->setLocalDescription(type);
        } else if ((type == rtc::Description::Type::Answer || type == rtc::Description::Type::Pranswer)
            && state == rtc::PeerConnection::SignalingState::HaveRemoteOffer) {
            m_peerConnection->setLocalDescription(type,
                m_local_description_init.value_or(rtc::LocalDescriptionInit {}));
        }
    } catch (std::exception& e) {
        return result_t::make(CALL_E_INVALID_DATA).with_type_name(errtype::kError).with_code("ERR_RTC_ERROR").with_message(e.what());
    }

    return result_t();
}

result_t RTCPeerConnection::setRemoteDescription(Union_setRemoteDescription_description description, AsyncHandle ac)
{
    // keep the connection alive across the async operation; the JS wrapper
    // exists only on the JS call path (the sync phase), so this side effect
    // stays here
    if (ac.isSync())
        m_self = new ValueHolder(wrap());

    obj_ptr<RTCSessionDescription_base> description_;
    result_t hr = ctor_object_from_union<RTCSessionDescription_base>(description, description_, ac);
    if (hr.failed())
        return hr;

    RTCSessionDescription* desc = static_cast<RTCSessionDescription*>(description_.get());
    try {
        m_peerConnection->setRemoteDescription(desc->m_desc);

        // When custom ICE credentials are configured the automatic
        // negotiation is disabled, so generate the answer here to keep the
        // previous contract that setRemoteDescription also applies it.
        if (m_local_description_init.has_value()
            && m_peerConnection->signalingState() == rtc::PeerConnection::SignalingState::HaveRemoteOffer)
            m_peerConnection->setLocalDescription(rtc::Description::Type::Answer, *m_local_description_init);

        isolate_ref();
    } catch (std::exception& e) {
        m_self.Release();
        return result_t::make(CALL_E_INVALID_DATA).with_type_name(errtype::kError).with_code("ERR_RTC_ERROR").with_message(e.what());
    }

    return result_t();
}

result_t RTCPeerConnection::addIceCandidate(Union_addIceCandidate_candidate candidate, AsyncHandle ac)
{
    // the async-aware helper keeps the phase logic: the object alternative is
    // built in the sync phase (it reads JS properties), the class alternative
    // is resolved in the async phase, so a cc_ caller can pass it directly
    obj_ptr<RTCIceCandidate_base> candidate_;
    result_t hr = ctor_object_from_union<RTCIceCandidate_base>(candidate, candidate_, ac);
    if (hr.failed())
        return hr;

    RTCIceCandidate* cand = static_cast<RTCIceCandidate*>(candidate_.get());

    // The standard's end-of-candidates marker: an object without a usable
    // candidate string carries no address and only tells the agent that the
    // peer finished gathering, which the library does not need.
    if (cand->is_empty())
        return result_t();

    try {
        m_peerConnection->addRemoteCandidate(cand->m_candidate);
    } catch (std::exception& e) {
        return result_t::make(CALL_E_INVALID_DATA).with_type_name(errtype::kError).with_code("ERR_RTC_ERROR").with_message(e.what());
    }

    return result_t();
}

result_t RTCPeerConnection::createOffer(v8::Local<v8::Object> options, Variant& retVal, AsyncHandle ac)
{
    if (ac.isSync())
        return result_t::nosync();

    // With custom ICE credentials the implicit negotiation is disabled, so
    // create the local offer here before waiting for the localdescription
    // callback that resolves the pending promise.
    if (m_local_description_init.has_value()
        && m_peerConnection->signalingState() == rtc::PeerConnection::SignalingState::Stable
        && !m_peerConnection->localDescription().has_value()
        && m_peerConnection->negotiationNeeded()) {
        try {
            m_peerConnection->setLocalDescription(rtc::Description::Type::Offer, *m_local_description_init);
        } catch (std::exception& e) {
            return result_t::make(CALL_E_INVALID_DATA).with_type_name(errtype::kError).with_code("ERR_RTC_ERROR").with_message(e.what());
        }
    }

    m_lock.lock();
    if (m_offer_description) {
        retVal = m_offer_description;
        m_lock.unlock();
        return result_t();
    }
    m_offers.emplace_back(retVal, std::move(ac));
    m_lock.unlock();

    return result_t::pending();
}

result_t RTCPeerConnection::createAnswer(v8::Local<v8::Object> options, Variant& retVal, AsyncHandle ac)
{
    if (ac.isSync())
        return result_t::nosync();

    // Same as createOffer: with custom ICE credentials the answer is not
    // generated implicitly when the remote offer is applied.
    if (m_local_description_init.has_value()
        && m_peerConnection->signalingState() == rtc::PeerConnection::SignalingState::HaveRemoteOffer) {
        try {
            m_peerConnection->setLocalDescription(rtc::Description::Type::Answer, *m_local_description_init);
        } catch (std::exception& e) {
            return result_t::make(CALL_E_INVALID_DATA).with_type_name(errtype::kError).with_code("ERR_RTC_ERROR").with_message(e.what());
        }
    }

    m_lock.lock();
    if (m_answer_description) {
        retVal = m_answer_description;
        m_lock.unlock();
        return result_t();
    }
    m_answers.emplace_back(retVal, std::move(ac));
    m_lock.unlock();

    return result_t::pending();
}

result_t RTCPeerConnection::getStats(obj_ptr<NMap>& retVal, AsyncHandle ac)
{
    if (ac.isSync())
        return result_t::nosync();

    obj_ptr<NMap> stats = new NMap();

    rtc::Candidate local, remote;
    std::chrono::milliseconds timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch());

    // The pair is selected only once the connectivity checks complete; the
    // standard still resolves with a report before that, so the candidate
    // entries are simply absent from it.
    bool has_pair = m_peerConnection->getSelectedCandidatePair(&local, &remote);

    size_t bytesSent = m_peerConnection->bytesSent();
    size_t bytesReceived = m_peerConnection->bytesReceived();
    std::chrono::milliseconds rtt = m_peerConnection->rtt().value_or(std::chrono::milliseconds(0));

    obj_ptr<NObject> stat;

    exlib::string peer_id;

    if (has_pair) {
        exlib::string local_id = "RTCIceCandidate_" + m_local_id;
        stat = new NObject();
        stats->add(local_id, stat);

        stat->add("id", local_id);
        stat->add("type", "localcandidate");
        stat->add("timestamp", (double)timestamp.count());
        stat->add("candidateType", candidate_type_str(local.type()));
        stat->add("ip", local.address().value_or(""));
        stat->add("port", (int32_t)local.port().value_or(0));

        exlib::string remote_id = "RTCIceCandidate_" + m_remote_id;
        stat = new NObject();
        stats->add(remote_id, stat);

        stat->add("id", remote_id);
        stat->add("type", "remotecandidate");
        stat->add("timestamp", (double)timestamp.count());
        stat->add("candidateType", candidate_type_str(remote.type()));
        stat->add("ip", remote.address().value_or(""));
        stat->add("port", (int32_t)remote.port().value_or(0));

        peer_id = "RTCIceCandidatePair_" + m_local_id + "_" + m_remote_id;
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
    }

    exlib::string transport_id = "RTCTransport_0_1";
    stat = new NObject();
    stats->add(transport_id, stat);

    stat->add("id", transport_id);
    stat->add("type", "transport");
    stat->add("timestamp", (double)timestamp.count());
    stat->add("bytesSent", (double)bytesSent);
    stat->add("bytesReceived", (double)bytesReceived);
    stat->add("dtlsState", dtls_state_str(m_peerConnection->state()));
    if (has_pair)
        stat->add("selectedCandidatePairId", peer_id);
    stat->add("selectedCandidatePairChanges", has_pair ? 1 : 0);

    retVal = stats;

    return result_t();
}

result_t RTCPeerConnection::close()
{
    m_peerConnection->close();

    // A pending createOffer/createAnswer can never settle once the connection
    // is closed: reject it here instead of leaving the awaiting fiber - and
    // therefore the process - held forever.
    result_t hr = result_t::make(CALL_E_INVALID_DATA).with_type_name(errtype::kError).with_code("ERR_INVALID_STATE").with_message("The connection is closed");

    m_lock.lock();
    for (auto& [retVal, ac] : m_offers)
        ac.post(hr);
    m_offers.clear();
    for (auto& [retVal, ac] : m_answers)
        ac.post(hr);
    m_answers.clear();
    m_lock.unlock();

    return result_t();
}

result_t RTCPeerConnection::get_connectionState(exlib::string& retVal)
{
    retVal = connection_state_str(m_peerConnection->state());
    return result_t();
}

result_t RTCPeerConnection::get_iceConnectionState(exlib::string& retVal)
{
    retVal = ice_connection_state_str(m_peerConnection->iceState());
    return result_t();
}

result_t RTCPeerConnection::get_iceGatheringState(exlib::string& retVal)
{
    retVal = ice_gathering_state_str(m_peerConnection->gatheringState());
    return result_t();
}

result_t RTCPeerConnection::get_localDescription(v8::Local<v8::Object>& retVal)
{
    auto desc = m_peerConnection->localDescription();
    if (desc.has_value())
        return description_to_object(desc.value())->valueOf(retVal);

    return result_t();
}

result_t RTCPeerConnection::get_remoteDescription(v8::Local<v8::Object>& retVal)
{
    auto desc = m_peerConnection->remoteDescription();
    if (desc.has_value())
        return description_to_object(desc.value())->valueOf(retVal);

    return result_t();
}

result_t RTCPeerConnection::get_remoteFingerprint(v8::Local<v8::Object>& retVal)
{
    obj_ptr<NObject> obj = fingerprint_to_object(m_peerConnection->remoteFingerprint());
    return obj->valueOf(retVal);
}

obj_ptr<NObject> RTCPeerConnection::fingerprint_to_object(rtc::CertificateFingerprint fingerprint)
{
    NObject* obj = new NObject();

    switch (fingerprint.algorithm) {
    case rtc::CertificateFingerprint::Algorithm::Sha1:
        obj->add("algorithm", "SHA-1");
        break;
    case rtc::CertificateFingerprint::Algorithm::Sha224:
        obj->add("algorithm", "SHA-224");
        break;
    case rtc::CertificateFingerprint::Algorithm::Sha256:
        obj->add("algorithm", "SHA-256");
        break;
    case rtc::CertificateFingerprint::Algorithm::Sha384:
        obj->add("algorithm", "SHA-384");
        break;
    case rtc::CertificateFingerprint::Algorithm::Sha512:
        obj->add("algorithm", "SHA-512");
        break;
    }

    obj->add("fingerprint", exlib::string(fingerprint.value));

    return obj;
}

result_t RTCPeerConnection::get_signalingState(exlib::string& retVal)
{
    retVal = signaling_state_str(m_peerConnection->signalingState());
    return result_t();
}

void RTCPeerConnection::onStateChange(rtc::PeerConnection::State state)
{
    if (state == rtc::PeerConnection::State::Connected) {
        m_lock.lock();
        m_dataChannels.clear();
        m_lock.unlock();

        if (!m_secure_emitted) {
            m_secure_emitted = true;

            obj_ptr<NObject> ev = new NObject();
            obj_ptr<NObject> fp = fingerprint_to_object(m_peerConnection->remoteFingerprint());
            ev->add("fingerprint", fp);
            _emit("secure", ev);
        }
    } else if (state == rtc::PeerConnection::State::Closed) {
        m_peerConnection->resetCallbacks();

        m_lock.lock();
        while (!m_dataChannels.empty()) {
            m_dataChannels.front()->close();
            m_dataChannels.pop_front();
        }
        m_lock.unlock();
    }

    obj_ptr<NObject> ev = new NObject();

    ev->add("state", connection_state_str(state));

    _emit("connectionstatechange", ev);

    if (state == rtc::PeerConnection::State::Closed) {
        auto on_close = std::move(m_on_close);
        m_on_close = nullptr;
        if (on_close)
            on_close();

        m_self.Release();
        isolate_unref();
    }
}

void RTCPeerConnection::onDataChannel(std::shared_ptr<rtc::DataChannel> dataChannel)
{
    obj_ptr<NObject> ev = new NObject();

    ev->add("channel", new RTCDataChannel(holder(), dataChannel, false));

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
    if (state == rtc::PeerConnection::GatheringState::Complete) {
        // The standard ends the candidate stream with one event carrying a
        // null candidate, so the handler that forwards candidates can pass
        // the end-of-candidates marker on and stop.
        obj_ptr<NObject> ev = new NObject();

        ev->add("candidate", Variant());
        _emit("icecandidate", ev);
    }

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
            ac.post(result_t());
        }
        m_offers.clear();
    } else if (description.type() == rtc::Description::Type::Answer) {
        m_answer_description = obj_description;
        for (auto& [retVal, ac] : m_answers) {
            retVal = obj_description;
            ac.post(result_t());
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

result_t RTCPeerConnection::create(RTCPeerConnectionOptions* options)
{
    rtc::Configuration config;
    std::optional<rtc::LocalDescriptionInit> local_init;

    if (options) {
        if (options->bindAddress.has_value())
            config.bindAddress = options->bindAddress.value();

        if (options->certificateType.has_value()) {
            exlib::string certificateType = options->certificateType.value();
            if (certificateType == "default")
                config.certificateType = rtc::CertificateType::Default;
            else if (certificateType == "ecdsa")
                config.certificateType = rtc::CertificateType::Ecdsa;
            else if (certificateType == "rsa")
                config.certificateType = rtc::CertificateType::Rsa;
            else
                return result_t::make(CALL_E_INVALID_DATA).with_type_name(errtype::kTypeError).with_code("ERR_INVALID_ARG_VALUE").with_format("Invalid certificateType: '%s'.", certificateType.c_str()).with_result(result_t::invalid_arg());
        }

        if (options->iceTransportPolicy.has_value()) {
            exlib::string iceTransportPolicy = options->iceTransportPolicy.value();
            if (iceTransportPolicy == "all")
                config.iceTransportPolicy = rtc::TransportPolicy::All;
            else if (iceTransportPolicy == "relay")
                config.iceTransportPolicy = rtc::TransportPolicy::Relay;
            else
                return result_t::make(CALL_E_INVALID_DATA).with_type_name(errtype::kTypeError).with_code("ERR_INVALID_ARG_VALUE").with_format("Invalid iceTransportPolicy: '%s'.", iceTransportPolicy.c_str()).with_result(result_t::invalid_arg());
        }

        if (options->iceServers.has_value()) {
            try {
                for (auto& entry : options->iceServers.value()) {
                    if (!entry->urls.has_value())
                        return setRuntimeError(CALL_E_PARAMNOTOPTIONAL, "urls");

                    auto addServer = [&](exlib::string url) {
                        rtc::IceServer iceServer(url);
                        if (entry->username.has_value())
                            iceServer.username = entry->username.value();
                        if (entry->credential.has_value())
                            iceServer.password = entry->credential.value();

                        config.iceServers.push_back(iceServer);
                    };

                    if (std::holds_alternative<exlib::string>(entry->urls.value()))
                        addServer(std::get<exlib::string>(entry->urls.value()));
                    else
                        for (auto& url : std::get<std::vector<exlib::string>>(entry->urls.value()))
                            addServer(url);
                }
            } catch (std::exception& e) {
                return result_t::make(CALL_E_INVALID_DATA).with_type_name(errtype::kError).with_code("ERR_RTC_ERROR").with_message(e.what());
            }
        } else
            config.iceServers.push_back(rtc::IceServer("stun:stun.l.google.com:19302"));

        if (options->port.has_value())
            config.portRangeBegin = config.portRangeEnd = options->port.value();

        if (options->maxMessageSize.has_value())
            config.maxMessageSize = options->maxMessageSize.value();

        if (options->enableIceUdpMux.has_value())
            config.enableIceUdpMux = options->enableIceUdpMux.value();

        if (options->disableFingerprintVerification.has_value())
            config.disableFingerprintVerification = options->disableFingerprintVerification.value();

        if (options->iceUfrag.has_value() && options->icePwd.has_value()
            && !options->iceUfrag->empty() && !options->icePwd->empty()) {
            rtc::LocalDescriptionInit init;
            init.iceUfrag = std::string(options->iceUfrag.value());
            init.icePwd = std::string(options->icePwd.value());
            local_init = std::move(init);

            // libdatachannel can apply custom ICE attributes only through
            // setLocalDescription(type, init); the implicit offer/answer paths
            // have no way to carry them, so fibjs generates both explicitly.
            config.disableAutoNegotiation = true;
        }

        if (options->certPem.has_value())
            config.certificatePemFile = options->certPem.value();

        if (options->keyPem.has_value())
            config.keyPemFile = options->keyPem.value();

        if (options->keyPass.has_value())
            config.keyPemPass = options->keyPass.value();
    }

    std::shared_ptr<rtc::PeerConnection> peer;
    try {
        peer = std::make_shared<rtc::PeerConnection>(config);
    } catch (std::exception& e) {
        return result_t::make(CALL_E_INVALID_DATA).with_type_name(errtype::kError).with_code("ERR_RTC_ERROR").with_message(e.what());
    }

    return init(std::move(peer), std::move(local_init));
}

result_t RTCPeerConnection::init(std::shared_ptr<rtc::PeerConnection> peer,
    std::optional<rtc::LocalDescriptionInit> localInit)
{
    m_local_description_init = std::move(localInit);

    m_local_id = std::to_string(rand());
    m_remote_id = std::to_string(rand());

    m_peerConnection = std::move(peer);

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

    return result_t();
}

result_t RTCPeerConnection::accept(std::shared_ptr<rtc::PeerConnection> peer,
    rtc::LocalDescriptionInit localInit, exlib::string remoteSdp,
    std::function<void()> onClose)
{
    m_on_close = std::move(onClose);

    result_t hr = init(std::move(peer), std::move(localInit));
    if (hr.failed())
        return hr;

    // The listener-created connection owns the process hold until it closes,
    // like a connection that applied a remote description through JavaScript.
    isolate_ref();

    try {
        m_peerConnection->setRemoteDescription(
            rtc::Description(remoteSdp, rtc::Description::Type::Offer));
        m_peerConnection->setLocalDescription(rtc::Description::Type::Answer,
            *m_local_description_init);
    } catch (std::exception& e) {
        isolate_unref();
        return result_t::make(CALL_E_INVALID_DATA).with_type_name(errtype::kError).with_code("ERR_RTC_ERROR").with_message(e.what());
    }

    return result_t();
}

}
