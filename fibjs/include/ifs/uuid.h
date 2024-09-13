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

class uuid_base : public object_base {
    DECLARE_CLASS(uuid_base);

public:
    enum {
        C_DNS = 0,
        C_URL = 1,
        C_OID = 2,
        C_X509 = 3
    };

public:
    // uuid_base
    static result_t node(obj_ptr<Buffer_base>& retVal);
    static result_t md5(int32_t ns, exlib::string name, obj_ptr<Buffer_base>& retVal);
    static result_t random(obj_ptr<Buffer_base>& retVal);
    static result_t sha1(int32_t ns, exlib::string name, obj_ptr<Buffer_base>& retVal);
    static result_t snowflake(obj_ptr<Buffer_base>& retVal);
    static result_t get_hostID(int32_t& retVal);
    static result_t set_hostID(int32_t newVal);

public:
    static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        CONSTRUCT_INIT();

        isolate->m_isolate->ThrowException(
            isolate->NewString("not a constructor"));
    }

public:
    static void s_static_node(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_md5(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_random(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_sha1(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_snowflake(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_get_hostID(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_set_hostID(const v8::FunctionCallbackInfo<v8::Value>& args);
};
}

#include "ifs/Buffer.h"

namespace fibjs {
inline ClassInfo& uuid_base::class_info()
{
    static ClassData::ClassMethod s_method[] = {
        { "node", s_static_node, true, ClassData::ASYNC_SYNC },
        { "md5", s_static_md5, true, ClassData::ASYNC_SYNC },
        { "random", s_static_random, true, ClassData::ASYNC_SYNC },
        { "sha1", s_static_sha1, true, ClassData::ASYNC_SYNC },
        { "snowflake", s_static_snowflake, true, ClassData::ASYNC_SYNC }
    };

    static ClassData::ClassProperty s_property[] = {
        { "hostID", s_static_get_hostID, s_static_set_hostID, true }
    };

    static ClassData::ClassConst s_const[] = {
        { "DNS", C_DNS },
        { "URL", C_URL },
        { "OID", C_OID },
        { "X509", C_X509 }
    };

    static ClassData s_cd = {
        "uuid", true, s__new, NULL,
        ARRAYSIZE(s_method), s_method, 0, NULL, ARRAYSIZE(s_property), s_property, ARRAYSIZE(s_const), s_const, NULL, NULL,
        &object_base::class_info(),
        false
    };

    static ClassInfo s_ci(s_cd);
    return s_ci;
}

inline void uuid_base::s_static_node(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<Buffer_base> vr;

    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = node(vr);

    METHOD_RETURN();
}

inline void uuid_base::s_static_md5(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<Buffer_base> vr;

    METHOD_ENTER();

    METHOD_OVER(2, 2);

    ARG(int32_t, 0);
    ARG(exlib::string, 1);

    hr = md5(v0, v1, vr);

    METHOD_RETURN();
}

inline void uuid_base::s_static_random(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<Buffer_base> vr;

    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = random(vr);

    METHOD_RETURN();
}

inline void uuid_base::s_static_sha1(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<Buffer_base> vr;

    METHOD_ENTER();

    METHOD_OVER(2, 2);

    ARG(int32_t, 0);
    ARG(exlib::string, 1);

    hr = sha1(v0, v1, vr);

    METHOD_RETURN();
}

inline void uuid_base::s_static_snowflake(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<Buffer_base> vr;

    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = snowflake(vr);

    METHOD_RETURN();
}

inline void uuid_base::s_static_get_hostID(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    int32_t vr;

    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = get_hostID(vr);

    METHOD_RETURN();
}

inline void uuid_base::s_static_set_hostID(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(int32_t, 0);

    hr = set_hostID(v0);

    METHOD_VOID();
}
}
