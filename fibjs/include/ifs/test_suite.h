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

class test_suite_base : public object_base {
    DECLARE_CLASS(test_suite_base);

public:
    // test_suite_base
    static result_t _function(exlib::string name, v8::Local<v8::Function> block);
    static result_t skip(exlib::string name, v8::Local<v8::Function> block);
    static result_t only(exlib::string name, v8::Local<v8::Function> block);

public:
    static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        s__function(args);
    }

public:
    static void s__function(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_skip(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_only(const v8::FunctionCallbackInfo<v8::Value>& args);
};
}

namespace fibjs {
inline ClassInfo& test_suite_base::class_info()
{
    static ClassData::ClassMethod s_method[] = {
        { "skip", s_static_skip, true, ClassData::ASYNC_SYNC },
        { "only", s_static_only, true, ClassData::ASYNC_SYNC }
    };

    static ClassData s_cd = {
        "test_suite", true, s__new, s__function,
        ARRAYSIZE(s_method), s_method, 0, NULL, 0, NULL, 0, NULL, NULL, NULL,
        &object_base::class_info(),
        false
    };

    static ClassInfo s_ci(s_cd);
    return s_ci;
}

inline void test_suite_base::s__function(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_ENTER();

    METHOD_OVER(2, 2);

    ARG(exlib::string, 0);
    ARG(v8::Local<v8::Function>, 1);

    hr = _function(v0, v1);

    METHOD_VOID();
}

inline void test_suite_base::s_static_skip(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_ENTER();

    METHOD_OVER(2, 2);

    ARG(exlib::string, 0);
    ARG(v8::Local<v8::Function>, 1);

    hr = skip(v0, v1);

    METHOD_VOID();
}

inline void test_suite_base::s_static_only(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_ENTER();

    METHOD_OVER(2, 2);

    ARG(exlib::string, 0);
    ARG(v8::Local<v8::Function>, 1);

    hr = only(v0, v1);

    METHOD_VOID();
}
}
