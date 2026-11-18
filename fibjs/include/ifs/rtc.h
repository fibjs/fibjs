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

class RTCPeerConnection_base;
class RTCSessionDescription_base;
class RTCIceCandidate_base;
class RTCListener_base;
class RTCServerOptions;
class RTCSctpSettings;

class rtc_base : public object_base {
    DECLARE_CLASS(rtc_base);

public:
    // rtc_base
    static result_t startServer(RTCServerOptions* config);
    static result_t setSctpSettings(RTCSctpSettings* settings);
    static result_t get_loglevel(exlib::string& retVal);
    static result_t set_loglevel(exlib::string newVal);

public:
    static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        CONSTRUCT_INIT();

        ThrowTypeError("not a constructor");
    }

    static result_t load(v8::Local<v8::Value> v, obj_ptr<rtc_base>& retVal)
    { return result_t::type_mismatch(); }

public:
    static void s_static_startServer(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_setSctpSettings(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_get_loglevel(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_set_loglevel(const v8::FunctionCallbackInfo<v8::Value>& args);
};
}

#include "ifs/RTCPeerConnection.h"
#include "ifs/RTCSessionDescription.h"
#include "ifs/RTCIceCandidate.h"
#include "ifs/RTCListener.h"
#include "ifs/RTCServerOptions.h"
#include "ifs/RTCSctpSettings.h"

namespace fibjs {
inline ClassInfo& rtc_base::class_info()
{
    static ClassData::ClassMethod s_method[] = {
        { "startServer", s_static_startServer, true, ClassData::ASYNC_SYNC },
        { "setSctpSettings", s_static_setSctpSettings, true, ClassData::ASYNC_SYNC }
    };

    static ClassData::ClassObject s_object[] = {
        { "RTCPeerConnection", RTCPeerConnection_base::class_info },
        { "RTCSessionDescription", RTCSessionDescription_base::class_info },
        { "RTCIceCandidate", RTCIceCandidate_base::class_info },
        { "RTCListener", RTCListener_base::class_info }
    };

    static ClassData::ClassProperty s_property[] = {
        { "loglevel", s_static_get_loglevel, s_static_set_loglevel, true }
    };

    static ClassData s_cd = {
        "rtc", true, s__new, NULL,
        ARRAYSIZE(s_method), s_method, ARRAYSIZE(s_object), s_object, ARRAYSIZE(s_property), s_property, 0, NULL, NULL, NULL,
        &object_base::class_info(),
        false
    };

    static ClassInfo s_ci(s_cd);
    return s_ci;
}

inline void rtc_base::s_static_startServer(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_ENTER();

    METHOD_OVER_NAMED(1, 1, "config");

    ARG_NAMED(obj_ptr<RTCServerOptions>, 0, "config");

    hr = startServer((RTCServerOptions*)v0);

    METHOD_VOID();
}

inline void rtc_base::s_static_setSctpSettings(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_ENTER();

    METHOD_OVER_NAMED(1, 1, "settings");

    ARG_NAMED(obj_ptr<RTCSctpSettings>, 0, "settings");

    hr = setSctpSettings((RTCSctpSettings*)v0);

    METHOD_VOID();
}

inline void rtc_base::s_static_get_loglevel(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = get_loglevel(vr);

    METHOD_RETURN();
}

inline void rtc_base::s_static_set_loglevel(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG_NAMED(exlib::string, 0, "loglevel");

    hr = set_loglevel(v0);

    METHOD_VOID();
}
}
