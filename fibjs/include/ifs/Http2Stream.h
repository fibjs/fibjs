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
#include "ifs/Stream.h"

namespace fibjs {

class Stream_base;

class Http2Stream_base : public Stream_base {
    DECLARE_CLASS(Http2Stream_base);

public:
    // Http2Stream_base
    virtual result_t get_id(int32_t& retVal) = 0;
    virtual result_t get_closed(bool& retVal) = 0;
    virtual result_t get_destroyed(bool& retVal) = 0;
    virtual result_t get_headers(obj_ptr<NObject>& retVal) = 0;
    virtual result_t respond(v8::Local<v8::Object> headers) = 0;
    virtual result_t additionalHeaders(v8::Local<v8::Object> headers) = 0;
    virtual result_t sendTrailers(v8::Local<v8::Object> headers) = 0;
    virtual result_t rstStream(int32_t code) = 0;

public:
    static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        CONSTRUCT_INIT();

        ThrowTypeError("not a constructor");
    }

    static result_t load(v8::Local<v8::Value> v, obj_ptr<Http2Stream_base>& retVal)
    { return CALL_E_TYPEMISMATCH; }

public:
    static void s_get_id(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_closed(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_destroyed(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_headers(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_respond(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_additionalHeaders(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_sendTrailers(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_rstStream(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_onheaders(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_set_onheaders(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_ontrailers(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_set_ontrailers(const v8::FunctionCallbackInfo<v8::Value>& args);
};
}

namespace fibjs {
inline ClassInfo& Http2Stream_base::class_info()
{
    static ClassData::ClassMethod s_method[] = {
        { "respond", s_respond, false, ClassData::ASYNC_SYNC },
        { "additionalHeaders", s_additionalHeaders, false, ClassData::ASYNC_SYNC },
        { "sendTrailers", s_sendTrailers, false, ClassData::ASYNC_SYNC },
        { "rstStream", s_rstStream, false, ClassData::ASYNC_SYNC }
    };

    static ClassData::ClassProperty s_property[] = {
        { "id", s_get_id, block_set, false },
        { "closed", s_get_closed, block_set, false },
        { "destroyed", s_get_destroyed, block_set, false },
        { "headers", s_get_headers, block_set, false },
        { "onheaders", s_get_onheaders, s_set_onheaders, false },
        { "ontrailers", s_get_ontrailers, s_set_ontrailers, false }
    };

    static ClassData s_cd = {
        "Http2Stream", false, s__new, NULL,
        ARRAYSIZE(s_method), s_method, 0, NULL, ARRAYSIZE(s_property), s_property, 0, NULL, NULL, NULL,
        &Stream_base::class_info(),
        true
    };

    static ClassInfo s_ci(s_cd);
    return s_ci;
}

inline void Http2Stream_base::s_get_id(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    int32_t vr;

    METHOD_INSTANCE(Http2Stream_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->get_id(vr);

    METHOD_RETURN();
}

inline void Http2Stream_base::s_get_closed(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    bool vr;

    METHOD_INSTANCE(Http2Stream_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->get_closed(vr);

    METHOD_RETURN();
}

inline void Http2Stream_base::s_get_destroyed(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    bool vr;

    METHOD_INSTANCE(Http2Stream_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->get_destroyed(vr);

    METHOD_RETURN();
}

inline void Http2Stream_base::s_get_headers(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<NObject> vr;

    METHOD_INSTANCE(Http2Stream_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->get_headers(vr);

    METHOD_RETURN();
}

inline void Http2Stream_base::s_respond(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_INSTANCE(Http2Stream_base);
    METHOD_ENTER();

    METHOD_OVER(1, 0);

    OPT_ARG(v8::Local<v8::Object>, 0, v8::Object::New(isolate->m_isolate));

    hr = pInst->respond(v0);

    METHOD_VOID();
}

inline void Http2Stream_base::s_additionalHeaders(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_INSTANCE(Http2Stream_base);
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(v8::Local<v8::Object>, 0);

    hr = pInst->additionalHeaders(v0);

    METHOD_VOID();
}

inline void Http2Stream_base::s_sendTrailers(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_INSTANCE(Http2Stream_base);
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(v8::Local<v8::Object>, 0);

    hr = pInst->sendTrailers(v0);

    METHOD_VOID();
}

inline void Http2Stream_base::s_rstStream(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_INSTANCE(Http2Stream_base);
    METHOD_ENTER();

    METHOD_OVER(1, 0);

    OPT_ARG(int32_t, 0, 0);

    hr = pInst->rstStream(v0);

    METHOD_VOID();
}

inline void Http2Stream_base::s_get_onheaders(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Function> vr;

    METHOD_INSTANCE(Http2Stream_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->getListener("headers", vr);

    METHOD_RETURN();
}

inline void Http2Stream_base::s_set_onheaders(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_INSTANCE(Http2Stream_base);
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(v8::Local<v8::Function>, 0);

    hr = pInst->setListener("headers", v0);

    METHOD_VOID();
}

inline void Http2Stream_base::s_get_ontrailers(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Function> vr;

    METHOD_INSTANCE(Http2Stream_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->getListener("trailers", vr);

    METHOD_RETURN();
}

inline void Http2Stream_base::s_set_ontrailers(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_INSTANCE(Http2Stream_base);
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(v8::Local<v8::Function>, 0);

    hr = pInst->setListener("trailers", v0);

    METHOD_VOID();
}
}
