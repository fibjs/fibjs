/***************************************************************************
 *                                                                         *
 *   This file was automatically generated using idlc.js                   *
 *   PLEASE DO NOT EDIT!!!!                                                *
 *                                                                         *
 ***************************************************************************/

#ifndef _rpc_base_H_
#define _rpc_base_H_

/**
 @author Leo Hoo <lion@9465.net>
 */

#include "../object.h"

namespace fibjs
{

class Handler_base;
class RpcTask_base;

class rpc_base : public object_base
{
    DECLARE_CLASS(rpc_base);

public:
    // rpc_base
    static result_t json(v8::Local<v8::Value> hdlr, obj_ptr<Handler_base>& retVal);
    static result_t open(exlib::string id, obj_ptr<RpcTask_base>& retVal);

public:
    static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        CONSTRUCT_INIT();

        Isolate* isolate = Isolate::current();

        isolate->m_isolate->ThrowException(
            isolate->NewFromUtf8("not a constructor"));
    }

public:
    static void s_json(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_open(const v8::FunctionCallbackInfo<v8::Value>& args);
};

}

#include "Handler.h"
#include "RpcTask.h"

namespace fibjs
{
    inline ClassInfo& rpc_base::class_info()
    {
        static ClassData::ClassMethod s_method[] = 
        {
            {"json", s_json, true},
            {"open", s_open, true}
        };

        static ClassData s_cd = 
        { 
            "rpc", s__new, NULL, 
            2, s_method, 0, NULL, 0, NULL, NULL, NULL,
            NULL
        };

        static ClassInfo s_ci(s_cd);
        return s_ci;
    }


    inline void rpc_base::s_json(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        obj_ptr<Handler_base> vr;

        METHOD_ENTER();

        METHOD_OVER(1, 1);

        ARG(v8::Local<v8::Value>, 0);

        hr = json(v0, vr);

        METHOD_RETURN();
    }

    inline void rpc_base::s_open(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        obj_ptr<RpcTask_base> vr;

        METHOD_ENTER();

        METHOD_OVER(1, 1);

        ARG(exlib::string, 0);

        hr = open(v0, vr);

        METHOD_RETURN();
    }

}

#endif

