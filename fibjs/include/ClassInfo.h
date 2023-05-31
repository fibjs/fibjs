/***************************************************************************
 *   Copyright (C) 2012 by Leo Hoo                                         *
 *   lion@9465.net                                                         *
 *                                                                         *
 ***************************************************************************/

#pragma once

/**
 @author Leo Hoo <lion@9465.net>
 */

#include <v8/include/v8.h>
#include <string>

namespace fibjs {

extern bool g_track_native_object;
class ClassInfo;

struct ClassData {
    struct ClassProperty {
        const char* name;
        v8::AccessorNameGetterCallback getter;
        v8::AccessorNameSetterCallback setter;
        bool is_static;
    };

    struct ClassConst {
        const char* name;
        int32_t value;
    };

    struct ClassMethod {
        const char* name;
        v8::FunctionCallback invoker;
        bool is_static;
        bool is_async;
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
        v8::GenericNamedPropertyGetterCallback getter;
        v8::GenericNamedPropertySetterCallback setter;
        v8::GenericNamedPropertyDeleterCallback remover;
        v8::GenericNamedPropertyEnumeratorCallback enumerator;
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
    int32_t cc;
    const ClassConst* ccs;
    const ClassIndexed* cis;
    const ClassNamed* cns;
    ClassInfo* base;
};

result_t promisify(Isolate* isolate, v8::Local<v8::Function> func, v8::Local<v8::Function>& retVal);

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

    v8::Local<v8::Function> getFunction(Isolate* isolate)
    {
        assert(!m_cd.module);
        return _init(isolate)->m_function.Get(isolate->m_isolate);
    }

    v8::Local<v8::Object> getModule(Isolate* isolate)
    {
        if (m_cd.module)
            return _init(isolate)->m_cache.Get(isolate->m_isolate);
        else
            return _init(isolate)->m_function.Get(isolate->m_isolate);
    }

    v8::Local<v8::Object> CreateInstance(Isolate* isolate)
    {
        assert(!m_cd.module);
        return _init(isolate)->m_cache.Get(isolate->m_isolate)->Clone();
    }

    bool isInstance(ClassInfo& ci)
    {
        ClassInfo* _ci = &ci;
        while (_ci) {
            if (_ci == this)
                return true;
            _ci = _ci->m_cd.base;
        }

        return false;
    }

    bool init_isolate(Isolate* isolate)
    {
        cache* _cache = _init(isolate);
        if (_cache->m_init_isolate)
            return false;

        _cache->m_init_isolate = true;
        return true;
    }

    v8::Local<v8::Name> get_prop_name(Isolate* isolate, const char* name)
    {
        if (name[0] != '@')
            return isolate->NewString(name);

        if (!qstrcmp("iterator", name + 1))
            return v8::Symbol::GetIterator(isolate->m_isolate);

        if (!qstrcmp("toStringTag", name + 1))
            return v8::Symbol::GetToStringTag(isolate->m_isolate);

        return isolate->NewString(name);
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

        for (i = 0; i < m_cd.cc; i++)
            if (!qstrcmp(name, m_cd.ccs[i].name))
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
        int32_t i, j;
        v8::Local<v8::Context> _context = isolate->context();
        v8::Local<v8::Object> op;

        for (i = 0; i < m_cd.mc; i++) {
            if (m_cd.cms[i].is_static) {
                if (skips)
                    for (j = 0; skips[j] && qstrcmp(skips[j], m_cd.cms[i].name); j++)
                        ;

                if (!skips || !skips[j]) {
                    v8::Local<v8::Function> func = isolate->NewFunction(m_cd.cms[i].name, m_cd.cms[i].invoker);
                    v8::Local<v8::Function> pfunc;
                    v8::Local<v8::Name> name = get_prop_name(isolate, m_cd.cms[i].name);

                    o->Set(_context, name, func).IsJust();

                    if (m_cd.cms[i].is_async) {
                        if (op.IsEmpty())
                            op = v8::Object::New(isolate->m_isolate);

                        promisify(isolate, func, pfunc);
                        op->Set(_context, name, pfunc).IsJust();
                    }
                }
            }
        }

        for (i = 0; i < m_cd.oc; i++) {
            if (skips)
                for (j = 0; skips[j] && qstrcmp(skips[j], m_cd.cos[i].name); j++)
                    ;

            if (!skips || !skips[j])
                o->Set(_context, isolate->NewString(m_cd.cos[i].name),
                     m_cd.cos[i].invoker().getModule(isolate))
                    .IsJust();
        }

        for (i = 0; i < m_cd.pc; i++)
            if (m_cd.cps[i].is_static) {
                if (skips)
                    for (j = 0; skips[j] && qstrcmp(skips[j], m_cd.cps[i].name); j++)
                        ;

                if (!skips || !skips[j])
                    o->SetAccessor(_context, get_prop_name(isolate, m_cd.cps[i].name),
                         m_cd.cps[i].getter, m_cd.cps[i].setter)
                        .IsJust();
            }

        for (i = 0; i < m_cd.cc; i++) {
            if (skips)
                for (j = 0; skips[j] && qstrcmp(skips[j], m_cd.ccs[i].name); j++)
                    ;

            if (!skips || !skips[j])
                o->Set(_context, isolate->NewString(m_cd.ccs[i].name),
                     v8::Integer::New(isolate->m_isolate, m_cd.ccs[i].value))
                    .IsJust();
        }

        if (m_cd.base)
            m_cd.base->Attach(isolate, o, skips);

        if (!op.IsEmpty())
            o->Set(_context, isolate->NewString("promises"), op).IsJust();
    }

public:
    void Ref()
    {
        if (g_track_native_object)
            refs_.inc();
    }

