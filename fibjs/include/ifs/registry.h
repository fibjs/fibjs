/***************************************************************************
 *                                                                         *
 *   This file was automatically generated using idlc.js                   *
 *   PLEASE DO NOT EDIT!!!!                                                *
 *                                                                         *
 ***************************************************************************/

#ifndef _registry_base_H_
#define _registry_base_H_

/**
 @author Leo Hoo <lion@9465.net>
 */

#include "../object.h"

namespace fibjs {

class List_base;
class Buffer_base;

class registry_base : public object_base {
    DECLARE_CLASS(registry_base);

public:
    enum {
        _CLASSES_ROOT = 0,
        _CURRENT_USER = 1,
        _LOCAL_MACHINE = 2,
        _USERS = 3,
        _CURRENT_CONFIG = 5,
        _SZ = 1,
        _EXPAND_SZ = 2,
        _DWORD = 4,
        _QWORD = 11
    };

public:
    // registry_base
    static result_t listSubKey(int32_t root, exlib::string key, obj_ptr<List_base>& retVal);
    static result_t listValue(int32_t root, exlib::string key, obj_ptr<List_base>& retVal);
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
            isolate->NewFromUtf8("not a constructor"));
    }

public:
    static void s_get_CLASSES_ROOT(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_get_CURRENT_USER(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_get_LOCAL_MACHINE(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_get_USERS(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_get_CURRENT_CONFIG(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_get_SZ(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_get_EXPAND_SZ(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_get_DWORD(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_get_QWORD(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_listSubKey(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_listValue(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_set(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_del(const v8::FunctionCallbackInfo<v8::Value>& args);
};
}

#include "List.h"
#include "Buffer.h"

namespace fibjs {
inline ClassInfo& registry_base::class_info()
{
    static ClassData::ClassMethod s_method[] = {
        { "listSubKey", s_listSubKey, true },
        { "listValue", s_listValue, true },
        { "get", s_get, true },
        { "set", s_set, true },
        { "del", s_del, true }
    };

    static ClassData::ClassProperty s_property[] = {
        { "CLASSES_ROOT", s_get_CLASSES_ROOT, block_set, true },
        { "CURRENT_USER", s_get_CURRENT_USER, block_set, true },
        { "LOCAL_MACHINE", s_get_LOCAL_MACHINE, block_set, true },
        { "USERS", s_get_USERS, block_set, true },
        { "CURRENT_CONFIG", s_get_CURRENT_CONFIG, block_set, true },
        { "SZ", s_get_SZ, block_set, true },
        { "EXPAND_SZ", s_get_EXPAND_SZ, block_set, true },
        { "DWORD", s_get_DWORD, block_set, true },
        { "QWORD", s_get_QWORD, block_set, true }
    };

    static ClassData s_cd = {
        "registry", true, s__new, NULL,
        ARRAYSIZE(s_method), s_method, 0, NULL, ARRAYSIZE(s_property), s_property, NULL, NULL,
        &object_base::class_info()
    };

    static ClassInfo s_ci(s_cd);
    return s_ci;
}

inline void registry_base::s_get_CLASSES_ROOT(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    int32_t vr = _CLASSES_ROOT;
    PROPERTY_ENTER();
    METHOD_RETURN();
}

inline void registry_base::s_get_CURRENT_USER(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    int32_t vr = _CURRENT_USER;
    PROPERTY_ENTER();
    METHOD_RETURN();
}

inline void registry_base::s_get_LOCAL_MACHINE(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    int32_t vr = _LOCAL_MACHINE;
    PROPERTY_ENTER();
    METHOD_RETURN();
}

inline void registry_base::s_get_USERS(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    int32_t vr = _USERS;
    PROPERTY_ENTER();
    METHOD_RETURN();
}

inline void registry_base::s_get_CURRENT_CONFIG(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    int32_t vr = _CURRENT_CONFIG;
    PROPERTY_ENTER();
    METHOD_RETURN();
}

inline void registry_base::s_get_SZ(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    int32_t vr = _SZ;
    PROPERTY_ENTER();
    METHOD_RETURN();
}

inline void registry_base::s_get_EXPAND_SZ(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    int32_t vr = _EXPAND_SZ;
    PROPERTY_ENTER();
    METHOD_RETURN();
}

inline void registry_base::s_get_DWORD(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    int32_t vr = _DWORD;
    PROPERTY_ENTER();
    METHOD_RETURN();
}

inline void registry_base::s_get_QWORD(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    int32_t vr = _QWORD;
    PROPERTY_ENTER();
    METHOD_RETURN();
}

inline void registry_base::s_listSubKey(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<List_base> vr;

    METHOD_ENTER();

    METHOD_OVER(2, 2);

    ARG(int32_t, 0);
    ARG(exlib::string, 1);

    hr = listSubKey(v0, v1, vr);

    METHOD_RETURN();
}

inline void registry_base::s_listValue(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<List_base> vr;

    METHOD_ENTER();

    METHOD_OVER(2, 2);

    ARG(int32_t, 0);
    ARG(exlib::string, 1);

    hr = listValue(v0, v1, vr);

    METHOD_RETURN();
}

inline void registry_base::s_get(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Value> vr;

    METHOD_ENTER();

    METHOD_OVER(2, 2);

    ARG(int32_t, 0);
    ARG(exlib::string, 1);

    hr = get(v0, v1, vr);

    METHOD_RETURN();
}

inline void registry_base::s_set(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_ENTER();

    METHOD_OVER(4, 3);

    ARG(int32_t, 0);
    ARG(exlib::string, 1);
    ARG(double, 2);
    OPT_ARG(int32_t, 3, _DWORD);

    hr = set(v0, v1, v2, v3);

    METHOD_OVER(4, 3);

    ARG(int32_t, 0);
    ARG(exlib::string, 1);
    ARG(exlib::string, 2);
    OPT_ARG(int32_t, 3, _SZ);

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

inline void registry_base::s_del(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_ENTER();

    METHOD_OVER(2, 2);

    ARG(int32_t, 0);
    ARG(exlib::string, 1);

    hr = del(v0, v1);

    METHOD_VOID();
}
}

#endif
