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
#include <string.h>
#endif

namespace fibjs
{

typedef int result_t;

class object_base
{
public:
    virtual ~object_base()
    {}

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

    struct ClassProperty
    {
        const char* name;
        v8::AccessorGetter getter;
        v8::AccessorSetter setter;
    };

    struct ClassMethod
    {
        const char* name;
        v8::InvocationCallback invoker;
    };

    class ClassInfo
    {
    public:
        ClassInfo(const char* name,
                  int mc, const ClassMethod* cms,
                  int pc, const ClassProperty* cps,
                  int smc, const ClassMethod* scms,
                  int spc, const ClassProperty* scps)
        {
            v8::HandleScope handle_scope;
            int i;

            {
                v8::Handle<v8::FunctionTemplate> ft = v8::FunctionTemplate::New();
                ft->SetClassName(v8::String::New(name));
                v8::Handle<v8::ObjectTemplate> module = ft->InstanceTemplate();

                for(i = 0; i < smc; i ++)
                    module->Set(scms[i].name, v8::FunctionTemplate::New(scms[i].invoker));

                for(i = 0; i < spc; i ++)
                    if(scps[i].setter)
                        module->SetAccessor(v8::String::New(scps[i].name), scps[i].getter, scps[i].setter);
                    else
                        module->SetAccessor(v8::String::New(scps[i].name), scps[i].getter, block_set);

                module->MarkAsUndetectable();
                m_module = v8::Persistent<v8::Object>::New(module->NewInstance());
            }

            if(mc > 0 || pc > 0)
            {
                m_class = v8::Persistent<v8::FunctionTemplate>::New(v8::FunctionTemplate::New());
                m_class->SetClassName(v8::String::New(name));
                v8::Handle<v8::ObjectTemplate> _class = m_class->InstanceTemplate();

                _class->SetInternalFieldCount(1);

                for(i = 0; i < mc; i ++)
                    _class->Set(cms[i].name, v8::FunctionTemplate::New(cms[i].invoker));

                for(i = 0; i < pc; i ++)
                    if(cps[i].setter)
                        _class->SetAccessor(v8::String::New(cps[i].name), cps[i].getter, cps[i].setter);
                    else
                        _class->SetAccessor(v8::String::New(cps[i].name), cps[i].getter, block_set);

                _class->MarkAsUndetectable();
            }
        }

        v8::Handle<v8::Value> wrap(void* o)
        {
            if(m_class.IsEmpty())
                return ThrowException(v8::String::New("no class."));

            v8::Local<v8::Object> obj = m_class->GetFunction()->NewInstance();
            obj->SetPointerInInternalField(0, o);

            v8::V8::AdjustAmountOfExternalAllocatedMemory(128);

            v8::Persistent<v8::Object>::New(obj).MakeWeak(NULL, WeakCallback);

            return obj;
        }

        void* getInstance(v8::Handle<v8::Value> o)
        {
            if(m_class.IsEmpty() || o.IsEmpty() || !o->IsObject() || !m_class->HasInstance(o))
                return NULL;

            return v8::Handle<v8::Object>::Cast(o)->GetPointerFromInternalField(0);
        }

        operator v8::Handle<v8::Value>() const
        {
            return m_module;
        }

    private:
        static void WeakCallback(v8::Persistent<v8::Value> value, void* data)
        {
            v8::Object* obj = v8::Object::Cast(*value);

            delete (object_base*)obj->GetPointerFromInternalField(0);

            v8::V8::AdjustAmountOfExternalAllocatedMemory(-128);

            value.ClearWeak();
            value.Dispose();
        }

        static void block_set(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::AccessorInfo &info)
        {
            ThrowException(v8::String::New("Property is ReadOnly."));
        }

    private:
        v8::Persistent<v8::Object> m_module;
        v8::Persistent<v8::FunctionTemplate> m_class;
    };

    class ReturnValue
    {
    public:
        ReturnValue(int32_t v)
        {
            m_value = v8::Int32::New(v);
        }

        ReturnValue(bool v)
        {
            m_value = v ? v8::True() : v8::False();
        }

        ReturnValue(double v)
        {
            m_value = v8::Number::New(v);
        }

        ReturnValue(std::string& str)
        {
            m_value = v8::String::New(str.c_str(), str.length());
        }

        ReturnValue(v8::Handle<v8::Object>& obj)
        {
            m_value = obj;
        }

        ReturnValue(v8::Handle<v8::Array>& array)
        {
            m_value = array;
        }

        template<class T>
        ReturnValue(T* obj)
        {
            m_value = T::info().wrap(obj);
        }

        operator v8::Handle<v8::Value>() const
        {
            return m_value;
        }

    private:
        v8::Handle<v8::Value> m_value;
    };


    static v8::Handle<v8::Value> ThrowResult(result_t hr)
    {

        return ThrowException(v8::String::New(strerror(-hr)));
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

#define PROPERTY_VAL_string() \
    v8::String::Utf8Value tv0(value); \
    const char* v0 = *tv0;


#define ARG_integer(n) \
    int32_t v##n = args[n]->Int32Value();

#define OPT_ARG_integer(n, d) \
    int32_t v##n = (n) < argc ? args[n]->Int32Value() : (d);

#define PROPERTY_VAL_integer() \
    int32_t v0 = value->Int32Value();


#define ARG_number(n) \
    double v##n = args[n]->NumberValue();

#define OPT_ARG_number(n, d) \
    double v##n = (n) < argc ? args[n]->NumberValue() : (d);

#define PROPERTY_VAL_number() \
    double v0 = value->NumberValue();


#define ARG_boolean(n) \
    bool v##n = args[n]->BooleanValue();

#define OPT_ARG_boolean(n, d) \
    bool v##n = (n) < argc ? args[n]->BooleanValue() : (d);

#define PROPERTY_VAL_boolean() \
    bool v0 = value->BooleanValue();

#define PROPERTY_SET_LEAVE() \
    if(hr < 0)ThrowResult(hr);

#define METHOD_RETURN() \
    if(hr < 0)return ThrowResult(hr); \
    return ReturnValue(vr);

#define METHOD_VOID() \
    if(hr < 0)return ThrowResult(hr); \
    return v8::Undefined();


#define METHOD_INSTANCE(cls) \
    cls* pInst = (cls*)cls::info().getInstance(args.This()); \
    if(pInst == NULL)return ThrowException(v8::String::New("Not Instance."));

#define PROPERTY_INSTANCE(cls) \
    cls* pInst = (cls*)cls::info().getInstance(info.This()); \
    if(pInst == NULL)return ThrowException(v8::String::New("Not Instance."));

#define PROPERTY_SET_INSTANCE(cls) \
    cls* pInst = (cls*)cls::info().getInstance(info.This()); \
    if(pInst == NULL){ThrowException(v8::String::New("Not Instance."));return;}

}

#endif

