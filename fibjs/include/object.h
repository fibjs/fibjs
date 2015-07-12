/***************************************************************************
 *   Copyright (C) 2012 by Leo Hoo                                         *
 *   lion@9465.net                                                         *
 *                                                                         *
 ***************************************************************************/

#ifndef _fj_object_H_
#define _fj_object_H_

/**
 @author Leo Hoo <lion@9465.net>
 */

#include "utils.h"
#include "ClassInfo.h"
#include "Runtime.h"

#undef min
#undef max

namespace fibjs
{

#include "object_async.inl"

class asyncEvent: public exlib::AsyncEvent
{
public:
    virtual void js_callback()
    {
    }

    virtual void invoke()
    {
    }
};

class asyncCallBack: public asyncEvent
{
public:
    virtual void callback();
};

class object_base: public obj_base
{
public:
    object_base() :
        m_nExtMemory(sizeof(object_base) * 2), m_nExtMemoryDelay(0)
    {
        object_base::class_info().Ref();
    }

    virtual ~object_base()
    {
        object_base::class_info().Unref();
    }

public:
    virtual void Ref()
    {
        if (internalRef() == 1)
        {
            if (!handle_.IsEmpty())
                handle_.ClearWeak();
        }
    }

    virtual void Unref()
    {
        if (!isJSObject())
        {
            if (internalUnref() == 0)
                delete this;
            return;
        }

        if (exlib::Service::hasService())
        {
            if (internalUnref() == 0)
            {
                if (!handle_.IsEmpty())
                    handle_.SetWeak(this, WeakCallback);
                else
                    delete this;
            }

            return;
        }

        m_fast_lock.lock();

        if (internalUnref() == 0)
        {
            internalRef();
            m_ar.post(0);
        }

        m_fast_lock.unlock();
    }

    exlib::spinlock m_fast_lock;

    virtual bool isJSObject()
    {
        return !handle_.IsEmpty();
    }

public:
    virtual void enter()
    {
        if (!m_lock.trylock())
        {
            Isolate::rt _rt;
            m_lock.lock();
        }
    }

    virtual void leave()
    {
        m_lock.unlock();
    }

    exlib::Locker m_lock;

public:
    class asyncRelease: public asyncCallBack
    {
    public:
        virtual void js_callback()
        {
            object_base *pThis = NULL;

            pThis = (object_base *) ((char *) this
                                     - ((char *) &pThis->m_ar - (char *) 0));

            pThis->Unref();
        }
    };

private:

    asyncRelease m_ar;
    v8::Persistent<v8::Object> handle_;

private:
    static void WeakCallback(const v8::WeakCallbackData<v8::Object, object_base> &data)
    {
        data.GetParameter()->internalDispose();
    }

public:
    v8::Local<v8::Object> wrap(v8::Local<v8::Object> o)
    {
        Isolate & isolate = Isolate::now();

        if (handle_.IsEmpty())
        {
            if (o.IsEmpty())
                o = Classinfo().CreateInstance();
            handle_.Reset(isolate.isolate, o);
            o->SetAlignedPointerInInternalField(0, this);

            isolate.isolate->AdjustAmountOfExternalAllocatedMemory(m_nExtMemory);

            return o;
        }

        return v8::Local<v8::Object>::New(isolate.isolate, handle_);
    }

    v8::Local<v8::Object> wrap()
    {
        if (handle_.IsEmpty())
            return wrap(Classinfo().CreateInstance());

        return v8::Local<v8::Object>::New(Isolate::now().isolate, handle_);
    }

public:
    class scope
    {
    public:
        scope(object_base *pInst) :
            m_pInst(pInst)
        {
            m_pInst->enter();
        }

        ~scope()
        {
            m_pInst->leave();
        }

    private:
        object_base *m_pInst;
    };

public:
    // Event
    result_t on(const char *ev, v8::Local<v8::Function> func, int32_t &retVal);
    result_t on(v8::Local<v8::Object> map, int32_t &retVal);
    result_t once(const char *ev, v8::Local<v8::Function> func, int32_t &retVal);
    result_t once(v8::Local<v8::Object> map, int32_t &retVal);
    result_t off(const char *ev, v8::Local<v8::Function> func, int32_t &retVal);
    result_t off(const char *ev, int32_t &retVal);
    result_t off(v8::Local<v8::Object> map, int32_t &retVal);
    result_t trigger(const char *ev, const v8::FunctionCallbackInfo<v8::Value> &args);
    result_t _trigger(const char *ev, v8::Local<v8::Value> *args, int argCount);
    result_t _trigger(const char *ev, Variant *args, int argCount);

    void extMemory(int ext)
    {
        if (handle_.IsEmpty())
            m_nExtMemory += ext;
        else
        {
            ext += m_nExtMemoryDelay;
            m_nExtMemoryDelay = 0;

            if (ext != 0)
            {
                if (exlib::Service::hasService())
                {
                    Isolate::now().isolate->AdjustAmountOfExternalAllocatedMemory(ext);
                    m_nExtMemory += ext;
                }
                else
                    m_nExtMemoryDelay = ext;
            }
        }
    }

private:
    v8::Local<v8::Object> GetHiddenList(const char *k, bool create = false,
                                        bool autoDelete = false);

private:
    int m_nExtMemory;
    int m_nExtMemoryDelay;

