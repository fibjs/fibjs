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

class WASIHost_base;

class wasi_base : public object_base {
    DECLARE_CLASS(wasi_base);

public:
    static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        CONSTRUCT_INIT();

        ThrowTypeError("not a constructor");
    }

    static result_t load(v8::Local<v8::Value> v, obj_ptr<wasi_base>& retVal)
    { return result_t::type_mismatch(); }
};
}

#include "ifs/WASIHost.h"

namespace fibjs {
inline ClassInfo& wasi_base::class_info()
{
    static ClassData::ClassObject s_object[] = {
        { "WASI", WASIHost_base::class_info }
    };

    static ClassData s_cd = {
        "wasi", true, s__new, NULL,
        0, NULL, ARRAYSIZE(s_object), s_object, 0, NULL, 0, NULL, NULL, NULL,
        &object_base::class_info(),
        false
    };

    static ClassInfo s_ci(s_cd);
    return s_ci;
}
}
