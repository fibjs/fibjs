/*
 * Event.cpp
 *
 *  Created on: Apr 8, 2012
 *      Author: lion
 */

#include "object.h"
#include "Fiber.h"
#include "Trigger.h"
#include "ifs/coroutine.h"
#include "ifs/EventEmitter.h"
#include "QuickArray.h"
#include <vector>

namespace fibjs {

class RootModule_events : public RootModule {
public:
    RootModule_events()
    {
        static ClassData::ClassMethod s_method[] = {
            { "on", JSTrigger::s_on, true },
            { "addListener", JSTrigger::s_on, true },
            { "once", JSTrigger::s_once, true },
            { "off", JSTrigger::s_off, true },
            { "removeListener", JSTrigger::s_off, true },
            { "removeAllListeners", JSTrigger::s_removeAllListeners, true },
            { "setMaxListeners", JSTrigger::s_setMaxListeners, true },
            { "listeners", JSTrigger::s_listeners, true },
            { "emit", JSTrigger::s_emit, true }
        };

        ClassData& cd = EventEmitter_base::class_info().data();

        cd.cor = JSTrigger::s__new;
        cd.mc = ARRAYSIZE(s_method);
        cd.cms = s_method;
    }

    virtual ClassInfo& class_info()
    {
        return EventEmitter_base::class_info();
    }

    virtual const char* name()
    {
        return "events";
    }
} s_RootModule_events;

result_t EventEmitter_base::_new(obj_ptr<EventEmitter_base>& retVal, v8::Local<v8::Object> This)
{
    retVal = new EventEmitter();
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

result_t object_base::once(exlib::string ev, v8::Local<v8::Function> func, v8::Local<v8::Object>& retVal)
{
    return JSTrigger(this).once(ev, func, retVal);
}

result_t object_base::once(v8::Local<v8::Object> map, v8::Local<v8::Object>& retVal)
{
    return JSTrigger(this).once(map, retVal);
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

result_t object_base::removeAllListeners(v8::Local<v8::Array> evs, v8::Local<v8::Object>& retVal)
{
    return JSTrigger(this).removeAllListeners(evs, retVal);
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

result_t object_base::_emit(exlib::string ev, Variant* args, int32_t argCount)
{
    class jsTrigger : public AsyncEvent {
    public:
        jsTrigger(object_base* obj, exlib::string ev, Variant* args, int32_t argCount)
            : m_obj(obj)
            , m_ev(ev)
        {
            m_args.append((VariantEx*)args, argCount);
        }

    public:
        virtual result_t js_invoke()
        {
            JSFiber::scope s;
            size_t i;

            std::vector<v8::Local<v8::Value>> argv;

            argv.resize(m_args.size());
            for (i = 0; i < m_args.size(); i++)
                argv[i] = v8::Local<v8::Value>::New(m_obj->holder()->m_isolate, m_args[i]);

            bool r;
            JSTrigger(m_obj)._emit(m_ev, argv.data(), (int32_t)argv.size(), r);

            delete this;

            return 0;
        }

    private:
        obj_ptr<object_base> m_obj;
        exlib::string m_ev;
        QuickArray<VariantEx> m_args;
    };

    Isolate* isolate = holder();
    if (isolate)
        (new jsTrigger(this, ev, args, argCount))->sync(isolate);

    return 0;
}

result_t object_base::emit(exlib::string ev, const v8::FunctionCallbackInfo<v8::Value>& args,
    bool& retVal)
{
    return JSTrigger(this).emit(ev, args, retVal);
}
}
