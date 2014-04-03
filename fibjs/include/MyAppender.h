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
            m_wLight = m_wAttr & FOREGROUND_INTENSITY;

            if (type == STD_ERROR_HANDLE)
                m_stream = stderr;
            else if (type == STD_OUTPUT_HANDLE)
                m_stream = stdout;
        }

        void _out(const std::wstring &s)
        {
            LPWSTR ptr = (LPWSTR) s.c_str();
            LPWSTR ptr2;

            while (ptr2 = (LPWSTR) qstrchr(ptr, L'\x1b'))
            {
                if (ptr2[1] == '[')
                {
                    ptr2[0] = 0;
                    fputws(ptr, m_stream);
                    fflush(m_stream);

                    ptr2 += 2;

                    while (true)
                    {
                        if (ptr2[0] == 'm')
                        {
                            m_Now = m_wAttr;
                            SetConsoleTextAttribute(m_handle, m_Now);
                            ptr2 ++;
                            break;
                        }

                        if (qisdigit(ptr2[0]))
                        {
                            if (ptr2[1] == 'm')
                            {
                                if (ptr2[0] == '0')
                                {
                                    m_Now = m_wAttr;
                                    SetConsoleTextAttribute(m_handle, m_Now);
                                }
                                ptr2 += 2;
                                break;
                            }

                            WORD mask, val;
                            WORD light = m_wLight;

                            if (ptr2[1] == ';')
                            {
                                if (ptr2[0] == '0')
                                    m_wLight = light = 0;
                                else if (ptr2[0] == '1')
                                    m_wLight = light = FOREGROUND_INTENSITY;
                                ptr2 += 2;
                            }

                            if (ptr2[0] == '3')
                            {
                                mask = 0xf0;
                                ptr2 ++;
                            }
                            else if (ptr2[0] == '4')
                            {
                                mask = 0x0f;
                                ptr2 ++;
                            }
                            else if (ptr2[0] == '9')
                            {
                                mask = 0xf0;
                                light |= FOREGROUND_INTENSITY;
                                ptr2 ++;
                            }
                            else if (ptr2[0] == '1' && ptr2[1] == '0')
                            {
                                mask = 0x0f;
                                light |= FOREGROUND_INTENSITY << 4;
                                ptr2 += 2;
                            }
                            else
                                break;

                            if (!qisdigit(ptr2[0]))
                                break;

                            val = ptr2[0] - '0';

                            if (val != 8)
                            {
                                if (val == 9)
                                {
                                    val = (m_wAttr & 0x0f) | (m_Now & 0xf0);

                                    m_Now = val | light;
                                    SetConsoleTextAttribute(m_handle, m_Now);
                                }
                                else
                                {
                                    val = (val & 2) | ((val & 1) ? 4 : 0)
                                          | ((val & 4) ? 1 : 0);

                                    if (mask == 0x0f)
                                        val <<= 4;

                                    m_Now = (m_Now & mask) | val | light;
                                    SetConsoleTextAttribute(m_handle, m_Now);
                                }
                            }

                            ptr2 ++;
                            if (ptr2[0] == 'm')
                            {
                                ptr2 ++;
                                break;
                            }
                        }
                    }
                }
                ptr = ptr2;
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
            s.append(L"\n", 1);
            _out(s);
        }

    private:
        HANDLE m_handle;
        FILE *m_stream;
        WORD m_wAttr, m_Now;
        WORD m_wLight;
    };

    class _outs
    {
    public:
        _outs()
        {
            err.init(STD_ERROR_HANDLE);
            out.init(STD_OUTPUT_HANDLE);
        }

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
        std::cout.flush();
    }

protected:
    void _append(const log4cpp::LoggingEvent &event)
    {
        if (event.priority < log4cpp::Priority::NOTICE)
        {
            std::cerr << COLOR_RED << event.message << COLOR_NORMAL << std::endl;
            std::cerr.flush();
        }
        else
        {
            std::cout << event.message << std::endl;
            std::cout.flush();
        }
    }
};

#endif

}

#endif /* MYAPPENDER_H_ */
