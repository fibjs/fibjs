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

namespace fibjs
{

class Stats_base;
class LruCache_base;

class util_base : public object_base
{
    DECLARE_CLASS(util_base);

public:
    // util_base
    static result_t format(const char* fmt, const v8::FunctionCallbackInfo<v8::Value>& args, std::string& retVal);
    static result_t format(const v8::FunctionCallbackInfo<v8::Value>& args, std::string& retVal);
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
    static result_t isFunction(v8::Local<v8::Value> v, bool& retVal);
    static result_t isBuffer(v8::Local<v8::Value> v, bool& retVal);
    static result_t has(v8::Local<v8::Value> v, const char* key, bool& retVal);
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
    static result_t buildInfo(v8::Local<v8::Object>& retVal);

public:
    static void s_format(const v8::FunctionCallbackInfo<v8::Value>& args);
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
    static void s_buildInfo(const v8::FunctionCallbackInfo<v8::Value>& args);
};

}

#include "Stats.h"
#include "LruCache.h"

namespace fibjs
{
    inline ClassInfo& util_base::class_info()
    {
        static ClassData::ClassMethod s_method[] = 
        {
            {"format", s_format, true},
            {"isEmpty", s_isEmpty, true},
            {"isArray", s_isArray, true},
            {"isBoolean", s_isBoolean, true},
            {"isNull", s_isNull, true},
            {"isNullOrUndefined", s_isNullOrUndefined, true},
            {"isNumber", s_isNumber, true},
            {"isString", s_isString, true},
            {"isUndefined", s_isUndefined, true},
            {"isRegExp", s_isRegExp, true},
            {"isObject", s_isObject, true},
            {"isDate", s_isDate, true},
            {"isFunction", s_isFunction, true},
            {"isBuffer", s_isBuffer, true},
            {"has", s_has, true},
            {"keys", s_keys, true},
            {"values", s_values, true},
            {"clone", s_clone, true},
            {"extend", s_extend, true},
            {"pick", s_pick, true},
            {"omit", s_omit, true},
            {"first", s_first, true},
            {"last", s_last, true},
            {"unique", s_unique, true},
            {"union", s_union, true},
            {"intersection", s_intersection, true},
            {"flatten", s_flatten, true},
            {"without", s_without, true},
            {"difference", s_difference, true},
            {"each", s_each, true},
            {"map", s_map, true},
            {"reduce", s_reduce, true},
            {"buildInfo", s_buildInfo, true}
        };

        static ClassData::ClassObject s_object[] = 
        {
            {"Stats", Stats_base::class_info},
            {"LruCache", LruCache_base::class_info}
        };

        static ClassData s_cd = 
        { 
            "util", NULL, 
            33, s_method, 2, s_object, 0, NULL, NULL, NULL,
            NULL
        };

        static ClassInfo s_ci(s_cd);
        return s_ci;
    }


    inline void util_base::s_format(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        std::string vr;

        METHOD_ENTER(-1, 1);

        ARG(arg_string, 0);

        hr = format(v0, args, vr);

        METHOD_OVER(-1, 0);

        hr = format(args, vr);

        METHOD_RETURN();
    }

    inline void util_base::s_isEmpty(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        bool vr;

        METHOD_ENTER(1, 1);

        ARG(v8::Local<v8::Value>, 0);

        hr = isEmpty(v0, vr);

        METHOD_RETURN();
    }

    inline void util_base::s_isArray(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        bool vr;

        METHOD_ENTER(1, 1);

        ARG(v8::Local<v8::Value>, 0);

        hr = isArray(v0, vr);

        METHOD_RETURN();
    }

    inline void util_base::s_isBoolean(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        bool vr;

        METHOD_ENTER(1, 1);

        ARG(v8::Local<v8::Value>, 0);

        hr = isBoolean(v0, vr);

        METHOD_RETURN();
    }

    inline void util_base::s_isNull(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        bool vr;

        METHOD_ENTER(1, 1);

        ARG(v8::Local<v8::Value>, 0);

        hr = isNull(v0, vr);

        METHOD_RETURN();
    }

    inline void util_base::s_isNullOrUndefined(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        bool vr;

        METHOD_ENTER(1, 1);

        ARG(v8::Local<v8::Value>, 0);

        hr = isNullOrUndefined(v0, vr);

        METHOD_RETURN();
    }

    inline void util_base::s_isNumber(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        bool vr;

        METHOD_ENTER(1, 1);

        ARG(v8::Local<v8::Value>, 0);

        hr = isNumber(v0, vr);

        METHOD_RETURN();
    }

    inline void util_base::s_isString(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        bool vr;

        METHOD_ENTER(1, 1);

        ARG(v8::Local<v8::Value>, 0);

        hr = isString(v0, vr);

        METHOD_RETURN();
    }

    inline void util_base::s_isUndefined(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        bool vr;

        METHOD_ENTER(1, 1);

        ARG(v8::Local<v8::Value>, 0);

        hr = isUndefined(v0, vr);

        METHOD_RETURN();
    }

    inline void util_base::s_isRegExp(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        bool vr;

        METHOD_ENTER(1, 1);

        ARG(v8::Local<v8::Value>, 0);

        hr = isRegExp(v0, vr);

        METHOD_RETURN();
    }

    inline void util_base::s_isObject(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        bool vr;

        METHOD_ENTER(1, 1);

        ARG(v8::Local<v8::Value>, 0);

        hr = isObject(v0, vr);

        METHOD_RETURN();
    }

