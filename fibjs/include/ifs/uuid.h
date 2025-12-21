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
    static result_t get_NIL(exlib::string& retVal);
    static result_t get_MAX(exlib::string& retVal);
    static result_t get_DNS_NAMESPACE(exlib::string& retVal);
    static result_t get_URL_NAMESPACE(exlib::string& retVal);
    static result_t parse(exlib::string uuid, obj_ptr<Buffer_base>& retVal);
    static result_t stringify(Buffer_base* arr, int32_t offset, exlib::string& retVal);
    static result_t v1(v8::Local<v8::Object> options, exlib::string& retVal);
    static result_t v3(exlib::string name, Buffer_base* ns, exlib::string& retVal);
    static result_t v3(exlib::string name, exlib::string ns, exlib::string& retVal);
    static result_t v4(v8::Local<v8::Object> options, exlib::string& retVal);
    static result_t v5(exlib::string name, Buffer_base* ns, exlib::string& retVal);
    static result_t v5(exlib::string name, exlib::string ns, exlib::string& retVal);
    static result_t version(exlib::string uuid, int32_t& retVal);
    static result_t v6(v8::Local<v8::Object> options, exlib::string& retVal);
    static result_t v7(v8::Local<v8::Object> options, exlib::string& retVal);
    static result_t v1ToV6(exlib::string uuid, exlib::string& retVal);
    static result_t v6ToV1(exlib::string uuid, exlib::string& retVal);
    static result_t validate(exlib::string uuid, bool& retVal);
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

        ThrowTypeError("not a constructor");
    }

    static result_t load(v8::Local<v8::Value> v, obj_ptr<uuid_base>& retVal)
    { return CALL_E_TYPEMISMATCH; }

public:
    static void s_static_get_NIL(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_get_MAX(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_get_DNS_NAMESPACE(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_get_URL_NAMESPACE(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_parse(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_stringify(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_v1(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_v3(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_v4(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_v5(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_version(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_v6(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_v7(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_v1ToV6(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_v6ToV1(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_validate(const v8::FunctionCallbackInfo<v8::Value>& args);
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
        { "parse", s_static_parse, true, ClassData::ASYNC_SYNC },
        { "stringify", s_static_stringify, true, ClassData::ASYNC_SYNC },
        { "v1", s_static_v1, true, ClassData::ASYNC_SYNC },
        { "v3", s_static_v3, true, ClassData::ASYNC_SYNC },
        { "v4", s_static_v4, true, ClassData::ASYNC_SYNC },
        { "v5", s_static_v5, true, ClassData::ASYNC_SYNC },
        { "version", s_static_version, true, ClassData::ASYNC_SYNC },
        { "v6", s_static_v6, true, ClassData::ASYNC_SYNC },
        { "v7", s_static_v7, true, ClassData::ASYNC_SYNC },
        { "v1ToV6", s_static_v1ToV6, true, ClassData::ASYNC_SYNC },
        { "v6ToV1", s_static_v6ToV1, true, ClassData::ASYNC_SYNC },
        { "validate", s_static_validate, true, ClassData::ASYNC_SYNC },
        { "node", s_static_node, true, ClassData::ASYNC_SYNC },
        { "md5", s_static_md5, true, ClassData::ASYNC_SYNC },
        { "random", s_static_random, true, ClassData::ASYNC_SYNC },
        { "sha1", s_static_sha1, true, ClassData::ASYNC_SYNC },
        { "snowflake", s_static_snowflake, true, ClassData::ASYNC_SYNC }
    };

    static ClassData::ClassProperty s_property[] = {
        { "NIL", s_static_get_NIL, block_set, true },
        { "MAX", s_static_get_MAX, block_set, true },
        { "DNS_NAMESPACE", s_static_get_DNS_NAMESPACE, block_set, true },
        { "URL_NAMESPACE", s_static_get_URL_NAMESPACE, block_set, true },
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

inline void uuid_base::s_static_get_NIL(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = get_NIL(vr);

    METHOD_RETURN();
}

inline void uuid_base::s_static_get_MAX(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = get_MAX(vr);

    METHOD_RETURN();
}

inline void uuid_base::s_static_get_DNS_NAMESPACE(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = get_DNS_NAMESPACE(vr);

    METHOD_RETURN();
}

inline void uuid_base::s_static_get_URL_NAMESPACE(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = get_URL_NAMESPACE(vr);

    METHOD_RETURN();
}

inline void uuid_base::s_static_parse(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<Buffer_base> vr;

    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(exlib::string, 0);

    hr = parse(v0, vr);

    METHOD_RETURN();
}

inline void uuid_base::s_static_stringify(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_ENTER();

    METHOD_OVER(2, 1);

    ARG(obj_ptr<Buffer_base>, 0);
    OPT_ARG(int32_t, 1, 0);

    hr = stringify(v0.get(), v1, vr);

    METHOD_RETURN();
}

inline void uuid_base::s_static_v1(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_ENTER();

    METHOD_OVER(1, 0);

    OPT_ARG(v8::Local<v8::Object>, 0, v8::Object::New(isolate->m_isolate));

    hr = v1(v0, vr);

    METHOD_RETURN();
}

inline void uuid_base::s_static_v3(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_ENTER();

    METHOD_OVER(2, 2);

    ARG(exlib::string, 0);
    ARG(obj_ptr<Buffer_base>, 1);

    hr = v3(v0, v1.get(), vr);

    METHOD_OVER(2, 2);

    ARG(exlib::string, 0);
    ARG(exlib::string, 1);

    hr = v3(v0, v1, vr);

    METHOD_RETURN();
}

inline void uuid_base::s_static_v4(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_ENTER();

    METHOD_OVER(1, 0);

    OPT_ARG(v8::Local<v8::Object>, 0, v8::Object::New(isolate->m_isolate));

    hr = v4(v0, vr);

    METHOD_RETURN();
}

inline void uuid_base::s_static_v5(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_ENTER();

    METHOD_OVER(2, 2);

    ARG(exlib::string, 0);
    ARG(obj_ptr<Buffer_base>, 1);

    hr = v5(v0, v1.get(), vr);

    METHOD_OVER(2, 2);

    ARG(exlib::string, 0);
    ARG(exlib::string, 1);

    hr = v5(v0, v1, vr);

    METHOD_RETURN();
}

inline void uuid_base::s_static_version(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    int32_t vr;

    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(exlib::string, 0);

    hr = version(v0, vr);

    METHOD_RETURN();
}

inline void uuid_base::s_static_v6(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_ENTER();

    METHOD_OVER(1, 0);

    OPT_ARG(v8::Local<v8::Object>, 0, v8::Object::New(isolate->m_isolate));

    hr = v6(v0, vr);

    METHOD_RETURN();
}

inline void uuid_base::s_static_v7(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_ENTER();

    METHOD_OVER(1, 0);

    OPT_ARG(v8::Local<v8::Object>, 0, v8::Object::New(isolate->m_isolate));

    hr = v7(v0, vr);

    METHOD_RETURN();
}

inline void uuid_base::s_static_v1ToV6(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(exlib::string, 0);

    hr = v1ToV6(v0, vr);

    METHOD_RETURN();
}

inline void uuid_base::s_static_v6ToV1(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(exlib::string, 0);

    hr = v6ToV1(v0, vr);

    METHOD_RETURN();
}

inline void uuid_base::s_static_validate(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    bool vr;

    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(exlib::string, 0);

    hr = validate(v0, vr);

    METHOD_RETURN();
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
