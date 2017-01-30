/***************************************************************************
 *                                                                         *
 *   This file was automatically generated using idlc.js                   *
 *   PLEASE DO NOT EDIT!!!!                                                *
 *                                                                         *
 ***************************************************************************/

#ifndef _uuid_base_H_
#define _uuid_base_H_

/**
 @author Leo Hoo <lion@9465.net>
 */

#include "../object.h"

namespace fibjs
{

class Buffer_base;

class uuid_base : public object_base
{
    DECLARE_CLASS(uuid_base);

public:
    enum{
        _DNS = 0,
        _URL = 1,
        _OID = 2,
        _X509 = 3
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

        Isolate* isolate = Isolate::current();

        isolate->m_isolate->ThrowException(
            isolate->NewFromUtf8("not a constructor"));
    }

public:
    static void s_get_DNS(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
    static void s_get_URL(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
    static void s_get_OID(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
    static void s_get_X509(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
    static void s_node(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_md5(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_random(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_sha1(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_snowflake(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_hostID(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
    static void s_set_hostID(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void> &args);
};

}

#include "Buffer.h"

namespace fibjs
{
    inline ClassInfo& uuid_base::class_info()
    {
        static ClassData::ClassMethod s_method[] = 
        {
            {"node", s_node, true},
            {"md5", s_md5, true},
            {"random", s_random, true},
            {"sha1", s_sha1, true},
            {"snowflake", s_snowflake, true}
        };

        static ClassData::ClassProperty s_property[] = 
        {
            {"DNS", s_get_DNS, block_set, true},
            {"URL", s_get_URL, block_set, true},
            {"OID", s_get_OID, block_set, true},
            {"X509", s_get_X509, block_set, true},
            {"hostID", s_get_hostID, s_set_hostID, true}
        };

        static ClassData s_cd = 
        { 
            "uuid", s__new, NULL, 
            5, s_method, 0, NULL, 5, s_property, NULL, NULL,
            NULL
        };

        static ClassInfo s_ci(s_cd);
        return s_ci;
    }

    inline void uuid_base::s_get_DNS(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
    {
        int32_t vr = _DNS;
        PROPERTY_ENTER();
        METHOD_RETURN();
    }

    inline void uuid_base::s_get_URL(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
    {
        int32_t vr = _URL;
        PROPERTY_ENTER();
        METHOD_RETURN();
    }

    inline void uuid_base::s_get_OID(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
    {
        int32_t vr = _OID;
        PROPERTY_ENTER();
        METHOD_RETURN();
    }

    inline void uuid_base::s_get_X509(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
    {
        int32_t vr = _X509;
        PROPERTY_ENTER();
        METHOD_RETURN();
    }

    inline void uuid_base::s_get_hostID(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
    {
        int32_t vr;

        PROPERTY_ENTER();

        hr = get_hostID(vr);

        METHOD_RETURN();
    }

    inline void uuid_base::s_set_hostID(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void> &args)
    {
        PROPERTY_ENTER();
        PROPERTY_VAL(int32_t);

        hr = set_hostID(v0);

        PROPERTY_SET_LEAVE();
    }

    inline void uuid_base::s_node(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        obj_ptr<Buffer_base> vr;

        METHOD_ENTER(0, 0);

        hr = node(vr);

        METHOD_RETURN();
    }

    inline void uuid_base::s_md5(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        obj_ptr<Buffer_base> vr;

        METHOD_ENTER(2, 2);

        ARG(int32_t, 0);
        ARG(exlib::string, 1);

        hr = md5(v0, v1, vr);

        METHOD_RETURN();
    }

    inline void uuid_base::s_random(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        obj_ptr<Buffer_base> vr;

        METHOD_ENTER(0, 0);

        hr = random(vr);

        METHOD_RETURN();
    }

    inline void uuid_base::s_sha1(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        obj_ptr<Buffer_base> vr;

        METHOD_ENTER(2, 2);

        ARG(int32_t, 0);
        ARG(exlib::string, 1);

        hr = sha1(v0, v1, vr);

        METHOD_RETURN();
    }

    inline void uuid_base::s_snowflake(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        obj_ptr<Buffer_base> vr;

        METHOD_ENTER(0, 0);

        hr = snowflake(vr);

        METHOD_RETURN();
    }

}

#endif

