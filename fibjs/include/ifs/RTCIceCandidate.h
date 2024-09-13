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

namespace fibjs {

class RTCIceCandidate_base : public object_base {
    DECLARE_CLASS(RTCIceCandidate_base);

public:
    // RTCIceCandidate_base
    static result_t _new(v8::Local<v8::Object> description, obj_ptr<RTCIceCandidate_base>& retVal, v8::Local<v8::Object> This = v8::Local<v8::Object>());
    virtual result_t get_candidate(exlib::string& retVal) = 0;
    virtual result_t get_sdpMid(exlib::string& retVal) = 0;
    virtual result_t get_priority(int32_t& retVal) = 0;
    virtual result_t get_transport(exlib::string& retVal) = 0;
    virtual result_t get_address(exlib::string& retVal) = 0;
    virtual result_t get_port(int32_t& retVal) = 0;
    virtual result_t get_type(exlib::string& retVal) = 0;

public:
    template <typename T>
    static void __new(const T& args);

public:
    static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_candidate(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_sdpMid(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_priority(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_transport(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_address(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_port(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_type(const v8::FunctionCallbackInfo<v8::Value>& args);
};
}

namespace fibjs {
inline ClassInfo& RTCIceCandidate_base::class_info()
{
    static ClassData::ClassProperty s_property[] = {
        { "candidate", s_get_candidate, block_set, false },
        { "sdpMid", s_get_sdpMid, block_set, false },
        { "priority", s_get_priority, block_set, false },
        { "transport", s_get_transport, block_set, false },
        { "address", s_get_address, block_set, false },
        { "port", s_get_port, block_set, false },
        { "type", s_get_type, block_set, false }
    };

    static ClassData s_cd = {
        "RTCIceCandidate", false, s__new, NULL,
        0, NULL, 0, NULL, ARRAYSIZE(s_property), s_property, 0, NULL, NULL, NULL,
        &object_base::class_info(),
        false
    };

    static ClassInfo s_ci(s_cd);
    return s_ci;
}

inline void RTCIceCandidate_base::s__new(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    CONSTRUCT_INIT();
    __new(args);
}

template <typename T>
void RTCIceCandidate_base::__new(const T& args)
{
    obj_ptr<RTCIceCandidate_base> vr;

    CONSTRUCT_ENTER();

    METHOD_OVER(1, 0);

    OPT_ARG(v8::Local<v8::Object>, 0, v8::Object::New(isolate->m_isolate));

    hr = _new(v0, vr, args.This());

    CONSTRUCT_RETURN();
}

inline void RTCIceCandidate_base::s_get_candidate(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_INSTANCE(RTCIceCandidate_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->get_candidate(vr);

    METHOD_RETURN();
}

inline void RTCIceCandidate_base::s_get_sdpMid(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_INSTANCE(RTCIceCandidate_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->get_sdpMid(vr);

    METHOD_RETURN();
}

inline void RTCIceCandidate_base::s_get_priority(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    int32_t vr;

    METHOD_INSTANCE(RTCIceCandidate_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->get_priority(vr);

    METHOD_RETURN();
}

inline void RTCIceCandidate_base::s_get_transport(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_INSTANCE(RTCIceCandidate_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->get_transport(vr);

    METHOD_RETURN();
}

inline void RTCIceCandidate_base::s_get_address(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_INSTANCE(RTCIceCandidate_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->get_address(vr);

    METHOD_RETURN();
}

inline void RTCIceCandidate_base::s_get_port(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    int32_t vr;

    METHOD_INSTANCE(RTCIceCandidate_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->get_port(vr);

    METHOD_RETURN();
}

inline void RTCIceCandidate_base::s_get_type(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_INSTANCE(RTCIceCandidate_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->get_type(vr);

    METHOD_RETURN();
}
}
