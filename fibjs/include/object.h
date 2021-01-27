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
#include "AsyncCall.h"

#undef min
#undef max

namespace fibjs {

#include "object_async.inl"

#define JSOBJECT_JSVALUE 1
#define JSOBJECT_JSHANDLE 2

class object_base : public obj_base {
public:
    object_base()
        : m_isolate(NULL)
        , m_isJSObject(0)
        , m_nExtMemory(sizeof(object_base) * 2)
        , m_nExtMemoryDelay(0)
        , m_holding(false)
    {
        object_base::class_info().Ref();
    }

    virtual ~object_base()
    {
        clear_handle();
        object_base::class_info().Unref();
    }

public:
    virtual void Unref()
    {
        if (internalUnref() == 0) {
            if (m_isJSObject) {
                assert(!m_weak.m_inlist);

                Isolate* isolate = m_isolate;

                isolate->m_weakLock.lock();
                isolate->m_weak.putTail(&m_weak);
                isolate->m_weakLock.unlock();
            } else
                Delete();
        }
    }

    virtual void Delete()
    {
        delete this;
    }

private:
    exlib::linkitem m_weak;

public:
    virtual bool enterTask(exlib::Task_base* current)
    {
        return m_lock.lock(current);
    }

    virtual void enter()
    {
        if (!m_lock.trylock()) {
            Isolate::LeaveJsScope _rt(holder());
            m_lock.lock();
        }
    }

    virtual void leave(exlib::Task_base* current = NULL)
    {
        m_lock.unlock(current);
    }

    exlib::Locker m_lock;

private:
    v8::Global<v8::Object> handle_;

public:
    static void gc_delete(exlib::linkitem* node)
    {
        object_base* pThis = NULL;
        pThis = (object_base*)((char*)node
            - ((char*)&pThis->m_weak - (char*)0));

        pThis->Delete();
    }

private:
    Isolate* m_isolate;
    int32_t m_isJSObject;

public:
    Isolate* holder()
    {
        Isolate* isolate = m_isolate;

        if (isolate)
            return isolate;

        return m_isolate = Isolate::current();
    }

    Isolate* get_holder()
    {
        return m_isolate;
    }

    const char* typeName()
    {
        return Classinfo().name();
    }

    void setJSObject()
    {
        m_isJSObject |= JSOBJECT_JSVALUE;
        holder();

        assert(m_isolate != 0);
    }

private:
    void clear_handle()
    {
        if (m_isJSObject & JSOBJECT_JSHANDLE) {
            m_isJSObject &= ~JSOBJECT_JSHANDLE;

            Isolate* isolate = holder();
            v8::Isolate* v8_isolate = isolate->m_isolate;

            handle_.ClearWeak();
            handle_.Reset();

            v8_isolate->AdjustAmountOfExternalAllocatedMemory(-m_nExtMemory);

            if (internalUnref() == 0)
                Delete();
        }
    }

    static void WeakCallback(const v8::WeakCallbackInfo<object_base>& data)
    {
        assert(!data.GetParameter()->handle_.IsEmpty());
        data.GetParameter()->clear_handle();
    }

public:
    v8::Local<v8::Object> wrap(v8::Local<v8::Object> o = v8::Local<v8::Object>())
    {
        Isolate* isolate = holder();
        v8::Isolate* v8_isolate = isolate->m_isolate;

        if (!(m_isJSObject & JSOBJECT_JSHANDLE)) {
            if (o.IsEmpty())
                o = Classinfo().CreateInstance(isolate);
            handle_.Reset(v8_isolate, o);
            o->SetAlignedPointerInInternalField(0, this);

            v8_isolate->AdjustAmountOfExternalAllocatedMemory(m_nExtMemory);

            internalRef();
            handle_.SetWeak(this, WeakCallback, v8::WeakCallbackType::kParameter);

            m_isJSObject |= JSOBJECT_JSHANDLE;
        } else
            o = v8::Local<v8::Object>::New(v8_isolate, handle_);

        return o;
    }

public:
    class scope {
    public:
        scope(object_base* pInst)
            : m_pInst(pInst)
        {
            m_pInst->enter();
        }

        ~scope()
        {
            m_pInst->leave();
        }

