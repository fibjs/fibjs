/*
 * console_std.cpp
 *
 *  Created on: Apr 25, 2014
 *      Author: lion
 */

#include "console.h"
#include "utf8.h"
#include <stdio.h>

namespace fibjs {

TextColor* logger::get_std_color()
{
    static obj_ptr<TextColor> s_tc;

    if (!s_tc)
        s_tc = new TextColor;

    return s_tc;
}

#ifdef _WIN32

void std_logger::out(exlib::string& txt)
{
    class color_out {
    public:
        color_out()
        {
            m_tty = _isatty(_fileno(stdout)) != FALSE;

            if (!m_tty) {
                fpos_t pos;
                fgetpos(stdout, &pos);
                m_tty = pos < 0;
            }

            if (!m_tty) {
                m_handle = GetStdHandle(STD_OUTPUT_HANDLE);
                m_Now = m_wAttr = 0x7;
                m_wLight = m_wAttr & FOREGROUND_INTENSITY;
            }
        }

        void WriteConsole(exlib::wchar* ptr, size_t sz)
        {
            DWORD dwWrite;

            while (sz) {
                size_t sz1 = sz;
                if (sz1 >= 16384)
                    sz1 = 16384;
                WriteConsoleW(m_handle, ptr, (DWORD)sz1, &dwWrite, NULL);
                ptr += sz1;
                sz -= sz1;
            }
        }

        void out(exlib::string& s)
        {
            static HANDLE s_console;

            if (!m_tty) {
                fwrite(s.c_str(), 1, s.length(), stdout);
                return;
            }

            if (!m_handle) {
                if (!s_console) {
                    AllocConsole();

                    freopen("CONIN$", "r", stdin);
                    freopen("CONOUT$", "w", stdout);
                    freopen("CONOUT$", "w", stderr);

                    s_console = GetStdHandle(STD_OUTPUT_HANDLE);
                }

                m_handle = s_console;
                m_Now = m_wAttr = 0x7;
                m_wLight = m_wAttr & FOREGROUND_INTENSITY;
            }

            exlib::wstring ws = utf8to16String(s);
            exlib::wchar* ptr = &ws[0];
            exlib::wchar* pend = ptr + ws.length();
            exlib::wchar* ptr2;

            while (ptr2 = (exlib::wchar*)qstrchr(ptr, L'\x1b')) {
                if (ptr2[1] == '[') {
                    WriteConsole(ptr, ptr2 - ptr);

                    ptr2 += 2;

                    while (true) {
                        if (ptr2[0] == 'm') {
                            m_Now = m_wAttr;
                            m_wLight = m_wAttr & FOREGROUND_INTENSITY;
                            SetConsoleTextAttribute(m_handle, m_Now);
                            ptr2++;
                            break;
                        }

                        if (qisdigit(ptr2[0])) {
                            if (ptr2[1] == 'm') {
                                if (ptr2[0] == '0') {
                                    m_Now = m_wAttr;
                                    m_wLight = m_wAttr & FOREGROUND_INTENSITY;
                                    SetConsoleTextAttribute(m_handle, m_Now);
                                }
                                ptr2 += 2;
                                break;
                            }

                            WORD mask, val;
                            WORD light = m_wLight;

                            if (ptr2[1] == ';') {
                                if (ptr2[0] == '0')
                                    m_wLight = light = 0;
                                else if (ptr2[0] == '1')
                                    m_wLight = light = FOREGROUND_INTENSITY;
                                ptr2 += 2;
                            }

                            if (ptr2[0] == '3') {
                                mask = 0xf0;
                                ptr2++;
                            } else if (ptr2[0] == '4') {
                                mask = 0x0f;
                                ptr2++;
                            } else if (ptr2[0] == '9') {
                                mask = 0xf0;
                                light |= FOREGROUND_INTENSITY;
                                ptr2++;
                            } else if (ptr2[0] == '1' && ptr2[1] == '0') {
                                mask = 0x0f;
                                light |= FOREGROUND_INTENSITY << 4;
                                ptr2 += 2;
                            } else
                                break;

                            if (!qisdigit(ptr2[0]))
                                break;

                            val = ptr2[0] - '0';

                            if (val != 8) {
                                if (val == 9) {
                                    val = (m_wAttr & 0x0f) | (m_Now & 0xf0);

                                    m_Now = val | light;
                                    SetConsoleTextAttribute(m_handle, m_Now);
                                } else {
                                    val = (val & 2) | ((val & 1) ? 4 : 0)
                                        | ((val & 4) ? 1 : 0);

                                    if (mask == 0x0f)
                                        val <<= 4;

                                    m_Now = (m_Now & mask) | val | light;
                                    SetConsoleTextAttribute(m_handle, m_Now);
                                }
                            }

                            ptr2++;
                            if (ptr2[0] == 'm') {
                                ptr2++;
                                break;
                            }
                        }
                    }
                }
                ptr = ptr2;
            }

            WriteConsole(ptr, pend - ptr);
        }

    private:
        HANDLE m_handle;
        WORD m_wAttr, m_Now;
        WORD m_wLight;
        bool m_tty;
    };

    static color_out s_out;
    s_out.out(txt);
}

#else

void std_logger::out(exlib::string& txt)
{
    fwrite(txt.c_str(), 1, txt.length(), stdout);
}

#endif

result_t std_logger::write(AsyncEvent* ac)
{
    item* p1;

    while ((p1 = m_workinglogs.getHead()) != 0) {
        exlib::string txt;
        if (p1->m_priority == console_base::_NOTICE)
            txt = logger::notice() + p1->m_msg + COLOR_RESET + "\n";
        else if (p1->m_priority == console_base::_WARN)
            txt = logger::warn() + p1->m_msg + COLOR_RESET + "\n";
        else if (p1->m_priority <= console_base::_ERROR)
            txt = logger::error() + p1->m_msg + COLOR_RESET + "\n";
        else if (p1->m_priority == console_base::_PRINT)
            txt = p1->m_msg;
        else
            txt = p1->m_msg + "\n";

        out(txt);

        delete p1;
    }
    fflush(stdout);

    return 0;
}
}
