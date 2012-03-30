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

#include <fiber.h>

namespace fibjs
{

#define PROPERTY_ENTER() result_t hr = 0;

#define METHOD_ENTER(c, o) \
    result_t hr = 0; \
    int argc; \
    argc = args.Length(); \
    if((c) >= 0 && argc > (c))return ThrowException(v8::String::NewSymbol("Invalid number of parameters.")); \
    if((o) > 0 && argc < (o))return ThrowException(v8::String::NewSymbol("Parameter not optional."));


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
    if(pInst == NULL)return ThrowException(v8::String::NewSymbol("Object is not an instance of declaring class."));

#define PROPERTY_INSTANCE(cls) \
    cls* pInst = (cls*)cls::info().getInstance(info.This()); \
    if(pInst == NULL)return ThrowException(v8::String::NewSymbol("Object is not an instance of declaring class."));

#define PROPERTY_SET_INSTANCE(cls) \
    cls* pInst = (cls*)cls::info().getInstance(info.This()); \
    if(pInst == NULL){ThrowException(v8::String::NewSymbol("Object is not an instance of declaring class."));return;}

typedef int result_t;

class object_base
{
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
                  ClassInfo *base = NULL)
        {
            v8::HandleScope handle_scope;

            m_class = v8::Persistent<v8::FunctionTemplate>::New(v8::FunctionTemplate::New());

            m_class->SetClassName(v8::String::NewSymbol(name));

            if(base)
                m_class->Inherit(base->m_class);

            v8::Handle<v8::ObjectTemplate> ot = m_class->InstanceTemplate();
            ot->SetInternalFieldCount(1);

            v8::Local<v8::ObjectTemplate> pt = m_class->PrototypeTemplate();
            int i;

            pt->MarkAsUndetectable();

            for(i = 0; i < mc; i ++)
                pt->Set(cms[i].name, v8::FunctionTemplate::New(cms[i].invoker));

            for(i = 0; i < pc; i ++)
                if(cps[i].setter)
                    pt->SetAccessor(v8::String::NewSymbol(cps[i].name), cps[i].getter, cps[i].setter);
                else
                    pt->SetAccessor(v8::String::NewSymbol(cps[i].name), cps[i].getter, block_set);

            m_cache = v8::Persistent<v8::Object>::New(m_class->GetFunction()->NewInstance());
        }

        void* getInstance(v8::Handle<v8::Value> o)
        {
            if(o.IsEmpty() || !o->IsObject() || !m_class->HasInstance(o))
                return NULL;

            return o->ToObject()->GetPointerFromInternalField(0);
        }

        v8::Handle<v8::Object> CreateInstance() const
        {
            return m_cache->Clone();
        }

    private:
        static void block_set(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::AccessorInfo &info)
        {
            std::string strError = "Property \'";

            strError += *v8::String::Utf8Value(property);
            strError += "\' is read-only.";
            ThrowException(v8::String::NewSymbol(strError.c_str(), strError.length()));
        }

    private:
        v8::Persistent<v8::FunctionTemplate> m_class;
        v8::Persistent<v8::Object> m_cache;
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
            m_value = obj->ToJSObject();
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
        return ThrowException(v8::String::NewSymbol(strerror(-hr)));
    }

public:
    static result_t Error()
    {
#ifdef _WIN32
        return GetLastError();
#else
        return  - errno;
#endif
    }

private:
#define GC_LEVEL    10
#define GC_MASK    ((1 << GC_LEVEL) - 1)

    class GC
    {
    public:
        GC() : m_count(1)
        {
        }

    public:
        void Collect()
        {
        }

        void Register(object_base* obj)
        {
            int i, n = 1 << (GC_LEVEL - 1), m = GC_MASK;
            object_base *o;

            m_count = (m_count + 1) & GC_MASK;

            for(i = GC_LEVEL; i >= 0; i --, n >>= 1, m >>= 1)
                if(n == (m_count & m))
                {
                    object_base *pHead = NULL;

                    while(((o = gcs[i].get()) != NULL) && (o != pHead))
                    {
                        if(o->try_dispose())
                            o->Unref();
                        else if(i == GC_LEVEL)
                        {
                            gcs[i].put(o);
                            if(pHead == NULL)
                                pHead = o;
                        }else gcs[i + 1].put(o);
                    }
                }

            obj->Ref();
            gcs[0].put(obj);
        }

    private:
        int m_count;
        fiber::List<object_base> gcs[GC_LEVEL + 1];
    };

public:
    static GC& getGC()
    {
        static GC gc;
        return gc;
    }

//----------------------------------------------------------------------

public:
    object_base()
    {
        refs_ = 0;
        m_next = NULL;
    }

    virtual ~object_base()
    {
    }

    void Ref()
    {
        refs_ ++;
    }

    void Unref()
    {
        if (--refs_ == 0)
            delete this;
    }

    bool try_dispose()
    {
        if(handle_.IsEmpty())
            return true;

        if(handle_.IsWeak())
        {
            dispose();
            return true;
        }

        return false;
    }

public:
    object_base* m_next;

private:
    static void WeakCallback(v8::Persistent<v8::Value> value, void* data)
    {
        (static_cast<object_base*>(data))->dispose();
    }

protected:
    v8::Persistent<v8::Object> handle_;
    int refs_;

    v8::Handle<v8::Value> wrap(ClassInfo& i)
    {
        if (handle_.IsEmpty())
        {
            handle_ = v8::Persistent<v8::Object>::New(i.CreateInstance());
            handle_->SetPointerInInternalField(0, this);

            handle_.MakeWeak(this, WeakCallback);
            handle_.MarkIndependent();

            v8::V8::AdjustAmountOfExternalAllocatedMemory(128);

            getGC().Register(this);

            Ref();
        }

        return handle_;
    }

public:
    // object_base
    result_t dispose()
    {
        if (!handle_.IsEmpty()) {
            v8::V8::AdjustAmountOfExternalAllocatedMemory(-128);

            handle_.ClearWeak();
            handle_->SetInternalField(0, v8::Undefined());
            handle_.Dispose();
            handle_.Clear();

            Unref();
        }

        return 0;
    }

    virtual result_t toString(std::string& retVal)
    {
        retVal = "[Native Object]";
        return 0;
    }

public:
    static ClassInfo& info()
    {
        static ClassMethod s_method[] =
        {
            {"dispose", m_dispose},
            {"toString", m_toString}
        };

        static ClassInfo s_ci("object", 2, s_method, 0, NULL);

        return s_ci;
    }

    virtual v8::Handle<v8::Value> ToJSObject()
    {
        //GC_Collect(this);
        return wrap(info());
    }

private:
    static v8::Handle<v8::Value> m_dispose(const v8::Arguments& args)
    {
        METHOD_ENTER(0, 0);
        METHOD_INSTANCE(object_base);

        hr = pInst->dispose();

        METHOD_VOID();
    }

    static v8::Handle<v8::Value> m_toString(const v8::Arguments& args)
    {
        METHOD_ENTER(0, 0);
        METHOD_INSTANCE(object_base);

        std::string vr;
        hr = pInst->toString(vr);

        METHOD_RETURN();
    }
};

}

#endif

