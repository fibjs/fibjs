/***************************************************************************
 *                                                                         *
 *   This file was automatically generated using idlc.js                   *
 *   PLEASE DO NOT EDIT!!!!                                                *
 *                                                                         *
 ***************************************************************************/

#ifndef _json_base_H_
#define _json_base_H_

/**
 @author Leo Hoo <lion@9465.net>
 */

#include "../object.h"

namespace fibjs
{

class json_base : public object_base
{
    DECLARE_CLASS(json_base);

public:
    // json_base
    static result_t encode(v8::Local<v8::Value> data, std::string& retVal);
    static result_t decode(const char* data, v8::Local<v8::Value>& retVal);

public:
    static void s_encode(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_decode(const v8::FunctionCallbackInfo<v8::Value>& args);
};

}

namespace fibjs
{
    inline ClassInfo& json_base::class_info()
    {
        static ClassData::ClassMethod s_method[] = 
        {
            {"encode", s_encode, true},
            {"decode", s_decode, true}
        };

        static ClassData s_cd = 
        { 
            "json", NULL, 
            2, s_method, 0, NULL, 0, NULL, NULL, NULL,
            NULL
        };

        static ClassInfo s_ci(s_cd);
        return s_ci;
    }


    inline void json_base::s_encode(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        std::string vr;

        METHOD_ENTER(1, 1);

        ARG(v8::Local<v8::Value>, 0);

        hr = encode(v0, vr);

        METHOD_RETURN();
    }

    inline void json_base::s_decode(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        v8::Local<v8::Value> vr;

        METHOD_ENTER(1, 1);

        ARG(arg_string, 0);

        hr = decode(v0, vr);

        METHOD_RETURN();
    }

}

#endif

