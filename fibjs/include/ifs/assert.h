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

class assert_base : public object_base {
    DECLARE_CLASS(assert_base);

public:
    // assert_base
    static result_t _function(v8::Local<v8::Value> actual, exlib::string msg);
    static result_t ok(v8::Local<v8::Value> actual, exlib::string msg);
    static result_t notOk(v8::Local<v8::Value> actual, exlib::string msg);
    static result_t equal(v8::Local<v8::Value> actual, v8::Local<v8::Value> expected, exlib::string msg);
    static result_t notEqual(v8::Local<v8::Value> actual, v8::Local<v8::Value> expected, exlib::string msg);
    static result_t strictEqual(v8::Local<v8::Value> actual, v8::Local<v8::Value> expected, exlib::string msg);
    static result_t notStrictEqual(v8::Local<v8::Value> actual, v8::Local<v8::Value> expected, exlib::string msg);
    static result_t deepEqual(v8::Local<v8::Value> actual, v8::Local<v8::Value> expected, exlib::string msg);
    static result_t notDeepEqual(v8::Local<v8::Value> actual, v8::Local<v8::Value> expected, exlib::string msg);
    static result_t closeTo(v8::Local<v8::Value> actual, v8::Local<v8::Value> expected, v8::Local<v8::Value> delta, exlib::string msg);
    static result_t notCloseTo(v8::Local<v8::Value> actual, v8::Local<v8::Value> expected, v8::Local<v8::Value> delta, exlib::string msg);
    static result_t lessThan(v8::Local<v8::Value> actual, v8::Local<v8::Value> expected, exlib::string msg);
    static result_t notLessThan(v8::Local<v8::Value> actual, v8::Local<v8::Value> expected, exlib::string msg);
    static result_t greaterThan(v8::Local<v8::Value> actual, v8::Local<v8::Value> expected, exlib::string msg);
    static result_t notGreaterThan(v8::Local<v8::Value> actual, v8::Local<v8::Value> expected, exlib::string msg);
    static result_t exist(v8::Local<v8::Value> actual, exlib::string msg);
    static result_t notExist(v8::Local<v8::Value> actual, exlib::string msg);
    static result_t isTrue(v8::Local<v8::Value> actual, exlib::string msg);
    static result_t isNotTrue(v8::Local<v8::Value> actual, exlib::string msg);
    static result_t isFalse(v8::Local<v8::Value> actual, exlib::string msg);
    static result_t isNotFalse(v8::Local<v8::Value> actual, exlib::string msg);
    static result_t isNull(v8::Local<v8::Value> actual, exlib::string msg);
    static result_t isNotNull(v8::Local<v8::Value> actual, exlib::string msg);
    static result_t isUndefined(v8::Local<v8::Value> actual, exlib::string msg);
    static result_t isDefined(v8::Local<v8::Value> actual, exlib::string msg);
    static result_t isFunction(v8::Local<v8::Value> actual, exlib::string msg);
    static result_t isNotFunction(v8::Local<v8::Value> actual, exlib::string msg);
    static result_t isObject(v8::Local<v8::Value> actual, exlib::string msg);
    static result_t isNotObject(v8::Local<v8::Value> actual, exlib::string msg);
    static result_t isArray(v8::Local<v8::Value> actual, exlib::string msg);
    static result_t isNotArray(v8::Local<v8::Value> actual, exlib::string msg);
    static result_t isString(v8::Local<v8::Value> actual, exlib::string msg);
    static result_t isNotString(v8::Local<v8::Value> actual, exlib::string msg);
    static result_t isNumber(v8::Local<v8::Value> actual, exlib::string msg);
    static result_t isNotNumber(v8::Local<v8::Value> actual, exlib::string msg);
    static result_t isBoolean(v8::Local<v8::Value> actual, exlib::string msg);
    static result_t isNotBoolean(v8::Local<v8::Value> actual, exlib::string msg);
    static result_t typeOf(v8::Local<v8::Value> actual, exlib::string type, exlib::string msg);
    static result_t notTypeOf(v8::Local<v8::Value> actual, exlib::string type, exlib::string msg);
    static result_t property(v8::Local<v8::Value> object, v8::Local<v8::Value> prop, exlib::string msg);
    static result_t notProperty(v8::Local<v8::Value> object, v8::Local<v8::Value> prop, exlib::string msg);
    static result_t deepProperty(v8::Local<v8::Value> object, v8::Local<v8::Value> prop, exlib::string msg);
    static result_t notDeepProperty(v8::Local<v8::Value> object, v8::Local<v8::Value> prop, exlib::string msg);
    static result_t propertyVal(v8::Local<v8::Value> object, v8::Local<v8::Value> prop, v8::Local<v8::Value> value, exlib::string msg);
    static result_t propertyNotVal(v8::Local<v8::Value> object, v8::Local<v8::Value> prop, v8::Local<v8::Value> value, exlib::string msg);
    static result_t deepPropertyVal(v8::Local<v8::Value> object, v8::Local<v8::Value> prop, v8::Local<v8::Value> value, exlib::string msg);
    static result_t deepPropertyNotVal(v8::Local<v8::Value> object, v8::Local<v8::Value> prop, v8::Local<v8::Value> value, exlib::string msg);
    static result_t throws(v8::Local<v8::Function> block, exlib::string msg);
    static result_t doesNotThrow(v8::Local<v8::Function> block, exlib::string msg);
    static result_t ifError(v8::Local<v8::Value> object);

public:
    static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        s__function(args);
    }

