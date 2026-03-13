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

class Buffer_base;
class URLSearchParams_base;
class UrlObject_base;
class Blob_base;
class File_base;
class Headers_base;
class FormData_base;
class HttpRequest_base;
class WebResponse_base;
class TextDecoder_base;
class TextEncoder_base;
class AbortController_base;
class AbortSignal_base;
class CryptoKey_base;
class DOMParser_base;
class XMLSerializer_base;
class XmlDocument_base;
class console_base;
class process_base;
class performance_base;
class PerformanceObserver_base;
class webcrypto_base;
class Timer_base;

class global_base : public object_base {
    DECLARE_CLASS(global_base);

public:
    // global_base
    static result_t get_global(v8::Local<v8::Object>& retVal);
    static result_t get_globalThis(v8::Local<v8::Object>& retVal);
    static result_t run(exlib::string fname);
    static result_t require(exlib::string id, v8::Local<v8::Value>& retVal);
    static result_t setTimeout(v8::Local<v8::Function> callback, double timeout, OptArgs args, obj_ptr<Timer_base>& retVal);
    static result_t clearTimeout(v8::Local<v8::Value> t);
    static result_t setInterval(v8::Local<v8::Function> callback, double timeout, OptArgs args, obj_ptr<Timer_base>& retVal);
    static result_t clearInterval(v8::Local<v8::Value> t);
    static result_t setHrInterval(v8::Local<v8::Function> callback, double timeout, OptArgs args, obj_ptr<Timer_base>& retVal);
    static result_t clearHrInterval(v8::Local<v8::Value> t);
    static result_t setImmediate(v8::Local<v8::Function> callback, OptArgs args, obj_ptr<Timer_base>& retVal);
    static result_t clearImmediate(v8::Local<v8::Value> t);
    static result_t btoa(exlib::string data, exlib::string& retVal);
    static result_t atob(exlib::string data, exlib::string& retVal);
    static result_t structuredClone(v8::Local<v8::Value> value, v8::Local<v8::Object> options, v8::Local<v8::Value>& retVal);
    static result_t fetch(exlib::string url, v8::Local<v8::Object> opts, obj_ptr<WebResponse_base>& retVal, AsyncEvent* ac);
    static result_t fetch(HttpRequest_base* request, v8::Local<v8::Object> opts, obj_ptr<WebResponse_base>& retVal, AsyncEvent* ac);
    static result_t queueMicrotask(v8::Local<v8::Function> callback);

public:
    static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        CONSTRUCT_INIT();

        ThrowTypeError("not a constructor");
    }

    static result_t load(v8::Local<v8::Value> v, obj_ptr<global_base>& retVal)
    { return CALL_E_TYPEMISMATCH; }

