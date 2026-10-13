/*
 * util_types.cpp
 *
 *  Created on: Oct 20, 2023
 *      Author: lion
 */

#include "object.h"
#include "ifs/util.h"
#include "ifs/CryptoKey.h"
#include "ifs/KeyObject.h"

namespace fibjs {

DECLARE_MODULE(types);

result_t types_base::isEmpty(v8::Local<v8::Value> v, bool& retVal)
{
    if (v->IsNull()) {
        retVal = true;
        return result_t();
    }

    if (v->IsString()) {
        retVal = v.As<v8::String>()->Length() == 0;
        return result_t();
    }

    if (v->IsStringObject()) {
        retVal = v.As<v8::StringObject>()->ValueOf()->Length() == 0;
        return result_t();
    }

    if (v->IsArray()) {
        retVal = v.As<v8::Array>()->Length() == 0;
        return result_t();
    }

    if (v->IsObject()) {
        v8::Local<v8::Object> o = v.As<v8::Object>();

        v8::Local<v8::Context> context;
        if (!o->GetCreationContext().ToLocal(&context))
            return result_t(CALL_E_JAVASCRIPT);

        v8::Local<v8::Array> keys;
        if (!o->GetOwnPropertyNames(context).ToLocal(&keys) || keys.IsEmpty())
            return result_t(CALL_E_JAVASCRIPT);
        retVal = keys->Length() == 0;
        return result_t();
    }

    retVal = true;

    return result_t();
}

result_t types_base::isArray(v8::Local<v8::Value> v, bool& retVal)
{
    retVal = v->IsArray();
    return result_t();
}

result_t types_base::isBoolean(v8::Local<v8::Value> v, bool& retVal)
{
    retVal = v->IsBoolean() || v->IsBooleanObject();
    return result_t();
}

result_t types_base::isNull(v8::Local<v8::Value> v, bool& retVal)
{
    retVal = v->IsNull();
    return result_t();
}

result_t types_base::isNullOrUndefined(v8::Local<v8::Value> v, bool& retVal)
{
    retVal = v->IsNull() || v->IsUndefined();
    return result_t();
}

result_t types_base::isNumber(v8::Local<v8::Value> v, bool& retVal)
{
    retVal = v->IsNumber() || v->IsNumberObject();
    return result_t();
}

result_t types_base::isBigInt(v8::Local<v8::Value> v, bool& retVal)
{
    retVal = v->IsBigInt() || v->IsBigIntObject();
    return result_t();
}

result_t types_base::isString(v8::Local<v8::Value> v, bool& retVal)
{
    retVal = v->IsString() || v->IsStringObject();
    return result_t();
}

result_t types_base::isUndefined(v8::Local<v8::Value> v, bool& retVal)
{
    retVal = v->IsUndefined();
    return result_t();
}

result_t types_base::isRegExp(v8::Local<v8::Value> v, bool& retVal)
{
    retVal = v->IsRegExp();
    return result_t();
}

result_t types_base::isObject(v8::Local<v8::Value> v, bool& retVal)
{
    retVal = v->IsObject();
    return result_t();
}

result_t types_base::isDate(v8::Local<v8::Value> v, bool& retVal)
{
    retVal = v->IsDate();
    return result_t();
}

result_t types_base::isNativeError(v8::Local<v8::Value> v, bool& retVal)
{
    retVal = v->IsNativeError();
    return result_t();
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
        if (hr.failed())
            return hr;
        if (type != "object" && type != "function") {
            retVal = true;
        }
    }
    return result_t();
}

result_t types_base::isSymbol(v8::Local<v8::Value> v, bool& retVal)
{
    retVal = v->IsSymbol();
    return result_t();
}

result_t types_base::isDataView(v8::Local<v8::Value> v, bool& retVal)
{
    retVal = v->IsDataView();
    return result_t();
}

result_t types_base::isExternal(v8::Local<v8::Value> v, bool& retVal)
{
    retVal = v->IsExternal();
    return result_t();
}

result_t types_base::isMap(v8::Local<v8::Value> v, bool& retVal)
{
    retVal = v->IsMap();
    return result_t();
}