    void Unref()
    {
        if (g_track_native_object)
            refs_.dec();
    }

    ClassData& data()
    {
        return m_cd;
    }

    intptr_t dump(v8::Local<v8::Object>& o)
    {
        Isolate* isolate = Isolate::current();
        v8::Local<v8::Context> context = isolate->context();
        intptr_t cnt = refs_;

        if (cnt) {
            o = v8::Object::New(isolate->m_isolate);
            o->Set(context, isolate->NewString("class"),
                 isolate->NewString(m_cd.name))
                .IsJust();
            o->Set(context, isolate->NewString("objects"),
                 v8::Integer::New(isolate->m_isolate, (int32_t)cnt))
                .IsJust();

            v8::Local<v8::Array> inherits = v8::Array::New(isolate->m_isolate);

            ClassInfo* p = m_Inherit;
            intptr_t icnt = 0;

            while (p) {
                v8::Local<v8::Object> o1;
                intptr_t cnt1 = p->dump(o1);
                if (cnt1)
                    inherits->Set(context, (int32_t)(icnt++), o1).IsJust();
                p = p->m_next;
            }

            if (icnt)
                o->Set(context, isolate->NewString("inherits"), inherits).IsJust();
        }

        return cnt;
    }

private:
    cache* _init(Isolate* isolate)
    {
        static exlib::atomic m_gid;

        if (m_id < 0)
            m_id = (int32_t)m_gid.inc();

        void* p = NULL;
        while ((int32_t)isolate->m_classInfo.size() < m_id)
            isolate->m_classInfo.append(p);

        cache* _cache = (cache*)isolate->m_classInfo[m_id];

        if (_cache)
            return _cache;

        isolate->m_classInfo[m_id] = _cache = new cache();

        v8::Local<v8::Context> context = isolate->context();

        if (!m_cd.module) {
            v8::Local<v8::FunctionTemplate> _class = v8::FunctionTemplate::New(
                isolate->m_isolate, m_cd.cor);
            _cache->m_class.Reset(isolate->m_isolate, _class);

            _class->SetClassName(isolate->NewString(m_cd.name));

            if (m_cd.base) {
                cache* _cache1 = m_cd.base->_init(isolate);
                _class->Inherit(_cache1->m_class.Get(isolate->m_isolate));
            }

            v8::Local<v8::ObjectTemplate> pt = _class->PrototypeTemplate();
            int32_t i;

            pt->Set(get_prop_name(isolate, "@toStringTag"), isolate->NewString(m_cd.name));

            for (i = 0; i < m_cd.mc; i++)
                if (!m_cd.cms[i].is_static)
                    pt->Set(get_prop_name(isolate, m_cd.cms[i].name),
                        v8::FunctionTemplate::New(isolate->m_isolate, m_cd.cms[i].invoker));

            for (i = 0; i < m_cd.pc; i++)
                if (!m_cd.cps[i].is_static)
                    pt->SetAccessor(get_prop_name(isolate, m_cd.cps[i].name),
                        m_cd.cps[i].getter, m_cd.cps[i].setter,
                        v8::Local<v8::Value>(), v8::DEFAULT, v8::DontDelete);

            for (i = 0; i < m_cd.cc; i++) {
                pt->Set(isolate->NewString(m_cd.ccs[i].name),
                    v8::Integer::New(isolate->m_isolate, m_cd.ccs[i].value));
            }

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

            if (pcd) {
                ot->SetHandler(v8::NamedPropertyHandlerConfiguration(
                    pcd->cns->getter, pcd->cns->setter, nullptr, pcd->cns->remover, pcd->cns->enumerator, v8::Local<v8::Value>(),
                    v8::PropertyHandlerFlags::kOnlyInterceptStrings));
            }
            if (m_cd.caf)
                ot->SetCallAsFunctionHandler(m_cd.caf);

            v8::Local<v8::Function> _function = _class->GetFunction(context).FromMaybe(v8::Local<v8::Function>());
            _cache->m_function.Reset(isolate->m_isolate, _function);

            v8::Local<v8::Object> o = _function->NewInstance(isolate->context()).FromMaybe(v8::Local<v8::Object>());

            o->SetAlignedPointerInInternalField(0, 0);
            _cache->m_cache.Reset(isolate->m_isolate, o);

            Attach(isolate, _function);
        } else {
            v8::Local<v8::Object> o;

            if (m_cd.caf)
                o = v8::Function::New(context, m_cd.caf).FromMaybe(v8::Local<v8::Function>());
            else
                o = v8::Object::New(isolate->m_isolate);

            o->Set(context, get_prop_name(isolate, "@toStringTag"), isolate->NewString(m_cd.name)).IsJust();

            _cache->m_cache.Reset(isolate->m_isolate, o);

            Attach(isolate, o);
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
