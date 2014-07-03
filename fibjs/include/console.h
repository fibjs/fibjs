/*
 * console.h
 *
 *  Created on: Apr 25, 2014
 *      Author: lion
 */

#ifndef _fj_CONSOLE_H
#define _fj_CONSOLE_H

#include "utils.h"
#include "TextColor.h"
#include "ifs/console.h"
#include "ifs/coroutine.h"

namespace fibjs
{

class logger : public obj_base, exlib::OSThread
{
public:
    class item : public asyncEvent
    {
    public:
        item(int32_t priority, std::string msg) :
            m_priority(priority), m_msg(msg)
        {
        }

    public:
        int32_t m_priority;
        std::string m_msg;
    };

public:
    logger() : m_logEmpty(true), m_bStop(false)
    {
        int32_t i;

        for (i = 0; i < console_base::_NOTSET; i ++)
            m_levels[i] = true;

        Ref();
        start();
    }

    virtual result_t config(v8::Local<v8::Object> o)
    {
        int32_t i;
        result_t hr;

        v8::Local<v8::Value> v = o->Get(v8::String::NewFromUtf8(isolate, "levels",
                                        v8::String::kNormalString, 6));

        if (v->IsArray())
        {
            for (i = 0; i < console_base::_NOTSET; i ++)
                m_levels[i] = false;

            v8::Local<v8::Array> levels = v8::Local<v8::Array>::Cast(v);
            int32_t sz = levels->Length();

            for (i = 0; i < sz; i ++)
            {
                v8::Local<v8::Value> l = levels->Get(i);
                int32_t num;

                hr = GetArgumentValue(l, num);
                if (hr < 0)
                    return hr;

                if (num >= 0 && num < console_base::_NOTSET)
                    m_levels[num] = true;
                else
                    return CALL_E_INVALIDARG;
            }
        }
        else
        {
            for (i = 0; i < console_base::_NOTSET; i ++)
                m_levels[i] = true;
        }

        return 0;
    }

    virtual void Run()
    {
        logger::item *p1, *p2, *pn;

        while (!m_bStop)
        {
            m_sem.Wait();

            m_logEmpty = false;

            p1 = (logger::item *)m_acLog.getList();
            pn = NULL;

            while (p1)
            {
                p2 = p1;
                p1 = (logger::item *) p1->m_next;
                p2->m_next = pn;
                pn = p2;
            }

            if (pn)
                write(pn);

            m_logEmpty = true;
        }

        Unref();
    }

public:
    virtual void write(item *pn) = 0;

    void log(int priority, std::string msg)
    {
        if (priority >= 0 && priority < console_base::_NOTSET && m_levels[priority])
        {
            m_acLog.put(new item(priority, msg));
            m_sem.Post();
        }
    }

    void flush()
    {
        while (!m_acLog.empty() || !m_logEmpty)
            coroutine_base::ac_sleep(1);
    }

    void stop()
    {
        m_bStop = true;
        m_sem.Post();
    }

public:
    static std::string &notice()
    {
        return get_std_color()->m_notice;
    }

    static std::string &warn()
    {
        return get_std_color()->m_warn;
    }

    static std::string &error()
    {
        return get_std_color()->m_error;
    }

    static std::string &highLight()
    {
        return get_std_color()->m_highLight;
    }

    static TextColor *get_std_color();

private:
    exlib::AsyncQueue m_acLog;
    exlib::OSSemaphore m_sem;
    bool m_logEmpty;
    bool m_bStop;
    bool m_levels[console_base::_NOTSET];
};

class std_logger : public logger
{
public:
    virtual void write(item *pn);
    static void out(const char *txt);
};

class sys_logger : public logger
{
public:
    virtual void write(item *pn);
};

}

#endif // _fj_CONSOLE_H
