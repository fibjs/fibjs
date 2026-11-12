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
class colors_base;
class ConsoleObject_base;
class Buffer_base;
class AbortSignal_base;

class util_base : public object_base {
    DECLARE_CLASS(util_base);

public:
    // util_base
    static result_t format(OptArgs args, exlib::string& retVal);
    static result_t formatWithOptions(v8::Local<v8::Object> options, v8::Local<v8::Value> fmt, OptArgs args, exlib::string& retVal);
    static result_t inherits(v8::Local<v8::Value> constructor, v8::Local<v8::Value> superConstructor);
    static result_t parseEnv(exlib::string content, v8::Local<v8::Object>& retVal);
    static result_t inspect(v8::Local<v8::Value> obj, v8::Local<v8::Object> options, exlib::string& retVal);
    static result_t styleText(std::vector<exlib::string>& format, exlib::string text, exlib::string& retVal);
    static result_t styleText(exlib::string format, exlib::string text, exlib::string& retVal);
    static result_t debuglog(exlib::string section, obj_ptr<ConsoleObject_base>& retVal);
    static result_t debuglog(exlib::string section, v8::Local<v8::Function> fn, obj_ptr<ConsoleObject_base>& retVal);
    static result_t debug(exlib::string section, obj_ptr<ConsoleObject_base>& retVal);
    static result_t debug(exlib::string section, v8::Local<v8::Function> fn, obj_ptr<ConsoleObject_base>& retVal);
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
    static result_t isFloat16Array(v8::Local<v8::Value> v, bool& retVal);
    static result_t isAnyArrayBuffer(v8::Local<v8::Value> v, bool& retVal);
    static result_t isSharedArrayBuffer(v8::Local<v8::Value> v, bool& retVal);
    static result_t isArgumentsObject(v8::Local<v8::Value> v, bool& retVal);
    static result_t isBoxedPrimitive(v8::Local<v8::Value> v, bool& retVal);
    static result_t isGeneratorFunction(v8::Local<v8::Value> v, bool& retVal);
    static result_t isGeneratorObject(v8::Local<v8::Value> v, bool& retVal);
    static result_t isProxy(v8::Local<v8::Value> v, bool& retVal);
    static result_t isModuleNamespaceObject(v8::Local<v8::Value> v, bool& retVal);
    static result_t isCryptoKey(v8::Local<v8::Value> v, bool& retVal);
    static result_t isKeyObject(v8::Local<v8::Value> v, bool& retVal);
    static result_t isDeepEqual(v8::Local<v8::Value> actual, v8::Local<v8::Value> expected, bool& retVal);
    static result_t isDeepStrictEqual(v8::Local<v8::Value> actual, v8::Local<v8::Value> expected, bool& retVal);
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
    static result_t stripTypeScript(exlib::string code, exlib::string& retVal);
    static result_t getStringWidth(exlib::string str, int32_t& retVal);
    static result_t getSystemErrorName(int32_t err, exlib::string& retVal);
    static result_t getSystemErrorMessage(int32_t err, exlib::string& retVal);
    static result_t getSystemErrorMap(v8::Local<v8::Value>& retVal);
    static result_t stripVTControlCharacters(exlib::string str, exlib::string& retVal);
    static result_t aborted(AbortSignal_base* signal, v8::Local<v8::Object> resource, AsyncHandle ac);

public:
    static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        CONSTRUCT_INIT();

        ThrowTypeError("not a constructor");
    }

    static result_t load(v8::Local<v8::Value> v, obj_ptr<util_base>& retVal)
    { return result_t::type_mismatch(); }

