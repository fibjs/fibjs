/***************************************************************************
 *                                                                         *
 *   This file was automatically generated using idlc.js                   *
 *   PLEASE DO NOT EDIT!!!!                                                *
 *                                                                         *
 ***************************************************************************/

#ifndef _hyperscan_base_H_
#define _hyperscan_base_H_

/**
 @author Leo Hoo <lion@9465.net>
 */

#include "../object.h"

namespace fibjs {

class HsRegExp_base;

class hyperscan_base : public object_base {
    DECLARE_CLASS(hyperscan_base);

public:
    // hyperscan_base
    static result_t compile(exlib::string pattern, exlib::string flag, obj_ptr<HsRegExp_base>& retVal);
    static result_t compile(v8::Local<v8::Array> patterns, v8::Local<v8::Array> flags, obj_ptr<HsRegExp_base>& retVal);

public:
    static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        CONSTRUCT_INIT();

        Isolate* isolate = Isolate::current();

        isolate->m_isolate->ThrowException(
            isolate->NewString("not a constructor"));
    }

public:
    static void s_compile(const v8::FunctionCallbackInfo<v8::Value>& args);
};
}

#include "HsRegExp.h"

namespace fibjs {
inline ClassInfo& hyperscan_base::class_info()
{
    static ClassData::ClassMethod s_method[] = {
        { "compile", s_compile, true }
    };

    static ClassData s_cd = {
        "hyperscan", true, s__new, NULL,
        ARRAYSIZE(s_method), s_method, 0, NULL, 0, NULL, 0, NULL, NULL, NULL,
        &object_base::class_info()
    };

    static ClassInfo s_ci(s_cd);
    return s_ci;
}

inline void hyperscan_base::s_compile(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<HsRegExp_base> vr;

    METHOD_NAME("hyperscan.compile");
    METHOD_ENTER();

    METHOD_OVER(2, 1);

    ARG(exlib::string, 0);
    OPT_ARG(exlib::string, 1, "");

    hr = compile(v0, v1, vr);

    METHOD_OVER(2, 1);

    ARG(v8::Local<v8::Array>, 0);
    OPT_ARG(v8::Local<v8::Array>, 1, v8::Array::New(isolate));

    hr = compile(v0, v1, vr);

    METHOD_RETURN();
}
}

#endif
