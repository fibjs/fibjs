/***************************************************************************
 *                                                                         *
 *   This file was automatically generated using idlc.js                   *
 *   PLEASE DO NOT EDIT!!!!                                                *
 *                                                                         *
 ***************************************************************************/

#ifndef _re_base_H_
#define _re_base_H_

/**
 @author Leo Hoo <lion@9465.net>
 */

#include "../object.h"

namespace fibjs
{

class Regex_base;

class re_base : public object_base
{
    DECLARE_CLASS(re_base);

public:
    // re_base
    static result_t compile(exlib::string pattern, exlib::string opt, obj_ptr<Regex_base>& retVal);

public:
    static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        CONSTRUCT_INIT();

        Isolate* isolate = Isolate::current();

        isolate->m_isolate->ThrowException(
            isolate->NewFromUtf8("not a constructor"));
    }

public:
    static void s_compile(const v8::FunctionCallbackInfo<v8::Value>& args);
};

}

#include "Regex.h"

namespace fibjs
{
    inline ClassInfo& re_base::class_info()
    {
        static ClassData::ClassMethod s_method[] = 
        {
            {"compile", s_compile, true}
        };

        static ClassData s_cd = 
        { 
            "re", true, s__new, NULL, 
            ARRAYSIZE(s_method), s_method, 0, NULL, 0, NULL, NULL, NULL,
            &object_base::class_info()
        };

        static ClassInfo s_ci(s_cd);
        return s_ci;
    }

    inline void re_base::s_compile(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        obj_ptr<Regex_base> vr;

        METHOD_ENTER();

        METHOD_OVER(2, 1);

        ARG(exlib::string, 0);
        OPT_ARG(exlib::string, 1, "");

        hr = compile(v0, v1, vr);

        METHOD_RETURN();
    }

}

#endif

