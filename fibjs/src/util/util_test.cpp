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
        retVal = v->ToObject()->GetOwnPropertyNames()->Length() == 0;
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
}