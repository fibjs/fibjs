/***************************************************************************
 *   Copyright (C) 2012 by Leo Hoo                                         *
 *   lion@9465.net                                                         *
 *                                                                         *
 ***************************************************************************/

#pragma once

/**
 @author Leo Hoo <lion@9465.net>
 */

#include <v8.h>
#include <string>

namespace fibjs {

extern bool g_track_native_object;
class ClassInfo;

struct ClassData {

    enum AsyncType {
        ASYNC_SYNC = 0,
        ASYNC_ASYNC = 1,
        ASYNC_PROMISE = 2
    };

    struct ClassProperty {
        const char* name;
        v8::FunctionCallback getter;
        v8::FunctionCallback setter;
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
        AsyncType async_type;
    };

    struct ClassObject {
        const char* name;
        ClassInfo& (*invoker)();
    };

    struct ClassIndexed {
        v8::IndexedPropertyGetterCallbackV2 getter;
        v8::IndexedPropertySetterCallbackV2 setter;
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
    int32_t cc;
    const ClassConst* ccs;
    const ClassIndexed* cis;
    const ClassNamed* cns;
    ClassInfo* base;
    bool has_async;
};

class ClassInfo {
public:
    class cache {
    public:
        cache()
            : m_init_isolate(0)
        {
        }

    public:
        int32_t m_init_isolate;

        v8::Global<v8::FunctionTemplate> m_class;
        v8::Global<v8::Object> m_cache;
        v8::Global<v8::Function> m_function;

        v8::Global<v8::FunctionTemplate> m_pclass;
        v8::Global<v8::Object> m_pcache;
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

    v8::Local<v8::Value> GetAsyncPrototype(Isolate* isolate)
    {
        assert(!m_cd.module);
        return _init(isolate)->m_pcache.Get(isolate->m_isolate)->GetPrototype();
    }

