/*
 * Event.cpp
 *
 *  Created on: Apr 8, 2012
 *      Author: lion
 */

#include "object.h"
#include "Fiber.h"
#include "EventEmitter.h"
#include "ifs/coroutine.h"
#include "QuickArray.h"
#include "../util/util.h"
#include <vector>

namespace fibjs {
result_t EventEmitter_base::_new(obj_ptr<EventEmitter_base>& retVal, v8::Local<v8::Object> This)
{
    retVal = new EventEmitter();
    return 0;
}

result_t EventEmitter_base::get_defaultMaxListeners(int32_t& retVal)
{
    return 0;
}

result_t EventEmitter_base::set_defaultMaxListeners(int32_t newVal)
{
    return 0;
}

result_t object_base::on(exlib::string ev, v8::Local<v8::Function> func, v8::Local<v8::Object>& retVal)
{
    return JSTrigger(this).on(ev, func, retVal);
}

result_t object_base::on(v8::Local<v8::Object> map, v8::Local<v8::Object>& retVal)
{
    return JSTrigger(this).on(map, retVal);
}

result_t object_base::addEventListener(exlib::string ev, v8::Local<v8::Function> func, v8::Local<v8::Object> options, v8::Local<v8::Object>& retVal)
{
    return JSTrigger(this).addEventListener(ev, func, options, retVal);
}

result_t object_base::prependListener(exlib::string ev, v8::Local<v8::Function> func, v8::Local<v8::Object>& retVal)
{
    return JSTrigger(this).prependListener(ev, func, retVal);
}

result_t object_base::prependListener(v8::Local<v8::Object> map, v8::Local<v8::Object>& retVal)
{
    return JSTrigger(this).prependListener(map, retVal);
}

result_t object_base::once(exlib::string ev, v8::Local<v8::Function> func, v8::Local<v8::Object>& retVal)
{
    return JSTrigger(this).once(ev, func, retVal);
}

result_t object_base::once(v8::Local<v8::Object> map, v8::Local<v8::Object>& retVal)
{
    return JSTrigger(this).once(map, retVal);
}

result_t object_base::prependOnceListener(exlib::string ev, v8::Local<v8::Function> func, v8::Local<v8::Object>& retVal)
{
    return JSTrigger(this).prependOnceListener(ev, func, retVal);
}

result_t object_base::prependOnceListener(v8::Local<v8::Object> map, v8::Local<v8::Object>& retVal)
{
    return JSTrigger(this).prependOnceListener(map, retVal);
}

result_t object_base::off(exlib::string ev, v8::Local<v8::Function> func, v8::Local<v8::Object>& retVal)
{
    return JSTrigger(this).off(ev, func, retVal);
}

result_t object_base::off(exlib::string ev, v8::Local<v8::Object>& retVal)
{
    return JSTrigger(this).off(ev, retVal);
}

result_t object_base::off(v8::Local<v8::Object> map, v8::Local<v8::Object>& retVal)
{
    return JSTrigger(this).off(map, retVal);
}

result_t object_base::removeEventListener(exlib::string ev, v8::Local<v8::Function> func,
    v8::Local<v8::Object> options, v8::Local<v8::Object>& retVal)
{
    return JSTrigger(this).removeEventListener(ev, func, options, retVal);
}

result_t object_base::removeAllListeners(exlib::string ev, v8::Local<v8::Object>& retVal)
{
    return JSTrigger(this).removeAllListeners(ev, retVal);
}

result_t object_base::removeAllListeners(v8::Local<v8::Array> evs, v8::Local<v8::Object>& retVal)
{
    return JSTrigger(this).removeAllListeners(evs, retVal);
}

result_t object_base::setMaxListeners(int32_t n)
{
    return JSTrigger(this).setMaxListeners(n);
}

result_t object_base::getMaxListeners(int32_t& retVal)
{
    return JSTrigger(this).getMaxListeners(retVal);
}

result_t object_base::setListener(exlib::string ev, v8::Local<v8::Function> func)
{
    result_t hr;
    JSTrigger e(this);
    v8::Local<v8::Object> r;

    hr = e.off(ev, r);
    if (hr < 0)
        return hr;

    return e.on(ev, func, r);
}

result_t object_base::getListener(exlib::string ev, v8::Local<v8::Function>& func)
{
    result_t hr;
    JSTrigger e(this);
    v8::Local<v8::Array> r;

    hr = listeners(ev, r);
    if (hr < 0)
        return hr;

    if (r->Length() == 0)
        return CALL_RETURN_NULL;

    func = JSValue(r->Get(e.context, 0)).As<v8::Function>();
    return 0;
}

result_t object_base::listeners(exlib::string ev, v8::Local<v8::Array>& retVal)
{
    return JSTrigger(this).listeners(ev, retVal);
}

result_t object_base::rawListeners(exlib::string ev, v8::Local<v8::Array>& retVal)
{
    return JSTrigger(this).rawListeners(ev, retVal);
}

result_t object_base::_emit(exlib::string ev, v8::Local<v8::Value>* args, int32_t argCount, bool& retVal)
{
    return JSTrigger(this)._emit(ev, args, argCount, retVal);
}

result_t object_base::_emit(exlib::string ev, Variant arg)
{
    return _emit(ev, &arg, 1);
}

result_t object_base::_emit(exlib::string ev, Variant* args, int32_t argCount)
{
    Isolate* isolate = get_holder();
    if (isolate)
        (new JSTrigger::AsyncEmitter(isolate, this))->emit(ev, args, argCount);

    return 0;
}

result_t object_base::emit(exlib::string ev, OptArgs args, bool& retVal)
{
    onEventEmit(ev);
    return JSTrigger(this).emit(ev, args, retVal);
}

result_t object_base::listenerCount(exlib::string ev, int32_t& retVal)
{
    return JSTrigger(this).listenerCount(ev, retVal);
}

result_t object_base::listenerCount(v8::Local<v8::Value> o, exlib::string ev, int32_t& retVal)
{
    Isolate* isolate = holder();
    v8::Local<v8::Object> o1 = o->ToObject(isolate->context()).FromMaybe(v8::Local<v8::Object>());
    if (o1.IsEmpty())
        return CALL_E_TYPEMISMATCH;

    return JSTrigger(isolate->m_isolate, o1).listenerCount(ev, retVal);
}

result_t object_base::eventNames(v8::Local<v8::Array>& retVal)
{
    return JSTrigger(this).eventNames(retVal);
}

// Static stubs required by IDL-generated code; actual implementations are in JSTrigger
// and registered via events.cpp override.
result_t EventEmitter_base::on(EventEmitter_base* emitter, exlib::string ev, v8::Local<v8::Object> options, v8::Local<v8::Object>& retVal)
{
    return CALL_E_INVALID_CALL;
}

result_t EventEmitter_base::once(EventEmitter_base* emitter, exlib::string ev, v8::Local<v8::Object> options, v8::Local<v8::Object>& retVal)
{
    return CALL_E_INVALID_CALL;
}

result_t EventEmitter_base::addAbortListener(EventEmitter_base* signal, v8::Local<v8::Function> func, v8::Local<v8::Object>& retVal)
{
    return CALL_E_INVALID_CALL;
}

void JSTrigger::initEv()
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

v8::Local<v8::Array> JSTrigger::GetHiddenList(exlib::string k, bool create)
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

result_t JSTrigger::onEventChange(exlib::string type, exlib::string ev, v8::Local<v8::Function> func)
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

int32_t JSTrigger::putFunction(v8::Local<v8::Array> esa, v8::Local<v8::Function> func, exlib::string ev)
{
    result_t hr;
    hr = onEventChange("newListener", ev, func);
    if (hr < 0)
        return hr;

    int32_t len = esa->Length();
    esa->Set(context, len, func).IsJust();
    return 0;
}

int32_t JSTrigger::prependPutFunction(v8::Local<v8::Array> esa, v8::Local<v8::Function> func, exlib::string ev)
{
    result_t hr;
    hr = onEventChange("newListener", ev, func);
    if (hr < 0)
        return hr;

    int32_t len = esa->Length();
    int32_t i;

    for (i = len; i > 0; i--)
        esa->Set(context, i, esa->Get(context, i - 1).FromMaybe(v8::Local<v8::Value>())).IsJust();

    esa->Set(context, 0, func).IsJust();
    return 0;
}

void JSTrigger::spliceOne(v8::Local<v8::Array> esa, int32_t index)
{
    int32_t i;
    int32_t len = esa->Length();
    for (i = index; i < len - 1; i++)
        esa->Set(context, i, esa->Get(context, i + 1).FromMaybe(v8::Local<v8::Value>())).IsJust();
    esa->Delete(context, len - 1).IsJust();
    esa->Set(context, NewString("length"),
           v8::Integer::New(isolate, len - 1))
        .IsJust();
}

int32_t JSTrigger::removeFunction(v8::Local<v8::Array> esa, v8::Local<v8::Function> func, exlib::string ev)
{
    if (esa.IsEmpty())
        return 0;

    int32_t len = esa->Length();
    int32_t i;

    for (i = len - 1; i >= 0; i--) {
        JSValue v = esa->Get(context, i);
        bool match = v->Equals(isolate->GetCurrentContext(), func).FromMaybe(false);

        // Also check once-wrapped functions via _func property
        if (!match && v->IsFunction()) {
            v8::Local<v8::Function> f = v8::Local<v8::Function>::Cast(v);
            v8::Local<v8::Value> origFunc = JSValue(f->Get(context, NewString("_func")));
            if (!origFunc.IsEmpty() && origFunc->IsFunction())
                match = origFunc->Equals(isolate->GetCurrentContext(), func).FromMaybe(false);
        }

        if (match) {
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

result_t JSTrigger::_map(v8::Local<v8::Object> m,
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

void JSTrigger::_onceWrap(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    Isolate* isolate = Isolate::current(args);
    v8::Local<v8::Object> _data = v8::Local<v8::Object>::Cast(args.Data());
    v8::Local<v8::Context> context = isolate->context();

    v8::Local<v8::Function> func = v8::Local<v8::Function>::Cast(JSValue(_data->Get(context, isolate->NewString("_func"))));
    JSValue v = _data->Get(context, isolate->NewString("_ev"));
    v8::Local<v8::Function> _wrap = v8::Local<v8::Function>::Cast(JSValue(_data->Get(context, isolate->NewString("_wrap"))));

    exlib::string ev;
    GetArgumentValue(isolate, v, ev, true);

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

result_t JSTrigger::once(exlib::string ev, v8::Local<v8::Function> func, v8::Local<v8::Object>& retVal)
{
    Isolate* _isolate = Isolate::current(func);
    v8::Local<v8::Object> _data = v8::Object::New(isolate);
    _data->Set(context, NewString("_func"), func).IsJust();
    _data->Set(context, NewString("_ev"), NewString(ev)).IsJust();

    v8::Local<v8::Function> wrap = _isolate->NewFunction("_onceWrap", _onceWrap, _data);
    if (wrap.IsEmpty())
        return CHECK_ERROR(Runtime::setError("function alloc error."));

    wrap->Set(context, NewString("_func"), func).IsJust();
    _data->Set(context, NewString("_wrap"), wrap).IsJust();

    putFunction(GetHiddenList(ev, true), wrap, ev);

    retVal = o;
    return 0;
}

result_t JSTrigger::addEventListener(exlib::string ev, v8::Local<v8::Function> func, v8::Local<v8::Object> options, v8::Local<v8::Object>& retVal)
{
    bool _once = false;
    result_t hr = GetConfigValue(options, "once", _once, true);
    if (hr < 0 && hr != CALL_E_PARAMNOTOPTIONAL)
        return hr;

    return _once ? once(ev, func, retVal) : on(ev, func, retVal);
}

result_t JSTrigger::prependOnceListener(exlib::string ev, v8::Local<v8::Function> func, v8::Local<v8::Object>& retVal)
{
    Isolate* _isolate = Isolate::current(func);
    v8::Local<v8::Object> _data = v8::Object::New(isolate);
    _data->Set(context, NewString("_func"), func).IsJust();
    _data->Set(context, NewString("_ev"), NewString(ev)).IsJust();

    v8::Local<v8::Function> wrap = _isolate->NewFunction("_onceWrap", _onceWrap, _data);
    if (wrap.IsEmpty())
        return CHECK_ERROR(Runtime::setError("function alloc error."));

    _data->Set(context, NewString("_wrap"), wrap).IsJust();

    prependPutFunction(GetHiddenList(ev, true), wrap, ev);

    retVal = o;
    return 0;
}

result_t JSTrigger::off(exlib::string ev, v8::Local<v8::Function> func, v8::Local<v8::Object>& retVal)
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

result_t JSTrigger::off(exlib::string ev, v8::Local<v8::Object>& retVal)
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

result_t JSTrigger::removeAllListeners(v8::Local<v8::Array> evs, v8::Local<v8::Object>& retVal)
{
    int32_t len = evs->Length();
    int32_t i;
    result_t hr;

    if (len == 0) {
        evs = events->GetPropertyNames(context).FromMaybe(v8::Local<v8::Array>());
        len = evs->Length();
    }

    Isolate* _isolate = Isolate::current(isolate);

    for (i = 0; i < len; i++) {
        JSValue v = evs->Get(context, i);
        exlib::string key;

        hr = GetArgumentValue(_isolate, v, key, true);
        if (hr < 0)
            return hr;

        hr = off(key, retVal);
        if (hr < 0)
            return hr;
    }

    retVal = o;
    return 0;
}

result_t JSTrigger::setMaxListeners(int32_t n)
{
    if (n < 0)
        return Runtime::setError("\"defaultMaxListeners\" must be a positive number");

    o->SetPrivate(context, v8::Private::ForApi(isolate, NewString("_maxListeners")), v8::Integer::New(isolate, n));
    return 0;
}

result_t JSTrigger::getMaxListeners(int32_t& retVal)
{
    Isolate* _isolate = Isolate::current(isolate);
    JSValue maxListeners = o->GetPrivate(context, v8::Private::ForApi(isolate, NewString("_maxListeners")));
    if (maxListeners->IsUndefined() || maxListeners->IsNull()) {
        retVal = _isolate->m_defaultMaxListeners;
    } else {
        GetArgumentValue(_isolate, maxListeners, retVal, true);
    }
    return 0;
}

result_t JSTrigger::set_defaultMaxListeners(int32_t newVal)
{
    if (newVal < 0)
        return Runtime::setError("\"defaultMaxListeners\" must be a positive number");

    Isolate* isolate = Isolate::current();
    isolate->m_defaultMaxListeners = newVal;
    return 0;
}

result_t JSTrigger::get_defaultMaxListeners(int32_t& retVal)
{
    Isolate* isolate = Isolate::current();
    retVal = isolate->m_defaultMaxListeners;
    return 0;
}

result_t JSTrigger::listeners(exlib::string ev, v8::Local<v8::Array>& retVal)
{
    int32_t n = 0;

    retVal = v8::Array::New(isolate);

    v8::Local<v8::Array> esa = GetHiddenList(ev);
    if (!esa.IsEmpty()) {
        int32_t len = esa->Length();
        int32_t i;

        for (i = 0; i < len; i++) {
            v8::Local<v8::Value> v = JSValue(esa->Get(context, i));
            if (v->IsFunction()) {
                v8::Local<v8::Function> func = v8::Local<v8::Function>::Cast(v);
                v8::Local<v8::Value> origFunc = JSValue(func->Get(context, NewString("_func")));
                if (!origFunc.IsEmpty() && origFunc->IsFunction())
                    retVal->Set(context, n++, origFunc).IsJust();
                else
                    retVal->Set(context, n++, v).IsJust();
            } else {
                retVal->Set(context, n++, v).IsJust();
            }
        }
    }

    return 0;
}

result_t JSTrigger::rawListeners(exlib::string ev, v8::Local<v8::Array>& retVal)
{
    int32_t n = 0;

    retVal = v8::Array::New(isolate);

    v8::Local<v8::Array> esa = GetHiddenList(ev);
    if (!esa.IsEmpty()) {
        int32_t len = esa->Length();
        int32_t i;

        for (i = 0; i < len; i++)
            retVal->Set(context, n++, JSValue(esa->Get(context, i))).IsJust();
    }

    return 0;
}

result_t JSTrigger::listenerCount(exlib::string ev, int32_t& retVal)
{
    int32_t n = 0;

    v8::Local<v8::Array> esa = GetHiddenList(ev);
    if (!esa.IsEmpty()) {
        n = esa->Length();
    }

    retVal = n;
    return 0;
}

result_t JSTrigger::fireTrigger(v8::Local<v8::Array> esa, v8::Local<v8::Value>* args, int32_t argCount,
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

result_t JSTrigger::_emit(exlib::string ev, v8::Local<v8::Value>* args,
    int32_t argCount, bool& retVal)
{
    result_t hr;
    retVal = false;
    QuickArray<obj_ptr<Fiber_base>> evs;
    v8::Local<v8::Function> ff;
    exlib::string msg;

    obj_ptr<object_base> _obj = object_base::getInstance(o);
    if (_obj) {
        _obj->onEventEmit(ev);
    }

    hr = fireTrigger(GetHiddenList(ev), args, argCount, evs, ff);
    if (hr < 0)
        return hr;

    if (ev == "error" && ff.IsEmpty()) {
        if (argCount > 0 && !args[0].IsEmpty() && args[0]->IsNativeError()) {
            isolate->ThrowException(args[0]);
        } else {
            exlib::string errMsg = "Unhandled error.";
            if (argCount > 0 && !args[0].IsEmpty()) {
                errMsg += " (";
                // use json_format for objects to show their content
                if (args[0]->IsObject() && !args[0]->IsFunction()) {
                    errMsg += json_format(Isolate::current(isolate), args[0]);
                } else {
                    errMsg += ToString(isolate, args[0]);
                }
                errMsg += ")";
            }
            isolate->ThrowException(v8::Exception::Error(NewString(errMsg)));
        }
        return CALL_E_JAVASCRIPT;
    }

    if (!ff.IsEmpty()) {
        JSValue r = ff->Call(context, o, argCount, args);
        retVal = true;
        if (r.IsEmpty())
            hr = CALL_E_JAVASCRIPT;
    }

    if (evs.size() > 0) {
        int32_t i;
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

result_t JSTrigger::emit(exlib::string ev, OptArgs args, bool& retVal)
{
    std::vector<v8::Local<v8::Value>> datas;
    args.GetData(datas);

    return _emit(ev, datas.data(), args.Length(), retVal);
}

void JSTrigger::s__new(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    if (args.IsConstructCall()) {
        if (EventEmitter_base::class_info().init_isolate(Isolate::current(args)))
            return;
        EventEmitter_base::__new(args);
    } else {
        v8::Local<v8::Object> o = args.This();

        if (!o.IsEmpty())
            EventEmitter_base::class_info().Attach(Isolate::current(args), o);
    }
}

void JSTrigger::s_on(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    // static events.on(emitter, event[, options]) => AsyncIterator
    if (args.Length() >= 2
        && args[0]->IsObject() && !args[0]->IsFunction()
        && args[1]->IsString()) {
        s_on_static(args);
        return;
    }

    v8::Local<v8::Object> vr;

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

void JSTrigger::s_addEventListener(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Object> vr;

    METHOD_ENTER();

    METHOD_OVER(3, 2);

    ARG(exlib::string, 0);
    ARG(v8::Local<v8::Function>, 1);
    OPT_ARG(v8::Local<v8::Object>, 2, v8::Object::New(isolate->m_isolate));

    hr = JSTrigger(args).addEventListener(v0, v1, v2, vr);

    METHOD_RETURN();
}

void JSTrigger::s_prependListener(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Object> vr;

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

void JSTrigger::s_once(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    // static events.once(emitter, event[, options]) => Promise
    if (args.Length() >= 2
        && args[0]->IsObject() && !args[0]->IsFunction()
        && args[1]->IsString()) {
        s_once_static(args);
        return;
    }

    v8::Local<v8::Object> vr;

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

void JSTrigger::s_prependOnceListener(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Object> vr;

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

void JSTrigger::s_off(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Object> vr;

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

void JSTrigger::s_removeEventListener(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Object> vr;

    METHOD_ENTER();

    METHOD_OVER(3, 2);

    ARG(exlib::string, 0);
    ARG(v8::Local<v8::Function>, 1);
    OPT_ARG(v8::Local<v8::Object>, 2, v8::Object::New(isolate->m_isolate));

    hr = JSTrigger(args).removeEventListener(v0, v1, v2, vr);

    METHOD_RETURN();
}

void JSTrigger::s_removeAllListeners(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Object> vr;

    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(exlib::string, 0);

    hr = JSTrigger(args).removeAllListeners(v0, vr);

    METHOD_OVER(1, 0);

    OPT_ARG(v8::Local<v8::Array>, 0, v8::Array::New(isolate->m_isolate));

    hr = JSTrigger(args).removeAllListeners(v0, vr);

    METHOD_RETURN();
}

void JSTrigger::s_setMaxListeners(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(int32_t, 0);

    JSTrigger(args).setMaxListeners(v0);

    METHOD_VOID();
}

void JSTrigger::s_getMaxListeners(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    int32_t vr;

    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = JSTrigger(args).getMaxListeners(vr);

    METHOD_RETURN();
}

void JSTrigger::s_get_defaultMaxListeners(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    int32_t vr;

    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = get_defaultMaxListeners(vr);

    METHOD_RETURN();
}

void JSTrigger::s_set_defaultMaxListeners(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(int32_t, 0);

    hr = set_defaultMaxListeners(v0);

    METHOD_VOID();
}

void JSTrigger::s_listeners(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Array> vr;

    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(exlib::string, 0);

    hr = JSTrigger(args).listeners(v0, vr);

    METHOD_RETURN();
}

void JSTrigger::s_rawListeners(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Array> vr;

    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(exlib::string, 0);

    hr = JSTrigger(args).rawListeners(v0, vr);

    METHOD_RETURN();
}

void JSTrigger::s_listenerCount(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    int32_t vr;

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

void JSTrigger::s_emit(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    bool vr;

    METHOD_ENTER();

    METHOD_OVER(-1, 1);

    ARG(exlib::string, 0);
    ARG_LIST(1);

    hr = JSTrigger(args).emit(v0, v1, vr);

    METHOD_RETURN();
}

void JSTrigger::s_eventNames(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Array> vr;

    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = JSTrigger(args).eventNames(vr);

    METHOD_RETURN();
}

// ---- events.once() helpers ----

void JSTrigger::_once_event_cb(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    Isolate* _isolate = Isolate::current(args);
    v8::Local<v8::Object> _data = args.Data().As<v8::Object>();
    v8::Local<v8::Context> context = _isolate->context();

    v8::Local<v8::Promise::Resolver> resolver = JSValue(_data->Get(context,
                                                            _isolate->NewString("resolver")))
                                                    .As<v8::Promise::Resolver>();

    // Remove error listener if present
    v8::Local<v8::Value> errLis = JSValue(_data->Get(context, _isolate->NewString("errorListener")));
    if (!errLis.IsEmpty() && errLis->IsFunction()) {
        v8::Local<v8::Value> emitter = JSValue(_data->Get(context, _isolate->NewString("emitter")));
        if (!emitter.IsEmpty() && emitter->IsObject()) {
            v8::Local<v8::Object> retVal;
            JSTrigger(_isolate->m_isolate, emitter.As<v8::Object>()).off("error", errLis.As<v8::Function>(), retVal);
        }
    }

    // Resolve with args array
    v8::Local<v8::Array> result = v8::Array::New(_isolate->m_isolate, args.Length());
    for (int32_t i = 0; i < args.Length(); i++)
        result->Set(context, i, args[i]).IsJust();

    resolver->Resolve(context, result).IsJust();
}

void JSTrigger::_once_error_cb(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    Isolate* _isolate = Isolate::current(args);
    v8::Local<v8::Object> _data = args.Data().As<v8::Object>();
    v8::Local<v8::Context> context = _isolate->context();

    v8::Local<v8::Promise::Resolver> resolver = JSValue(_data->Get(context,
                                                            _isolate->NewString("resolver")))
                                                    .As<v8::Promise::Resolver>();

    // Remove event listener
    v8::Local<v8::Value> evLis = JSValue(_data->Get(context, _isolate->NewString("eventListener")));
    v8::Local<v8::Value> emitter = JSValue(_data->Get(context, _isolate->NewString("emitter")));
    v8::Local<v8::Value> evName = JSValue(_data->Get(context, _isolate->NewString("event")));

    if (!evLis.IsEmpty() && evLis->IsFunction() && !emitter.IsEmpty() && emitter->IsObject()) {
        exlib::string ev;
        GetArgumentValue(_isolate, evName, ev, true);
        v8::Local<v8::Object> retVal;
        JSTrigger(_isolate->m_isolate, emitter.As<v8::Object>()).off(ev, evLis.As<v8::Function>(), retVal);
    }

    v8::Local<v8::Value> err = args.Length() > 0
        ? args[0]
        : v8::Undefined(_isolate->m_isolate).As<v8::Value>();
    resolver->Reject(context, err).IsJust();
}

v8::Local<v8::Value> JSTrigger::_make_abort_error(Isolate* _isolate)
{
    v8::Local<v8::Context> context = _isolate->context();
    v8::Local<v8::Value> err = v8::Exception::Error(_isolate->NewString("The operation was aborted"));
    err.As<v8::Object>()->Set(context, _isolate->NewString("name"), _isolate->NewString("AbortError")).IsJust();
    err.As<v8::Object>()->Set(context, _isolate->NewString("code"), _isolate->NewString("ABORT_ERR")).IsJust();
    return err;
}

void JSTrigger::_once_abort_cb(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    Isolate* _isolate = Isolate::current(args);
    v8::Local<v8::Object> _data = args.Data().As<v8::Object>();
    v8::Local<v8::Context> context = _isolate->context();

    v8::Local<v8::Promise::Resolver> resolver = JSValue(_data->Get(context,
                                                            _isolate->NewString("resolver")))
                                                    .As<v8::Promise::Resolver>();
    v8::Local<v8::Value> emitter = JSValue(_data->Get(context, _isolate->NewString("emitter")));
    v8::Local<v8::Value> evName = JSValue(_data->Get(context, _isolate->NewString("event")));

    if (!emitter.IsEmpty() && emitter->IsObject()) {
        exlib::string ev;
        GetArgumentValue(_isolate, evName, ev, true);
        JSTrigger t(_isolate->m_isolate, emitter.As<v8::Object>());
        v8::Local<v8::Object> retVal;

        v8::Local<v8::Value> evLis = JSValue(_data->Get(context, _isolate->NewString("eventListener")));
        if (!evLis.IsEmpty() && evLis->IsFunction())
            t.off(ev, evLis.As<v8::Function>(), retVal);

        v8::Local<v8::Value> errLis = JSValue(_data->Get(context, _isolate->NewString("errorListener")));
        if (!errLis.IsEmpty() && errLis->IsFunction())
            t.off("error", errLis.As<v8::Function>(), retVal);
    }

    resolver->Reject(context, _make_abort_error(_isolate)).IsJust();
}

// ---- events.on() helpers ----
// State keys on data object: emitter, event, unconsumedEvents (Array), unconsumedPromises (Array),
//   error, finished (Boolean), closeEvents (Array)

void JSTrigger::_on_event_cb(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    Isolate* _isolate = Isolate::current(args);
    v8::Local<v8::Object> _data = args.Data().As<v8::Object>();
    v8::Local<v8::Context> context = _isolate->context();

    v8::Local<v8::Array> unconsumedPromises = JSValue(_data->Get(context,
                                                          _isolate->NewString("unconsumedPromises")))
                                                  .As<v8::Array>();
    v8::Local<v8::Array> unconsumedEvents = JSValue(_data->Get(context,
                                                        _isolate->NewString("unconsumedEvents")))
                                                .As<v8::Array>();

    // Check if kFirstEventParam is set - yield first arg directly instead of args array
    v8::Local<v8::Value> firstEventParam = JSValue(_data->Get(context, _isolate->NewString("firstEventParam")));
    bool useFirstParam = !firstEventParam.IsEmpty() && firstEventParam->IsTrue();

    // Build value: either args[0] (if firstEventParam) or args array
    v8::Local<v8::Value> value;
    if (useFirstParam && args.Length() > 0) {
        value = args[0];
    } else {
        v8::Local<v8::Array> argsArr = v8::Array::New(_isolate->m_isolate, args.Length());
        for (int32_t i = 0; i < args.Length(); i++)
            argsArr->Set(context, i, args[i]).IsJust();
        value = argsArr;
    }

    uint32_t pLen = unconsumedPromises->Length();
    if (pLen > 0) {
        // Shift first promise
        v8::Local<v8::Object> p = JSValue(unconsumedPromises->Get(context, 0)).As<v8::Object>();
        for (uint32_t i = 0; i < pLen - 1; i++)
            unconsumedPromises->Set(context, i, JSValue(unconsumedPromises->Get(context, i + 1))).IsJust();
        unconsumedPromises->Delete(context, pLen - 1).IsJust();
        unconsumedPromises->Set(context, _isolate->NewString("length"),
                              v8::Integer::New(_isolate->m_isolate, pLen - 1))
            .IsJust();

        // Create { value, done: false }
        v8::Local<v8::Object> iterResult = v8::Object::New(_isolate->m_isolate);
        iterResult->Set(context, _isolate->NewString("value"), value).IsJust();
        iterResult->Set(context, _isolate->NewString("done"), v8::False(_isolate->m_isolate)).IsJust();

        v8::Local<v8::Function> resolve = JSValue(p->Get(context, _isolate->NewString("resolve"))).As<v8::Function>();
        v8::Local<v8::Value> argv[] = { iterResult };
        resolve->Call(context, v8::Undefined(_isolate->m_isolate), 1, argv).IsEmpty();
    } else {
        // Push to unconsumedEvents
        uint32_t eLen = unconsumedEvents->Length();
        unconsumedEvents->Set(context, eLen, value).IsJust();
    }
}

void JSTrigger::_on_cleanup(Isolate* _isolate, v8::Local<v8::Object> _data)
{
    v8::Local<v8::Context> context = _isolate->context();
    v8::Local<v8::Value> emitter = JSValue(_data->Get(context, _isolate->NewString("emitter")));
    if (emitter.IsEmpty() || !emitter->IsObject())
        return;

    v8::Local<v8::Object> retVal;
    JSTrigger t(_isolate->m_isolate, emitter.As<v8::Object>());

    v8::Local<v8::Value> evHandler = JSValue(_data->Get(context, _isolate->NewString("eventHandler")));
    v8::Local<v8::Value> evName = JSValue(_data->Get(context, _isolate->NewString("event")));
    exlib::string ev;
    GetArgumentValue(_isolate, evName, ev, true);

    if (!evHandler.IsEmpty() && evHandler->IsFunction())
        t.off(ev, evHandler.As<v8::Function>(), retVal);

    v8::Local<v8::Value> errHandler = JSValue(_data->Get(context, _isolate->NewString("errorHandler")));
    if (!errHandler.IsEmpty() && errHandler->IsFunction())
        t.off("error", errHandler.As<v8::Function>(), retVal);

    v8::Local<v8::Value> closeEvents = JSValue(_data->Get(context, _isolate->NewString("closeEvents")));
    if (!closeEvents.IsEmpty() && closeEvents->IsArray()) {
        v8::Local<v8::Array> closeArr = closeEvents.As<v8::Array>();
        v8::Local<v8::Value> closeHandler = JSValue(_data->Get(context, _isolate->NewString("closeHandler")));
        if (!closeHandler.IsEmpty() && closeHandler->IsFunction()) {
            for (uint32_t i = 0; i < closeArr->Length(); i++) {
                exlib::string cev;
                GetArgumentValue(_isolate, JSValue(closeArr->Get(context, i)), cev, true);
                t.off(cev, closeHandler.As<v8::Function>(), retVal);
            }
        }
    }
}

void JSTrigger::_on_error_cb(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    Isolate* _isolate = Isolate::current(args);
    v8::Local<v8::Object> _data = args.Data().As<v8::Object>();
    v8::Local<v8::Context> context = _isolate->context();

    v8::Local<v8::Value> err = args.Length() > 0
        ? args[0]
        : v8::Undefined(_isolate->m_isolate).As<v8::Value>();

    _data->Set(context, _isolate->NewString("error"), err).IsJust();

    v8::Local<v8::Array> unconsumedPromises = JSValue(_data->Get(context,
                                                          _isolate->NewString("unconsumedPromises")))
                                                  .As<v8::Array>();
    uint32_t pLen = unconsumedPromises->Length();
    if (pLen > 0) {
        v8::Local<v8::Object> p = JSValue(unconsumedPromises->Get(context, 0)).As<v8::Object>();
        for (uint32_t i = 0; i < pLen - 1; i++)
            unconsumedPromises->Set(context, i, JSValue(unconsumedPromises->Get(context, i + 1))).IsJust();
        unconsumedPromises->Delete(context, pLen - 1).IsJust();
        unconsumedPromises->Set(context, _isolate->NewString("length"),
                              v8::Integer::New(_isolate->m_isolate, pLen - 1))
            .IsJust();

        v8::Local<v8::Function> reject = JSValue(p->Get(context, _isolate->NewString("reject"))).As<v8::Function>();
        v8::Local<v8::Value> argv[] = { err };
        reject->Call(context, v8::Undefined(_isolate->m_isolate), 1, argv).IsEmpty();
    }

    _on_cleanup(_isolate, _data);
}

void JSTrigger::_on_close_cb(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    Isolate* _isolate = Isolate::current(args);
    v8::Local<v8::Object> _data = args.Data().As<v8::Object>();
    v8::Local<v8::Context> context = _isolate->context();

    _data->Set(context, _isolate->NewString("finished"), v8::True(_isolate->m_isolate)).IsJust();

    v8::Local<v8::Array> unconsumedPromises = JSValue(_data->Get(context,
                                                          _isolate->NewString("unconsumedPromises")))
                                                  .As<v8::Array>();
    uint32_t pLen = unconsumedPromises->Length();
    if (pLen > 0) {
        v8::Local<v8::Object> p = JSValue(unconsumedPromises->Get(context, 0)).As<v8::Object>();
        for (uint32_t i = 0; i < pLen - 1; i++)
            unconsumedPromises->Set(context, i, JSValue(unconsumedPromises->Get(context, i + 1))).IsJust();
        unconsumedPromises->Delete(context, pLen - 1).IsJust();
        unconsumedPromises->Set(context, _isolate->NewString("length"),
                              v8::Integer::New(_isolate->m_isolate, pLen - 1))
            .IsJust();

        v8::Local<v8::Object> doneResult = v8::Object::New(_isolate->m_isolate);
        doneResult->Set(context, _isolate->NewString("value"), v8::Undefined(_isolate->m_isolate)).IsJust();
        doneResult->Set(context, _isolate->NewString("done"), v8::True(_isolate->m_isolate)).IsJust();

        v8::Local<v8::Function> resolve = JSValue(p->Get(context, _isolate->NewString("resolve"))).As<v8::Function>();
        v8::Local<v8::Value> argv[] = { doneResult };
        resolve->Call(context, v8::Undefined(_isolate->m_isolate), 1, argv).IsEmpty();
    }

    _on_cleanup(_isolate, _data);
}

// ---- events.addAbortListener helpers ----

void JSTrigger::_disposeAbortListener(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    Isolate* _isolate = Isolate::current(args);
    v8::Local<v8::Object> _data = v8::Local<v8::Object>::Cast(args.Data());
    v8::Local<v8::Context> context = _isolate->context();

    v8::Local<v8::Value> sig = JSValue(_data->Get(context, _isolate->NewString("signal")));
    v8::Local<v8::Value> lis = JSValue(_data->Get(context, _isolate->NewString("listener")));

    if (!sig.IsEmpty() && sig->IsObject() && !lis.IsEmpty() && lis->IsFunction()) {
        v8::Local<v8::Object> retVal;
        JSTrigger(args.GetIsolate(), sig.As<v8::Object>()).off("abort", lis.As<v8::Function>(), retVal);
    }
}

void JSTrigger::s_addAbortListener(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    Isolate* _isolate = Isolate::current(args);
    v8::Local<v8::Context> context = _isolate->context();

    if (args.Length() < 2) {
        ThrowResult(CALL_E_BADPARAMCOUNT);
        return;
    }

    if (!args[0]->IsObject()) {
        ThrowResult(CALL_E_TYPEMISMATCH);
        return;
    }

    if (!args[1]->IsFunction()) {
        ThrowResult(CALL_E_TYPEMISMATCH);
        return;
    }

    v8::Local<v8::Object> signal = args[0].As<v8::Object>();
    v8::Local<v8::Function> listener = args[1].As<v8::Function>();
    v8::Local<v8::Object> retVal;

    // Check signal.aborted
    v8::Local<v8::Value> aborted = JSValue(signal->Get(context, _isolate->NewString("aborted")));

    if (!aborted.IsEmpty() && aborted->IsTrue()) {
        // If already aborted, call listener synchronously
        listener->Call(context, v8::Undefined(_isolate->m_isolate), 0, nullptr).IsEmpty();
    } else {
        // signal.once('abort', listener)
        JSTrigger(_isolate->m_isolate, signal).once("abort", listener, retVal);
    }

    // Create disposable: { [Symbol.dispose]() { signal.off('abort', listener) } }
    v8::Local<v8::Object> _data = v8::Object::New(_isolate->m_isolate);
    _data->Set(context, _isolate->NewString("signal"), signal).IsJust();
    _data->Set(context, _isolate->NewString("listener"), listener).IsJust();

    v8::Local<v8::Function> disposeFn = _isolate->NewFunction("_disposeAbortListener",
        _disposeAbortListener, _data);

    v8::Local<v8::Object> result = v8::Object::New(_isolate->m_isolate);
    result->SetPrototype(context, v8::Null(_isolate->m_isolate)).Check();
    result->Set(context, v8::Symbol::GetDispose(_isolate->m_isolate), disposeFn).Check();

    args.GetReturnValue().Set(result);
}

void JSTrigger::_add_signal_listener(Isolate* _isolate, v8::Local<v8::Object> sigObj,
    v8::Local<v8::Function> cb)
{
    v8::Local<v8::Context> context = _isolate->context();
    v8::Local<v8::Value> addEvFn = JSValue(sigObj->Get(context, _isolate->NewString("addEventListener")));
    if (!addEvFn.IsEmpty() && addEvFn->IsFunction()) {
        v8::Local<v8::Object> opts = v8::Object::New(_isolate->m_isolate);
        opts->Set(context, _isolate->NewString("once"), v8::True(_isolate->m_isolate)).IsJust();
        v8::Local<v8::Value> argv[3] = { _isolate->NewString("abort"), cb, opts };
        addEvFn.As<v8::Function>()->Call(context, sigObj, 3, argv).IsEmpty();
    }
}

void JSTrigger::s_once_static(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    Isolate* _isolate = Isolate::current(args);
    v8::Local<v8::Context> context = _isolate->context();

    if (args.Length() < 2) {
        ThrowResult(CALL_E_BADPARAMCOUNT);
        return;
    }
    if (!args[0]->IsObject()) {
        ThrowResult(CALL_E_TYPEMISMATCH);
        return;
    }

    v8::Local<v8::Object> emitter = args[0].As<v8::Object>();
    exlib::string event;
    result_t hr = GetArgumentValue(_isolate, args[1], event, true);
    if (hr < 0) {
        ThrowResult(hr);
        return;
    }

    v8::Local<v8::Promise::Resolver> resolver = v8::Promise::Resolver::New(context)
                                                    .FromMaybe(v8::Local<v8::Promise::Resolver>());
    if (resolver.IsEmpty()) {
        ThrowResult(CALL_E_INTERNAL);
        return;
    }

    // Shared data for event callback
    v8::Local<v8::Object> evData = v8::Object::New(_isolate->m_isolate);
    evData->Set(context, _isolate->NewString("resolver"), resolver).IsJust();
    evData->Set(context, _isolate->NewString("emitter"), emitter).IsJust();
    evData->Set(context, _isolate->NewString("event"), _isolate->NewString(event)).IsJust();

    v8::Local<v8::Function> eventListener = _isolate->NewFunction("_once_event_cb", _once_event_cb, evData);

    // error listener (unless event IS 'error')
    v8::Local<v8::Function> errorListener;
    if (event != "error") {
        v8::Local<v8::Object> errData = v8::Object::New(_isolate->m_isolate);
        errData->Set(context, _isolate->NewString("resolver"), resolver).IsJust();
        errData->Set(context, _isolate->NewString("emitter"), emitter).IsJust();
        errData->Set(context, _isolate->NewString("event"), _isolate->NewString(event)).IsJust();
        errData->Set(context, _isolate->NewString("eventListener"), eventListener).IsJust();

        errorListener = _isolate->NewFunction("_once_error_cb", _once_error_cb, errData);
        evData->Set(context, _isolate->NewString("errorListener"), errorListener).IsJust();

        v8::Local<v8::Object> retVal;
        JSTrigger(_isolate->m_isolate, emitter).once("error", errorListener, retVal);
    }

    v8::Local<v8::Object> retVal;
    JSTrigger(_isolate->m_isolate, emitter).once(event, eventListener, retVal);

    // Handle options.signal
    if (args.Length() > 2 && args[2]->IsObject()) {
        v8::Local<v8::Object> options = args[2].As<v8::Object>();
        v8::Local<v8::Value> signal = JSValue(options->Get(context, _isolate->NewString("signal")));

        if (!signal.IsEmpty() && signal->IsObject()) {
            v8::Local<v8::Object> sigObj = signal.As<v8::Object>();
            v8::Local<v8::Value> aborted = JSValue(sigObj->Get(context, _isolate->NewString("aborted")));

            if (!aborted.IsEmpty() && aborted->IsTrue()) {
                JSTrigger t(_isolate->m_isolate, emitter);
                t.off(event, eventListener, retVal);
                if (!errorListener.IsEmpty())
                    t.off("error", errorListener, retVal);
                resolver->Reject(context, _make_abort_error(_isolate)).IsJust();
            } else {
                v8::Local<v8::Object> abortData = v8::Object::New(_isolate->m_isolate);
                abortData->Set(context, _isolate->NewString("resolver"), resolver).IsJust();
                abortData->Set(context, _isolate->NewString("emitter"), emitter).IsJust();
                abortData->Set(context, _isolate->NewString("event"), _isolate->NewString(event)).IsJust();
                abortData->Set(context, _isolate->NewString("eventListener"), eventListener).IsJust();
                if (!errorListener.IsEmpty())
                    abortData->Set(context, _isolate->NewString("errorListener"), errorListener).IsJust();

                v8::Local<v8::Function> abortCb = _isolate->NewFunction("_once_abort_cb", _once_abort_cb, abortData);
                _add_signal_listener(_isolate, sigObj, abortCb);
            }
        }
    }

    args.GetReturnValue().Set(resolver->GetPromise());
}

// ---- events.on() iterator method callbacks ----

void JSTrigger::_on_next_cb(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    Isolate* _isolate = Isolate::current(args);
    v8::Local<v8::Object> _data = args.Data().As<v8::Object>();
    v8::Local<v8::Context> context = _isolate->context();

    v8::Local<v8::Array> unconsumedEvents = JSValue(_data->Get(context,
                                                        _isolate->NewString("unconsumedEvents")))
                                                .As<v8::Array>();
    v8::Local<v8::Array> unconsumedPromises = JSValue(_data->Get(context,
                                                          _isolate->NewString("unconsumedPromises")))
                                                  .As<v8::Array>();

    // If there are buffered events, return immediately
    if (unconsumedEvents->Length() > 0) {
        v8::Local<v8::Value> val = JSValue(unconsumedEvents->Get(context, 0));
        uint32_t eLen = unconsumedEvents->Length();
        for (uint32_t i = 0; i < eLen - 1; i++)
            unconsumedEvents->Set(context, i, JSValue(unconsumedEvents->Get(context, i + 1))).IsJust();
        unconsumedEvents->Delete(context, eLen - 1).IsJust();
        unconsumedEvents->Set(context, _isolate->NewString("length"),
                            v8::Integer::New(_isolate->m_isolate, eLen - 1))
            .IsJust();

        v8::Local<v8::Object> r = v8::Object::New(_isolate->m_isolate);
        r->Set(context, _isolate->NewString("value"), val).IsJust();
        r->Set(context, _isolate->NewString("done"), v8::False(_isolate->m_isolate)).IsJust();

        v8::Local<v8::Promise::Resolver> resolver = v8::Promise::Resolver::New(context)
                                                        .FromMaybe(v8::Local<v8::Promise::Resolver>());
        resolver->Resolve(context, r).IsJust();
        args.GetReturnValue().Set(resolver->GetPromise());
        return;
    }

    // Check error
    v8::Local<v8::Value> error = JSValue(_data->Get(context, _isolate->NewString("error")));
    if (!error.IsEmpty() && !error->IsUndefined()) {
        _data->Set(context, _isolate->NewString("error"), v8::Undefined(_isolate->m_isolate)).IsJust();
        v8::Local<v8::Promise::Resolver> resolver = v8::Promise::Resolver::New(context)
                                                        .FromMaybe(v8::Local<v8::Promise::Resolver>());
        resolver->Reject(context, error).IsJust();
        args.GetReturnValue().Set(resolver->GetPromise());
        return;
    }

    // Check finished
    v8::Local<v8::Value> finished = JSValue(_data->Get(context, _isolate->NewString("finished")));
    if (!finished.IsEmpty() && finished->IsTrue()) {
        v8::Local<v8::Object> r = v8::Object::New(_isolate->m_isolate);
        r->Set(context, _isolate->NewString("value"), v8::Undefined(_isolate->m_isolate)).IsJust();
        r->Set(context, _isolate->NewString("done"), v8::True(_isolate->m_isolate)).IsJust();

        v8::Local<v8::Promise::Resolver> resolver = v8::Promise::Resolver::New(context)
                                                        .FromMaybe(v8::Local<v8::Promise::Resolver>());
        resolver->Resolve(context, r).IsJust();
        args.GetReturnValue().Set(resolver->GetPromise());
        return;
    }

    // No data available, push to pending promises
    v8::Local<v8::Promise::Resolver> resolver = v8::Promise::Resolver::New(context)
                                                    .FromMaybe(v8::Local<v8::Promise::Resolver>());
    v8::Local<v8::Object> entry = v8::Object::New(_isolate->m_isolate);
    entry->Set(context, _isolate->NewString("resolve"),
             _isolate->NewFunction("_resolve", [](const v8::FunctionCallbackInfo<v8::Value>& a) {
                Isolate* iso = Isolate::current(a);
                v8::Local<v8::Promise::Resolver> res = a.Data().As<v8::Promise::Resolver>();
                res->Resolve(iso->context(), a[0]).IsJust(); }, resolver))
        .IsJust();
    entry->Set(context, _isolate->NewString("reject"),
             _isolate->NewFunction("_reject", [](const v8::FunctionCallbackInfo<v8::Value>& a) {
                Isolate* iso = Isolate::current(a);
                v8::Local<v8::Promise::Resolver> res = a.Data().As<v8::Promise::Resolver>();
                res->Reject(iso->context(), a[0]).IsJust(); }, resolver))
        .IsJust();

    uint32_t pLen = unconsumedPromises->Length();
    unconsumedPromises->Set(context, pLen, entry).IsJust();

    args.GetReturnValue().Set(resolver->GetPromise());
}

void JSTrigger::_on_return_cb(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    Isolate* _isolate = Isolate::current(args);
    v8::Local<v8::Object> _data = args.Data().As<v8::Object>();
    v8::Local<v8::Context> context = _isolate->context();

    _on_cleanup(_isolate, _data);
    _data->Set(context, _isolate->NewString("finished"), v8::True(_isolate->m_isolate)).IsJust();

    // Resolve all pending promises with done
    v8::Local<v8::Object> doneResult = v8::Object::New(_isolate->m_isolate);
    doneResult->Set(context, _isolate->NewString("value"), v8::Undefined(_isolate->m_isolate)).IsJust();
    doneResult->Set(context, _isolate->NewString("done"), v8::True(_isolate->m_isolate)).IsJust();

    v8::Local<v8::Array> unconsumedPromises = JSValue(_data->Get(context,
                                                          _isolate->NewString("unconsumedPromises")))
                                                  .As<v8::Array>();
    for (uint32_t i = 0; i < unconsumedPromises->Length(); i++) {
        v8::Local<v8::Object> p = JSValue(unconsumedPromises->Get(context, i)).As<v8::Object>();
        v8::Local<v8::Function> resolve = JSValue(p->Get(context, _isolate->NewString("resolve"))).As<v8::Function>();
        v8::Local<v8::Value> argv[] = { doneResult };
        resolve->Call(context, v8::Undefined(_isolate->m_isolate), 1, argv).IsEmpty();
    }
    unconsumedPromises->Set(context, _isolate->NewString("length"),
                          v8::Integer::New(_isolate->m_isolate, 0))
        .IsJust();

    v8::Local<v8::Promise::Resolver> resolver = v8::Promise::Resolver::New(context)
                                                    .FromMaybe(v8::Local<v8::Promise::Resolver>());
    resolver->Resolve(context, doneResult).IsJust();
    args.GetReturnValue().Set(resolver->GetPromise());
}

void JSTrigger::_on_throw_cb(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    Isolate* _isolate = Isolate::current(args);
    v8::Local<v8::Object> _data = args.Data().As<v8::Object>();
    v8::Local<v8::Context> context = _isolate->context();

    v8::Local<v8::Value> err = args.Length() > 0
        ? args[0]
        : v8::Undefined(_isolate->m_isolate).As<v8::Value>();

    _on_cleanup(_isolate, _data);
    _data->Set(context, _isolate->NewString("error"), err).IsJust();

    // Reject all pending promises
    v8::Local<v8::Array> unconsumedPromises = JSValue(_data->Get(context,
                                                          _isolate->NewString("unconsumedPromises")))
                                                  .As<v8::Array>();
    for (uint32_t i = 0; i < unconsumedPromises->Length(); i++) {
        v8::Local<v8::Object> p = JSValue(unconsumedPromises->Get(context, i)).As<v8::Object>();
        v8::Local<v8::Function> reject = JSValue(p->Get(context, _isolate->NewString("reject"))).As<v8::Function>();
        v8::Local<v8::Value> argv[] = { err };
        reject->Call(context, v8::Undefined(_isolate->m_isolate), 1, argv).IsEmpty();
    }
    unconsumedPromises->Set(context, _isolate->NewString("length"),
                          v8::Integer::New(_isolate->m_isolate, 0))
        .IsJust();

    v8::Local<v8::Promise::Resolver> resolver = v8::Promise::Resolver::New(context)
                                                    .FromMaybe(v8::Local<v8::Promise::Resolver>());
    resolver->Reject(context, err).IsJust();
    args.GetReturnValue().Set(resolver->GetPromise());
}

void JSTrigger::s_on_static(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    Isolate* _isolate = Isolate::current(args);
    v8::Local<v8::Context> context = _isolate->context();

    if (args.Length() < 2) {
        ThrowResult(CALL_E_BADPARAMCOUNT);
        return;
    }
    if (!args[0]->IsObject()) {
        ThrowResult(CALL_E_TYPEMISMATCH);
        return;
    }

    v8::Local<v8::Object> emitter = args[0].As<v8::Object>();
    exlib::string event;
    result_t hr = GetArgumentValue(_isolate, args[1], event, true);
    if (hr < 0) {
        ThrowResult(hr);
        return;
    }

    // State object shared by all callbacks
    v8::Local<v8::Object> state = v8::Object::New(_isolate->m_isolate);
    state->Set(context, _isolate->NewString("emitter"), emitter).IsJust();
    state->Set(context, _isolate->NewString("event"), _isolate->NewString(event)).IsJust();
    state->Set(context, _isolate->NewString("unconsumedEvents"), v8::Array::New(_isolate->m_isolate)).IsJust();
    state->Set(context, _isolate->NewString("unconsumedPromises"), v8::Array::New(_isolate->m_isolate)).IsJust();
    state->Set(context, _isolate->NewString("finished"), v8::False(_isolate->m_isolate)).IsJust();

    // Create handler functions
    v8::Local<v8::Function> eventHandler = _isolate->NewFunction("_on_event_cb", _on_event_cb, state);
    v8::Local<v8::Function> errorHandler = _isolate->NewFunction("_on_error_cb", _on_error_cb, state);
    v8::Local<v8::Function> closeHandler = _isolate->NewFunction("_on_close_cb", _on_close_cb, state);

    state->Set(context, _isolate->NewString("eventHandler"), eventHandler).IsJust();
    state->Set(context, _isolate->NewString("errorHandler"), errorHandler).IsJust();
    state->Set(context, _isolate->NewString("closeHandler"), closeHandler).IsJust();

    // Register event listeners
    v8::Local<v8::Object> retVal;
    JSTrigger t(_isolate->m_isolate, emitter);
    t.on(event, eventHandler, retVal);
    t.on("error", errorHandler, retVal);

    // Handle options
    if (args.Length() > 2 && args[2]->IsObject()) {
        v8::Local<v8::Object> options = args[2].As<v8::Object>();

        // Check for Symbol-keyed kFirstEventParam option
        v8::Local<v8::Array> ownProps = options->GetOwnPropertyNames(context,
                                                   static_cast<v8::PropertyFilter>(v8::PropertyFilter::ALL_PROPERTIES),
                                                   v8::KeyConversionMode::kKeepNumbers)
                                            .FromMaybe(v8::Local<v8::Array>());
        if (!ownProps.IsEmpty()) {
            for (uint32_t i = 0; i < ownProps->Length(); i++) {
                v8::Local<v8::Value> key = JSValue(ownProps->Get(context, i));
                if (!key.IsEmpty() && key->IsSymbol()) {
                    v8::Local<v8::Symbol> sym = key.As<v8::Symbol>();
                    v8::Local<v8::Value> desc = sym->Description(_isolate->m_isolate);
                    if (!desc.IsEmpty() && desc->IsString()) {
                        v8::String::Utf8Value descStr(_isolate->m_isolate, desc);
                        if (strcmp(*descStr, "nodejs.kFirstEventParam") == 0) {
                            v8::Local<v8::Value> val = JSValue(options->Get(context, sym));
                            if (!val.IsEmpty() && val->IsTrue()) {
                                state->Set(context, _isolate->NewString("firstEventParam"),
                                         v8::True(_isolate->m_isolate))
                                    .IsJust();
                            }
                        }
                    }
                }
            }
        }

        // options.close - array of event names that end iteration
        v8::Local<v8::Value> closeVal = JSValue(options->Get(context, _isolate->NewString("close")));
        if (!closeVal.IsEmpty() && closeVal->IsArray()) {
            v8::Local<v8::Array> closeArr = closeVal.As<v8::Array>();
            state->Set(context, _isolate->NewString("closeEvents"), closeArr).IsJust();
            for (uint32_t i = 0; i < closeArr->Length(); i++) {
                exlib::string cev;
                GetArgumentValue(_isolate, JSValue(closeArr->Get(context, i)), cev, true);
                t.on(cev, closeHandler, retVal);
            }
        }

        // options.signal
        v8::Local<v8::Value> signal = JSValue(options->Get(context, _isolate->NewString("signal")));
        if (!signal.IsEmpty() && signal->IsObject()) {
            v8::Local<v8::Object> sigObj = signal.As<v8::Object>();
            v8::Local<v8::Value> aborted = JSValue(sigObj->Get(context, _isolate->NewString("aborted")));

            if (!aborted.IsEmpty() && aborted->IsTrue()) {
                _on_cleanup(_isolate, state);
                v8::Local<v8::Value> err = _make_abort_error(_isolate);

                // Return a dead async iterator
                v8::Local<v8::Object> iter = v8::Object::New(_isolate->m_isolate);
                v8::Local<v8::Object> errData = v8::Object::New(_isolate->m_isolate);
                errData->Set(context, _isolate->NewString("error"), err).IsJust();

                iter->Set(context, v8::Symbol::GetAsyncIterator(_isolate->m_isolate),
                        _isolate->NewFunction("asyncIterator", [](const v8::FunctionCallbackInfo<v8::Value>& a) {
                            a.GetReturnValue().Set(a.This());
                        }))
                    .IsJust();
                iter->Set(context, _isolate->NewString("next"),
                        _isolate->NewFunction("next", [](const v8::FunctionCallbackInfo<v8::Value>& a) {
                            Isolate* iso = Isolate::current(a);
                            v8::Local<v8::Value> e = JSValue(a.Data().As<v8::Object>()->Get(iso->context(), iso->NewString("error")));
                            v8::Local<v8::Promise::Resolver> r = v8::Promise::Resolver::New(iso->context())
                                .FromMaybe(v8::Local<v8::Promise::Resolver>());
                            r->Reject(iso->context(), e).IsJust();
                            a.GetReturnValue().Set(r->GetPromise()); }, errData))
                    .IsJust();
                iter->Set(context, _isolate->NewString("return"),
                        _isolate->NewFunction("return", [](const v8::FunctionCallbackInfo<v8::Value>& a) {
                            Isolate* iso = Isolate::current(a);
                            v8::Local<v8::Object> r2 = v8::Object::New(iso->m_isolate);
                            r2->Set(iso->context(), iso->NewString("value"), v8::Undefined(iso->m_isolate)).IsJust();
                            r2->Set(iso->context(), iso->NewString("done"), v8::True(iso->m_isolate)).IsJust();
                            v8::Local<v8::Promise::Resolver> r = v8::Promise::Resolver::New(iso->context())
                                                                     .FromMaybe(v8::Local<v8::Promise::Resolver>());
                            r->Resolve(iso->context(), r2).IsJust();
                            a.GetReturnValue().Set(r->GetPromise());
                        }))
                    .IsJust();

                args.GetReturnValue().Set(iter);
                return;
            } else {
                v8::Local<v8::Function> abortCb = _isolate->NewFunction("_on_abort", [](const v8::FunctionCallbackInfo<v8::Value>& a) {
                        Isolate* iso = Isolate::current(a);
                        v8::Local<v8::Object> d = a.Data().As<v8::Object>();
                        _on_error_cb_invoke(iso, d, _make_abort_error(iso)); }, state);
                _add_signal_listener(_isolate, sigObj, abortCb);
            }
        }
    }

    // Build async iterator object
    v8::Local<v8::Object> iter = v8::Object::New(_isolate->m_isolate);
    iter->Set(context, v8::Symbol::GetAsyncIterator(_isolate->m_isolate),
            _isolate->NewFunction("asyncIterator", [](const v8::FunctionCallbackInfo<v8::Value>& a) {
                a.GetReturnValue().Set(a.This());
            }))
        .IsJust();
    iter->Set(context, _isolate->NewString("next"),
            _isolate->NewFunction("next", _on_next_cb, state))
        .IsJust();
    iter->Set(context, _isolate->NewString("return"),
            _isolate->NewFunction("return", _on_return_cb, state))
        .IsJust();
    iter->Set(context, _isolate->NewString("throw"),
            _isolate->NewFunction("throw", _on_throw_cb, state))
        .IsJust();

    args.GetReturnValue().Set(iter);
}

// Helper for signal abort triggering error from outside a FunctionCallbackInfo
void JSTrigger::_on_error_cb_invoke(Isolate* _isolate, v8::Local<v8::Object> _data, v8::Local<v8::Value> err)
{
    v8::Local<v8::Context> context = _isolate->context();

    _data->Set(context, _isolate->NewString("error"), err).IsJust();

    v8::Local<v8::Array> unconsumedPromises = JSValue(_data->Get(context,
                                                          _isolate->NewString("unconsumedPromises")))
                                                  .As<v8::Array>();
    uint32_t pLen = unconsumedPromises->Length();
    if (pLen > 0) {
        v8::Local<v8::Object> p = JSValue(unconsumedPromises->Get(context, 0)).As<v8::Object>();
        for (uint32_t i = 0; i < pLen - 1; i++)
            unconsumedPromises->Set(context, i, JSValue(unconsumedPromises->Get(context, i + 1))).IsJust();
        unconsumedPromises->Delete(context, pLen - 1).IsJust();
        unconsumedPromises->Set(context, _isolate->NewString("length"),
                              v8::Integer::New(_isolate->m_isolate, pLen - 1))
            .IsJust();

        v8::Local<v8::Function> reject = JSValue(p->Get(context, _isolate->NewString("reject"))).As<v8::Function>();
        v8::Local<v8::Value> argv[] = { err };
        reject->Call(context, v8::Undefined(_isolate->m_isolate), 1, argv).IsEmpty();
    }

    _on_cleanup(_isolate, _data);
}

}
