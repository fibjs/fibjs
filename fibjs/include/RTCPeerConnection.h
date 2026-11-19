/*
 * RTCPeerConnection.h
 *
 *  Created on: Apr 27, 2024
 *      Author: lion
 */

#pragma once

#ifndef RTC_STATIC
#define RTC_STATIC
#endif

#include "ifs/RTCPeerConnection.h"
#include "datachannel/include/rtc/rtc.hpp"
#include "RTCDataChannel.h"
#include <functional>
#include <list>
#include <optional>

namespace fibjs {

class RTCPeerConnection : public RTCPeerConnection_base {
public:
    ~RTCPeerConnection();

public:
    virtual result_t stop()
    {
        return close();
    }

    // RTCPeerConnection_base
    virtual result_t createDataChannel(exlib::string label, RTCDataChannelOptions* options, obj_ptr<RTCDataChannel_base>& retVal);
    virtual result_t setLocalDescription(AsyncHandle ac);
    virtual result_t setLocalDescription(Union_setLocalDescription_description description, AsyncHandle ac);
    virtual result_t setRemoteDescription(Union_setRemoteDescription_description description, AsyncHandle ac);
    virtual result_t addIceCandidate(Union_addIceCandidate_candidate candidate, AsyncHandle ac);
    virtual result_t createOffer(v8::Local<v8::Object> options, Variant& retVal, AsyncHandle ac);
    virtual result_t createAnswer(v8::Local<v8::Object> options, Variant& retVal, AsyncHandle ac);
    virtual result_t getStats(obj_ptr<NMap>& retVal, AsyncHandle ac);
    virtual result_t close();
    virtual result_t get_connectionState(exlib::string& retVal);
    virtual result_t get_iceConnectionState(exlib::string& retVal);
    virtual result_t get_iceGatheringState(exlib::string& retVal);
    virtual result_t get_localDescription(v8::Local<v8::Object>& retVal);
    virtual result_t get_remoteDescription(v8::Local<v8::Object>& retVal);
    virtual result_t get_remoteFingerprint(v8::Local<v8::Object>& retVal);
    virtual result_t get_signalingState(exlib::string& retVal);

public:
    result_t create(RTCPeerConnectionOptions* options);
    obj_ptr<NObject> description_to_object(rtc::Description description);

    // Internal entry used by RTCListener: applies a synthetic remote offer and
    // generates the answer with the ICE credentials derived from the incoming
    // STUN request. Not exposed to JavaScript.
    result_t accept(std::shared_ptr<rtc::PeerConnection> peer, rtc::LocalDescriptionInit init,
        exlib::string remoteSdp, std::function<void()> onClose);

private:
    result_t init(std::shared_ptr<rtc::PeerConnection> peer,
        std::optional<rtc::LocalDescriptionInit> localInit);
    obj_ptr<NObject> fingerprint_to_object(rtc::CertificateFingerprint fingerprint);

    void onStateChange(rtc::PeerConnection::State state);
    void onLocalCandidate(rtc::Candidate candidate);
    void onLocalDescription(rtc::Description description);
    void onDataChannel(std::shared_ptr<rtc::DataChannel> dataChannel);
    void onIceStateChange(rtc::PeerConnection::IceState state);
    void onGatheringStateChange(rtc::PeerConnection::GatheringState state);
    void onSignalingStateChange(rtc::PeerConnection::SignalingState state);
    void onTrack(std::shared_ptr<rtc::Track> track);

private:
    std::shared_ptr<rtc::PeerConnection> m_peerConnection;
    exlib::string m_local_id, m_remote_id;

    // libdatachannel applies forced ICE credentials only when the local
    // description is set explicitly; keep them here instead of adding fields
    // to the library Configuration. Set only when both iceUfrag and icePwd
    // are supplied, which also turns off the implicit negotiation below.
    std::optional<rtc::LocalDescriptionInit> m_local_description_init;

    obj_ptr<ValueHolder> m_self;

    exlib::spinlock m_lock;

    obj_ptr<NObject> m_offer_description;
    std::list<std::pair<Variant&, AsyncHandle>> m_offers;

    obj_ptr<NObject> m_answer_description;
    std::list<std::pair<Variant&, AsyncHandle>> m_answers;

    std::list<obj_ptr<RTCDataChannel>> m_dataChannels;

    std::function<void()> m_on_close;
    bool m_secure_emitted = false;

    // close() sets this before it tears the library down: the async phase of a
    // createOffer/createAnswer can still be on its way after the pending lists
    // were swept, and only this flag can tell that late call it is too late.
    bool m_closed = false;
};

} /* namespace fibjs */