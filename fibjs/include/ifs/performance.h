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

class performance_base : public object_base {
    DECLARE_CLASS(performance_base);

public:
    // performance_base
    static result_t clearMarks(exlib::string name);
    static result_t clearMeasures(exlib::string name);
    static result_t mark(exlib::string name, v8::Local<v8::Object> options);
    static result_t measure(exlib::string name, exlib::string startMark, exlib::string endMark);
    static result_t measure(exlib::string name, v8::Local<v8::Object> options);
    static result_t getEntries(obj_ptr<NArray>& retVal);
    static result_t getEntriesByType(exlib::string type, obj_ptr<NArray>& retVal);
    static result_t getEntriesByName(exlib::string name, exlib::string type, obj_ptr<NArray>& retVal);
    static result_t markResourceTiming(v8::Local<v8::Value> timingInfo, exlib::string requestedUrl, exlib::string initiatorType, v8::Local<v8::Value> global, exlib::string cacheState, v8::Local<v8::Value> bodyInfo, int32_t responseStatus);
    static result_t now(double& retVal);

public:
    static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        CONSTRUCT_INIT();

        ThrowTypeError("not a constructor");
    }

    static result_t load(v8::Local<v8::Value> v, obj_ptr<performance_base>& retVal)
    { return CALL_E_TYPEMISMATCH; }

public:
    static void s_static_clearMarks(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_clearMeasures(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_mark(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_measure(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_getEntries(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_getEntriesByType(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_getEntriesByName(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_markResourceTiming(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_now(const v8::FunctionCallbackInfo<v8::Value>& args);
};
}

namespace fibjs {
inline ClassInfo& performance_base::class_info()
{
    static ClassData::ClassMethod s_method[] = {
        { "clearMarks", s_static_clearMarks, true, ClassData::ASYNC_SYNC },
        { "clearMeasures", s_static_clearMeasures, true, ClassData::ASYNC_SYNC },
        { "mark", s_static_mark, true, ClassData::ASYNC_SYNC },
        { "measure", s_static_measure, true, ClassData::ASYNC_SYNC },
        { "getEntries", s_static_getEntries, true, ClassData::ASYNC_SYNC },
        { "getEntriesByType", s_static_getEntriesByType, true, ClassData::ASYNC_SYNC },
        { "getEntriesByName", s_static_getEntriesByName, true, ClassData::ASYNC_SYNC },
        { "markResourceTiming", s_static_markResourceTiming, true, ClassData::ASYNC_SYNC },
        { "now", s_static_now, true, ClassData::ASYNC_SYNC }
    };

    static ClassData s_cd = {
        "performance", true, s__new, NULL,
        ARRAYSIZE(s_method), s_method, 0, NULL, 0, NULL, 0, NULL, NULL, NULL,
        &object_base::class_info(),
        false
    };

    static ClassInfo s_ci(s_cd);
    return s_ci;
}

inline void performance_base::s_static_clearMarks(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_ENTER();

    METHOD_OVER(1, 0);

    OPT_ARG(exlib::string, 0, "");

    hr = clearMarks(v0);

    METHOD_VOID();
}

inline void performance_base::s_static_clearMeasures(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_ENTER();

    METHOD_OVER(1, 0);

    OPT_ARG(exlib::string, 0, "");

    hr = clearMeasures(v0);

    METHOD_VOID();
}

inline void performance_base::s_static_mark(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_ENTER();

    METHOD_OVER(2, 1);

    ARG(exlib::string, 0);
    OPT_ARG(v8::Local<v8::Object>, 1, v8::Object::New(isolate->m_isolate));

    hr = mark(v0, v1);

    METHOD_VOID();
}

inline void performance_base::s_static_measure(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_ENTER();

    METHOD_OVER(3, 1);

    ARG(exlib::string, 0);
    OPT_ARG(exlib::string, 1, "");
    OPT_ARG(exlib::string, 2, "");

    hr = measure(v0, v1, v2);

    METHOD_OVER(2, 1);

    ARG(exlib::string, 0);
    OPT_ARG(v8::Local<v8::Object>, 1, v8::Object::New(isolate->m_isolate));

    hr = measure(v0, v1);

    METHOD_VOID();
}

inline void performance_base::s_static_getEntries(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<NArray> vr;

    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = getEntries(vr);

    METHOD_RETURN();
}

inline void performance_base::s_static_getEntriesByType(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<NArray> vr;

    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(exlib::string, 0);

    hr = getEntriesByType(v0, vr);

    METHOD_RETURN();
}

inline void performance_base::s_static_getEntriesByName(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<NArray> vr;

    METHOD_ENTER();

    METHOD_OVER(2, 1);

    ARG(exlib::string, 0);
    OPT_ARG(exlib::string, 1, "");

    hr = getEntriesByName(v0, v1, vr);

    METHOD_RETURN();
}

inline void performance_base::s_static_markResourceTiming(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_ENTER();

    METHOD_OVER(7, 7);

    ARG(v8::Local<v8::Value>, 0);
    ARG(exlib::string, 1);
    ARG(exlib::string, 2);
    ARG(v8::Local<v8::Value>, 3);
    ARG(exlib::string, 4);
    ARG(v8::Local<v8::Value>, 5);
    ARG(int32_t, 6);

    hr = markResourceTiming(v0, v1, v2, v3, v4, v5, v6);

    METHOD_VOID();
}

inline void performance_base::s_static_now(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    double vr;

    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = now(vr);

    METHOD_RETURN();
}
}
