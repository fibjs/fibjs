/***************************************************************************
 *   Copyright (C) 2012 by Leo Hoo                                         *
 *   lion@9465.net                                                         *
 *                                                                         *
 ***************************************************************************/

#ifndef _fj_classinfo_H_
#define _fj_classinfo_H_

/**
 @author Leo Hoo <lion@9465.net>
 */

#include <v8/include/v8.h>
#include <string>

namespace fibjs
{

class ClassInfo;

struct ClassData
{
    struct ClassProperty
    {
        const char *name;
        v8::AccessorGetterCallback getter;
        v8::AccessorSetterCallback setter;
        bool is_static;
    };

    struct ClassMethod
    {
        const char *name;
        v8::FunctionCallback invoker;
        bool is_static;
    };

    struct ClassObject
    {
        const char *name;
        ClassInfo &(*invoker)();
    };

    struct ClassIndexed
    {
        v8::IndexedPropertyGetterCallback getter;
        v8::IndexedPropertySetterCallback setter;
    };

    struct ClassNamed
    {
        v8::NamedPropertyGetterCallback getter;
        v8::NamedPropertySetterCallback setter;
        v8::NamedPropertyDeleterCallback remover;
        v8::NamedPropertyEnumeratorCallback enumerator;
    };

    const char *name;
    v8::FunctionCallback cor;
    int32_t mc;
    const ClassMethod *cms;
    int32_t oc;
    const ClassObject *cos;
    int32_t pc;
    const ClassProperty *cps;
    const ClassIndexed *cis;
    const ClassNamed *cns;
    ClassInfo *base;
};

class ClassInfo
{
public:
    ClassInfo(ClassData &cd) :
        m_cd(cd), refs_(0), m_next(NULL), m_Inherit(NULL)
    {
        if (m_cd.base)
        {
            if (m_cd.base->m_Inherit)
                m_next = m_cd.base->m_Inherit;
            m_cd.base->m_Inherit = this;
        }
    }

    void *getInstance(void *o);
    void *getInstance(v8::Local<v8::Value> o)
    {
        _init();
        if (o.IsEmpty() || !o->IsObject())
            return NULL;

        v8::Local<v8::Object> obj = v8::Local<v8::Object>::Cast(o);

        if (obj->InternalFieldCount() != 1)
            return NULL;

        return getInstance(obj->GetAlignedPointerFromInternalField(0));
    }

    v8::Local<v8::Function> getFunction()
    {
        _init();
        return v8::Local<v8::Function>::New(Isolate::now()->isolate, m_function);
    }

    v8::Local<v8::Object> CreateInstance()
    {
        _init();

        Isolate* isolate = Isolate::now();
        v8::Local<v8::Object> o;

        if (m_cache.IsEmpty())
        {
            o = v8::Local<v8::Function>::New(isolate->isolate, m_function)->NewInstance();
            o->SetAlignedPointerInInternalField(0, 0);
            m_cache.Reset(isolate->isolate, o);

            o = o->Clone();
        } else
            o = v8::Local<v8::Object>::New(isolate->isolate, m_cache)->Clone();

        return o;
    }

    bool has(const char *name)
    {
        int32_t i;

        for (i = 0; i < m_cd.mc; i++)
            if (!qstrcmp(name, m_cd.cms[i].name))
                return true;

        for (i = 0; i < m_cd.oc; i++)
            if (!qstrcmp(name, m_cd.cos[i].name))
                return true;

        for (i = 0; i < m_cd.pc; i++)
            if (!qstrcmp(name, m_cd.cps[i].name))
                return true;

        if (m_cd.base)
            return m_cd.base->has(name);

        return false;
    }

    const char *name()
    {
        return m_cd.name;
    }

    void Attach(v8::Local<v8::Object> o)
    {
        Isolate* isolate = Isolate::now();
        v8::Local<v8::Context> _context = v8::Local<v8::Context>::New(isolate->isolate, isolate->s_context);

        _init();
        int32_t i;

        for (i = 0; i < m_cd.mc; i++)
            if (m_cd.cms[i].is_static)
                o->ForceSet(_context, v8::String::NewFromUtf8(isolate->isolate, m_cd.cms[i].name),
                            v8::Function::New(isolate->isolate, m_cd.cms[i].invoker),
                            v8::ReadOnly);

        for (i = 0; i < m_cd.oc; i++)
        {
            m_cd.cos[i].invoker()._init();
            o->ForceSet(_context, v8::String::NewFromUtf8(isolate->isolate, m_cd.cos[i].name),
                        v8::Local<v8::Function>::New(isolate->isolate, m_cd.cos[i].invoker().m_function),
                        v8::ReadOnly);
        }

        for (i = 0; i < m_cd.pc; i++)
            if (m_cd.cps[i].is_static)
                o->SetAccessor(v8::String::NewFromUtf8(isolate->isolate, m_cd.cps[i].name),
                               m_cd.cps[i].getter, m_cd.cps[i].setter);
    }

public:
    void Ref()
    {
        refs_.inc();
    }

