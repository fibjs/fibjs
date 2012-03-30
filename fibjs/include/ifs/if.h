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

#include <stdlib.h>

#include <fiber.h>

namespace fibjs
{

#define PROPERTY_ENTER() result_t hr = 0;

#define METHOD_ENTER(c, o) \
    result_t hr = 0; \
    int argc; \
    argc = args.Length(); \
    if((c) >= 0 && argc > (c))return ThrowTypeError("Invalid number of parameters."); \
    if((o) > 0 && argc < (o))return ThrowTypeError("Parameter not optional.");

#define CONSTRUCT_ENTER(c, o) \
    if (!args.IsConstructCall())return ThrowTypeError("Constructor cannot be called as a function."); \
    METHOD_ENTER(c, o)

#define ARG_String(n) \
    v8::String::Utf8Value tv##n(args[n]); \
    const char* v##n = *tv##n;

#define OPT_ARG_String(n, d) \
    v8::Local<v8::Value> tvv##n; \
    if(n < argc)tvv##n = args[n]; \
    v8::String::Utf8Value tv##n(tvv##n); \
    const char* v##n = (n) < argc ? *tv##n : (d);

#define PROPERTY_VAL_String() \
    v8::String::Utf8Value tv0(value); \
    const char* v0 = *tv0;


#define ARG_Integer(n) \
    int32_t v##n = args[n]->Int32Value();

#define OPT_ARG_Integer(n, d) \
    int32_t v##n = (n) < argc ? args[n]->Int32Value() : (d);

#define PROPERTY_VAL_Integer() \
    int32_t v0 = value->Int32Value();


#define ARG_Number(n) \
    double v##n = args[n]->NumberValue();

#define OPT_ARG_Number(n, d) \
    double v##n = (n) < argc ? args[n]->NumberValue() : (d);

#define PROPERTY_VAL_Number() \
    double v0 = value->NumberValue();


#define ARG_Boolean(n) \
    bool v##n = args[n]->BooleanValue();

#define OPT_ARG_Boolean(n, d) \
    bool v##n = (n) < argc ? args[n]->BooleanValue() : (d);

#define PROPERTY_VAL_Boolean() \
    bool v0 = value->BooleanValue();

#define PROPERTY_SET_LEAVE() \
    if(hr < 0)ThrowResult(hr);

#define METHOD_RETURN() \
    if(hr < 0)return ThrowResult(hr); \
    return ReturnValue(vr);

#define METHOD_VOID() \
    if(hr < 0)return ThrowResult(hr); \
    return v8::Undefined();


#define CONSTRUCT_RETURN() \
    if(hr < 0)return ThrowResult(hr); \
    return vr->wrapThis(args.This());


#define METHOD_INSTANCE(cls) \
    cls* pInst = (cls*)cls::info().getInstance(args.This()); \
    if(pInst == NULL)return ThrowTypeError("Object is not an instance of declaring class.");

#define PROPERTY_INSTANCE(cls) \
    cls* pInst = (cls*)cls::info().getInstance(info.This()); \
    if(pInst == NULL)return ThrowTypeError("Object is not an instance of declaring class.");

#define PROPERTY_SET_INSTANCE(cls) \
    cls* pInst = (cls*)cls::info().getInstance(info.This()); \
    if(pInst == NULL){ThrowTypeError("Object is not an instance of declaring class.");return;}

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

    struct ClassIndexed
    {
        v8::IndexedPropertyGetter getter;
        v8::IndexedPropertySetter setter;
    };

    class ClassInfo
    {
    public:
        ClassInfo(const char* name, v8::InvocationCallback cor,
                  int mc, const ClassMethod* cms,
                  int pc, const ClassProperty* cps,
                  const ClassIndexed* cis,
                  ClassInfo *base = NULL)
        {
            v8::HandleScope handle_scope;

            m_class = v8::Persistent<v8::FunctionTemplate>::New(v8::FunctionTemplate::New(cor));

            m_class->SetClassName(v8::String::NewSymbol(name));

            if(base)
                m_class->Inherit(base->m_class);

            v8::Handle<v8::ObjectTemplate> ot = m_class->InstanceTemplate();

            ot->SetInternalFieldCount(1);
            if(cis)
                ot->SetIndexedPropertyHandler(cis->getter, cis->setter);

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

        v8::Handle<v8::FunctionTemplate> getTemplate() const
        {
            return m_class;
        }

    protected:
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

    static v8::Handle<v8::Value> ThrowError(const char* msg)
    {
        return v8::ThrowException(v8::Exception::Error(v8::String::New(msg)));
    }

    static v8::Handle<v8::Value> ThrowTypeError(const char* msg)
    {
        return v8::ThrowException(v8::Exception::TypeError(v8::String::New(msg)));
    }

    static v8::Handle<v8::Value> ThrowRangeError(const char* msg)
    {
        return v8::ThrowException(v8::Exception::RangeError(v8::String::New(msg)));
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

#define GC_MAX  20480

    class GC
    {
    public:
        GC() : m_count(0)
        {
        }

    public:
        void Counter(int n = 1)
        {
            m_count += n;

            if(m_count < 0)
                m_count = 0;

            if(GC_MAX <= m_count)
            {
                v8::V8::LowMemoryNotification();
                m_count = 0;
            }
        }

    private:
        int m_count;
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
            wrapThis(i.CreateInstance());

        return handle_;
    }

    v8::Handle<v8::Value> wrapThis(v8::Handle<v8::Object> o)
    {
        if (handle_.IsEmpty())
        {
            handle_ = v8::Persistent<v8::Object>::New(o);
            handle_->SetPointerInInternalField(0, this);

            handle_.MakeWeak(this, WeakCallback);
            handle_.MarkIndependent();

            Ref();

            v8::V8::AdjustAmountOfExternalAllocatedMemory(1024);

            getGC().Counter();
        }

        return handle_;
    }

public:
    // object_base
    result_t dispose()
    {
        if (!handle_.IsEmpty())
        {
            handle_.ClearWeak();
            handle_->SetPointerInInternalField(0, 0);
            handle_.Dispose();
            handle_.Clear();

            Unref();

            v8::V8::AdjustAmountOfExternalAllocatedMemory(-1024);

            getGC().Counter(-1);
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

        static ClassInfo s_ci("object", NULL, 2, s_method, 0, NULL, NULL);

        return s_ci;
    }

    virtual v8::Handle<v8::Value> ToJSObject()
    {
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