    result_t internalDispose()
    {
        if (!handle_.IsEmpty())
        {
            Isolate & isolate = Isolate::now();

            handle_.ClearWeak();
            v8::Local<v8::Object>::New(isolate.isolate, handle_)->SetAlignedPointerInInternalField(
                0, 0);
            handle_.Reset();

            isolate.isolate->AdjustAmountOfExternalAllocatedMemory(-m_nExtMemory);

            obj_base::dispose();
        }

        return 0;
    }

public:
    template<typename T>
    static void __new(const T &args) {}

public:
    // object_base
    virtual result_t dispose()
    {
        return internalDispose();
    }

    virtual result_t toString(std::string &retVal)
    {
        retVal = Classinfo().name();
        return 0;
    }

    virtual result_t toJSON(const char *key, v8::Local<v8::Value> &retVal)
    {
        v8::Local<v8::Object> o = wrap();
        v8::Local<v8::Object> o1 = v8::Object::New(Isolate::now().isolate);

        extend(o, o1);
        retVal = o1;

        return 0;
    }

    virtual result_t valueOf(v8::Local<v8::Value> &retVal)
    {
        retVal = wrap();
        return 0;
    }

public:
    static void block_set(v8::Local<v8::String> property,
                          v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void> &info)
    {
        Isolate & isolate = Isolate::now();

        std::string strError = "Property \'";

        strError += *v8::String::Utf8Value(property);
        strError += "\' is read-only.";
        isolate.isolate->ThrowException(
            v8::String::NewFromUtf8(isolate.isolate, strError.c_str(),
                                    v8::String::kNormalString, (int) strError.length()));
    }

    static void i_IndexedSetter(uint32_t index,
                                v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<v8::Value> &info)
    {
        Isolate & isolate = Isolate::now();

        isolate.isolate->ThrowException(
            v8::String::NewFromUtf8(isolate.isolate, "Indexed Property is read-only."));
    }

    static void i_NamedSetter(v8::Local<v8::String> property,
                              v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<v8::Value> &info)
    {
        Isolate & isolate = Isolate::now();

        isolate.isolate->ThrowException(
            v8::String::NewFromUtf8(isolate.isolate, "Named Property is read-only."));
    }

    static void i_NamedDeleter(
        v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Boolean> &info)
    {
        Isolate & isolate = Isolate::now();

        isolate.isolate->ThrowException(
            v8::String::NewFromUtf8(isolate.isolate, "Named Property is read-only."));
    }

    //------------------------------------------------------------------
    DECLARE_CLASSINFO(object_base);

private:
    static void s_dispose(const v8::FunctionCallbackInfo<v8::Value> &args);
    static void s_toString(const v8::FunctionCallbackInfo<v8::Value> &args);
    static void s_toJSON(const v8::FunctionCallbackInfo<v8::Value> &args);
    static void s_valueOf(const v8::FunctionCallbackInfo<v8::Value> &args);
};

}

#include "AsyncCall.h"

namespace fibjs
{

inline void *ClassInfo::getInstance(void *o)
{
    object_base *obj = (object_base *) o;

    if (!obj)
        return NULL;

    ClassInfo *cls = &obj->Classinfo();
    ClassInfo *tcls = this;

    while (cls && cls != tcls)
        cls = cls->m_cd.base;

    if (!cls)
        return NULL;

    return obj;
}

inline ClassInfo &object_base::class_info()
{
    static ClassData::ClassMethod s_method[] =
    {
        { "dispose", s_dispose },
        { "toString", s_toString },
        { "toJSON", s_toJSON },
        { "valueOf", s_valueOf }
    };

    static ClassData s_cd =
    { "object", NULL, 4, s_method, 0, NULL, 0, NULL, NULL, NULL, NULL };

    static ClassInfo s_ci(s_cd);
    return s_ci;
}

inline void object_base::s_dispose(const v8::FunctionCallbackInfo<v8::Value> &args)
{
    METHOD_INSTANCE(object_base);
    METHOD_ENTER(0, 0);

    hr = pInst->dispose();

    METHOD_VOID();
}

inline void object_base::s_toString(const v8::FunctionCallbackInfo<v8::Value> &args)
{
    std::string vr;

    METHOD_INSTANCE(object_base);
    METHOD_ENTER(0, 0);

    hr = pInst->toString(vr);

    METHOD_RETURN();
}

inline void object_base::s_toJSON(const v8::FunctionCallbackInfo<v8::Value> &args)
{
    obj_ptr<object_base> pInst = object_base::getInstance(args.This());
    if (pInst == NULL)
    {
        V8_SCOPE();

        v8::Local<v8::Object> o = args.This();
        v8::Local<v8::Object> o1 = v8::Object::New(Isolate::now().isolate);

        extend(o, o1);

        args.GetReturnValue().Set(V8_RETURN(o1));
        return;
    }

    v8::Local<v8::Value> vr;

    scope l(pInst);

    METHOD_ENTER(1, 0);

    OPT_ARG(arg_string, 0, "");

    hr = pInst->toJSON(v0, vr);

    METHOD_RETURN();
}

inline void object_base::s_valueOf(const v8::FunctionCallbackInfo<v8::Value> &args)
{
    v8::Local<v8::Value> vr;

    METHOD_INSTANCE(object_base);
    METHOD_ENTER(0, 0);

    hr = pInst->valueOf(vr);

    METHOD_RETURN();
}

}

#endif

