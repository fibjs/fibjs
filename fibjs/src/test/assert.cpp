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
#include "QuickArray.h"

namespace fibjs {

DECLARE_MODULE(assert);

exlib::string json_format(v8::Local<v8::Value> obj);

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
                str.append(json_format(*vs[0]));
                str.append(strs[1]);

                if (strs[2]) {
                    str.append(json_format(*vs[1]));
                    str.append(strs[2]);

                    if (strs[3]) {
                        str.append(json_format(*vs[2]));
                        str.append(strs[3]);

                        if (strs[4]) {
                            str.append(json_format(*vs[3]));
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
    _test(actual->BooleanValue(),
        _msg(msg, "expected ", actual, " to be truthy"));
    return 0;
}

result_t assert_base::notOk(v8::Local<v8::Value> actual, exlib::string msg)
{
    _test(!actual->BooleanValue(),
        _msg(msg, "expected ", actual, " to be falsy"));
    return 0;
}

bool regexpEquals(v8::Local<v8::Value> actual, v8::Local<v8::Value> expected)
{
    v8::Local<v8::RegExp> re1 = v8::Local<v8::RegExp>::Cast(actual);
    v8::Local<v8::String> src1 = re1->GetSource();
    v8::RegExp::Flags flgs1 = re1->GetFlags();

    v8::Local<v8::RegExp> re2 = v8::Local<v8::RegExp>::Cast(expected);
    v8::Local<v8::String> src2 = re2->GetSource();
    v8::RegExp::Flags flgs2 = re2->GetFlags();

    return src1->StrictEquals(src2) && flgs1 == flgs2;
}

bool deepEquals(QuickArray<v8::Local<v8::Object>>& acts,
    QuickArray<v8::Local<v8::Object>>& exps,
    v8::Local<v8::Value> actual, v8::Local<v8::Value> expected);

int32_t checkStack(QuickArray<v8::Local<v8::Object>>& acts,
    QuickArray<v8::Local<v8::Object>>& exps,
    v8::Local<v8::Object> actual, v8::Local<v8::Object> expected)
{
    int32_t i;

    for (i = 0; i < (int32_t)acts.size(); i++)
        if (actual->Equals(acts[i])) {
            if (expected->Equals(exps[i]))
                return 0;
            return -1;
        }

    acts.append(actual);
    exps.append(expected);

    return 1;
}

bool arrayEquals(QuickArray<v8::Local<v8::Object>>& acts,
    QuickArray<v8::Local<v8::Object>>& exps,
    v8::Local<v8::Value> actual, v8::Local<v8::Value> expected)
{
    v8::Local<v8::Array> act = v8::Local<v8::Array>::Cast(actual);
    v8::Local<v8::Array> exp = v8::Local<v8::Array>::Cast(expected);
    int32_t len = (int32_t)act->Length();
    int32_t i;

    i = checkStack(acts, exps, act, exp);
    if (i == 0)
        return true;
    if (i == -1)
        return false;

    if (len != (int32_t)exp->Length()) {
        acts.pop();
        exps.pop();
        return false;
    }

    for (i = 0; i < len; i++)
        if (!deepEquals(acts, exps, act->Get(i), exp->Get(i))) {
            acts.pop();
            exps.pop();
            return false;
        }

    acts.pop();
    exps.pop();
    return true;
}

bool objectEquals(QuickArray<v8::Local<v8::Object>>& acts,
    QuickArray<v8::Local<v8::Object>>& exps,
    v8::Local<v8::Value> actual, v8::Local<v8::Value> expected)
{
    v8::Local<v8::Object> act = v8::Local<v8::Object>::Cast(actual);
    v8::Local<v8::Object> exp = v8::Local<v8::Object>::Cast(expected);

    int32_t i;

    i = checkStack(acts, exps, act, exp);
    if (i == 0)
        return true;
    if (i == -1)
        return false;

    v8::Local<v8::Array> keys = act->GetPropertyNames();
    int32_t len = (int32_t)keys->Length();

    if (len != (int32_t)exp->GetPropertyNames()->Length()) {
        acts.pop();
        exps.pop();
        return false;
    }

    for (i = 0; i < len; i++) {
        v8::Local<v8::Value> ks = keys->Get(i);
        if (!deepEquals(acts, exps, act->Get(ks), exp->Get(ks))) {
            acts.pop();
            exps.pop();
            return false;
        }
    }

    acts.pop();
    exps.pop();
    return true;
}

bool deepEquals(QuickArray<v8::Local<v8::Object>>& acts,
    QuickArray<v8::Local<v8::Object>>& exps,
    v8::Local<v8::Value> actual, v8::Local<v8::Value> expected)
{
    if (!IsEmpty(actual) && !IsEmpty(expected) && !actual->IsFunction()
        && !expected->IsFunction()) {
        if (actual->IsDate())
            return expected->IsDate()
                && (actual->NumberValue() == expected->NumberValue());

        if (expected->IsDate())
            return false;

        if (actual->IsRegExp())
            return expected->IsRegExp() && regexpEquals(actual, expected);

        if (expected->IsRegExp())
            return false;

        if (actual->IsArray() && expected->IsArray())
            return arrayEquals(acts, exps, actual, expected);

        obj_ptr<object_base> obj1 = object_base::getInstance(actual);
        obj_ptr<object_base> obj2 = object_base::getInstance(expected);
        if (obj1 && obj2) {
            bool v;
            obj1->equals(obj2, v);
            return v;
        }

        if (actual->IsObject() && expected->IsObject())
            return objectEquals(acts, exps, actual, expected);
    }

    return actual->StrictEquals(expected);
}

result_t assert_base::equal(v8::Local<v8::Value> actual,
    v8::Local<v8::Value> expected, exlib::string msg)
{
    _test(actual->Equals(expected),
        _msg(msg, "expected ", actual, " to equal ", expected));
    return 0;
}

result_t assert_base::notEqual(v8::Local<v8::Value> actual,
    v8::Local<v8::Value> expected, exlib::string msg)
{
    _test(!actual->Equals(expected),
        _msg(msg, "expected ", actual, " to not equal ", expected));
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
    QuickArray<v8::Local<v8::Object>> acts;
    QuickArray<v8::Local<v8::Object>> exps;

    _test(deepEquals(acts, exps, actual, expected),
        _msg(msg, "expected ", actual, " to deeply equal ", expected));
    return 0;
}

result_t assert_base::notDeepEqual(v8::Local<v8::Value> actual,
    v8::Local<v8::Value> expected, exlib::string msg)
{
    QuickArray<v8::Local<v8::Object>> acts;
    QuickArray<v8::Local<v8::Object>> exps;

    _test(!deepEquals(acts, exps, actual, expected),
        _msg(msg, "expected ", actual, " to not deeply equal ", expected));
    return 0;
}

result_t assert_base::closeTo(v8::Local<v8::Value> actual,
    v8::Local<v8::Value> expected, v8::Local<v8::Value> delta,
    exlib::string msg)
{
    double n, n1;

    n = actual->NumberValue();
    if (std::isnan(n))
        return CHECK_ERROR(CALL_E_INVALIDARG);

    n1 = expected->NumberValue();
    if (std::isnan(n1))
        return CHECK_ERROR(CALL_E_INVALIDARG);

    n -= n1;

    n1 = delta->NumberValue();
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

    n = actual->NumberValue();
    if (std::isnan(n))
        return CHECK_ERROR(CALL_E_INVALIDARG);

    n1 = expected->NumberValue();
    if (std::isnan(n1))
        return CHECK_ERROR(CALL_E_INVALIDARG);

    n -= n1;

    n1 = delta->NumberValue();
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

    if (n1 && n2)
        return val1->NumberValue() - val2->NumberValue();

    if (n1) {
        v1 = val2->NumberValue();
        if (!std::isnan(v1))
            return val1->NumberValue() - v1;
    } else if (n2) {
        v1 = val1->NumberValue();
        if (!std::isnan(v1))
            return v1 - val2->NumberValue();
    }

    v8::String::Utf8Value s1(val1);
    v8::String::Utf8Value s2(val2);

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

result_t has_prop(v8::Local<v8::Value> object, v8::Local<v8::Value> prop,
    bool& retVal)
{
    if ((!object->IsObject() && !object->IsString()) || !prop->IsString())
        return CHECK_ERROR(CALL_E_INVALIDARG);

    v8::Local<v8::Object> v = object->ToObject();
    retVal = v->Has(prop);

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
    v8::Local<v8::Object> v = object->ToObject();
    v8::String::Utf8Value s(prop);
    const char *p, *p1;

    p = *s;
    while ((p1 = qstrchr(p, '.')) != NULL) {
        object = v->Get(isolate->NewString(p, (int32_t)(p1 - p)));

        if (object.IsEmpty() || (!object->IsObject() && !object->IsString())) {
            retVal = false;
            return 0;
        }

        v = object->ToObject();
        p = p1 + 1;
    }

    retVal = v->Has(isolate->NewString(p));

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

    v8::Local<v8::Object> v = object->ToObject();
    got = v->Get(prop);
    retVal = value->Equals(got);

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
    v8::Local<v8::Object> v = object->ToObject();
    v8::String::Utf8Value s(prop);
    const char *p, *p1;

    p = *s;
    while ((p1 = qstrchr(p, '.')) != NULL) {
        object = v->Get(isolate->NewString(p, (int32_t)(p1 - p)));

        if (object.IsEmpty() || (!object->IsObject() && !object->IsString())) {
            retVal = false;
            return 0;
        }

        v = object->ToObject();
        p = p1 + 1;
    }

    got = v->Get(isolate->NewString(p));
    retVal = value->Equals(got);

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
        block->Call(v8::Undefined(Isolate::current()->m_isolate), 0, NULL);
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
        block->Call(v8::Undefined(Isolate::current()->m_isolate), 0, NULL);
        err = try_catch.HasCaught();
    }
    _test(!err, _msg(msg, "Got unwanted exception."));

    return 0;
}

result_t assert_base::ifError(v8::Local<v8::Value> object)
{
    if (object->BooleanValue()) {
        ThrowError(object);
    }

    return 0;
}
}