    private:
        object_base* m_pInst;
    };

public:
    // Event
    result_t on(exlib::string ev, v8::Local<v8::Function> func, v8::Local<v8::Object>& retVal);
    result_t on(v8::Local<v8::Object> map, v8::Local<v8::Object>& retVal);
    result_t prependListener(exlib::string ev, v8::Local<v8::Function> func, v8::Local<v8::Object>& retVal);
    result_t prependListener(v8::Local<v8::Object> map, v8::Local<v8::Object>& retVal);
    result_t once(exlib::string ev, v8::Local<v8::Function> func, v8::Local<v8::Object>& retVal);
    result_t once(v8::Local<v8::Object> map, v8::Local<v8::Object>& retVal);
    result_t prependOnceListener(exlib::string ev, v8::Local<v8::Function> func, v8::Local<v8::Object>& retVal);
    result_t prependOnceListener(v8::Local<v8::Object> map, v8::Local<v8::Object>& retVal);
    result_t off(exlib::string ev, v8::Local<v8::Function> func, v8::Local<v8::Object>& retVal);
    result_t off(exlib::string ev, v8::Local<v8::Object>& retVal);
    result_t off(v8::Local<v8::Object> map, v8::Local<v8::Object>& retVal);
    result_t removeAllListeners(v8::Local<v8::Array> evs, v8::Local<v8::Object>& retVal);
    result_t setMaxListeners(int32_t n);
    result_t getMaxListeners(int32_t& retVal);
    result_t setListener(exlib::string ev, v8::Local<v8::Function> func);
    result_t getListener(exlib::string ev, v8::Local<v8::Function> func);
    result_t listeners(exlib::string ev, v8::Local<v8::Array>& retVal);
    result_t listenerCount(exlib::string ev, int32_t& retVal);
    result_t emit(exlib::string ev, OptArgs args, bool& retVal);
    result_t eventNames(v8::Local<v8::Array>& retVal);
    result_t _emit(exlib::string ev, v8::Local<v8::Value>* args, int32_t argCount, bool& retVal);
    result_t _emit(exlib::string ev, Variant* args = NULL, int32_t argCount = 0);
    result_t _emit(exlib::string ev, Variant arg);

    virtual result_t onEventChange(v8::Local<v8::Function> func, exlib::string ev, exlib::string type)
    {
        return 0;
    }

    virtual result_t onEventEmit(exlib::string ev)
    {
        return 0;
    }

    void extMemory(int32_t ext)
    {
        if (handle_.IsEmpty())
            m_nExtMemory += ext;
        else {
            ext += m_nExtMemoryDelay;
            m_nExtMemoryDelay = 0;

            if (ext != 0) {
                Isolate* isolate = m_isolate;

                if (isolate && Runtime::is_current(isolate)) {
                    isolate->m_isolate->AdjustAmountOfExternalAllocatedMemory(ext);
                    m_nExtMemory += ext;
                } else
                    m_nExtMemoryDelay = ext;
            }
        }
    }

    void isolate_ref()
    {
        if (!m_holding) {
            m_holding = true;
            holder()->Ref();
        }
    }

    void isolate_unref()
    {
        if (m_holding) {
            m_holding = false;
            holder()->Unref();
        }
    }

private:
    v8::Local<v8::Array> GetHiddenList(const char* k, bool create = false,
        bool autoDelete = false);

private:
    int32_t m_nExtMemory;
    int32_t m_nExtMemoryDelay;
    bool m_holding;

public:
    template <typename T>
    static void __new(const T& args) { }

public:
    v8::Local<v8::Object> GetPrivateObject()
    {
        v8::Local<v8::Object> o = wrap();
        Isolate* isolate = holder();

        v8::Local<v8::Private> k = v8::Private::ForApi(isolate->m_isolate, isolate->NewString("_private_object"));
        JSValue v = o->GetPrivate(o->CreationContext(), k);

        if (v->IsObject())
            return v8::Local<v8::Object>::Cast(v);

        v8::Local<v8::Object> po = v8::Object::New(isolate->m_isolate);
        o->SetPrivate(o->CreationContext(), k, po);
        return po;
    }

    v8::Local<v8::Value> GetPrivate(exlib::string key)
    {
        return GetPrivateObject()->Get(holder()->NewString(key));
    }

    void SetPrivate(exlib::string key, v8::Local<v8::Value> value)
    {
        GetPrivateObject()->Set(holder()->NewString(key), value);
    }

    void DeletePrivate(exlib::string key)
    {
        GetPrivateObject()->Delete(holder()->NewString(key));
    }

public:
    virtual result_t equals(object_base* expected, bool& retVal)
    {
        retVal = expected == this;
        return 0;
    }

    virtual result_t toString(exlib::string& retVal)
    {
        retVal = Classinfo().name();
        return 0;
    }

    virtual result_t toJSON(exlib::string key, v8::Local<v8::Value>& retVal)
    {
        v8::Local<v8::Object> o = wrap();
        v8::Local<v8::Object> o1 = v8::Object::New(holder()->m_isolate);

        extend(o, o1);
        retVal = o1;

        return 0;
    }

