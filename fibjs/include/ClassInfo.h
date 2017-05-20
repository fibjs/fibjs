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

namespace fibjs {

class ClassInfo;

struct ClassData {
    struct ClassProperty {
        const char* name;
        v8::AccessorGetterCallback getter;
        v8::AccessorSetterCallback setter;
        bool is_static;
    };

    struct ClassMethod {
        const char* name;
        v8::FunctionCallback invoker;
        bool is_static;
    };

    struct ClassObject {
        const char* name;
        ClassInfo& (*invoker)();
    };

    struct ClassIndexed {
        v8::IndexedPropertyGetterCallback getter;
        v8::IndexedPropertySetterCallback setter;
    };

    struct ClassNamed {
        v8::NamedPropertyGetterCallback getter;
        v8::NamedPropertySetterCallback setter;
        v8::NamedPropertyDeleterCallback remover;
        v8::NamedPropertyEnumeratorCallback enumerator;
    };

    const char* name;
    bool module;
    v8::FunctionCallback cor;
    v8::FunctionCallback caf;
    int32_t mc;
    const ClassMethod* cms;
    int32_t oc;
    const ClassObject* cos;
    int32_t pc;
    const ClassProperty* cps;
    const ClassIndexed* cis;
    const ClassNamed* cns;
    ClassInfo* base;
};

class ClassInfo {
public:
    class cache {
    public:
        cache()
            : m_init_isolate(false)
        {
        }

    public:
        v8::Global<v8::FunctionTemplate> m_class;
        v8::Global<v8::Function> m_function;
        v8::Global<v8::Object> m_cache;
        bool m_init_isolate;
    };

public:
    ClassInfo(ClassData& cd)
        : m_cd(cd)
        , refs_(0)
        , m_next(NULL)
        , m_Inherit(NULL)
        , m_id(-1)
    {
        if (m_cd.base) {
            if (m_cd.base->m_Inherit)
                m_next = m_cd.base->m_Inherit;
            m_cd.base->m_Inherit = this;
        }
    }

    void* getInstance(void* o);
    void* getInstance(v8::Local<v8::Value> o)
    {
        assert(!m_cd.module);

        if (o.IsEmpty() || !o->IsObject())
            return NULL;

        v8::Local<v8::Object> obj = v8::Local<v8::Object>::Cast(o);

        if (obj->InternalFieldCount() != 1)
            return NULL;

        return getInstance(obj->GetAlignedPointerFromInternalField(0));
    }

    v8::Local<v8::Function> getFunction(Isolate* isolate)
    {
        assert(!m_cd.module);

        cache* _cache = _init(isolate);
        return v8::Local<v8::Function>::New(isolate->m_isolate, _cache->m_function);
    }

    v8::Local<v8::Object> getModule(Isolate* isolate)
    {
        cache* _cache = _init(isolate);

        if (m_cd.module)
            return v8::Local<v8::Object>::New(isolate->m_isolate, _cache->m_cache);
        else
            return v8::Local<v8::Function>::New(isolate->m_isolate, _cache->m_function);
    }

    v8::Local<v8::Object> CreateInstance(Isolate* isolate)
    {
        assert(!m_cd.module);

        v8::Local<v8::Object> o;
        cache* _cache = _init(isolate);

        if (_cache->m_cache.IsEmpty()) {
            o = v8::Local<v8::Function>::New(isolate->m_isolate, _cache->m_function)->NewInstance();
            o->SetAlignedPointerInInternalField(0, 0);
            _cache->m_cache.Reset(isolate->m_isolate, o);

            o = o->Clone();
        } else
            o = v8::Local<v8::Object>::New(isolate->m_isolate, _cache->m_cache)->Clone();

        return o;
    }

    bool init_isolate()
    {
        cache* _cache = _init(Isolate::current());
        if (_cache->m_init_isolate)
            return false;

        _cache->m_init_isolate = true;
        return true;
    }

    bool has(const char* name)
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

    const char* name()
    {
        return m_cd.name;
    }

