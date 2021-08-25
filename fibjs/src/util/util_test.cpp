/*
 * util_test.cpp
 *
 *  Created on: Jun 26, 2017
 *      Author: lion
 */

#include "object.h"
#include "ifs/util.h"

namespace fibjs {

result_t util_base::isEmpty(v8::Local<v8::Value> v, bool& retVal)
{
    if (v->IsNull()) {
        retVal = true;
        return 0;
    }

    if (v->IsString()) {
        retVal = v8::Local<v8::String>::Cast(v)->Length() == 0;
        return 0;
    }

    if (v->IsStringObject()) {
        retVal = v8::Local<v8::StringObject>::Cast(v)->ValueOf()->Length() == 0;
        return 0;
    }

    if (v->IsArray()) {
        retVal = v8::Local<v8::Array>::Cast(v)->Length() == 0;
        return 0;
    }

    if (v->IsObject()) {
        v8::Local<v8::Object> o = v8::Local<v8::Object>::Cast(v);
        retVal = o->GetOwnPropertyNames(o->CreationContext()).ToLocalChecked()->Length() == 0;
        return 0;
    }

    retVal = true;

    return 0;
}

result_t util_base::isArray(v8::Local<v8::Value> v, bool& retVal)
{
    retVal = v->IsArray();
    return 0;
}

result_t util_base::isBoolean(v8::Local<v8::Value> v, bool& retVal)
{
    retVal = v->IsBoolean() || v->IsBooleanObject();
    return 0;
}

result_t util_base::isNull(v8::Local<v8::Value> v, bool& retVal)
{
    retVal = v->IsNull();
    return 0;
}

result_t util_base::isNullOrUndefined(v8::Local<v8::Value> v, bool& retVal)
{
    retVal = v->IsNull() || v->IsUndefined();
    return 0;
}

result_t util_base::isNumber(v8::Local<v8::Value> v, bool& retVal)
{
    retVal = v->IsNumber() || v->IsNumberObject();
    return 0;
}

result_t util_base::isBigInt(v8::Local<v8::Value> v, bool& retVal)
{
    retVal = v->IsBigInt() || v->IsBigIntObject();
    return 0;
}

result_t util_base::isString(v8::Local<v8::Value> v, bool& retVal)
{
    retVal = v->IsString() || v->IsStringObject();
    return 0;
}

result_t util_base::isUndefined(v8::Local<v8::Value> v, bool& retVal)
{
    retVal = v->IsUndefined();
    return 0;
}

result_t util_base::isRegExp(v8::Local<v8::Value> v, bool& retVal)
{
    retVal = v->IsRegExp();
    return 0;
}

result_t util_base::isObject(v8::Local<v8::Value> v, bool& retVal)
{
    retVal = v->IsObject();
    return 0;
}

result_t util_base::isDate(v8::Local<v8::Value> v, bool& retVal)
{
    retVal = v->IsDate();
    return 0;
}

result_t util_base::isNativeError(v8::Local<v8::Value> v, bool& retVal)
{
    retVal = v->IsNativeError();
    return 0;
}

result_t util_base::isPrimitive(v8::Local<v8::Value> v, bool& retVal)
{
    retVal = false;
    if (v->IsNullOrUndefined()) {
        retVal = true;
    } else {
        result_t hr;
        Isolate* isolate = Isolate::current();
        exlib::string type;
        hr = GetArgumentValue(v->TypeOf(isolate->m_isolate), type);
        if (hr < 0)
            return CHECK_ERROR(hr);
        if (type != "object" && type != "function") {
            retVal = true;
        }
    }
    return 0;
}

result_t util_base::isSymbol(v8::Local<v8::Value> v, bool& retVal)
{
    retVal = v->IsSymbol();
    return 0;
}

result_t util_base::isDataView(v8::Local<v8::Value> v, bool& retVal)
{
    retVal = v->IsDataView();
    return 0;
}

result_t util_base::isExternal(v8::Local<v8::Value> v, bool& retVal)
{
    retVal = v->IsExternal();
    return 0;
}

result_t util_base::isMap(v8::Local<v8::Value> v, bool& retVal)
{
    retVal = v->IsMap();
    return 0;
}

result_t util_base::isMapIterator(v8::Local<v8::Value> v, bool& retVal)
{
    retVal = v->IsMapIterator();
    return 0;
}

result_t util_base::isPromise(v8::Local<v8::Value> v, bool& retVal)
{
    retVal = v->IsPromise();
    return 0;
}

result_t util_base::isAsyncFunction(v8::Local<v8::Value> v, bool& retVal)
{
    retVal = v->IsAsyncFunction();
    return 0;
}

result_t util_base::isSet(v8::Local<v8::Value> v, bool& retVal)
{
    retVal = v->IsSet();
    return 0;
}

result_t util_base::isSetIterator(v8::Local<v8::Value> v, bool& retVal)
{
    retVal = v->IsSetIterator();
    return 0;
}

result_t util_base::isTypedArray(v8::Local<v8::Value> v, bool& retVal)
{
    retVal = v->IsTypedArray();
    return 0;
}

result_t util_base::isUint8Array(v8::Local<v8::Value> v, bool& retVal)
{
    retVal = v->IsUint8Array();
    return 0;
}

result_t util_base::isFunction(v8::Local<v8::Value> v, bool& retVal)
{
    retVal = v->IsFunction();
    return 0;
}

result_t util_base::isBuffer(v8::Local<v8::Value> v, bool& retVal)
{
    retVal = !!Buffer_base::getInstance(v);
    return 0;
}

static bool regexpEquals(v8::Local<v8::Value> actual, v8::Local<v8::Value> expected)
{
    v8::Local<v8::RegExp> re1 = v8::Local<v8::RegExp>::Cast(actual);
    v8::Local<v8::String> src1 = re1->GetSource();
    v8::RegExp::Flags flgs1 = re1->GetFlags();

    v8::Local<v8::RegExp> re2 = v8::Local<v8::RegExp>::Cast(expected);
    v8::Local<v8::String> src2 = re2->GetSource();
    v8::RegExp::Flags flgs2 = re2->GetFlags();

    return src1->StrictEquals(src2) && flgs1 == flgs2;
}

static bool deepEquals(QuickArray<v8::Local<v8::Object>>& acts,
    QuickArray<v8::Local<v8::Object>>& exps,
    v8::Local<v8::Value> actual, v8::Local<v8::Value> expected);

static int32_t checkStack(QuickArray<v8::Local<v8::Object>>& acts,
    QuickArray<v8::Local<v8::Object>>& exps,
    v8::Local<v8::Object> actual, v8::Local<v8::Object> expected)
{
    int32_t i;

    for (i = 0; i < (int32_t)acts.size(); i++)
        if (actual->StrictEquals(acts[i])) {
            if (expected->StrictEquals(exps[i]))
                return 0;
            return -1;
        }

    acts.append(actual);
    exps.append(expected);

    return 1;
}

static bool arrayEquals(QuickArray<v8::Local<v8::Object>>& acts,
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

    v8::Local<v8::Context> context = act->CreationContext();

    for (i = 0; i < len; i++)
        if (!deepEquals(acts, exps, JSValue(act->Get(context, i)), JSValue(exp->Get(context, i)))) {
            acts.pop();
            exps.pop();
            return false;
        }

    acts.pop();
    exps.pop();
    return true;
}

static bool objectEquals(QuickArray<v8::Local<v8::Object>>& acts,
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

    v8::Local<v8::Context> context = act->CreationContext();
    JSArray keys = act->GetPropertyNames(context);
    int32_t len = (int32_t)keys->Length();

    if (len != (int32_t)JSArray(exp->GetPropertyNames(context))->Length()) {
        acts.pop();
        exps.pop();
        return false;
    }

    for (i = 0; i < len; i++) {
        JSValue ks = keys->Get(context, (uint32_t)i);
        if (ks.IsEmpty()) {
            acts.pop();
            exps.pop();
            return false;
        }

        JSValue v1 = act->Get(context, ks);
        JSValue v2 = exp->Get(context, ks);

        if (v1.IsEmpty() || v2.IsEmpty()) {
            acts.pop();
            exps.pop();
            return false;
        }

        if (!deepEquals(acts, exps, v1, v2)) {
            acts.pop();
            exps.pop();
            return false;
        }
    }

    acts.pop();
    exps.pop();
    return true;
}

static bool deepEquals(QuickArray<v8::Local<v8::Object>>& acts,
    QuickArray<v8::Local<v8::Object>>& exps,
    v8::Local<v8::Value> actual, v8::Local<v8::Value> expected)
{
    Isolate* isolate = Isolate::current();

    if (!IsEmpty(actual) && !IsEmpty(expected) && !actual->IsFunction()
        && !expected->IsFunction()) {
        if (actual->IsDate())
            return expected->IsDate()
                && (isolate->toNumber(actual) == isolate->toNumber(expected));

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

result_t util_base::isDeepEqual(v8::Local<v8::Value> actual, v8::Local<v8::Value> expected, bool& retVal)
{
    QuickArray<v8::Local<v8::Object>> acts;
    QuickArray<v8::Local<v8::Object>> exps;

    retVal = deepEquals(acts, exps, actual, expected);

    return 0;
}

}