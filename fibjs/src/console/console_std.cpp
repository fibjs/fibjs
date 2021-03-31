/*
 * console_std.cpp
 *
 *  Created on: Apr 25, 2014
 *      Author: lion
 */

#include "console.h"
#include "Buffer.h"
#include "UVStream.h"

namespace fibjs {

extern exlib::LockedList<Isolate> s_isolates;
void std_logger::out(exlib::string& txt, bool is_error)
{
    obj_ptr<Stream_base> out;

    Isolate* isolate = s_isolates.head();

    if (is_error) {
        if (!isolate->m_stderr)
            isolate->m_stderr = new UVStream(_fileno(stderr));
        out = isolate->m_stderr;
    } else {
        if (!isolate->m_stdout)
            isolate->m_stdout = new UVStream(_fileno(stdout));
        out = isolate->m_stdout;
    }

    obj_ptr<Buffer_base> data = new Buffer(txt);
    out->cc_write(data);
}

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
