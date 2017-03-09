/***************************************************************************
 *                                                                         *
 *   This file was automatically generated using idlc.js                   *
 *   PLEASE DO NOT EDIT!!!!                                                *
 *                                                                         *
 ***************************************************************************/

#ifndef _bson_base_H_
#define _bson_base_H_

/**
 @author Leo Hoo <lion@9465.net>
 */

#include "../object.h"

namespace fibjs
{

class Buffer_base;

class bson_base : public object_base
{
    DECLARE_CLASS(bson_base);

public:
    // bson_base
    static result_t encode(v8::Local<v8::Object> data, obj_ptr<Buffer_base>& retVal);
    static result_t decode(Buffer_base* data, v8::Local<v8::Object>& retVal);

public:
    static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        CONSTRUCT_INIT();

        Isolate* isolate = Isolate::current();

        isolate->m_isolate->ThrowException(
            isolate->NewFromUtf8("not a constructor"));
    }

public:
    static void s_encode(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_decode(const v8::FunctionCallbackInfo<v8::Value>& args);
};

}

#include "Buffer.h"

namespace fibjs
{
    inline ClassInfo& bson_base::class_info()
    {
        static ClassData::ClassMethod s_method[] = 
        {
            {"encode", s_encode, true},
            {"decode", s_decode, true}
        };

        static ClassData s_cd = 
        { 
            "bson", true, s__new, NULL, 
            ARRAYSIZE(s_method), s_method, 0, NULL, 0, NULL, NULL, NULL,
            NULL
        };

        static ClassInfo s_ci(s_cd);
        return s_ci;
    }

    inline void bson_base::s_encode(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        obj_ptr<Buffer_base> vr;

        METHOD_ENTER();

        METHOD_OVER(1, 1);

        ARG(v8::Local<v8::Object>, 0);

        hr = encode(v0, vr);

        METHOD_RETURN();
    }

    inline void bson_base::s_decode(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        v8::Local<v8::Object> vr;

        METHOD_ENTER();

        METHOD_OVER(1, 1);

        ARG(obj_ptr<Buffer_base>, 0);

        hr = decode(v0, vr);

        METHOD_RETURN();
    }

}

#endif

