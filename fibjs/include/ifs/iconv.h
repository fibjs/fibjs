/***************************************************************************
 *                                                                         *
 *   This file was automatically generated using idlc.js                   *
 *   PLEASE DO NOT EDIT!!!!                                                *
 *                                                                         *
 ***************************************************************************/

#ifndef _iconv_base_H_
#define _iconv_base_H_

/**
 @author Leo Hoo <lion@9465.net>
 */

#include "../object.h"

namespace fibjs
{

class Buffer_base;

class iconv_base : public object_base
{
    DECLARE_CLASS(iconv_base);

public:
    // iconv_base
    static result_t encode(exlib::string charset, exlib::string data, obj_ptr<Buffer_base>& retVal);
    static result_t decode(exlib::string charset, Buffer_base* data, exlib::string& retVal);

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
    inline ClassInfo& iconv_base::class_info()
    {
        static ClassData::ClassMethod s_method[] = 
        {
            {"encode", s_encode, true},
            {"decode", s_decode, true}
        };

        static ClassData s_cd = 
        { 
            "iconv", true, s__new, NULL, 
            ARRAYSIZE(s_method), s_method, 0, NULL, 0, NULL, NULL, NULL,
            NULL
        };

        static ClassInfo s_ci(s_cd);
        return s_ci;
    }

    inline void iconv_base::s_encode(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        obj_ptr<Buffer_base> vr;

        METHOD_ENTER();

        METHOD_OVER(2, 2);

        ARG(exlib::string, 0);
        ARG(exlib::string, 1);

        hr = encode(v0, v1, vr);

        METHOD_RETURN();
    }

    inline void iconv_base::s_decode(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        exlib::string vr;

        METHOD_ENTER();

        METHOD_OVER(2, 2);

        ARG(exlib::string, 0);
        ARG(obj_ptr<Buffer_base>, 1);

        hr = decode(v0, v1, vr);

        METHOD_RETURN();
    }

}

#endif

