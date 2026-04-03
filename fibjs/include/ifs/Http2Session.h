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
class Stream_base;
class Http2Stream_base;

class Http2Session_base : public EventEmitter_base {
    DECLARE_CLASS(Http2Session_base);
    EVENT_SUPPORT();

public:
    // Http2Session_base
    virtual result_t get_remoteSettings(v8::Local<v8::Object>& retVal) = 0;
    virtual result_t get_localSettings(v8::Local<v8::Object>& retVal) = 0;
    virtual result_t get_destroyed(bool& retVal) = 0;
    virtual result_t get_closed(bool& retVal) = 0;
    virtual result_t get_alpnProtocol(exlib::string& retVal) = 0;
    virtual result_t get_socket(obj_ptr<Stream_base>& retVal) = 0;
    virtual result_t request(v8::Local<v8::Object> headers, v8::Local<v8::Object> options, obj_ptr<Http2Stream_base>& retVal) = 0;
    virtual result_t goaway(int32_t code, int32_t lastStreamId) = 0;
    virtual result_t ping(int32_t& retVal, AsyncEvent* ac) = 0;
    virtual result_t settings(v8::Local<v8::Object> settings) = 0;
    virtual result_t close(AsyncEvent* ac) = 0;
    virtual result_t destroy() = 0;

public:
    static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        CONSTRUCT_INIT();

        ThrowTypeError("not a constructor");
    }

    static result_t load(v8::Local<v8::Value> v, obj_ptr<Http2Session_base>& retVal)
    { return CALL_E_TYPEMISMATCH; }