result_t types_base::isMapIterator(v8::Local<v8::Value> v, bool& retVal)
{
    retVal = v->IsMapIterator();
    return result_t();
}

result_t types_base::isPromise(v8::Local<v8::Value> v, bool& retVal)
{
    retVal = v->IsPromise();
    return result_t();
}

result_t types_base::isAsyncFunction(v8::Local<v8::Value> v, bool& retVal)
{
    retVal = v->IsAsyncFunction();
    return result_t();
}

result_t types_base::isSet(v8::Local<v8::Value> v, bool& retVal)
{
    retVal = v->IsSet();
    return result_t();
}

result_t types_base::isSetIterator(v8::Local<v8::Value> v, bool& retVal)
{
    retVal = v->IsSetIterator();
    return result_t();
}

result_t types_base::isTypedArray(v8::Local<v8::Value> v, bool& retVal)
{
    retVal = v->IsTypedArray();
    return result_t();
}

result_t types_base::isFloat32Array(v8::Local<v8::Value> v, bool& retVal)
{
    retVal = v->IsFloat32Array();
    return result_t();
}

result_t types_base::isFloat64Array(v8::Local<v8::Value> v, bool& retVal)
{
    retVal = v->IsFloat64Array();
    return result_t();
}

result_t types_base::isInt8Array(v8::Local<v8::Value> v, bool& retVal)
{
    retVal = v->IsInt8Array();
    return result_t();
}

result_t types_base::isInt16Array(v8::Local<v8::Value> v, bool& retVal)
{
    retVal = v->IsInt16Array();
    return result_t();
}

result_t types_base::isInt32Array(v8::Local<v8::Value> v, bool& retVal)
{
    retVal = v->IsInt32Array();
    return result_t();
}

result_t types_base::isUint8Array(v8::Local<v8::Value> v, bool& retVal)
{
    retVal = v->IsUint8Array();
    return result_t();
}

result_t types_base::isUint8ClampedArray(v8::Local<v8::Value> v, bool& retVal)
{
    retVal = v->IsUint8ClampedArray();
    return result_t();
}

result_t types_base::isUint16Array(v8::Local<v8::Value> v, bool& retVal)
{
    retVal = v->IsUint16Array();
    return result_t();
}

result_t types_base::isUint32Array(v8::Local<v8::Value> v, bool& retVal)
{
    retVal = v->IsUint32Array();
    return result_t();
}

result_t types_base::isFunction(v8::Local<v8::Value> v, bool& retVal)
{
    retVal = v->IsFunction();
    return result_t();
}

result_t types_base::isBuffer(v8::Local<v8::Value> v, bool& retVal)
{
    retVal = IsJSBuffer(v);
    return result_t();
}

result_t types_base::isBigIntObject(v8::Local<v8::Value> v, bool& retVal)
{
    retVal = v->IsBigIntObject();
    return result_t();
}

result_t types_base::isBooleanObject(v8::Local<v8::Value> v, bool& retVal)
{
    retVal = v->IsBooleanObject();
    return result_t();
}

result_t types_base::isNumberObject(v8::Local<v8::Value> v, bool& retVal)
{
    retVal = v->IsNumberObject();
    return result_t();
}

result_t types_base::isStringObject(v8::Local<v8::Value> v, bool& retVal)
{
    retVal = v->IsStringObject();
    return result_t();
}

result_t types_base::isSymbolObject(v8::Local<v8::Value> v, bool& retVal)
{
    retVal = v->IsSymbolObject();
    return result_t();
}

result_t types_base::isWeakMap(v8::Local<v8::Value> v, bool& retVal)
{
    retVal = v->IsWeakMap();
    return result_t();
}

result_t types_base::isWeakSet(v8::Local<v8::Value> v, bool& retVal)
{
    retVal = v->IsWeakSet();
    return result_t();
}

result_t types_base::isArrayBuffer(v8::Local<v8::Value> v, bool& retVal)
{
    retVal = v->IsArrayBuffer();
    return result_t();
}

result_t types_base::isArrayBufferView(v8::Local<v8::Value> v, bool& retVal)
{
    retVal = v->IsArrayBufferView();
    return result_t();
}

