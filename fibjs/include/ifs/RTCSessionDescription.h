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

class RTCSessionDescription_base : public object_base {
    DECLARE_CLASS(RTCSessionDescription_base);

public:
    // RTCSessionDescription_base
    static result_t _new(v8::Local<v8::Object> description, obj_ptr<RTCSessionDescription_base>& retVal, v8::Local<v8::Object> This = v8::Local<v8::Object>());
    virtual result_t get_type(exlib::string& retVal) = 0;
    virtual result_t get_sdp(exlib::string& retVal) = 0;

public:
    template <typename T>
    static void __new(const T& args);

public:
    static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_type(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_sdp(const v8::FunctionCallbackInfo<v8::Value>& args);
};
}

namespace fibjs {
inline ClassInfo& RTCSessionDescription_base::class_info()
{
    static ClassData::ClassProperty s_property[] = {
        { "type", s_get_type, block_set, false },
        { "sdp", s_get_sdp, block_set, false }
    };

    static ClassData s_cd = {
        "RTCSessionDescription", false, s__new, NULL,
        0, NULL, 0, NULL, ARRAYSIZE(s_property), s_property, 0, NULL, NULL, NULL,
        &object_base::class_info(),
        false
    };

    static ClassInfo s_ci(s_cd);
    return s_ci;
}

inline void RTCSessionDescription_base::s__new(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    CONSTRUCT_INIT();
    __new(args);
}

template <typename T>
void RTCSessionDescription_base::__new(const T& args)
{
    obj_ptr<RTCSessionDescription_base> vr;

    CONSTRUCT_ENTER();

    METHOD_OVER(1, 0);

    OPT_ARG(v8::Local<v8::Object>, 0, v8::Object::New(isolate->m_isolate));

    hr = _new(v0, vr, args.This());

    CONSTRUCT_RETURN();
}

inline void RTCSessionDescription_base::s_get_type(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_INSTANCE(RTCSessionDescription_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->get_type(vr);

    METHOD_RETURN();
}

inline void RTCSessionDescription_base::s_get_sdp(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_INSTANCE(RTCSessionDescription_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->get_sdp(vr);

    METHOD_RETURN();
}
}
