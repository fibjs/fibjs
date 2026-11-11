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

class Buffer_base;

class registry_base : public object_base {
    DECLARE_CLASS(registry_base);

public:
    enum {
        C_CLASSES_ROOT = 0,
        C_CURRENT_USER = 1,
        C_LOCAL_MACHINE = 2,
        C_USERS = 3,
        C_CURRENT_CONFIG = 5,
        C_SZ = 1,
        C_EXPAND_SZ = 2,
        C_DWORD = 4,
        C_QWORD = 11
    };

public:
    // registry_base
    static result_t listSubKey(int32_t root, exlib::string key, std::vector<exlib::string>& retVal);
    static result_t listValue(int32_t root, exlib::string key, std::vector<exlib::string>& retVal);
    static result_t get(int32_t root, exlib::string key, v8::Local<v8::Value>& retVal);
    static result_t get(int32_t root, exlib::string key, exlib::string name, v8::Local<v8::Value>& retVal);
    static result_t set(int32_t root, exlib::string key, double value, int32_t type);
    static result_t set(int32_t root, exlib::string key, std::vector<exlib::string>& values);
    static result_t set(int32_t root, exlib::string key, Buffer_base* value);
    static result_t set(int32_t root, exlib::string key, exlib::string value, int32_t type);
    static result_t set(int32_t root, exlib::string key, exlib::string name, double value, int32_t type);
    static result_t set(int32_t root, exlib::string key, exlib::string name, std::vector<exlib::string>& values);
    static result_t set(int32_t root, exlib::string key, exlib::string name, Buffer_base* value);
    static result_t set(int32_t root, exlib::string key, exlib::string name, exlib::string value, int32_t type);
    static result_t has(int32_t root, exlib::string key, bool& retVal);
    static result_t has(int32_t root, exlib::string key, exlib::string name, bool& retVal);
    static result_t del(int32_t root, exlib::string key);
    static result_t del(int32_t root, exlib::string key, exlib::string name);

public:
    static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        CONSTRUCT_INIT();

        ThrowTypeError("not a constructor");
    }

    static result_t load(v8::Local<v8::Value> v, obj_ptr<registry_base>& retVal)
    { return result_t::type_mismatch(); }

