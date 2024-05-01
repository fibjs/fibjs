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

class console_base;
class assert_base;

class test_base : public object_base {
    DECLARE_CLASS(test_base);

public:
    // test_base
    static result_t describe(exlib::string name, v8::Local<v8::Function> block);
    static result_t xdescribe(exlib::string name, v8::Local<v8::Function> block);
    static result_t odescribe(exlib::string name, v8::Local<v8::Function> block);
    static result_t it(exlib::string name, v8::Local<v8::Function> block);
    static result_t xit(exlib::string name, v8::Local<v8::Function> block);
    static result_t oit(exlib::string name, v8::Local<v8::Function> block);
    static result_t todo(exlib::string name, v8::Local<v8::Function> block);
    static result_t before(v8::Local<v8::Function> func);
    static result_t after(v8::Local<v8::Function> func);
    static result_t beforeEach(v8::Local<v8::Function> func);
    static result_t afterEach(v8::Local<v8::Function> func);
    static result_t mustCall(v8::Local<v8::Function> func, v8::Local<v8::Function>& retVal);
    static result_t mustNotCall(v8::Local<v8::Function> func, v8::Local<v8::Function>& retVal);
    static result_t mustNotCall(v8::Local<v8::Function>& retVal);
    static result_t run(int32_t mode, v8::Local<v8::Object>& retVal);
    static result_t setup();
    static result_t get_slow(int32_t& retVal);
    static result_t set_slow(int32_t newVal);

public:
    static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        CONSTRUCT_INIT();

        isolate->m_isolate->ThrowException(
            isolate->NewString("not a constructor"));
    }

public:
    static void s_static_describe(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_xdescribe(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_odescribe(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_it(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_xit(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_oit(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_todo(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_before(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_after(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_beforeEach(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_afterEach(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_mustCall(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_mustNotCall(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_run(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_setup(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_get_slow(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_static_set_slow(v8::Local<v8::Name> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args);
};
}

#include "ifs/console.h"
#include "ifs/assert.h"

namespace fibjs {
inline ClassInfo& test_base::class_info()
{
    static ClassData::ClassMethod s_method[] = {
        { "describe", s_static_describe, true, ClassData::ASYNC_SYNC },
        { "xdescribe", s_static_xdescribe, true, ClassData::ASYNC_SYNC },
        { "odescribe", s_static_odescribe, true, ClassData::ASYNC_SYNC },
        { "it", s_static_it, true, ClassData::ASYNC_SYNC },
        { "xit", s_static_xit, true, ClassData::ASYNC_SYNC },
        { "oit", s_static_oit, true, ClassData::ASYNC_SYNC },
        { "todo", s_static_todo, true, ClassData::ASYNC_SYNC },
        { "before", s_static_before, true, ClassData::ASYNC_SYNC },
        { "after", s_static_after, true, ClassData::ASYNC_SYNC },
        { "beforeEach", s_static_beforeEach, true, ClassData::ASYNC_SYNC },
        { "afterEach", s_static_afterEach, true, ClassData::ASYNC_SYNC },
        { "mustCall", s_static_mustCall, true, ClassData::ASYNC_SYNC },
        { "mustNotCall", s_static_mustNotCall, true, ClassData::ASYNC_SYNC },
        { "run", s_static_run, true, ClassData::ASYNC_SYNC },
        { "setup", s_static_setup, true, ClassData::ASYNC_SYNC }
    };

    static ClassData::ClassObject s_object[] = {
        { "assert", assert_base::class_info }
    };

    static ClassData::ClassProperty s_property[] = {
        { "slow", s_static_get_slow, s_static_set_slow, true }
    };

    static ClassData s_cd = {
        "test", true, s__new, NULL,
        ARRAYSIZE(s_method), s_method, ARRAYSIZE(s_object), s_object, ARRAYSIZE(s_property), s_property, 0, NULL, NULL, NULL,
        &object_base::class_info(),
        false
    };

    static ClassInfo s_ci(s_cd);
    return s_ci;
}

inline void test_base::s_static_describe(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_ENTER();

    METHOD_OVER(2, 2);

    ARG(exlib::string, 0);
    ARG(v8::Local<v8::Function>, 1);

    hr = describe(v0, v1);

    METHOD_VOID();
}

inline void test_base::s_static_xdescribe(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_ENTER();

    METHOD_OVER(2, 2);

    ARG(exlib::string, 0);
    ARG(v8::Local<v8::Function>, 1);

    hr = xdescribe(v0, v1);

    METHOD_VOID();
}

inline void test_base::s_static_odescribe(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_ENTER();

    METHOD_OVER(2, 2);

    ARG(exlib::string, 0);
    ARG(v8::Local<v8::Function>, 1);

    hr = odescribe(v0, v1);

    METHOD_VOID();
}

inline void test_base::s_static_it(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_ENTER();

    METHOD_OVER(2, 2);

    ARG(exlib::string, 0);
    ARG(v8::Local<v8::Function>, 1);

    hr = it(v0, v1);

    METHOD_VOID();
}

inline void test_base::s_static_xit(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_ENTER();

    METHOD_OVER(2, 2);

    ARG(exlib::string, 0);
    ARG(v8::Local<v8::Function>, 1);

    hr = xit(v0, v1);

    METHOD_VOID();
}

inline void test_base::s_static_oit(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_ENTER();

    METHOD_OVER(2, 2);

    ARG(exlib::string, 0);
    ARG(v8::Local<v8::Function>, 1);

    hr = oit(v0, v1);

    METHOD_VOID();
}

inline void test_base::s_static_todo(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_ENTER();

    METHOD_OVER(2, 2);

    ARG(exlib::string, 0);
    ARG(v8::Local<v8::Function>, 1);

    hr = todo(v0, v1);

    METHOD_VOID();
}

inline void test_base::s_static_before(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(v8::Local<v8::Function>, 0);

    hr = before(v0);

    METHOD_VOID();
}

inline void test_base::s_static_after(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(v8::Local<v8::Function>, 0);

    hr = after(v0);

    METHOD_VOID();
}

inline void test_base::s_static_beforeEach(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(v8::Local<v8::Function>, 0);

    hr = beforeEach(v0);

    METHOD_VOID();
}

inline void test_base::s_static_afterEach(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(v8::Local<v8::Function>, 0);

    hr = afterEach(v0);

    METHOD_VOID();
}

inline void test_base::s_static_mustCall(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Function> vr;

    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(v8::Local<v8::Function>, 0);

    hr = mustCall(v0, vr);

    METHOD_RETURN();
}

inline void test_base::s_static_mustNotCall(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Function> vr;

    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(v8::Local<v8::Function>, 0);

    hr = mustNotCall(v0, vr);

    METHOD_OVER(0, 0);

    hr = mustNotCall(vr);

    METHOD_RETURN();
}

inline void test_base::s_static_run(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Object> vr;

    METHOD_ENTER();

    METHOD_OVER(1, 0);

    OPT_ARG(int32_t, 0, console_base::C_ERROR);

    hr = run(v0, vr);

    METHOD_RETURN();
}

inline void test_base::s_static_setup(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = setup();

    METHOD_VOID();
}

inline void test_base::s_static_get_slow(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    int32_t vr;

    PROPERTY_ENTER();

    hr = get_slow(vr);

    METHOD_RETURN();
}

inline void test_base::s_static_set_slow(v8::Local<v8::Name> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args)
{
    PROPERTY_ENTER();
    PROPERTY_VAL(int32_t);

    hr = set_slow(v0);

    PROPERTY_SET_LEAVE();
}
}
