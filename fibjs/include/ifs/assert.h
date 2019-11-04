/***************************************************************************
 *                                                                         *
 *   This file was automatically generated using idlc.js                   *
 *   PLEASE DO NOT EDIT!!!!                                                *
 *                                                                         *
 ***************************************************************************/

#ifndef _assert_base_H_
#define _assert_base_H_

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
        CONSTRUCT_INIT();
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
        { "ok", s_static_ok, true },
        { "notOk", s_static_notOk, true },
        { "equal", s_static_equal, true },
        { "notEqual", s_static_notEqual, true },
        { "strictEqual", s_static_strictEqual, true },
        { "notStrictEqual", s_static_notStrictEqual, true },
        { "deepEqual", s_static_deepEqual, true },
        { "notDeepEqual", s_static_notDeepEqual, true },
        { "closeTo", s_static_closeTo, true },
        { "notCloseTo", s_static_notCloseTo, true },
        { "lessThan", s_static_lessThan, true },
        { "notLessThan", s_static_notLessThan, true },
        { "greaterThan", s_static_greaterThan, true },
        { "notGreaterThan", s_static_notGreaterThan, true },
        { "exist", s_static_exist, true },
        { "notExist", s_static_notExist, true },
        { "isTrue", s_static_isTrue, true },
        { "isNotTrue", s_static_isNotTrue, true },
        { "isFalse", s_static_isFalse, true },
        { "isNotFalse", s_static_isNotFalse, true },
        { "isNull", s_static_isNull, true },
        { "isNotNull", s_static_isNotNull, true },
        { "isUndefined", s_static_isUndefined, true },
        { "isDefined", s_static_isDefined, true },
        { "isFunction", s_static_isFunction, true },
        { "isNotFunction", s_static_isNotFunction, true },
        { "isObject", s_static_isObject, true },
        { "isNotObject", s_static_isNotObject, true },
        { "isArray", s_static_isArray, true },
        { "isNotArray", s_static_isNotArray, true },
        { "isString", s_static_isString, true },
        { "isNotString", s_static_isNotString, true },
        { "isNumber", s_static_isNumber, true },
        { "isNotNumber", s_static_isNotNumber, true },
        { "isBoolean", s_static_isBoolean, true },
        { "isNotBoolean", s_static_isNotBoolean, true },
        { "typeOf", s_static_typeOf, true },
        { "notTypeOf", s_static_notTypeOf, true },
        { "property", s_static_property, true },
        { "notProperty", s_static_notProperty, true },
        { "deepProperty", s_static_deepProperty, true },
        { "notDeepProperty", s_static_notDeepProperty, true },
        { "propertyVal", s_static_propertyVal, true },
        { "propertyNotVal", s_static_propertyNotVal, true },
        { "deepPropertyVal", s_static_deepPropertyVal, true },
        { "deepPropertyNotVal", s_static_deepPropertyNotVal, true },
        { "throws", s_static_throws, true },
        { "doesNotThrow", s_static_doesNotThrow, true },
        { "ifError", s_static_ifError, true }
    };

    static ClassData s_cd = {
        "assert", true, s__new, s__function,
        ARRAYSIZE(s_method), s_method, 0, NULL, 0, NULL, 0, NULL, NULL, NULL,
        &object_base::class_info()
    };

    static ClassInfo s_ci(s_cd);
    return s_ci;
}

inline void assert_base::s__function(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_NAME("assert");
    METHOD_ENTER();

    METHOD_OVER(2, 0);

    OPT_ARG(v8::Local<v8::Value>, 0, v8::Undefined(isolate));
    OPT_ARG(exlib::string, 1, "");

    hr = _function(v0, v1);

    METHOD_VOID();
}

inline void assert_base::s_static_ok(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_NAME("assert.ok");
    METHOD_ENTER();

    METHOD_OVER(2, 0);

    OPT_ARG(v8::Local<v8::Value>, 0, v8::Undefined(isolate));
    OPT_ARG(exlib::string, 1, "");

    hr = ok(v0, v1);

    METHOD_VOID();
}

