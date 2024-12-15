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

class PerformanceEntry_base : public object_base {
    DECLARE_CLASS(PerformanceEntry_base);

public:
    // PerformanceEntry_base
    virtual result_t get_name(exlib::string& retVal) = 0;
    virtual result_t get_entryType(exlib::string& retVal) = 0;
    virtual result_t get_startTime(double& retVal) = 0;
    virtual result_t get_duration(double& retVal) = 0;

public:
    static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        CONSTRUCT_INIT();

        isolate->m_isolate->ThrowException(
            isolate->NewString("not a constructor"));
    }

    static result_t load(Isolate* isolate, v8::Local<v8::Value> v, obj_ptr<PerformanceEntry_base>& retVal)
    { return CALL_E_TYPEMISMATCH; }

public:
    static void s_get_name(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_entryType(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_startTime(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_duration(const v8::FunctionCallbackInfo<v8::Value>& args);
};
}

namespace fibjs {
inline ClassInfo& PerformanceEntry_base::class_info()
{
    static ClassData::ClassProperty s_property[] = {
        { "name", s_get_name, block_set, false },
        { "entryType", s_get_entryType, block_set, false },
        { "startTime", s_get_startTime, block_set, false },
        { "duration", s_get_duration, block_set, false }
    };

    static ClassData s_cd = {
        "PerformanceEntry", false, s__new, NULL,
        0, NULL, 0, NULL, ARRAYSIZE(s_property), s_property, 0, NULL, NULL, NULL,
        &object_base::class_info(),
        false
    };

    static ClassInfo s_ci(s_cd);
    return s_ci;
}

inline void PerformanceEntry_base::s_get_name(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_INSTANCE(PerformanceEntry_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->get_name(vr);

    METHOD_RETURN();
}

inline void PerformanceEntry_base::s_get_entryType(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_INSTANCE(PerformanceEntry_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->get_entryType(vr);

    METHOD_RETURN();
}

inline void PerformanceEntry_base::s_get_startTime(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    double vr;

    METHOD_INSTANCE(PerformanceEntry_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->get_startTime(vr);

    METHOD_RETURN();
}

inline void PerformanceEntry_base::s_get_duration(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    double vr;

    METHOD_INSTANCE(PerformanceEntry_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->get_duration(vr);

    METHOD_RETURN();
}
}
