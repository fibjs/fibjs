/*
 * coroutine.cpp
 *
 *  Created on: Apr 9, 2012
 *      Author: lion
 */

#include "object.h"
#include "ifs/coroutine.h"
#include "Event.h"
#include "ifs/os.h"
#include "Fiber.h"
#include <vector>

namespace fibjs {

DECLARE_MODULE(coroutine);

extern int32_t g_spareFibers;

result_t coroutine_base::start(v8::Local<v8::Function> func, OptArgs args,
    obj_ptr<Fiber_base>& retVal)
{
    return JSFiber::New(func, args, retVal);
}

class _parallels {
private:
    result_t _worker()
    {
        v8::Local<v8::Array> datas = v8::Local<v8::Array>::New(m_isolate->m_isolate, m_datas);
        v8::Local<v8::Function> func = v8::Local<v8::Function>::New(m_isolate->m_isolate, m_func);
        v8::Local<v8::Array> retVal = v8::Local<v8::Array>::New(m_isolate->m_isolate, m_retVal);

        while (m_pos < m_count) {
            JSFiber::scope s;
            v8::Local<v8::Value> v;
            int32_t pos = m_pos;

            s->set_caller(m_caller);

            m_pos++;
            if (func.IsEmpty())
                v = v8::Local<v8::Function>::Cast(datas->Get(pos))
                        ->Call(s->wrap(), 0, NULL);
            else {
                v8::Local<v8::Value> a = datas->Get(pos);
                v = func->Call(s->wrap(), 1, &a);
            }

            if (!v.IsEmpty())
                retVal->Set(pos, v);
            else
                m_error = true;
        }

        m_fibers--;
        if (m_fibers == 0)
            m_event->set();

        return 0;
    }

    static result_t worker(_parallels* pThis)
    {
        return pThis->_worker();
    }

    result_t run(v8::Local<v8::Array>& retVal)
    {
        int32_t i;

        retVal = v8::Array::New(m_isolate->m_isolate, m_count);
        m_retVal.Reset(m_isolate->m_isolate, retVal);

        m_event = new Event();
        m_error = false;
        m_pos = 0;
        m_caller = JSFiber::current();

        for (i = 0; i < m_fibers; i++)
            syncCall(m_isolate, worker, this);

        m_event->wait();

        m_datas.Reset();
        m_func.Reset();
        m_retVal.Reset();

        return m_error ? CHECK_ERROR(CALL_E_INTERNAL) : 0;
    }

public:
    result_t run(v8::Local<v8::Array> funcs,
        v8::Local<v8::Array>& retVal, int32_t fibers = -1)
    {
        m_isolate = Isolate::current();
        int32_t i;

        m_count = funcs->Length();
        if (m_count == 0) {
            retVal = v8::Array::New(m_isolate->m_isolate, 0);
            return 0;
        }

        m_fibers = (fibers > 0 && fibers < m_count) ? fibers : m_count;

        for (i = 0; i < m_count; i++) {
            v8::Local<v8::Value> v = funcs->Get(i);
            if (v.IsEmpty() || !v->IsFunction())
                return CHECK_ERROR(CALL_E_INVALIDARG);
        }

        m_datas.Reset(m_isolate->m_isolate, funcs);

        return run(retVal);
    }

