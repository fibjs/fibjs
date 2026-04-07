/*
 * EventEmitter.h
 *
 *  Created on: Apr 8, 2012
 *      Author: lion
 */

#pragma once

#include "ifs/EventEmitter.h"
#include "Fiber.h"
#include "ifs/coroutine.h"
#include "QuickArray.h"

namespace fibjs {

class JSTrigger {
public:
    JSTrigger(v8::Isolate* _iso, v8::Local<v8::Object> _o)
        : isolate(_iso)
        , o(_o)
    {
        initEv();
    }

    JSTrigger(object_base* _o)
        : isolate(_o->holder()->m_isolate)
        , o(_o->wrap())
    {
        initEv();
    }

    JSTrigger(const v8::FunctionCallbackInfo<v8::Value>& args)
        : isolate(args.GetIsolate())
        , o(args.This())
    {
        initEv();
    }

private:
    v8::Local<v8::String> NewString(exlib::string str)
    {
        return fibjs::NewString(isolate, str);
    }

    void initEv();

public:
    v8::Local<v8::Value> GetPrivate(exlib::string key)
    {
        return JSValue(events->Get(context, NewString(key)));
    }

    // Symbol-aware overload: pass the key directly (string or Symbol)
    v8::Local<v8::Value> GetPrivate(v8::Local<v8::Value> key)
    {
        return JSValue(events->Get(context, key));
    }

    void SetPrivate(exlib::string key, v8::Local<v8::Value> value)
    {
        events->Set(context, NewString(key), value).IsJust();
    }

    // Symbol-aware overload
    void SetPrivate(v8::Local<v8::Value> key, v8::Local<v8::Value> value)
    {
        events->Set(context, key, value).IsJust();
    }

    void DeletePrivate(exlib::string key)
    {
        events->Delete(context, NewString(key)).IsJust();
    }

    // Symbol-aware overload
    void DeletePrivate(v8::Local<v8::Value> key)
    {
        events->Delete(context, key).IsJust();
    }

    v8::Local<v8::Array> GetHiddenList(v8::Local<v8::Value> k, bool create = false);

    // Convenience overload for internal C++ callers using string literals
    v8::Local<v8::Array> GetHiddenList(exlib::string k, bool create = false)
    {
        return GetHiddenList(static_cast<v8::Local<v8::Value>>(NewString(k)), create);
    }

    result_t onEventChange(exlib::string type, v8::Local<v8::Value> ev, v8::Local<v8::Function> func);
    int32_t putFunction(v8::Local<v8::Array> esa, v8::Local<v8::Function> func, v8::Local<v8::Value> ev);

    // String overload for internal C++ callers
    int32_t putFunction(v8::Local<v8::Array> esa, v8::Local<v8::Function> func, exlib::string ev)
    {
        return putFunction(esa, func, static_cast<v8::Local<v8::Value>>(NewString(ev)));
    }

    int32_t prependPutFunction(v8::Local<v8::Array> esa, v8::Local<v8::Function> func, v8::Local<v8::Value> ev);
    void spliceOne(v8::Local<v8::Array> esa, int32_t index);
    int32_t removeFunction(v8::Local<v8::Array> esa, v8::Local<v8::Function> func, v8::Local<v8::Value> ev);
    result_t _map(v8::Local<v8::Object> m,
        result_t (JSTrigger::*fn)(v8::Local<v8::Value>, v8::Local<v8::Function>, v8::Local<v8::Object>&),
        v8::Local<v8::Object>& retVal);

    result_t on(v8::Local<v8::Value> ev, v8::Local<v8::Function> func, v8::Local<v8::Object>& retVal)
    {
        putFunction(GetHiddenList(ev, true), func, ev);
        retVal = o;
        return 0;
    }

    // String overload for internal C++ callers
    result_t on(exlib::string ev, v8::Local<v8::Function> func, v8::Local<v8::Object>& retVal)
    {
        return on(static_cast<v8::Local<v8::Value>>(NewString(ev)), func, retVal);
    }

