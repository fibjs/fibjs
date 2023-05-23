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

class performance_base;

class perf_hooks_base : public object_base {
    DECLARE_CLASS(perf_hooks_base);

public:
    static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        CONSTRUCT_INIT();

        isolate->m_isolate->ThrowException(
            isolate->NewString("not a constructor"));
    }
};
}

#include "ifs/performance.h"

namespace fibjs {
inline ClassInfo& perf_hooks_base::class_info()
{
    static ClassData::ClassObject s_object[] = {
        { "performance", performance_base::class_info }
    };

    static ClassData s_cd = {
        "perf_hooks", true, s__new, NULL,
        0, NULL, ARRAYSIZE(s_object), s_object, 0, NULL, 0, NULL, NULL, NULL,
        &object_base::class_info()
    };

    static ClassInfo s_ci(s_cd);
    return s_ci;
}
}