public:
    static void s_static_listSubKey(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_listValue(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_get(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_set(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_has(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_del(const v8::FunctionCallbackInfo<v8::Value>& args);
};
}

#include "ifs/Buffer.h"

namespace fibjs {
inline ClassInfo& registry_base::class_info()
{
    static ClassData::ClassMethod s_method[] = {
        { "listSubKey", s_static_listSubKey, true, ClassData::ASYNC_SYNC },
        { "listValue", s_static_listValue, true, ClassData::ASYNC_SYNC },
        { "get", s_static_get, true, ClassData::ASYNC_SYNC },
        { "set", s_static_set, true, ClassData::ASYNC_SYNC },
        { "has", s_static_has, true, ClassData::ASYNC_SYNC },
        { "del", s_static_del, true, ClassData::ASYNC_SYNC }
    };

    static ClassData::ClassConst s_const[] = {
        { "CLASSES_ROOT", ClassData::CONST_Integer, { .intValue = C_CLASSES_ROOT } },
        { "CURRENT_USER", ClassData::CONST_Integer, { .intValue = C_CURRENT_USER } },
        { "LOCAL_MACHINE", ClassData::CONST_Integer, { .intValue = C_LOCAL_MACHINE } },
        { "USERS", ClassData::CONST_Integer, { .intValue = C_USERS } },
        { "CURRENT_CONFIG", ClassData::CONST_Integer, { .intValue = C_CURRENT_CONFIG } },
        { "SZ", ClassData::CONST_Integer, { .intValue = C_SZ } },
        { "EXPAND_SZ", ClassData::CONST_Integer, { .intValue = C_EXPAND_SZ } },
        { "DWORD", ClassData::CONST_Integer, { .intValue = C_DWORD } },
        { "QWORD", ClassData::CONST_Integer, { .intValue = C_QWORD } }
    };

    static ClassData s_cd = {
        "registry", true, s__new, NULL,
        ARRAYSIZE(s_method), s_method, 0, NULL, 0, NULL, ARRAYSIZE(s_const), s_const, NULL, NULL,
        &object_base::class_info(),
        false
    };

    static ClassInfo s_ci(s_cd);
    return s_ci;
}

inline void registry_base::s_static_listSubKey(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    std::vector<exlib::string> vr;

    METHOD_ENTER();

    METHOD_OVER_NAMED(2, 2, "key");

    ARG_NAMED(int32_t, 0, "root");
    ARG_NAMED(exlib::string, 1, "key");

    hr = listSubKey(v0, v1, vr);

    METHOD_RETURN();
}

inline void registry_base::s_static_listValue(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    std::vector<exlib::string> vr;

    METHOD_ENTER();

    METHOD_OVER_NAMED(2, 2, "key");

    ARG_NAMED(int32_t, 0, "root");
    ARG_NAMED(exlib::string, 1, "key");

    hr = listValue(v0, v1, vr);

    METHOD_RETURN();
}

inline void registry_base::s_static_get(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Value> vr;

    METHOD_ENTER();

    METHOD_OVER_NAMED(2, 2, "key");

    ARG_NAMED(int32_t, 0, "root");
    ARG_NAMED(exlib::string, 1, "key");

    hr = get(v0, v1, vr);

    METHOD_OVER_NAMED(3, 3, "name");

    ARG_NAMED(int32_t, 0, "root");
    ARG_NAMED(exlib::string, 1, "key");
    ARG_NAMED(exlib::string, 2, "name");

    hr = get(v0, v1, v2, vr);

    METHOD_RETURN();
}

inline void registry_base::s_static_set(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_ENTER();

    METHOD_OVER_NAMED(4, 3, "value");

    ARG_NAMED(int32_t, 0, "root");
    ARG_NAMED(exlib::string, 1, "key");
    ARG_NAMED(double, 2, "value");
    OPT_ARG_NAMED(int32_t, 3, C_DWORD, "type");

    hr = set(v0, v1, v2, v3);

    METHOD_OVER_NAMED(3, 3, "values");

    ARG_NAMED(int32_t, 0, "root");
    ARG_NAMED(exlib::string, 1, "key");
    ARG_NAMED(std::vector<exlib::string>, 2, "values");

    hr = set(v0, v1, v2);

    METHOD_OVER_NAMED(3, 3, "value");

    ARG_NAMED(int32_t, 0, "root");
    ARG_NAMED(exlib::string, 1, "key");
    ARG_NAMED(obj_ptr<Buffer_base>, 2, "value");

    hr = set(v0, v1, v2.get());

    METHOD_OVER_NAMED(4, 3, "value");

    ARG_NAMED(int32_t, 0, "root");
    ARG_NAMED(exlib::string, 1, "key");
    ARG_NAMED(exlib::string, 2, "value");
    OPT_ARG_NAMED(int32_t, 3, C_SZ, "type");

    hr = set(v0, v1, v2, v3);

    METHOD_OVER_NAMED(5, 4, "value");

    ARG_NAMED(int32_t, 0, "root");
    ARG_NAMED(exlib::string, 1, "key");
    ARG_NAMED(exlib::string, 2, "name");
    ARG_NAMED(double, 3, "value");
    OPT_ARG_NAMED(int32_t, 4, C_DWORD, "type");

    hr = set(v0, v1, v2, v3, v4);

    METHOD_OVER_NAMED(4, 4, "values");

    ARG_NAMED(int32_t, 0, "root");
    ARG_NAMED(exlib::string, 1, "key");
    ARG_NAMED(exlib::string, 2, "name");
    ARG_NAMED(std::vector<exlib::string>, 3, "values");

    hr = set(v0, v1, v2, v3);

    METHOD_OVER_NAMED(4, 4, "value");

    ARG_NAMED(int32_t, 0, "root");
    ARG_NAMED(exlib::string, 1, "key");
    ARG_NAMED(exlib::string, 2, "name");
    ARG_NAMED(obj_ptr<Buffer_base>, 3, "value");

    hr = set(v0, v1, v2, v3.get());

    METHOD_OVER_NAMED(5, 4, "value");

    ARG_NAMED(int32_t, 0, "root");
    ARG_NAMED(exlib::string, 1, "key");
    ARG_NAMED(exlib::string, 2, "name");
    ARG_NAMED(exlib::string, 3, "value");
    OPT_ARG_NAMED(int32_t, 4, C_SZ, "type");

    hr = set(v0, v1, v2, v3, v4);

    METHOD_VOID();
}

inline void registry_base::s_static_has(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    bool vr;

    METHOD_ENTER();

    METHOD_OVER_NAMED(2, 2, "key");

    ARG_NAMED(int32_t, 0, "root");
    ARG_NAMED(exlib::string, 1, "key");

    hr = has(v0, v1, vr);

    METHOD_OVER_NAMED(3, 3, "name");

    ARG_NAMED(int32_t, 0, "root");
    ARG_NAMED(exlib::string, 1, "key");
    ARG_NAMED(exlib::string, 2, "name");

    hr = has(v0, v1, v2, vr);

    METHOD_RETURN();
}

inline void registry_base::s_static_del(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_ENTER();

    METHOD_OVER_NAMED(2, 2, "key");

    ARG_NAMED(int32_t, 0, "root");
    ARG_NAMED(exlib::string, 1, "key");

    hr = del(v0, v1);

    METHOD_OVER_NAMED(3, 3, "name");

    ARG_NAMED(int32_t, 0, "root");
    ARG_NAMED(exlib::string, 1, "key");
    ARG_NAMED(exlib::string, 2, "name");

    hr = del(v0, v1, v2);

    METHOD_VOID();
}
}
