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

result_t object_base::removeAllListeners(exlib::string evs, v8::Local<v8::Object>& retVal)
{
    return JSTrigger(this).removeAllListeners(evs, retVal);
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

result_t object_base::getListener(exlib::string ev, v8::Local<v8::Function> func)
{
    result_t hr;
    JSTrigger e(this);
    v8::Local<v8::Array> r;

    hr = listeners(ev, r);
    if (hr < 0)
        return hr;

    if (r->Length() == 0)
        return CALL_RETURN_NULL;

    func = v8::Local<v8::Function>::Cast(r->Get(0));
    return 0;
}

result_t object_base::listeners(exlib::string ev, v8::Local<v8::Array>& retVal)
{
    return JSTrigger(this).listeners(ev, retVal);
}

result_t object_base::_emit(exlib::string ev, Variant arg)
{
    return _emit(ev, &arg, 1);
}

class jsTrigger {
public:
    jsTrigger(object_base* obj, exlib::string ev, Variant* args, int32_t argCount)
        : m_obj(obj)
        , m_ev(ev)
    {
        m_args.append((VariantEx*)args, argCount);
    }

public:
    static result_t jsEmit(jsTrigger* p)
    {
        JSFiber::EnterJsScope s;
        size_t i;

        std::vector<v8::Local<v8::Value>> argv;

        argv.resize(p->m_args.size());
        for (i = 0; i < p->m_args.size(); i++)
            argv[i] = v8::Local<v8::Value>::New(p->m_obj->holder()->m_isolate, p->m_args[i]);

        p->m_obj->onEventEmit(p->m_ev);

        bool r;
        JSTrigger(p->m_obj)._emit(p->m_ev, argv.data(), (int32_t)argv.size(), r);

        delete p;

        return 0;
    }

public:
    obj_ptr<object_base> m_obj;
    exlib::string m_ev;
    QuickArray<VariantEx> m_args;
};

result_t object_base::_emit(exlib::string ev, Variant* args, int32_t argCount)
{
    Isolate* isolate = get_holder();
    if (isolate)
        syncCall(isolate, jsTrigger::jsEmit, new jsTrigger(this, ev, args, argCount));

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

result_t object_base::eventNames(v8::Local<v8::Array>& retVal)
{
    return JSTrigger(this).eventNames(retVal);
}
}
