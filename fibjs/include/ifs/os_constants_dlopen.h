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

class os_constants_dlopen_base : public object_base {
    DECLARE_CLASS(os_constants_dlopen_base);

public:
    enum {
        C_RTLD_LAZY = 1,
        C_RTLD_NOW = 2,
        C_RTLD_GLOBAL = 256,
        C_RTLD_LOCAL = 0,
        C_RTLD_DEEPBIND = 8
    };

public:
    static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        CONSTRUCT_INIT();

        ThrowTypeError("not a constructor");
    }

    static result_t load(v8::Local<v8::Value> v, obj_ptr<os_constants_dlopen_base>& retVal)
    { return CALL_E_TYPEMISMATCH; }
};
}

namespace fibjs {
inline ClassInfo& os_constants_dlopen_base::class_info()
{
    static ClassData::ClassConst s_const[] = {
        { "RTLD_LAZY", ClassData::CONST_Integer, { .intValue = C_RTLD_LAZY } },
        { "RTLD_NOW", ClassData::CONST_Integer, { .intValue = C_RTLD_NOW } },
        { "RTLD_GLOBAL", ClassData::CONST_Integer, { .intValue = C_RTLD_GLOBAL } },
        { "RTLD_LOCAL", ClassData::CONST_Integer, { .intValue = C_RTLD_LOCAL } },
        { "RTLD_DEEPBIND", ClassData::CONST_Integer, { .intValue = C_RTLD_DEEPBIND } }
    };

    static ClassData s_cd = {
        "os_constants_dlopen", true, s__new, NULL,
        0, NULL, 0, NULL, 0, NULL, ARRAYSIZE(s_const), s_const, NULL, NULL,
        &object_base::class_info(),
        false
    };

    static ClassInfo s_ci(s_cd);
    return s_ci;
}
}
