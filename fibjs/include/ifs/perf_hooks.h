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

class PerformanceObserver_base;
class performance_base;

class perf_hooks_base : public object_base {
    DECLARE_CLASS(perf_hooks_base);

public:
    static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        CONSTRUCT_INIT();

        ThrowTypeError("not a constructor");
    }

    static result_t load(Isolate* isolate, v8::Local<v8::Value> v, obj_ptr<perf_hooks_base>& retVal)
    { return CALL_E_TYPEMISMATCH; }
};
}

#include "ifs/PerformanceObserver.h"
#include "ifs/performance.h"

namespace fibjs {
inline ClassInfo& perf_hooks_base::class_info()
{
    static ClassData::ClassObject s_object[] = {
        { "PerformanceObserver", PerformanceObserver_base::class_info },
        { "performance", performance_base::class_info }
    };

    static ClassData s_cd = {
        "perf_hooks", true, s__new, NULL,
        0, NULL, ARRAYSIZE(s_object), s_object, 0, NULL, 0, NULL, NULL, NULL,
        &object_base::class_info(),
        false
    };

    static ClassInfo s_ci(s_cd);
    return s_ci;
}
}
