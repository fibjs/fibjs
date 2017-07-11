/***************************************************************************
 *                                                                         *
 *   This file was automatically generated using idlc.js                   *
 *   PLEASE DO NOT EDIT!!!!                                                *
 *                                                                         *
 ***************************************************************************/

#ifndef _util_base_H_
#define _util_base_H_

/**
 @author Leo Hoo <lion@9465.net>
 */

#include "../object.h"

namespace fibjs {

class Stats_base;
class LruCache_base;
class Buffer_base;

class util_base : public object_base {
    DECLARE_CLASS(util_base);

public:
    // util_base
    static result_t format(exlib::string fmt, const v8::FunctionCallbackInfo<v8::Value>& args, exlib::string& retVal);
    static result_t format(const v8::FunctionCallbackInfo<v8::Value>& args, exlib::string& retVal);
    static result_t inherits(v8::Local<v8::Value> constructor, v8::Local<v8::Value> superConstructor);
    static result_t isEmpty(v8::Local<v8::Value> v, bool& retVal);
    static result_t isArray(v8::Local<v8::Value> v, bool& retVal);
    static result_t isBoolean(v8::Local<v8::Value> v, bool& retVal);
    static result_t isNull(v8::Local<v8::Value> v, bool& retVal);
    static result_t isNullOrUndefined(v8::Local<v8::Value> v, bool& retVal);
    static result_t isNumber(v8::Local<v8::Value> v, bool& retVal);
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
    static result_t isUint8Array(v8::Local<v8::Value> v, bool& retVal);
    static result_t isFunction(v8::Local<v8::Value> v, bool& retVal);
    static result_t isBuffer(v8::Local<v8::Value> v, bool& retVal);
    static result_t has(v8::Local<v8::Value> v, exlib::string key, bool& retVal);
    static result_t keys(v8::Local<v8::Value> v, v8::Local<v8::Array>& retVal);
    static result_t values(v8::Local<v8::Value> v, v8::Local<v8::Array>& retVal);
    static result_t clone(v8::Local<v8::Value> v, v8::Local<v8::Value>& retVal);
    static result_t extend(v8::Local<v8::Value> v, const v8::FunctionCallbackInfo<v8::Value>& args, v8::Local<v8::Value>& retVal);
    static result_t pick(v8::Local<v8::Value> v, const v8::FunctionCallbackInfo<v8::Value>& args, v8::Local<v8::Object>& retVal);
    static result_t omit(v8::Local<v8::Value> v, const v8::FunctionCallbackInfo<v8::Value>& args, v8::Local<v8::Object>& retVal);
    static result_t first(v8::Local<v8::Value> v, v8::Local<v8::Value>& retVal);
    static result_t first(v8::Local<v8::Value> v, int32_t n, v8::Local<v8::Value>& retVal);
    static result_t last(v8::Local<v8::Value> v, v8::Local<v8::Value>& retVal);
    static result_t last(v8::Local<v8::Value> v, int32_t n, v8::Local<v8::Value>& retVal);
    static result_t unique(v8::Local<v8::Value> v, bool sorted, v8::Local<v8::Array>& retVal);
    static result_t _union(const v8::FunctionCallbackInfo<v8::Value>& args, v8::Local<v8::Array>& retVal);
    static result_t intersection(const v8::FunctionCallbackInfo<v8::Value>& args, v8::Local<v8::Array>& retVal);
    static result_t flatten(v8::Local<v8::Value> arr, bool shallow, v8::Local<v8::Array>& retVal);
    static result_t without(v8::Local<v8::Value> arr, const v8::FunctionCallbackInfo<v8::Value>& args, v8::Local<v8::Array>& retVal);
    static result_t difference(v8::Local<v8::Array> list, const v8::FunctionCallbackInfo<v8::Value>& args, v8::Local<v8::Array>& retVal);
    static result_t each(v8::Local<v8::Value> list, v8::Local<v8::Function> iterator, v8::Local<v8::Value> context, v8::Local<v8::Value>& retVal);
    static result_t map(v8::Local<v8::Value> list, v8::Local<v8::Function> iterator, v8::Local<v8::Value> context, v8::Local<v8::Array>& retVal);
    static result_t reduce(v8::Local<v8::Value> list, v8::Local<v8::Function> iterator, v8::Local<v8::Value> memo, v8::Local<v8::Value> context, v8::Local<v8::Value>& retVal);
    static result_t compile(exlib::string srcname, exlib::string script, int32_t mode, obj_ptr<Buffer_base>& retVal);
    static result_t sync(v8::Local<v8::Function> func, bool async_func, v8::Local<v8::Function>& retVal);
    static result_t buildInfo(v8::Local<v8::Object>& retVal);

public:
    static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        CONSTRUCT_INIT();

