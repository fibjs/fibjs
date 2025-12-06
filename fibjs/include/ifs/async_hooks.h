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

class AsyncLocalStorage_base;

class async_hooks_base : public object_base {
    DECLARE_CLASS(async_hooks_base);

public:
    static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        CONSTRUCT_INIT();

        ThrowTypeError("not a constructor");
    }

    static result_t load(v8::Local<v8::Value> v, obj_ptr<async_hooks_base>& retVal)
    { return CALL_E_TYPEMISMATCH; }
};
}

#include "ifs/AsyncLocalStorage.h"

namespace fibjs {
inline ClassInfo& async_hooks_base::class_info()
{
    static ClassData::ClassObject s_object[] = {
        { "AsyncLocalStorage", AsyncLocalStorage_base::class_info }
    };

    static ClassData s_cd = {
        "async_hooks", true, s__new, NULL,
        0, NULL, ARRAYSIZE(s_object), s_object, 0, NULL, 0, NULL, NULL, NULL,
        &object_base::class_info(),
        false
    };

    static ClassInfo s_ci(s_cd);
    return s_ci;
}
}