inline void assert_base::s_static_notOk(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_NAME("assert.notOk");
    METHOD_ENTER();

    METHOD_OVER(2, 0);

    OPT_ARG(v8::Local<v8::Value>, 0, v8::Undefined(isolate));
    OPT_ARG(exlib::string, 1, "");

    hr = notOk(v0, v1);

    METHOD_VOID();
}

inline void assert_base::s_static_equal(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_NAME("assert.equal");
    METHOD_ENTER();

    METHOD_OVER(3, 0);

    OPT_ARG(v8::Local<v8::Value>, 0, v8::Undefined(isolate));
    OPT_ARG(v8::Local<v8::Value>, 1, v8::Undefined(isolate));
    OPT_ARG(exlib::string, 2, "");

    hr = equal(v0, v1, v2);

    METHOD_VOID();
}

inline void assert_base::s_static_notEqual(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_NAME("assert.notEqual");
    METHOD_ENTER();

    METHOD_OVER(3, 0);

    OPT_ARG(v8::Local<v8::Value>, 0, v8::Undefined(isolate));
    OPT_ARG(v8::Local<v8::Value>, 1, v8::Undefined(isolate));
    OPT_ARG(exlib::string, 2, "");

    hr = notEqual(v0, v1, v2);

    METHOD_VOID();
}

inline void assert_base::s_static_strictEqual(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_NAME("assert.strictEqual");
    METHOD_ENTER();

    METHOD_OVER(3, 0);

    OPT_ARG(v8::Local<v8::Value>, 0, v8::Undefined(isolate));
    OPT_ARG(v8::Local<v8::Value>, 1, v8::Undefined(isolate));
    OPT_ARG(exlib::string, 2, "");

    hr = strictEqual(v0, v1, v2);

    METHOD_VOID();
}

inline void assert_base::s_static_notStrictEqual(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_NAME("assert.notStrictEqual");
    METHOD_ENTER();

    METHOD_OVER(3, 0);

    OPT_ARG(v8::Local<v8::Value>, 0, v8::Undefined(isolate));
    OPT_ARG(v8::Local<v8::Value>, 1, v8::Undefined(isolate));
    OPT_ARG(exlib::string, 2, "");

    hr = notStrictEqual(v0, v1, v2);

    METHOD_VOID();
}

inline void assert_base::s_static_deepEqual(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_NAME("assert.deepEqual");
    METHOD_ENTER();

    METHOD_OVER(3, 0);

    OPT_ARG(v8::Local<v8::Value>, 0, v8::Undefined(isolate));
    OPT_ARG(v8::Local<v8::Value>, 1, v8::Undefined(isolate));
    OPT_ARG(exlib::string, 2, "");

    hr = deepEqual(v0, v1, v2);

    METHOD_VOID();
}

inline void assert_base::s_static_notDeepEqual(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_NAME("assert.notDeepEqual");
    METHOD_ENTER();

    METHOD_OVER(3, 0);

    OPT_ARG(v8::Local<v8::Value>, 0, v8::Undefined(isolate));
    OPT_ARG(v8::Local<v8::Value>, 1, v8::Undefined(isolate));
    OPT_ARG(exlib::string, 2, "");

    hr = notDeepEqual(v0, v1, v2);

    METHOD_VOID();
}

inline void assert_base::s_static_closeTo(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_NAME("assert.closeTo");
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
    METHOD_NAME("assert.notCloseTo");
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
    METHOD_NAME("assert.lessThan");
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
    METHOD_NAME("assert.notLessThan");
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
    METHOD_NAME("assert.greaterThan");
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
    METHOD_NAME("assert.notGreaterThan");
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
    METHOD_NAME("assert.exist");
    METHOD_ENTER();

    METHOD_OVER(2, 1);

    ARG(v8::Local<v8::Value>, 0);
    OPT_ARG(exlib::string, 1, "");

    hr = exist(v0, v1);

    METHOD_VOID();
}

inline void assert_base::s_static_notExist(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_NAME("assert.notExist");
    METHOD_ENTER();

    METHOD_OVER(2, 1);

    ARG(v8::Local<v8::Value>, 0);
    OPT_ARG(exlib::string, 1, "");

    hr = notExist(v0, v1);

    METHOD_VOID();
}

