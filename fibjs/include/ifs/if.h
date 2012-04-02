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
#include <math.h>

#include <fiber.h>

namespace fibjs
{

typedef int result_t;

// Invalid number of parameters.
#define CALL_E_BADPARAMCOUNT    -1
// Parameter not optional.
#define CALL_E_PARAMNOTOPTIONAL -2
// Constructor cannot be called as a function.
#define CALL_E_CONSTRUCTOR      -3
// Object is not an instance of declaring class.
#define CALL_E_NOTINSTANCE      -4
// The input parameter is not a valid type.
#define CALL_E_BADVARTYPE       -5
// An argument is invalid.
#define CALL_E_INVALIDARG       -6
// The argument could not be coerced to the specified type.
#define CALL_E_TYPEMISMATCH     -7
// Value is out of range.
#define CALL_E_OUTRANGE         -8
// Index was out of range.
#define CALL_E_BADINDEX         -9
#define CALL_E_MAX              -10

#define PROPERTY_ENTER() result_t hr = 0;do{

#define METHOD_ENTER(c, o) \
    result_t hr = 0; do{\
    int argc; \
    argc = args.Length(); \
    if((c) >= 0 && argc > (c)){hr = CALL_E_BADPARAMCOUNT;break;} \
    if((o) > 0 && argc < (o)){hr = CALL_E_PARAMNOTOPTIONAL;break;}

#define CONSTRUCT_ENTER(c, o) \
    if (!args.IsConstructCall())return ThrowResult(CALL_E_CONSTRUCTOR); \
    METHOD_ENTER(c, o)

#define METHOD_INSTANCE(cls) \
    obj_ptr<cls> pInst = (cls*)cls::class_info().getInstance(args.This()); \
    if(pInst == NULL){hr = CALL_E_NOTINSTANCE;break;}

#define PROPERTY_INSTANCE(cls) \
    obj_ptr<cls> pInst = (cls*)cls::class_info().getInstance(info.This()); \
    if(pInst == NULL){hr = CALL_E_NOTINSTANCE;break;}


#define PROPERTY_SET_LEAVE() \
    }while(0); \
    if(hr < 0)ThrowResult(hr);

#define METHOD_RETURN() \
    if(hr >= 0)return ReturnValue(vr); \
    }while(0); \
    return ThrowResult(hr);

#define METHOD_VOID() \
    if(hr >= 0)return v8::Undefined(); \
    }while(0); \
    return ThrowResult(hr);

#define CONSTRUCT_RETURN() \
    if(hr >= 0)return vr->wrap(args.This()); \
    }while(0); \
    return ThrowResult(hr);


