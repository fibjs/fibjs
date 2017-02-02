/***************************************************************************
 *                                                                         *
 *   This file was automatically generated using idlc.js                   *
 *   PLEASE DO NOT EDIT!!!!                                                *
 *                                                                         *
 ***************************************************************************/

#ifndef _object_base_H_
#define _object_base_H_

/**
 @author Leo Hoo <lion@9465.net>
 */

#include "../object.h"

namespace fibjs
{

class object_base : public object_base
{
    DECLARE_CLASS(object_base);

public:
    // object_base
    virtual result_t dispose() = 0;
    virtual result_t equals(object_base* expected, bool& retVal) = 0;
    virtual result_t toString(exlib::string& retVal) = 0;
    virtual result_t toJSON(exlib::string key, v8::Local<v8::Value>& retVal) = 0;
    virtual result_t valueOf(v8::Local<v8::Value>& retVal) = 0;

public:
    static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        CONSTRUCT_INIT();

        Isolate* isolate = Isolate::current();

        isolate->m_isolate->ThrowException(
            isolate->NewFromUtf8("not a constructor"));
    }

public:
    static void s_dispose(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_equals(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_toString(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_toJSON(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_valueOf(const v8::FunctionCallbackInfo<v8::Value>& args);
};

}

namespace fibjs
{
    inline ClassInfo& object_base::class_info()
    {
        static ClassData::ClassMethod s_method[] = 
        {
            {"dispose", s_dispose, false},
            {"equals", s_equals, false},
            {"toString", s_toString, false},
            {"toJSON", s_toJSON, false},
            {"valueOf", s_valueOf, false}
        };

        static ClassData s_cd = 
        { 
            "object", s__new, NULL, 
            5, s_method, 0, NULL, 0, NULL, NULL, NULL,
            NULL
        };

        static ClassInfo s_ci(s_cd);
        return s_ci;
    }


    inline void object_base::s_dispose(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        METHOD_INSTANCE(object_base);
        METHOD_ENTER();

        METHOD_OVER(0, 0);

        hr = pInst->dispose();

        METHOD_VOID();
    }

    inline void object_base::s_equals(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        bool vr;

        METHOD_INSTANCE(object_base);
        METHOD_ENTER();

        METHOD_OVER(1, 1);

        ARG(obj_ptr<object_base>, 0);

        hr = pInst->equals(v0, vr);

        METHOD_RETURN();
    }

    inline void object_base::s_toString(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        exlib::string vr;

        METHOD_INSTANCE(object_base);
        METHOD_ENTER();

        METHOD_OVER(0, 0);

        hr = pInst->toString(vr);

        METHOD_RETURN();
    }

    inline void object_base::s_toJSON(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        v8::Local<v8::Value> vr;

        METHOD_INSTANCE(object_base);
        METHOD_ENTER();

        METHOD_OVER(1, 0);

        OPT_ARG(exlib::string, 0, "");

        hr = pInst->toJSON(v0, vr);

        METHOD_RETURN();
    }

    inline void object_base::s_valueOf(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        v8::Local<v8::Value> vr;

        METHOD_INSTANCE(object_base);
        METHOD_ENTER();

        METHOD_OVER(0, 0);

        hr = pInst->valueOf(vr);

        METHOD_RETURN();
    }

}

#endif