public:
    static void s_static_format(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_formatWithOptions(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_inherits(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_parseEnv(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_inspect(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_styleText(const v8::FunctionCallbackInfo<v8::Value>& args);
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
    static void s_static_isFloat16Array(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_isAnyArrayBuffer(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_isSharedArrayBuffer(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_isArgumentsObject(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_isBoxedPrimitive(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_isGeneratorFunction(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_isGeneratorObject(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_isProxy(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_isModuleNamespaceObject(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_isCryptoKey(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_isKeyObject(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_isDeepEqual(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_isDeepStrictEqual(const v8::FunctionCallbackInfo<v8::Value>& args);
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
    static void s_static_stripTypeScript(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_getStringWidth(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_getSystemErrorName(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_getSystemErrorMessage(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_getSystemErrorMap(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_stripVTControlCharacters(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_aborted(const v8::FunctionCallbackInfo<v8::Value>& args);

public:
    ASYNC_STATIC2(util_base, aborted, AbortSignal_base*, v8::Local<v8::Object>);
};
}

#include "ifs/TextDecoder.h"
#include "ifs/TextEncoder.h"
#include "ifs/types.h"
#include "ifs/colors.h"
#include "ifs/ConsoleObject.h"
#include "ifs/Buffer.h"
#include "ifs/AbortSignal.h"

namespace fibjs {
inline ClassInfo& util_base::class_info()
{
    static ClassData::ClassMethod s_method[] = {
        { "format", s_static_format, true, ClassData::ASYNC_SYNC },
        { "formatWithOptions", s_static_formatWithOptions, true, ClassData::ASYNC_SYNC },
        { "inherits", s_static_inherits, true, ClassData::ASYNC_SYNC },
        { "parseEnv", s_static_parseEnv, true, ClassData::ASYNC_SYNC },
        { "inspect", s_static_inspect, true, ClassData::ASYNC_SYNC },
        { "styleText", s_static_styleText, true, ClassData::ASYNC_SYNC },
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
        { "isFloat16Array", s_static_isFloat16Array, true, ClassData::ASYNC_SYNC },
        { "isAnyArrayBuffer", s_static_isAnyArrayBuffer, true, ClassData::ASYNC_SYNC },
        { "isSharedArrayBuffer", s_static_isSharedArrayBuffer, true, ClassData::ASYNC_SYNC },
        { "isArgumentsObject", s_static_isArgumentsObject, true, ClassData::ASYNC_SYNC },
        { "isBoxedPrimitive", s_static_isBoxedPrimitive, true, ClassData::ASYNC_SYNC },
        { "isGeneratorFunction", s_static_isGeneratorFunction, true, ClassData::ASYNC_SYNC },
        { "isGeneratorObject", s_static_isGeneratorObject, true, ClassData::ASYNC_SYNC },
        { "isProxy", s_static_isProxy, true, ClassData::ASYNC_SYNC },
        { "isModuleNamespaceObject", s_static_isModuleNamespaceObject, true, ClassData::ASYNC_SYNC },
        { "isCryptoKey", s_static_isCryptoKey, true, ClassData::ASYNC_SYNC },
        { "isKeyObject", s_static_isKeyObject, true, ClassData::ASYNC_SYNC },
        { "isDeepEqual", s_static_isDeepEqual, true, ClassData::ASYNC_SYNC },
        { "isDeepStrictEqual", s_static_isDeepStrictEqual, true, ClassData::ASYNC_SYNC },
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
        { "buildInfo", s_static_buildInfo, true, ClassData::ASYNC_SYNC },
        { "stripTypeScript", s_static_stripTypeScript, true, ClassData::ASYNC_SYNC },
        { "getStringWidth", s_static_getStringWidth, true, ClassData::ASYNC_SYNC },
        { "getSystemErrorName", s_static_getSystemErrorName, true, ClassData::ASYNC_SYNC },
        { "getSystemErrorMessage", s_static_getSystemErrorMessage, true, ClassData::ASYNC_SYNC },
        { "getSystemErrorMap", s_static_getSystemErrorMap, true, ClassData::ASYNC_SYNC },
        { "stripVTControlCharacters", s_static_stripVTControlCharacters, true, ClassData::ASYNC_SYNC },
        { "aborted", s_static_aborted, true, ClassData::ASYNC_PROMISE }
    };

    static ClassData::ClassObject s_object[] = {
        { "TextDecoder", TextDecoder_base::class_info },
        { "TextEncoder", TextEncoder_base::class_info },
        { "types", types_base::class_info },
        { "colors", colors_base::class_info }
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

    METHOD_OVER(-1, 0);

    ARG_LIST(0);

    hr = format(v0, vr);

    METHOD_RETURN();
}

inline void util_base::s_static_formatWithOptions(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_ENTER();

    METHOD_OVER_NAMED(-1, 2, "fmt");

    ARG_NAMED(v8::Local<v8::Object>, 0, "options");
    ARG_NAMED(v8::Local<v8::Value>, 1, "fmt");
    ARG_LIST(2);

    hr = formatWithOptions(v0, v1, v2, vr);

    METHOD_RETURN();
}

inline void util_base::s_static_inherits(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_ENTER();

    METHOD_OVER_NAMED(2, 2, "superConstructor");

    ARG_NAMED(v8::Local<v8::Value>, 0, "constructor");
    ARG_NAMED(v8::Local<v8::Value>, 1, "superConstructor");

    hr = inherits(v0, v1);

    METHOD_VOID();
}

inline void util_base::s_static_parseEnv(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Object> vr;

    METHOD_ENTER();

    METHOD_OVER_NAMED(1, 1, "content");

    ARG_NAMED(exlib::string, 0, "content");

    hr = parseEnv(v0, vr);

    METHOD_RETURN();
}

inline void util_base::s_static_inspect(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_ENTER();

    METHOD_OVER_NAMED(2, 1, "obj");

    ARG_NAMED(v8::Local<v8::Value>, 0, "obj");
    OPT_ARG_NAMED(v8::Local<v8::Object>, 1, v8::Object::New(isolate->m_isolate), "options");

    hr = inspect(v0, v1, vr);

    METHOD_RETURN();
}

inline void util_base::s_static_styleText(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_ENTER();

    METHOD_OVER_NAMED(2, 2, "text");

    ARG_NAMED(std::vector<exlib::string>, 0, "format");
    ARG_NAMED(exlib::string, 1, "text");

    hr = styleText(v0, v1, vr);

    METHOD_OVER_NAMED(2, 2, "text");

    ARG_NAMED(exlib::string, 0, "format");
    ARG_NAMED(exlib::string, 1, "text");

    hr = styleText(v0, v1, vr);

    METHOD_RETURN();
}

inline void util_base::s_static_debuglog(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<ConsoleObject_base> vr;

    METHOD_ENTER();

    METHOD_OVER_NAMED(1, 1, "section");

    ARG_NAMED(exlib::string, 0, "section");

    hr = debuglog(v0, vr);

    METHOD_OVER_NAMED(2, 2, "fn");

    ARG_NAMED(exlib::string, 0, "section");
    ARG_NAMED(v8::Local<v8::Function>, 1, "fn");

    hr = debuglog(v0, v1, vr);

    METHOD_RETURN();
}

inline void util_base::s_static_debug(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<ConsoleObject_base> vr;

    METHOD_ENTER();

    METHOD_OVER_NAMED(1, 1, "section");

    ARG_NAMED(exlib::string, 0, "section");

    hr = debug(v0, vr);

    METHOD_OVER_NAMED(2, 2, "fn");

    ARG_NAMED(exlib::string, 0, "section");
    ARG_NAMED(v8::Local<v8::Function>, 1, "fn");

    hr = debug(v0, v1, vr);

    METHOD_RETURN();
}

inline void util_base::s_static_deprecate(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Function> vr;

    METHOD_ENTER();

    METHOD_OVER_NAMED(3, 2, "msg");

    ARG_NAMED(v8::Local<v8::Function>, 0, "fn");
    ARG_NAMED(exlib::string, 1, "msg");
    OPT_ARG_NAMED(exlib::string, 2, "", "code");

    hr = deprecate(v0, v1, v2, vr);

    METHOD_RETURN();
}

inline void util_base::s_static_isEmpty(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    bool vr;

    METHOD_ENTER();

    METHOD_OVER_NAMED(1, 1, "v");

    ARG_NAMED(v8::Local<v8::Value>, 0, "v");

    hr = isEmpty(v0, vr);

    METHOD_RETURN();
}

inline void util_base::s_static_isArray(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    bool vr;

    METHOD_ENTER();

    METHOD_OVER_NAMED(1, 1, "v");

    ARG_NAMED(v8::Local<v8::Value>, 0, "v");

    hr = isArray(v0, vr);

    METHOD_RETURN();
}

inline void util_base::s_static_isBoolean(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    bool vr;

    METHOD_ENTER();

    METHOD_OVER_NAMED(1, 1, "v");

    ARG_NAMED(v8::Local<v8::Value>, 0, "v");

    hr = isBoolean(v0, vr);

    METHOD_RETURN();
}

inline void util_base::s_static_isNull(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    bool vr;

    METHOD_ENTER();

    METHOD_OVER_NAMED(1, 1, "v");

    ARG_NAMED(v8::Local<v8::Value>, 0, "v");

    hr = isNull(v0, vr);

    METHOD_RETURN();
}

inline void util_base::s_static_isNullOrUndefined(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    bool vr;

    METHOD_ENTER();

    METHOD_OVER_NAMED(1, 1, "v");

    ARG_NAMED(v8::Local<v8::Value>, 0, "v");

    hr = isNullOrUndefined(v0, vr);

    METHOD_RETURN();
}

inline void util_base::s_static_isNumber(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    bool vr;

    METHOD_ENTER();

    METHOD_OVER_NAMED(1, 1, "v");

    ARG_NAMED(v8::Local<v8::Value>, 0, "v");

    hr = isNumber(v0, vr);

    METHOD_RETURN();
}

inline void util_base::s_static_isBigInt(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    bool vr;

    METHOD_ENTER();

    METHOD_OVER_NAMED(1, 1, "v");

    ARG_NAMED(v8::Local<v8::Value>, 0, "v");

    hr = isBigInt(v0, vr);

    METHOD_RETURN();
}

inline void util_base::s_static_isString(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    bool vr;

    METHOD_ENTER();

    METHOD_OVER_NAMED(1, 1, "v");

    ARG_NAMED(v8::Local<v8::Value>, 0, "v");

    hr = isString(v0, vr);

    METHOD_RETURN();
}

inline void util_base::s_static_isUndefined(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    bool vr;

    METHOD_ENTER();

    METHOD_OVER_NAMED(1, 1, "v");

    ARG_NAMED(v8::Local<v8::Value>, 0, "v");

    hr = isUndefined(v0, vr);

    METHOD_RETURN();
}

inline void util_base::s_static_isRegExp(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    bool vr;

    METHOD_ENTER();

    METHOD_OVER_NAMED(1, 1, "v");

    ARG_NAMED(v8::Local<v8::Value>, 0, "v");

    hr = isRegExp(v0, vr);

    METHOD_RETURN();
}

inline void util_base::s_static_isObject(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    bool vr;

    METHOD_ENTER();

    METHOD_OVER_NAMED(1, 1, "v");

    ARG_NAMED(v8::Local<v8::Value>, 0, "v");

    hr = isObject(v0, vr);

    METHOD_RETURN();
}

inline void util_base::s_static_isDate(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    bool vr;

    METHOD_ENTER();

    METHOD_OVER_NAMED(1, 1, "v");

    ARG_NAMED(v8::Local<v8::Value>, 0, "v");

    hr = isDate(v0, vr);

    METHOD_RETURN();
}

inline void util_base::s_static_isNativeError(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    bool vr;

    METHOD_ENTER();

    METHOD_OVER_NAMED(1, 1, "v");

    ARG_NAMED(v8::Local<v8::Value>, 0, "v");

    hr = isNativeError(v0, vr);

    METHOD_RETURN();
}

inline void util_base::s_static_isPrimitive(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    bool vr;

    METHOD_ENTER();

    METHOD_OVER_NAMED(1, 1, "v");

    ARG_NAMED(v8::Local<v8::Value>, 0, "v");

    hr = isPrimitive(v0, vr);

    METHOD_RETURN();
}

inline void util_base::s_static_isSymbol(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    bool vr;

    METHOD_ENTER();

    METHOD_OVER_NAMED(1, 1, "v");

    ARG_NAMED(v8::Local<v8::Value>, 0, "v");

    hr = isSymbol(v0, vr);

    METHOD_RETURN();
}

inline void util_base::s_static_isDataView(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    bool vr;

    METHOD_ENTER();

    METHOD_OVER_NAMED(1, 1, "v");

    ARG_NAMED(v8::Local<v8::Value>, 0, "v");

    hr = isDataView(v0, vr);

    METHOD_RETURN();
}

inline void util_base::s_static_isExternal(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    bool vr;

    METHOD_ENTER();

    METHOD_OVER_NAMED(1, 1, "v");

    ARG_NAMED(v8::Local<v8::Value>, 0, "v");

    hr = isExternal(v0, vr);

    METHOD_RETURN();
}

inline void util_base::s_static_isMap(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    bool vr;

    METHOD_ENTER();

    METHOD_OVER_NAMED(1, 1, "v");

    ARG_NAMED(v8::Local<v8::Value>, 0, "v");

    hr = isMap(v0, vr);

    METHOD_RETURN();
}

inline void util_base::s_static_isMapIterator(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    bool vr;

    METHOD_ENTER();

    METHOD_OVER_NAMED(1, 1, "v");

    ARG_NAMED(v8::Local<v8::Value>, 0, "v");

    hr = isMapIterator(v0, vr);

    METHOD_RETURN();
}

inline void util_base::s_static_isPromise(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    bool vr;

    METHOD_ENTER();

    METHOD_OVER_NAMED(1, 1, "v");

    ARG_NAMED(v8::Local<v8::Value>, 0, "v");

    hr = isPromise(v0, vr);

    METHOD_RETURN();
}

inline void util_base::s_static_isAsyncFunction(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    bool vr;

    METHOD_ENTER();

    METHOD_OVER_NAMED(1, 1, "v");

    ARG_NAMED(v8::Local<v8::Value>, 0, "v");

    hr = isAsyncFunction(v0, vr);

    METHOD_RETURN();
}

inline void util_base::s_static_isSet(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    bool vr;

    METHOD_ENTER();

    METHOD_OVER_NAMED(1, 1, "v");

    ARG_NAMED(v8::Local<v8::Value>, 0, "v");

    hr = isSet(v0, vr);

    METHOD_RETURN();
}

inline void util_base::s_static_isSetIterator(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    bool vr;

    METHOD_ENTER();

    METHOD_OVER_NAMED(1, 1, "v");

    ARG_NAMED(v8::Local<v8::Value>, 0, "v");

    hr = isSetIterator(v0, vr);

    METHOD_RETURN();
}

inline void util_base::s_static_isTypedArray(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    bool vr;

    METHOD_ENTER();

    METHOD_OVER_NAMED(1, 1, "v");

    ARG_NAMED(v8::Local<v8::Value>, 0, "v");

    hr = isTypedArray(v0, vr);

    METHOD_RETURN();
}

inline void util_base::s_static_isUint8Array(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    bool vr;

    METHOD_ENTER();

    METHOD_OVER_NAMED(1, 1, "v");

    ARG_NAMED(v8::Local<v8::Value>, 0, "v");

    hr = isUint8Array(v0, vr);

    METHOD_RETURN();
}

inline void util_base::s_static_isFunction(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    bool vr;

    METHOD_ENTER();

    METHOD_OVER_NAMED(1, 1, "v");

    ARG_NAMED(v8::Local<v8::Value>, 0, "v");

    hr = isFunction(v0, vr);

    METHOD_RETURN();
}

inline void util_base::s_static_isBuffer(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    bool vr;

    METHOD_ENTER();

    METHOD_OVER_NAMED(1, 1, "v");

    ARG_NAMED(v8::Local<v8::Value>, 0, "v");

    hr = isBuffer(v0, vr);

    METHOD_RETURN();
}

inline void util_base::s_static_isFloat16Array(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    bool vr;

    METHOD_ENTER();

    METHOD_OVER_NAMED(1, 1, "v");

    ARG_NAMED(v8::Local<v8::Value>, 0, "v");

    hr = isFloat16Array(v0, vr);

    METHOD_RETURN();
}

inline void util_base::s_static_isAnyArrayBuffer(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    bool vr;

    METHOD_ENTER();

    METHOD_OVER_NAMED(1, 1, "v");

    ARG_NAMED(v8::Local<v8::Value>, 0, "v");

    hr = isAnyArrayBuffer(v0, vr);

    METHOD_RETURN();
}

inline void util_base::s_static_isSharedArrayBuffer(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    bool vr;

    METHOD_ENTER();

    METHOD_OVER_NAMED(1, 1, "v");

    ARG_NAMED(v8::Local<v8::Value>, 0, "v");

    hr = isSharedArrayBuffer(v0, vr);

    METHOD_RETURN();
}

inline void util_base::s_static_isArgumentsObject(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    bool vr;

    METHOD_ENTER();

    METHOD_OVER_NAMED(1, 1, "v");

    ARG_NAMED(v8::Local<v8::Value>, 0, "v");

    hr = isArgumentsObject(v0, vr);

    METHOD_RETURN();
}

inline void util_base::s_static_isBoxedPrimitive(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    bool vr;

    METHOD_ENTER();

    METHOD_OVER_NAMED(1, 1, "v");

    ARG_NAMED(v8::Local<v8::Value>, 0, "v");

    hr = isBoxedPrimitive(v0, vr);

    METHOD_RETURN();
}

inline void util_base::s_static_isGeneratorFunction(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    bool vr;

    METHOD_ENTER();

    METHOD_OVER_NAMED(1, 1, "v");

    ARG_NAMED(v8::Local<v8::Value>, 0, "v");

    hr = isGeneratorFunction(v0, vr);

    METHOD_RETURN();
}

inline void util_base::s_static_isGeneratorObject(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    bool vr;

    METHOD_ENTER();

    METHOD_OVER_NAMED(1, 1, "v");

    ARG_NAMED(v8::Local<v8::Value>, 0, "v");

    hr = isGeneratorObject(v0, vr);

    METHOD_RETURN();
}

inline void util_base::s_static_isProxy(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    bool vr;

    METHOD_ENTER();

    METHOD_OVER_NAMED(1, 1, "v");

    ARG_NAMED(v8::Local<v8::Value>, 0, "v");

    hr = isProxy(v0, vr);

    METHOD_RETURN();
}

inline void util_base::s_static_isModuleNamespaceObject(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    bool vr;

    METHOD_ENTER();

    METHOD_OVER_NAMED(1, 1, "v");

    ARG_NAMED(v8::Local<v8::Value>, 0, "v");

    hr = isModuleNamespaceObject(v0, vr);

    METHOD_RETURN();
}

inline void util_base::s_static_isCryptoKey(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    bool vr;

    METHOD_ENTER();

    METHOD_OVER_NAMED(1, 1, "v");

    ARG_NAMED(v8::Local<v8::Value>, 0, "v");

    hr = isCryptoKey(v0, vr);

    METHOD_RETURN();
}

inline void util_base::s_static_isKeyObject(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    bool vr;

    METHOD_ENTER();

    METHOD_OVER_NAMED(1, 1, "v");

    ARG_NAMED(v8::Local<v8::Value>, 0, "v");

    hr = isKeyObject(v0, vr);

    METHOD_RETURN();
}

inline void util_base::s_static_isDeepEqual(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    bool vr;

    METHOD_ENTER();

    METHOD_OVER_NAMED(2, 2, "expected");

    ARG_NAMED(v8::Local<v8::Value>, 0, "actual");
    ARG_NAMED(v8::Local<v8::Value>, 1, "expected");

    hr = isDeepEqual(v0, v1, vr);

    METHOD_RETURN();
}

inline void util_base::s_static_isDeepStrictEqual(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    bool vr;

    METHOD_ENTER();

    METHOD_OVER_NAMED(2, 2, "expected");

    ARG_NAMED(v8::Local<v8::Value>, 0, "actual");
    ARG_NAMED(v8::Local<v8::Value>, 1, "expected");

    hr = isDeepStrictEqual(v0, v1, vr);

    METHOD_RETURN();
}

inline void util_base::s_static_has(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    bool vr;

    METHOD_ENTER();

    METHOD_OVER_NAMED(2, 2, "key");

    ARG_NAMED(v8::Local<v8::Value>, 0, "v");
    ARG_NAMED(exlib::string, 1, "key");

    hr = has(v0, v1, vr);

    METHOD_RETURN();
}

inline void util_base::s_static_keys(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Array> vr;

    METHOD_ENTER();

    METHOD_OVER_NAMED(1, 1, "v");

    ARG_NAMED(v8::Local<v8::Value>, 0, "v");

    hr = keys(v0, vr);

    METHOD_RETURN();
}

inline void util_base::s_static_values(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Array> vr;

    METHOD_ENTER();

    METHOD_OVER_NAMED(1, 1, "v");

    ARG_NAMED(v8::Local<v8::Value>, 0, "v");

    hr = values(v0, vr);

    METHOD_RETURN();
}

inline void util_base::s_static_clone(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Value> vr;

    METHOD_ENTER();

    METHOD_OVER_NAMED(1, 1, "v");

    ARG_NAMED(v8::Local<v8::Value>, 0, "v");

    hr = clone(v0, vr);

    METHOD_RETURN();
}

inline void util_base::s_static_deepFreeze(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_ENTER();

    METHOD_OVER_NAMED(1, 1, "v");

    ARG_NAMED(v8::Local<v8::Value>, 0, "v");

    hr = deepFreeze(v0);

    METHOD_VOID();
}

inline void util_base::s_static_extend(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Value> vr;

    METHOD_ENTER();

    METHOD_OVER_NAMED(-1, 1, "v");

    ARG_NAMED(v8::Local<v8::Value>, 0, "v");
    ARG_LIST(1);

    hr = extend(v0, v1, vr);

    METHOD_RETURN();
}

inline void util_base::s_static__extend(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Value> vr;

    METHOD_ENTER();

    METHOD_OVER_NAMED(-1, 1, "v");

    ARG_NAMED(v8::Local<v8::Value>, 0, "v");
    ARG_LIST(1);

    hr = _extend(v0, v1, vr);

    METHOD_RETURN();
}

inline void util_base::s_static_pick(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Object> vr;

    METHOD_ENTER();

    METHOD_OVER_NAMED(-1, 1, "v");

    ARG_NAMED(v8::Local<v8::Value>, 0, "v");
    ARG_LIST(1);

    hr = pick(v0, v1, vr);

    METHOD_RETURN();
}

inline void util_base::s_static_omit(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Object> vr;

    METHOD_ENTER();

    METHOD_OVER_NAMED(-1, 1, "v");

    ARG_NAMED(v8::Local<v8::Value>, 0, "v");
    ARG_LIST(1);

    hr = omit(v0, v1, vr);

    METHOD_RETURN();
}

inline void util_base::s_static_first(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Value> vr;

    METHOD_ENTER();

    METHOD_OVER_NAMED(1, 1, "v");

    ARG_NAMED(v8::Local<v8::Value>, 0, "v");

    hr = first(v0, vr);

    METHOD_OVER_NAMED(2, 2, "n");

    ARG_NAMED(v8::Local<v8::Value>, 0, "v");
    ARG_NAMED(int32_t, 1, "n");

    hr = first(v0, v1, vr);

    METHOD_RETURN();
}

inline void util_base::s_static_last(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Value> vr;

    METHOD_ENTER();

    METHOD_OVER_NAMED(1, 1, "v");

    ARG_NAMED(v8::Local<v8::Value>, 0, "v");

    hr = last(v0, vr);

    METHOD_OVER_NAMED(2, 2, "n");

    ARG_NAMED(v8::Local<v8::Value>, 0, "v");
    ARG_NAMED(int32_t, 1, "n");

    hr = last(v0, v1, vr);

    METHOD_RETURN();
}

inline void util_base::s_static_unique(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Array> vr;

    METHOD_ENTER();

    METHOD_OVER_NAMED(2, 1, "v");

    ARG_NAMED(v8::Local<v8::Value>, 0, "v");
    OPT_ARG_NAMED(bool, 1, false, "sorted");

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

    METHOD_OVER_NAMED(2, 1, "arr");

    ARG_NAMED(v8::Local<v8::Value>, 0, "arr");
    OPT_ARG_NAMED(bool, 1, false, "shallow");

    hr = flatten(v0, v1, vr);

    METHOD_RETURN();
}

inline void util_base::s_static_without(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Array> vr;

    METHOD_ENTER();

    METHOD_OVER_NAMED(-1, 1, "arr");

    ARG_NAMED(v8::Local<v8::Value>, 0, "arr");
    ARG_LIST(1);

    hr = without(v0, v1, vr);

    METHOD_RETURN();
}

inline void util_base::s_static_difference(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Array> vr;

    METHOD_ENTER();

    METHOD_OVER_NAMED(-1, 1, "list");

    ARG_NAMED(v8::Local<v8::Array>, 0, "list");
    ARG_LIST(1);

    hr = difference(v0, v1, vr);

    METHOD_RETURN();
}

inline void util_base::s_static_each(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Value> vr;

    METHOD_ENTER();

    METHOD_OVER_NAMED(3, 2, "iterator");

    ARG_NAMED(v8::Local<v8::Value>, 0, "list");
    ARG_NAMED(v8::Local<v8::Function>, 1, "iterator");
    OPT_ARG_NAMED(v8::Local<v8::Value>, 2, v8::Undefined(isolate->m_isolate), "context");

    hr = each(v0, v1, v2, vr);

    METHOD_RETURN();
}

inline void util_base::s_static_map(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Array> vr;

    METHOD_ENTER();

    METHOD_OVER_NAMED(3, 2, "iterator");

    ARG_NAMED(v8::Local<v8::Value>, 0, "list");
    ARG_NAMED(v8::Local<v8::Function>, 1, "iterator");
    OPT_ARG_NAMED(v8::Local<v8::Value>, 2, v8::Undefined(isolate->m_isolate), "context");

    hr = map(v0, v1, v2, vr);

    METHOD_RETURN();
}

inline void util_base::s_static_reduce(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Value> vr;

    METHOD_ENTER();

    METHOD_OVER_NAMED(4, 3, "memo");

    ARG_NAMED(v8::Local<v8::Value>, 0, "list");
    ARG_NAMED(v8::Local<v8::Function>, 1, "iterator");
    ARG_NAMED(v8::Local<v8::Value>, 2, "memo");
    OPT_ARG_NAMED(v8::Local<v8::Value>, 3, v8::Undefined(isolate->m_isolate), "context");

    hr = reduce(v0, v1, v2, v3, vr);

    METHOD_RETURN();
}

inline void util_base::s_static_parseArgs(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<NArray> vr;

    METHOD_ENTER();

    METHOD_OVER_NAMED(1, 1, "command");

    ARG_NAMED(exlib::string, 0, "command");

    hr = parseArgs(v0, vr);

    METHOD_RETURN();
}

inline void util_base::s_static_compile(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<Buffer_base> vr;

    METHOD_ENTER();

    METHOD_OVER_NAMED(3, 2, "script");

    ARG_NAMED(exlib::string, 0, "srcname");
    ARG_NAMED(exlib::string, 1, "script");
    OPT_ARG_NAMED(int32_t, 2, 0, "mode");

    hr = compile(v0, v1, v2, vr);

    METHOD_RETURN();
}

inline void util_base::s_static_sync(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Function> vr;

    METHOD_ENTER();

    METHOD_OVER_NAMED(2, 1, "func");

    ARG_NAMED(v8::Local<v8::Function>, 0, "func");
    OPT_ARG_NAMED(bool, 1, false, "async_func");

    hr = sync(v0, v1, vr);

    METHOD_RETURN();
}

inline void util_base::s_static_promisify(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Function> vr;

    METHOD_ENTER();

    METHOD_OVER_NAMED(1, 1, "func");

    ARG_NAMED(v8::Local<v8::Function>, 0, "func");

    hr = promisify(v0, vr);

    METHOD_RETURN();
}

inline void util_base::s_static_callbackify(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Function> vr;

    METHOD_ENTER();

    METHOD_OVER_NAMED(1, 1, "func");

    ARG_NAMED(v8::Local<v8::Function>, 0, "func");

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

inline void util_base::s_static_stripTypeScript(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_ENTER();

    METHOD_OVER_NAMED(1, 1, "code");

    ARG_NAMED(exlib::string, 0, "code");

    hr = stripTypeScript(v0, vr);

    METHOD_RETURN();
}

inline void util_base::s_static_getStringWidth(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    int32_t vr;

    METHOD_ENTER();

    METHOD_OVER_NAMED(1, 1, "str");

    ARG_NAMED(exlib::string, 0, "str");

    hr = getStringWidth(v0, vr);

    METHOD_RETURN();
}

inline void util_base::s_static_getSystemErrorName(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_ENTER();

    METHOD_OVER_NAMED(1, 1, "err");

    ARG_NAMED(int32_t, 0, "err");

    hr = getSystemErrorName(v0, vr);

    METHOD_RETURN();
}

inline void util_base::s_static_getSystemErrorMessage(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_ENTER();

    METHOD_OVER_NAMED(1, 1, "err");

    ARG_NAMED(int32_t, 0, "err");

    hr = getSystemErrorMessage(v0, vr);

    METHOD_RETURN();
}

inline void util_base::s_static_getSystemErrorMap(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Value> vr;

    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = getSystemErrorMap(vr);

    METHOD_RETURN();
}

inline void util_base::s_static_stripVTControlCharacters(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_ENTER();

    METHOD_OVER_NAMED(1, 1, "str");

    ARG_NAMED(exlib::string, 0, "str");

    hr = stripVTControlCharacters(v0, vr);

    METHOD_RETURN();
}

inline void util_base::s_static_aborted(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    ASYNC_METHOD_ENTER("util.aborted");

    METHOD_OVER_NAMED(2, 1, "signal");

    ARG_NAMED(obj_ptr<AbortSignal_base>, 0, "signal");
    OPT_ARG_NAMED(v8::Local<v8::Object>, 1, v8::Object::New(isolate->m_isolate), "resource");

    if (!cb.IsEmpty())
        hr = acb_aborted(v0.get(), v1, cb, args);
    else
        hr = ac_aborted(v0.get(), v1);

    ASYNC_METHOD_VOID();
}
}