#define ARG_CLASS(cls, n) \
    obj_ptr<cls> v##n = (cls*)cls::class_info().getInstance(args[n]); \
    if(v##n == NULL){hr = CALL_E_NOTINSTANCE;break;}


#define ARG_String(n) \
    v8::String::Utf8Value tv##n(args[n]); \
    const char* v##n = *tv##n; \
    if(v##n == NULL){hr = CALL_E_INVALIDARG;break;}

#define OPT_ARG_String(n, d) \
    v8::Local<v8::Value> tvv##n; \
    if(n < argc)tvv##n = args[n]; \
    v8::String::Utf8Value tv##n(tvv##n); \
    const char* v##n = (n) < argc ? *tv##n : (d); \
    if(v##n == NULL){hr = CALL_E_INVALIDARG;break;}

#define PROPERTY_VAL_String() \
    v8::String::Utf8Value tv0(value); \
    const char* v0 = *tv0; \
    if(v0 == NULL){hr = CALL_E_INVALIDARG;break;}

#define ARG(t, n) \
    t v##n; \
    hr = SafeGetValue(args[n], v##n); \
    if(hr < 0)break;

#define OPT_ARG(t, n, d) \
    t v##n; \
    if((n) < argc){ \
        hr = SafeGetValue(args[n], v##n); \
        if(hr < 0)break; \
    }else v##n = (d);

#define PROPERTY_VAL(t) \
    t v0; \
    hr = SafeGetValue(value, v0); \
    if(hr < 0)break;


template<class T>
class obj_ptr
{
public:
    obj_ptr() : p(NULL)
    {}

    obj_ptr(T* lp) : p(NULL)
    {
        operator=(lp);
    }

    obj_ptr(const obj_ptr<T>& lp) : p(NULL)
    {
        operator=(lp);
    }

    ~obj_ptr()
    {
        Release();
    }

    T* operator=(T* lp)
    {
        if (lp != NULL)
            lp->Ref();

        return Attach(lp);
    }

    T* operator=(const obj_ptr<T>& lp)
    {
        return operator=(lp.p);
    }

    operator T*() const
    {
        return p;
    }

    T& operator*() const
    {
        return *p;
    }

    T** operator&()
    {
        return &p;
    }

    bool operator!() const
    {
        return (p == NULL);
    }

    bool operator==(T* pT) const
    {
        return p == pT;
    }

    T* operator->()
    {
        return p;
    }

    void Release()
    {
        T* pTemp = Detach();
        if (pTemp)
            pTemp->Unref();
    }

    T* Attach(T* p2)
    {
        T* p1 = p;
        p = p2;

        if (p1)
            p1->Unref();

        return p2;
    }

    T* Detach()
    {
        T* p1 = p;

        p = NULL;
        return p1;
    }

    T* p;
};

#ifdef _MSC_VER
#define isnan _isnan
#endif

inline result_t SafeGetValue(v8::Handle<v8::Value> v, double& n)
{
    if(v.IsEmpty())
        return CALL_E_INVALIDARG;

    n = v->NumberValue();
    if(isnan(n))
        return CALL_E_INVALIDARG;

    return 0;
}

inline result_t SafeGetValue(v8::Handle<v8::Value> v, int64_t& n)
{
    double num;

    result_t hr = SafeGetValue(v, num);
    if(hr < 0)return hr;

    if(num < -9007199254740992ll || num > 9007199254740992ll)
        return CALL_E_OUTRANGE;

    n = (int64_t)num;

    return 0;
}

inline result_t SafeGetValue(v8::Handle<v8::Value> v, int32_t& n)
{
    double num;

    result_t hr = SafeGetValue(v, num);
    if(hr < 0)return hr;

    if(num < -2147483648ll || num > 2147483647ll)
        return CALL_E_OUTRANGE;

    n = (int32_t)num;

    return 0;
}

inline result_t SafeGetValue(v8::Handle<v8::Value> v, bool& n)
{
    n = v->BooleanValue();
    return 0;
}

inline v8::Handle<v8::Value> ReturnValue(int32_t v)
{
    return v8::Int32::New(v);
}

inline v8::Handle<v8::Value> ReturnValue(bool v)
{
    return v ? v8::True() : v8::False();
}

inline v8::Handle<v8::Value> ReturnValue(double v)
{
    return v8::Number::New(v);
}

inline v8::Handle<v8::Value> ReturnValue(std::string& str)
{
    return v8::String::New(str.c_str(), str.length());
}

inline v8::Handle<v8::Value> ReturnValue(v8::Handle<v8::Object>& obj)
{
    return obj;
}

inline v8::Handle<v8::Value> ReturnValue(v8::Handle<v8::Array>& array)
{
    return array;
}

inline v8::Handle<v8::Value> ReturnValue(v8::Handle<v8::Function>& func)
{
    return func;
}

template<class T>
inline v8::Handle<v8::Value> ReturnValue(obj_ptr<T>& obj)
{
    return obj->JSObject();
}

inline v8::Handle<v8::Value> ThrowError(const char* msg)
{
    return v8::ThrowException(v8::Exception::Error(v8::String::New(msg)));
}

inline v8::Handle<v8::Value> ThrowTypeError(const char* msg)
{
    return v8::ThrowException(v8::Exception::TypeError(v8::String::New(msg)));
}

inline v8::Handle<v8::Value> ThrowRangeError(const char* msg)
{
    return v8::ThrowException(v8::Exception::RangeError(v8::String::New(msg)));
}

inline v8::Handle<v8::Value> ThrowResult(result_t hr)
{
    static const char* s_errors[] =
    {
        "",
        "Invalid number of parameters.",
        "Parameter not optional.",
        "Constructor cannot be called as a function.",
        "Object is not an instance of declaring class.",
        "The input parameter is not a valid type.",
        "An argument is invalid.",
        "The argument could not be coerced to the specified type.",
        "Value is out of range.",
        "Index was out of range."
    };

    if(hr < 0 && hr > CALL_E_MAX)
        return ThrowError(s_errors[-hr]);

#ifdef _WIN32
    LPWSTR pMsgBuf = NULL;
    if (FormatMessageW(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                       NULL, CALL_E_MAX - hr, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPWSTR) &pMsgBuf, 0, NULL ) && pMsgBuf)
    {
        v8::ThrowException(v8::Exception::Error(v8::String::New((const uint16_t*)pMsgBuf)));
        LocalFree(pMsgBuf);
    }
    else
        ThrowError("Unknown error.");

    return v8::Undefined();
//    return GetLastError();
#else
    return ThrowError(strerror(CALL_E_MAX - hr));
#endif
}

inline result_t LastError()
{
#ifdef _WIN32
    return CALL_E_MAX - GetLastError();
#else
    return CALL_E_MAX - errno;
#endif
}

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

class ClassInfo;

struct ClassData
{
    const char* name;
    v8::InvocationCallback cor;
    int mc;
    const ClassMethod* cms;
    int pc;
    const ClassProperty* cps;
    const ClassIndexed* cis;
    ClassInfo *base;
};

class ClassInfo
{
public:
    ClassInfo(ClassData& cd) : m_cd(cd)
    {
        v8::HandleScope handle_scope;

        m_class = v8::Persistent<v8::FunctionTemplate>::New(v8::FunctionTemplate::New(cd.cor));

        m_class->SetClassName(v8::String::NewSymbol(cd.name));

        if(cd.base)
            m_class->Inherit(cd.base->m_class);

        v8::Handle<v8::ObjectTemplate> ot = m_class->InstanceTemplate();

        ot->SetInternalFieldCount(1);
        if(cd.cis)
            ot->SetIndexedPropertyHandler(cd.cis->getter, cd.cis->setter);

        v8::Local<v8::ObjectTemplate> pt = m_class->PrototypeTemplate();
        int i;

        pt->MarkAsUndetectable();

        for(i = 0; i < cd.mc; i ++)
            pt->Set(cd.cms[i].name, v8::FunctionTemplate::New(cd.cms[i].invoker));

        for(i = 0; i < cd.pc; i ++)
            if(cd.cps[i].setter)
                pt->SetAccessor(v8::String::NewSymbol(cd.cps[i].name), cd.cps[i].getter, cd.cps[i].setter);
            else
                pt->SetAccessor(v8::String::NewSymbol(cd.cps[i].name), cd.cps[i].getter, block_set);

        m_cache = v8::Persistent<v8::Object>::New(m_class->GetFunction()->NewInstance());
    }

    void* getInstance(v8::Handle<v8::Value> o)
    {
        if(o.IsEmpty() || !o->IsObject() || !m_class->HasInstance(o))
            return NULL;

        return o->ToObject()->GetPointerFromInternalField(0);
    }

    v8::Handle<v8::Object> CreateInstance()
    {
        return m_cache->Clone();
    }

    v8::Handle<v8::FunctionTemplate> getTemplate() const
    {
        return m_class;
    }

    const char* name()
    {
        return m_cd.name;
    }

    void Attach(v8::Handle<v8::Object> o)
    {
        int i;

        for(i = 0; i < m_cd.mc; i ++)
            o->Set(v8::String::NewSymbol(m_cd.cms[i].name), v8::FunctionTemplate::New(m_cd.cms[i].invoker)->GetFunction());

        for(i = 0; i < m_cd.pc; i ++)
            if(m_cd.cps[i].setter)
                o->SetAccessor(v8::String::NewSymbol(m_cd.cps[i].name), m_cd.cps[i].getter, m_cd.cps[i].setter);
            else
                o->SetAccessor(v8::String::NewSymbol(m_cd.cps[i].name), m_cd.cps[i].getter, block_set);
    }

protected:
    static void block_set(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::AccessorInfo &info)
    {
        std::string strError = "Property \'";

        strError += *v8::String::Utf8Value(property);
        strError += "\' is read-only.";
        ThrowException(v8::String::New(strError.c_str(), strError.length()));
    }

private:
    v8::Persistent<v8::FunctionTemplate> m_class;
    v8::Persistent<v8::Object> m_cache;
    ClassData& m_cd;
};

class object_base
{
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
            return wrap(i.CreateInstance());

        return handle_;
    }

    v8::Handle<v8::Value> wrap(v8::Handle<v8::Object> o)
    {
        if (handle_.IsEmpty())
        {
            handle_ = v8::Persistent<v8::Object>::New(o);
            handle_->SetPointerInInternalField(0, this);
            handle_.MakeWeak(this, WeakCallback);

            Ref();
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
        }

        return 0;
    }

    virtual result_t toString(std::string& retVal)
    {
        retVal = Classinfo().name();
        return 0;
    }

public:
    static ClassInfo& class_info()
    {
        static ClassMethod s_method[] =
        {
            {"dispose", m_dispose},
            {"toString", m_toString}
        };

        static ClassData s_cd =
        {
            "object", NULL,
            2, s_method, 0, NULL, NULL
        };

        static ClassInfo s_ci(s_cd);

        return s_ci;
    }

    virtual ClassInfo& Classinfo()
    {
        return class_info();
    }

    v8::Handle<v8::Value> JSObject()
    {
        return wrap(Classinfo());
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