public:
    static void s__function(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_ok(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_notOk(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_equal(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_notEqual(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_strictEqual(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_notStrictEqual(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_deepEqual(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_notDeepEqual(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_closeTo(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_notCloseTo(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_lessThan(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_notLessThan(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_greaterThan(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_notGreaterThan(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_exist(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_notExist(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_isTrue(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_isNotTrue(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_isFalse(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_isNotFalse(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_isNull(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_isNotNull(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_isUndefined(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_isDefined(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_isFunction(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_isNotFunction(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_isObject(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_isNotObject(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_isArray(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_isNotArray(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_isString(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_isNotString(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_isNumber(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_isNotNumber(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_isBoolean(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_isNotBoolean(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_typeOf(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_notTypeOf(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_property(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_notProperty(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_deepProperty(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_notDeepProperty(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_propertyVal(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_propertyNotVal(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_deepPropertyVal(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_deepPropertyNotVal(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_throws(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_doesNotThrow(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_ifError(const v8::FunctionCallbackInfo<v8::Value>& args);
};
}

namespace fibjs {
inline ClassInfo& assert_base::class_info()
{
    static ClassData::ClassMethod s_method[] = {
        { "ok", s_static_ok, true, ClassData::ASYNC_SYNC },
        { "notOk", s_static_notOk, true, ClassData::ASYNC_SYNC },
        { "equal", s_static_equal, true, ClassData::ASYNC_SYNC },
        { "notEqual", s_static_notEqual, true, ClassData::ASYNC_SYNC },
        { "strictEqual", s_static_strictEqual, true, ClassData::ASYNC_SYNC },
        { "notStrictEqual", s_static_notStrictEqual, true, ClassData::ASYNC_SYNC },
        { "deepEqual", s_static_deepEqual, true, ClassData::ASYNC_SYNC },
        { "notDeepEqual", s_static_notDeepEqual, true, ClassData::ASYNC_SYNC },
        { "closeTo", s_static_closeTo, true, ClassData::ASYNC_SYNC },
        { "notCloseTo", s_static_notCloseTo, true, ClassData::ASYNC_SYNC },
        { "lessThan", s_static_lessThan, true, ClassData::ASYNC_SYNC },
        { "notLessThan", s_static_notLessThan, true, ClassData::ASYNC_SYNC },
        { "greaterThan", s_static_greaterThan, true, ClassData::ASYNC_SYNC },
        { "notGreaterThan", s_static_notGreaterThan, true, ClassData::ASYNC_SYNC },
        { "exist", s_static_exist, true, ClassData::ASYNC_SYNC },
        { "notExist", s_static_notExist, true, ClassData::ASYNC_SYNC },
        { "isTrue", s_static_isTrue, true, ClassData::ASYNC_SYNC },
        { "isNotTrue", s_static_isNotTrue, true, ClassData::ASYNC_SYNC },
        { "isFalse", s_static_isFalse, true, ClassData::ASYNC_SYNC },
        { "isNotFalse", s_static_isNotFalse, true, ClassData::ASYNC_SYNC },
        { "isNull", s_static_isNull, true, ClassData::ASYNC_SYNC },
        { "isNotNull", s_static_isNotNull, true, ClassData::ASYNC_SYNC },
        { "isUndefined", s_static_isUndefined, true, ClassData::ASYNC_SYNC },
        { "isDefined", s_static_isDefined, true, ClassData::ASYNC_SYNC },
        { "isFunction", s_static_isFunction, true, ClassData::ASYNC_SYNC },
        { "isNotFunction", s_static_isNotFunction, true, ClassData::ASYNC_SYNC },
        { "isObject", s_static_isObject, true, ClassData::ASYNC_SYNC },
        { "isNotObject", s_static_isNotObject, true, ClassData::ASYNC_SYNC },
        { "isArray", s_static_isArray, true, ClassData::ASYNC_SYNC },
        { "isNotArray", s_static_isNotArray, true, ClassData::ASYNC_SYNC },
        { "isString", s_static_isString, true, ClassData::ASYNC_SYNC },
        { "isNotString", s_static_isNotString, true, ClassData::ASYNC_SYNC },
        { "isNumber", s_static_isNumber, true, ClassData::ASYNC_SYNC },
        { "isNotNumber", s_static_isNotNumber, true, ClassData::ASYNC_SYNC },
        { "isBoolean", s_static_isBoolean, true, ClassData::ASYNC_SYNC },
        { "isNotBoolean", s_static_isNotBoolean, true, ClassData::ASYNC_SYNC },
        { "typeOf", s_static_typeOf, true, ClassData::ASYNC_SYNC },
        { "notTypeOf", s_static_notTypeOf, true, ClassData::ASYNC_SYNC },
        { "property", s_static_property, true, ClassData::ASYNC_SYNC },
        { "notProperty", s_static_notProperty, true, ClassData::ASYNC_SYNC },
        { "deepProperty", s_static_deepProperty, true, ClassData::ASYNC_SYNC },
        { "notDeepProperty", s_static_notDeepProperty, true, ClassData::ASYNC_SYNC },
        { "propertyVal", s_static_propertyVal, true, ClassData::ASYNC_SYNC },
        { "propertyNotVal", s_static_propertyNotVal, true, ClassData::ASYNC_SYNC },
        { "deepPropertyVal", s_static_deepPropertyVal, true, ClassData::ASYNC_SYNC },
        { "deepPropertyNotVal", s_static_deepPropertyNotVal, true, ClassData::ASYNC_SYNC },
        { "throws", s_static_throws, true, ClassData::ASYNC_SYNC },
        { "doesNotThrow", s_static_doesNotThrow, true, ClassData::ASYNC_SYNC },
        { "ifError", s_static_ifError, true, ClassData::ASYNC_SYNC }
    };

    static ClassData s_cd = {
        "assert", true, s__new, s__function,
        ARRAYSIZE(s_method), s_method, 0, NULL, 0, NULL, 0, NULL, NULL, NULL,
        &object_base::class_info(),
        false
    };

    static ClassInfo s_ci(s_cd);
    return s_ci;
}

inline void assert_base::s__function(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_ENTER();

    METHOD_OVER(2, 0);

    OPT_ARG(v8::Local<v8::Value>, 0, v8::Undefined(isolate->m_isolate));
    OPT_ARG(exlib::string, 1, "");

    hr = _function(v0, v1);

    METHOD_VOID();
}

inline void assert_base::s_static_ok(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_ENTER();

    METHOD_OVER(2, 1);

    ARG(v8::Local<v8::Value>, 0);
    OPT_ARG(exlib::string, 1, "");

    hr = ok(v0, v1);

    METHOD_VOID();
}

inline void assert_base::s_static_notOk(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_ENTER();

    METHOD_OVER(2, 1);

    ARG(v8::Local<v8::Value>, 0);
    OPT_ARG(exlib::string, 1, "");

    hr = notOk(v0, v1);

    METHOD_VOID();
}

inline void assert_base::s_static_equal(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_ENTER();

    METHOD_OVER(3, 2);

    ARG(v8::Local<v8::Value>, 0);
    ARG(v8::Local<v8::Value>, 1);
    OPT_ARG(exlib::string, 2, "");

    hr = equal(v0, v1, v2);

    METHOD_VOID();
}

inline void assert_base::s_static_notEqual(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_ENTER();

    METHOD_OVER(3, 2);

    ARG(v8::Local<v8::Value>, 0);
    ARG(v8::Local<v8::Value>, 1);
    OPT_ARG(exlib::string, 2, "");

    hr = notEqual(v0, v1, v2);

    METHOD_VOID();
}

inline void assert_base::s_static_strictEqual(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_ENTER();

    METHOD_OVER(3, 2);

    ARG(v8::Local<v8::Value>, 0);
    ARG(v8::Local<v8::Value>, 1);
    OPT_ARG(exlib::string, 2, "");

    hr = strictEqual(v0, v1, v2);

    METHOD_VOID();
}

inline void assert_base::s_static_notStrictEqual(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_ENTER();

    METHOD_OVER(3, 2);

    ARG(v8::Local<v8::Value>, 0);
    ARG(v8::Local<v8::Value>, 1);
    OPT_ARG(exlib::string, 2, "");

    hr = notStrictEqual(v0, v1, v2);

    METHOD_VOID();
}

inline void assert_base::s_static_deepEqual(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_ENTER();

    METHOD_OVER(3, 2);

    ARG(v8::Local<v8::Value>, 0);
    ARG(v8::Local<v8::Value>, 1);
    OPT_ARG(exlib::string, 2, "");

    hr = deepEqual(v0, v1, v2);

    METHOD_VOID();
}

inline void assert_base::s_static_notDeepEqual(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_ENTER();

    METHOD_OVER(3, 2);

    ARG(v8::Local<v8::Value>, 0);
    ARG(v8::Local<v8::Value>, 1);
    OPT_ARG(exlib::string, 2, "");

    hr = notDeepEqual(v0, v1, v2);

    METHOD_VOID();
}

inline void assert_base::s_static_closeTo(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_ENTER();

    METHOD_OVER(4, 3);

    ARG(v8::Local<v8::Value>, 0);
    ARG(v8::Local<v8::Value>, 1);
    ARG(v8::Local<v8::Value>, 2);
    OPT_ARG(exlib::string, 3, "");

    hr = closeTo(v0, v1, v2, v3);

    METHOD_VOID();
}

inline void assert_base::s_static_notCloseTo(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_ENTER();

    METHOD_OVER(4, 3);

    ARG(v8::Local<v8::Value>, 0);
    ARG(v8::Local<v8::Value>, 1);
    ARG(v8::Local<v8::Value>, 2);
    OPT_ARG(exlib::string, 3, "");

    hr = notCloseTo(v0, v1, v2, v3);

    METHOD_VOID();
}

inline void assert_base::s_static_lessThan(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_ENTER();

    METHOD_OVER(3, 2);

    ARG(v8::Local<v8::Value>, 0);
    ARG(v8::Local<v8::Value>, 1);
    OPT_ARG(exlib::string, 2, "");

    hr = lessThan(v0, v1, v2);

    METHOD_VOID();
}

inline void assert_base::s_static_notLessThan(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_ENTER();

    METHOD_OVER(3, 2);

    ARG(v8::Local<v8::Value>, 0);
    ARG(v8::Local<v8::Value>, 1);
    OPT_ARG(exlib::string, 2, "");

    hr = notLessThan(v0, v1, v2);

    METHOD_VOID();
}

inline void assert_base::s_static_greaterThan(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_ENTER();

    METHOD_OVER(3, 2);

    ARG(v8::Local<v8::Value>, 0);
    ARG(v8::Local<v8::Value>, 1);
    OPT_ARG(exlib::string, 2, "");

    hr = greaterThan(v0, v1, v2);

    METHOD_VOID();
}

inline void assert_base::s_static_notGreaterThan(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_ENTER();

    METHOD_OVER(3, 2);

    ARG(v8::Local<v8::Value>, 0);
    ARG(v8::Local<v8::Value>, 1);
    OPT_ARG(exlib::string, 2, "");

    hr = notGreaterThan(v0, v1, v2);

    METHOD_VOID();
}

inline void assert_base::s_static_exist(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_ENTER();

    METHOD_OVER(2, 1);

    ARG(v8::Local<v8::Value>, 0);
    OPT_ARG(exlib::string, 1, "");

    hr = exist(v0, v1);

    METHOD_VOID();
}

inline void assert_base::s_static_notExist(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_ENTER();

    METHOD_OVER(2, 1);

    ARG(v8::Local<v8::Value>, 0);
    OPT_ARG(exlib::string, 1, "");

    hr = notExist(v0, v1);

    METHOD_VOID();
}

inline void assert_base::s_static_isTrue(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_ENTER();

    METHOD_OVER(2, 1);

    ARG(v8::Local<v8::Value>, 0);
    OPT_ARG(exlib::string, 1, "");

    hr = isTrue(v0, v1);

    METHOD_VOID();
}

inline void assert_base::s_static_isNotTrue(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_ENTER();

    METHOD_OVER(2, 1);

    ARG(v8::Local<v8::Value>, 0);
    OPT_ARG(exlib::string, 1, "");

    hr = isNotTrue(v0, v1);

    METHOD_VOID();
}

inline void assert_base::s_static_isFalse(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_ENTER();

    METHOD_OVER(2, 1);

    ARG(v8::Local<v8::Value>, 0);
    OPT_ARG(exlib::string, 1, "");

    hr = isFalse(v0, v1);

    METHOD_VOID();
}

inline void assert_base::s_static_isNotFalse(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_ENTER();

    METHOD_OVER(2, 1);

    ARG(v8::Local<v8::Value>, 0);
    OPT_ARG(exlib::string, 1, "");

    hr = isNotFalse(v0, v1);

    METHOD_VOID();
}

inline void assert_base::s_static_isNull(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_ENTER();

    METHOD_OVER(2, 1);

    ARG(v8::Local<v8::Value>, 0);
    OPT_ARG(exlib::string, 1, "");

    hr = isNull(v0, v1);

    METHOD_VOID();
}

inline void assert_base::s_static_isNotNull(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_ENTER();

    METHOD_OVER(2, 1);

    ARG(v8::Local<v8::Value>, 0);
    OPT_ARG(exlib::string, 1, "");

    hr = isNotNull(v0, v1);

    METHOD_VOID();
}

inline void assert_base::s_static_isUndefined(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_ENTER();

    METHOD_OVER(2, 1);

    ARG(v8::Local<v8::Value>, 0);
    OPT_ARG(exlib::string, 1, "");

    hr = isUndefined(v0, v1);

    METHOD_VOID();
}

inline void assert_base::s_static_isDefined(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_ENTER();

    METHOD_OVER(2, 1);

    ARG(v8::Local<v8::Value>, 0);
    OPT_ARG(exlib::string, 1, "");

    hr = isDefined(v0, v1);

    METHOD_VOID();
}

inline void assert_base::s_static_isFunction(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_ENTER();

    METHOD_OVER(2, 1);

    ARG(v8::Local<v8::Value>, 0);
    OPT_ARG(exlib::string, 1, "");

    hr = isFunction(v0, v1);

    METHOD_VOID();
}

inline void assert_base::s_static_isNotFunction(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_ENTER();

    METHOD_OVER(2, 1);

    ARG(v8::Local<v8::Value>, 0);
    OPT_ARG(exlib::string, 1, "");

    hr = isNotFunction(v0, v1);

    METHOD_VOID();
}

inline void assert_base::s_static_isObject(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_ENTER();

    METHOD_OVER(2, 1);

    ARG(v8::Local<v8::Value>, 0);
    OPT_ARG(exlib::string, 1, "");

    hr = isObject(v0, v1);

    METHOD_VOID();
}

inline void assert_base::s_static_isNotObject(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_ENTER();

    METHOD_OVER(2, 1);

    ARG(v8::Local<v8::Value>, 0);
    OPT_ARG(exlib::string, 1, "");

    hr = isNotObject(v0, v1);

    METHOD_VOID();
}

inline void assert_base::s_static_isArray(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_ENTER();

    METHOD_OVER(2, 1);

    ARG(v8::Local<v8::Value>, 0);
    OPT_ARG(exlib::string, 1, "");

    hr = isArray(v0, v1);

    METHOD_VOID();
}

inline void assert_base::s_static_isNotArray(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_ENTER();

    METHOD_OVER(2, 1);

    ARG(v8::Local<v8::Value>, 0);
    OPT_ARG(exlib::string, 1, "");

    hr = isNotArray(v0, v1);

    METHOD_VOID();
}

inline void assert_base::s_static_isString(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_ENTER();

    METHOD_OVER(2, 1);

    ARG(v8::Local<v8::Value>, 0);
    OPT_ARG(exlib::string, 1, "");

    hr = isString(v0, v1);

    METHOD_VOID();
}

inline void assert_base::s_static_isNotString(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_ENTER();

    METHOD_OVER(2, 1);

    ARG(v8::Local<v8::Value>, 0);
    OPT_ARG(exlib::string, 1, "");

    hr = isNotString(v0, v1);

    METHOD_VOID();
}

inline void assert_base::s_static_isNumber(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_ENTER();

    METHOD_OVER(2, 1);

    ARG(v8::Local<v8::Value>, 0);
    OPT_ARG(exlib::string, 1, "");

    hr = isNumber(v0, v1);

    METHOD_VOID();
}

inline void assert_base::s_static_isNotNumber(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_ENTER();

    METHOD_OVER(2, 1);

    ARG(v8::Local<v8::Value>, 0);
    OPT_ARG(exlib::string, 1, "");

    hr = isNotNumber(v0, v1);

    METHOD_VOID();
}

inline void assert_base::s_static_isBoolean(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_ENTER();

    METHOD_OVER(2, 1);

    ARG(v8::Local<v8::Value>, 0);
    OPT_ARG(exlib::string, 1, "");

    hr = isBoolean(v0, v1);

    METHOD_VOID();
}

inline void assert_base::s_static_isNotBoolean(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_ENTER();

    METHOD_OVER(2, 1);

    ARG(v8::Local<v8::Value>, 0);
    OPT_ARG(exlib::string, 1, "");

    hr = isNotBoolean(v0, v1);

    METHOD_VOID();
}

inline void assert_base::s_static_typeOf(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_ENTER();

    METHOD_OVER(3, 2);

    ARG(v8::Local<v8::Value>, 0);
    ARG(exlib::string, 1);
    OPT_ARG(exlib::string, 2, "");

    hr = typeOf(v0, v1, v2);

    METHOD_VOID();
}

inline void assert_base::s_static_notTypeOf(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_ENTER();

    METHOD_OVER(3, 2);

    ARG(v8::Local<v8::Value>, 0);
    ARG(exlib::string, 1);
    OPT_ARG(exlib::string, 2, "");

    hr = notTypeOf(v0, v1, v2);

    METHOD_VOID();
}

inline void assert_base::s_static_property(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_ENTER();

    METHOD_OVER(3, 2);

    ARG(v8::Local<v8::Value>, 0);
    ARG(v8::Local<v8::Value>, 1);
    OPT_ARG(exlib::string, 2, "");

    hr = property(v0, v1, v2);

    METHOD_VOID();
}

inline void assert_base::s_static_notProperty(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_ENTER();

    METHOD_OVER(3, 2);

    ARG(v8::Local<v8::Value>, 0);
    ARG(v8::Local<v8::Value>, 1);
    OPT_ARG(exlib::string, 2, "");

    hr = notProperty(v0, v1, v2);

    METHOD_VOID();
}

inline void assert_base::s_static_deepProperty(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_ENTER();

    METHOD_OVER(3, 2);

    ARG(v8::Local<v8::Value>, 0);
    ARG(v8::Local<v8::Value>, 1);
    OPT_ARG(exlib::string, 2, "");

    hr = deepProperty(v0, v1, v2);

    METHOD_VOID();
}

inline void assert_base::s_static_notDeepProperty(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_ENTER();

    METHOD_OVER(3, 2);

    ARG(v8::Local<v8::Value>, 0);
    ARG(v8::Local<v8::Value>, 1);
    OPT_ARG(exlib::string, 2, "");

    hr = notDeepProperty(v0, v1, v2);

    METHOD_VOID();
}

inline void assert_base::s_static_propertyVal(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_ENTER();

    METHOD_OVER(4, 3);

    ARG(v8::Local<v8::Value>, 0);
    ARG(v8::Local<v8::Value>, 1);
    ARG(v8::Local<v8::Value>, 2);
    OPT_ARG(exlib::string, 3, "");

    hr = propertyVal(v0, v1, v2, v3);

    METHOD_VOID();
}

inline void assert_base::s_static_propertyNotVal(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_ENTER();

    METHOD_OVER(4, 3);

    ARG(v8::Local<v8::Value>, 0);
    ARG(v8::Local<v8::Value>, 1);
    ARG(v8::Local<v8::Value>, 2);
    OPT_ARG(exlib::string, 3, "");

    hr = propertyNotVal(v0, v1, v2, v3);

    METHOD_VOID();
}

inline void assert_base::s_static_deepPropertyVal(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_ENTER();

    METHOD_OVER(4, 3);

    ARG(v8::Local<v8::Value>, 0);
    ARG(v8::Local<v8::Value>, 1);
    ARG(v8::Local<v8::Value>, 2);
    OPT_ARG(exlib::string, 3, "");

    hr = deepPropertyVal(v0, v1, v2, v3);

    METHOD_VOID();
}

inline void assert_base::s_static_deepPropertyNotVal(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_ENTER();

    METHOD_OVER(4, 3);

    ARG(v8::Local<v8::Value>, 0);
    ARG(v8::Local<v8::Value>, 1);
    ARG(v8::Local<v8::Value>, 2);
    OPT_ARG(exlib::string, 3, "");

    hr = deepPropertyNotVal(v0, v1, v2, v3);

    METHOD_VOID();
}

inline void assert_base::s_static_throws(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_ENTER();

    METHOD_OVER(2, 1);

    ARG(v8::Local<v8::Function>, 0);
    OPT_ARG(exlib::string, 1, "");

    hr = throws(v0, v1);

    METHOD_VOID();
}

inline void assert_base::s_static_doesNotThrow(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_ENTER();

    METHOD_OVER(2, 1);

    ARG(v8::Local<v8::Function>, 0);
    OPT_ARG(exlib::string, 1, "");

    hr = doesNotThrow(v0, v1);

    METHOD_VOID();
}

inline void assert_base::s_static_ifError(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_ENTER();

    METHOD_OVER(1, 0);

    OPT_ARG(v8::Local<v8::Value>, 0, v8::Undefined(isolate->m_isolate));

    hr = ifError(v0);

    METHOD_VOID();
}
}
