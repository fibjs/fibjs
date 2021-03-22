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
        __CLASSES_ROOT = 0,
        __CURRENT_USER = 1,
        __LOCAL_MACHINE = 2,
        __USERS = 3,
        __CURRENT_CONFIG = 5,
        __SZ = 1,
        __EXPAND_SZ = 2,
        __DWORD = 4,
        __QWORD = 11
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
        { "listSubKey", s_static_listSubKey, true },
        { "listValue", s_static_listValue, true },
        { "get", s_static_get, true },
        { "set", s_static_set, true },
        { "del", s_static_del, true }
    };

    static ClassData::ClassConst s_const[] = {
        { "CLASSES_ROOT", __CLASSES_ROOT },
        { "CURRENT_USER", __CURRENT_USER },
        { "LOCAL_MACHINE", __LOCAL_MACHINE },
        { "USERS", __USERS },
        { "CURRENT_CONFIG", __CURRENT_CONFIG },
        { "SZ", __SZ },
        { "EXPAND_SZ", __EXPAND_SZ },
        { "DWORD", __DWORD },
        { "QWORD", __QWORD }
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
    OPT_ARG(int32_t, 3, __DWORD);

    hr = set(v0, v1, v2, v3);

    METHOD_OVER(4, 3);

    ARG(int32_t, 0);
    ARG(exlib::string, 1);
    ARG(exlib::string, 2);
    OPT_ARG(int32_t, 3, __SZ);

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
