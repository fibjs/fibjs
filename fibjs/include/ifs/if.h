/***************************************************************************
 *   Copyright (C) 2012 by Leo Hoo                                         *
 *   lion@9465.net                                                         *
 *                                                                         *
 ***************************************************************************/

#ifndef _IF_H_
#define _IF_H_

/**
 @author Leo Hoo <lion@9465.net>
 */

#include <v8/v8.h>
#include <string>

#ifdef _WIN32
#include <windows.h>
#else
#include <errno.h>
#endif

namespace fibjs
{

typedef int result_t;

class object_base
{
public:
    static result_t Error()
    {
#ifdef _WIN32
        return GetLastError();
#else
        return  - errno;
#endif
    }
protected:
    enum MethodType
    {
        CLASS_PROPERTY,
        CLASS_METHOD
    };

    struct ClassMethod
    {
        const char* name;
        MethodType type;
        union
        {
            v8::InvocationCallback invoker;
            struct
            {
                v8::AccessorGetter getter;
                v8::AccessorSetter setter;
            };
        };
    };

    class ClassInfo
    {
    public:
        ClassInfo(const char* name, int mc, const ClassMethod* cms, int smc, const ClassMethod* scms)
        {
            v8::HandleScope handle_scope;
            int i;

            v8::Handle<v8::FunctionTemplate> ft = v8::FunctionTemplate::New();
            ft->SetClassName(v8::String::New(name));
            m_class = v8::Persistent<v8::ObjectTemplate>::New(ft->PrototypeTemplate());

            for(i = 0; i < smc; i ++)
            {
                if(scms[i].type == CLASS_METHOD)
                    m_class->Set(scms[i].name, v8::FunctionTemplate::New(scms[i].invoker));
                else if(scms[i].setter)
                    m_class->SetAccessor(v8::String::New(scms[i].name), scms[i].getter, scms[i].setter);
                else
                    m_class->SetAccessor(v8::String::New(scms[i].name), scms[i].getter, block_set);
            }

            m_class->MarkAsUndetectable();
        }

        operator v8::Handle<v8::Value>() const
        {
            return m_class->NewInstance();
        }

    private:
        static void block_set(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::AccessorInfo &info)
        {
            ThrowException(v8::String::New("Property is ReadOnly."));
        }

    private:
        v8::Persistent<v8::ObjectTemplate> m_class;
    };

    class ReturnValue
    {
    public:
        ReturnValue(int v)
        {
            m_value = v8::Integer::New(v);
        }

        ReturnValue(v8::Local<v8::Object>& obj)
        {
            m_value = obj;
        }

        ReturnValue(std::string& str)
        {
            m_value = v8::String::New(str.c_str(), str.length());
        }

        ReturnValue(v8::Local<v8::Array>& array)
        {
            m_value = array;
        }

        operator v8::Local<v8::Value>() const
        {
            return m_value;
        }

    private:
        v8::Local<v8::Value> m_value;
    };


    static v8::Handle<v8::Value> ThrowResult(result_t hr)
    {

		return ThrowException(v8::String::New("ERROR."));
    }
};

#define PROPERTY_ENTER() result_t hr = 0;

#define METHOD_ENTER(c, o) \
    result_t hr = 0; \
    int argc; \
    argc = args.Length(); \
    if((c) >= 0 && argc > (c))return ThrowException(v8::String::New("Invalid number of parameters.")); \
    if((o) > 0 && argc < (o))return ThrowException(v8::String::New("Parameter not optional."));

#define ARG_string(n) \
    v8::String::Utf8Value tv##n(args[n]); \
    const char* v##n = *tv##n;

#define OPT_ARG_string(n, d) \
    v8::Local<v8::Value> tvv##n; \
    if(n < argc)tvv##n = args[n]; \
    v8::String::Utf8Value tv##n(tvv##n); \
    const char* v##n = (n) < argc ? *tv##n : (d);


#define METHOD_RETURN() \
    if(hr < 0)return ThrowResult(hr); \
    return ReturnValue(vr);

#define METHOD_VOID() \
    if(hr < 0)return ThrowResult(hr); \
    return v8::Undefined();

}

#endif

