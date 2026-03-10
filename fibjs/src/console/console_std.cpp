/*
 * console_std.cpp
 *
 *  Created on: Apr 25, 2014
 *      Author: lion
 */

#include <exlib/include/osconfig.h>

#include "console.h"
#include "Buffer.h"
#include "UVStream.h"
#include "ifs/tty.h"
#include <stdio.h>

namespace fibjs {

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
    bool retVal;
    out->cc_write(data, retVal);
}

result_t std_logger::write(AsyncEvent* ac)
{
    return 0;
}

// Format a log item into text, same logic as write()
static exlib::string format_log_msg(int32_t priority, const exlib::string& msg)
{
    if (priority == console_base::C_NOTICE)
        return logger::notice() + msg + COLOR_RESET + "\n";
    else if (priority == console_base::C_WARN)
        return logger::warn() + msg + COLOR_RESET + "\n";
    else if (priority <= console_base::C_ERROR)
        return logger::error() + msg + COLOR_RESET + "\n";
    else if (priority == console_base::C_PRINT)
        return msg;
    else
        return msg + "\n";
}

void std_logger::putLog(int32_t priority, exlib::string& msg)
{
    bool is_error = (priority <= console_base::C_WARN);
    exlib::string txt = format_log_msg(priority, msg);

    return out(txt, is_error);
}

}