    result_t run(v8::Local<v8::Array> datas, v8::Local<v8::Function> func,
        v8::Local<v8::Array>& retVal, int32_t fibers = -1)
    {
        m_isolate = Isolate::current();

        m_count = datas->Length();
        if (m_count == 0) {
            retVal = v8::Array::New(m_isolate->m_isolate, 0);
            return 0;
        }

        m_fibers = (fibers > 0 && fibers < m_count) ? fibers : m_count;

        m_datas.Reset(m_isolate->m_isolate, datas);
        m_func.Reset(m_isolate->m_isolate, func);

        return run(retVal);
    }

public:
    Isolate* m_isolate;
    int32_t m_count, m_fibers, m_pos;
    v8::Global<v8::Array> m_datas;
    v8::Global<v8::Function> m_func;
    v8::Global<v8::Array> m_retVal;
    obj_ptr<Event> m_event;
    obj_ptr<Fiber_base> m_caller;
    bool m_error;
};

result_t coroutine_base::parallel(v8::Local<v8::Array> funcs, int32_t fibers,
    v8::Local<v8::Array>& retVal)
{
    _parallels _p;
    return _p.run(funcs, retVal, fibers);
}

result_t coroutine_base::parallel(OptArgs funcs, v8::Local<v8::Array>& retVal)
{
    v8::Isolate* isolate = Isolate::current()->m_isolate;
    int32_t num = funcs.Length();
    v8::Local<v8::Array> _funcs = v8::Array::New(isolate, num);
    int32_t i;

    for (i = 0; i < num; i++)
        _funcs->Set(i, funcs[i]);

    return parallel(_funcs, -1, retVal);
}

result_t coroutine_base::parallel(v8::Local<v8::Array> datas, v8::Local<v8::Function> func,
    int32_t fibers, v8::Local<v8::Array>& retVal)
{
    _parallels _p;
    return _p.run(datas, func, retVal, fibers);
}

result_t coroutine_base::parallel(v8::Local<v8::Function> func, int32_t num,
    int32_t fibers, v8::Local<v8::Array>& retVal)
{
    v8::Isolate* isolate = Isolate::current()->m_isolate;
    v8::Local<v8::Array> datas = v8::Array::New(isolate, num);
    int32_t i;

    for (i = 0; i < num; i++)
        datas->Set(i, v8::Int32::New(isolate, i));

    _parallels _p;
    return _p.run(datas, func, retVal, fibers);
}

result_t coroutine_base::current(obj_ptr<Fiber_base>& retVal)
{
    Fiber_base* fb = JSFiber::current();

    if (!fb)
        return CALL_RETURN_NULL;

    retVal = fb;

    return 0;
}

result_t coroutine_base::sleep(int32_t ms, AsyncEvent* ac)
{
    class AcyncSleep : public exlib::Task_base {
    public:
        AcyncSleep(AsyncEvent* ac)
            : m_ac(ac)
        {
        }

    public:
        // exlib::Task_base
        virtual void suspend()
        {
        }

        virtual void suspend(exlib::spinlock& lock)
        {
            lock.unlock();
        }

        virtual void resume()
        {
            m_ac->post(0);
            delete this;
        }

    private:
        AsyncEvent* m_ac;
    };

    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    if (ms <= 0)
        return 0;

    exlib::Fiber::sleep(ms, new AcyncSleep(ac));
    return CALL_E_PENDDING;
}

result_t coroutine_base::get_fibers(v8::Local<v8::Array>& retVal)
{
    Isolate* isolate = Isolate::current();
    exlib::linkitem* p = isolate->m_fibers.head();
    int32_t n = 0;

    retVal = v8::Array::New(isolate->m_isolate);

    while (p) {
        retVal->Set(n++, ((JSFiber*)p)->wrap());
        p = p->m_next;
    }

    return 0;
}

result_t coroutine_base::get_spareFibers(int32_t& retVal)
{
    retVal = g_spareFibers;
    return 0;
}

result_t coroutine_base::set_spareFibers(int32_t newVal)
{
    g_spareFibers = newVal;
    return 0;
}

result_t coroutine_base::get_vmid(int32_t& retVal)
{
    Isolate* isolate = Isolate::current();

    retVal = isolate->m_id;
    return 0;
}

result_t coroutine_base::get_loglevel(int32_t& retVal)
{
    Isolate* isolate = Isolate::current();

    retVal = isolate->m_loglevel;
    return 0;
}

result_t coroutine_base::set_loglevel(int32_t newVal)
{
    Isolate* isolate = Isolate::current();

    isolate->m_loglevel = newVal;
    return 0;
}
}
