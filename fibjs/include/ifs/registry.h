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
    static result_t listSubKey(int32_t root, exlib::string key, obj_ptr<NArray>& retVal);
    static result_t listValue(int32_t root, exlib::string key, obj_ptr<NArray>& retVal);
    static result_t get(int32_t root, exlib::string key, v8::Local<v8::Value>& retVal);
    static result_t set(int32_t root, exlib::string key, double value, int32_t type);
    static result_t set(int32_t root, exlib::string key, exlib::string value, int32_t type);
    static result_t set(int32_t root, exlib::string key, v8::Local<v8::Array> value);
    static result_t set(int32_t root, exlib::string key, Buffer_base* value);
    static result_t del(int32_t root, exlib::string key);

public:
    static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        CONSTRUCT_INIT();

        Isolate* isolate = Isolate::current();

        isolate->m_isolate->ThrowException(
            isolate->NewString("not a constructor"));
    }

public:
    static void s_static_listSubKey(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_listValue(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_get(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_set(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_del(const v8::FunctionCallbackInfo<v8::Value>& args);
};
}

#include "ifs/Buffer.h"

namespace fibjs {
inline ClassInfo& registry_base::class_info()
{
    static ClassData::ClassMethod s_method[] = {
        { "listSubKey", s_static_listSubKey, true, false },
        { "listValue", s_static_listValue, true, false },
        { "get", s_static_get, true, false },
        { "set", s_static_set, true, false },
        { "del", s_static_del, true, false }
    };

    static ClassData::ClassConst s_const[] = {
        { "CLASSES_ROOT", C_CLASSES_ROOT },
        { "CURRENT_USER", C_CURRENT_USER },
        { "LOCAL_MACHINE", C_LOCAL_MACHINE },
        { "USERS", C_USERS },
        { "CURRENT_CONFIG", C_CURRENT_CONFIG },
        { "SZ", C_SZ },
        { "EXPAND_SZ", C_EXPAND_SZ },
        { "DWORD", C_DWORD },
        { "QWORD", C_QWORD }
    };

    static ClassData s_cd = {
        "registry", true, s__new, NULL,
        ARRAYSIZE(s_method), s_method, 0, NULL, 0, NULL, ARRAYSIZE(s_const), s_const, NULL, NULL,
        &object_base::class_info()
    };

    static ClassInfo s_ci(s_cd);
    return s_ci;
}

inline void registry_base::s_static_listSubKey(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<NArray> vr;

    METHOD_NAME("registry.listSubKey");
    METHOD_ENTER();

    METHOD_OVER(2, 2);

    ARG(int32_t, 0);
    ARG(exlib::string, 1);

    hr = listSubKey(v0, v1, vr);

    METHOD_RETURN();
}

inline void registry_base::s_static_listValue(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<NArray> vr;

    METHOD_NAME("registry.listValue");
    METHOD_ENTER();

    METHOD_OVER(2, 2);

    ARG(int32_t, 0);
    ARG(exlib::string, 1);

    hr = listValue(v0, v1, vr);

    METHOD_RETURN();
}

inline void registry_base::s_static_get(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Value> vr;

    METHOD_NAME("registry.get");
    METHOD_ENTER();

    METHOD_OVER(2, 2);

    ARG(int32_t, 0);
    ARG(exlib::string, 1);

    hr = get(v0, v1, vr);

    METHOD_RETURN();
}

inline void registry_base::s_static_set(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_NAME("registry.set");
    METHOD_ENTER();

    METHOD_OVER(4, 3);

    ARG(int32_t, 0);
    ARG(exlib::string, 1);
    ARG(double, 2);
    OPT_ARG(int32_t, 3, C_DWORD);

    hr = set(v0, v1, v2, v3);

    METHOD_OVER(4, 3);

    ARG(int32_t, 0);
    ARG(exlib::string, 1);
    ARG(exlib::string, 2);
    OPT_ARG(int32_t, 3, C_SZ);

    hr = set(v0, v1, v2, v3);

    METHOD_OVER(3, 3);

    ARG(int32_t, 0);
    ARG(exlib::string, 1);
    ARG(v8::Local<v8::Array>, 2);

    hr = set(v0, v1, v2);

    METHOD_OVER(3, 3);

    ARG(int32_t, 0);
    ARG(exlib::string, 1);
    ARG(obj_ptr<Buffer_base>, 2);

    hr = set(v0, v1, v2);

    METHOD_VOID();
}

inline void registry_base::s_static_del(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_NAME("registry.del");
    METHOD_ENTER();

    METHOD_OVER(2, 2);

    ARG(int32_t, 0);
    ARG(exlib::string, 1);

    hr = del(v0, v1);

    METHOD_VOID();
}
}