    void Attach(Isolate* isolate, v8::Local<v8::Object> o, const char** skips = NULL)
    {
        v8::Local<v8::Context> _context = isolate->context();

        int32_t i, j;

        for (i = 0; i < m_cd.mc; i++) {
            if (m_cd.cms[i].is_static) {
                if (skips)
                    for (j = 0; skips[j] && qstrcmp(skips[j], m_cd.cms[i].name); j++)
                        ;

                if (!skips || !skips[j])
                    o->DefineOwnProperty(_context, isolate->NewFromUtf8(m_cd.cms[i].name),
                         isolate->NewFunction(m_cd.name, m_cd.cms[i].invoker),
                         (v8::PropertyAttribute)(v8::ReadOnly | v8::DontDelete))
                        .IsJust();
            }
        }

        for (i = 0; i < m_cd.oc; i++) {
            if (skips)
                for (j = 0; skips[j] && qstrcmp(skips[j], m_cd.cos[i].name); j++)
                    ;

            if (!skips || !skips[j])
                o->DefineOwnProperty(_context, isolate->NewFromUtf8(m_cd.cos[i].name),
                     m_cd.cos[i].invoker().getModule(isolate),
                     (v8::PropertyAttribute)(v8::ReadOnly | v8::DontDelete))
                    .IsJust();
        }

        for (i = 0; i < m_cd.pc; i++)
            if (m_cd.cps[i].is_static) {
                if (skips)
                    for (j = 0; skips[j] && qstrcmp(skips[j], m_cd.cps[i].name); j++)
                        ;

                if (!skips || !skips[j])
                    o->SetAccessor(isolate->NewFromUtf8(m_cd.cps[i].name),
                        m_cd.cps[i].getter, m_cd.cps[i].setter);
            }

        if (m_cd.base)
            m_cd.base->Attach(isolate, o, skips);
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

    ClassData& data()
    {
        return m_cd;
    }

    intptr_t dump(v8::Local<v8::Object>& o)
    {
        Isolate* isolate = Isolate::current();
        intptr_t cnt = refs_;

        if (cnt) {
            o = v8::Object::New(isolate->m_isolate);
            o->Set(isolate->NewFromUtf8("class"),
                isolate->NewFromUtf8(m_cd.name));
            o->Set(isolate->NewFromUtf8("objects"),
                v8::Integer::New(isolate->m_isolate, (int32_t)cnt));

            v8::Local<v8::Array> inherits = v8::Array::New(isolate->m_isolate);

            ClassInfo* p = m_Inherit;
            intptr_t icnt = 0;

            while (p) {
                v8::Local<v8::Object> o1;
                intptr_t cnt1 = p->dump(o1);
                if (cnt1)
                    inherits->Set((int32_t)(icnt++), o1);
                p = p->m_next;
            }

            if (icnt)
                o->Set(isolate->NewFromUtf8("inherits"), inherits);
        }

        return cnt;
    }

private:
    cache* _init(Isolate* isolate)
    {
        if (m_id < 0)
            m_id = (int32_t)isolate->m_classInfo.size();

        void* p = NULL;
        while ((int32_t)isolate->m_classInfo.size() < m_id + 1)
            isolate->m_classInfo.append(p);

        cache* _cache = (cache*)isolate->m_classInfo[m_id];

        if (_cache)
            return _cache;

        isolate->m_classInfo[m_id] = _cache = new cache();

        if (!m_cd.module) {
            v8::Local<v8::FunctionTemplate> _class = v8::FunctionTemplate::New(
                isolate->m_isolate, m_cd.cor);
            _cache->m_class.Reset(isolate->m_isolate, _class);

            _class->SetClassName(isolate->NewFromUtf8(m_cd.name));

            if (m_cd.base) {
                cache* _cache1 = m_cd.base->_init(isolate);
                _class->Inherit(
                    v8::Local<v8::FunctionTemplate>::New(isolate->m_isolate,
                        _cache1->m_class));
            }

            v8::Local<v8::ObjectTemplate> pt = _class->PrototypeTemplate();
            int32_t i;

            for (i = 0; i < m_cd.mc; i++)
                pt->Set(isolate->NewFromUtf8(m_cd.cms[i].name),
                    v8::FunctionTemplate::New(isolate->m_isolate, m_cd.cms[i].invoker),
                    (v8::PropertyAttribute)(v8::ReadOnly | v8::DontDelete));

            for (i = 0; i < m_cd.oc; i++) {
                cache* _cache1 = m_cd.cos[i].invoker()._init(isolate);
                pt->Set(isolate->NewFromUtf8(m_cd.cos[i].name),
                    v8::Local<v8::FunctionTemplate>::New(isolate->m_isolate, _cache1->m_class),
                    (v8::PropertyAttribute)(v8::ReadOnly | v8::DontDelete));
            }

            for (i = 0; i < m_cd.pc; i++)
                pt->SetAccessor(isolate->NewFromUtf8(m_cd.cps[i].name),
                    m_cd.cps[i].getter, m_cd.cps[i].setter,
                    v8::Local<v8::Value>(), v8::DEFAULT, v8::DontDelete);

            v8::Local<v8::ObjectTemplate> ot = _class->InstanceTemplate();
            ot->SetInternalFieldCount(1);

            ClassData* pcd;

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

            if (m_cd.caf)
                ot->SetCallAsFunctionHandler(m_cd.caf);

            v8::Local<v8::Function> _function = _class->GetFunction();
            Attach(isolate, _function);
            _cache->m_function.Reset(isolate->m_isolate, _function);

            if (m_cd.cor) {
                v8::Local<v8::Object> o = _function->NewInstance();
                o->SetAlignedPointerInInternalField(0, 0);
                _cache->m_cache.Reset(isolate->m_isolate, o);
            }
        } else {
            v8::Local<v8::Object> o;

            if (m_cd.caf)
                o = v8::Function::New(isolate->m_isolate, m_cd.caf);
            else
                o = v8::Object::New(isolate->m_isolate);

            Attach(isolate, o);
            _cache->m_cache.Reset(isolate->m_isolate, o);
        }

        return _cache;
    }

private:
    ClassData& m_cd;
    exlib::atomic refs_;
    ClassInfo* m_next;
    ClassInfo* m_Inherit;
    int32_t m_id;
};
}

#endif
