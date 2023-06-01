/*
 * console_std.cpp
 *
 *  Created on: Apr 25, 2014
 *      Author: lion
 */

#include "console.h"
#include "Buffer.h"
#include "UVStream.h"
#include "ifs/tty.h"
#include <stdio.h>

namespace fibjs {

#ifdef WIN32

inline bool is_atty(int32_t fd)
{
    bool _tty;
    tty_base::isatty(fd, _tty);
    return _tty;
}

void std_logger::out(exlib::string& txt, bool is_error)
{
    static bool s_tty_out = is_atty(_fileno(stdout));
    static bool s_tty_err = is_atty(_fileno(stderr));

    obj_ptr<Stream_base> out;
    Isolate* isolate = Isolate::main();

    if (is_error) {
        if (!s_tty_err) {
            fwrite(txt.c_str(), 1, txt.length(), stderr);
            return;
        }

        isolate->get_stderr(out);
    } else {
        if (!s_tty_out) {
            fwrite(txt.c_str(), 1, txt.length(), stdout);
            return;
        }

        isolate->get_stdout(out);
    }

    obj_ptr<Buffer_base> data = new Buffer(txt.c_str(), txt.length());
    out->cc_write(data);
}

#else

void std_logger::out(exlib::string& txt, bool is_error)
{
    obj_ptr<Stream_base> out;

    Isolate* isolate = Isolate::main();

    if (is_error) {
        isolate->get_stderr(out);
    } else {
        isolate->get_stdout(out);
    }

    obj_ptr<Buffer_base> data = new Buffer(txt.c_str(), txt.length());
    out->cc_write(data);
}

#endif

result_t std_logger::write(AsyncEvent* ac)
{
    item* p1;

    while ((p1 = m_workinglogs.getHead()) != 0) {
        exlib::string txt;

        if (p1->m_priority == console_base::C_NOTICE)
            txt = logger::notice() + p1->m_msg + COLOR_RESET + "\n";
        else if (p1->m_priority == console_base::C_WARN)
            txt = logger::warn() + p1->m_msg + COLOR_RESET + "\n";
        else if (p1->m_priority <= console_base::C_ERROR)
            txt = logger::error() + p1->m_msg + COLOR_RESET + "\n";
        else if (p1->m_priority == console_base::C_PRINT)
            txt = p1->m_msg;
        else
            txt = p1->m_msg + "\n";

        out(txt, p1->m_priority <= console_base::C_WARN);

        delete p1;
    }
    fflush(stdout);

    return 0;
}
}
