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

class module_base : public object_base {
    DECLARE_CLASS(module_base);

public:
    // module_base
    static result_t createRequire(exlib::string base, v8::Local<v8::Function>& retVal);
    static result_t get_builtinModules(v8::Local<v8::Array>& retVal);

public:
    static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        CONSTRUCT_INIT();

        ThrowTypeError("not a constructor");
    }

    static result_t load(v8::Local<v8::Value> v, obj_ptr<module_base>& retVal)
    { return CALL_E_TYPEMISMATCH; }

public:
    static void s_static_createRequire(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_get_builtinModules(const v8::FunctionCallbackInfo<v8::Value>& args);
};
}

namespace fibjs {
inline ClassInfo& module_base::class_info()
{
    static ClassData::ClassMethod s_method[] = {
        { "createRequire", s_static_createRequire, true, ClassData::ASYNC_SYNC }
    };

    static ClassData::ClassProperty s_property[] = {
        { "builtinModules", s_static_get_builtinModules, block_set, true }
    };

    static ClassData s_cd = {
        "module", true, s__new, NULL,
        ARRAYSIZE(s_method), s_method, 0, NULL, ARRAYSIZE(s_property), s_property, 0, NULL, NULL, NULL,
        &object_base::class_info(),
        false
    };

    static ClassInfo s_ci(s_cd);
    return s_ci;
}

inline void module_base::s_static_createRequire(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Function> vr;

    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(exlib::string, 0);

    hr = createRequire(v0, vr);

    METHOD_RETURN();
}

inline void module_base::s_static_get_builtinModules(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Array> vr;

    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = get_builtinModules(vr);

    METHOD_RETURN();
}
}
