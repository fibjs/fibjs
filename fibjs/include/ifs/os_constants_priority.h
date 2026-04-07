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

class os_constants_priority_base : public object_base {
    DECLARE_CLASS(os_constants_priority_base);

public:
    enum {
        C_PRIORITY_LOW = 19,
        C_PRIORITY_BELOW_NORMAL = 10,
        C_PRIORITY_NORMAL = 0,
        C_PRIORITY_ABOVE_NORMAL = -7,
        C_PRIORITY_HIGH = -14,
        C_PRIORITY_HIGHEST = -20
    };

public:
    static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        CONSTRUCT_INIT();

        ThrowTypeError("not a constructor");
    }

    static result_t load(v8::Local<v8::Value> v, obj_ptr<os_constants_priority_base>& retVal)
    { return CALL_E_TYPEMISMATCH; }
};
}

namespace fibjs {
inline ClassInfo& os_constants_priority_base::class_info()
{
    static ClassData::ClassConst s_const[] = {
        { "PRIORITY_LOW", ClassData::CONST_Integer, { .intValue = C_PRIORITY_LOW } },
        { "PRIORITY_BELOW_NORMAL", ClassData::CONST_Integer, { .intValue = C_PRIORITY_BELOW_NORMAL } },
        { "PRIORITY_NORMAL", ClassData::CONST_Integer, { .intValue = C_PRIORITY_NORMAL } },
        { "PRIORITY_ABOVE_NORMAL", ClassData::CONST_Integer, { .intValue = C_PRIORITY_ABOVE_NORMAL } },
        { "PRIORITY_HIGH", ClassData::CONST_Integer, { .intValue = C_PRIORITY_HIGH } },
        { "PRIORITY_HIGHEST", ClassData::CONST_Integer, { .intValue = C_PRIORITY_HIGHEST } }
    };

    static ClassData s_cd = {
        "os_constants_priority", true, s__new, NULL,
        0, NULL, 0, NULL, 0, NULL, ARRAYSIZE(s_const), s_const, NULL, NULL,
        &object_base::class_info(),
        false
    };

    static ClassInfo s_ci(s_cd);
    return s_ci;
}
}
