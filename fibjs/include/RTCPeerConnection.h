/*
 * RTCPeerConnection.h
 *
 *  Created on: Apr 27, 2024
 *      Author: lion
 */

#pragma once

#define RTC_STATIC

#include "ifs/RTCPeerConnection.h"
#include "datachannel/include/rtc/rtc.hpp"
#include <list>

namespace fibjs {

class RTCPeerConnection : public RTCPeerConnection_base {
public:
    ~RTCPeerConnection();

    EVENT_SUPPORT();

public:
    // RTCPeerConnection_base
    virtual result_t createDataChannel(exlib::string label, v8::Local<v8::Object> options, obj_ptr<RTCDataChannel_base>& retVal);
    virtual result_t setLocalDescription(AsyncEvent* ac);
    virtual result_t setLocalDescription(RTCSessionDescription_base* description, AsyncEvent* ac);
    virtual result_t setRemoteDescription(RTCSessionDescription_base* description, AsyncEvent* ac);
    virtual result_t addIceCandidate(RTCIceCandidate_base* candidate, AsyncEvent* ac);
    virtual result_t createOffer(v8::Local<v8::Object> options, Variant& retVal, AsyncEvent* ac);
    virtual result_t createAnswer(v8::Local<v8::Object> options, Variant& retVal, AsyncEvent* ac);
    virtual result_t getStats(obj_ptr<NMap>& retVal, AsyncEvent* ac);
    virtual result_t close();
    virtual result_t get_connectionState(exlib::string& retVal);
    virtual result_t get_iceConnectionState(exlib::string& retVal);
    virtual result_t get_iceGatheringState(exlib::string& retVal);
    virtual result_t get_localDescription(v8::Local<v8::Object>& retVal);
    virtual result_t get_remoteDescription(v8::Local<v8::Object>& retVal);
    virtual result_t get_signalingState(exlib::string& retVal);

public:
    EVENT_FUNC(connectionstatechange);
    EVENT_FUNC(datachannel);
    EVENT_FUNC(icecandidate);
    EVENT_FUNC(iceconnectionstatechange);
    EVENT_FUNC(icegatheringstatechange);
    EVENT_FUNC(localdescription);
    EVENT_FUNC(signalingstatechange);
    EVENT_FUNC(track);

public:
    result_t create(v8::Local<v8::Object> options);
    v8::Local<v8::Object> description_to_object(rtc::Description description);

private:
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

    exlib::spinlock m_lock;

    obj_ptr<NObject> m_offer_description;
    std::list<std::pair<Variant&, AsyncEvent*>> m_offers;

    obj_ptr<NObject> m_answer_description;
    std::list<std::pair<Variant&, AsyncEvent*>> m_answers;
};

} /* namespace fibjs */