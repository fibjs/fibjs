/*
 * assert.cpp
 *
 *  Created on: Apr 7, 2012
 *      Author: lion
 */

#include "utils.h"
#include "object.h"
#include "ifs/assert.h"
#include "ifs/encoding.h"
#include "ifs/util.h"
#include "QuickArray.h"
#include "../util/util.h"
#include "assert.h"

namespace fibjs {

DECLARE_MODULE(assert);

result_t assert_base::_function(v8::Local<v8::Value> actual, exlib::string msg)
{
    return ok(actual, msg);
}

result_t assert_base::ok(v8::Local<v8::Value> actual, exlib::string msg)
{
    return _test(Isolate::current()->toBoolean(actual),
        _msg(msg, "expected ", actual, " to be truthy"));
}

result_t assert_base::notOk(v8::Local<v8::Value> actual, exlib::string msg)
{
    return _test(!Isolate::current()->toBoolean(actual),
        _msg(msg, "expected ", actual, " to be falsy"));
}

result_t assert_base::equal(v8::Local<v8::Value> actual,
    v8::Local<v8::Value> expected, exlib::string msg)
{
    bool tst = actual->Equals(Isolate::current()->context(), expected).FromMaybe(false);
    return _test(tst, _msg(msg, "expected ", actual, " to equal ", expected));
}

result_t assert_base::notEqual(v8::Local<v8::Value> actual,
    v8::Local<v8::Value> expected, exlib::string msg)
{
    bool tst = !actual->Equals(Isolate::current()->context(), expected).FromMaybe(false);
    return _test(tst, _msg(msg, "expected ", actual, " to not equal ", expected));
}

result_t assert_base::strictEqual(v8::Local<v8::Value> actual,
    v8::Local<v8::Value> expected, exlib::string msg)
{
    return _test(actual->StrictEquals(expected),
        _msg(msg, "expected ", actual, " to strictly equal ", expected));
}

result_t assert_base::notStrictEqual(v8::Local<v8::Value> actual,
    v8::Local<v8::Value> expected, exlib::string msg)
{
    return _test(!actual->StrictEquals(expected),
        _msg(msg, "expected ", actual, " to not strictly equal ",
            expected));
}

result_t assert_base::deepEqual(v8::Local<v8::Value> actual,
    v8::Local<v8::Value> expected, exlib::string msg)
{
    bool r;

    util_base::isDeepEqual(actual, expected, r);
    return _test(r, _msg(msg, "expected ", actual, " to deeply equal ", expected));
}

result_t assert_base::notDeepEqual(v8::Local<v8::Value> actual,
    v8::Local<v8::Value> expected, exlib::string msg)
{
    bool r;

    util_base::isDeepEqual(actual, expected, r);
    return _test(!r, _msg(msg, "expected ", actual, " to not deeply equal ", expected));
}

result_t assert_base::deepStrictEqual(v8::Local<v8::Value> actual,
    v8::Local<v8::Value> expected, exlib::string msg)
{
    bool r;

    util_base::isDeepStrictEqual(actual, expected, r);
    return _test(r, _msg(msg, "expected ", actual, " to deeply strict equal ", expected));
}

result_t assert_base::notDeepStrictEqual(v8::Local<v8::Value> actual,
    v8::Local<v8::Value> expected, exlib::string msg)
{
    bool r;

    util_base::isDeepStrictEqual(actual, expected, r);
    return _test(!r, _msg(msg, "expected ", actual, " to not deeply strict equal ", expected));
}

result_t assert_base::match(exlib::string actual, v8::Local<v8::RegExp> expected, exlib::string msg)
{
    bool r;

    Isolate* isolate = Isolate::current();
    v8::Local<v8::Context> context = isolate->context();
    v8::Local<v8::String> str = isolate->NewString(actual);
    v8::Local<v8::Object> v = expected->Exec(isolate->context(), str).ToLocalChecked();
    r = !v.IsEmpty() && v->IsArray() && v.As<v8::Array>()->Length() > 0;

    v8::Local<v8::Value> regexp_val = expected->ToString(context).ToLocalChecked();
    v8::Local<v8::Value> actual_val = str;
    return _test(r, _msg(msg, "expected ", actual_val, " to match ", regexp_val));
}

result_t assert_base::doesNotMatch(exlib::string actual, v8::Local<v8::RegExp> expected, exlib::string msg)
{
    bool r;

    Isolate* isolate = Isolate::current();
    v8::Local<v8::Context> context = isolate->context();
    v8::Local<v8::String> str = isolate->NewString(actual);
    v8::Local<v8::Object> v = expected->Exec(isolate->context(), str).ToLocalChecked();
    r = !v.IsEmpty() && v->IsArray() && v.As<v8::Array>()->Length() > 0;

    v8::Local<v8::Value> regexp_val = expected->ToString(context).ToLocalChecked();
    v8::Local<v8::Value> actual_val = str;
    return _test(!r, _msg(msg, "expected ", actual_val, " to not match ", regexp_val));
}

result_t assert_base::closeTo(v8::Local<v8::Value> actual,
    v8::Local<v8::Value> expected, v8::Local<v8::Value> delta,
    exlib::string msg)
{
    double n, n1;

    Isolate* isolate = Isolate::current();

    n = isolate->toNumber(actual);
    if (std::isnan(n))
        return CHECK_ERROR(CALL_E_INVALIDARG);

    n1 = isolate->toNumber(expected);
    if (std::isnan(n1))
        return CHECK_ERROR(CALL_E_INVALIDARG);

    n -= n1;

    n1 = isolate->toNumber(delta);
    if (std::isnan(n1))
        return CHECK_ERROR(CALL_E_INVALIDARG);

    if (n < 0)
        n = -n;
    return _test(n <= n1,
        _msg(msg, "expected ", actual, " to be close to ", expected,
            " +/- ", delta));
}

result_t assert_base::notCloseTo(v8::Local<v8::Value> actual,
    v8::Local<v8::Value> expected, v8::Local<v8::Value> delta,
    exlib::string msg)
{
    double n, n1;

    Isolate* isolate = Isolate::current();

    n = isolate->toNumber(actual);
    if (std::isnan(n))
        return CHECK_ERROR(CALL_E_INVALIDARG);

    n1 = isolate->toNumber(expected);
    if (std::isnan(n1))
        return CHECK_ERROR(CALL_E_INVALIDARG);

    n -= n1;

    n1 = isolate->toNumber(delta);
    if (std::isnan(n1))
        return CHECK_ERROR(CALL_E_INVALIDARG);

    if (n < 0)
        n = -n;
    return _test(n > n1,
        _msg(msg, "expected ", actual, " not to be close to ", expected,
            " +/- ", delta));
}

double valcmp(v8::Local<v8::Value>& val1, v8::Local<v8::Value>& val2)
{
    bool n1 = val1->IsNumber() || val1->IsNumberObject();
    bool n2 = val2->IsNumber() || val2->IsNumberObject();
    double v1;

    Isolate* isolate = Isolate::current();

    if (n1 && n2)
        return isolate->toNumber(val1) - isolate->toNumber(val2);

    if (n1) {
        v1 = isolate->toNumber(val2);
        if (!std::isnan(v1))
            return isolate->toNumber(val1) - v1;
    } else if (n2) {
        v1 = isolate->toNumber(val1);
        if (!std::isnan(v1))
            return v1 - isolate->toNumber(val2);
    }

    v8::String::Utf8Value s1(isolate->m_isolate, val1);
    v8::String::Utf8Value s2(isolate->m_isolate, val2);

    if (!*s1 || !*s2)
        return NAN;

    return qstrcmp(*s1, *s2);
}

result_t assert_base::lessThan(v8::Local<v8::Value> actual,
    v8::Local<v8::Value> expected, exlib::string msg)
{
    double r = valcmp(actual, expected);

    if (std::isnan(r))
        return CHECK_ERROR(CALL_E_INVALIDARG);

    return _test(r < 0, _msg(msg, "expected ", actual, " to be below ", expected));
}

result_t assert_base::notLessThan(v8::Local<v8::Value> actual,
    v8::Local<v8::Value> expected, exlib::string msg)
{
    double r = valcmp(actual, expected);

    if (std::isnan(r))
        return CHECK_ERROR(CALL_E_INVALIDARG);

    return _test(r >= 0, _msg(msg, "expected ", actual, " to be at least ", expected));
}

result_t assert_base::greaterThan(v8::Local<v8::Value> actual,
    v8::Local<v8::Value> expected, exlib::string msg)
{
    double r = valcmp(actual, expected);

    if (std::isnan(r))
        return CHECK_ERROR(CALL_E_INVALIDARG);

    return _test(r > 0, _msg(msg, "expected ", actual, " to be above ", expected));
}

result_t assert_base::notGreaterThan(v8::Local<v8::Value> actual,
    v8::Local<v8::Value> expected, exlib::string msg)
{
    double r = valcmp(actual, expected);

    if (std::isnan(r))
        return CHECK_ERROR(CALL_E_INVALIDARG);

    return _test(r <= 0, _msg(msg, "expected ", actual, " to be at most ", expected));
}

result_t assert_base::exist(v8::Local<v8::Value> actual, exlib::string msg)
{
    return _test(!actual.IsEmpty() && !actual->IsNull() && !actual->IsUndefined(),
        _msg(msg, "expected ", actual, " to exist"));
}

result_t assert_base::notExist(v8::Local<v8::Value> actual, exlib::string msg)
{
    return _test(actual.IsEmpty() || actual->IsNull() || actual->IsUndefined(),
        _msg(msg, "expected ", actual, " to not exist"));
}

result_t assert_base::isTrue(v8::Local<v8::Value> actual, exlib::string msg)
{
    return _test(actual->IsTrue(),
        _msg(msg, "expected ", actual, " to be true"));
}

result_t assert_base::isNotTrue(v8::Local<v8::Value> actual, exlib::string msg)
{
    return _test(!actual->IsTrue(),
        _msg(msg, "expected ", actual, " to not be true"));
}

result_t assert_base::isFalse(v8::Local<v8::Value> actual, exlib::string msg)
{
    return _test(actual->IsFalse(),
        _msg(msg, "expected ", actual, " to be false"));
}

result_t assert_base::isNotFalse(v8::Local<v8::Value> actual, exlib::string msg)
{
    return _test(!actual->IsFalse(),
        _msg(msg, "expected ", actual, " to not be false"));
}

result_t assert_base::isNull(v8::Local<v8::Value> actual, exlib::string msg)
{
    return _test(actual->IsNull(),
        _msg(msg, "expected ", actual, " to be null"));
}

result_t assert_base::isNotNull(v8::Local<v8::Value> actual, exlib::string msg)
{
    return _test(!actual->IsNull(),
        _msg(msg, "expected ", actual, " to not be null"));
}

result_t assert_base::isUndefined(v8::Local<v8::Value> actual, exlib::string msg)
{
    return _test(actual->IsUndefined(),
        _msg(msg, "expected ", actual, " to be undefined"));
}

result_t assert_base::isDefined(v8::Local<v8::Value> actual, exlib::string msg)
{
    return _test(!actual->IsUndefined(),
        _msg(msg, "expected ", actual, " to be defined"));
}

result_t assert_base::isFunction(v8::Local<v8::Value> actual, exlib::string msg)
{
    return _test(actual->IsFunction(),
        _msg(msg, "expected ", actual, " to be a function"));
}

result_t assert_base::isNotFunction(v8::Local<v8::Value> actual, exlib::string msg)
{
    return _test(!actual->IsFunction(),
        _msg(msg, "expected ", actual, " to not be a function"));
}

result_t assert_base::isObject(v8::Local<v8::Value> actual, exlib::string msg)
{
    return _test(actual->IsObject(),
        _msg(msg, "expected ", actual, " to be an object"));
}

result_t assert_base::isNotObject(v8::Local<v8::Value> actual, exlib::string msg)
{
    return _test(!actual->IsObject(),
        _msg(msg, "expected ", actual, " to not be an object"));
}

result_t assert_base::isArray(v8::Local<v8::Value> actual, exlib::string msg)
{
    return _test(actual->IsArray(),
        _msg(msg, "expected ", actual, " to be an array"));
}

result_t assert_base::isNotArray(v8::Local<v8::Value> actual, exlib::string msg)
{
    return _test(!actual->IsArray(),
        _msg(msg, "expected ", actual, " to not be an array"));
}

result_t assert_base::isString(v8::Local<v8::Value> actual, exlib::string msg)
{
    return _test(actual->IsString() || actual->IsStringObject(),
        _msg(msg, "expected ", actual, " to be a string"));
}

result_t assert_base::isNotString(v8::Local<v8::Value> actual, exlib::string msg)
{
    return _test(!actual->IsString() || actual->IsStringObject(),
        _msg(msg, "expected ", actual, " to not be a string"));
}

result_t assert_base::isNumber(v8::Local<v8::Value> actual, exlib::string msg)
{
    return _test(actual->IsNumber(),
        _msg(msg, "expected ", actual, " to be a number"));
}

result_t assert_base::isNotNumber(v8::Local<v8::Value> actual, exlib::string msg)
{
    return _test(!actual->IsNumber(),
        _msg(msg, "expected ", actual, " to not be a number"));
}

result_t assert_base::isBoolean(v8::Local<v8::Value> actual, exlib::string msg)
{
    return _test(actual->IsBoolean(),
        _msg(msg, "expected ", actual, " to be a boolean"));
}

result_t assert_base::isNotBoolean(v8::Local<v8::Value> actual, exlib::string msg)
{
    return _test(!actual->IsBoolean(),
        _msg(msg, "expected ", actual, " to not be a boolean"));
}

result_t assert_base::typeOf(v8::Local<v8::Value> actual, exlib::string type,
    exlib::string msg)
{
    if ((type == "array"))
        return isArray(actual, msg);
    if ((type == "function"))
        return isFunction(actual, msg);
    if ((type == "string"))
        return isString(actual, msg);
    if ((type == "object"))
        return isObject(actual, msg);
    if ((type == "number"))
        return isNumber(actual, msg);
    if ((type == "boolean"))
        return isBoolean(actual, msg);
    if ((type == "null"))
        return isNull(actual, msg);
    if ((type == "undefined"))
        return isUndefined(actual, msg);

    return CHECK_ERROR(CALL_E_INVALIDARG);
}

result_t assert_base::notTypeOf(v8::Local<v8::Value> actual, exlib::string type,
    exlib::string msg)
{
    if ((type == "array"))
        return isNotArray(actual, msg);
    if ((type == "function"))
        return isNotFunction(actual, msg);
    if ((type == "string"))
        return isNotString(actual, msg);
    if ((type == "object"))
        return isNotObject(actual, msg);
    if ((type == "number"))
        return isNotNumber(actual, msg);
    if ((type == "boolean"))
        return isNotBoolean(actual, msg);
    if ((type == "null"))
        return isNotNull(actual, msg);
    if ((type == "undefined"))
        return isDefined(actual, msg);

    return CHECK_ERROR(CALL_E_INVALIDARG);
}

result_t has_prop(v8::Local<v8::Value> v, v8::Local<v8::Value> prop,
    bool& retVal)
{
    if ((!v->IsObject() && !v->IsString()) || !prop->IsString())
        return CHECK_ERROR(CALL_E_INVALIDARG);

    Isolate* isolate = Isolate::current();
    retVal = v.As<v8::Object>()->Has(isolate->context(), prop).FromMaybe(false);

    return 0;
}

result_t assert_base::property(v8::Local<v8::Value> object,
    v8::Local<v8::Value> prop, exlib::string msg)
{
    bool r;
    result_t hr = has_prop(object, prop, r);
    if (hr < 0)
        return hr;

    return _test(r, _msg(msg, "expected ", object, " to have a property ", prop));
}

result_t assert_base::notProperty(v8::Local<v8::Value> object,
    v8::Local<v8::Value> prop, exlib::string msg)
{
    bool r;
    result_t hr = has_prop(object, prop, r);
    if (hr < 0)
        return hr;

    return _test(!r, _msg(msg, "expected ", object, " to not have a property ", prop));
}

result_t deep_has_prop(v8::Local<v8::Value> object, v8::Local<v8::Value> prop,
    bool& retVal)
{
    if ((!object->IsObject() && !object->IsString()) || !prop->IsString())
        return CHECK_ERROR(CALL_E_INVALIDARG);

    Isolate* isolate = Isolate::current();
    v8::Local<v8::Context> context = isolate->context();
    v8::Local<v8::Object> v = object.As<v8::Object>();
    v8::String::Utf8Value s(isolate->m_isolate, prop);
    const char *p, *p1;

    p = ToCString(s);
    while ((p1 = qstrchr(p, '.')) != NULL) {
        object = JSValue(v->Get(context, isolate->NewString(p, (int32_t)(p1 - p))));

        if (object.IsEmpty() || (!object->IsObject() && !object->IsString())) {
            retVal = false;
            return 0;
        }

        v = object.As<v8::Object>();
        p = p1 + 1;
    }

    retVal = v->Has(context, isolate->NewString(p)).FromMaybe(false);

    return 0;
}

result_t assert_base::deepProperty(v8::Local<v8::Value> object,
    v8::Local<v8::Value> prop, exlib::string msg)
{
    bool r;
    result_t hr = deep_has_prop(object, prop, r);
    if (hr < 0)
        return hr;

    return _test(r, _msg(msg, "expected ", object, " to have a deep property ", prop));
}

result_t assert_base::notDeepProperty(v8::Local<v8::Value> object,
    v8::Local<v8::Value> prop, exlib::string msg)
{
    bool r;
    result_t hr = deep_has_prop(object, prop, r);
    if (hr < 0)
        return hr;

    return _test(!r,
        _msg(msg, "expected ", object, " to not have a deep property ",
            prop));
}

result_t has_val(v8::Local<v8::Value> object, v8::Local<v8::Value> prop,
    v8::Local<v8::Value> value, bool& retVal, v8::Local<v8::Value>& got)
{
    if ((!object->IsObject() && !object->IsString()) || !prop->IsString())
        return CHECK_ERROR(CALL_E_INVALIDARG);

    Isolate* isolate = Isolate::current();
    v8::Local<v8::Context> context = isolate->context();

    got = JSValue(object.As<v8::Object>()->Get(context, prop));
    if (got.IsEmpty())
        return CALL_E_JAVASCRIPT;

    retVal = value->StrictEquals(got);

    return 0;
}

result_t assert_base::propertyVal(v8::Local<v8::Value> object,
    v8::Local<v8::Value> prop, v8::Local<v8::Value> value,
    exlib::string msg)
{
    bool r;
    v8::Local<v8::Value> got;

    result_t hr = has_val(object, prop, value, r, got);
    if (hr < 0)
        return hr;

    return _test(r,
        _msg(msg, "expected ", object, " to have a property ", prop, " of ",
            value, ", but got ", got));
}

result_t assert_base::propertyNotVal(v8::Local<v8::Value> object,
    v8::Local<v8::Value> prop, v8::Local<v8::Value> value,
    exlib::string msg)
{
    bool r;
    v8::Local<v8::Value> got;

    result_t hr = has_val(object, prop, value, r, got);
    if (hr < 0)
        return hr;

    return _test(!r,
        _msg(msg, "expected ", object, " not to have a property ", prop,
            " of ", value));
}

result_t deep_has_val(v8::Local<v8::Value> object, v8::Local<v8::Value> prop,
    v8::Local<v8::Value> value, bool& retVal, v8::Local<v8::Value>& got)
{
    if ((!object->IsObject() && !object->IsString()) || !prop->IsString())
        return CHECK_ERROR(CALL_E_INVALIDARG);

    Isolate* isolate = Isolate::current();
    v8::Local<v8::Context> context = isolate->context();
    v8::Local<v8::Object> v = object.As<v8::Object>();
    v8::String::Utf8Value s(isolate->m_isolate, prop);
    const char *p, *p1;

    p = ToCString(s);
    while ((p1 = qstrchr(p, '.')) != NULL) {
        object = JSValue(v->Get(context, isolate->NewString(p, (int32_t)(p1 - p))));

        if (object.IsEmpty() || (!object->IsObject() && !object->IsString())) {
            retVal = false;
            return 0;
        }

        v = object.As<v8::Object>();
        p = p1 + 1;
    }

    got = JSValue(v->Get(context, isolate->NewString(p)));
    if (got.IsEmpty())
        return CALL_E_JAVASCRIPT;
    retVal = value->StrictEquals(got);

    return 0;
}

result_t assert_base::deepPropertyVal(v8::Local<v8::Value> object,
    v8::Local<v8::Value> prop, v8::Local<v8::Value> value,
    exlib::string msg)
{
    bool r;
    v8::Local<v8::Value> got;

    result_t hr = deep_has_val(object, prop, value, r, got);
    if (hr < 0)
        return hr;

    return _test(r,
        _msg(msg, "expected ", object, " to have a deep property ", prop,
            " of ", value, ", but got ", got));
}

result_t assert_base::deepPropertyNotVal(v8::Local<v8::Value> object,
    v8::Local<v8::Value> prop, v8::Local<v8::Value> value,
    exlib::string msg)
{
    bool r;
    v8::Local<v8::Value> got;

    result_t hr = deep_has_val(object, prop, value, r, got);
    if (hr < 0)
        return hr;

    return _test(!r,
        _msg(msg, "expected ", object, " not to have a deep property ",
            prop, " of ", value));
}

result_t assert_base::ifError(v8::Local<v8::Value> v)
{
    if (Isolate::current()->toBoolean(v)) {
        ThrowError(v);
    }

    return 0;
}
}
