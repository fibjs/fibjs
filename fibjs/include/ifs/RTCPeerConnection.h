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
    virtual result_t get_signalingState(exlib::string& retVal) = 0;
    virtual result_t get_onconnectionstatechange(v8::Local<v8::Function>& retVal) = 0;
    virtual result_t set_onconnectionstatechange(v8::Local<v8::Function> newVal) = 0;
    virtual result_t get_ondatachannel(v8::Local<v8::Function>& retVal) = 0;
    virtual result_t set_ondatachannel(v8::Local<v8::Function> newVal) = 0;
    virtual result_t get_onicecandidate(v8::Local<v8::Function>& retVal) = 0;
    virtual result_t set_onicecandidate(v8::Local<v8::Function> newVal) = 0;
    virtual result_t get_oniceconnectionstatechange(v8::Local<v8::Function>& retVal) = 0;
    virtual result_t set_oniceconnectionstatechange(v8::Local<v8::Function> newVal) = 0;
    virtual result_t get_onicegatheringstatechange(v8::Local<v8::Function>& retVal) = 0;
    virtual result_t set_onicegatheringstatechange(v8::Local<v8::Function> newVal) = 0;
    virtual result_t get_onlocaldescription(v8::Local<v8::Function>& retVal) = 0;
    virtual result_t set_onlocaldescription(v8::Local<v8::Function> newVal) = 0;
    virtual result_t get_onsignalingstatechange(v8::Local<v8::Function>& retVal) = 0;
    virtual result_t set_onsignalingstatechange(v8::Local<v8::Function> newVal) = 0;
    virtual result_t get_ontrack(v8::Local<v8::Function>& retVal) = 0;
    virtual result_t set_ontrack(v8::Local<v8::Function> newVal) = 0;