inline void assert_base::s_static_isTrue(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_NAME("assert.isTrue");
    METHOD_ENTER();

    METHOD_OVER(2, 1);

    ARG(v8::Local<v8::Value>, 0);
    OPT_ARG(exlib::string, 1, "");

    hr = isTrue(v0, v1);

    METHOD_VOID();
}

inline void assert_base::s_static_isNotTrue(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_NAME("assert.isNotTrue");
    METHOD_ENTER();

    METHOD_OVER(2, 1);

    ARG(v8::Local<v8::Value>, 0);
    OPT_ARG(exlib::string, 1, "");

    hr = isNotTrue(v0, v1);

    METHOD_VOID();
}

inline void assert_base::s_static_isFalse(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_NAME("assert.isFalse");
    METHOD_ENTER();

    METHOD_OVER(2, 1);

    ARG(v8::Local<v8::Value>, 0);
    OPT_ARG(exlib::string, 1, "");

    hr = isFalse(v0, v1);

    METHOD_VOID();
}

inline void assert_base::s_static_isNotFalse(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_NAME("assert.isNotFalse");
    METHOD_ENTER();

    METHOD_OVER(2, 1);

    ARG(v8::Local<v8::Value>, 0);
    OPT_ARG(exlib::string, 1, "");

    hr = isNotFalse(v0, v1);

    METHOD_VOID();
}

inline void assert_base::s_static_isNull(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_NAME("assert.isNull");
    METHOD_ENTER();

    METHOD_OVER(2, 1);

    ARG(v8::Local<v8::Value>, 0);
    OPT_ARG(exlib::string, 1, "");

    hr = isNull(v0, v1);

    METHOD_VOID();
}

inline void assert_base::s_static_isNotNull(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_NAME("assert.isNotNull");
    METHOD_ENTER();

    METHOD_OVER(2, 1);

    ARG(v8::Local<v8::Value>, 0);
    OPT_ARG(exlib::string, 1, "");

    hr = isNotNull(v0, v1);

    METHOD_VOID();
}

inline void assert_base::s_static_isUndefined(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_NAME("assert.isUndefined");
    METHOD_ENTER();

    METHOD_OVER(2, 1);

    ARG(v8::Local<v8::Value>, 0);
    OPT_ARG(exlib::string, 1, "");

    hr = isUndefined(v0, v1);

    METHOD_VOID();
}

inline void assert_base::s_static_isDefined(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_NAME("assert.isDefined");
    METHOD_ENTER();

    METHOD_OVER(2, 1);

    ARG(v8::Local<v8::Value>, 0);
    OPT_ARG(exlib::string, 1, "");

    hr = isDefined(v0, v1);

    METHOD_VOID();
}

inline void assert_base::s_static_isFunction(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_NAME("assert.isFunction");
    METHOD_ENTER();

    METHOD_OVER(2, 1);

    ARG(v8::Local<v8::Value>, 0);
    OPT_ARG(exlib::string, 1, "");

    hr = isFunction(v0, v1);

    METHOD_VOID();
}

inline void assert_base::s_static_isNotFunction(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_NAME("assert.isNotFunction");
    METHOD_ENTER();

    METHOD_OVER(2, 1);

    ARG(v8::Local<v8::Value>, 0);
    OPT_ARG(exlib::string, 1, "");

    hr = isNotFunction(v0, v1);

    METHOD_VOID();
}

inline void assert_base::s_static_isObject(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_NAME("assert.isObject");
    METHOD_ENTER();

    METHOD_OVER(2, 1);

    ARG(v8::Local<v8::Value>, 0);
    OPT_ARG(exlib::string, 1, "");

    hr = isObject(v0, v1);

    METHOD_VOID();
}

inline void assert_base::s_static_isNotObject(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_NAME("assert.isNotObject");
    METHOD_ENTER();

    METHOD_OVER(2, 1);

    ARG(v8::Local<v8::Value>, 0);
    OPT_ARG(exlib::string, 1, "");

    hr = isNotObject(v0, v1);

    METHOD_VOID();
}

