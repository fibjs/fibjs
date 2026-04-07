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

class os_constants_errno_base;
class os_constants_signals_base;
class os_constants_priority_base;
class os_constants_dlopen_base;

class os_constants_base : public object_base {
    DECLARE_CLASS(os_constants_base);

public:
    enum {
        C_UV_UDP_REUSEADDR = 4
    };

public:
    static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        CONSTRUCT_INIT();

        ThrowTypeError("not a constructor");
    }

    static result_t load(v8::Local<v8::Value> v, obj_ptr<os_constants_base>& retVal)
    { return CALL_E_TYPEMISMATCH; }
};
}

#include "ifs/os_constants_errno.h"
#include "ifs/os_constants_signals.h"
#include "ifs/os_constants_priority.h"
#include "ifs/os_constants_dlopen.h"

namespace fibjs {
inline ClassInfo& os_constants_base::class_info()
{
    static ClassData::ClassObject s_object[] = {
        { "errno", os_constants_errno_base::class_info },
        { "signals", os_constants_signals_base::class_info },
        { "priority", os_constants_priority_base::class_info },
        { "dlopen", os_constants_dlopen_base::class_info }
    };

    static ClassData::ClassConst s_const[] = {
        { "UV_UDP_REUSEADDR", ClassData::CONST_Integer, { .intValue = C_UV_UDP_REUSEADDR } }
    };

    static ClassData s_cd = {
        "os_constants", true, s__new, NULL,
        0, NULL, ARRAYSIZE(s_object), s_object, 0, NULL, ARRAYSIZE(s_const), s_const, NULL, NULL,
        &object_base::class_info(),
        false
    };

    static ClassInfo s_ci(s_cd);
    return s_ci;
}
}