    bool hasAsync()
    {
        return m_cd.has_async;
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

        if (_cache->m_init_isolate > (m_cd.has_async ? 1 : 0))
            return false;

        _cache->m_init_isolate++;
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

    void Attach(Isolate* isolate, v8::Local<v8::Object> o)
    {
        int32_t i;
        v8::Local<v8::Context> _context = isolate->context();
        v8::Local<v8::Object> op;

        if (m_cd.has_async)
            op = v8::Object::New(isolate->m_isolate);

        for (i = 0; i < m_cd.mc; i++) {
            if (m_cd.cms[i].is_static) {
                v8::Local<v8::Function> func = isolate->NewFunction(m_cd.cms[i].name, m_cd.cms[i].invoker);
                v8::Local<v8::Name> name = get_prop_name(isolate, m_cd.cms[i].name);

                if (m_cd.cms[i].async_type == ClassData::ASYNC_SYNC) {
                    o->Set(_context, name, func).IsJust();
                    if (m_cd.has_async)
                        op->Set(_context, name, func).IsJust();
                } else {
                    func->SetPrivate(_context, v8::Private::ForApi(isolate->m_isolate, isolate->NewString("_async")), func);
                    func->SetPrivate(_context, v8::Private::ForApi(isolate->m_isolate, isolate->NewString("_sync")), func);

                    v8::Local<v8::Function> pfunc = isolate->NewFunction(m_cd.cms[i].name, m_cd.cms[i].invoker, v8::True(isolate->m_isolate));
                    setAsyncFunctoin(pfunc);

                    func->SetPrivate(_context, v8::Private::ForApi(isolate->m_isolate, isolate->NewString("_promise")), pfunc);
                    pfunc->SetPrivate(_context, v8::Private::ForApi(isolate->m_isolate, isolate->NewString("_async")), func);
                    pfunc->SetPrivate(_context, v8::Private::ForApi(isolate->m_isolate, isolate->NewString("_sync")), func);

                    exlib::string name_sync(m_cd.cms[i].name);
                    name_sync.append("Sync");
                    v8::Local<v8::Name> _name_sync = get_prop_name(isolate, name_sync.c_str());

                    exlib::string name_async(m_cd.cms[i].name);
                    name_async.append("Async");
                    v8::Local<v8::Name> _name_async = get_prop_name(isolate, name_async.c_str());

                    if (m_cd.cms[i].async_type == ClassData::ASYNC_ASYNC)
                        o->Set(_context, name, func).IsJust();
                    else
                        o->Set(_context, name, pfunc).IsJust();
                    o->Set(_context, _name_sync, func).IsJust();
                    o->Set(_context, _name_async, pfunc).IsJust();

                    if (m_cd.has_async) {
                        op->Set(_context, name, pfunc).IsJust();
                        op->Set(_context, _name_sync, func).IsJust();
                        op->Set(_context, _name_async, pfunc).IsJust();
                    }
                }
            }
        }

        for (i = 0; i < m_cd.oc; i++) {
            o->Set(_context, isolate->NewString(m_cd.cos[i].name),
                 m_cd.cos[i].invoker().getModule(isolate))
                .IsJust();
        }

        for (i = 0; i < m_cd.pc; i++)
            if (m_cd.cps[i].is_static) {
                o->SetAccessorProperty(get_prop_name(isolate, m_cd.cps[i].name),
                    isolate->NewFunction(m_cd.cps[i].name, m_cd.cps[i].getter),
                    isolate->NewFunction(m_cd.cps[i].name, m_cd.cps[i].setter));
            }

        for (i = 0; i < m_cd.cc; i++) {
            o->Set(_context, isolate->NewString(m_cd.ccs[i].name),
                 v8::Integer::New(isolate->m_isolate, m_cd.ccs[i].value))
                .IsJust();
        }

        if (m_cd.base)
            m_cd.base->Attach(isolate, o);

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
        v8::Local<v8::String> class_name = isolate->NewString(m_cd.name);

        if (!m_cd.module) {
            v8::Local<v8::FunctionTemplate> _pclass;
            v8::Local<v8::FunctionTemplate> _class = v8::FunctionTemplate::New(isolate->m_isolate, m_cd.cor);

            _class->SetClassName(class_name);
            _cache->m_class.Reset(isolate->m_isolate, _class);

            if (m_cd.has_async) {
                _pclass = v8::FunctionTemplate::New(isolate->m_isolate, m_cd.cor);
                _pclass->SetClassName(class_name);

                _cache->m_pclass.Reset(isolate->m_isolate, _pclass);
            } else
                _cache->m_pclass.Reset(isolate->m_isolate, _class);

            if (m_cd.base) {
                cache* _cache1 = m_cd.base->_init(isolate);
                _class->Inherit(_cache1->m_class.Get(isolate->m_isolate));
                if (m_cd.has_async)
                    _pclass->Inherit(_cache1->m_pclass.Get(isolate->m_isolate));
            }

            v8::Local<v8::ObjectTemplate> pt = _class->PrototypeTemplate();
            pt->Set(get_prop_name(isolate, "@toStringTag"), class_name,
                v8::PropertyAttribute::DontEnum);

            v8::Local<v8::ObjectTemplate> ppt;
            if (m_cd.has_async) {
                ppt = _pclass->PrototypeTemplate();
                ppt->Set(get_prop_name(isolate, "@toStringTag"), class_name,
                    v8::PropertyAttribute::DontEnum);
            }

            int32_t i;
            for (i = 0; i < m_cd.mc; i++)
                if (!m_cd.cms[i].is_static) {
                    v8::Local<v8::FunctionTemplate> ft = v8::FunctionTemplate::New(isolate->m_isolate, m_cd.cms[i].invoker);
                    v8::Local<v8::Name> name = get_prop_name(isolate, m_cd.cms[i].name);

                    if (m_cd.cms[i].async_type == ClassData::ASYNC_SYNC) {
                        pt->Set(name, ft);
                        if (m_cd.has_async)
                            ppt->Set(name, ft);
                    } else {
                        v8::Local<v8::Function> func = ft->GetFunction(context).FromMaybe(v8::Local<v8::Function>());
                        func->SetPrivate(context, v8::Private::ForApi(isolate->m_isolate, isolate->NewString("_async")), func);
                        func->SetPrivate(context, v8::Private::ForApi(isolate->m_isolate, isolate->NewString("_sync")), func);

                        v8::Local<v8::FunctionTemplate> pft = v8::FunctionTemplate::New(isolate->m_isolate, m_cd.cms[i].invoker, v8::True(isolate->m_isolate));
                        v8::Local<v8::Function> pfunc = pft->GetFunction(context).FromMaybe(v8::Local<v8::Function>());

                        setAsyncFunctoin(pfunc);

                        func->SetPrivate(context, v8::Private::ForApi(isolate->m_isolate, isolate->NewString("_promise")), pfunc);
                        pfunc->SetPrivate(context, v8::Private::ForApi(isolate->m_isolate, isolate->NewString("_async")), func);
                        pfunc->SetPrivate(context, v8::Private::ForApi(isolate->m_isolate, isolate->NewString("_sync")), func);

                        exlib::string name_sync(m_cd.cms[i].name);
                        name_sync.append("Sync");
                        v8::Local<v8::Name> _name_sync = get_prop_name(isolate, name_sync.c_str());

                        exlib::string name_async(m_cd.cms[i].name);
                        name_async.append("Async");
                        v8::Local<v8::Name> _name_async = get_prop_name(isolate, name_async.c_str());

                        if (m_cd.cms[i].async_type == ClassData::ASYNC_ASYNC)
                            pt->Set(name, ft);
                        else
                            pt->Set(name, pft);
                        pt->Set(_name_sync, ft);
                        pt->Set(_name_async, pft);

                        if (m_cd.has_async) {
                            ppt->Set(name, pft);
                            ppt->Set(_name_sync, ft);
                            ppt->Set(_name_async, pft);
                        }
                    }
                }

            for (i = 0; i < m_cd.pc; i++)
                if (!m_cd.cps[i].is_static) {
                    v8::Local<v8::Name> name = get_prop_name(isolate, m_cd.cps[i].name);
                    v8::Local<v8::FunctionTemplate> ft_getter = v8::FunctionTemplate::New(isolate->m_isolate, m_cd.cps[i].getter);
                    v8::Local<v8::FunctionTemplate> ft_setter = v8::FunctionTemplate::New(isolate->m_isolate, m_cd.cps[i].setter);

                    pt->SetAccessorProperty(name, ft_getter, ft_setter, v8::DontDelete);
                    if (m_cd.has_async)
                        ppt->SetAccessorProperty(name, ft_getter, ft_setter, v8::DontDelete);
                }

            for (i = 0; i < m_cd.cc; i++) {
                v8::Local<v8::String> name = isolate->NewString(m_cd.ccs[i].name);

                pt->Set(name, v8::Integer::New(isolate->m_isolate, m_cd.ccs[i].value));
                if (m_cd.has_async)
                    ppt->Set(name, v8::Integer::New(isolate->m_isolate, m_cd.ccs[i].value));
            }

            v8::Local<v8::ObjectTemplate> ot = _class->InstanceTemplate();
            ot->SetInternalFieldCount(1);

            v8::Local<v8::ObjectTemplate> pot;
            if (m_cd.has_async) {
                pot = _pclass->InstanceTemplate();
                pot->SetInternalFieldCount(1);
            }

            ClassData* pcd;

            pcd = &m_cd;
            while (pcd && !pcd->cis)
                pcd = pcd->base ? &pcd->base->m_cd : NULL;

            if (pcd) {
                ot->SetHandler(v8::IndexedPropertyHandlerConfiguration(
                    pcd->cis->getter, pcd->cis->setter));
                if (m_cd.has_async)
                    pot->SetHandler(v8::IndexedPropertyHandlerConfiguration(
                        pcd->cis->getter, pcd->cis->setter));
            }

            pcd = &m_cd;
            while (pcd && !pcd->cns)
                pcd = pcd->base ? &pcd->base->m_cd : NULL;

            if (pcd) {
                ot->SetHandler(v8::NamedPropertyHandlerConfiguration(
                    pcd->cns->getter, pcd->cns->setter, nullptr, pcd->cns->remover, pcd->cns->enumerator, v8::Local<v8::Value>(),
                    v8::PropertyHandlerFlags::kOnlyInterceptStrings));

                if (m_cd.has_async)
                    pot->SetHandler(v8::NamedPropertyHandlerConfiguration(
                        pcd->cns->getter, pcd->cns->setter, nullptr, pcd->cns->remover, pcd->cns->enumerator, v8::Local<v8::Value>(),
                        v8::PropertyHandlerFlags::kOnlyInterceptStrings));
            }
            if (m_cd.caf) {
                ot->SetCallAsFunctionHandler(m_cd.caf);
                if (m_cd.has_async)
                    pot->SetCallAsFunctionHandler(m_cd.caf);
            }

            v8::Local<v8::Function> _function = _class->GetFunction(context).FromMaybe(v8::Local<v8::Function>());
            _cache->m_function.Reset(isolate->m_isolate, _function);

            v8::Local<v8::Object> o = _function->NewInstance(isolate->context()).FromMaybe(v8::Local<v8::Object>());

            o->SetAlignedPointerInInternalField(0, 0);
            _cache->m_cache.Reset(isolate->m_isolate, o);

            Attach(isolate, _function);

            if (m_cd.has_async) {
                v8::Local<v8::Function> _pfunction = _pclass->GetFunction(context).FromMaybe(v8::Local<v8::Function>());

                v8::Local<v8::Object> po = _pfunction->NewInstance(isolate->context()).FromMaybe(v8::Local<v8::Object>());

                po->SetAlignedPointerInInternalField(0, 0);
                _cache->m_pcache.Reset(isolate->m_isolate, po);
            } else
                _cache->m_pcache.Reset(isolate->m_isolate, o);

        } else {
            v8::Local<v8::Object> o;

            if (m_cd.caf)
                o = v8::Function::New(context, m_cd.caf).FromMaybe(v8::Local<v8::Function>());
            else
                o = v8::Object::New(isolate->m_isolate);

            o->Set(context, get_prop_name(isolate, "@toStringTag"), class_name).IsJust();

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
