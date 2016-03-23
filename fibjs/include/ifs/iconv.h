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
    static result_t encode(const char* charset, const char* data, obj_ptr<Buffer_base>& retVal);
    static result_t decode(const char* charset, Buffer_base* data, std::string& retVal);

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
            "iconv", s__new, NULL, 
            2, s_method, 0, NULL, 0, NULL, NULL, NULL,
            NULL
        };

        static ClassInfo s_ci(s_cd);
        return s_ci;
    }


    inline void iconv_base::s_encode(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        obj_ptr<Buffer_base> vr;

        METHOD_ENTER(2, 2);

        ARG(arg_string, 0);
        ARG(arg_string, 1);

        hr = encode(v0, v1, vr);

        METHOD_RETURN();
    }

    inline void iconv_base::s_decode(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        std::string vr;

        METHOD_ENTER(2, 2);

        ARG(arg_string, 0);
        ARG(obj_ptr<Buffer_base>, 1);

        hr = decode(v0, v1, vr);

        METHOD_RETURN();
    }

}

#endif

