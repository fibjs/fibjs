/***************************************************************************
 *                                                                         *
 *   This file was automatically generated using idlc.js                   *
 *   PLEASE DO NOT EDIT!!!!                                                *
 *                                                                         *
 ***************************************************************************/

#pragma once

/**
 @author Leo Hoo <lion@9465.net>
 */

#include "../object.h"

namespace fibjs {

class types_base : public object_base {
    DECLARE_CLASS(types_base);

public:
    // types_base
    static result_t isEmpty(v8::Local<v8::Value> v, bool& retVal);
    static result_t isArray(v8::Local<v8::Value> v, bool& retVal);
    static result_t isBoolean(v8::Local<v8::Value> v, bool& retVal);
    static result_t isNull(v8::Local<v8::Value> v, bool& retVal);
    static result_t isNullOrUndefined(v8::Local<v8::Value> v, bool& retVal);
    static result_t isNumber(v8::Local<v8::Value> v, bool& retVal);
    static result_t isBigInt(v8::Local<v8::Value> v, bool& retVal);
    static result_t isString(v8::Local<v8::Value> v, bool& retVal);
    static result_t isUndefined(v8::Local<v8::Value> v, bool& retVal);
    static result_t isRegExp(v8::Local<v8::Value> v, bool& retVal);
    static result_t isObject(v8::Local<v8::Value> v, bool& retVal);
    static result_t isDate(v8::Local<v8::Value> v, bool& retVal);
    static result_t isNativeError(v8::Local<v8::Value> v, bool& retVal);
    static result_t isPrimitive(v8::Local<v8::Value> v, bool& retVal);
    static result_t isSymbol(v8::Local<v8::Value> v, bool& retVal);
    static result_t isDataView(v8::Local<v8::Value> v, bool& retVal);
    static result_t isExternal(v8::Local<v8::Value> v, bool& retVal);
    static result_t isMap(v8::Local<v8::Value> v, bool& retVal);
    static result_t isMapIterator(v8::Local<v8::Value> v, bool& retVal);
    static result_t isPromise(v8::Local<v8::Value> v, bool& retVal);
    static result_t isAsyncFunction(v8::Local<v8::Value> v, bool& retVal);
    static result_t isSet(v8::Local<v8::Value> v, bool& retVal);
    static result_t isSetIterator(v8::Local<v8::Value> v, bool& retVal);
    static result_t isTypedArray(v8::Local<v8::Value> v, bool& retVal);
    static result_t isFloat32Array(v8::Local<v8::Value> v, bool& retVal);
    static result_t isFloat64Array(v8::Local<v8::Value> v, bool& retVal);
    static result_t isInt8Array(v8::Local<v8::Value> v, bool& retVal);
    static result_t isInt16Array(v8::Local<v8::Value> v, bool& retVal);
    static result_t isInt32Array(v8::Local<v8::Value> v, bool& retVal);
    static result_t isUint8Array(v8::Local<v8::Value> v, bool& retVal);
    static result_t isUint8ClampedArray(v8::Local<v8::Value> v, bool& retVal);
    static result_t isUint16Array(v8::Local<v8::Value> v, bool& retVal);
    static result_t isUint32Array(v8::Local<v8::Value> v, bool& retVal);
    static result_t isFunction(v8::Local<v8::Value> v, bool& retVal);
    static result_t isBuffer(v8::Local<v8::Value> v, bool& retVal);

public:
    static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        CONSTRUCT_INIT();

