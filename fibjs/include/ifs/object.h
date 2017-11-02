/***************************************************************************
 *                                                                         *
 *   This file was automatically generated using idlc.js                   *
 *   PLEASE DO NOT EDIT!!!!                                                *
 *                                                                         *
 ***************************************************************************/

#ifndef _object_base_H_
#define _object_base_H_

/**
 @author Leo Hoo <lion@9465.net>
 */

#include "../object.h"

namespace fibjs {

class object_base {
    DECLARE_CLASS(object_base);

public:
    // object_base
    virtual result_t toString(exlib::string& retVal) = 0;
    virtual result_t toJSON(exlib::string key, v8::Local<v8::Value>& retVal) = 0;

public:
    static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        CONSTRUCT_INIT();

        Isolate* isolate = Isolate::current();

        isolate->m_isolate->ThrowException(
            isolate->NewString("not a constructor"));
    }

public:
    static void s_toString(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_toJSON(const v8::FunctionCallbackInfo<v8::Value>& args);
};
}

namespace fibjs {
inline ClassInfo& object_base::class_info()
{
    static ClassData::ClassMethod s_method[] = {
        { "toString", s_toString, false },
        { "toJSON", s_toJSON, false }
    };

    static ClassData s_cd = {
        "object", false, s__new, NULL,
        ARRAYSIZE(s_method), s_method, 0, NULL, 0, NULL, 0, NULL, NULL, NULL,
        NULL
    };

    static ClassInfo s_ci(s_cd);
    return s_ci;
}

inline void object_base::s_toString(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_NAME("object.toString");
    METHOD_INSTANCE(object_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->toString(vr);

    METHOD_RETURN();
}

inline void object_base::s_toJSON(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Value> vr;

    METHOD_NAME("object.toJSON");
    METHOD_INSTANCE(object_base);
    METHOD_ENTER();

    METHOD_OVER(1, 0);

    OPT_ARG(exlib::string, 0, "");

    hr = pInst->toJSON(v0, vr);

    METHOD_RETURN();
}
}

#endif
