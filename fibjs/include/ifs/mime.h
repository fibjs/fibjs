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

class mime_base : public object_base {
    DECLARE_CLASS(mime_base);

public:
    // mime_base
    static result_t getType(exlib::string fname, exlib::string& retVal);
    static result_t addType(exlib::string ext, exlib::string type);

public:
    static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        CONSTRUCT_INIT();

        isolate->m_isolate->ThrowException(
            isolate->NewString("not a constructor"));
    }

public:
    static void s_static_getType(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_addType(const v8::FunctionCallbackInfo<v8::Value>& args);
};
}

namespace fibjs {
inline ClassInfo& mime_base::class_info()
{
    static ClassData::ClassMethod s_method[] = {
        { "getType", s_static_getType, true, ClassData::ASYNC_SYNC },
        { "addType", s_static_addType, true, ClassData::ASYNC_SYNC }
    };

    static ClassData s_cd = {
        "mime", true, s__new, NULL,
        ARRAYSIZE(s_method), s_method, 0, NULL, 0, NULL, 0, NULL, NULL, NULL,
        &object_base::class_info(),
        false
    };

    static ClassInfo s_ci(s_cd);
    return s_ci;
}

inline void mime_base::s_static_getType(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(exlib::string, 0);

    hr = getType(v0, vr);

    METHOD_RETURN();
}

inline void mime_base::s_static_addType(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_ENTER();

    METHOD_OVER(2, 2);

    ARG(exlib::string, 0);
    ARG(exlib::string, 1);

    hr = addType(v0, v1);

    METHOD_VOID();
}
}
