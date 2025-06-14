/***************************************************************************
 *                                                                         *
 *   This file was automatically generated using idlc.js                   *
 *   PLEASE DO NOT EDIT!!!!                                                *
 *                                                                         *
 ***************************************************************************/

#pragma once

/**
 @author Leo Hoo <lion@9465.net>
 */

#include "../object.h"
#include "ifs/EventEmitter.h"

namespace fibjs {

class EventEmitter_base;
class RTCDataChannel_base;
class RTCSessionDescription_base;
class RTCIceCandidate_base;

class RTCPeerConnection_base : public EventEmitter_base {
    DECLARE_CLASS(RTCPeerConnection_base);
    EVENT_SUPPORT();

public:
    // RTCPeerConnection_base
    static result_t _new(v8::Local<v8::Object> options, obj_ptr<RTCPeerConnection_base>& retVal, v8::Local<v8::Object> This = v8::Local<v8::Object>());
    virtual result_t createDataChannel(exlib::string label, v8::Local<v8::Object> options, obj_ptr<RTCDataChannel_base>& retVal) = 0;
    virtual result_t setLocalDescription(AsyncEvent* ac) = 0;
    virtual result_t setLocalDescription(RTCSessionDescription_base* description, AsyncEvent* ac) = 0;
    virtual result_t setRemoteDescription(RTCSessionDescription_base* description, AsyncEvent* ac) = 0;
    virtual result_t addIceCandidate(RTCIceCandidate_base* candidate, AsyncEvent* ac) = 0;
    virtual result_t createOffer(v8::Local<v8::Object> options, Variant& retVal, AsyncEvent* ac) = 0;
    virtual result_t createAnswer(v8::Local<v8::Object> options, Variant& retVal, AsyncEvent* ac) = 0;
    virtual result_t getStats(obj_ptr<NMap>& retVal, AsyncEvent* ac) = 0;
    virtual result_t close() = 0;
    virtual result_t get_connectionState(exlib::string& retVal) = 0;
    virtual result_t get_iceConnectionState(exlib::string& retVal) = 0;
    virtual result_t get_iceGatheringState(exlib::string& retVal) = 0;
    virtual result_t get_localDescription(v8::Local<v8::Object>& retVal) = 0;
    virtual result_t get_remoteDescription(v8::Local<v8::Object>& retVal) = 0;
    virtual result_t get_remoteFingerprint(v8::Local<v8::Object>& retVal) = 0;
    virtual result_t get_signalingState(exlib::string& retVal) = 0;

public:
    static void __new(const v8::FunctionCallbackInfo<v8::Value>& args);
    static result_t load(Isolate* isolate, v8::Local<v8::Value> v, obj_ptr<RTCPeerConnection_base>& retVal);

public:
    static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_createDataChannel(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_setLocalDescription(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_setRemoteDescription(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_addIceCandidate(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_createOffer(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_createAnswer(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_getStats(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_close(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_connectionState(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_iceConnectionState(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_iceGatheringState(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_localDescription(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_remoteDescription(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_remoteFingerprint(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_signalingState(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_onconnectionstatechange(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_set_onconnectionstatechange(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_ondatachannel(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_set_ondatachannel(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_onicecandidate(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_set_onicecandidate(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_oniceconnectionstatechange(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_set_oniceconnectionstatechange(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_onicegatheringstatechange(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_set_onicegatheringstatechange(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_onlocaldescription(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_set_onlocaldescription(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_onsignalingstatechange(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_set_onsignalingstatechange(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_ontrack(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_set_ontrack(const v8::FunctionCallbackInfo<v8::Value>& args);

public:
    ASYNC_MEMBER0(RTCPeerConnection_base, setLocalDescription);
    ASYNC_MEMBER1(RTCPeerConnection_base, setLocalDescription, RTCSessionDescription_base*);
    ASYNC_MEMBER1(RTCPeerConnection_base, setRemoteDescription, RTCSessionDescription_base*);
    ASYNC_MEMBER1(RTCPeerConnection_base, addIceCandidate, RTCIceCandidate_base*);
    ASYNC_MEMBERVALUE2(RTCPeerConnection_base, createOffer, v8::Local<v8::Object>, Variant);
    ASYNC_MEMBERVALUE2(RTCPeerConnection_base, createAnswer, v8::Local<v8::Object>, Variant);
    ASYNC_MEMBERVALUE1(RTCPeerConnection_base, getStats, obj_ptr<NMap>);
};
}

#include "ifs/RTCDataChannel.h"
#include "ifs/RTCSessionDescription.h"
#include "ifs/RTCIceCandidate.h"

namespace fibjs {
inline ClassInfo& RTCPeerConnection_base::class_info()
{
    static ClassData::ClassMethod s_method[] = {
        { "createDataChannel", s_createDataChannel, false, ClassData::ASYNC_SYNC },
        { "setLocalDescription", s_setLocalDescription, false, ClassData::ASYNC_PROMISE },
        { "setRemoteDescription", s_setRemoteDescription, false, ClassData::ASYNC_PROMISE },
        { "addIceCandidate", s_addIceCandidate, false, ClassData::ASYNC_PROMISE },
        { "createOffer", s_createOffer, false, ClassData::ASYNC_PROMISE },
        { "createAnswer", s_createAnswer, false, ClassData::ASYNC_PROMISE },
        { "getStats", s_getStats, false, ClassData::ASYNC_PROMISE },
        { "close", s_close, false, ClassData::ASYNC_SYNC }
    };

    static ClassData::ClassProperty s_property[] = {
        { "connectionState", s_get_connectionState, block_set, false },
        { "iceConnectionState", s_get_iceConnectionState, block_set, false },
        { "iceGatheringState", s_get_iceGatheringState, block_set, false },
        { "localDescription", s_get_localDescription, block_set, false },
        { "remoteDescription", s_get_remoteDescription, block_set, false },
        { "remoteFingerprint", s_get_remoteFingerprint, block_set, false },
        { "signalingState", s_get_signalingState, block_set, false },
        { "onconnectionstatechange", s_get_onconnectionstatechange, s_set_onconnectionstatechange, false },
        { "ondatachannel", s_get_ondatachannel, s_set_ondatachannel, false },
        { "onicecandidate", s_get_onicecandidate, s_set_onicecandidate, false },
        { "oniceconnectionstatechange", s_get_oniceconnectionstatechange, s_set_oniceconnectionstatechange, false },
        { "onicegatheringstatechange", s_get_onicegatheringstatechange, s_set_onicegatheringstatechange, false },
        { "onlocaldescription", s_get_onlocaldescription, s_set_onlocaldescription, false },
        { "onsignalingstatechange", s_get_onsignalingstatechange, s_set_onsignalingstatechange, false },
        { "ontrack", s_get_ontrack, s_set_ontrack, false }
    };

    static ClassData s_cd = {
        "RTCPeerConnection", false, s__new, NULL,
        ARRAYSIZE(s_method), s_method, 0, NULL, ARRAYSIZE(s_property), s_property, 0, NULL, NULL, NULL,
        &EventEmitter_base::class_info(),
        false
    };

    static ClassInfo s_ci(s_cd);
    return s_ci;
}

inline void RTCPeerConnection_base::s__new(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    CONSTRUCT_INIT();
    __new(args);
}

inline void RTCPeerConnection_base::__new(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<RTCPeerConnection_base> vr;

    CONSTRUCT_ENTER();

    METHOD_OVER(1, 0);

    OPT_ARG(v8::Local<v8::Object>, 0, v8::Object::New(isolate->m_isolate));

    hr = _new(v0, vr, args.This());

    CONSTRUCT_RETURN();
}

inline result_t RTCPeerConnection_base::load(Isolate* isolate, v8::Local<v8::Value> v, obj_ptr<RTCPeerConnection_base>& retVal)
{
    obj_ptr<RTCPeerConnection_base> vr;

    LOAD_ENTER();

    METHOD_OVER(1, 0);

    OPT_ARG(v8::Local<v8::Object>, 0, v8::Object::New(isolate->m_isolate));

    hr = _new(v0, vr, args.This());

    LOAD_RETURN();
}

inline void RTCPeerConnection_base::s_createDataChannel(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<RTCDataChannel_base> vr;

    METHOD_INSTANCE(RTCPeerConnection_base);
    METHOD_ENTER();

    METHOD_OVER(2, 1);

    ARG(exlib::string, 0);
    OPT_ARG(v8::Local<v8::Object>, 1, v8::Object::New(isolate->m_isolate));

    hr = pInst->createDataChannel(v0, v1, vr);

    METHOD_RETURN();
}

inline void RTCPeerConnection_base::s_setLocalDescription(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    ASYNC_METHOD_INSTANCE(RTCPeerConnection_base);
    ASYNC_METHOD_ENTER("RTCPeerConnection.setLocalDescription");

    METHOD_OVER(0, 0);

    if (!cb.IsEmpty())
        hr = pInst->acb_setLocalDescription(cb, args);
    else
        hr = pInst->ac_setLocalDescription();

    METHOD_OVER(1, 1);

    ARG(obj_ptr<RTCSessionDescription_base>, 0);

    if (!cb.IsEmpty())
        hr = pInst->acb_setLocalDescription(v0, cb, args);
    else
        hr = pInst->ac_setLocalDescription(v0);

    METHOD_VOID();
}

inline void RTCPeerConnection_base::s_setRemoteDescription(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    ASYNC_METHOD_INSTANCE(RTCPeerConnection_base);
    ASYNC_METHOD_ENTER("RTCPeerConnection.setRemoteDescription");

    METHOD_OVER(1, 1);

    ARG(obj_ptr<RTCSessionDescription_base>, 0);

    if (!cb.IsEmpty())
        hr = pInst->acb_setRemoteDescription(v0, cb, args);
    else
        hr = pInst->ac_setRemoteDescription(v0);

    METHOD_VOID();
}

inline void RTCPeerConnection_base::s_addIceCandidate(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    ASYNC_METHOD_INSTANCE(RTCPeerConnection_base);
    ASYNC_METHOD_ENTER("RTCPeerConnection.addIceCandidate");

    METHOD_OVER(1, 1);

    ARG(obj_ptr<RTCIceCandidate_base>, 0);

    if (!cb.IsEmpty())
        hr = pInst->acb_addIceCandidate(v0, cb, args);
    else
        hr = pInst->ac_addIceCandidate(v0);

    METHOD_VOID();
}

inline void RTCPeerConnection_base::s_createOffer(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    Variant vr;

    ASYNC_METHOD_INSTANCE(RTCPeerConnection_base);
    ASYNC_METHOD_ENTER("RTCPeerConnection.createOffer");

    METHOD_OVER(1, 0);

    OPT_ARG(v8::Local<v8::Object>, 0, v8::Object::New(isolate->m_isolate));

    if (!cb.IsEmpty())
        hr = pInst->acb_createOffer(v0, cb, args);
    else
        hr = pInst->ac_createOffer(v0, vr);

    METHOD_RETURN();
}

inline void RTCPeerConnection_base::s_createAnswer(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    Variant vr;

    ASYNC_METHOD_INSTANCE(RTCPeerConnection_base);
    ASYNC_METHOD_ENTER("RTCPeerConnection.createAnswer");

    METHOD_OVER(1, 0);

    OPT_ARG(v8::Local<v8::Object>, 0, v8::Object::New(isolate->m_isolate));

    if (!cb.IsEmpty())
        hr = pInst->acb_createAnswer(v0, cb, args);
    else
        hr = pInst->ac_createAnswer(v0, vr);

    METHOD_RETURN();
}

inline void RTCPeerConnection_base::s_getStats(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<NMap> vr;

    ASYNC_METHOD_INSTANCE(RTCPeerConnection_base);
    ASYNC_METHOD_ENTER("RTCPeerConnection.getStats");

    METHOD_OVER(0, 0);

    if (!cb.IsEmpty())
        hr = pInst->acb_getStats(cb, args);
    else
        hr = pInst->ac_getStats(vr);

    METHOD_RETURN();
}

inline void RTCPeerConnection_base::s_close(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_INSTANCE(RTCPeerConnection_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->close();

    METHOD_VOID();
}

inline void RTCPeerConnection_base::s_get_connectionState(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_INSTANCE(RTCPeerConnection_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->get_connectionState(vr);

    METHOD_RETURN();
}

inline void RTCPeerConnection_base::s_get_iceConnectionState(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_INSTANCE(RTCPeerConnection_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->get_iceConnectionState(vr);

    METHOD_RETURN();
}

inline void RTCPeerConnection_base::s_get_iceGatheringState(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_INSTANCE(RTCPeerConnection_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->get_iceGatheringState(vr);

    METHOD_RETURN();
}

inline void RTCPeerConnection_base::s_get_localDescription(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Object> vr;

    METHOD_INSTANCE(RTCPeerConnection_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->get_localDescription(vr);

    METHOD_RETURN();
}

inline void RTCPeerConnection_base::s_get_remoteDescription(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Object> vr;

    METHOD_INSTANCE(RTCPeerConnection_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->get_remoteDescription(vr);

    METHOD_RETURN();
}

inline void RTCPeerConnection_base::s_get_remoteFingerprint(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Object> vr;

    METHOD_INSTANCE(RTCPeerConnection_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->get_remoteFingerprint(vr);

    METHOD_RETURN();
}

inline void RTCPeerConnection_base::s_get_signalingState(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_INSTANCE(RTCPeerConnection_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->get_signalingState(vr);

    METHOD_RETURN();
}

inline void RTCPeerConnection_base::s_get_onconnectionstatechange(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Function> vr;

    METHOD_INSTANCE(RTCPeerConnection_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->getListener("connectionstatechange", vr);

    METHOD_RETURN();
}

inline void RTCPeerConnection_base::s_set_onconnectionstatechange(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_INSTANCE(RTCPeerConnection_base);
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(v8::Local<v8::Function>, 0);

    hr = pInst->setListener("connectionstatechange", v0);

    METHOD_VOID();
}

inline void RTCPeerConnection_base::s_get_ondatachannel(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Function> vr;

    METHOD_INSTANCE(RTCPeerConnection_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->getListener("datachannel", vr);

    METHOD_RETURN();
}

inline void RTCPeerConnection_base::s_set_ondatachannel(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_INSTANCE(RTCPeerConnection_base);
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(v8::Local<v8::Function>, 0);

    hr = pInst->setListener("datachannel", v0);

    METHOD_VOID();
}

inline void RTCPeerConnection_base::s_get_onicecandidate(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Function> vr;

    METHOD_INSTANCE(RTCPeerConnection_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->getListener("icecandidate", vr);

    METHOD_RETURN();
}

inline void RTCPeerConnection_base::s_set_onicecandidate(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_INSTANCE(RTCPeerConnection_base);
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(v8::Local<v8::Function>, 0);

    hr = pInst->setListener("icecandidate", v0);

    METHOD_VOID();
}

inline void RTCPeerConnection_base::s_get_oniceconnectionstatechange(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Function> vr;

    METHOD_INSTANCE(RTCPeerConnection_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->getListener("iceconnectionstatechange", vr);

    METHOD_RETURN();
}

inline void RTCPeerConnection_base::s_set_oniceconnectionstatechange(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_INSTANCE(RTCPeerConnection_base);
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(v8::Local<v8::Function>, 0);

    hr = pInst->setListener("iceconnectionstatechange", v0);

    METHOD_VOID();
}

inline void RTCPeerConnection_base::s_get_onicegatheringstatechange(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Function> vr;

    METHOD_INSTANCE(RTCPeerConnection_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->getListener("icegatheringstatechange", vr);

    METHOD_RETURN();
}

inline void RTCPeerConnection_base::s_set_onicegatheringstatechange(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_INSTANCE(RTCPeerConnection_base);
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(v8::Local<v8::Function>, 0);

    hr = pInst->setListener("icegatheringstatechange", v0);

    METHOD_VOID();
}

inline void RTCPeerConnection_base::s_get_onlocaldescription(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Function> vr;

    METHOD_INSTANCE(RTCPeerConnection_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->getListener("localdescription", vr);

    METHOD_RETURN();
}

inline void RTCPeerConnection_base::s_set_onlocaldescription(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_INSTANCE(RTCPeerConnection_base);
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(v8::Local<v8::Function>, 0);

    hr = pInst->setListener("localdescription", v0);

    METHOD_VOID();
}

inline void RTCPeerConnection_base::s_get_onsignalingstatechange(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Function> vr;

    METHOD_INSTANCE(RTCPeerConnection_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->getListener("signalingstatechange", vr);

    METHOD_RETURN();
}

inline void RTCPeerConnection_base::s_set_onsignalingstatechange(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_INSTANCE(RTCPeerConnection_base);
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(v8::Local<v8::Function>, 0);

    hr = pInst->setListener("signalingstatechange", v0);

    METHOD_VOID();
}

inline void RTCPeerConnection_base::s_get_ontrack(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Function> vr;

    METHOD_INSTANCE(RTCPeerConnection_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->getListener("track", vr);

    METHOD_RETURN();
}

inline void RTCPeerConnection_base::s_set_ontrack(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_INSTANCE(RTCPeerConnection_base);
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(v8::Local<v8::Function>, 0);

    hr = pInst->setListener("track", v0);

    METHOD_VOID();
}
}
