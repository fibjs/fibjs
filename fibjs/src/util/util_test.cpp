/*
 * util_test.cpp
 *
 *  Created on: Jun 26, 2017
 *      Author: lion
 */

#include "object.h"
#include "ifs/util.h"

namespace fibjs {

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

static bool deepEquals(Isolate* isolate, QuickArray<v8::Local<v8::Object>>& acts,
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

static bool arrayEquals(Isolate* isolate, QuickArray<v8::Local<v8::Object>>& acts,
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

    v8::Local<v8::Context> context = isolate->context();

    for (i = 0; i < len; i++)
        if (!deepEquals(isolate, acts, exps, JSValue(act->Get(context, i)), JSValue(exp->Get(context, i)))) {
            acts.pop();
            exps.pop();
            return false;
        }

    acts.pop();
    exps.pop();
    return true;
}

static bool objectEquals(Isolate* isolate, QuickArray<v8::Local<v8::Object>>& acts,
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

    v8::Local<v8::Context> context = act->GetCreationContextChecked();
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

        if (!deepEquals(isolate, acts, exps, v1, v2)) {
            acts.pop();
            exps.pop();
            return false;
        }
    }

    acts.pop();
    exps.pop();
    return true;
}

static bool deepEquals(Isolate* isolate, QuickArray<v8::Local<v8::Object>>& acts,
    QuickArray<v8::Local<v8::Object>>& exps,
    v8::Local<v8::Value> actual, v8::Local<v8::Value> expected)
{
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
            return arrayEquals(isolate, acts, exps, actual, expected);

        obj_ptr<object_base> obj1 = object_base::getInstance(actual);
        obj_ptr<object_base> obj2 = object_base::getInstance(expected);
        if (obj1 && obj2) {
            bool v;
            obj1->equals(obj2, v);
            return v;
        }

        if (actual->IsObject() && expected->IsObject())
            return objectEquals(isolate, acts, exps, actual, expected);
    }

    return actual->StrictEquals(expected);
}

result_t util_base::isDeepEqual(v8::Local<v8::Value> actual, v8::Local<v8::Value> expected, bool& retVal)
{
    QuickArray<v8::Local<v8::Object>> acts;
    QuickArray<v8::Local<v8::Object>> exps;

    retVal = deepEquals(Isolate::current(), acts, exps, actual, expected);

    return 0;
}

}