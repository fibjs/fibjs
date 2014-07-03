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
    logger() : m_logEmpty(false)
    {
        start();
    }

    virtual void Run()
    {
        logger::item *p1, *p2, *pn;

        while (1)
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

            write(pn);

            m_logEmpty = true;
        }
    }

public:
    virtual void write(item *pn) = 0;

    void log(int priority, std::string msg)
    {
        m_acLog.put(new item(priority, msg));
        m_sem.Post();
    }

    void flush()
    {
        while (!m_acLog.empty() || !m_logEmpty)
            coroutine_base::ac_sleep(1);
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

public:
    exlib::AsyncQueue m_acLog;
    exlib::OSSemaphore m_sem;
    bool m_logEmpty;
};

class std_logger : public logger
{
public:
    virtual void write(item *pn);

    static void out(const char *txt);
};

}

#endif // _fj_CONSOLE_H