    result_t on(v8::Local<v8::Object> map, v8::Local<v8::Object>& retVal)
    {
        return _map(map, &JSTrigger::on, retVal);
    }

    result_t prependListener(v8::Local<v8::Value> ev, v8::Local<v8::Function> func, v8::Local<v8::Object>& retVal)
    {
        prependPutFunction(GetHiddenList(ev, true), func, ev);
        retVal = o;
        return 0;
    }

    result_t prependListener(v8::Local<v8::Object> map, v8::Local<v8::Object>& retVal)
    {
        return _map(map, &JSTrigger::prependListener, retVal);
    }

    static void _onceWrap(const v8::FunctionCallbackInfo<v8::Value>& args);
    result_t once(v8::Local<v8::Value> ev, v8::Local<v8::Function> func, v8::Local<v8::Object>& retVal);

    result_t once(v8::Local<v8::Object> map, v8::Local<v8::Object>& retVal)
    {
        return _map(map, &JSTrigger::once, retVal);
    }

    result_t addEventListener(v8::Local<v8::Value> ev, v8::Local<v8::Function> func,
        v8::Local<v8::Object> options, v8::Local<v8::Object>& retVal);
    result_t prependOnceListener(v8::Local<v8::Value> ev, v8::Local<v8::Function> func, v8::Local<v8::Object>& retVal);

    result_t prependOnceListener(v8::Local<v8::Object> map, v8::Local<v8::Object>& retVal)
    {
        return _map(map, &JSTrigger::prependOnceListener, retVal);
    }

    result_t off(v8::Local<v8::Value> ev, v8::Local<v8::Function> func, v8::Local<v8::Object>& retVal);
    result_t off(v8::Local<v8::Value> ev, v8::Local<v8::Object>& retVal);

    // String overloads for internal C++ callers
    result_t off(exlib::string ev, v8::Local<v8::Function> func, v8::Local<v8::Object>& retVal)
    {
        return off(static_cast<v8::Local<v8::Value>>(NewString(ev)), func, retVal);
    }
    result_t off(exlib::string ev, v8::Local<v8::Object>& retVal)
    {
        return off(static_cast<v8::Local<v8::Value>>(NewString(ev)), retVal);
    }

    result_t off(v8::Local<v8::Object> map, v8::Local<v8::Object>& retVal)
    {
        return _map(map, &JSTrigger::off, retVal);
    }

    result_t removeEventListener(v8::Local<v8::Value> ev, v8::Local<v8::Function> func,
        v8::Local<v8::Object> options, v8::Local<v8::Object>& retVal)
    {
        return off(ev, func, retVal);
    }

    result_t removeAllListeners(v8::Local<v8::Value> ev, v8::Local<v8::Object>& retVal)
    {
        return off(ev, retVal);
    }

    result_t removeAllListeners(v8::Local<v8::Array> evs, v8::Local<v8::Object>& retVal);
    result_t setMaxListeners(int32_t n);
    result_t getMaxListeners(int32_t& retVal);
    static result_t set_defaultMaxListeners(int32_t newVal);
    static result_t get_defaultMaxListeners(int32_t& retVal);
    result_t listeners(v8::Local<v8::Value> ev, v8::Local<v8::Array>& retVal);
    result_t rawListeners(v8::Local<v8::Value> ev, v8::Local<v8::Array>& retVal);
    result_t listenerCount(v8::Local<v8::Value> ev, int32_t& retVal);

    // String overload for internal C++ callers
    result_t listenerCount(exlib::string ev, int32_t& retVal)
    {
        return listenerCount(static_cast<v8::Local<v8::Value>>(NewString(ev)), retVal);
    }
    result_t fireTrigger(v8::Local<v8::Array> esa, v8::Local<v8::Value>* args, int32_t argCount,
        QuickArray<obj_ptr<Fiber_base>>& evs, v8::Local<v8::Function>& ff);
    result_t _emit(exlib::string ev, v8::Local<v8::Value>* args, int32_t argCount, bool& retVal);

