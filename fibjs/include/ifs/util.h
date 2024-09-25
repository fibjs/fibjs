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

class TextDecoder_base;
class TextEncoder_base;
class types_base;
class Logger_base;
class Buffer_base;

class util_base : public object_base {
    DECLARE_CLASS(util_base);

public:
    // util_base
    static result_t format(exlib::string fmt, OptArgs args, exlib::string& retVal);
    static result_t format(OptArgs args, exlib::string& retVal);
    static result_t inherits(v8::Local<v8::Value> constructor, v8::Local<v8::Value> superConstructor);
    static result_t inspect(v8::Local<v8::Value> obj, v8::Local<v8::Object> options, exlib::string& retVal);
    static result_t debuglog(exlib::string section, obj_ptr<Logger_base>& retVal);
    static result_t debuglog(exlib::string section, v8::Local<v8::Function> fn, obj_ptr<Logger_base>& retVal);
    static result_t debug(exlib::string section, obj_ptr<Logger_base>& retVal);
    static result_t debug(exlib::string section, v8::Local<v8::Function> fn, obj_ptr<Logger_base>& retVal);
    static result_t deprecate(v8::Local<v8::Function> fn, exlib::string msg, exlib::string code, v8::Local<v8::Function>& retVal);
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
    static result_t isUint8Array(v8::Local<v8::Value> v, bool& retVal);
    static result_t isFunction(v8::Local<v8::Value> v, bool& retVal);
    static result_t isBuffer(v8::Local<v8::Value> v, bool& retVal);
    static result_t isDeepEqual(v8::Local<v8::Value> actual, v8::Local<v8::Value> expected, bool& retVal);
    static result_t has(v8::Local<v8::Value> v, exlib::string key, bool& retVal);
    static result_t keys(v8::Local<v8::Value> v, v8::Local<v8::Array>& retVal);
    static result_t values(v8::Local<v8::Value> v, v8::Local<v8::Array>& retVal);
    static result_t clone(v8::Local<v8::Value> v, v8::Local<v8::Value>& retVal);
    static result_t deepFreeze(v8::Local<v8::Value> v);
    static result_t extend(v8::Local<v8::Value> v, OptArgs objs, v8::Local<v8::Value>& retVal);
    static result_t _extend(v8::Local<v8::Value> v, OptArgs objs, v8::Local<v8::Value>& retVal);
    static result_t pick(v8::Local<v8::Value> v, OptArgs objs, v8::Local<v8::Object>& retVal);
    static result_t omit(v8::Local<v8::Value> v, OptArgs keys, v8::Local<v8::Object>& retVal);
    static result_t first(v8::Local<v8::Value> v, v8::Local<v8::Value>& retVal);
    static result_t first(v8::Local<v8::Value> v, int32_t n, v8::Local<v8::Value>& retVal);
    static result_t last(v8::Local<v8::Value> v, v8::Local<v8::Value>& retVal);
    static result_t last(v8::Local<v8::Value> v, int32_t n, v8::Local<v8::Value>& retVal);
    static result_t unique(v8::Local<v8::Value> v, bool sorted, v8::Local<v8::Array>& retVal);
    static result_t _union(OptArgs arrs, v8::Local<v8::Array>& retVal);
    static result_t intersection(OptArgs arrs, v8::Local<v8::Array>& retVal);
    static result_t flatten(v8::Local<v8::Value> arr, bool shallow, v8::Local<v8::Array>& retVal);
    static result_t without(v8::Local<v8::Value> arr, OptArgs els, v8::Local<v8::Array>& retVal);
    static result_t difference(v8::Local<v8::Array> list, OptArgs arrs, v8::Local<v8::Array>& retVal);
    static result_t each(v8::Local<v8::Value> list, v8::Local<v8::Function> iterator, v8::Local<v8::Value> context, v8::Local<v8::Value>& retVal);
    static result_t map(v8::Local<v8::Value> list, v8::Local<v8::Function> iterator, v8::Local<v8::Value> context, v8::Local<v8::Array>& retVal);
    static result_t reduce(v8::Local<v8::Value> list, v8::Local<v8::Function> iterator, v8::Local<v8::Value> memo, v8::Local<v8::Value> context, v8::Local<v8::Value>& retVal);
    static result_t parseArgs(exlib::string command, obj_ptr<NArray>& retVal);
    static result_t compile(exlib::string srcname, exlib::string script, int32_t mode, obj_ptr<Buffer_base>& retVal);
    static result_t sync(v8::Local<v8::Function> func, bool async_func, v8::Local<v8::Function>& retVal);
    static result_t promisify(v8::Local<v8::Function> func, v8::Local<v8::Function>& retVal);
    static result_t callbackify(v8::Local<v8::Function> func, v8::Local<v8::Function>& retVal);
    static result_t buildInfo(v8::Local<v8::Object>& retVal);

public:
    static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        CONSTRUCT_INIT();