public:
    template <typename T>
    static void __new(const T& args);

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
    static void s_get_connectionState(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_get_iceConnectionState(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_get_iceGatheringState(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_get_localDescription(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_get_remoteDescription(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_get_signalingState(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_get_onconnectionstatechange(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_set_onconnectionstatechange(v8::Local<v8::Name> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args);
    static void s_get_ondatachannel(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_set_ondatachannel(v8::Local<v8::Name> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args);
    static void s_get_onicecandidate(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_set_onicecandidate(v8::Local<v8::Name> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args);
    static void s_get_oniceconnectionstatechange(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_set_oniceconnectionstatechange(v8::Local<v8::Name> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args);
    static void s_get_onicegatheringstatechange(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_set_onicegatheringstatechange(v8::Local<v8::Name> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args);
    static void s_get_onlocaldescription(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_set_onlocaldescription(v8::Local<v8::Name> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args);
    static void s_get_onsignalingstatechange(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_set_onsignalingstatechange(v8::Local<v8::Name> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args);
    static void s_get_ontrack(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_set_ontrack(v8::Local<v8::Name> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args);

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

template <typename T>
void RTCPeerConnection_base::__new(const T& args)
{
    obj_ptr<RTCPeerConnection_base> vr;

    CONSTRUCT_ENTER();

    METHOD_OVER(1, 0);

    OPT_ARG(v8::Local<v8::Object>, 0, v8::Object::New(isolate->m_isolate));

    hr = _new(v0, vr, args.This());

    CONSTRUCT_RETURN();
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
    METHOD_ENTER();

    ASYNC_METHOD_OVER(0, 0);

    if (!cb.IsEmpty())
        hr = pInst->acb_setLocalDescription(cb, args);
    else
        hr = pInst->ac_setLocalDescription();

    ASYNC_METHOD_OVER(1, 1);

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
    METHOD_ENTER();

    ASYNC_METHOD_OVER(1, 1);

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
    METHOD_ENTER();

    ASYNC_METHOD_OVER(1, 1);

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
    METHOD_ENTER();

    ASYNC_METHOD_OVER(1, 0);

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
    METHOD_ENTER();

    ASYNC_METHOD_OVER(1, 0);

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
    METHOD_ENTER();

    ASYNC_METHOD_OVER(0, 0);

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

inline void RTCPeerConnection_base::s_get_connectionState(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_INSTANCE(RTCPeerConnection_base);
    PROPERTY_ENTER();

    hr = pInst->get_connectionState(vr);

    METHOD_RETURN();
}

inline void RTCPeerConnection_base::s_get_iceConnectionState(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_INSTANCE(RTCPeerConnection_base);
    PROPERTY_ENTER();

    hr = pInst->get_iceConnectionState(vr);

    METHOD_RETURN();
}

inline void RTCPeerConnection_base::s_get_iceGatheringState(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_INSTANCE(RTCPeerConnection_base);
    PROPERTY_ENTER();

    hr = pInst->get_iceGatheringState(vr);

    METHOD_RETURN();
}

inline void RTCPeerConnection_base::s_get_localDescription(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Object> vr;

    METHOD_INSTANCE(RTCPeerConnection_base);
    PROPERTY_ENTER();

    hr = pInst->get_localDescription(vr);

    METHOD_RETURN();
}

inline void RTCPeerConnection_base::s_get_remoteDescription(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Object> vr;

    METHOD_INSTANCE(RTCPeerConnection_base);
    PROPERTY_ENTER();

    hr = pInst->get_remoteDescription(vr);

    METHOD_RETURN();
}

inline void RTCPeerConnection_base::s_get_signalingState(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_INSTANCE(RTCPeerConnection_base);
    PROPERTY_ENTER();

    hr = pInst->get_signalingState(vr);

    METHOD_RETURN();
}

inline void RTCPeerConnection_base::s_get_onconnectionstatechange(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Function> vr;

    METHOD_INSTANCE(RTCPeerConnection_base);
    PROPERTY_ENTER();

    hr = pInst->get_onconnectionstatechange(vr);

    METHOD_RETURN();
}

inline void RTCPeerConnection_base::s_set_onconnectionstatechange(v8::Local<v8::Name> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args)
{
    METHOD_INSTANCE(RTCPeerConnection_base);
    PROPERTY_ENTER();
    PROPERTY_VAL(v8::Local<v8::Function>);

    hr = pInst->set_onconnectionstatechange(v0);

    PROPERTY_SET_LEAVE();
}

inline void RTCPeerConnection_base::s_get_ondatachannel(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Function> vr;

    METHOD_INSTANCE(RTCPeerConnection_base);
    PROPERTY_ENTER();

    hr = pInst->get_ondatachannel(vr);

    METHOD_RETURN();
}

inline void RTCPeerConnection_base::s_set_ondatachannel(v8::Local<v8::Name> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args)
{
    METHOD_INSTANCE(RTCPeerConnection_base);
    PROPERTY_ENTER();
    PROPERTY_VAL(v8::Local<v8::Function>);

    hr = pInst->set_ondatachannel(v0);

    PROPERTY_SET_LEAVE();
}

inline void RTCPeerConnection_base::s_get_onicecandidate(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Function> vr;

    METHOD_INSTANCE(RTCPeerConnection_base);
    PROPERTY_ENTER();

    hr = pInst->get_onicecandidate(vr);

    METHOD_RETURN();
}

inline void RTCPeerConnection_base::s_set_onicecandidate(v8::Local<v8::Name> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args)
{
    METHOD_INSTANCE(RTCPeerConnection_base);
    PROPERTY_ENTER();
    PROPERTY_VAL(v8::Local<v8::Function>);

    hr = pInst->set_onicecandidate(v0);

    PROPERTY_SET_LEAVE();
}

inline void RTCPeerConnection_base::s_get_oniceconnectionstatechange(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Function> vr;

    METHOD_INSTANCE(RTCPeerConnection_base);
    PROPERTY_ENTER();

    hr = pInst->get_oniceconnectionstatechange(vr);

    METHOD_RETURN();
}

inline void RTCPeerConnection_base::s_set_oniceconnectionstatechange(v8::Local<v8::Name> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args)
{
    METHOD_INSTANCE(RTCPeerConnection_base);
    PROPERTY_ENTER();
    PROPERTY_VAL(v8::Local<v8::Function>);

    hr = pInst->set_oniceconnectionstatechange(v0);

    PROPERTY_SET_LEAVE();
}

inline void RTCPeerConnection_base::s_get_onicegatheringstatechange(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Function> vr;

    METHOD_INSTANCE(RTCPeerConnection_base);
    PROPERTY_ENTER();

    hr = pInst->get_onicegatheringstatechange(vr);

    METHOD_RETURN();
}

inline void RTCPeerConnection_base::s_set_onicegatheringstatechange(v8::Local<v8::Name> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args)
{
    METHOD_INSTANCE(RTCPeerConnection_base);
    PROPERTY_ENTER();
    PROPERTY_VAL(v8::Local<v8::Function>);

    hr = pInst->set_onicegatheringstatechange(v0);

    PROPERTY_SET_LEAVE();
}

inline void RTCPeerConnection_base::s_get_onlocaldescription(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Function> vr;

    METHOD_INSTANCE(RTCPeerConnection_base);
    PROPERTY_ENTER();

    hr = pInst->get_onlocaldescription(vr);

    METHOD_RETURN();
}

inline void RTCPeerConnection_base::s_set_onlocaldescription(v8::Local<v8::Name> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args)
{
    METHOD_INSTANCE(RTCPeerConnection_base);
    PROPERTY_ENTER();
    PROPERTY_VAL(v8::Local<v8::Function>);

    hr = pInst->set_onlocaldescription(v0);

    PROPERTY_SET_LEAVE();
}

inline void RTCPeerConnection_base::s_get_onsignalingstatechange(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Function> vr;

    METHOD_INSTANCE(RTCPeerConnection_base);
    PROPERTY_ENTER();

    hr = pInst->get_onsignalingstatechange(vr);

    METHOD_RETURN();
}

inline void RTCPeerConnection_base::s_set_onsignalingstatechange(v8::Local<v8::Name> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args)
{
    METHOD_INSTANCE(RTCPeerConnection_base);
    PROPERTY_ENTER();
    PROPERTY_VAL(v8::Local<v8::Function>);

    hr = pInst->set_onsignalingstatechange(v0);

    PROPERTY_SET_LEAVE();
}

inline void RTCPeerConnection_base::s_get_ontrack(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Function> vr;

    METHOD_INSTANCE(RTCPeerConnection_base);
    PROPERTY_ENTER();

    hr = pInst->get_ontrack(vr);

    METHOD_RETURN();
}

inline void RTCPeerConnection_base::s_set_ontrack(v8::Local<v8::Name> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args)
{
    METHOD_INSTANCE(RTCPeerConnection_base);
    PROPERTY_ENTER();
    PROPERTY_VAL(v8::Local<v8::Function>);

    hr = pInst->set_ontrack(v0);

    PROPERTY_SET_LEAVE();
}
}
