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

namespace fibjs {

DECLARE_MODULE(assert);
class _msg {
public:
    _msg(exlib::string s0, const char* s1)
    {
        if (!s0.empty()) {
            msg = s0;
        } else {
            strs[0] = s1;
            strs[1] = NULL;
        }
    }

    _msg(exlib::string s0, const char* s1, v8::Local<v8::Value>& v1,
        const char* s2)
    {
        if (!s0.empty()) {
            msg = s0;
        } else {
            strs[0] = s1;
            strs[1] = s2;
            strs[2] = NULL;

            vs[0] = &v1;
        }
    }

    _msg(exlib::string s0, const char* s1, v8::Local<v8::Value>& v1,
        const char* s2, v8::Local<v8::Value>& v2, const char* s3 = "")
    {
        if (!s0.empty()) {
            msg = s0;
        } else {
            strs[0] = s1;
            strs[1] = s2;
            strs[2] = s3;
            strs[3] = NULL;

            vs[0] = &v1;
            vs[1] = &v2;
        }
    }

    _msg(exlib::string s0, const char* s1, v8::Local<v8::Value>& v1,
        const char* s2, v8::Local<v8::Value>& v2, const char* s3,
        v8::Local<v8::Value>& v3, const char* s4 = "")
    {
        if (!s0.empty()) {
            msg = s0;
        } else {
            strs[0] = s1;
            strs[1] = s2;
            strs[2] = s3;
            strs[3] = s4;
            strs[4] = NULL;

            vs[0] = &v1;
            vs[1] = &v2;
            vs[2] = &v3;
        }
    }

    _msg(exlib::string s0, const char* s1, v8::Local<v8::Value>& v1,
        const char* s2, v8::Local<v8::Value>& v2, const char* s3,
        v8::Local<v8::Value>& v3, const char* s4,
        v8::Local<v8::Value>& v4, const char* s5 = "")
    {
        if (!s0.empty()) {
            msg = s0;
        } else {
            strs[0] = s1;
            strs[1] = s2;
            strs[2] = s3;
            strs[3] = s4;
            strs[4] = s5;

            vs[0] = &v1;
            vs[1] = &v2;
            vs[2] = &v3;
            vs[3] = &v4;
        }
    }