        isolate->m_isolate->ThrowException(
            isolate->NewString("not a constructor"));
    }

public:
    static void s_static_isEmpty(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_isArray(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_isBoolean(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_isNull(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_isNullOrUndefined(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_isNumber(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_isBigInt(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_isString(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_isUndefined(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_isRegExp(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_isObject(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_isDate(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_isNativeError(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_isPrimitive(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_isSymbol(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_isDataView(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_isExternal(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_isMap(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_isMapIterator(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_isPromise(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_isAsyncFunction(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_isSet(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_isSetIterator(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_isTypedArray(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_isFloat32Array(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_isFloat64Array(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_isInt8Array(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_isInt16Array(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_isInt32Array(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_isUint8Array(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_isUint8ClampedArray(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_isUint16Array(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_isUint32Array(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_isFunction(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_isBuffer(const v8::FunctionCallbackInfo<v8::Value>& args);
};
}

namespace fibjs {
inline ClassInfo& types_base::class_info()
{
    static ClassData::ClassMethod s_method[] = {
        { "isEmpty", s_static_isEmpty, true, false },
        { "isArray", s_static_isArray, true, false },
        { "isBoolean", s_static_isBoolean, true, false },
        { "isNull", s_static_isNull, true, false },
        { "isNullOrUndefined", s_static_isNullOrUndefined, true, false },
        { "isNumber", s_static_isNumber, true, false },
        { "isBigInt", s_static_isBigInt, true, false },
        { "isString", s_static_isString, true, false },
        { "isUndefined", s_static_isUndefined, true, false },
        { "isRegExp", s_static_isRegExp, true, false },
        { "isObject", s_static_isObject, true, false },
        { "isDate", s_static_isDate, true, false },
        { "isNativeError", s_static_isNativeError, true, false },
        { "isPrimitive", s_static_isPrimitive, true, false },
        { "isSymbol", s_static_isSymbol, true, false },
        { "isDataView", s_static_isDataView, true, false },
        { "isExternal", s_static_isExternal, true, false },
        { "isMap", s_static_isMap, true, false },
        { "isMapIterator", s_static_isMapIterator, true, false },
        { "isPromise", s_static_isPromise, true, false },
        { "isAsyncFunction", s_static_isAsyncFunction, true, false },
        { "isSet", s_static_isSet, true, false },
        { "isSetIterator", s_static_isSetIterator, true, false },
        { "isTypedArray", s_static_isTypedArray, true, false },
        { "isFloat32Array", s_static_isFloat32Array, true, false },
        { "isFloat64Array", s_static_isFloat64Array, true, false },
        { "isInt8Array", s_static_isInt8Array, true, false },
        { "isInt16Array", s_static_isInt16Array, true, false },
        { "isInt32Array", s_static_isInt32Array, true, false },
        { "isUint8Array", s_static_isUint8Array, true, false },
        { "isUint8ClampedArray", s_static_isUint8ClampedArray, true, false },
        { "isUint16Array", s_static_isUint16Array, true, false },
        { "isUint32Array", s_static_isUint32Array, true, false },
        { "isFunction", s_static_isFunction, true, false },
        { "isBuffer", s_static_isBuffer, true, false }
    };

    static ClassData s_cd = {
        "types", true, s__new, NULL,
        ARRAYSIZE(s_method), s_method, 0, NULL, 0, NULL, 0, NULL, NULL, NULL,
        &object_base::class_info(),
        false
    };

    static ClassInfo s_ci(s_cd);
    return s_ci;
}

inline void types_base::s_static_isEmpty(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    bool vr;

    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(v8::Local<v8::Value>, 0);

    hr = isEmpty(v0, vr);

    METHOD_RETURN();
}

inline void types_base::s_static_isArray(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    bool vr;

    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(v8::Local<v8::Value>, 0);

    hr = isArray(v0, vr);

    METHOD_RETURN();
}

inline void types_base::s_static_isBoolean(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    bool vr;

    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(v8::Local<v8::Value>, 0);

    hr = isBoolean(v0, vr);

    METHOD_RETURN();
}

inline void types_base::s_static_isNull(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    bool vr;

    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(v8::Local<v8::Value>, 0);

    hr = isNull(v0, vr);

    METHOD_RETURN();
}

inline void types_base::s_static_isNullOrUndefined(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    bool vr;

    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(v8::Local<v8::Value>, 0);

    hr = isNullOrUndefined(v0, vr);

    METHOD_RETURN();
}

inline void types_base::s_static_isNumber(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    bool vr;

    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(v8::Local<v8::Value>, 0);

    hr = isNumber(v0, vr);

    METHOD_RETURN();
}

inline void types_base::s_static_isBigInt(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    bool vr;

    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(v8::Local<v8::Value>, 0);

    hr = isBigInt(v0, vr);

    METHOD_RETURN();
}

inline void types_base::s_static_isString(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    bool vr;

    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(v8::Local<v8::Value>, 0);

    hr = isString(v0, vr);

    METHOD_RETURN();
}

inline void types_base::s_static_isUndefined(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    bool vr;

    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(v8::Local<v8::Value>, 0);

    hr = isUndefined(v0, vr);

    METHOD_RETURN();
}

inline void types_base::s_static_isRegExp(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    bool vr;

    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(v8::Local<v8::Value>, 0);

    hr = isRegExp(v0, vr);

    METHOD_RETURN();
}

inline void types_base::s_static_isObject(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    bool vr;

    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(v8::Local<v8::Value>, 0);

    hr = isObject(v0, vr);

    METHOD_RETURN();
}

inline void types_base::s_static_isDate(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    bool vr;

    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(v8::Local<v8::Value>, 0);

    hr = isDate(v0, vr);

    METHOD_RETURN();
}

inline void types_base::s_static_isNativeError(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    bool vr;

    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(v8::Local<v8::Value>, 0);

    hr = isNativeError(v0, vr);

    METHOD_RETURN();
}

inline void types_base::s_static_isPrimitive(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    bool vr;

    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(v8::Local<v8::Value>, 0);

    hr = isPrimitive(v0, vr);

    METHOD_RETURN();
}

inline void types_base::s_static_isSymbol(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    bool vr;

    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(v8::Local<v8::Value>, 0);

    hr = isSymbol(v0, vr);

    METHOD_RETURN();
}

inline void types_base::s_static_isDataView(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    bool vr;

    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(v8::Local<v8::Value>, 0);

    hr = isDataView(v0, vr);

    METHOD_RETURN();
}

inline void types_base::s_static_isExternal(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    bool vr;

    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(v8::Local<v8::Value>, 0);

    hr = isExternal(v0, vr);

    METHOD_RETURN();
}

inline void types_base::s_static_isMap(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    bool vr;

    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(v8::Local<v8::Value>, 0);

    hr = isMap(v0, vr);

    METHOD_RETURN();
}

inline void types_base::s_static_isMapIterator(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    bool vr;

    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(v8::Local<v8::Value>, 0);

    hr = isMapIterator(v0, vr);

    METHOD_RETURN();
}

inline void types_base::s_static_isPromise(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    bool vr;

    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(v8::Local<v8::Value>, 0);

    hr = isPromise(v0, vr);

    METHOD_RETURN();
}

inline void types_base::s_static_isAsyncFunction(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    bool vr;

    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(v8::Local<v8::Value>, 0);

    hr = isAsyncFunction(v0, vr);

    METHOD_RETURN();
}

inline void types_base::s_static_isSet(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    bool vr;

    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(v8::Local<v8::Value>, 0);

    hr = isSet(v0, vr);

    METHOD_RETURN();
}

inline void types_base::s_static_isSetIterator(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    bool vr;

    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(v8::Local<v8::Value>, 0);

    hr = isSetIterator(v0, vr);

    METHOD_RETURN();
}

inline void types_base::s_static_isTypedArray(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    bool vr;

    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(v8::Local<v8::Value>, 0);

    hr = isTypedArray(v0, vr);

    METHOD_RETURN();
}

inline void types_base::s_static_isFloat32Array(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    bool vr;

    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(v8::Local<v8::Value>, 0);

    hr = isFloat32Array(v0, vr);

    METHOD_RETURN();
}

inline void types_base::s_static_isFloat64Array(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    bool vr;

    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(v8::Local<v8::Value>, 0);

    hr = isFloat64Array(v0, vr);

    METHOD_RETURN();
}

inline void types_base::s_static_isInt8Array(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    bool vr;

    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(v8::Local<v8::Value>, 0);

    hr = isInt8Array(v0, vr);

    METHOD_RETURN();
}

inline void types_base::s_static_isInt16Array(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    bool vr;

    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(v8::Local<v8::Value>, 0);

    hr = isInt16Array(v0, vr);

    METHOD_RETURN();
}

inline void types_base::s_static_isInt32Array(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    bool vr;

    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(v8::Local<v8::Value>, 0);

    hr = isInt32Array(v0, vr);

    METHOD_RETURN();
}

inline void types_base::s_static_isUint8Array(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    bool vr;

    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(v8::Local<v8::Value>, 0);

    hr = isUint8Array(v0, vr);

    METHOD_RETURN();
}

inline void types_base::s_static_isUint8ClampedArray(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    bool vr;

    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(v8::Local<v8::Value>, 0);

    hr = isUint8ClampedArray(v0, vr);

    METHOD_RETURN();
}

inline void types_base::s_static_isUint16Array(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    bool vr;

    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(v8::Local<v8::Value>, 0);

    hr = isUint16Array(v0, vr);

    METHOD_RETURN();
}

inline void types_base::s_static_isUint32Array(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    bool vr;

    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(v8::Local<v8::Value>, 0);

    hr = isUint32Array(v0, vr);

    METHOD_RETURN();
}

inline void types_base::s_static_isFunction(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    bool vr;

    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(v8::Local<v8::Value>, 0);

    hr = isFunction(v0, vr);

    METHOD_RETURN();
}

inline void types_base::s_static_isBuffer(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    bool vr;

    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(v8::Local<v8::Value>, 0);

    hr = isBuffer(v0, vr);

    METHOD_RETURN();
}
}
