/***************************************************************************
 *                                                                         *
 *   This file was automatically generated using idlc.js                   *
 *   PLEASE DO NOT EDIT!!!!                                                *
 *                                                                         *
 ***************************************************************************/

#ifndef _test_base_H_
#define _test_base_H_

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
    static result_t before(v8::Local<v8::Function> func);
    static result_t after(v8::Local<v8::Function> func);
    static result_t beforeEach(v8::Local<v8::Function> func);
    static result_t afterEach(v8::Local<v8::Function> func);
    static result_t run(int32_t loglevel, int32_t& retVal);
    static result_t setup();
    static result_t get_slow(int32_t& retVal);
    static result_t set_slow(int32_t newVal);

public:
    static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        CONSTRUCT_INIT();

        Isolate* isolate = Isolate::current();

        isolate->m_isolate->ThrowException(
            isolate->NewString("not a constructor"));
    }

public:
    static void s_describe(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_xdescribe(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_odescribe(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_it(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_xit(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_oit(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_before(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_after(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_beforeEach(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_afterEach(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_run(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_setup(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_slow(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_set_slow(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args);
};
}

#include "console.h"
#include "assert.h"

namespace fibjs {
inline ClassInfo& test_base::class_info()
{
    static ClassData::ClassMethod s_method[] = {
        { "describe", s_describe, true },
        { "xdescribe", s_xdescribe, true },
        { "odescribe", s_odescribe, true },
        { "it", s_it, true },
        { "xit", s_xit, true },
        { "oit", s_oit, true },
        { "before", s_before, true },
        { "after", s_after, true },
        { "beforeEach", s_beforeEach, true },
        { "afterEach", s_afterEach, true },
        { "run", s_run, true },
        { "setup", s_setup, true }
    };

    static ClassData::ClassObject s_object[] = {
        { "assert", assert_base::class_info }
    };

    static ClassData::ClassProperty s_property[] = {
        { "slow", s_get_slow, s_set_slow, true }
    };

    static ClassData s_cd = {
        "test", true, s__new, NULL,
        ARRAYSIZE(s_method), s_method, ARRAYSIZE(s_object), s_object, ARRAYSIZE(s_property), s_property, 0, NULL, NULL, NULL,
        &object_base::class_info()
    };

    static ClassInfo s_ci(s_cd);
    return s_ci;
}

inline void test_base::s_describe(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_NAME("test.describe");
    METHOD_ENTER();

    METHOD_OVER(2, 2);

    ARG(exlib::string, 0);
    ARG(v8::Local<v8::Function>, 1);

    hr = describe(v0, v1);

    METHOD_VOID();
}

inline void test_base::s_xdescribe(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_NAME("test.xdescribe");
    METHOD_ENTER();

    METHOD_OVER(2, 2);

    ARG(exlib::string, 0);
    ARG(v8::Local<v8::Function>, 1);

    hr = xdescribe(v0, v1);

    METHOD_VOID();
}

inline void test_base::s_odescribe(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_NAME("test.odescribe");
    METHOD_ENTER();

    METHOD_OVER(2, 2);

    ARG(exlib::string, 0);
    ARG(v8::Local<v8::Function>, 1);

    hr = odescribe(v0, v1);

    METHOD_VOID();
}

inline void test_base::s_it(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_NAME("test.it");
    METHOD_ENTER();

    METHOD_OVER(2, 2);

    ARG(exlib::string, 0);
    ARG(v8::Local<v8::Function>, 1);

    hr = it(v0, v1);

    METHOD_VOID();
}

inline void test_base::s_xit(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_NAME("test.xit");
    METHOD_ENTER();

    METHOD_OVER(2, 2);

    ARG(exlib::string, 0);
    ARG(v8::Local<v8::Function>, 1);

    hr = xit(v0, v1);

    METHOD_VOID();
}

inline void test_base::s_oit(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_NAME("test.oit");
    METHOD_ENTER();

    METHOD_OVER(2, 2);

    ARG(exlib::string, 0);
    ARG(v8::Local<v8::Function>, 1);

    hr = oit(v0, v1);

    METHOD_VOID();
}

inline void test_base::s_before(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_NAME("test.before");
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(v8::Local<v8::Function>, 0);

    hr = before(v0);

    METHOD_VOID();
}

inline void test_base::s_after(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_NAME("test.after");
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(v8::Local<v8::Function>, 0);

    hr = after(v0);

    METHOD_VOID();
}

inline void test_base::s_beforeEach(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_NAME("test.beforeEach");
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(v8::Local<v8::Function>, 0);

    hr = beforeEach(v0);

    METHOD_VOID();
}

inline void test_base::s_afterEach(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_NAME("test.afterEach");
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(v8::Local<v8::Function>, 0);

    hr = afterEach(v0);

    METHOD_VOID();
}

inline void test_base::s_run(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    int32_t vr;

    METHOD_NAME("test.run");
    METHOD_ENTER();

    METHOD_OVER(1, 0);

    OPT_ARG(int32_t, 0, console_base::_ERROR);

    hr = run(v0, vr);

    METHOD_RETURN();
}

inline void test_base::s_setup(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_NAME("test.setup");
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = setup();

    METHOD_VOID();
}

inline void test_base::s_get_slow(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    int32_t vr;

    METHOD_NAME("test.slow");
    PROPERTY_ENTER();

    hr = get_slow(vr);

    METHOD_RETURN();
}

inline void test_base::s_set_slow(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args)
{
    METHOD_NAME("test.slow");
    PROPERTY_ENTER();
    PROPERTY_VAL(int32_t);

    hr = set_slow(v0);

    PROPERTY_SET_LEAVE();
}
}

#endif