result_t types_base::isBigInt64Array(v8::Local<v8::Value> v, bool& retVal)
{
    retVal = v->IsBigInt64Array();
    return result_t();
}

result_t types_base::isBigUint64Array(v8::Local<v8::Value> v, bool& retVal)
{
    retVal = v->IsBigUint64Array();
    return result_t();
}

result_t types_base::isFloat16Array(v8::Local<v8::Value> v, bool& retVal)
{
    retVal = v->IsFloat16Array();
    return result_t();
}

result_t types_base::isAnyArrayBuffer(v8::Local<v8::Value> v, bool& retVal)
{
    retVal = v->IsArrayBuffer() || v->IsSharedArrayBuffer();
    return result_t();
}

result_t types_base::isSharedArrayBuffer(v8::Local<v8::Value> v, bool& retVal)
{
    retVal = v->IsSharedArrayBuffer();
    return result_t();
}

result_t types_base::isArgumentsObject(v8::Local<v8::Value> v, bool& retVal)
{
    retVal = v->IsArgumentsObject();
    return result_t();
}

result_t types_base::isBoxedPrimitive(v8::Local<v8::Value> v, bool& retVal)
{
    retVal = v->IsBooleanObject() || v->IsNumberObject() || v->IsStringObject()
        || v->IsSymbolObject() || v->IsBigIntObject();
    return result_t();
}

result_t types_base::isGeneratorFunction(v8::Local<v8::Value> v, bool& retVal)
{
    retVal = v->IsGeneratorFunction();
    return result_t();
}

result_t types_base::isGeneratorObject(v8::Local<v8::Value> v, bool& retVal)
{
    retVal = v->IsGeneratorObject();
    return result_t();
}

result_t types_base::isProxy(v8::Local<v8::Value> v, bool& retVal)
{
    retVal = v->IsProxy();
    return result_t();
}

result_t types_base::isModuleNamespaceObject(v8::Local<v8::Value> v, bool& retVal)
{
    retVal = v->IsModuleNamespaceObject();
    return result_t();
}

result_t types_base::isCryptoKey(v8::Local<v8::Value> v, bool& retVal)
{
    retVal = CryptoKey_base::getInstance(v) != NULL;
    return result_t();
}

result_t types_base::isKeyObject(v8::Local<v8::Value> v, bool& retVal)
{
    retVal = KeyObject_base::getInstance(v) != NULL;
    return result_t();
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

result_t util_base::isFloat16Array(v8::Local<v8::Value> v, bool& retVal)
{
    return types_base::isFloat16Array(v, retVal);
}

result_t util_base::isAnyArrayBuffer(v8::Local<v8::Value> v, bool& retVal)
{
    return types_base::isAnyArrayBuffer(v, retVal);
}

result_t util_base::isSharedArrayBuffer(v8::Local<v8::Value> v, bool& retVal)
{
    return types_base::isSharedArrayBuffer(v, retVal);
}

result_t util_base::isArgumentsObject(v8::Local<v8::Value> v, bool& retVal)
{
    return types_base::isArgumentsObject(v, retVal);
}

result_t util_base::isBoxedPrimitive(v8::Local<v8::Value> v, bool& retVal)
{
    return types_base::isBoxedPrimitive(v, retVal);
}

result_t util_base::isGeneratorFunction(v8::Local<v8::Value> v, bool& retVal)
{
    return types_base::isGeneratorFunction(v, retVal);
}

result_t util_base::isGeneratorObject(v8::Local<v8::Value> v, bool& retVal)
{
    return types_base::isGeneratorObject(v, retVal);
}

result_t util_base::isProxy(v8::Local<v8::Value> v, bool& retVal)
{
    return types_base::isProxy(v, retVal);
}

result_t util_base::isModuleNamespaceObject(v8::Local<v8::Value> v, bool& retVal)
{
    return types_base::isModuleNamespaceObject(v, retVal);
}

result_t util_base::isCryptoKey(v8::Local<v8::Value> v, bool& retVal)
{
    return types_base::isCryptoKey(v, retVal);
}

result_t util_base::isKeyObject(v8::Local<v8::Value> v, bool& retVal)
{
    return types_base::isKeyObject(v, retVal);
}

}