public:
    static void s_get_remoteSettings(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_localSettings(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_destroyed(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_closed(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_alpnProtocol(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_socket(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_request(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_goaway(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_ping(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_settings(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_close(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_destroy(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_onstream(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_set_onstream(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_ongoaway(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_set_ongoaway(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_onerror(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_set_onerror(const v8::FunctionCallbackInfo<v8::Value>& args);

public:
    ASYNC_MEMBERVALUE1(Http2Session_base, ping, int32_t);
    ASYNC_MEMBER0(Http2Session_base, close);
};
}

#include "ifs/Stream.h"
#include "ifs/Http2Stream.h"

namespace fibjs {
inline ClassInfo& Http2Session_base::class_info()
{
    static ClassData::ClassMethod s_method[] = {
        { "request", s_request, false, ClassData::ASYNC_SYNC },
        { "goaway", s_goaway, false, ClassData::ASYNC_SYNC },
        { "ping", s_ping, false, ClassData::ASYNC_ASYNC },
        { "settings", s_settings, false, ClassData::ASYNC_SYNC },
        { "close", s_close, false, ClassData::ASYNC_ASYNC },
        { "destroy", s_destroy, false, ClassData::ASYNC_SYNC }
    };

    static ClassData::ClassProperty s_property[] = {
        { "remoteSettings", s_get_remoteSettings, block_set, false },
        { "localSettings", s_get_localSettings, block_set, false },
        { "destroyed", s_get_destroyed, block_set, false },
        { "closed", s_get_closed, block_set, false },
        { "alpnProtocol", s_get_alpnProtocol, block_set, false },
        { "socket", s_get_socket, block_set, false },
        { "onstream", s_get_onstream, s_set_onstream, false },
        { "ongoaway", s_get_ongoaway, s_set_ongoaway, false },
        { "onerror", s_get_onerror, s_set_onerror, false }
    };

    static ClassData s_cd = {
        "Http2Session", false, s__new, NULL,
        ARRAYSIZE(s_method), s_method, 0, NULL, ARRAYSIZE(s_property), s_property, 0, NULL, NULL, NULL,
        &EventEmitter_base::class_info(),
        true
    };

    static ClassInfo s_ci(s_cd);
    return s_ci;
}

inline void Http2Session_base::s_get_remoteSettings(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Object> vr;

    METHOD_INSTANCE(Http2Session_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->get_remoteSettings(vr);

    METHOD_RETURN();
}

inline void Http2Session_base::s_get_localSettings(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Object> vr;

    METHOD_INSTANCE(Http2Session_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->get_localSettings(vr);

    METHOD_RETURN();
}

inline void Http2Session_base::s_get_destroyed(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    bool vr;

    METHOD_INSTANCE(Http2Session_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->get_destroyed(vr);

    METHOD_RETURN();
}

inline void Http2Session_base::s_get_closed(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    bool vr;

    METHOD_INSTANCE(Http2Session_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->get_closed(vr);

    METHOD_RETURN();
}

inline void Http2Session_base::s_get_alpnProtocol(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_INSTANCE(Http2Session_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->get_alpnProtocol(vr);

    METHOD_RETURN();
}

inline void Http2Session_base::s_get_socket(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<Stream_base> vr;

    METHOD_INSTANCE(Http2Session_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->get_socket(vr);

    METHOD_RETURN();
}

inline void Http2Session_base::s_request(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<Http2Stream_base> vr;

    METHOD_INSTANCE(Http2Session_base);
    METHOD_ENTER();

    METHOD_OVER(2, 1);

    ARG(v8::Local<v8::Object>, 0);
    OPT_ARG(v8::Local<v8::Object>, 1, v8::Object::New(isolate->m_isolate));

    hr = pInst->request(v0, v1, vr);

    METHOD_RETURN();
}

inline void Http2Session_base::s_goaway(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_INSTANCE(Http2Session_base);
    METHOD_ENTER();

    METHOD_OVER(2, 0);

    OPT_ARG(int32_t, 0, 0);
    OPT_ARG(int32_t, 1, 0);

    hr = pInst->goaway(v0, v1);

    METHOD_VOID();
}

inline void Http2Session_base::s_ping(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    int32_t vr;

    ASYNC_METHOD_INSTANCE(Http2Session_base);
    ASYNC_METHOD_ENTER("Http2Session.ping");

    METHOD_OVER(0, 0);

    if (!cb.IsEmpty())
        hr = pInst->acb_ping(cb, args);
    else
        hr = pInst->ac_ping(vr);

    ASYNC_METHOD_RETURN();
}

inline void Http2Session_base::s_settings(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_INSTANCE(Http2Session_base);
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(v8::Local<v8::Object>, 0);

    hr = pInst->settings(v0);

    METHOD_VOID();
}

inline void Http2Session_base::s_close(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    ASYNC_METHOD_INSTANCE(Http2Session_base);
    ASYNC_METHOD_ENTER("Http2Session.close");

    METHOD_OVER(0, 0);

    if (!cb.IsEmpty())
        hr = pInst->acb_close(cb, args);
    else
        hr = pInst->ac_close();

    ASYNC_METHOD_VOID();
}

inline void Http2Session_base::s_destroy(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_INSTANCE(Http2Session_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->destroy();

    METHOD_VOID();
}

inline void Http2Session_base::s_get_onstream(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Function> vr;

    METHOD_INSTANCE(Http2Session_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->getListener("stream", vr);

    METHOD_RETURN();
}

inline void Http2Session_base::s_set_onstream(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_INSTANCE(Http2Session_base);
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(v8::Local<v8::Function>, 0);

    hr = pInst->setListener("stream", v0);

    METHOD_VOID();
}

inline void Http2Session_base::s_get_ongoaway(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Function> vr;

    METHOD_INSTANCE(Http2Session_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->getListener("goaway", vr);

    METHOD_RETURN();
}

inline void Http2Session_base::s_set_ongoaway(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_INSTANCE(Http2Session_base);
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(v8::Local<v8::Function>, 0);

    hr = pInst->setListener("goaway", v0);

    METHOD_VOID();
}

inline void Http2Session_base::s_get_onerror(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Function> vr;

    METHOD_INSTANCE(Http2Session_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->getListener("error", vr);

    METHOD_RETURN();
}

inline void Http2Session_base::s_set_onerror(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_INSTANCE(Http2Session_base);
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(v8::Local<v8::Function>, 0);

    hr = pInst->setListener("error", v0);

    METHOD_VOID();
}
}
