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

class Menu_base;

class Tray_base : public object_base {
    DECLARE_CLASS(Tray_base);

public:
    // Tray_base
    virtual result_t getMenu(obj_ptr<Menu_base>& retVal) = 0;
    virtual result_t close() = 0;

public:
    static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        CONSTRUCT_INIT();

        ThrowTypeError("not a constructor");
    }

    static result_t load(v8::Local<v8::Value> v, obj_ptr<Tray_base>& retVal)
    { return result_t::type_mismatch(); }

public:
    static void s_getMenu(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_close(const v8::FunctionCallbackInfo<v8::Value>& args);
};
}

#include "ifs/Menu.h"

namespace fibjs {
inline ClassInfo& Tray_base::class_info()
{
    static ClassData::ClassMethod s_method[] = {
        { "getMenu", s_getMenu, false, ClassData::ASYNC_SYNC },
        { "close", s_close, false, ClassData::ASYNC_SYNC }
    };

    static ClassData s_cd = {
        "Tray", false, s__new, NULL,
        ARRAYSIZE(s_method), s_method, 0, NULL, 0, NULL, 0, NULL, NULL, NULL,
        &object_base::class_info(),
        false
    };

    static ClassInfo s_ci(s_cd);
    return s_ci;
}

inline void Tray_base::s_getMenu(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<Menu_base> vr;

    METHOD_INSTANCE(Tray_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->getMenu(vr);

    METHOD_RETURN();
}

inline void Tray_base::s_close(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_INSTANCE(Tray_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->close();

    METHOD_VOID();
}
}
