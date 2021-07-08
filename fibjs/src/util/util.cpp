/*
 * util.cpp
 *
 *  Created on: Dec 10, 2013
 *      Author: lion
 */

#include "object.h"
#include "ifs/util.h"
#include "ifs/encoding.h"
#include "QuickArray.h"
#include "parse.h"
#include "SimpleObject.h"
#include <map>

namespace fibjs {

DECLARE_MODULE(util);

result_t util_base::inherits(v8::Local<v8::Value> constructor,
    v8::Local<v8::Value> superConstructor)
{
    if (!constructor->IsObject() || !superConstructor->IsObject()
        || constructor->IsNullOrUndefined() || superConstructor->IsNullOrUndefined())
        return CALL_E_TYPEMISMATCH;

    Isolate* isolate = Isolate::current();
    v8::Local<v8::Context> context = isolate->context();

    v8::Local<v8::Object> _constructor = constructor.As<v8::Object>();
    v8::Local<v8::Object> _superConstructor = superConstructor.As<v8::Object>();

    v8::Local<v8::Object> constructor_proto = JSValue(_constructor->Get(context, isolate->NewString("prototype"))).As<v8::Object>();
    v8::Local<v8::Object> superConstructor_proto = JSValue(_superConstructor->Get(context, isolate->NewString("prototype"))).As<v8::Object>();

    if (superConstructor_proto->IsUndefined())
        return CALL_E_TYPEMISMATCH;

    _constructor->Set(context, isolate->NewString("super_"), _superConstructor);
    constructor_proto->Set(context, isolate->NewString("__proto__"), superConstructor_proto);
    return 0;
}

result_t util_base::has(v8::Local<v8::Value> v, exlib::string key, bool& retVal)
{
    if (v->IsUndefined() || v->IsNull()) {
        retVal = false;
        return 0;
    }

    if (!v->IsObject())
        return CHECK_ERROR(CALL_E_INVALIDARG);

    Isolate* isolate = Isolate::current();
    v8::Local<v8::Context> context = isolate->context();
    v8::Local<v8::Object> obj = v8::Local<v8::Object>::Cast(v);
    retVal = obj->HasOwnProperty(context, isolate->NewString(key)).ToChecked();
    return 0;
}

result_t util_base::keys(v8::Local<v8::Value> v, v8::Local<v8::Array>& retVal)
{
    Isolate* isolate = Isolate::current();

    if (v->IsObject()) {
        v8::Local<v8::Context> context = isolate->context();

        v8::Local<v8::Object> obj = v8::Local<v8::Object>::Cast(v);

        retVal = JSArray(obj->GetPropertyNames(context));
        if (obj->IsArray()) {
            int32_t len = retVal->Length();
            int32_t i;

            for (i = 0; i < len; i++)
                retVal->Set(context, i, isolate->toLocalString(JSValue(retVal->Get(context, i))));
        }
    } else
        retVal = v8::Array::New(isolate->m_isolate);

    return 0;
}

result_t util_base::values(v8::Local<v8::Value> v, v8::Local<v8::Array>& retVal)
{
    Isolate* isolate = Isolate::current();
    v8::Local<v8::Context> context = isolate->context();

    if (v->IsObject()) {
        v8::Local<v8::Object> obj = v8::Local<v8::Object>::Cast(v);
        JSArray keys = obj->GetPropertyNames(context);
        v8::Local<v8::Array> arr = v8::Array::New(isolate->m_isolate);

        int32_t len = keys->Length();
        int32_t i, n = 0;

        for (i = 0; i < len; i++) {
            JSValue key = keys->Get(context, i);
            arr->Set(context, n++, JSValue(obj->Get(context, key)));
        }

        retVal = arr;
    } else
        retVal = v8::Array::New(isolate->m_isolate);

    return 0;
}

result_t util_base::clone(v8::Local<v8::Value> v, v8::Local<v8::Value>& retVal)
{
    if (v.IsEmpty())
        return CALL_E_JAVASCRIPT;

    if (!v->IsProxy() && v->IsObject() && !object_base::getInstance(v)) {
        Isolate* isolate = Isolate::current();
        v8::Local<v8::Context> _context = isolate->context();

        if (v->IsFunction() || v->IsArgumentsObject() || v->IsSymbolObject())
            retVal = v;
        else if (v->IsDate())
            v8::Date::New(_context, isolate->toNumber(v)).ToLocal(&retVal);
        else if (v->IsBooleanObject())
            retVal = v8::BooleanObject::New(isolate->m_isolate, isolate->toBoolean(v));
        else if (v->IsNumberObject())
            retVal = v8::NumberObject::New(isolate->m_isolate, isolate->toNumber(v));
        else if (v->IsStringObject()) {
            v8::Local<v8::StringObject> so = v8::Local<v8::StringObject>::Cast(v);
            retVal = v8::StringObject::New(isolate->m_isolate, so->ValueOf());
        } else if (v->IsRegExp()) {
            v8::Local<v8::RegExp> re = v8::Local<v8::RegExp>::Cast(v);
            retVal = v8::RegExp::New(_context, re->GetSource(), re->GetFlags()).ToLocalChecked();
        } else if (v->IsFunction() || v->IsArray() || IsJSObject(v))
            retVal = v8::Local<v8::Object>::Cast(v)->Clone();
        else
            retVal = v;
    } else
        retVal = v;

    return 0;
}

result_t util_base::deepFreeze(v8::Local<v8::Value> v)
{
    if (v.IsEmpty() || !v->IsObject())
        return 0;

    v8::Local<v8::Object> obj = v8::Local<v8::Object>::Cast(v);

    if (!isFrozen(obj)) {
        v8::Local<v8::Context> _context = obj->CreationContext();
        if (obj->SetIntegrityLevel(_context, v8::IntegrityLevel::kFrozen).IsNothing())
            return CALL_E_JAVASCRIPT;

        JSArray names = obj->GetPropertyNames(_context);
        for (int32_t i = 0; i < (int32_t)names->Length(); i++) {
            result_t hr = deepFreeze(JSValue(obj->Get(_context, JSValue(names->Get(_context, i)))));
            if (hr < 0)
                return hr;
        }
    }

    return 0;
}

result_t util_base::extend(v8::Local<v8::Value> v, OptArgs objs,
    v8::Local<v8::Value>& retVal)
{
    if (v->IsUndefined() || v->IsNull()) {
        retVal = v;
        return 0;
    }

    if (!v->IsObject())
        return CHECK_ERROR(CALL_E_INVALIDARG);

    Isolate* isolate = Isolate::current();
    v8::Local<v8::Context> context = isolate->context();

    v8::Local<v8::Object> obj = v8::Local<v8::Object>::Cast(v);
    int32_t argc = objs.Length();
    int32_t i, j;

    for (i = 0; i < argc; i++) {
        v8::Local<v8::Value> val = objs[i];

        if (val->IsUndefined() || val->IsNull())
            continue;

        if (!val->IsObject())
            return CHECK_ERROR(CALL_E_INVALIDARG);

        v8::Local<v8::Object> obj1 = v8::Local<v8::Object>::Cast(val);
        JSArray keys = obj1->GetPropertyNames(context);
        int32_t len = keys->Length();

        for (j = 0; j < len; j++) {
            JSValue key = keys->Get(context, j);
            obj->Set(context, key, JSValue(obj1->Get(context, key)));
        }
    }

    retVal = obj;

    return 0;
}

result_t util_base::_extend(v8::Local<v8::Value> v, OptArgs objs,
    v8::Local<v8::Value>& retVal)
{
    return extend(v, objs, retVal);
}

result_t util_base::pick(v8::Local<v8::Value> v, OptArgs objs,
    v8::Local<v8::Object>& retVal)
{
    Isolate* isolate = Isolate::current();
    v8::Local<v8::Context> context = isolate->context();

    if (v->IsUndefined() || v->IsNull()) {
        retVal = v8::Object::New(isolate->m_isolate);
        return 0;
    }

    if (!v->IsObject())
        return CHECK_ERROR(CALL_E_INVALIDARG);

    v8::Local<v8::Object> obj = v8::Local<v8::Object>::Cast(v);
    v8::Local<v8::Object> obj1 = v8::Object::New(isolate->m_isolate);

    int32_t argc = objs.Length();
    int32_t i, j;

    for (i = 0; i < argc; i++) {
        v8::Local<v8::Value> o = objs[i];
        if (o->IsArray()) {
            v8::Local<v8::Array> arr = v8::Local<v8::Array>::Cast(o);
            int32_t len = arr->Length();

            for (j = 0; j < len; j++) {
                JSValue k = arr->Get(context, j);

                if (obj->Has(context, k).ToChecked())
                    obj1->Set(context, k, JSValue(obj->Get(context, k)));
            }
        } else {
            JSValue k = o;

            if (obj->Has(context, k).ToChecked())
                obj1->Set(context, k, JSValue(obj->Get(context, k)));
        }
    }

    retVal = obj1;

    return 0;
}

result_t util_base::omit(v8::Local<v8::Value> v, OptArgs keys,
    v8::Local<v8::Object>& retVal)
{
    Isolate* isolate = Isolate::current();
    v8::Local<v8::Context> context = isolate->context();

    if (v->IsUndefined() || v->IsNull()) {
        retVal = v8::Object::New(isolate->m_isolate);
        return 0;
    }

    if (!v->IsObject())
        return CHECK_ERROR(CALL_E_INVALIDARG);

    v8::Local<v8::Object> obj = v8::Local<v8::Object>::Cast(v);

    std::map<exlib::string, bool> _map;
    int32_t argc = keys.Length();
    int32_t i, j;
    result_t hr;

    for (i = 0; i < argc; i++) {
        v8::Local<v8::Value> o = keys[i];
        if (o->IsArray()) {
            v8::Local<v8::Array> arr = v8::Local<v8::Array>::Cast(o);
            int32_t len = arr->Length();

            for (j = 0; j < len; j++) {
                exlib::string k;
                hr = GetArgumentValue(JSValue(arr->Get(context, j)), k);
                if (hr < 0)
                    return CHECK_ERROR(hr);

                _map.insert(std::pair<exlib::string, bool>(k, true));
            }
        } else {
            exlib::string k;
            hr = GetArgumentValue(o, k);
            if (hr < 0)
                return CHECK_ERROR(hr);

            _map.insert(std::pair<exlib::string, bool>(k, true));
        }
    }

    JSArray keys1 = obj->GetPropertyNames(context);
    int32_t len = keys1->Length();
    v8::Local<v8::Object> obj1 = v8::Object::New(isolate->m_isolate);

    for (i = 0; i < len; i++) {
        JSValue key = keys1->Get(context, i);

        if (_map.find(isolate->toString(key)) == _map.end()) {
            JSValue value = obj->Get(context, key);
            obj1->Set(context, key, value);
        }
    }

    retVal = obj1;

    return 0;
}

result_t util_base::intersection(OptArgs arrs,
    v8::Local<v8::Array>& retVal)
{
    Isolate* isolate = Isolate::current();
    v8::Local<v8::Context> context = isolate->context();
    v8::Local<v8::Array> arr = v8::Array::New(isolate->m_isolate);
    int32_t argc = arrs.Length();
    int32_t i, j, k, n = 0;

    if (argc > 0) {
        JSValue v0 = arrs[0];

        if (v0->IsUndefined() || v0->IsNull()) {
            retVal = arr;
            return 0;
        }

        if (!v0->IsArray())
            return CHECK_ERROR(CALL_E_INVALIDARG);

        v8::Local<v8::Array> base = v8::Local<v8::Array>::Cast(v0);
        int32_t len = base->Length();
        int32_t left = len;
        QuickArray<JSValue> erase;

        erase.resize(len);
        for (i = 0; i < len; i++)
            erase[i] = base->Get(context, i);

        for (i = 1; left > 0 && i < argc; i++) {
            v0 = arrs[i];
            if (!v0->IsArray())
                return CHECK_ERROR(CALL_E_INVALIDARG);

            v8::Local<v8::Array> other = v8::Local<v8::Array>::Cast(v0);
            int32_t len1 = other->Length();

            for (j = 0; left > 0 && j < len; j++)
                if (!erase[j].IsEmpty()) {
                    for (k = 0; k < len1; k++)
                        if (erase[j]->StrictEquals(JSValue(other->Get(context, k))))
                            break;

                    if (k == len1) {
                        erase[j] = v8::Local<v8::Value>();
                        left--;
                    }
                }
        }

        if (left) {
            Isolate* isolate = Isolate::current();

            for (i = 0; i < len; i++)
                if (!erase[i].IsEmpty()) {
                    for (j = 0; j < i; j++)
                        if (!erase[j].IsEmpty() && erase[i]->StrictEquals(erase[j]))
                            break;

                    if (j == i)
                        arr->Set(context, n++, erase[i]);
                }
        }
    }

    retVal = arr;

    return 0;
}

result_t util_base::first(v8::Local<v8::Value> v, v8::Local<v8::Value>& retVal)
{
    Isolate* isolate = Isolate::current();
    v8::Local<v8::Context> context = isolate->context();

    if (v->IsUndefined() || v->IsNull()) {
        retVal = v8::Undefined(isolate->m_isolate);
        return 0;
    }

    if (!v->IsArray())
        return CHECK_ERROR(CALL_E_INVALIDARG);

    v8::Local<v8::Array> arr = v8::Local<v8::Array>::Cast(v);
    int32_t len = arr->Length();

    if (len == 0) {
        retVal = v8::Undefined(isolate->m_isolate);
        return 0;
    }

    retVal = JSValue(arr->Get(context, 0));

    return 0;
}

result_t util_base::first(v8::Local<v8::Value> v, int32_t n, v8::Local<v8::Value>& retVal)
{
    Isolate* isolate = Isolate::current();
    v8::Local<v8::Context> context = isolate->context();

    if (v->IsUndefined() || v->IsNull() || n <= 0) {
        retVal = v8::Array::New(isolate->m_isolate);
        return 0;
    }

    if (!v->IsArray())
        return CHECK_ERROR(CALL_E_INVALIDARG);

    v8::Local<v8::Array> arr = v8::Local<v8::Array>::Cast(v);
    int32_t len = arr->Length();
    int32_t i;

    if (n > len)
        n = len;

    v8::Local<v8::Array> arr1 = v8::Array::New(isolate->m_isolate);

    for (i = 0; i < n; i++)
        arr1->Set(context, i, JSValue(arr->Get(context, i)));

    retVal = arr1;

    return 0;
}

result_t util_base::last(v8::Local<v8::Value> v, v8::Local<v8::Value>& retVal)
{
    Isolate* isolate = Isolate::current();
    v8::Local<v8::Context> context = isolate->context();

    if (v->IsUndefined() || v->IsNull()) {
        retVal = v8::Undefined(isolate->m_isolate);
        return 0;
    }

    if (!v->IsArray())
        return CHECK_ERROR(CALL_E_INVALIDARG);

    v8::Local<v8::Array> arr = v8::Local<v8::Array>::Cast(v);
    int32_t len = arr->Length();

    if (len == 0) {
        retVal = v8::Undefined(isolate->m_isolate);
        return 0;
    }

    retVal = JSValue(arr->Get(context, len - 1));
    return 0;
}

result_t util_base::last(v8::Local<v8::Value> v, int32_t n, v8::Local<v8::Value>& retVal)
{
    Isolate* isolate = Isolate::current();
    v8::Local<v8::Context> context = isolate->context();
    if (v->IsUndefined() || v->IsNull() || n <= 0) {
        retVal = v8::Array::New(isolate->m_isolate);
        return 0;
    }

    if (!v->IsArray())
        return CHECK_ERROR(CALL_E_INVALIDARG);

    v8::Local<v8::Array> arr = v8::Local<v8::Array>::Cast(v);
    int32_t len = arr->Length();
    int32_t i;

    if (n > len)
        n = len;

    v8::Local<v8::Array> arr1 = v8::Array::New(isolate->m_isolate);

    for (i = 0; i < n; i++)
        arr1->Set(context, i, JSValue(arr->Get(context, len - n + i)));

    retVal = arr1;

    return 0;
}

result_t util_base::unique(v8::Local<v8::Value> v, bool sorted, v8::Local<v8::Array>& retVal)
{
    Isolate* isolate = Isolate::current();
    v8::Local<v8::Context> context = isolate->context();
    if (v->IsUndefined() || v->IsNull()) {
        retVal = v8::Array::New(isolate->m_isolate);
        return 0;
    }

    if (!v->IsArray())
        return CHECK_ERROR(CALL_E_INVALIDARG);

    v8::Local<v8::Array> arr1 = v8::Array::New(isolate->m_isolate);
    v8::Local<v8::Array> arr = v8::Local<v8::Array>::Cast(v);
    int32_t len = arr->Length();
    QuickArray<JSValue> vals;
    int32_t i, j, n = 0;

    vals.resize(len);
    for (i = 0; i < len; i++) {
        JSValue val = arr->Get(context, i);

        for (j = i - 1; j >= 0; j--)
            if (!vals[j].IsEmpty()) {
                if (val->StrictEquals(vals[j]))
                    break;

                if (sorted) {
                    j = -1;
                    break;
                }
            }

        if (j < 0)
            vals[i] = val;
    }

    for (i = 0; i < len; i++)
        if (!vals[i].IsEmpty())
            arr1->Set(context, n++, vals[i]);

    retVal = arr1;

    return 0;
}

result_t util_base::_union(OptArgs arrs,
    v8::Local<v8::Array>& retVal)
{
    Isolate* isolate = Isolate::current();
    v8::Local<v8::Context> context = isolate->context();
    v8::Local<v8::Array> arr = v8::Array::New(isolate->m_isolate);
    int32_t argc = arrs.Length();
    int32_t i, j, k, n = 0;

    for (i = 0; i < argc; i++) {
        v8::Local<v8::Value> a = arrs[i];

        if (!a->IsArray())
            return CHECK_ERROR(CALL_E_INVALIDARG);

        v8::Local<v8::Array> arr1 = v8::Local<v8::Array>::Cast(a);
        int32_t len = arr1->Length();

        for (j = 0; j < len; j++) {
            JSValue v = arr1->Get(context, j);

            for (k = 0; k < n; k++)
                if (v->StrictEquals(JSValue(arr->Get(context, k))))
                    break;

            if (k == n)
                arr->Set(context, n++, v);
        }
    }

    retVal = arr;

    return 0;
}

static result_t util_flatten(v8::Local<v8::Value> list, bool shallow,
    v8::Local<v8::Array>& retVal, QuickArray<JSValue>& flatten_list)
{
    if (!list->IsObject())
        return CHECK_ERROR(CALL_E_INVALIDARG);

    flatten_list.append(list);

    bool bNext = true;

    Isolate* isolate = Isolate::current();
    v8::Local<v8::Context> context = isolate->context();

    if (retVal.IsEmpty())
        retVal = v8::Array::New(isolate->m_isolate);
    else if (shallow)
        bNext = false;

    v8::Local<v8::Object> o = v8::Local<v8::Object>::Cast(list);
    JSValue v = o->Get(context, isolate->NewString("length"));
    if (IsEmpty(v))
        return CHECK_ERROR(CALL_E_INVALIDARG);

    int32_t len = isolate->toInt32Value(v);
    int32_t cnt = retVal->Length();
    int32_t i, j;

    for (i = 0; i < len; i++) {
        v = o->Get(context, i);
        if (bNext && v->IsObject()) {
            v8::Local<v8::Object> o1 = v8::Local<v8::Object>::Cast(v);
            v = o->Get(context, isolate->NewString("length"));
            if (IsEmpty(v))
                retVal->Set(context, cnt++, JSValue(o->Get(context, i)));
            else {
                for (j = 0; j < (int32_t)flatten_list.size(); j++)
                    if (flatten_list[j]->StrictEquals(o1))
                        return CHECK_ERROR(Runtime::setError("util: circular reference object."));

                result_t hr = util_flatten(o1, shallow, retVal, flatten_list);
                if (hr < 0)
                    return hr;

                cnt = retVal->Length();
            }
        } else
            retVal->Set(context, cnt++, JSValue(o->Get(context, i)));
    }

    return 0;
}

result_t util_base::flatten(v8::Local<v8::Value> list, bool shallow,
    v8::Local<v8::Array>& retVal)
{
    QuickArray<JSValue> flatten_list;
    return util_flatten(list, shallow, retVal, flatten_list);
}

result_t util_base::without(v8::Local<v8::Value> arr, OptArgs els,
    v8::Local<v8::Array>& retVal)
{
    if (!arr->IsObject())
        return CHECK_ERROR(CALL_E_INVALIDARG);

    Isolate* isolate = Isolate::current();
    v8::Local<v8::Context> context = isolate->context();

    v8::Local<v8::Object> o = v8::Local<v8::Object>::Cast(arr);
    JSValue v = o->Get(context, isolate->NewString("length"));
    if (IsEmpty(v))
        return CHECK_ERROR(CALL_E_INVALIDARG);

    int32_t len = isolate->toInt32Value(v);

    v8::Local<v8::Array> arr1 = v8::Array::New(isolate->m_isolate);
    int32_t argc = els.Length();
    int32_t i, j, n = 0;

    for (i = 0; i < len; i++) {
        JSValue v = o->Get(context, i);

        for (j = 0; j < argc; j++)
            if (v->StrictEquals(els[j]))
                break;

        if (j == argc)
            arr1->Set(context, n++, v);
    }

    retVal = arr1;

    return 0;
}

result_t util_base::difference(v8::Local<v8::Array> arr, OptArgs arrs,
    v8::Local<v8::Array>& retVal)
{
    Isolate* isolate = Isolate::current();
    v8::Local<v8::Context> context = isolate->context();
    v8::Local<v8::Array> arr1 = v8::Array::New(isolate->m_isolate);
    int32_t len = arr->Length();
    int32_t argc = arrs.Length();
    int32_t i, j, k, n = 0, len1;

    for (i = 0; i < len; i++) {
        JSValue v = arr->Get(context, i);

        for (j = 0; j < argc; j++) {
            v8::Local<v8::Value> o = arrs[j];
            if (!o->IsArray())
                return CHECK_ERROR(CALL_E_INVALIDARG);

            v8::Local<v8::Array> without = v8::Local<v8::Array>::Cast(o);
            len1 = without->Length();

            for (k = 0; k < len1; k++)
                if (v->StrictEquals(JSValue(without->Get(context, k))))
                    break;

            if (k < len1)
                break;
        }

        if (j == argc)
            arr1->Set(context, n++, v);
    }

    retVal = arr1;

    return 0;
}

result_t util_base::each(v8::Local<v8::Value> list, v8::Local<v8::Function> iterator,
    v8::Local<v8::Value> context, v8::Local<v8::Value>& retVal)
{
    if (!list->IsObject()) {
        retVal = list;
        return 0;
    }

    v8::Local<v8::Value> args[3];
    args[2] = list;

    Isolate* isolate = Isolate::current();
    v8::Local<v8::Context> _context = isolate->context();
    v8::Local<v8::Object> o = v8::Local<v8::Object>::Cast(list);
    JSValue v = o->Get(_context, isolate->NewString("length"));

    if (IsEmpty(v)) {
        int32_t len = 0;
        JSArray keys = o->GetPropertyNames(_context);
        if (!keys.IsEmpty())
            len = keys->Length();

        int32_t i;

        for (i = 0; i < len; i++) {
            args[1] = JSValue(keys->Get(_context, i));
            args[0] = JSValue(o->Get(_context, args[1]));
            if (args[0].IsEmpty() || args[1].IsEmpty())
                return CALL_E_JAVASCRIPT;

            iterator->Call(_context, context, 3, args).ToLocal(&v);
            if (v.IsEmpty())
                return CALL_E_JAVASCRIPT;
        }
    } else {
        int32_t len = isolate->toInt32Value(v);

        int32_t i;

        for (i = 0; i < len; i++) {
            args[1] = v8::Int32::New(isolate->m_isolate, i);
            args[0] = JSValue(o->Get(_context, args[1]));
            if (args[0].IsEmpty() || args[1].IsEmpty())
                return CALL_E_JAVASCRIPT;

            iterator->Call(_context, context, 3, args).ToLocal(&v);
            if (v.IsEmpty())
                return CALL_E_JAVASCRIPT;
        }
    }

    retVal = list;

    return 0;
}

result_t util_base::map(v8::Local<v8::Value> list, v8::Local<v8::Function> iterator,
    v8::Local<v8::Value> context, v8::Local<v8::Array>& retVal)
{
    Isolate* isolate = Isolate::current();
    v8::Local<v8::Context> _context = isolate->context();
    v8::Local<v8::Array> arr = v8::Array::New(isolate->m_isolate);

    if (!list->IsObject()) {
        retVal = arr;
        return 0;
    }

    v8::Local<v8::Value> args[3];
    args[2] = list;

    v8::Local<v8::Object> o = v8::Local<v8::Object>::Cast(list);
    JSValue v = o->Get(_context, isolate->NewString("length"));
    int32_t cnt = 0;

    if (IsEmpty(v)) {
        int32_t len = 0;
        JSArray keys = o->GetPropertyNames(o->CreationContext());
        if (!keys.IsEmpty())
            len = keys->Length();
        int32_t i;

        for (i = 0; i < len; i++) {
            args[1] = JSValue(keys->Get(_context, i));
            args[0] = JSValue(o->Get(_context, args[1]));
            if (args[0].IsEmpty() || args[1].IsEmpty())
                return CALL_E_JAVASCRIPT;

            iterator->Call(_context, context, 3, args).ToLocal(&v);
            if (v.IsEmpty())
                return CALL_E_JAVASCRIPT;

            arr->Set(_context, cnt++, v);
        }
    } else {
        int32_t len = isolate->toInt32Value(v);

        int32_t i;

        for (i = 0; i < len; i++) {
            args[1] = v8::Int32::New(isolate->m_isolate, i);
            args[0] = JSValue(o->Get(_context, args[1]));
            if (args[0].IsEmpty() || args[1].IsEmpty())
                return CALL_E_JAVASCRIPT;

            iterator->Call(_context, context, 3, args).ToLocal(&v);
            if (v.IsEmpty())
                return CALL_E_JAVASCRIPT;

            arr->Set(_context, cnt++, v);
        }
    }

    retVal = arr;

    return 0;
}

result_t util_base::reduce(v8::Local<v8::Value> list, v8::Local<v8::Function> iterator,
    v8::Local<v8::Value> memo, v8::Local<v8::Value> context,
    v8::Local<v8::Value>& retVal)
{
    if (!list->IsObject()) {
        retVal = memo;
        return 0;
    }

    v8::Local<v8::Value> args[4];
    args[3] = list;

    Isolate* isolate = Isolate::current();
    v8::Local<v8::Context> _context = isolate->context();
    v8::Local<v8::Object> o = v8::Local<v8::Object>::Cast(list);
    JSValue v = o->Get(_context, isolate->NewString("length"));

    if (IsEmpty(v)) {
        int32_t len = 0;
        JSArray keys = o->GetPropertyNames(_context);
        if (!keys.IsEmpty())
            len = keys->Length();

        int32_t i;

        for (i = 0; i < len; i++) {
            args[2] = JSValue(keys->Get(_context, i));
            args[1] = JSValue(o->Get(_context, args[2]));
            if (args[1].IsEmpty() || args[2].IsEmpty())
                return CALL_E_JAVASCRIPT;

            args[0] = memo;

            iterator->Call(_context, context, 4, args).ToLocal(&memo);
            if (memo.IsEmpty())
                return CALL_E_JAVASCRIPT;
        }
    } else {
        int32_t len = isolate->toInt32Value(v);

        int32_t i;

        for (i = 0; i < len; i++) {
            args[2] = v8::Int32::New(isolate->m_isolate, i);
            args[1] = JSValue(o->Get(_context, args[2]));
            if (args[1].IsEmpty() || args[2].IsEmpty())
                return CALL_E_JAVASCRIPT;

            args[0] = memo;

            iterator->Call(_context, context, 4, args).ToLocal(&memo);
            if (memo.IsEmpty())
                return CALL_E_JAVASCRIPT;
        }
    }

    retVal = memo;

    return 0;
}

result_t util_base::deprecate(v8::Local<v8::Function> fn, exlib::string msg,
    exlib::string code, v8::Local<v8::Function>& retVal)
{
    retVal = fn;
    return 0;
}

result_t util_base::parseArgs(exlib::string command, obj_ptr<NArray>& retVal)
{
    obj_ptr<NArray> arr = new NArray;
    _parser p(command);

    while (!p.end()) {
        exlib::string s;

        if (p.want('\"')) {
            while (!p.end()) {
                exlib::string s1;

                p.getString(s1, '\"', '\\');
                s.append(s1);

                if (p.get() == '\\') {
                    p.skip();
                    if (!p.end())
                        s.append(1, p.getChar());
                } else {
                    p.skip();
                    if (qisspace(p.get()))
                        break;
                }
            }
        } else {
            while (!p.end() && !qisspace(p.get())) {
                exlib::string s1;

                p.getWord(s1, '\\');
                s.append(s1);

                if (p.get() == '\\') {
                    p.skip();
                    if (!p.end())
                        s.append(1, p.getChar());
                }
            }
        }

        arr->append(s);
    }

    retVal = arr;
    return 0;
}
}