    class AsyncEmitter {
    public:
        AsyncEmitter(Isolate* isolate, v8::Local<v8::Object> o)
            : m_isolate(isolate)
            , m_o(isolate->m_isolate, o)
        {
        }

        AsyncEmitter(Isolate* isolate, object_base* obj)
            : m_isolate(isolate)
            , m_obj(obj)
        {
        }

    public:
        result_t emitter_func()
        {
            JSFiber::EnterJsScope s;
            size_t i, sz;
            bool r;

            std::vector<v8::Local<v8::Value>> argv;

            sz = m_variant_args.size();
            if (sz) {
                argv.resize(sz);
                for (i = 0; i < sz; i++)
                    argv[i] = m_variant_args[i];
            } else {
                sz = m_value_args.size();
                if (sz) {
                    argv.resize(sz);
                    for (i = 0; i < sz; i++)
                        argv[i] = m_value_args[i].Get(m_isolate->m_isolate);
                }
            }

            if (!m_obj)
                m_obj = object_base::getInstance(m_o.Get(m_isolate->m_isolate));

            if (m_obj) {
                JSTrigger(m_obj)._emit(m_ev, argv.data(), (int32_t)argv.size(), r);
            } else
                JSTrigger(m_isolate->m_isolate, m_o.Get(m_isolate->m_isolate))._emit(m_ev, argv.data(), (int32_t)argv.size(), r);

            delete this;

            return 0;
        }

        void emit(exlib::string ev, Variant* args, int32_t argCount)
        {
            m_ev = ev;
            m_variant_args.resize(argCount);

            for (int32_t i = 0; i < argCount; i++)
                m_variant_args[i] = args[i];

            m_isolate->sync([this]() -> int {
                return emitter_func();
            });
        }

        void emit(exlib::string ev, v8::Local<v8::Value>* args, int32_t argCount)
        {
            m_ev = ev;
            m_value_args.resize(argCount);

            for (int32_t i = 0; i < argCount; i++)
                m_value_args[i].Reset(m_isolate->m_isolate, args[i]);

            m_isolate->sync([this]() -> int {
                return emitter_func();
            });
        }

    public:
        Isolate* m_isolate;
        v8::Global<v8::Object> m_o;
        obj_ptr<object_base> m_obj;
        exlib::string m_ev;
        std::vector<Variant> m_variant_args;
        std::vector<v8::Global<v8::Value>> m_value_args;
    };

    result_t emit(v8::Local<v8::Value> ev, OptArgs args, bool& retVal);

    result_t eventNames(v8::Local<v8::Array>& retVal)
    {
        retVal = JSArray(events->GetOwnPropertyNames(context));
        return 0;
    }

public:
    static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_on(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_addEventListener(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_prependListener(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_once(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_prependOnceListener(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_off(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_removeEventListener(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_removeAllListeners(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_setMaxListeners(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_getMaxListeners(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_defaultMaxListeners(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_set_defaultMaxListeners(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_listeners(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_rawListeners(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_listenerCount(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_emit(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_eventNames(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void _once_event_cb(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void _once_error_cb(const v8::FunctionCallbackInfo<v8::Value>& args);
    static v8::Local<v8::Value> _make_abort_error(Isolate* _isolate);
    static void _once_abort_cb(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void _on_event_cb(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void _on_cleanup(Isolate* _isolate, v8::Local<v8::Object> _data);
    static void _on_error_cb(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void _on_close_cb(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void _disposeAbortListener(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_addAbortListener(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void _add_signal_listener(Isolate* _isolate, v8::Local<v8::Object> sigObj,
        v8::Local<v8::Function> cb);
    static void s_once_static(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void _on_next_cb(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void _on_return_cb(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void _on_throw_cb(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_on_static(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void _on_error_cb_invoke(Isolate* _isolate, v8::Local<v8::Object> _data,
        v8::Local<v8::Value> err);

public:
    v8::Isolate* isolate;
    v8::Local<v8::Context> context;
    v8::Local<v8::Object> o;
    v8::Local<v8::Object> events;
};

class EventEmitter : public EventEmitter_base {
    EVENT_SUPPORT();
};
}
