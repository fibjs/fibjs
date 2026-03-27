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

    enum ConstType {
        CONST_Integer = 0,
        CONST_Boolean = 1,
        CONST_String = 2,
        CONST_Long = 3
    };

    struct ClassConst {
        const char* name;
        ConstType type;
        union {
            int32_t intValue;
            int64_t longValue;
            bool boolValue;
            const char* stringValue;
        };
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

// Wrapper for prototype property setters: when 'this' is not a native instance
// (e.g. Object.create(proto)), or property is readonly, fall back to defining
// a data property on 'this'. This enables Node.js patterns like
// Express's Object.create(http.ServerResponse.prototype).
inline void prop_setter_wrapper(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    ClassData::ClassProperty* cp = (ClassData::ClassProperty*)v8::Local<v8::External>::Cast(args.Data())->Value();

    v8::Local<v8::Object> self = args.This();
    if (self->InternalFieldCount() > 0) {
        // Native instance: delegate to the native setter (which may throw for readonly props).
        // Never fall back to CreateDataProperty — that would shadow the native getter.
        if (cp->setter)
            cp->setter(args);
        return;
    }

    // Non-native instance (e.g. Object.create(proto)): create a JS data property so
    // that assignment works on the derived object.
    v8::Local<v8::Context> context = args.GetIsolate()->GetCurrentContext();
    v8::Local<v8::String> name = v8::String::NewFromUtf8(args.GetIsolate(), cp->name).ToLocalChecked();
    self->CreateDataProperty(context, name, args[0]).FromMaybe(false);
}

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
        v8::Global<v8::Function> m_pfunction;
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
        ex_assert(!m_cd.module);
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
        ex_assert(!m_cd.module);
        return _init(isolate)->m_cache.Get(isolate->m_isolate)->Clone();
    }

    v8::Local<v8::Object> getAsyncModule(Isolate* isolate)
    {
        ex_assert(!m_cd.module);
        return _init(isolate)->m_pfunction.Get(isolate->m_isolate);
    }

    v8::Local<v8::Value> GetAsyncPrototype(Isolate* isolate)
    {
        ex_assert(!m_cd.module);
        return _init(isolate)->m_pcache.Get(isolate->m_isolate)->GetPrototype();
    }

    v8::Local<v8::Value> GetPrototype(Isolate* isolate)
    {
        ex_assert(!m_cd.module);
        return _init(isolate)->m_cache.Get(isolate->m_isolate)->GetPrototype();
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

    // Enumerates all non-static instance property names from the full class hierarchy,
    // so that Object.assign() / Object.keys() / spread can see them as own keys
    // (values are still read via the prototype chain accessor — zero extra overhead).
    static void s_proto_prop_enumerator(const v8::PropertyCallbackInfo<v8::Array>& info)
    {
        ClassInfo* ci = (ClassInfo*)v8::Local<v8::External>::Cast(info.Data())->Value();
        v8::Isolate* isolate = info.GetIsolate();
        v8::Local<v8::Context> ctx = isolate->GetCurrentContext();

        int32_t count = 0;
        for (ClassData* cd = &ci->m_cd; cd; cd = cd->base ? &cd->base->m_cd : nullptr)
            for (int32_t i = 0; i < cd->pc; i++)
                if (!cd->cps[i].is_static)
                    count++;

        v8::Local<v8::Array> arr = v8::Array::New(isolate, count);
        int32_t idx = 0;
        for (ClassData* cd = &ci->m_cd; cd; cd = cd->base ? &cd->base->m_cd : nullptr)
            for (int32_t i = 0; i < cd->pc; i++)
                if (!cd->cps[i].is_static)
                    arr->Set(ctx, idx++,
                        v8::String::NewFromUtf8(isolate, cd->cps[i].name).ToLocalChecked())
                        .IsJust();
        info.GetReturnValue().Set(arr);
    }

    // Reports prototype accessor properties as own+enumerable so that [[GetOwnProperty]]
    // sees them. The actual Get() still goes through the prototype chain accessor.
    static v8::Intercepted s_proto_prop_query(
        v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Integer>& info)
    {
        ClassInfo* ci = (ClassInfo*)v8::Local<v8::External>::Cast(info.Data())->Value();
        if (!property->IsString())
            return v8::Intercepted::kNo;
        v8::String::Utf8Value propName(info.GetIsolate(), property);
        if (!*propName)
            return v8::Intercepted::kNo;
        for (ClassData* cd = &ci->m_cd; cd; cd = cd->base ? &cd->base->m_cd : nullptr)
            for (int32_t i = 0; i < cd->pc; i++)
                if (!cd->cps[i].is_static && !::strcmp(cd->cps[i].name, *propName)) {
                    // ReadOnly: writable=false, enumerable=true, configurable=true.
                    // Object.assign only requires enumerable; ReadOnly prevents
                    // incorrect writable=true in getOwnPropertyDescriptor results.
                    info.GetReturnValue().Set(v8::ReadOnly);
                    return v8::Intercepted::kYes;
                }
        return v8::Intercepted::kNo;
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

    bool has_declared_method(const char* method_name, bool is_static)
    {
        for (int32_t i = 0; i < m_cd.mc; i++) {
            if (m_cd.cms[i].is_static != is_static)
                continue;
            if (!qstrcmp(m_cd.cms[i].name, method_name))
                return true;
        }

        return false;
    }

    template <typename SetMainPrimaryFn, typename SetMainAliasFn, typename SetPromisePrimaryFn, typename SetPromiseAliasFn>
    void bind_async_method(Isolate* isolate, v8::Local<v8::Context> context,
        const char* method_name, bool is_static, ClassData::AsyncType async_type,
        v8::Local<v8::Function> sync_func, v8::Local<v8::Function> promise_func,
        SetMainPrimaryFn set_main_primary,
        SetMainAliasFn set_main_alias,
        bool has_promise_target,
        SetPromisePrimaryFn set_promise_primary,
        SetPromiseAliasFn set_promise_alias)
    {
        sync_func->SetPrivate(context, v8::Private::ForApi(isolate->m_isolate, isolate->NewString("_async")), sync_func);
        sync_func->SetPrivate(context, v8::Private::ForApi(isolate->m_isolate, isolate->NewString("_sync")), sync_func);

        setAsyncFunctoin(promise_func);

        sync_func->SetPrivate(context, v8::Private::ForApi(isolate->m_isolate, isolate->NewString("_promise")), promise_func);
        promise_func->SetPrivate(context, v8::Private::ForApi(isolate->m_isolate, isolate->NewString("_async")), sync_func);
        promise_func->SetPrivate(context, v8::Private::ForApi(isolate->m_isolate, isolate->NewString("_sync")), sync_func);

        exlib::string name_sync(method_name);
        name_sync.append("Sync");
        v8::Local<v8::Name> prop_sync = get_prop_name(isolate, name_sync.c_str());

        exlib::string name_async(method_name);
        name_async.append("Async");
        v8::Local<v8::Name> prop_async = get_prop_name(isolate, name_async.c_str());

        bool has_name_sync = has_declared_method(name_sync.c_str(), is_static);
        bool has_name_async = has_declared_method(name_async.c_str(), is_static);

        bool use_sync_primary = (async_type == ClassData::ASYNC_ASYNC);
        set_main_primary(use_sync_primary);

        if (!has_name_sync)
            set_main_alias(true, prop_sync);
        if (!has_name_async)
            set_main_alias(false, prop_async);

        if (has_promise_target) {
            set_promise_primary();
            if (!has_name_sync)
                set_promise_alias(true, prop_sync);
            if (!has_name_async)
                set_promise_alias(false, prop_async);
        }
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
                    v8::Local<v8::Function> pfunc = isolate->NewFunction(m_cd.cms[i].name, m_cd.cms[i].invoker, v8::True(isolate->m_isolate));
                    bind_async_method(isolate, _context, m_cd.cms[i].name, true, m_cd.cms[i].async_type,
                        func, pfunc,
                        [&](bool use_sync) {
                            o->Set(_context, name, use_sync ? func : pfunc).IsJust();
                        },
                        [&](bool use_sync, v8::Local<v8::Name> alias) {
                            o->Set(_context, alias, use_sync ? func : pfunc).IsJust();
                        },
                        m_cd.has_async,
                        [&]() {
                            op->Set(_context, name, pfunc).IsJust();
                        },
                        [&](bool use_sync, v8::Local<v8::Name> alias) {
                            op->Set(_context, alias, use_sync ? func : pfunc).IsJust();
                        });
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
            v8::Local<v8::Value> constVal;
            switch (m_cd.ccs[i].type) {
            case ClassData::CONST_Integer:
                constVal = v8::Integer::New(isolate->m_isolate, m_cd.ccs[i].intValue);
                break;
            case ClassData::CONST_Long:
                constVal = v8::Number::New(isolate->m_isolate, (double)m_cd.ccs[i].longValue);
                break;
            case ClassData::CONST_Boolean:
                constVal = v8::Boolean::New(isolate->m_isolate, m_cd.ccs[i].boolValue);
                break;
            case ClassData::CONST_String:
                constVal = isolate->NewString(m_cd.ccs[i].stringValue);
                break;
            }
            o->Set(_context, isolate->NewString(m_cd.ccs[i].name), constVal).IsJust();
        }

        if (m_cd.base)
            m_cd.base->Attach(isolate, o);

        if (!op.IsEmpty())
            o->Set(_context, isolate->NewString("promises"), op).IsJust();
    }

public:
    void RefClass()
    {
        refs_.inc();
    }

    void UnrefClass()
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

                        v8::Local<v8::FunctionTemplate> pft = v8::FunctionTemplate::New(isolate->m_isolate, m_cd.cms[i].invoker, v8::True(isolate->m_isolate));
                        v8::Local<v8::Function> pfunc = pft->GetFunction(context).FromMaybe(v8::Local<v8::Function>());
                        bind_async_method(isolate, context, m_cd.cms[i].name, false, m_cd.cms[i].async_type,
                            func, pfunc,
                            [&](bool use_sync) {
                                pt->Set(name, use_sync ? ft : pft);
                            },
                            [&](bool use_sync, v8::Local<v8::Name> alias) {
                                pt->Set(alias, use_sync ? ft : pft);
                            },
                            m_cd.has_async,
                            [&]() {
                                ppt->Set(name, pft);
                            },
                            [&](bool use_sync, v8::Local<v8::Name> alias) {
                                ppt->Set(alias, use_sync ? ft : pft);
                            });
                    }
                }

            for (i = 0; i < m_cd.pc; i++)
                if (!m_cd.cps[i].is_static) {
                    v8::Local<v8::Name> name = get_prop_name(isolate, m_cd.cps[i].name);
                    v8::Local<v8::FunctionTemplate> ft_getter = v8::FunctionTemplate::New(isolate->m_isolate, m_cd.cps[i].getter);

                    v8::Local<v8::External> setter_data = v8::External::New(isolate->m_isolate, (void*)&m_cd.cps[i]);
                    v8::Local<v8::FunctionTemplate> ft_setter = v8::FunctionTemplate::New(isolate->m_isolate, prop_setter_wrapper, setter_data);

                    pt->SetAccessorProperty(name, ft_getter, ft_setter, v8::None);
                    if (m_cd.has_async)
                        ppt->SetAccessorProperty(name, ft_getter, ft_setter, v8::None);
                }

            for (i = 0; i < m_cd.cc; i++) {
                v8::Local<v8::String> name = isolate->NewString(m_cd.ccs[i].name);
                v8::Local<v8::Value> constVal;
                switch (m_cd.ccs[i].type) {
                case ClassData::CONST_Integer:
                    constVal = v8::Integer::New(isolate->m_isolate, m_cd.ccs[i].intValue);
                    break;
                case ClassData::CONST_Long:
                    constVal = v8::Number::New(isolate->m_isolate, (double)m_cd.ccs[i].longValue);
                    break;
                case ClassData::CONST_Boolean:
                    constVal = v8::Boolean::New(isolate->m_isolate, m_cd.ccs[i].boolValue);
                    break;
                case ClassData::CONST_String:
                    constVal = isolate->NewString(m_cd.ccs[i].stringValue);
                    break;
                }

                pt->Set(name, constVal);
                if (m_cd.has_async)
                    ppt->Set(name, constVal);
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
            } else {
                // No custom named handler: install query+enumerator so prototype accessor
                // properties appear as own enumerable properties, enabling
                // Object.assign({}, obj) / Object.keys(obj) / {...obj} to work correctly.
                bool has_instance_props = false;
                for (ClassData* tmp = &m_cd; tmp && !has_instance_props;
                     tmp = tmp->base ? &tmp->base->m_cd : nullptr)
                    for (int32_t j = 0; j < tmp->pc; j++)
                        if (!tmp->cps[j].is_static) {
                            has_instance_props = true;
                            break;
                        }

                if (has_instance_props) {
                    v8::Local<v8::External> ci_data =
                        v8::External::New(isolate->m_isolate, (void*)this);
                    ot->SetHandler(v8::NamedPropertyHandlerConfiguration(
                        nullptr, nullptr, s_proto_prop_query, nullptr,
                        s_proto_prop_enumerator, ci_data,
                        v8::PropertyHandlerFlags::kOnlyInterceptStrings));
                    if (m_cd.has_async)
                        pot->SetHandler(v8::NamedPropertyHandlerConfiguration(
                            nullptr, nullptr, s_proto_prop_query, nullptr,
                            s_proto_prop_enumerator, ci_data,
                            v8::PropertyHandlerFlags::kOnlyInterceptStrings));
                }
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
                _cache->m_pfunction.Reset(isolate->m_isolate, _pfunction);
                Attach(isolate, _pfunction);
            } else {
                _cache->m_pcache.Reset(isolate->m_isolate, o);
                _cache->m_pfunction.Reset(isolate->m_isolate, _function);
            }

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
