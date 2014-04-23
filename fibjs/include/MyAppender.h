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
#include "TextColor.h"

namespace fibjs
{
class MyAppender: public log4cpp::LayoutAppender
{
public:
    MyAppender() :
        LayoutAppender("console")
    {
        m_colors = new TextColor();
        m_colors->m_error = COLOR_LIGHTRED;
        m_colors->m_warn = COLOR_YELLOW;
        m_colors->m_notice = COLOR_GREEN;
        m_colors->m_highLight = COLOR_TITLE;
    }

    virtual void close()
    {
    }

#ifdef _WIN32

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
                            m_wLight = m_wAttr & FOREGROUND_INTENSITY;
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
                                    m_wLight = m_wAttr & FOREGROUND_INTENSITY;
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
            out.init(STD_OUTPUT_HANDLE);
        }

    public:
        color_out out;
    };

public:
    void out(const char *txt)
    {
        m_outs.out.out(txt);
    }

protected:
    void _append(const log4cpp::LoggingEvent &event)
    {
        if (event.priority == log4cpp::Priority::NOTICE)
            m_outs.out.outline(m_colors->m_notice + event.message + COLOR_RESET);
        else if (event.priority == log4cpp::Priority::WARN)
            m_outs.out.outline(m_colors->m_warn + event.message + COLOR_RESET);
        else if (event.priority <= log4cpp::Priority::ERROR)
            m_outs.out.outline(m_colors->m_error + event.message + COLOR_RESET);
        else
            m_outs.out.outline(event.message);
    }

private:
    _outs m_outs;

#else

public:
    void out(const char *txt)
    {
        fputs(txt, stdout);
        fflush(stdout);
    }

protected:
    void _append(const log4cpp::LoggingEvent &event)
    {
        std::string txt;
        if (event.priority == log4cpp::Priority::NOTICE)
            txt = m_colors->m_notice + event.message + COLOR_RESET + "\n";
        else if (event.priority == log4cpp::Priority::WARN)
            txt = m_colors->m_warn + event.message + COLOR_RESET + "\n";
        else if (event.priority <= log4cpp::Priority::ERROR)
            txt = m_colors->m_error + event.message + COLOR_RESET + "\n";
        else
            txt = event.message + "\n";

        fputs(txt.c_str(), stdout);
        fflush(stdout);
    }
#endif

public:
    static MyAppender *getter()
    {
        static MyAppender *s_ma = NULL;
        if (!s_ma)
            s_ma = new MyAppender();
        return s_ma;
    }

    static log4cpp::Appender *getter(const std::string &appenderName)
    {
        return getter();
    }

    static std::string &notice()
    {
        return getter()->m_colors->m_notice;
    }

    static std::string &warn()
    {
        return getter()->m_colors->m_warn;
    }

    static std::string &error()
    {
        return getter()->m_colors->m_error;
    }

    static std::string &highLight()
    {
        return getter()->m_colors->m_highLight;
    }

public:
    obj_ptr<TextColor> m_colors;
};

}

#endif /* MYAPPENDER_H_ */
