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
#include "ifs/events.h"
#include "QuickArray.h"
#include <vector>

namespace fibjs
{

class RootModule_events : public RootModule
{
public:
    RootModule_events()
    {
        static ClassData::ClassMethod s_method[] =
        {
            {"on", JSTrigger::s_on, false},
            {"addListener", JSTrigger::s_on, false},
            {"once", JSTrigger::s_once, false},
            {"off", JSTrigger::s_off, false},
            {"removeListener", JSTrigger::s_off, false},
            {"removeAllListeners", JSTrigger::s_removeAllListeners, false},
            {"setMaxListeners", JSTrigger::s_setMaxListeners, false},
            {"listeners", JSTrigger::s_listeners, false},
            {"trigger", JSTrigger::s_trigger, false},
            {"emit", JSTrigger::s_trigger, false}
        };

        ClassData& cd = EventEmitter_base::class_info().date();
        cd.mc = ARRAYSIZE(s_method);
        cd.cms = s_method;
    }

public:
    virtual ClassInfo &class_info()
    {
        return events_base::class_info();
    }
} s_RootModule_events;

result_t EventEmitter_base::_new(obj_ptr<EventEmitter_base> &retVal, v8::Local<v8::Object> This)
{
    retVal = new EventEmitter();
    return 0;
}

result_t Trigger_base::_new(obj_ptr<Trigger_base> &retVal, v8::Local<v8::Object> This)
{
    retVal = new Trigger();
    return 0;
}

result_t object_base::on(exlib::string ev, v8::Local<v8::Function> func, int32_t &retVal)
{
    return JSTrigger(this).on(ev, func, retVal);
}

result_t object_base::on(v8::Local<v8::Object> map, int32_t &retVal)
{
    return JSTrigger(this).on(map, retVal);
}

result_t object_base::once(exlib::string ev, v8::Local<v8::Function> func, int32_t &retVal)
{
    return JSTrigger(this).once(ev, func, retVal);
}

result_t object_base::once(v8::Local<v8::Object> map, int32_t &retVal)
{
    return JSTrigger(this).once(map, retVal);
}

result_t object_base::off(exlib::string ev, v8::Local<v8::Function> func, int32_t &retVal)
{
    return JSTrigger(this).off(ev, func, retVal);
}

result_t object_base::off(exlib::string ev, int32_t &retVal)
{
    return JSTrigger(this).off(ev, retVal);
}

result_t object_base::off(v8::Local<v8::Object> map, int32_t &retVal)
{
    return JSTrigger(this).off(map, retVal);
}

result_t object_base::removeAllListeners(v8::Local<v8::Array> evs, int32_t& retVal)
{
    return JSTrigger(this).removeAllListeners(evs, retVal);
}

result_t object_base::listeners(exlib::string ev, v8::Local<v8::Array>& retVal)
{
    return JSTrigger(this).listeners(ev, retVal);
}

result_t object_base::_trigger(exlib::string ev, Variant *args, int32_t argCount)
{
    class jsTrigger: public AsyncEvent
    {
    public:
        jsTrigger(object_base *obj, exlib::string ev, Variant *args, int32_t argCount) :
            m_obj(obj), m_ev(ev)
        {
            m_args.append((VariantEx *)args, argCount);
        }

    public:
        virtual result_t js_invoke()
        {
            JSFiber::scope s;
            size_t i;

            std::vector<v8::Local<v8::Value> > argv;

            argv.resize(m_args.size());
            for (i = 0; i < m_args.size(); i++)
                argv[i] = v8::Local<v8::Value>::New(m_obj->holder()->m_isolate, m_args[i]);

            JSTrigger(m_obj)._trigger(m_ev, argv.data(), (int32_t) argv.size());

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

result_t object_base::trigger(exlib::string ev, const v8::FunctionCallbackInfo<v8::Value> &args)
{
    return JSTrigger(this).trigger(ev, args);
}

}
