/***************************************************************************
 *                                                                         *
 *   This file was automatically generated using idlc.js                   *
 *   PLEASE DO NOT EDIT!!!!                                                *
 *                                                                         *
 ***************************************************************************/

#ifndef _global_base_H_
#define _global_base_H_

/**
 @author Leo Hoo <lion@9465.net>
 */

#include "../object.h"

namespace fibjs
{

class Buffer_base;
class Int64_base;
class console_base;
class Stream_base;

class global_base : public object_base
{
    DECLARE_CLASS(global_base);

public:
    // global_base
    static result_t run(const char* fname, v8::Local<v8::Array> argv);
    static result_t get_argv(v8::Local<v8::Array>& retVal);
    static result_t get___filename(std::string& retVal);
    static result_t get___dirname(std::string& retVal);
    static result_t get___sbname(std::string& retVal);
    static result_t require(const char* id, v8::Local<v8::Value>& retVal);
    static result_t GC();
    static result_t repl(v8::Local<v8::Array> cmds);
    static result_t repl(Stream_base* out, v8::Local<v8::Array> cmds);

public:
    static void s_run(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_argv(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
    static void s_get___filename(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
    static void s_get___dirname(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
    static void s_get___sbname(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
    static void s_require(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_GC(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_repl(const v8::FunctionCallbackInfo<v8::Value>& args);
};

}

#include "Buffer.h"
#include "Int64.h"
#include "console.h"
#include "Stream.h"

namespace fibjs
{
    inline ClassInfo& global_base::class_info()
    {
        static ClassData::ClassMethod s_method[] = 
        {
            {"run", s_run, true},
            {"require", s_require, true},
            {"GC", s_GC, true},
            {"repl", s_repl, true}
        };

        static ClassData::ClassObject s_object[] = 
        {
            {"Buffer", Buffer_base::class_info},
            {"Int64", Int64_base::class_info},
            {"console", console_base::class_info}
        };

        static ClassData::ClassProperty s_property[] = 
        {
            {"argv", s_get_argv, block_set, true},
            {"__filename", s_get___filename, block_set, true},
            {"__dirname", s_get___dirname, block_set, true},
            {"__sbname", s_get___sbname, block_set, true}
        };

        static ClassData s_cd = 
        { 
            "global", NULL, 
            4, s_method, 3, s_object, 4, s_property, NULL, NULL,
            NULL
        };

        static ClassInfo s_ci(s_cd);
        return s_ci;
    }

    inline void global_base::s_get_argv(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
    {
        v8::Local<v8::Array> vr;

        PROPERTY_ENTER();

        hr = get_argv(vr);

        METHOD_RETURN();
    }

    inline void global_base::s_get___filename(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
    {
        std::string vr;

        PROPERTY_ENTER();

        hr = get___filename(vr);

        METHOD_RETURN();
    }

    inline void global_base::s_get___dirname(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
    {
        std::string vr;

        PROPERTY_ENTER();

        hr = get___dirname(vr);

        METHOD_RETURN();
    }

    inline void global_base::s_get___sbname(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
    {
        std::string vr;

        PROPERTY_ENTER();

        hr = get___sbname(vr);

        METHOD_RETURN();
    }

    inline void global_base::s_run(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        METHOD_ENTER(2, 1);

        ARG(arg_string, 0);
        OPT_ARG(v8::Local<v8::Array>, 1, v8::Array::New(Isolate::now()->m_isolate));

        hr = run(v0, v1);

        METHOD_VOID();
    }

    inline void global_base::s_require(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        v8::Local<v8::Value> vr;

        METHOD_ENTER(1, 1);

        ARG(arg_string, 0);

        hr = require(v0, vr);

        METHOD_RETURN();
    }

    inline void global_base::s_GC(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        METHOD_ENTER(0, 0);

        hr = GC();

        METHOD_VOID();
    }

    inline void global_base::s_repl(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        METHOD_ENTER(1, 0);

        OPT_ARG(v8::Local<v8::Array>, 0, v8::Array::New(Isolate::now()->m_isolate));

        hr = repl(v0);

        METHOD_OVER(2, 1);

        ARG(obj_ptr<Stream_base>, 0);
        OPT_ARG(v8::Local<v8::Array>, 1, v8::Array::New(Isolate::now()->m_isolate));

        hr = repl(v0, v1);

        METHOD_VOID();
    }

}

#endif