    virtual result_t valueOf(v8::Local<v8::Value>& retVal)
    {
        retVal = wrap();
        return 0;
    }

    virtual result_t unbind(obj_ptr<object_base>& retVal)
    {
        return CHECK_ERROR(CALL_E_INVALID_CALL);
    }

    result_t unbind_dispose(obj_ptr<object_base>& retVal)
    {
        Isolate* isolate = holder();
        v8::Isolate* v8_isolate = isolate->m_isolate;

        if (m_isJSObject & JSOBJECT_JSHANDLE)
            v8::Local<v8::Object>::New(v8_isolate, handle_)->SetAlignedPointerInInternalField(0, 0);

        clear_handle();

        isolate->m_weakLock.lock();
#ifdef DEBUG
        if (m_weak.m_inlist)
#endif
            isolate->m_weak.remove(&m_weak);
        isolate->m_weakLock.unlock();
        m_isolate = NULL;

        retVal = this;

        return 0;
    }

public:
    static void block_set(v8::Local<v8::Name> property,
        v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& info)
    {
        Isolate* isolate = Isolate::current();

        exlib::string strError = "Property \'";

        strError += ToCString(v8::String::Utf8Value(isolate->m_isolate, property));
        strError += "\' is read-only.";
        isolate->m_isolate->ThrowException(
            isolate->NewString(strError));
    }

    static void i_IndexedSetter(uint32_t index,
        v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<v8::Value>& info)
    {
        Isolate* isolate = Isolate::current();

        isolate->m_isolate->ThrowException(
            isolate->NewString("Indexed Property is read-only."));
    }

    static void i_NamedSetter(v8::Local<v8::Name> property,
        v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<v8::Value>& info)
    {
        Isolate* isolate = Isolate::current();

        isolate->m_isolate->ThrowException(
            isolate->NewString("Named Property is read-only."));
    }

    static void i_NamedDeleter(
        v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Boolean>& info)
    {
        Isolate* isolate = Isolate::current();

        isolate->m_isolate->ThrowException(
            isolate->NewString("Named Property is read-only."));
    }

    //------------------------------------------------------------------
    DECLARE_CLASSINFO(object_base);

private:
    static void s_toString(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_toJSON(const v8::FunctionCallbackInfo<v8::Value>& args);
};

class ValueHolder : public obj_base {
public:
    ValueHolder(v8::Local<v8::Value> v)
    {
        m_isolate = Isolate::current();
        m_v.Reset(m_isolate->m_isolate, v);
    }

public:
    virtual void Unref()
    {
        if (internalUnref() == 0)
            syncCall(m_isolate, _release, this);
    }

private:
    static result_t _release(ValueHolder* pThis)
    {
        delete pThis;
        return 0;
    }

private:
    v8::Global<v8::Value> m_v;
    Isolate* m_isolate;
};

class RootModule {
public:
    RootModule()
        : m_next(NULL)
    {
    }

public:
    void install()
    {
        if (!g_root)
            g_root = this;
        else
            g_last->m_next = this;

        g_last = this;
    }

public:
    virtual ClassInfo& class_info() = 0;

    virtual v8::Local<v8::Object> getModule(Isolate* isolate)
    {
        return class_info().getModule(isolate);
    }

    virtual const char* name()
    {
        return class_info().name();
    }

public:
    RootModule* m_next;
    static RootModule* g_root;
    static RootModule* g_last;
};

inline void* ClassInfo::getInstance(void* o)
{
    object_base* obj = (object_base*)o;

    if (!obj)
        return NULL;

    ClassInfo* cls = &obj->Classinfo();
    ClassInfo* tcls = this;

    while (cls && cls != tcls)
        cls = cls->m_cd.base;

    if (!cls)
        return NULL;

    return obj;
}

inline ClassInfo& object_base::class_info()
{
    static ClassData::ClassMethod s_method[] = {
        { "toString", s_toString },
        { "toJSON", s_toJSON }
    };

    static ClassData s_cd = {
        "object", false, NULL, NULL, ARRAYSIZE(s_method), s_method
    };

    static ClassInfo s_ci(s_cd);
    return s_ci;
}

inline void object_base::s_toString(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_NAME("object.toString");
    METHOD_INSTANCE(object_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->toString(vr);

    METHOD_RETURN();
}

inline void object_base::s_toJSON(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Value> vr;

    METHOD_NAME("object.toJSON");
    METHOD_INSTANCE(object_base);
    METHOD_ENTER();

    METHOD_OVER(1, 0);

    OPT_ARG(exlib::string, 0, "");

    hr = pInst->toJSON(v0, vr);

    METHOD_RETURN();
}
}

#endif

#ifdef _assert
#undef _assert
#endif

#include "SimpleObject.h"