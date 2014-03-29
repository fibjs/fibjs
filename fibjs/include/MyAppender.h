/*
 * MyAppender.h
 *
 *  Created on: Mar 29, 2014
 *      Author: lion
 */

#ifndef MYAPPENDER_H_
#define MYAPPENDER_H_

#include <log4cpp/ConsoleAppender.hh>
#include "utils.h"
#include "utf8.h"

namespace fibjs
{
#ifdef _WIN32

class MyAppender: public log4cpp::LayoutAppender
{
protected:
    class color_out
    {
    public:
        void init(DWORD type)
        {
            CONSOLE_SCREEN_BUFFER_INFO csbiInfo;

            m_handle = GetStdHandle(type);
            GetConsoleScreenBufferInfo(m_handle, &csbiInfo);
            m_Now = m_wAttr = csbiInfo.wAttributes;

            if (type == STD_ERROR_HANDLE)
                m_stream = stderr;
            else if (type == STD_OUTPUT_HANDLE)
                m_stream = stdout;
        }

        void _out(const std::wstring &s)
        {
            LPWSTR ptr = (LPWSTR) s.c_str();
            LPWSTR ptr1, ptr2;

            ptr1 = ptr;
            while (ptr2 = (LPWSTR) qstrchr(ptr1, L'\x1b'))
            {
                ptr1 = ptr2 + 1;

                if (ptr2[1] == '[')
                {
                    WORD mask, val;

                    if ( qisdigit(ptr2[2]) && ptr2[3] == 'm')
                    {
                        if (ptr2 > ptr)
                        {
                            ptr2[0] = 0;
                            fputws(ptr, m_stream);
                            fflush(m_stream);
                        }

                        ptr = ptr1 = ptr2 + 4;
                    }
                    else if ( ptr2[2] == '2' && qisdigit(ptr2[3]) && ptr2[4] == 'm')
                    {
                        if (ptr2 > ptr)
                        {
                            ptr2[0] = 0;
                            fputws(ptr, m_stream);
                            fflush(m_stream);
                        }

                        ptr = ptr1 = ptr2 + 5;
                    }
                    else if (ptr2[2] == '9' && ptr2[3] == '0' && ptr2[4] == 'm')
                    {
                        mask = 0xf0;
                        val = FOREGROUND_BLUE | FOREGROUND_GREEN
                              | FOREGROUND_RED;

                        if (ptr2 > ptr)
                        {
                            ptr2[0] = 0;
                            fputws(ptr, m_stream);
                            fflush(m_stream);
                        }

                        m_Now = (m_Now & mask) | val;
                        SetConsoleTextAttribute(m_handle, m_Now);

                        ptr = ptr1 = ptr2 + 5;
                    }
                    else if ((ptr2[2] == '3' || ptr2[2] == '4')
                             && qisdigit(ptr2[3])
                             && ptr2[4] == 'm')
                    {
                        if (ptr2[2] == '3')
                            mask = 0xf0;
                        else
                            mask = 0x0f;

                        val = ptr2[3] - '0';

                        if (val == 9)
                            val = (mask ^ 0xff) & m_wAttr;
                        else
                        {
                            val = (val & 2) | ((val & 1) ? 4 : 0)
                                  | ((val & 4) ? 1 : 0)
                                  | FOREGROUND_INTENSITY;

                            if (mask == 0x0f)
                                val <<= 4;
                        }

                        if (ptr2 > ptr)
                        {
                            ptr2[0] = 0;
                            fputws(ptr, m_stream);
                            fflush(m_stream);
                        }

                        m_Now = (m_Now & mask) | val;
                        SetConsoleTextAttribute(m_handle, m_Now);

                        ptr = ptr1 = ptr2 + 5;
                    }
                }
            }

            fputws(ptr, m_stream);
            fflush(m_stream);
        }

        void out(const std::string &message)
        {
            _out(UTF8_W(message));
        }

        void outline(const std::string &message)
        {
            std::wstring s = UTF8_W(message);
            s.append(L"\r\n", 2);
            _out(s);
        }

    private:
        HANDLE m_handle;
        FILE *m_stream;
        WORD m_wAttr, m_Now;
    };

    class _outs
    {
    public:
        color_out err, out;
    };

    static _outs &get_outs()
    {
        static _outs s_outs;
        return s_outs;
    }

public:
    MyAppender() :
        LayoutAppender("console"), m_outs(get_outs())
    {
        m_outs.err.init(STD_ERROR_HANDLE);
        m_outs.out.init(STD_OUTPUT_HANDLE);
    }

    virtual void close()
    {
    }

    void out(const char *txt)
    {
        m_outs.out.out(txt);
    }

protected:
    void _append(const log4cpp::LoggingEvent &event)
    {
        if (event.priority < log4cpp::Priority::NOTICE)
            m_outs.err.outline(COLOR_RED + event.message + COLOR_NORMAL);
        else
            m_outs.out.outline(event.message);
    }

private:
    _outs &m_outs;
};

#else

class MyAppender: public log4cpp::LayoutAppender
{
public:
    MyAppender() :
        LayoutAppender("console")
    {
    }

    virtual void close()
    {
    }

    void out(const char *txt)
    {
        std::cout << txt;
    }

protected:
    void _append(const log4cpp::LoggingEvent &event)
    {
        if (event.priority < log4cpp::Priority::NOTICE)
            std::cerr << COLOR_RED << event.message << COLOR_NORMAL << std::endl;
        else
            std::cout << event.message << std::endl;
    }
};

#endif

}

#endif /* MYAPPENDER_H_ */