    exlib::string str()
    {
        exlib::string str(msg);

        if (str.empty()) {
            str = strs[0];

            if (strs[1]) {
                str.append(json_format(*vs[0], false, DEFAULT_DEPTH, DEFAULT_MAX_ARRAY_LENGTH, DEFAULT_MAX_STRING_LENGTH));
                str.append(strs[1]);

                if (strs[2]) {
                    str.append(json_format(*vs[1], false, DEFAULT_DEPTH, DEFAULT_MAX_ARRAY_LENGTH, DEFAULT_MAX_STRING_LENGTH));
                    str.append(strs[2]);

                    if (strs[3]) {
                        str.append(json_format(*vs[2], false, DEFAULT_DEPTH, DEFAULT_MAX_ARRAY_LENGTH, DEFAULT_MAX_STRING_LENGTH));
                        str.append(strs[3]);

                        if (strs[4]) {
                            str.append(json_format(*vs[3], false, DEFAULT_DEPTH, DEFAULT_MAX_ARRAY_LENGTH, DEFAULT_MAX_STRING_LENGTH));
                            str.append(strs[4]);
                        }
                    }
                }
            }
        }

        return str;
    }

private:
    exlib::string msg;
    const char* strs[5];
    v8::Local<v8::Value>* vs[4];
};

inline void _test(bool value, _msg msg)
{
    if (!value)
        ThrowError(msg.str());
}

result_t assert_base::_function(v8::Local<v8::Value> actual, exlib::string msg)
{
    return ok(actual, msg);
}

result_t assert_base::ok(v8::Local<v8::Value> actual, exlib::string msg)
{
    _test(Isolate::current()->toBoolean(actual),
        _msg(msg, "expected ", actual, " to be truthy"));
    return 0;
}

result_t assert_base::notOk(v8::Local<v8::Value> actual, exlib::string msg)
{
    _test(!Isolate::current()->toBoolean(actual),
        _msg(msg, "expected ", actual, " to be falsy"));
    return 0;
}

result_t assert_base::equal(v8::Local<v8::Value> actual,
    v8::Local<v8::Value> expected, exlib::string msg)
{
    bool tst = actual->Equals(Isolate::current()->context(), expected).FromMaybe(false);
    _test(tst, _msg(msg, "expected ", actual, " to equal ", expected));
    return 0;
}

result_t assert_base::notEqual(v8::Local<v8::Value> actual,
    v8::Local<v8::Value> expected, exlib::string msg)
{
    bool tst = !actual->Equals(Isolate::current()->context(), expected).FromMaybe(false);
    _test(tst, _msg(msg, "expected ", actual, " to not equal ", expected));
    return 0;
}

result_t assert_base::strictEqual(v8::Local<v8::Value> actual,
    v8::Local<v8::Value> expected, exlib::string msg)
{
    _test(actual->StrictEquals(expected),
        _msg(msg, "expected ", actual, " to strictly equal ", expected));
    return 0;
}

result_t assert_base::notStrictEqual(v8::Local<v8::Value> actual,
    v8::Local<v8::Value> expected, exlib::string msg)
{
    _test(!actual->StrictEquals(expected),
        _msg(msg, "expected ", actual, " to not strictly equal ",
            expected));
    return 0;
}

result_t assert_base::deepEqual(v8::Local<v8::Value> actual,
    v8::Local<v8::Value> expected, exlib::string msg)
{
    bool r;

    util_base::isDeepEqual(actual, expected, r);
    _test(r, _msg(msg, "expected ", actual, " to deeply equal ", expected));
    return 0;
}

result_t assert_base::notDeepEqual(v8::Local<v8::Value> actual,
    v8::Local<v8::Value> expected, exlib::string msg)
{
    bool r;

    util_base::isDeepEqual(actual, expected, r);
    _test(!r, _msg(msg, "expected ", actual, " to not deeply equal ", expected));
    return 0;
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
    _test(n <= n1,
        _msg(msg, "expected ", actual, " to be close to ", expected,
            " +/- ", delta));
    return 0;
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
    _test(n > n1,
        _msg(msg, "expected ", actual, " not to be close to ", expected,
            " +/- ", delta));
    return 0;
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

    _test(r < 0, _msg(msg, "expected ", actual, " to be below ", expected));
    return 0;
}

result_t assert_base::notLessThan(v8::Local<v8::Value> actual,
    v8::Local<v8::Value> expected, exlib::string msg)
{
    double r = valcmp(actual, expected);

    if (std::isnan(r))
        return CHECK_ERROR(CALL_E_INVALIDARG);

    _test(r >= 0, _msg(msg, "expected ", actual, " to be at least ", expected));
    return 0;
}

result_t assert_base::greaterThan(v8::Local<v8::Value> actual,
    v8::Local<v8::Value> expected, exlib::string msg)
{
    double r = valcmp(actual, expected);

    if (std::isnan(r))
        return CHECK_ERROR(CALL_E_INVALIDARG);

    _test(r > 0, _msg(msg, "expected ", actual, " to be above ", expected));
    return 0;
}

result_t assert_base::notGreaterThan(v8::Local<v8::Value> actual,
    v8::Local<v8::Value> expected, exlib::string msg)
{
    double r = valcmp(actual, expected);

    if (std::isnan(r))
        return CHECK_ERROR(CALL_E_INVALIDARG);

    _test(r <= 0, _msg(msg, "expected ", actual, " to be at most ", expected));
    return 0;
}

result_t assert_base::exist(v8::Local<v8::Value> actual, exlib::string msg)
{
    _test(!actual->IsNull() && !actual->IsUndefined(),
        _msg(msg, "expected ", actual, " to be true"));
    return 0;
}

result_t assert_base::notExist(v8::Local<v8::Value> actual, exlib::string msg)
{
    _test(actual->IsNull() || actual->IsUndefined(),
        _msg(msg, "expected ", actual, " not to be true"));
    return 0;
}

result_t assert_base::isTrue(v8::Local<v8::Value> actual, exlib::string msg)
{
    _test(actual->IsTrue(), _msg(msg, "expected ", actual, " to be true"));
    return 0;
}

result_t assert_base::isNotTrue(v8::Local<v8::Value> actual, exlib::string msg)
{
    _test(!actual->IsTrue(), _msg(msg, "expected ", actual, " not to be true"));
    return 0;
}

result_t assert_base::isFalse(v8::Local<v8::Value> actual, exlib::string msg)
{
    _test(actual->IsFalse(), _msg(msg, "expected ", actual, " to be false"));
    return 0;
}

result_t assert_base::isNotFalse(v8::Local<v8::Value> actual, exlib::string msg)
{
    _test(!actual->IsFalse(),
        _msg(msg, "expected ", actual, " not to be false"));
    return 0;
}

result_t assert_base::isNull(v8::Local<v8::Value> actual, exlib::string msg)
{
    _test(actual->IsNull(), _msg(msg, "expected ", actual, " to be null"));
    return 0;
}

result_t assert_base::isNotNull(v8::Local<v8::Value> actual, exlib::string msg)
{
    _test(!actual->IsNull(), _msg(msg, "expected ", actual, " not to be null"));
    return 0;
}

result_t assert_base::isUndefined(v8::Local<v8::Value> actual, exlib::string msg)
{
    _test(actual->IsUndefined(),
        _msg(msg, "expected ", actual, " to be undefined"));
    return 0;
}

result_t assert_base::isDefined(v8::Local<v8::Value> actual, exlib::string msg)
{
    _test(!actual->IsUndefined(),
        _msg(msg, "expected ", actual, " not to be undefined"));
    return 0;
}

result_t assert_base::isFunction(v8::Local<v8::Value> actual, exlib::string msg)
{
    _test(actual->IsFunction(),
        _msg(msg, "expected ", actual, " to be function"));
    return 0;
}

result_t assert_base::isNotFunction(v8::Local<v8::Value> actual,
    exlib::string msg)
{
    _test(!actual->IsFunction(),
        _msg(msg, "expected ", actual, " not to be function"));
    return 0;
}

result_t assert_base::isObject(v8::Local<v8::Value> actual, exlib::string msg)
{
    _test(actual->IsObject(), _msg(msg, "expected ", actual, " to be object"));
    return 0;
}

result_t assert_base::isNotObject(v8::Local<v8::Value> actual, exlib::string msg)
{
    _test(!actual->IsObject(),
        _msg(msg, "expected ", actual, " not to be object"));
    return 0;
}

result_t assert_base::isArray(v8::Local<v8::Value> actual, exlib::string msg)
{
    _test(actual->IsArray(), _msg(msg, "expected ", actual, " to be array"));
    return 0;
}

result_t assert_base::isNotArray(v8::Local<v8::Value> actual, exlib::string msg)
{
    _test(!actual->IsArray(),
        _msg(msg, "expected ", actual, " not to be array"));
    return 0;
}

result_t assert_base::isString(v8::Local<v8::Value> actual, exlib::string msg)
{
    _test(actual->IsString() || actual->IsStringObject(),
        _msg(msg, "expected ", actual, " to be string"));
    return 0;
}

result_t assert_base::isNotString(v8::Local<v8::Value> actual, exlib::string msg)
{
    _test(!actual->IsString() && !actual->IsStringObject(),
        _msg(msg, "expected ", actual, " not to be string"));
    return 0;
}

result_t assert_base::isNumber(v8::Local<v8::Value> actual, exlib::string msg)
{
    _test(actual->IsNumber() || actual->IsNumberObject(),
        _msg(msg, "expected ", actual, " to be number"));
    return 0;
}

result_t assert_base::isNotNumber(v8::Local<v8::Value> actual, exlib::string msg)
{
    _test(!actual->IsNumber() && !actual->IsNumberObject(),
        _msg(msg, "expected ", actual, " not to be number"));
    return 0;
}

result_t assert_base::isBoolean(v8::Local<v8::Value> actual, exlib::string msg)
{
    _test(actual->IsBoolean() || actual->IsBooleanObject(),
        _msg(msg, "expected ", actual, " to be boolean"));
    return 0;
}

result_t assert_base::isNotBoolean(v8::Local<v8::Value> actual,
    exlib::string msg)
{
    _test(!actual->IsBoolean() && !actual->IsBooleanObject(),
        _msg(msg, "expected ", actual, " not to be boolean"));
    return 0;
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
    retVal = v8::Local<v8::Object>::Cast(v)->Has(isolate->context(), prop).FromMaybe(false);

    return 0;
}

result_t assert_base::property(v8::Local<v8::Value> object,
    v8::Local<v8::Value> prop, exlib::string msg)
{
    bool r;
    result_t hr = has_prop(object, prop, r);
    if (hr < 0)
        return hr;

    _test(r, _msg(msg, "expected ", object, " to have a property ", prop));
    return 0;
}

result_t assert_base::notProperty(v8::Local<v8::Value> object,
    v8::Local<v8::Value> prop, exlib::string msg)
{
    bool r;
    result_t hr = has_prop(object, prop, r);
    if (hr < 0)
        return hr;

    _test(!r, _msg(msg, "expected ", object, " to not have a property ", prop));
    return 0;
}

result_t deep_has_prop(v8::Local<v8::Value> object, v8::Local<v8::Value> prop,
    bool& retVal)
{
    if ((!object->IsObject() && !object->IsString()) || !prop->IsString())
        return CHECK_ERROR(CALL_E_INVALIDARG);

    Isolate* isolate = Isolate::current();
    v8::Local<v8::Context> context = isolate->context();
    v8::Local<v8::Object> v = v8::Local<v8::Object>::Cast(object);
    v8::String::Utf8Value s(isolate->m_isolate, prop);
    const char *p, *p1;

    p = ToCString(s);
    while ((p1 = qstrchr(p, '.')) != NULL) {
        object = JSValue(v->Get(context, isolate->NewString(p, (int32_t)(p1 - p))));

        if (object.IsEmpty() || (!object->IsObject() && !object->IsString())) {
            retVal = false;
            return 0;
        }

        v = v8::Local<v8::Object>::Cast(object);
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

    _test(r, _msg(msg, "expected ", object, " to have a deep property ", prop));
    return 0;
}

result_t assert_base::notDeepProperty(v8::Local<v8::Value> object,
    v8::Local<v8::Value> prop, exlib::string msg)
{
    bool r;
    result_t hr = deep_has_prop(object, prop, r);
    if (hr < 0)
        return hr;

    _test(!r,
        _msg(msg, "expected ", object, " to not have a deep property ",
            prop));
    return 0;
}

result_t has_val(v8::Local<v8::Value> object, v8::Local<v8::Value> prop,
    v8::Local<v8::Value> value, bool& retVal, v8::Local<v8::Value>& got)
{
    if ((!object->IsObject() && !object->IsString()) || !prop->IsString())
        return CHECK_ERROR(CALL_E_INVALIDARG);

    Isolate* isolate = Isolate::current();
    v8::Local<v8::Context> context = isolate->context();

    got = JSValue(v8::Local<v8::Object>::Cast(object)->Get(context, prop));
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

    _test(r,
        _msg(msg, "expected ", object, " to have a property ", prop, " of ",
            value, ", but got ", got));
    return 0;
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

    _test(!r,
        _msg(msg, "expected ", object, " not to have a property ", prop,
            " of ", value));
    return 0;
}

result_t deep_has_val(v8::Local<v8::Value> object, v8::Local<v8::Value> prop,
    v8::Local<v8::Value> value, bool& retVal, v8::Local<v8::Value>& got)
{
    if ((!object->IsObject() && !object->IsString()) || !prop->IsString())
        return CHECK_ERROR(CALL_E_INVALIDARG);

    Isolate* isolate = Isolate::current();
    v8::Local<v8::Context> context = isolate->context();
    v8::Local<v8::Object> v = v8::Local<v8::Object>::Cast(object);
    v8::String::Utf8Value s(isolate->m_isolate, prop);
    const char *p, *p1;

    p = ToCString(s);
    while ((p1 = qstrchr(p, '.')) != NULL) {
        object = JSValue(v->Get(context, isolate->NewString(p, (int32_t)(p1 - p))));

        if (object.IsEmpty() || (!object->IsObject() && !object->IsString())) {
            retVal = false;
            return 0;
        }

        v = v8::Local<v8::Object>::Cast(object);
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

    _test(r,
        _msg(msg, "expected ", object, " to have a deep property ", prop,
            " of ", value, ", but got ", got));
    return 0;
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

    _test(!r,
        _msg(msg, "expected ", object, " not to have a deep property ",
            prop, " of ", value));
    return 0;
}

result_t assert_base::throws(v8::Local<v8::Function> block, exlib::string msg)
{
    bool err;
    {
        TryCatch try_catch;
        block->Call(block->GetCreationContextChecked(), v8::Undefined(Isolate::current()->m_isolate), 0, NULL).IsEmpty();
        err = try_catch.HasCaught();
    }
    _test(err, _msg(msg, "Missing expected exception."));

    return 0;
}

result_t assert_base::doesNotThrow(v8::Local<v8::Function> block,
    exlib::string msg)
{
    bool err;
    {
        TryCatch try_catch;
        block->Call(block->GetCreationContextChecked(), v8::Undefined(Isolate::current()->m_isolate), 0, NULL).IsEmpty();
        err = try_catch.HasCaught();
    }
    _test(!err, _msg(msg, "Got unwanted exception."));

    return 0;
}

result_t assert_base::ifError(v8::Local<v8::Value> v)
{
    if (Isolate::current()->toBoolean(v)) {
        ThrowError(v);
    }

    return 0;
}
}
