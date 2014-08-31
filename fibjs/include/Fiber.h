/*
 * Fiber.h
 *
 *  Created on: Jul 23, 2012
 *      Author: lion
 */

#include "ifs/coroutine.h"
#include "ifs/Fiber.h"
#include "Runtime.h"
#include "QuickArray.h"

#ifndef FIBER_H_
#define FIBER_H_

namespace fibjs
{

class FiberBase: public Fiber_base, asyncEvent
{
    FIBER_FREE();

public:
    FiberBase()
    {
        m_rt.m_pDateCache = &g_dc;
    }

    ~FiberBase()
    {
    }

    result_t join()
    {
        v8::Unlocker unlocker(isolate);
        m_quit.wait();

        return 0;
    }

    result_t get_caller(obj_ptr<Fiber_base> &retVal)
    {
        if (m_caller == NULL)
            return CALL_RETURN_NULL;

        retVal = m_caller;
        return 0;
    }

public:
    static void *fiber_proc(void *p);
    void start();

    Runtime &runtime()
    {
        return m_rt;
    }

public:
    exlib::Event m_quit;
    Runtime m_rt;
    static DateCache g_dc;
    obj_ptr<Fiber_base> m_caller;
};

class JSFiber: public FiberBase
{
public:
    class scope
    {
    public:
        scope(JSFiber *fb = NULL);
        ~scope();

        JSFiber *operator->()
        {
            return m_pFiber;
        }

    public:
        result_t m_hr;

    private:
        obj_ptr<JSFiber> m_pFiber;
        void *m_pNext;
        v8::TryCatch try_catch;
    };

public:
    JSFiber() :
        m_error(false)
    {
    }

    ~JSFiber()
    {
        clear();
    }

    static JSFiber *current();
    virtual void js_callback();

    template<typename T>
    void New(v8::Local<v8::Function> func, T &args, int nArgStart,
             int nArgCount)
    {
        int i;

        m_argv.resize(nArgCount - nArgStart);
        for (i = nArgStart; i < nArgCount; i++)
            m_argv[i - nArgStart].Reset(isolate, args[i]);
        m_func.Reset(isolate, func);

        start();
    }

    template<typename T>
    static result_t New(v8::Local<v8::Function> func,
                        const v8::FunctionCallbackInfo<v8::Value> &args, int nArgStart,
                        obj_ptr<T> &retVal)
    {
        obj_ptr<JSFiber> fb = new JSFiber();
        fb->New(func, args, nArgStart, args.Length());
        retVal = fb;

        return 0;
    }

    template<typename T>
    static result_t New(v8::Local<v8::Function> func,
                        v8::Local<v8::Value> *args, int argCount, obj_ptr<T> &retVal)
    {
        obj_ptr<JSFiber> fb = new JSFiber();
        fb->New(func, args, 0, argCount);
        retVal = fb;

        return 0;
    }

    static void call(v8::Local<v8::Function> func, v8::Local<v8::Value> *args,
                     int argCount, v8::Local<v8::Value> &retVal)
    {
        JSFiber *fb = (JSFiber *) current();

        if (fb)
            fb->callFunction1(func, args, argCount, retVal);
    }

    result_t get_result(v8::Local<v8::Value> &retVal)
    {
        if (m_result.IsEmpty())
            return CALL_RETURN_NULL;

        retVal = v8::Local<v8::Value>::New(isolate, m_result);
        return 0;
    }

    bool isError()
    {
        return m_error;
    }

    void clear()
    {
        size_t i;

        for (i = 0; i < m_argv.size(); i++)
            m_argv[i].Reset();
        m_argv.resize(0);

        m_func.Reset();
        m_result.Reset();
    }

private:
    void callFunction(v8::Local<v8::Value> &retVal);
    void callFunction1(v8::Local<v8::Function> func,
                       v8::Local<v8::Value> *args, int argCount,
                       v8::Local<v8::Value> &retVal);

private:
    v8::Persistent<v8::Function> m_func;
    QuickArray<v8::Persistent<v8::Value> > m_argv;
    v8::Persistent<v8::Value> m_result;
    bool m_error;
};

} /* namespace fibjs */
#endif /* FIBER_H_ */
