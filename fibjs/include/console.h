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
#include "File.h"

namespace fibjs
{

#define LOGTIME true

class logger : public AsyncEvent
{
public:
    class item : public exlib::linkitem
    {
    public:
        item(int32_t priority, qstring& msg) :
            m_priority(priority), m_msg(msg)
        {
            m_d.now();
        }

        qstring full(bool type = LOGTIME)
        {
            static const char *s_levels[] =
            {
                "FATAL  - ",
                "ALERT  - ",
                "CRIT   - ",
                "ERROR  - ",
                "WARN   - ",
                "NOTICE - ",
                "INFO   - ",
                "DEBUG  - ",
                "",
                "",
                ""
            };
            qstring s;
            if (type) {
                m_d.sqlString(s);
                s.append(" ", 1);
            }

            s.append(s_levels[m_priority]);
            s.append(m_msg);

            return s;
        }

    public:
        int32_t m_priority;
        qstring m_msg;
        date_t m_d;
    };

public:
    logger() : m_bWorking(false), m_bStop(false)
    {
        int32_t i;

        for (i = 0; i < console_base::_NOTSET; i ++)
            m_levels[i] = true;
    }

    virtual result_t config(Isolate* isolate, v8::Local<v8::Object> o)
    {
        int32_t i;
        result_t hr;
        v8::Local<v8::Array> levels;

        hr = GetConfigValue(isolate->m_isolate, o, "levels", levels);
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

            m_levels[console_base::_PRINT] = true;
        }

        return 0;
    }

    virtual int32_t post(int32_t v)
    {
        result_t hr = v;
        bool bStop = false;

        do
        {
            if (hr < 0)
            {
                m_lock.lock();
                m_bWorking = false;
                bStop = m_bStop;
                m_lock.unlock();

                break;
            }

            m_lock.lock();

            m_acLog.getList(m_workinglogs);
            if (m_workinglogs.empty()) {
                m_bWorking = false;
                bStop = m_bStop;
                m_lock.unlock();

                break;
            }

            m_lock.unlock();

            hr = write(this);
        } while (hr != CALL_E_PENDDING);

        if (bStop)
            destroy();

        return hr;
    }

    virtual void invoke()
    {
        post(0);
    }

public:
    virtual result_t write(AsyncEvent *ac) = 0;

    void log(int32_t priority, qstring& msg)
    {
        if (priority >= 0 && priority < console_base::_NOTSET && m_levels[priority])
        {
            item* i = new item(priority, msg);

            m_lock.lock();
            m_acLog.putTail(i);
            if (!m_bWorking)
            {
                m_bWorking = true;
                async(false);
            }
            m_lock.unlock();
        }
    }

    void flush(bool bFiber)
    {
        while (!m_acLog.empty() || m_bWorking)
            if (bFiber)
                coroutine_base::sleep(1);
            else
                exlib::OSThread::sleep(1);
    }

    void stop()
    {
        m_lock.lock();
        if (!m_bWorking)
        {
            destroy();
            return;
        }
        else
            m_bStop = true;
        m_lock.unlock();
    }

public:
    static qstring &notice()
    {
        return get_std_color()->m_notice;
    }

    static qstring &warn()
    {
        return get_std_color()->m_warn;
    }

    static qstring &error()
    {
        return get_std_color()->m_error;
    }

    static qstring &highLight()
    {
        return get_std_color()->m_highLight;
    }

    static TextColor *get_std_color();

protected:
    exlib::List<item> m_workinglogs;

    void destroy()
    {
        item* p1;
        while ((p1 = m_acLog.getHead()) != 0)
            delete p1;
        delete this;
    }

private:
    exlib::List<item> m_acLog;
    bool m_bWorking;
    bool m_bStop;
    exlib::spinlock m_lock;
    bool m_levels[console_base::_NOTSET];
};

class std_logger : public logger
{
public:
    virtual result_t write(AsyncEvent *ac);
    static void out(const char *txt);
};

class sys_logger : public logger
{
public:
    virtual result_t write(AsyncEvent *ac);
};

class stream_logger : public logger
{
public:
    stream_logger(Stream_base* out) : m_out(out)
    {}

public:
    virtual result_t write(AsyncEvent *ac);

    void close()
    {
        m_out->ac_close();
    }

private:
    obj_ptr<Stream_base> m_out;
};

class file_logger : public logger
{
public:
    virtual result_t config(Isolate* isolate, v8::Local<v8::Object> o);
    virtual result_t write(AsyncEvent *ac);

private:
    void clearFile();
    result_t initFile();

private:
    qstring m_path;
    qstring m_folder;
    int32_t m_split_mode;
    int64_t m_split_size;
    int32_t m_count;

    obj_ptr<File> m_file;
    int64_t m_size;
    date_t m_date;
};

}

#endif // _fj_CONSOLE_H