    void Unref()
    {
        refs_.dec();
    }

    exlib::atomic_t dump(v8::Local<v8::Object> &o)
    {
        Isolate* isolate = Isolate::now();
        exlib::atomic_t cnt = refs_;

        if (cnt)
        {
            o = v8::Object::New(isolate->isolate);
            o->Set(v8::String::NewFromUtf8(isolate->isolate, "class"),
                   v8::String::NewFromUtf8(isolate->isolate, m_cd.name));
            o->Set(v8::String::NewFromUtf8(isolate->isolate, "objects"),
                   v8::Integer::New(isolate->isolate, (int32_t)cnt));

            v8::Local<v8::Array> inherits = v8::Array::New(isolate->isolate);

            ClassInfo *p = m_Inherit;
            exlib::atomic_t icnt = 0;

            while (p)
            {
                v8::Local<v8::Object> o1;
                exlib::atomic_t cnt1 = p->dump(o1);
                if (cnt1)
                    inherits->Set((int32_t)(icnt ++), o1);
                p = p->m_next;
            }

            if (icnt)
                o->Set(v8::String::NewFromUtf8(isolate->isolate, "inherits"), inherits);
        }

        return cnt;
    }

private:
    void _init()
    {
        if (m_class.IsEmpty())
        {
            Isolate* isolate = Isolate::now();

            v8::Local<v8::FunctionTemplate> _class = v8::FunctionTemplate::New(
                        isolate->isolate, m_cd.cor);
            m_class.Reset(isolate->isolate, _class);

            _class->SetClassName(v8::String::NewFromUtf8(isolate->isolate, m_cd.name));

            if (m_cd.base)
            {
                m_cd.base->_init();
                _class->Inherit(
                    v8::Local<v8::FunctionTemplate>::New(isolate->isolate,
                            m_cd.base->m_class));
            }

            v8::Local<v8::ObjectTemplate> pt = _class->PrototypeTemplate();
            int32_t i;

            pt->MarkAsUndetectable();

            for (i = 0; i < m_cd.mc; i++)
                pt->Set(v8::String::NewFromUtf8(isolate->isolate, m_cd.cms[i].name),
                        v8::FunctionTemplate::New(isolate->isolate, m_cd.cms[i].invoker));

            for (i = 0; i < m_cd.oc; i++)
            {
                m_cd.cos[i].invoker()._init();
                pt->Set(v8::String::NewFromUtf8(isolate->isolate, m_cd.cos[i].name),
                        v8::Local<v8::FunctionTemplate>::New(isolate->isolate,
                                m_cd.cos[i].invoker().m_class));
            }

            for (i = 0; i < m_cd.pc; i++)
                pt->SetAccessor(v8::String::NewFromUtf8(isolate->isolate, m_cd.cps[i].name),
                                m_cd.cps[i].getter, m_cd.cps[i].setter);

            v8::Local<v8::ObjectTemplate> ot = _class->InstanceTemplate();
            ot->SetInternalFieldCount(1);

            ClassData *pcd;

            pcd = &m_cd;
            while (pcd && !pcd->cis)
                pcd = pcd->base ? &pcd->base->m_cd : NULL;

            if (pcd)
                ot->SetIndexedPropertyHandler(pcd->cis->getter, pcd->cis->setter);

            pcd = &m_cd;
            while (pcd && !pcd->cns)
                pcd = pcd->base ? &pcd->base->m_cd : NULL;

            if (pcd)
                ot->SetNamedPropertyHandler(pcd->cns->getter, pcd->cns->setter,
                                            NULL, pcd->cns->remover, pcd->cns->enumerator);

            v8::Local<v8::Function> _function = _class->GetFunction();
            Attach(_function);
            m_function.Reset(isolate->isolate, _function);

            if (m_cd.cor)
            {
                v8::Local<v8::Object> o = _function->NewInstance();
                o->SetAlignedPointerInInternalField(0, 0);
                m_cache.Reset(isolate->isolate, o);
            }
        }
    }

private:
    v8::Persistent<v8::FunctionTemplate> m_class;
    v8::Persistent<v8::Function> m_function;
    v8::Persistent<v8::Object> m_cache;
    ClassData &m_cd;
    exlib::atomic refs_;
    ClassInfo *m_next;
    ClassInfo *m_Inherit;
};

}

#endif

