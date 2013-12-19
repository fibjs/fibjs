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

#include <v8/v8.h>
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
        bool bStatic;
    };

    struct ClassMethod
    {
        const char *name;
        v8::FunctionCallback invoker;
        bool bStatic;
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
    int mc;
    const ClassMethod *cms;
    int oc;
    const ClassObject *cos;
    int pc;
    const ClassProperty *cps;
    const ClassIndexed *cis;
    const ClassNamed *cns;
    ClassInfo *base;
};

class ClassInfo
{
public:
    ClassInfo(ClassData &cd) :
        m_cd(cd)
    {
        v8::HandleScope handle_scope(isolate);

        v8::Handle<v8::FunctionTemplate> _class = v8::FunctionTemplate::New(
                    cd.cor);
        m_class.Reset(isolate, _class);

        _class->SetClassName(v8::String::NewSymbol(cd.name));

        if (cd.base)
            _class->Inherit(
                v8::Handle<v8::FunctionTemplate>::New(isolate,
                        cd.base->m_class));

        v8::Local<v8::ObjectTemplate> pt = _class->PrototypeTemplate();
        int i;

        pt->MarkAsUndetectable();

        for (i = 0; i < cd.mc; i++)
            pt->Set(cd.cms[i].name,
                    v8::FunctionTemplate::New(cd.cms[i].invoker));

        for (i = 0; i < cd.oc; i++)
            pt->Set(

                cd.cos[i].name,
                v8::Handle<v8::FunctionTemplate>::New(isolate,
                        cd.cos[i].invoker().m_class));

        for (i = 0; i < cd.pc; i++)
            pt->SetAccessor(v8::String::NewSymbol(cd.cps[i].name),
                            cd.cps[i].getter, cd.cps[i].setter);

        v8::Local<v8::ObjectTemplate> ot = _class->InstanceTemplate();
        ot->SetInternalFieldCount(1);

        ClassData *pcd;

        pcd = &cd;
        while (pcd && !pcd->cis)
            pcd = pcd->base ? &pcd->base->m_cd : NULL;

        if (pcd)
            ot->SetIndexedPropertyHandler(pcd->cis->getter, pcd->cis->setter);

        pcd = &cd;
        while (pcd && !pcd->cns)
            pcd = pcd->base ? &pcd->base->m_cd : NULL;

        if (pcd)
            ot->SetNamedPropertyHandler(pcd->cns->getter, pcd->cns->setter,
                                        NULL, pcd->cns->remover, pcd->cns->enumerator);

        v8::Handle<v8::Function> _function = _class->GetFunction();
        m_function.Reset(isolate, _function);

        v8::Handle<v8::Object> o = _function->NewInstance();
        o->SetAlignedPointerInInternalField(0, 0);
        m_cache.Reset(isolate, o);
    }

    void *getInstance(void *o);
    void *getInstance(v8::Handle<v8::Value> o)
    {
        if (o.IsEmpty() || !o->IsObject()
                || !v8::Handle<v8::FunctionTemplate>::New(isolate, m_class)->HasInstance(
                    o))
            return NULL;

        return o->ToObject()->GetAlignedPointerFromInternalField(0);
    }

    v8::Handle<v8::Object> CreateInstance()
    {
        return v8::Handle<v8::Object>::New(isolate, m_cache)->Clone();
    }

    bool has(const char *name)
    {
        int i;

        for (i = 0; i < m_cd.mc; i++)
            if (!qstrcmp(name, m_cd.cms[i].name))
                return true;

        for (i = 0; i < m_cd.oc; i++)
            if (!qstrcmp(name, m_cd.cos[i].name))
                return true;

        for (i = 0; i < m_cd.pc; i++)
            if (!qstrcmp(name, m_cd.cps[i].name))
                return true;

        return false;
    }

    const char *name()
    {
        return m_cd.name;
    }

    void Attach(v8::Handle<v8::Object> o)
    {
        int i;

        for (i = 0; i < m_cd.mc; i++)
            o->Set(v8::String::NewSymbol(m_cd.cms[i].name),
                   v8::FunctionTemplate::New(m_cd.cms[i].invoker)->GetFunction(),
                   v8::ReadOnly);

        for (i = 0; i < m_cd.oc; i++)
            o->Set(v8::String::NewSymbol(m_cd.cos[i].name),
                   v8::Handle<v8::Function>::New(isolate,
                                                 m_cd.cos[i].invoker().m_function), v8::ReadOnly);

        for (i = 0; i < m_cd.pc; i++)
            o->SetAccessor(v8::String::NewSymbol(m_cd.cps[i].name),
                           m_cd.cps[i].getter, m_cd.cps[i].setter);
    }

private:
    v8::Persistent<v8::FunctionTemplate> m_class;
    v8::Persistent<v8::Function> m_function;
    v8::Persistent<v8::Object> m_cache;
    ClassData &m_cd;
};

}

#endif