        Isolate* isolate = Isolate::current();

        isolate->m_isolate->ThrowException(
            isolate->NewFromUtf8("not a constructor"));
    }

public:
    static void s_format(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_inherits(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_isEmpty(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_isArray(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_isBoolean(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_isNull(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_isNullOrUndefined(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_isNumber(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_isString(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_isUndefined(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_isRegExp(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_isObject(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_isDate(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_isNativeError(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_isPrimitive(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_isSymbol(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_isDataView(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_isExternal(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_isMap(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_isMapIterator(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_isPromise(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_isAsyncFunction(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_isSet(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_isSetIterator(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_isTypedArray(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_isUint8Array(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_isFunction(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_isBuffer(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_has(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_keys(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_values(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_clone(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_extend(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_pick(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_omit(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_first(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_last(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_unique(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_union(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_intersection(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_flatten(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_without(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_difference(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_each(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_map(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_reduce(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_compile(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_sync(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_buildInfo(const v8::FunctionCallbackInfo<v8::Value>& args);
};
}

#include "Stats.h"
#include "LruCache.h"
#include "Buffer.h"

namespace fibjs {
inline ClassInfo& util_base::class_info()
{
    static ClassData::ClassMethod s_method[] = {
        { "format", s_format, true },
        { "inherits", s_inherits, true },
        { "isEmpty", s_isEmpty, true },
        { "isArray", s_isArray, true },
        { "isBoolean", s_isBoolean, true },
        { "isNull", s_isNull, true },
        { "isNullOrUndefined", s_isNullOrUndefined, true },
        { "isNumber", s_isNumber, true },
        { "isString", s_isString, true },
        { "isUndefined", s_isUndefined, true },
        { "isRegExp", s_isRegExp, true },
        { "isObject", s_isObject, true },
        { "isDate", s_isDate, true },
        { "isNativeError", s_isNativeError, true },
        { "isPrimitive", s_isPrimitive, true },
        { "isSymbol", s_isSymbol, true },
        { "isDataView", s_isDataView, true },
        { "isExternal", s_isExternal, true },
        { "isMap", s_isMap, true },
        { "isMapIterator", s_isMapIterator, true },
        { "isPromise", s_isPromise, true },
        { "isAsyncFunction", s_isAsyncFunction, true },
        { "isSet", s_isSet, true },
        { "isSetIterator", s_isSetIterator, true },
        { "isTypedArray", s_isTypedArray, true },
        { "isUint8Array", s_isUint8Array, true },
        { "isFunction", s_isFunction, true },
        { "isBuffer", s_isBuffer, true },
        { "has", s_has, true },
        { "keys", s_keys, true },
        { "values", s_values, true },
        { "clone", s_clone, true },
        { "extend", s_extend, true },
        { "pick", s_pick, true },
        { "omit", s_omit, true },
        { "first", s_first, true },
        { "last", s_last, true },
        { "unique", s_unique, true },
        { "union", s_union, true },
        { "intersection", s_intersection, true },
        { "flatten", s_flatten, true },
        { "without", s_without, true },
        { "difference", s_difference, true },
        { "each", s_each, true },
        { "map", s_map, true },
        { "reduce", s_reduce, true },
        { "compile", s_compile, true },
        { "sync", s_sync, true },
        { "buildInfo", s_buildInfo, true }
    };

    static ClassData::ClassObject s_object[] = {
        { "Stats", Stats_base::class_info },
        { "LruCache", LruCache_base::class_info }
    };

    static ClassData s_cd = {
        "util", true, s__new, NULL,
        ARRAYSIZE(s_method), s_method, ARRAYSIZE(s_object), s_object, 0, NULL, NULL, NULL,
        &object_base::class_info()
    };

    static ClassInfo s_ci(s_cd);
    return s_ci;
}

inline void util_base::s_format(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_ENTER();

    METHOD_OVER(-1, 1);

    ARG(exlib::string, 0);

    hr = format(v0, args, vr);

    METHOD_OVER(-1, 0);

    hr = format(args, vr);

    METHOD_RETURN();
}

inline void util_base::s_inherits(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_ENTER();

    METHOD_OVER(2, 2);

    ARG(v8::Local<v8::Value>, 0);
    ARG(v8::Local<v8::Value>, 1);

    hr = inherits(v0, v1);

    METHOD_VOID();
}

inline void util_base::s_isEmpty(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    bool vr;

    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(v8::Local<v8::Value>, 0);

    hr = isEmpty(v0, vr);

    METHOD_RETURN();
}

inline void util_base::s_isArray(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    bool vr;

    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(v8::Local<v8::Value>, 0);

    hr = isArray(v0, vr);

    METHOD_RETURN();
}

inline void util_base::s_isBoolean(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    bool vr;

    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(v8::Local<v8::Value>, 0);

    hr = isBoolean(v0, vr);

    METHOD_RETURN();
}

inline void util_base::s_isNull(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    bool vr;

    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(v8::Local<v8::Value>, 0);

    hr = isNull(v0, vr);

    METHOD_RETURN();
}

inline void util_base::s_isNullOrUndefined(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    bool vr;

    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(v8::Local<v8::Value>, 0);

    hr = isNullOrUndefined(v0, vr);

    METHOD_RETURN();
}

inline void util_base::s_isNumber(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    bool vr;

    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(v8::Local<v8::Value>, 0);

    hr = isNumber(v0, vr);

    METHOD_RETURN();
}

inline void util_base::s_isString(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    bool vr;

    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(v8::Local<v8::Value>, 0);

    hr = isString(v0, vr);

    METHOD_RETURN();
}

inline void util_base::s_isUndefined(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    bool vr;

    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(v8::Local<v8::Value>, 0);

    hr = isUndefined(v0, vr);

    METHOD_RETURN();
}

inline void util_base::s_isRegExp(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    bool vr;

    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(v8::Local<v8::Value>, 0);

    hr = isRegExp(v0, vr);

    METHOD_RETURN();
}

inline void util_base::s_isObject(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    bool vr;

    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(v8::Local<v8::Value>, 0);

    hr = isObject(v0, vr);

    METHOD_RETURN();
}

inline void util_base::s_isDate(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    bool vr;

    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(v8::Local<v8::Value>, 0);

    hr = isDate(v0, vr);

    METHOD_RETURN();
}

inline void util_base::s_isNativeError(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    bool vr;

    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(v8::Local<v8::Value>, 0);

    hr = isNativeError(v0, vr);

    METHOD_RETURN();
}

inline void util_base::s_isPrimitive(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    bool vr;

    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(v8::Local<v8::Value>, 0);

    hr = isPrimitive(v0, vr);

    METHOD_RETURN();
}

inline void util_base::s_isSymbol(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    bool vr;

    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(v8::Local<v8::Value>, 0);

    hr = isSymbol(v0, vr);

    METHOD_RETURN();
}

inline void util_base::s_isDataView(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    bool vr;

    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(v8::Local<v8::Value>, 0);

    hr = isDataView(v0, vr);

    METHOD_RETURN();
}

inline void util_base::s_isExternal(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    bool vr;

    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(v8::Local<v8::Value>, 0);

    hr = isExternal(v0, vr);

    METHOD_RETURN();
}

inline void util_base::s_isMap(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    bool vr;

    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(v8::Local<v8::Value>, 0);

    hr = isMap(v0, vr);

    METHOD_RETURN();
}

inline void util_base::s_isMapIterator(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    bool vr;

    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(v8::Local<v8::Value>, 0);

    hr = isMapIterator(v0, vr);

    METHOD_RETURN();
}

inline void util_base::s_isPromise(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    bool vr;

    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(v8::Local<v8::Value>, 0);

    hr = isPromise(v0, vr);

    METHOD_RETURN();
}

inline void util_base::s_isAsyncFunction(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    bool vr;

    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(v8::Local<v8::Value>, 0);

    hr = isAsyncFunction(v0, vr);

    METHOD_RETURN();
}

inline void util_base::s_isSet(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    bool vr;

    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(v8::Local<v8::Value>, 0);

    hr = isSet(v0, vr);

    METHOD_RETURN();
}

inline void util_base::s_isSetIterator(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    bool vr;

    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(v8::Local<v8::Value>, 0);

    hr = isSetIterator(v0, vr);

    METHOD_RETURN();
}

inline void util_base::s_isTypedArray(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    bool vr;

    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(v8::Local<v8::Value>, 0);

    hr = isTypedArray(v0, vr);

    METHOD_RETURN();
}

inline void util_base::s_isUint8Array(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    bool vr;

    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(v8::Local<v8::Value>, 0);

    hr = isUint8Array(v0, vr);

    METHOD_RETURN();
}

inline void util_base::s_isFunction(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    bool vr;

    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(v8::Local<v8::Value>, 0);

    hr = isFunction(v0, vr);

    METHOD_RETURN();
}

inline void util_base::s_isBuffer(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    bool vr;

    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(v8::Local<v8::Value>, 0);

    hr = isBuffer(v0, vr);

    METHOD_RETURN();
}

inline void util_base::s_has(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    bool vr;

    METHOD_ENTER();

    METHOD_OVER(2, 2);

    ARG(v8::Local<v8::Value>, 0);
    ARG(exlib::string, 1);

    hr = has(v0, v1, vr);

    METHOD_RETURN();
}

inline void util_base::s_keys(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Array> vr;

    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(v8::Local<v8::Value>, 0);

    hr = keys(v0, vr);

    METHOD_RETURN();
}

inline void util_base::s_values(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Array> vr;

    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(v8::Local<v8::Value>, 0);

    hr = values(v0, vr);

    METHOD_RETURN();
}

inline void util_base::s_clone(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Value> vr;

    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(v8::Local<v8::Value>, 0);

    hr = clone(v0, vr);

    METHOD_RETURN();
}

inline void util_base::s_extend(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Value> vr;

    METHOD_ENTER();

    METHOD_OVER(-1, 1);

    ARG(v8::Local<v8::Value>, 0);

    hr = extend(v0, args, vr);

    METHOD_RETURN();
}

inline void util_base::s_pick(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Object> vr;

    METHOD_ENTER();

    METHOD_OVER(-1, 1);

    ARG(v8::Local<v8::Value>, 0);

    hr = pick(v0, args, vr);

    METHOD_RETURN();
}

inline void util_base::s_omit(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Object> vr;

    METHOD_ENTER();

    METHOD_OVER(-1, 1);

    ARG(v8::Local<v8::Value>, 0);

    hr = omit(v0, args, vr);

    METHOD_RETURN();
}

inline void util_base::s_first(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Value> vr;

    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(v8::Local<v8::Value>, 0);

    hr = first(v0, vr);

    METHOD_OVER(2, 2);

    ARG(v8::Local<v8::Value>, 0);
    ARG(int32_t, 1);

    hr = first(v0, v1, vr);

    METHOD_RETURN();
}

inline void util_base::s_last(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Value> vr;

    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(v8::Local<v8::Value>, 0);

    hr = last(v0, vr);

    METHOD_OVER(2, 2);

    ARG(v8::Local<v8::Value>, 0);
    ARG(int32_t, 1);

    hr = last(v0, v1, vr);

    METHOD_RETURN();
}

inline void util_base::s_unique(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Array> vr;

    METHOD_ENTER();

    METHOD_OVER(2, 1);

    ARG(v8::Local<v8::Value>, 0);
    OPT_ARG(bool, 1, false);

    hr = unique(v0, v1, vr);

    METHOD_RETURN();
}

inline void util_base::s_union(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Array> vr;

    METHOD_ENTER();

    METHOD_OVER(-1, 0);

    hr = _union(args, vr);

    METHOD_RETURN();
}

inline void util_base::s_intersection(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Array> vr;

    METHOD_ENTER();

    METHOD_OVER(-1, 0);

    hr = intersection(args, vr);

    METHOD_RETURN();
}

inline void util_base::s_flatten(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Array> vr;

    METHOD_ENTER();

    METHOD_OVER(2, 1);

    ARG(v8::Local<v8::Value>, 0);
    OPT_ARG(bool, 1, false);

    hr = flatten(v0, v1, vr);

    METHOD_RETURN();
}

inline void util_base::s_without(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Array> vr;

    METHOD_ENTER();

    METHOD_OVER(-1, 1);

    ARG(v8::Local<v8::Value>, 0);

    hr = without(v0, args, vr);

    METHOD_RETURN();
}

inline void util_base::s_difference(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Array> vr;

    METHOD_ENTER();

    METHOD_OVER(-1, 1);

    ARG(v8::Local<v8::Array>, 0);

    hr = difference(v0, args, vr);

    METHOD_RETURN();
}

inline void util_base::s_each(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Value> vr;

    METHOD_ENTER();

    METHOD_OVER(3, 2);

    ARG(v8::Local<v8::Value>, 0);
    ARG(v8::Local<v8::Function>, 1);
    OPT_ARG(v8::Local<v8::Value>, 2, v8::Undefined(isolate));

    hr = each(v0, v1, v2, vr);

    METHOD_RETURN();
}

inline void util_base::s_map(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Array> vr;

    METHOD_ENTER();

    METHOD_OVER(3, 2);

    ARG(v8::Local<v8::Value>, 0);
    ARG(v8::Local<v8::Function>, 1);
    OPT_ARG(v8::Local<v8::Value>, 2, v8::Undefined(isolate));

    hr = map(v0, v1, v2, vr);

    METHOD_RETURN();
}

inline void util_base::s_reduce(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Value> vr;

    METHOD_ENTER();

    METHOD_OVER(4, 3);

    ARG(v8::Local<v8::Value>, 0);
    ARG(v8::Local<v8::Function>, 1);
    ARG(v8::Local<v8::Value>, 2);
    OPT_ARG(v8::Local<v8::Value>, 3, v8::Undefined(isolate));

    hr = reduce(v0, v1, v2, v3, vr);

    METHOD_RETURN();
}

inline void util_base::s_compile(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<Buffer_base> vr;

    METHOD_ENTER();

    METHOD_OVER(3, 2);

    ARG(exlib::string, 0);
    ARG(exlib::string, 1);
    OPT_ARG(int32_t, 2, 0);

    hr = compile(v0, v1, v2, vr);

    METHOD_RETURN();
}

inline void util_base::s_sync(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Function> vr;

    METHOD_ENTER();

    METHOD_OVER(2, 1);

    ARG(v8::Local<v8::Function>, 0);
    OPT_ARG(bool, 1, false);

    hr = sync(v0, v1, vr);

    METHOD_RETURN();
}

inline void util_base::s_buildInfo(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Object> vr;

    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = buildInfo(vr);

    METHOD_RETURN();
}
}

#endif