inline void assert_base::s_static_isArray(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_NAME("assert.isArray");
    METHOD_ENTER();

    METHOD_OVER(2, 1);

    ARG(v8::Local<v8::Value>, 0);
    OPT_ARG(exlib::string, 1, "");

    hr = isArray(v0, v1);

    METHOD_VOID();
}

inline void assert_base::s_static_isNotArray(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_NAME("assert.isNotArray");
    METHOD_ENTER();

    METHOD_OVER(2, 1);

    ARG(v8::Local<v8::Value>, 0);
    OPT_ARG(exlib::string, 1, "");

    hr = isNotArray(v0, v1);

    METHOD_VOID();
}

inline void assert_base::s_static_isString(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_NAME("assert.isString");
    METHOD_ENTER();

    METHOD_OVER(2, 1);

    ARG(v8::Local<v8::Value>, 0);
    OPT_ARG(exlib::string, 1, "");

    hr = isString(v0, v1);

    METHOD_VOID();
}

inline void assert_base::s_static_isNotString(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_NAME("assert.isNotString");
    METHOD_ENTER();

    METHOD_OVER(2, 1);

    ARG(v8::Local<v8::Value>, 0);
    OPT_ARG(exlib::string, 1, "");

    hr = isNotString(v0, v1);

    METHOD_VOID();
}

inline void assert_base::s_static_isNumber(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_NAME("assert.isNumber");
    METHOD_ENTER();

    METHOD_OVER(2, 1);

    ARG(v8::Local<v8::Value>, 0);
    OPT_ARG(exlib::string, 1, "");

    hr = isNumber(v0, v1);

    METHOD_VOID();
}

inline void assert_base::s_static_isNotNumber(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_NAME("assert.isNotNumber");
    METHOD_ENTER();

    METHOD_OVER(2, 1);

    ARG(v8::Local<v8::Value>, 0);
    OPT_ARG(exlib::string, 1, "");

    hr = isNotNumber(v0, v1);

    METHOD_VOID();
}

inline void assert_base::s_static_isBoolean(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_NAME("assert.isBoolean");
    METHOD_ENTER();

    METHOD_OVER(2, 1);

    ARG(v8::Local<v8::Value>, 0);
    OPT_ARG(exlib::string, 1, "");

    hr = isBoolean(v0, v1);

    METHOD_VOID();
}

inline void assert_base::s_static_isNotBoolean(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_NAME("assert.isNotBoolean");
    METHOD_ENTER();

    METHOD_OVER(2, 1);

    ARG(v8::Local<v8::Value>, 0);
    OPT_ARG(exlib::string, 1, "");

    hr = isNotBoolean(v0, v1);

    METHOD_VOID();
}

inline void assert_base::s_static_typeOf(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_NAME("assert.typeOf");
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
    METHOD_NAME("assert.notTypeOf");
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
    METHOD_NAME("assert.property");
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
    METHOD_NAME("assert.notProperty");
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
    METHOD_NAME("assert.deepProperty");
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
    METHOD_NAME("assert.notDeepProperty");
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
    METHOD_NAME("assert.propertyVal");
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
    METHOD_NAME("assert.propertyNotVal");
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
    METHOD_NAME("assert.deepPropertyVal");
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
    METHOD_NAME("assert.deepPropertyNotVal");
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
    METHOD_NAME("assert.throws");
    METHOD_ENTER();

    METHOD_OVER(2, 1);

    ARG(v8::Local<v8::Function>, 0);
    OPT_ARG(exlib::string, 1, "");

    hr = throws(v0, v1);

    METHOD_VOID();
}

inline void assert_base::s_static_doesNotThrow(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_NAME("assert.doesNotThrow");
    METHOD_ENTER();

    METHOD_OVER(2, 1);

    ARG(v8::Local<v8::Function>, 0);
    OPT_ARG(exlib::string, 1, "");

    hr = doesNotThrow(v0, v1);

    METHOD_VOID();
}

inline void assert_base::s_static_ifError(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_NAME("assert.ifError");
    METHOD_ENTER();

    METHOD_OVER(1, 0);

    OPT_ARG(v8::Local<v8::Value>, 0, v8::Undefined(isolate));

    hr = ifError(v0);

    METHOD_VOID();
}
}

#endif