    inline void util_base::s_isDate(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        bool vr;

        METHOD_ENTER(1, 1);

        ARG(v8::Local<v8::Value>, 0);

        hr = isDate(v0, vr);

        METHOD_RETURN();
    }

    inline void util_base::s_isFunction(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        bool vr;

        METHOD_ENTER(1, 1);

        ARG(v8::Local<v8::Value>, 0);

        hr = isFunction(v0, vr);

        METHOD_RETURN();
    }

    inline void util_base::s_isBuffer(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        bool vr;

        METHOD_ENTER(1, 1);

        ARG(v8::Local<v8::Value>, 0);

        hr = isBuffer(v0, vr);

        METHOD_RETURN();
    }

    inline void util_base::s_has(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        bool vr;

        METHOD_ENTER(2, 2);

        ARG(v8::Local<v8::Value>, 0);
        ARG(arg_string, 1);

        hr = has(v0, v1, vr);

        METHOD_RETURN();
    }

    inline void util_base::s_keys(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        v8::Local<v8::Array> vr;

        METHOD_ENTER(1, 1);

        ARG(v8::Local<v8::Value>, 0);

        hr = keys(v0, vr);

        METHOD_RETURN();
    }

    inline void util_base::s_values(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        v8::Local<v8::Array> vr;

        METHOD_ENTER(1, 1);

        ARG(v8::Local<v8::Value>, 0);

        hr = values(v0, vr);

        METHOD_RETURN();
    }

    inline void util_base::s_clone(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        v8::Local<v8::Value> vr;

        METHOD_ENTER(1, 1);

        ARG(v8::Local<v8::Value>, 0);

        hr = clone(v0, vr);

        METHOD_RETURN();
    }

    inline void util_base::s_extend(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        v8::Local<v8::Value> vr;

        METHOD_ENTER(-1, 1);

        ARG(v8::Local<v8::Value>, 0);

        hr = extend(v0, args, vr);

        METHOD_RETURN();
    }

    inline void util_base::s_pick(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        v8::Local<v8::Object> vr;

        METHOD_ENTER(-1, 1);

        ARG(v8::Local<v8::Value>, 0);

        hr = pick(v0, args, vr);

        METHOD_RETURN();
    }

    inline void util_base::s_omit(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        v8::Local<v8::Object> vr;

        METHOD_ENTER(-1, 1);

        ARG(v8::Local<v8::Value>, 0);

        hr = omit(v0, args, vr);

        METHOD_RETURN();
    }

    inline void util_base::s_first(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        v8::Local<v8::Value> vr;

        METHOD_ENTER(1, 1);

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

        METHOD_ENTER(1, 1);

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

        METHOD_ENTER(2, 1);

        ARG(v8::Local<v8::Value>, 0);
        OPT_ARG(bool, 1, false);

        hr = unique(v0, v1, vr);

        METHOD_RETURN();
    }

    inline void util_base::s_union(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        v8::Local<v8::Array> vr;

        METHOD_ENTER(-1, 0);

        hr = _union(args, vr);

        METHOD_RETURN();
    }

    inline void util_base::s_intersection(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        v8::Local<v8::Array> vr;

        METHOD_ENTER(-1, 0);

        hr = intersection(args, vr);

        METHOD_RETURN();
    }

    inline void util_base::s_flatten(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        v8::Local<v8::Array> vr;

        METHOD_ENTER(2, 1);

        ARG(v8::Local<v8::Value>, 0);
        OPT_ARG(bool, 1, false);

        hr = flatten(v0, v1, vr);

        METHOD_RETURN();
    }

    inline void util_base::s_without(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        v8::Local<v8::Array> vr;

        METHOD_ENTER(-1, 1);

        ARG(v8::Local<v8::Value>, 0);

        hr = without(v0, args, vr);

        METHOD_RETURN();
    }

    inline void util_base::s_difference(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        v8::Local<v8::Array> vr;

        METHOD_ENTER(-1, 1);

        ARG(v8::Local<v8::Array>, 0);

        hr = difference(v0, args, vr);

        METHOD_RETURN();
    }

    inline void util_base::s_each(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        v8::Local<v8::Value> vr;

        METHOD_ENTER(3, 2);

        ARG(v8::Local<v8::Value>, 0);
        ARG(v8::Local<v8::Function>, 1);
        OPT_ARG(v8::Local<v8::Value>, 2, v8::Undefined(Isolate::now()->m_isolate));

        hr = each(v0, v1, v2, vr);

        METHOD_RETURN();
    }

    inline void util_base::s_map(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        v8::Local<v8::Array> vr;

        METHOD_ENTER(3, 2);

        ARG(v8::Local<v8::Value>, 0);
        ARG(v8::Local<v8::Function>, 1);
        OPT_ARG(v8::Local<v8::Value>, 2, v8::Undefined(Isolate::now()->m_isolate));

        hr = map(v0, v1, v2, vr);

        METHOD_RETURN();
    }

    inline void util_base::s_reduce(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        v8::Local<v8::Value> vr;

        METHOD_ENTER(4, 3);

        ARG(v8::Local<v8::Value>, 0);
        ARG(v8::Local<v8::Function>, 1);
        ARG(v8::Local<v8::Value>, 2);
        OPT_ARG(v8::Local<v8::Value>, 3, v8::Undefined(Isolate::now()->m_isolate));

        hr = reduce(v0, v1, v2, v3, vr);

        METHOD_RETURN();
    }

    inline void util_base::s_buildInfo(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        v8::Local<v8::Object> vr;

        METHOD_ENTER(0, 0);

        hr = buildInfo(vr);

        METHOD_RETURN();
    }

}

#endif

