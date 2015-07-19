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
#include "ifs/fs.h"

namespace fibjs
{

#define LOGTIME true

class logger : public obj_base,
    public exlib::OSThread
{
public:
    class item : public exlib::linkitem
    {
    public:
        item(int32_t priority, std::string& msg) :
            m_priority(priority), m_msg(msg)
        {
            m_d.now();

            if (exlib::Service::hasService())
            {
                v8::Local<v8::StackTrace> stackTrace = v8::StackTrace::CurrentStackTrace(
                        Isolate::now().isolate, 1, v8::StackTrace::kOverview);

                if (stackTrace->GetFrameCount() > 0)
                {
                    char numStr[64];
                    v8::Local<v8::StackFrame> f = stackTrace->GetFrame(0);
                    v8::String::Utf8Value filename(f->GetScriptName());

                    if (*filename)
                        m_source.append(*filename);
                    else
                        m_source.append("[eval]");

                    sprintf(numStr, ":%d", f->GetLineNumber());
                    m_source.append(numStr);
                }
            }
        }

        std::string full(bool type = LOGTIME)
        {
            static const char *s_levels[] =
            {
                " FATAL  - ",
                " ALERT  - ",
                " CRIT   - ",
                " ERROR  - ",
                " WARN   - ",
                " NOTICE - ",
                " INFO   - ",
                " DEBUG  - ",
                " ",
                " ",
                " "
            };
            std::string s;
            if (type) {
                m_d.sqlString(s);
                s.append(" ", 1);
            }

            s.append(m_source);

            s.append(s_levels[m_priority]);
            s.append(m_msg);

            return s;
        }

    public:
        int32_t m_priority;
        std::string m_msg;
        std::string m_source;
        date_t m_d;
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
        v8::Local<v8::Array> levels;

        hr = GetConfigValue(o, "levels", levels);
        if (hr == CALL_E_PARAMNOTOPTIONAL)
        {
        }
        else if (hr < 0)
            return hr;
        else
        {
            for (i = 0; i < console_base::_NOTSET; i ++)
                m_levels[i] = false;

            int32_t sz = levels->Length();

            for (i = 0; i < sz; i ++)
            {
                v8::Local<v8::Value> l = levels->Get(i);
                int32_t num;

                hr = GetArgumentValue(l, num);
                if (hr < 0)
                    return CHECK_ERROR(hr);

                if (num >= 0 && num < console_base::_NOTSET)
                    m_levels[num] = true;
                else
                    return CHECK_ERROR(Runtime::setError("console: too many logger."));
            }
        }

        return 0;
    }

    virtual void Run()
    {
        Runtime rt;
        DateCache dc;
        rt.m_pDateCache = &dc;

        Runtime::reg(&rt);

        exlib::List<item> logs;

        while (!m_bStop)
        {
            m_sem.Wait();

            m_logEmpty = false;

            m_acLog.getList(logs);
            if (!logs.empty())
                write(logs);

            m_logEmpty = true;
        }

        item *p1;
        m_acLog.getList(logs);

        while ((p1 = logs.getHead()) != 0)
            delete p1;

        Unref();
    }

public:
    virtual void write(exlib::List<item> &logs) = 0;

    void log(int priority, std::string& msg)
    {
        if (priority >= 0 && priority < console_base::_NOTSET && m_levels[priority])
        {
            m_acLog.put(new item(priority, msg));
            m_sem.Post();
        }
    }

    void flush(bool bFiber)
    {
        while (!m_acLog.empty() || !m_logEmpty)
            if (bFiber)
                coroutine_base::sleep(1);
            else
                exlib::OSThread::Sleep(1);
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
    exlib::lockfree<item> m_acLog;
    exlib::OSSemaphore m_sem;
    bool m_logEmpty;
    bool m_bStop;
    bool m_levels[console_base::_NOTSET];
};

class std_logger : public logger
{
public:
    virtual void write(exlib::List<item> &logs);
    static void out(const char *txt);
};

class sys_logger : public logger
{
public:
    virtual void write(exlib::List<item> &logs);
};

class file_logger : public logger
{
public:
    virtual result_t config(v8::Local<v8::Object> o);
    virtual void write(exlib::List<item> &logs);

private:
    void clearFile();
    result_t initFile();

private:
    std::string m_path;
    std::string m_folder;
    int32_t m_split_mode;
    int64_t m_split_size;
    int32_t m_count;

    obj_ptr<Stream_base> m_file;
    int64_t m_size;
    date_t m_date;
};

}

#endif // _fj_CONSOLE_H