public:
    static void s_static_get_global(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_get_globalThis(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_run(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_require(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_setTimeout(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_clearTimeout(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_setInterval(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_clearInterval(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_setHrInterval(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_clearHrInterval(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_setImmediate(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_clearImmediate(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_btoa(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_atob(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_structuredClone(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_fetch(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_queueMicrotask(const v8::FunctionCallbackInfo<v8::Value>& args);

public:
    ASYNC_STATICVALUE3(global_base, fetch, exlib::string, v8::Local<v8::Object>, obj_ptr<WebResponse_base>);
    ASYNC_STATICVALUE3(global_base, fetch, HttpRequest_base*, v8::Local<v8::Object>, obj_ptr<WebResponse_base>);
};
}

#include "ifs/Buffer.h"
#include "ifs/URLSearchParams.h"
#include "ifs/UrlObject.h"
#include "ifs/Blob.h"
#include "ifs/File.h"
#include "ifs/Headers.h"
#include "ifs/FormData.h"
#include "ifs/HttpRequest.h"
#include "ifs/WebResponse.h"
#include "ifs/TextDecoder.h"
#include "ifs/TextEncoder.h"
#include "ifs/AbortController.h"
#include "ifs/AbortSignal.h"
#include "ifs/CryptoKey.h"
#include "ifs/DOMParser.h"
#include "ifs/XMLSerializer.h"
#include "ifs/XmlDocument.h"
#include "ifs/console.h"
#include "ifs/process.h"
#include "ifs/performance.h"
#include "ifs/PerformanceObserver.h"
#include "ifs/webcrypto.h"
#include "ifs/Timer.h"

namespace fibjs {
inline ClassInfo& global_base::class_info()
{
    static ClassData::ClassMethod s_method[] = {
        { "run", s_static_run, true, ClassData::ASYNC_SYNC },
        { "require", s_static_require, true, ClassData::ASYNC_SYNC },
        { "setTimeout", s_static_setTimeout, true, ClassData::ASYNC_SYNC },
        { "clearTimeout", s_static_clearTimeout, true, ClassData::ASYNC_SYNC },
        { "setInterval", s_static_setInterval, true, ClassData::ASYNC_SYNC },
        { "clearInterval", s_static_clearInterval, true, ClassData::ASYNC_SYNC },
        { "setHrInterval", s_static_setHrInterval, true, ClassData::ASYNC_SYNC },
        { "clearHrInterval", s_static_clearHrInterval, true, ClassData::ASYNC_SYNC },
        { "setImmediate", s_static_setImmediate, true, ClassData::ASYNC_SYNC },
        { "clearImmediate", s_static_clearImmediate, true, ClassData::ASYNC_SYNC },
        { "btoa", s_static_btoa, true, ClassData::ASYNC_SYNC },
        { "atob", s_static_atob, true, ClassData::ASYNC_SYNC },
        { "structuredClone", s_static_structuredClone, true, ClassData::ASYNC_SYNC },
        { "fetch", s_static_fetch, true, ClassData::ASYNC_PROMISE },
        { "queueMicrotask", s_static_queueMicrotask, true, ClassData::ASYNC_SYNC }
    };

    static ClassData::ClassObject s_object[] = {
        { "Buffer", Buffer_base::class_info },
        { "URLSearchParams", URLSearchParams_base::class_info },
        { "URL", UrlObject_base::class_info },
        { "Blob", Blob_base::class_info },
        { "File", File_base::class_info },
        { "Headers", Headers_base::class_info },
        { "FormData", FormData_base::class_info },
        { "Request", HttpRequest_base::class_info },
        { "Response", WebResponse_base::class_info },
        { "TextDecoder", TextDecoder_base::class_info },
        { "TextEncoder", TextEncoder_base::class_info },
        { "AbortController", AbortController_base::class_info },
        { "AbortSignal", AbortSignal_base::class_info },
        { "CryptoKey", CryptoKey_base::class_info },
        { "DOMParser", DOMParser_base::class_info },
        { "XMLSerializer", XMLSerializer_base::class_info },
        { "XMLDocument", XmlDocument_base::class_info },
        { "console", console_base::class_info },
        { "process", process_base::class_info },
        { "performance", performance_base::class_info },
        { "PerformanceObserver", PerformanceObserver_base::class_info },
        { "crypto", webcrypto_base::class_info }
    };

    static ClassData::ClassProperty s_property[] = {
        { "global", s_static_get_global, block_set, true },
        { "globalThis", s_static_get_globalThis, block_set, true }
    };

    static ClassData s_cd = {
        "global", true, s__new, NULL,
        ARRAYSIZE(s_method), s_method, ARRAYSIZE(s_object), s_object, ARRAYSIZE(s_property), s_property, 0, NULL, NULL, NULL,
        &object_base::class_info(),
        false
    };

    static ClassInfo s_ci(s_cd);
    return s_ci;
}

inline void global_base::s_static_get_global(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Object> vr;

    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = get_global(vr);

    METHOD_RETURN();
}

inline void global_base::s_static_get_globalThis(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Object> vr;

    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = get_globalThis(vr);

    METHOD_RETURN();
}

inline void global_base::s_static_run(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(exlib::string, 0);

    hr = run(v0);

    METHOD_VOID();
}

inline void global_base::s_static_require(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Value> vr;

    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(exlib::string, 0);

    hr = require(v0, vr);

    METHOD_RETURN();
}

inline void global_base::s_static_setTimeout(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<Timer_base> vr;

    METHOD_ENTER();

    METHOD_OVER(-1, 1);

    ARG(v8::Local<v8::Function>, 0);
    OPT_ARG(double, 1, 1);
    ARG_LIST(2);

    hr = setTimeout(v0, v1, v2, vr);

    METHOD_RETURN();
}

inline void global_base::s_static_clearTimeout(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(v8::Local<v8::Value>, 0);

    hr = clearTimeout(v0);

    METHOD_VOID();
}

inline void global_base::s_static_setInterval(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<Timer_base> vr;

    METHOD_ENTER();

    METHOD_OVER(-1, 2);

    ARG(v8::Local<v8::Function>, 0);
    ARG(double, 1);
    ARG_LIST(2);

    hr = setInterval(v0, v1, v2, vr);

    METHOD_RETURN();
}

inline void global_base::s_static_clearInterval(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(v8::Local<v8::Value>, 0);

    hr = clearInterval(v0);

    METHOD_VOID();
}

inline void global_base::s_static_setHrInterval(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<Timer_base> vr;

    METHOD_ENTER();

    METHOD_OVER(-1, 2);

    ARG(v8::Local<v8::Function>, 0);
    ARG(double, 1);
    ARG_LIST(2);

    hr = setHrInterval(v0, v1, v2, vr);

    METHOD_RETURN();
}

inline void global_base::s_static_clearHrInterval(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(v8::Local<v8::Value>, 0);

    hr = clearHrInterval(v0);

    METHOD_VOID();
}

inline void global_base::s_static_setImmediate(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<Timer_base> vr;

    METHOD_ENTER();

    METHOD_OVER(-1, 1);

    ARG(v8::Local<v8::Function>, 0);
    ARG_LIST(1);

    hr = setImmediate(v0, v1, vr);

    METHOD_RETURN();
}

inline void global_base::s_static_clearImmediate(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(v8::Local<v8::Value>, 0);

    hr = clearImmediate(v0);

    METHOD_VOID();
}

inline void global_base::s_static_btoa(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(exlib::string, 0);

    hr = btoa(v0, vr);

    METHOD_RETURN();
}

inline void global_base::s_static_atob(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(exlib::string, 0);

    hr = atob(v0, vr);

    METHOD_RETURN();
}

inline void global_base::s_static_structuredClone(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Value> vr;

    METHOD_ENTER();

    METHOD_OVER(2, 1);

    ARG(v8::Local<v8::Value>, 0);
    OPT_ARG(v8::Local<v8::Object>, 1, v8::Object::New(isolate->m_isolate));

    hr = structuredClone(v0, v1, vr);

    METHOD_RETURN();
}

inline void global_base::s_static_fetch(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<WebResponse_base> vr;

    ASYNC_METHOD_ENTER("global.fetch");

    METHOD_OVER(2, 1);

    ARG(exlib::string, 0);
    OPT_ARG(v8::Local<v8::Object>, 1, v8::Object::New(isolate->m_isolate));

    if (!cb.IsEmpty())
        hr = acb_fetch(v0, v1, cb, args);
    else
        hr = ac_fetch(v0, v1, vr);

    METHOD_OVER(2, 1);

    ARG(obj_ptr<HttpRequest_base>, 0);
    OPT_ARG(v8::Local<v8::Object>, 1, v8::Object::New(isolate->m_isolate));

    if (!cb.IsEmpty())
        hr = acb_fetch(v0.get(), v1, cb, args);
    else
        hr = ac_fetch(v0.get(), v1, vr);

    METHOD_RETURN();
}

inline void global_base::s_static_queueMicrotask(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(v8::Local<v8::Function>, 0);

    hr = queueMicrotask(v0);

    METHOD_VOID();
}
}
