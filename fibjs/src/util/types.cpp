/*
 * util_types.cpp
 *
 *  Created on: Oct 20, 2023
 *      Author: lion
 */

#include "object.h"
#include "ifs/util.h"

namespace fibjs {

DECLARE_MODULE(types);

result_t types_base::isEmpty(v8::Local<v8::Value> v, bool& retVal)
{
    if (v->IsNull()) {
        retVal = true;
        return 0;
    }

    if (v->IsString()) {
        retVal = v.As<v8::String>()->Length() == 0;
        return 0;
    }

    if (v->IsStringObject()) {
        retVal = v.As<v8::StringObject>()->ValueOf()->Length() == 0;
        return 0;
    }

    if (v->IsArray()) {
        retVal = v.As<v8::Array>()->Length() == 0;
        return 0;
    }

    if (v->IsObject()) {
        v8::Local<v8::Object> o = v.As<v8::Object>();
        retVal = o->GetOwnPropertyNames(o->GetCreationContextChecked()).FromMaybe(v8::Local<v8::Array>())->Length() == 0;
        return 0;
    }

    retVal = true;

    return 0;
}

result_t types_base::isArray(v8::Local<v8::Value> v, bool& retVal)
{
    retVal = v->IsArray();
    return 0;
}

result_t types_base::isBoolean(v8::Local<v8::Value> v, bool& retVal)
{
    retVal = v->IsBoolean() || v->IsBooleanObject();
    return 0;
}

result_t types_base::isNull(v8::Local<v8::Value> v, bool& retVal)
{
    retVal = v->IsNull();
    return 0;
}

result_t types_base::isNullOrUndefined(v8::Local<v8::Value> v, bool& retVal)
{
    retVal = v->IsNull() || v->IsUndefined();
    return 0;
}

result_t types_base::isNumber(v8::Local<v8::Value> v, bool& retVal)
{
    retVal = v->IsNumber() || v->IsNumberObject();
    return 0;
}

result_t types_base::isBigInt(v8::Local<v8::Value> v, bool& retVal)
{
    retVal = v->IsBigInt() || v->IsBigIntObject();
    return 0;
}

result_t types_base::isString(v8::Local<v8::Value> v, bool& retVal)
{
    retVal = v->IsString() || v->IsStringObject();
    return 0;
}

result_t types_base::isUndefined(v8::Local<v8::Value> v, bool& retVal)
{
    retVal = v->IsUndefined();
    return 0;
}

result_t types_base::isRegExp(v8::Local<v8::Value> v, bool& retVal)
{
    retVal = v->IsRegExp();
    return 0;
}

result_t types_base::isObject(v8::Local<v8::Value> v, bool& retVal)
{
    retVal = v->IsObject();
    return 0;
}

result_t types_base::isDate(v8::Local<v8::Value> v, bool& retVal)
{
    retVal = v->IsDate();
    return 0;
}

result_t types_base::isNativeError(v8::Local<v8::Value> v, bool& retVal)
{
    retVal = v->IsNativeError();
    return 0;
}

result_t types_base::isPrimitive(v8::Local<v8::Value> v, bool& retVal)
{
    retVal = false;
    if (v->IsNullOrUndefined()) {
        retVal = true;
    } else {
        result_t hr;
        Isolate* isolate = Isolate::current();
        exlib::string type;
        hr = GetArgumentValue(isolate, v->TypeOf(isolate->m_isolate), type);
        if (hr < 0)
            return CHECK_ERROR(hr);
        if (type != "object" && type != "function") {
            retVal = true;
        }
    }
    return 0;
}

result_t types_base::isSymbol(v8::Local<v8::Value> v, bool& retVal)
{
    retVal = v->IsSymbol();
    return 0;
}

result_t types_base::isDataView(v8::Local<v8::Value> v, bool& retVal)
{
    retVal = v->IsDataView();
    return 0;
}

result_t types_base::isExternal(v8::Local<v8::Value> v, bool& retVal)
{
    retVal = v->IsExternal();
    return 0;
}

result_t types_base::isMap(v8::Local<v8::Value> v, bool& retVal)
{
    retVal = v->IsMap();
    return 0;
}

result_t types_base::isMapIterator(v8::Local<v8::Value> v, bool& retVal)
{
    retVal = v->IsMapIterator();
    return 0;
}

result_t types_base::isPromise(v8::Local<v8::Value> v, bool& retVal)
{
    retVal = v->IsPromise();
    return 0;
}

result_t types_base::isAsyncFunction(v8::Local<v8::Value> v, bool& retVal)
{
    retVal = v->IsAsyncFunction();
    return 0;
}

result_t types_base::isSet(v8::Local<v8::Value> v, bool& retVal)
{
    retVal = v->IsSet();
    return 0;
}

result_t types_base::isSetIterator(v8::Local<v8::Value> v, bool& retVal)
{
    retVal = v->IsSetIterator();
    return 0;
}

result_t types_base::isTypedArray(v8::Local<v8::Value> v, bool& retVal)
{
    retVal = v->IsTypedArray();
    return 0;
}

result_t types_base::isFloat32Array(v8::Local<v8::Value> v, bool& retVal)
{
    retVal = v->IsFloat32Array();
    return 0;
}

result_t types_base::isFloat64Array(v8::Local<v8::Value> v, bool& retVal)
{
    retVal = v->IsFloat64Array();
    return 0;
}

result_t types_base::isInt8Array(v8::Local<v8::Value> v, bool& retVal)
{
    retVal = v->IsInt8Array();
    return 0;
}

result_t types_base::isInt16Array(v8::Local<v8::Value> v, bool& retVal)
{
    retVal = v->IsInt16Array();
    return 0;
}

result_t types_base::isInt32Array(v8::Local<v8::Value> v, bool& retVal)
{
    retVal = v->IsInt32Array();
    return 0;
}

result_t types_base::isUint8Array(v8::Local<v8::Value> v, bool& retVal)
{
    retVal = v->IsUint8Array();
    return 0;
}

result_t types_base::isUint8ClampedArray(v8::Local<v8::Value> v, bool& retVal)
{
    retVal = v->IsUint8ClampedArray();
    return 0;
}

result_t types_base::isUint16Array(v8::Local<v8::Value> v, bool& retVal)
{
    retVal = v->IsUint16Array();
    return 0;
}

result_t types_base::isUint32Array(v8::Local<v8::Value> v, bool& retVal)
{
    retVal = v->IsUint32Array();
    return 0;
}

result_t types_base::isFunction(v8::Local<v8::Value> v, bool& retVal)
{
    retVal = v->IsFunction();
    return 0;
}

result_t types_base::isBuffer(v8::Local<v8::Value> v, bool& retVal)
{
    retVal = IsJSBuffer(v);
    return 0;
}

result_t util_base::isEmpty(v8::Local<v8::Value> v, bool& retVal)
{
    return types_base::isEmpty(v, retVal);
}

result_t util_base::isArray(v8::Local<v8::Value> v, bool& retVal)
{
    return types_base::isArray(v, retVal);
}

result_t util_base::isBoolean(v8::Local<v8::Value> v, bool& retVal)
{
    return types_base::isBoolean(v, retVal);
}

result_t util_base::isNull(v8::Local<v8::Value> v, bool& retVal)
{
    return types_base::isNull(v, retVal);
}

result_t util_base::isNullOrUndefined(v8::Local<v8::Value> v, bool& retVal)
{
    return types_base::isNullOrUndefined(v, retVal);
}

result_t util_base::isNumber(v8::Local<v8::Value> v, bool& retVal)
{
    return types_base::isNumber(v, retVal);
}

result_t util_base::isBigInt(v8::Local<v8::Value> v, bool& retVal)
{
    return types_base::isBigInt(v, retVal);
}

result_t util_base::isString(v8::Local<v8::Value> v, bool& retVal)
{
    return types_base::isString(v, retVal);
}

result_t util_base::isUndefined(v8::Local<v8::Value> v, bool& retVal)
{
    return types_base::isUndefined(v, retVal);
}

result_t util_base::isRegExp(v8::Local<v8::Value> v, bool& retVal)
{
    return types_base::isRegExp(v, retVal);
}

result_t util_base::isObject(v8::Local<v8::Value> v, bool& retVal)
{
    return types_base::isObject(v, retVal);
}

result_t util_base::isDate(v8::Local<v8::Value> v, bool& retVal)
{
    return types_base::isDate(v, retVal);
}

result_t util_base::isNativeError(v8::Local<v8::Value> v, bool& retVal)
{
    return types_base::isNativeError(v, retVal);
}

result_t util_base::isPrimitive(v8::Local<v8::Value> v, bool& retVal)
{
    return types_base::isPrimitive(v, retVal);
}

result_t util_base::isSymbol(v8::Local<v8::Value> v, bool& retVal)
{
    return types_base::isSymbol(v, retVal);
}

result_t util_base::isDataView(v8::Local<v8::Value> v, bool& retVal)
{
    return types_base::isDataView(v, retVal);
}

result_t util_base::isExternal(v8::Local<v8::Value> v, bool& retVal)
{
    return types_base::isExternal(v, retVal);
}

result_t util_base::isMap(v8::Local<v8::Value> v, bool& retVal)
{
    return types_base::isMap(v, retVal);
}

result_t util_base::isMapIterator(v8::Local<v8::Value> v, bool& retVal)
{
    return types_base::isMapIterator(v, retVal);
}

result_t util_base::isPromise(v8::Local<v8::Value> v, bool& retVal)
{
    return types_base::isPromise(v, retVal);
}

result_t util_base::isAsyncFunction(v8::Local<v8::Value> v, bool& retVal)
{
    return types_base::isAsyncFunction(v, retVal);
}

result_t util_base::isSet(v8::Local<v8::Value> v, bool& retVal)
{
    return types_base::isSet(v, retVal);
}

result_t util_base::isSetIterator(v8::Local<v8::Value> v, bool& retVal)
{
    return types_base::isSetIterator(v, retVal);
}

result_t util_base::isTypedArray(v8::Local<v8::Value> v, bool& retVal)
{
    return types_base::isTypedArray(v, retVal);
}

result_t util_base::isUint8Array(v8::Local<v8::Value> v, bool& retVal)
{
    return types_base::isUint8Array(v, retVal);
}

result_t util_base::isFunction(v8::Local<v8::Value> v, bool& retVal)
{
    return types_base::isFunction(v, retVal);
}

result_t util_base::isBuffer(v8::Local<v8::Value> v, bool& retVal)
{
    return types_base::isBuffer(v, retVal);
}

}