        isolate->m_isolate->ThrowException(
            isolate->NewString("not a constructor"));
    }

public:
    static void s_static_format(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_inherits(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_inspect(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_debuglog(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_debug(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_deprecate(const v8::FunctionCallbackInfo<v8::Value>& args);
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
    static void s_static_isUint8Array(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_isFunction(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_isBuffer(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_isDeepEqual(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_has(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_keys(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_values(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_clone(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_deepFreeze(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_extend(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static__extend(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_pick(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_omit(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_first(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_last(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_unique(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static__union(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_intersection(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_flatten(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_without(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_difference(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_each(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_map(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_reduce(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_parseArgs(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_compile(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_sync(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_promisify(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_callbackify(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_buildInfo(const v8::FunctionCallbackInfo<v8::Value>& args);
};
}

#include "ifs/TextDecoder.h"
#include "ifs/TextEncoder.h"
#include "ifs/types.h"
#include "ifs/Logger.h"
#include "ifs/Buffer.h"

namespace fibjs {
inline ClassInfo& util_base::class_info()
{
    static ClassData::ClassMethod s_method[] = {
        { "format", s_static_format, true, ClassData::ASYNC_SYNC },
        { "inherits", s_static_inherits, true, ClassData::ASYNC_SYNC },
        { "inspect", s_static_inspect, true, ClassData::ASYNC_SYNC },
        { "debuglog", s_static_debuglog, true, ClassData::ASYNC_SYNC },
        { "debug", s_static_debug, true, ClassData::ASYNC_SYNC },
        { "deprecate", s_static_deprecate, true, ClassData::ASYNC_SYNC },
        { "isEmpty", s_static_isEmpty, true, ClassData::ASYNC_SYNC },
        { "isArray", s_static_isArray, true, ClassData::ASYNC_SYNC },
        { "isBoolean", s_static_isBoolean, true, ClassData::ASYNC_SYNC },
        { "isNull", s_static_isNull, true, ClassData::ASYNC_SYNC },
        { "isNullOrUndefined", s_static_isNullOrUndefined, true, ClassData::ASYNC_SYNC },
        { "isNumber", s_static_isNumber, true, ClassData::ASYNC_SYNC },
        { "isBigInt", s_static_isBigInt, true, ClassData::ASYNC_SYNC },
        { "isString", s_static_isString, true, ClassData::ASYNC_SYNC },
        { "isUndefined", s_static_isUndefined, true, ClassData::ASYNC_SYNC },
        { "isRegExp", s_static_isRegExp, true, ClassData::ASYNC_SYNC },
        { "isObject", s_static_isObject, true, ClassData::ASYNC_SYNC },
        { "isDate", s_static_isDate, true, ClassData::ASYNC_SYNC },
        { "isNativeError", s_static_isNativeError, true, ClassData::ASYNC_SYNC },
        { "isPrimitive", s_static_isPrimitive, true, ClassData::ASYNC_SYNC },
        { "isSymbol", s_static_isSymbol, true, ClassData::ASYNC_SYNC },
        { "isDataView", s_static_isDataView, true, ClassData::ASYNC_SYNC },
        { "isExternal", s_static_isExternal, true, ClassData::ASYNC_SYNC },
        { "isMap", s_static_isMap, true, ClassData::ASYNC_SYNC },
        { "isMapIterator", s_static_isMapIterator, true, ClassData::ASYNC_SYNC },
        { "isPromise", s_static_isPromise, true, ClassData::ASYNC_SYNC },
        { "isAsyncFunction", s_static_isAsyncFunction, true, ClassData::ASYNC_SYNC },
        { "isSet", s_static_isSet, true, ClassData::ASYNC_SYNC },
        { "isSetIterator", s_static_isSetIterator, true, ClassData::ASYNC_SYNC },
        { "isTypedArray", s_static_isTypedArray, true, ClassData::ASYNC_SYNC },
        { "isUint8Array", s_static_isUint8Array, true, ClassData::ASYNC_SYNC },
        { "isFunction", s_static_isFunction, true, ClassData::ASYNC_SYNC },
        { "isBuffer", s_static_isBuffer, true, ClassData::ASYNC_SYNC },
        { "isDeepEqual", s_static_isDeepEqual, true, ClassData::ASYNC_SYNC },
        { "has", s_static_has, true, ClassData::ASYNC_SYNC },
        { "keys", s_static_keys, true, ClassData::ASYNC_SYNC },
        { "values", s_static_values, true, ClassData::ASYNC_SYNC },
        { "clone", s_static_clone, true, ClassData::ASYNC_SYNC },
        { "deepFreeze", s_static_deepFreeze, true, ClassData::ASYNC_SYNC },
        { "extend", s_static_extend, true, ClassData::ASYNC_SYNC },
        { "_extend", s_static__extend, true, ClassData::ASYNC_SYNC },
        { "pick", s_static_pick, true, ClassData::ASYNC_SYNC },
        { "omit", s_static_omit, true, ClassData::ASYNC_SYNC },
        { "first", s_static_first, true, ClassData::ASYNC_SYNC },
        { "last", s_static_last, true, ClassData::ASYNC_SYNC },
        { "unique", s_static_unique, true, ClassData::ASYNC_SYNC },
        { "union", s_static__union, true, ClassData::ASYNC_SYNC },
        { "intersection", s_static_intersection, true, ClassData::ASYNC_SYNC },
        { "flatten", s_static_flatten, true, ClassData::ASYNC_SYNC },
        { "without", s_static_without, true, ClassData::ASYNC_SYNC },
        { "difference", s_static_difference, true, ClassData::ASYNC_SYNC },
        { "each", s_static_each, true, ClassData::ASYNC_SYNC },
        { "map", s_static_map, true, ClassData::ASYNC_SYNC },
        { "reduce", s_static_reduce, true, ClassData::ASYNC_SYNC },
        { "parseArgs", s_static_parseArgs, true, ClassData::ASYNC_SYNC },
        { "compile", s_static_compile, true, ClassData::ASYNC_SYNC },
        { "sync", s_static_sync, true, ClassData::ASYNC_SYNC },
        { "promisify", s_static_promisify, true, ClassData::ASYNC_SYNC },
        { "callbackify", s_static_callbackify, true, ClassData::ASYNC_SYNC },
        { "buildInfo", s_static_buildInfo, true, ClassData::ASYNC_SYNC }
    };

    static ClassData::ClassObject s_object[] = {
        { "TextDecoder", TextDecoder_base::class_info },
        { "TextEncoder", TextEncoder_base::class_info },
        { "types", types_base::class_info }
    };

    static ClassData s_cd = {
        "util", true, s__new, NULL,
        ARRAYSIZE(s_method), s_method, ARRAYSIZE(s_object), s_object, 0, NULL, 0, NULL, NULL, NULL,
        &object_base::class_info(),
        false
    };

    static ClassInfo s_ci(s_cd);
    return s_ci;
}

inline void util_base::s_static_format(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_ENTER();

    METHOD_OVER(-1, 1);

    ARG(exlib::string, 0);
    ARG_LIST(1);

    hr = format(v0, v1, vr);

    METHOD_OVER(-1, 0);

    ARG_LIST(0);

    hr = format(v0, vr);

    METHOD_RETURN();
}

inline void util_base::s_static_inherits(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_ENTER();

    METHOD_OVER(2, 2);

    ARG(v8::Local<v8::Value>, 0);
    ARG(v8::Local<v8::Value>, 1);

    hr = inherits(v0, v1);

    METHOD_VOID();
}

inline void util_base::s_static_inspect(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_ENTER();

    METHOD_OVER(2, 1);

    ARG(v8::Local<v8::Value>, 0);
    OPT_ARG(v8::Local<v8::Object>, 1, v8::Object::New(isolate->m_isolate));

    hr = inspect(v0, v1, vr);

    METHOD_RETURN();
}

inline void util_base::s_static_debuglog(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<Logger_base> vr;

    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(exlib::string, 0);

    hr = debuglog(v0, vr);

    METHOD_OVER(2, 2);

    ARG(exlib::string, 0);
    ARG(v8::Local<v8::Function>, 1);

    hr = debuglog(v0, v1, vr);

    METHOD_RETURN();
}

inline void util_base::s_static_debug(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<Logger_base> vr;

    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(exlib::string, 0);

    hr = debug(v0, vr);

    METHOD_OVER(2, 2);

    ARG(exlib::string, 0);
    ARG(v8::Local<v8::Function>, 1);

    hr = debug(v0, v1, vr);

    METHOD_RETURN();
}

inline void util_base::s_static_deprecate(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Function> vr;

    METHOD_ENTER();

    METHOD_OVER(3, 2);

    ARG(v8::Local<v8::Function>, 0);
    ARG(exlib::string, 1);
    OPT_ARG(exlib::string, 2, "");

    hr = deprecate(v0, v1, v2, vr);

    METHOD_RETURN();
}

inline void util_base::s_static_isEmpty(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    bool vr;

    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(v8::Local<v8::Value>, 0);

    hr = isEmpty(v0, vr);

    METHOD_RETURN();
}

inline void util_base::s_static_isArray(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    bool vr;

    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(v8::Local<v8::Value>, 0);

    hr = isArray(v0, vr);

    METHOD_RETURN();
}

inline void util_base::s_static_isBoolean(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    bool vr;

    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(v8::Local<v8::Value>, 0);

    hr = isBoolean(v0, vr);

    METHOD_RETURN();
}

inline void util_base::s_static_isNull(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    bool vr;

    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(v8::Local<v8::Value>, 0);

    hr = isNull(v0, vr);

    METHOD_RETURN();
}

inline void util_base::s_static_isNullOrUndefined(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    bool vr;

    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(v8::Local<v8::Value>, 0);

    hr = isNullOrUndefined(v0, vr);

    METHOD_RETURN();
}

inline void util_base::s_static_isNumber(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    bool vr;

    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(v8::Local<v8::Value>, 0);

    hr = isNumber(v0, vr);

    METHOD_RETURN();
}

inline void util_base::s_static_isBigInt(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    bool vr;

    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(v8::Local<v8::Value>, 0);

    hr = isBigInt(v0, vr);

    METHOD_RETURN();
}

inline void util_base::s_static_isString(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    bool vr;

    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(v8::Local<v8::Value>, 0);

    hr = isString(v0, vr);

    METHOD_RETURN();
}

inline void util_base::s_static_isUndefined(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    bool vr;

    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(v8::Local<v8::Value>, 0);

    hr = isUndefined(v0, vr);

    METHOD_RETURN();
}

inline void util_base::s_static_isRegExp(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    bool vr;

    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(v8::Local<v8::Value>, 0);

    hr = isRegExp(v0, vr);

    METHOD_RETURN();
}

inline void util_base::s_static_isObject(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    bool vr;

    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(v8::Local<v8::Value>, 0);

    hr = isObject(v0, vr);

    METHOD_RETURN();
}

inline void util_base::s_static_isDate(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    bool vr;

    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(v8::Local<v8::Value>, 0);

    hr = isDate(v0, vr);

    METHOD_RETURN();
}

inline void util_base::s_static_isNativeError(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    bool vr;

    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(v8::Local<v8::Value>, 0);

    hr = isNativeError(v0, vr);

    METHOD_RETURN();
}

inline void util_base::s_static_isPrimitive(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    bool vr;

    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(v8::Local<v8::Value>, 0);

    hr = isPrimitive(v0, vr);

    METHOD_RETURN();
}

inline void util_base::s_static_isSymbol(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    bool vr;

    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(v8::Local<v8::Value>, 0);

    hr = isSymbol(v0, vr);

    METHOD_RETURN();
}

inline void util_base::s_static_isDataView(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    bool vr;

    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(v8::Local<v8::Value>, 0);

    hr = isDataView(v0, vr);

    METHOD_RETURN();
}

inline void util_base::s_static_isExternal(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    bool vr;

    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(v8::Local<v8::Value>, 0);

    hr = isExternal(v0, vr);

    METHOD_RETURN();
}

inline void util_base::s_static_isMap(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    bool vr;

    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(v8::Local<v8::Value>, 0);

    hr = isMap(v0, vr);

    METHOD_RETURN();
}

inline void util_base::s_static_isMapIterator(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    bool vr;

    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(v8::Local<v8::Value>, 0);

    hr = isMapIterator(v0, vr);

    METHOD_RETURN();
}

inline void util_base::s_static_isPromise(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    bool vr;

    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(v8::Local<v8::Value>, 0);

    hr = isPromise(v0, vr);

    METHOD_RETURN();
}

inline void util_base::s_static_isAsyncFunction(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    bool vr;

    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(v8::Local<v8::Value>, 0);

    hr = isAsyncFunction(v0, vr);

    METHOD_RETURN();
}

inline void util_base::s_static_isSet(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    bool vr;

    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(v8::Local<v8::Value>, 0);

    hr = isSet(v0, vr);

    METHOD_RETURN();
}

inline void util_base::s_static_isSetIterator(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    bool vr;

    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(v8::Local<v8::Value>, 0);

    hr = isSetIterator(v0, vr);

    METHOD_RETURN();
}

inline void util_base::s_static_isTypedArray(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    bool vr;

    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(v8::Local<v8::Value>, 0);

    hr = isTypedArray(v0, vr);

    METHOD_RETURN();
}

inline void util_base::s_static_isUint8Array(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    bool vr;

    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(v8::Local<v8::Value>, 0);

    hr = isUint8Array(v0, vr);

    METHOD_RETURN();
}

inline void util_base::s_static_isFunction(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    bool vr;

    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(v8::Local<v8::Value>, 0);

    hr = isFunction(v0, vr);

    METHOD_RETURN();
}

inline void util_base::s_static_isBuffer(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    bool vr;

    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(v8::Local<v8::Value>, 0);

    hr = isBuffer(v0, vr);

    METHOD_RETURN();
}

inline void util_base::s_static_isDeepEqual(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    bool vr;

    METHOD_ENTER();

    METHOD_OVER(2, 2);

    ARG(v8::Local<v8::Value>, 0);
    ARG(v8::Local<v8::Value>, 1);

    hr = isDeepEqual(v0, v1, vr);

    METHOD_RETURN();
}

inline void util_base::s_static_has(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    bool vr;

    METHOD_ENTER();

    METHOD_OVER(2, 2);

    ARG(v8::Local<v8::Value>, 0);
    ARG(exlib::string, 1);

    hr = has(v0, v1, vr);

    METHOD_RETURN();
}

inline void util_base::s_static_keys(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Array> vr;

    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(v8::Local<v8::Value>, 0);

    hr = keys(v0, vr);

    METHOD_RETURN();
}

inline void util_base::s_static_values(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Array> vr;

    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(v8::Local<v8::Value>, 0);

    hr = values(v0, vr);

    METHOD_RETURN();
}

inline void util_base::s_static_clone(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Value> vr;

    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(v8::Local<v8::Value>, 0);

    hr = clone(v0, vr);

    METHOD_RETURN();
}

inline void util_base::s_static_deepFreeze(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(v8::Local<v8::Value>, 0);

    hr = deepFreeze(v0);

    METHOD_VOID();
}

inline void util_base::s_static_extend(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Value> vr;

    METHOD_ENTER();

    METHOD_OVER(-1, 1);

    ARG(v8::Local<v8::Value>, 0);
    ARG_LIST(1);

    hr = extend(v0, v1, vr);

    METHOD_RETURN();
}

inline void util_base::s_static__extend(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Value> vr;

    METHOD_ENTER();

    METHOD_OVER(-1, 1);

    ARG(v8::Local<v8::Value>, 0);
    ARG_LIST(1);

    hr = _extend(v0, v1, vr);

    METHOD_RETURN();
}

inline void util_base::s_static_pick(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Object> vr;

    METHOD_ENTER();

    METHOD_OVER(-1, 1);

    ARG(v8::Local<v8::Value>, 0);
    ARG_LIST(1);

    hr = pick(v0, v1, vr);

    METHOD_RETURN();
}

inline void util_base::s_static_omit(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Object> vr;

    METHOD_ENTER();

    METHOD_OVER(-1, 1);

    ARG(v8::Local<v8::Value>, 0);
    ARG_LIST(1);

    hr = omit(v0, v1, vr);

    METHOD_RETURN();
}

inline void util_base::s_static_first(const v8::FunctionCallbackInfo<v8::Value>& args)
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

inline void util_base::s_static_last(const v8::FunctionCallbackInfo<v8::Value>& args)
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

inline void util_base::s_static_unique(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Array> vr;

    METHOD_ENTER();

    METHOD_OVER(2, 1);

    ARG(v8::Local<v8::Value>, 0);
    OPT_ARG(bool, 1, false);

    hr = unique(v0, v1, vr);

    METHOD_RETURN();
}

inline void util_base::s_static__union(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Array> vr;

    METHOD_ENTER();

    METHOD_OVER(-1, 0);

    ARG_LIST(0);

    hr = _union(v0, vr);

    METHOD_RETURN();
}

inline void util_base::s_static_intersection(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Array> vr;

    METHOD_ENTER();

    METHOD_OVER(-1, 0);

    ARG_LIST(0);

    hr = intersection(v0, vr);

    METHOD_RETURN();
}

inline void util_base::s_static_flatten(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Array> vr;

    METHOD_ENTER();

    METHOD_OVER(2, 1);

    ARG(v8::Local<v8::Value>, 0);
    OPT_ARG(bool, 1, false);

    hr = flatten(v0, v1, vr);

    METHOD_RETURN();
}

inline void util_base::s_static_without(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Array> vr;

    METHOD_ENTER();

    METHOD_OVER(-1, 1);

    ARG(v8::Local<v8::Value>, 0);
    ARG_LIST(1);

    hr = without(v0, v1, vr);

    METHOD_RETURN();
}

inline void util_base::s_static_difference(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Array> vr;

    METHOD_ENTER();

    METHOD_OVER(-1, 1);

    ARG(v8::Local<v8::Array>, 0);
    ARG_LIST(1);

    hr = difference(v0, v1, vr);

    METHOD_RETURN();
}

inline void util_base::s_static_each(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Value> vr;

    METHOD_ENTER();

    METHOD_OVER(3, 2);

    ARG(v8::Local<v8::Value>, 0);
    ARG(v8::Local<v8::Function>, 1);
    OPT_ARG(v8::Local<v8::Value>, 2, v8::Undefined(isolate->m_isolate));

    hr = each(v0, v1, v2, vr);

    METHOD_RETURN();
}

inline void util_base::s_static_map(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Array> vr;

    METHOD_ENTER();

    METHOD_OVER(3, 2);

    ARG(v8::Local<v8::Value>, 0);
    ARG(v8::Local<v8::Function>, 1);
    OPT_ARG(v8::Local<v8::Value>, 2, v8::Undefined(isolate->m_isolate));

    hr = map(v0, v1, v2, vr);

    METHOD_RETURN();
}

inline void util_base::s_static_reduce(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Value> vr;

    METHOD_ENTER();

    METHOD_OVER(4, 3);

    ARG(v8::Local<v8::Value>, 0);
    ARG(v8::Local<v8::Function>, 1);
    ARG(v8::Local<v8::Value>, 2);
    OPT_ARG(v8::Local<v8::Value>, 3, v8::Undefined(isolate->m_isolate));

    hr = reduce(v0, v1, v2, v3, vr);

    METHOD_RETURN();
}

inline void util_base::s_static_parseArgs(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<NArray> vr;

    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(exlib::string, 0);

    hr = parseArgs(v0, vr);

    METHOD_RETURN();
}

inline void util_base::s_static_compile(const v8::FunctionCallbackInfo<v8::Value>& args)
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

inline void util_base::s_static_sync(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Function> vr;

    METHOD_ENTER();

    METHOD_OVER(2, 1);

    ARG(v8::Local<v8::Function>, 0);
    OPT_ARG(bool, 1, false);

    hr = sync(v0, v1, vr);

    METHOD_RETURN();
}

inline void util_base::s_static_promisify(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Function> vr;

    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(v8::Local<v8::Function>, 0);

    hr = promisify(v0, vr);

    METHOD_RETURN();
}

inline void util_base::s_static_callbackify(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Function> vr;

    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(v8::Local<v8::Function>, 0);

    hr = callbackify(v0, vr);

    METHOD_RETURN();
}

inline void util_base::s_static_buildInfo(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Object> vr;

    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = buildInfo(vr);

    METHOD_RETURN();
}
}
