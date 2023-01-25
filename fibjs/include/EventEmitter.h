/*
 * Event.h
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

    void initEv()
    {
        context = o->GetCreationContextChecked();
        JSValue obj = o->GetPrivate(context, v8::Private::ForApi(isolate, NewString("_ev")));
        if (obj->IsUndefined() || obj->IsNull()) {
            events = v8::Object::New(isolate);
            o->SetPrivate(context, v8::Private::ForApi(isolate, NewString("_ev")), events);
        } else {
            events = v8::Local<v8::Object>::Cast(obj);
        }
    }

public:
    v8::Local<v8::Value> GetPrivate(exlib::string key)
    {
        return JSValue(events->Get(context, NewString(key)));
    }

    void SetPrivate(exlib::string key, v8::Local<v8::Value> value)
    {
        events->Set(context, NewString(key), value).Check();
    }

    void DeletePrivate(exlib::string key)
    {
        events->Delete(context, NewString(key)).Check();
    }

    v8::Local<v8::Array> GetHiddenList(exlib::string k, bool create = false)
    {
        v8::Local<v8::Value> es = GetPrivate(k);
        v8::Local<v8::Array> esa;

        if (es->IsUndefined()) {
            if (create) {
                esa = v8::Array::New(isolate);
                SetPrivate(k, esa);
            }
        } else
            esa = v8::Local<v8::Array>::Cast(es);

        return esa;
    }

    inline result_t onEventChange(exlib::string type, exlib::string ev, v8::Local<v8::Function> func)
    {
        v8::Local<v8::Value> _args[3];
        bool b;

        _args[0] = NewString(ev);

        _args[1] = JSValue(func->Get(context, NewString("_func")));
        if (_args[1]->IsUndefined())
            _args[1] = func;
        else
            func = v8::Local<v8::Function>::Cast(_args[1]);

        obj_ptr<object_base> pThis = object_base::getInstance(o);
        if (pThis)
            pThis->onEventChange(type, ev, func);

        return _emit(type, _args, 2, b);
    }

    inline int32_t putFunction(v8::Local<v8::Array> esa, v8::Local<v8::Function> func, exlib::string ev)
    {
        result_t hr;
        hr = onEventChange("newListener", ev, func);
        if (hr < 0)
            return hr;

        int32_t len = esa->Length();
        esa->Set(context, len, func).Check();
        return 0;
    }

    inline int32_t prependPutFunction(v8::Local<v8::Array> esa, v8::Local<v8::Function> func, exlib::string ev)
    {
        result_t hr;
        hr = onEventChange("newListener", ev, func);
        if (hr < 0)
            return hr;

        int32_t len = esa->Length();
        int32_t i;

        for (i = len; i > 0; i--)
            esa->Set(context, i, esa->Get(context, i - 1).FromMaybe(v8::Local<v8::Value>())).Check();

        esa->Set(context, 0, func).Check();
        return 0;
    }

    inline void spliceOne(v8::Local<v8::Array> esa, int32_t index)
    {
        int32_t i;
        int32_t len = esa->Length();
        for (i = index; i < len - 1; i++)
            esa->Set(context, i, esa->Get(context, i + 1).FromMaybe(v8::Local<v8::Value>())).Check();
        esa->Delete(context, len - 1).ToChecked();
        esa->Set(context, NewString("length"),
            v8::Integer::New(isolate, len - 1)).Check();
    }

    inline int32_t removeFunction(v8::Local<v8::Array> esa, v8::Local<v8::Function> func, exlib::string ev)
    {
        if (esa.IsEmpty())
            return 0;

        int32_t len = esa->Length();
        int32_t i;

        for (i = len - 1; i >= 0; i--) {
            JSValue v = esa->Get(context, i);
            if (v->Equals(isolate->GetCurrentContext(), func).ToChecked()) {
                spliceOne(esa, i);
                result_t hr;
                hr = onEventChange("removeListener", ev, func);
                if (hr < 0)
                    return hr;
                return 0;
            }
        }

        return 0;
    }

    inline result_t _map(v8::Local<v8::Object> m,
        result_t (JSTrigger::*fn)(exlib::string, v8::Local<v8::Function>, v8::Local<v8::Object>&),
        v8::Local<v8::Object>& retVal)
    {
        JSArray ks = m->GetPropertyNames(context);
        int32_t len = ks->Length();
        int32_t i;

        for (i = 0; i < len; i++) {
            JSValue k = ks->Get(context, i);

            if (!k->IsNumber()) {
                JSValue v = m->Get(context, k);

                if (v->IsFunction())
                    (this->*fn)(ToString(isolate, k), v8::Local<v8::Function>::Cast(v), retVal);
                else
                    return CHECK_ERROR(CALL_E_BADVARTYPE);
            }
        }

        retVal = o;
        return 0;
    }

    result_t on(exlib::string ev, v8::Local<v8::Function> func, v8::Local<v8::Object>& retVal)
    {
        putFunction(GetHiddenList(ev, true), func, ev);
        retVal = o;
        return 0;
    }

    result_t on(v8::Local<v8::Object> map, v8::Local<v8::Object>& retVal)
    {
        return _map(map, &JSTrigger::on, retVal);
    }

    result_t prependListener(exlib::string ev, v8::Local<v8::Function> func, v8::Local<v8::Object>& retVal)
    {
        prependPutFunction(GetHiddenList(ev, true), func, ev);
        retVal = o;
        return 0;
    }

    result_t prependListener(v8::Local<v8::Object> map, v8::Local<v8::Object>& retVal)
    {
        return _map(map, &JSTrigger::prependListener, retVal);
    }

    static void _onceWrap(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        Isolate* isolate = Isolate::current();
        v8::Local<v8::Object> _data = v8::Local<v8::Object>::Cast(args.Data());
        v8::Local<v8::Context> context = isolate->context();

        v8::Local<v8::Function> func = v8::Local<v8::Function>::Cast(JSValue(_data->Get(context, isolate->NewString("_func"))));
        JSValue v = _data->Get(context, isolate->NewString("_ev"));
        v8::Local<v8::Function> _wrap = v8::Local<v8::Function>::Cast(JSValue(_data->Get(context, isolate->NewString("_wrap"))));

        exlib::string ev;
        GetArgumentValue(v, ev, true);

        std::vector<v8::Local<v8::Value>> _args;
        int32_t len = args.Length();
        _args.resize(len);

        for (int32_t i = 0; i < len; i++)
            _args[i] = args[i];

        v8::Local<v8::Object> vr;
        JSTrigger t(args);
        t.off(ev, _wrap, vr);

        if (!func.IsEmpty() && !_wrap.IsEmpty())
            func->Call(context, args.This(), (int32_t)_args.size(), _args.data()).IsEmpty();
    }

    result_t once(exlib::string ev, v8::Local<v8::Function> func, v8::Local<v8::Object>& retVal)
    {
        Isolate* _isolate = Isolate::current();
        v8::Local<v8::Object> _data = v8::Object::New(isolate);
        _data->Set(context, NewString("_func"), func).Check();
        _data->Set(context, NewString("_ev"), NewString(ev)).Check();

        v8::Local<v8::Function> wrap = _isolate->NewFunction("_onceWrap", _onceWrap, _data);
        if (wrap.IsEmpty())
            return CHECK_ERROR(Runtime::setError("function alloc error."));

        wrap->Set(context, NewString("_func"), func).Check();
        _data->Set(context, NewString("_wrap"), wrap).Check();

        putFunction(GetHiddenList(ev, true), wrap, ev);

        retVal = o;
        return 0;
    }

    result_t once(v8::Local<v8::Object> map, v8::Local<v8::Object>& retVal)
    {
        return _map(map, &JSTrigger::once, retVal);
    }

    result_t prependOnceListener(exlib::string ev, v8::Local<v8::Function> func, v8::Local<v8::Object>& retVal)
    {
        Isolate* _isolate = Isolate::current();
        v8::Local<v8::Object> _data = v8::Object::New(isolate);
        _data->Set(context, NewString("_func"), func).Check();
        _data->Set(context, NewString("_ev"), NewString(ev)).Check();

        v8::Local<v8::Function> wrap = _isolate->NewFunction("_onceWrap", _onceWrap, _data);
        if (wrap.IsEmpty())
            return CHECK_ERROR(Runtime::setError("function alloc error."));

        _data->Set(context, NewString("_wrap"), wrap).Check();

        prependPutFunction(GetHiddenList(ev, true), wrap, ev);

        retVal = o;
        return 0;
    }

    result_t prependOnceListener(v8::Local<v8::Object> map, v8::Local<v8::Object>& retVal)
    {
        return _map(map, &JSTrigger::prependOnceListener, retVal);
    }

    result_t off(exlib::string ev, v8::Local<v8::Function> func, v8::Local<v8::Object>& retVal)
    {
        v8::Local<v8::Array> esa = GetHiddenList(ev);

        if (!esa.IsEmpty()) {
            removeFunction(esa, func, ev);

            int32_t len = esa->Length();

            if (len == 0)
                DeletePrivate(ev);
        }

        retVal = o;
        return 0;
    }

    result_t off(exlib::string ev, v8::Local<v8::Object>& retVal)
    {
        v8::Local<v8::Array> esa = GetHiddenList(ev);

        if (!esa.IsEmpty())
            while (true) {
                int32_t len = esa->Length();

                if (len == 0) {
                    DeletePrivate(ev);
                    break;
                }

                JSValue v = esa->Get(context, len - 1);
                v8::Local<v8::Function> func = v8::Local<v8::Function>::Cast(v);

                removeFunction(esa, func, ev);
            }

        retVal = o;
        return 0;
    }

    result_t off(v8::Local<v8::Object> map, v8::Local<v8::Object>& retVal)
    {
        return _map(map, &JSTrigger::off, retVal);
    }

    result_t removeAllListeners(exlib::string ev, v8::Local<v8::Object>& retVal)
    {
        return off(ev, retVal);
    }

    result_t removeAllListeners(v8::Local<v8::Array> evs, v8::Local<v8::Object>& retVal)
    {
        int32_t len = evs->Length();
        int32_t i;
        result_t hr;

        if (len == 0) {
            evs = events->GetPropertyNames(context).FromMaybe(v8::Local<v8::Array>());
            len = evs->Length();
        }

        for (i = 0; i < len; i++) {
            JSValue v = evs->Get(context, i);
            exlib::string key;

            hr = GetArgumentValue(v, key, true);
            if (hr < 0)
                return hr;

            hr = off(key, retVal);
            if (hr < 0)
                return hr;
        }

        retVal = o;
        return 0;
    }

    result_t setMaxListeners(int32_t n)
    {
        if (n < 0)
            return Runtime::setError("\"defaultMaxListeners\" must be a positive number");

        o->SetPrivate(context, v8::Private::ForApi(isolate, NewString("_maxListeners")), v8::Integer::New(isolate, n));
        return 0;
    }

    result_t getMaxListeners(int32_t& retVal)
    {
        Isolate* _isolate = Isolate::current();
        JSValue maxListeners = o->GetPrivate(context, v8::Private::ForApi(isolate, NewString("_maxListeners")));
        if (maxListeners->IsUndefined() || maxListeners->IsNull()) {
            retVal = _isolate->m_defaultMaxListeners;
        } else {
            GetArgumentValue(maxListeners, retVal, true);
        }
        return 0;
    }

    static result_t set_defaultMaxListeners(int32_t newVal)
    {
        if (newVal < 0)
            return Runtime::setError("\"defaultMaxListeners\" must be a positive number");

        Isolate* isolate = Isolate::current();
        isolate->m_defaultMaxListeners = newVal;
        return 0;
    }

    static result_t get_defaultMaxListeners(int32_t& retVal)
    {
        Isolate* isolate = Isolate::current();
        retVal = isolate->m_defaultMaxListeners;
        return 0;
    }

    result_t listeners(exlib::string ev, v8::Local<v8::Array>& retVal)
    {
        int32_t n = 0;

        retVal = v8::Array::New(isolate);

        v8::Local<v8::Array> esa = GetHiddenList(ev);
        if (!esa.IsEmpty()) {
            int32_t len = esa->Length();
            int32_t i;

            for (i = 0; i < len; i++)
                retVal->Set(context, n++, JSValue(esa->Get(context, i))).Check();
        }

        return 0;
    }

    result_t listenerCount(exlib::string ev, int32_t& retVal)
    {
        int32_t n = 0;

        v8::Local<v8::Array> esa = GetHiddenList(ev);
        if (!esa.IsEmpty()) {
            n = esa->Length();
        }

        retVal = n;
        return 0;
    }

    result_t fireTrigger(v8::Local<v8::Array> esa, v8::Local<v8::Value>* args, int32_t argCount,
        QuickArray<obj_ptr<Fiber_base>>& evs, v8::Local<v8::Function>& ff)
    {
        if (esa.IsEmpty())
            return 0;

        int32_t len = esa->Length();
        int32_t i;
        result_t hr;

        for (i = 0; i < len; i++) {
            JSValue func = esa->Get(context, i);
            if (func->IsFunction()) {
                if (ff.IsEmpty())
                    ff = v8::Local<v8::Function>::Cast(func);
                else {
                    obj_ptr<Fiber_base> f;
                    hr = JSFiber::New(o, v8::Local<v8::Function>::Cast(func), args, argCount, f);
                    if (hr < 0)
                        return hr;
                    evs.append(f);
                }
            }
        }

        return 0;
    }

    result_t _emit(exlib::string ev, v8::Local<v8::Value>* args,
        int32_t argCount, bool& retVal)
    {
        result_t hr;
        retVal = false;
        QuickArray<obj_ptr<Fiber_base>> evs;
        v8::Local<v8::Function> ff;
        exlib::string msg;

        hr = fireTrigger(GetHiddenList(ev), args, argCount, evs, ff);
        if (hr < 0)
            return hr;

        if (!ff.IsEmpty()) {
            JSValue r = ff->Call(context, o, argCount, args);
            retVal = true;
            if (r.IsEmpty())
                hr = CALL_E_JAVASCRIPT;
        }

        if (evs.size() > 0) {
            int32_t i;
            METHOD_NAME("EventEmitter.emit");
            for (i = 0; i < (int32_t)evs.size(); i++) {
                evs[i]->join();
                msg = ((JSFiber*)(Fiber_base*)evs[i])->m_message;
            }

            retVal = true;
        }

        if (hr < 0)
            return hr;

        return !msg.empty() ? CHECK_ERROR(Runtime::setError(msg)) : 0;
    }

    result_t emit(exlib::string ev, OptArgs args, bool& retVal)
    {
        std::vector<v8::Local<v8::Value>> datas;
        args.GetData(datas);

        return _emit(ev, datas.data(), args.Length(), retVal);
    }

    result_t eventNames(v8::Local<v8::Array>& retVal)
    {
        retVal = JSArray(events->GetOwnPropertyNames(context));
        return 0;
    }

public:
    static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        if (args.IsConstructCall()) {
            if (EventEmitter_base::class_info().init_isolate())
                return;
            EventEmitter_base::__new(args);
        } else {
            v8::Local<v8::Object> o = args.This();

            if (!o.IsEmpty())
                EventEmitter_base::class_info().Attach(Isolate::current(), o);
        }
    }

    static void s_on(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        v8::Local<v8::Object> vr;

        METHOD_NAME("EventEmitter.on");
        METHOD_ENTER();

        METHOD_OVER(2, 2);

        ARG(exlib::string, 0);
        ARG(v8::Local<v8::Function>, 1);

        hr = JSTrigger(args).on(v0, v1, vr);

        METHOD_OVER(1, 1);

        ARG(v8::Local<v8::Object>, 0);

        hr = JSTrigger(args).on(v0, vr);

        METHOD_RETURN();
    }

    static void s_prependListener(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        v8::Local<v8::Object> vr;

        METHOD_NAME("EventEmitter.prependListener");
        METHOD_ENTER();

        METHOD_OVER(2, 2);

        ARG(exlib::string, 0);
        ARG(v8::Local<v8::Function>, 1);

        hr = JSTrigger(args).prependListener(v0, v1, vr);

        METHOD_OVER(1, 1);

        ARG(v8::Local<v8::Object>, 0);

        hr = JSTrigger(args).prependListener(v0, vr);

        METHOD_RETURN();
    }

    static void s_once(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        v8::Local<v8::Object> vr;

        METHOD_NAME("EventEmitter.once");
        METHOD_ENTER();

        METHOD_OVER(2, 2);

        ARG(exlib::string, 0);
        ARG(v8::Local<v8::Function>, 1);

        hr = JSTrigger(args).once(v0, v1, vr);

        METHOD_OVER(1, 1);

        ARG(v8::Local<v8::Object>, 0);

        hr = JSTrigger(args).once(v0, vr);

        METHOD_RETURN();
    }

    static void s_prependOnceListener(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        v8::Local<v8::Object> vr;

        METHOD_NAME("EventEmitter.prependOnceListener");
        METHOD_ENTER();

        METHOD_OVER(2, 2);

        ARG(exlib::string, 0);
        ARG(v8::Local<v8::Function>, 1);

        hr = JSTrigger(args).prependOnceListener(v0, v1, vr);

        METHOD_OVER(1, 1);

        ARG(v8::Local<v8::Object>, 0);

        hr = JSTrigger(args).prependOnceListener(v0, vr);

        METHOD_RETURN();
    }

    static void s_off(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        v8::Local<v8::Object> vr;

        METHOD_NAME("EventEmitter.off");
        METHOD_ENTER();

        METHOD_OVER(2, 2);

        ARG(exlib::string, 0);
        ARG(v8::Local<v8::Function>, 1);

        hr = JSTrigger(args).off(v0, v1, vr);

        METHOD_OVER(1, 1);

        ARG(exlib::string, 0);

        hr = JSTrigger(args).off(v0, vr);

        METHOD_OVER(1, 1);

        ARG(v8::Local<v8::Object>, 0);

        hr = JSTrigger(args).off(v0, vr);

        METHOD_RETURN();
    }

    static void s_removeAllListeners(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        v8::Local<v8::Object> vr;

        METHOD_NAME("EventEmitter.removeAllListeners");
        METHOD_ENTER();

        METHOD_OVER(1, 1);

        ARG(exlib::string, 0);

        hr = JSTrigger(args).removeAllListeners(v0, vr);

        METHOD_OVER(1, 0);

        OPT_ARG(v8::Local<v8::Array>, 0, v8::Array::New(isolate));

        hr = JSTrigger(args).removeAllListeners(v0, vr);

        METHOD_RETURN();
    }

    static void s_setMaxListeners(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        METHOD_NAME("EventEmitter.setMaxListeners");
        METHOD_ENTER();

        METHOD_OVER(1, 1);

        ARG(int32_t, 0);

        JSTrigger(args).setMaxListeners(v0);

        METHOD_VOID();
    }

    static void s_getMaxListeners(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        int32_t vr;

        METHOD_NAME("EventEmitter.getMaxListeners");
        METHOD_ENTER();

        METHOD_OVER(0, 0);

        hr = JSTrigger(args).getMaxListeners(vr);

        METHOD_RETURN();
    }

    static void s_get_defaultMaxListeners(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args)
    {
        int32_t vr;

        METHOD_NAME("EventEmitter.defaultMaxListeners");
        PROPERTY_ENTER();

        hr = get_defaultMaxListeners(vr);

        METHOD_RETURN();
    }

    static void s_set_defaultMaxListeners(v8::Local<v8::Name> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args)
    {
        METHOD_NAME("EventEmitter.defaultMaxListeners");
        PROPERTY_ENTER();
        PROPERTY_VAL(int32_t);

        hr = set_defaultMaxListeners(v0);

        PROPERTY_SET_LEAVE();
    }

    static void s_listeners(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        v8::Local<v8::Array> vr;

        METHOD_NAME("EventEmitter.listeners");
        METHOD_ENTER();

        METHOD_OVER(1, 1);

        ARG(exlib::string, 0);

        hr = JSTrigger(args).listeners(v0, vr);

        METHOD_RETURN();
    }

    static void s_listenerCount(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        int32_t vr;

        METHOD_NAME("EventEmitter.listenerCount");
        METHOD_ENTER();

        METHOD_OVER(1, 1);

        ARG(exlib::string, 0);

        hr = JSTrigger(args).listenerCount(v0, vr);

        METHOD_OVER(2, 2);

        ARG(v8::Local<v8::Value>, 0);
        ARG(exlib::string, 1);

        v8::Isolate* isolate = args.GetIsolate();
        v8::Local<v8::Object> o = v0->ToObject(isolate->GetCurrentContext()).FromMaybe(v8::Local<v8::Object>());
        if (o.IsEmpty()) {
            hr = CALL_E_TYPEMISMATCH;
            break;
        }

        hr = JSTrigger(isolate, o).listenerCount(v1, vr);

        METHOD_RETURN();
    }

    static void s_emit(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        bool vr;

        METHOD_NAME("EventEmitter.emit");
        METHOD_ENTER();

        METHOD_OVER(-1, 1);

        ARG(exlib::string, 0);
        ARG_LIST(1);

        hr = JSTrigger(args).emit(v0, v1, vr);

        METHOD_RETURN();
    }

    static void s_eventNames(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        v8::Local<v8::Array> vr;

        METHOD_NAME("EventEmitter.eventNames");
        METHOD_ENTER();

        METHOD_OVER(0, 0);

        hr = JSTrigger(args).eventNames(vr);

        METHOD_RETURN();
    }

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
