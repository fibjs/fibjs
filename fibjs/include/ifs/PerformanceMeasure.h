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
#include "ifs/PerformanceEntry.h"

namespace fibjs {

class PerformanceEntry_base;

class PerformanceMeasure_base : public PerformanceEntry_base {
    DECLARE_CLASS(PerformanceMeasure_base);

public:
    // PerformanceMeasure_base
    virtual result_t get_detail(v8::Local<v8::Value>& retVal) = 0;

public:
    static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        CONSTRUCT_INIT();

        isolate->m_isolate->ThrowException(
            isolate->NewString("not a constructor"));
    }

    static result_t load(Isolate* isolate, v8::Local<v8::Value> v, obj_ptr<PerformanceMeasure_base>& retVal)
    { return CALL_E_TYPEMISMATCH; }

public:
    static void s_get_detail(const v8::FunctionCallbackInfo<v8::Value>& args);
};
}

namespace fibjs {
inline ClassInfo& PerformanceMeasure_base::class_info()
{
    static ClassData::ClassProperty s_property[] = {
        { "detail", s_get_detail, block_set, false }
    };

    static ClassData s_cd = {
        "PerformanceMeasure", false, s__new, NULL,
        0, NULL, 0, NULL, ARRAYSIZE(s_property), s_property, 0, NULL, NULL, NULL,
        &PerformanceEntry_base::class_info(),
        false
    };

    static ClassInfo s_ci(s_cd);
    return s_ci;
}

inline void PerformanceMeasure_base::s_get_detail(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Value> vr;

    METHOD_INSTANCE(PerformanceMeasure_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->get_detail(vr);

    METHOD_RETURN();
}
}
