/*
 * process.cpp
 *
 *  Created on: Sep 19, 2012
 *      Author: lion
 */

#include "ifs/process.h"
#include "ifs/os.h"
#include "File.h"
#include "BufferedStream.h"

#ifdef _WIN32
#include <psapi.h>
#include "utf8.h"

#define pclose _pclose
#endif

namespace fibjs
{

static obj_ptr<BufferedStream_base> s_err, s_in, s_out;

result_t process_base::get_stderr(obj_ptr<BufferedStream_base> &retVal)
{
    if (s_err == NULL)
    {
        s_err = new BufferedStream(new File(stderr, true));
        s_err->set_EOL("\n");
    }

    retVal = s_err;
    return 0;
}

result_t process_base::get_stdin(obj_ptr<BufferedStream_base> &retVal)
{
    if (s_in == NULL)
    {
        s_in = new BufferedStream(new File(stdin, true));
        s_in->set_EOL("\n");
    }

    retVal = s_in;
    return 0;
}

result_t process_base::get_stdout(obj_ptr<BufferedStream_base> &retVal)
{
    if (s_out == NULL)
    {
        s_out = new BufferedStream(new File(stdout, true));
        s_out->set_EOL("\n");
    }

    retVal = s_out;
    return 0;
}

static int s_argc;
static char **s_argv;

void init_argv(int argc, char **argv)
{
    s_argc = argc;
    s_argv = argv;
}

result_t process_base::get_argv(v8::Handle<v8::Array> &retVal)
{
    v8::Handle<v8::Array> args = v8::Array::New(isolate, s_argc);

    for (int i = 0; i < s_argc; i ++)
        args->Set(i, v8::String::NewFromUtf8(isolate, s_argv[i]));

    retVal = args;

    return 0;
}

result_t process_base::get_execPath(std::string &retVal)
{
    return os_base::get_execPath(retVal);
}

void flushLog();

result_t process_base::exit(int32_t code)
{
    flushLog();

    ::exit(code);
    return 0;
}

result_t process_base::memoryUsage(v8::Handle<v8::Object> &retVal)
{
    return os_base::memoryUsage(retVal);
}

result_t process_base::system(const char *cmd, int32_t &retVal,
                              exlib::AsyncEvent *ac)
{
    if (!ac)
        return CALL_E_NOSYNC;

#ifdef _WIN32
    retVal = ::_wsystem(UTF8_W(cmd));
#else
    retVal = ::system(cmd) >> 8;
#endif

    return 0;
}

result_t process_base::exec(const char *cmd,
                            obj_ptr<BufferedStream_base> &retVal, exlib::AsyncEvent *ac)
{
    if (!ac)
        return CALL_E_NOSYNC;

#ifdef _WIN32
    FILE *pPipe = _wpopen(UTF8_W(cmd), L"r");
#else
    FILE *pPipe = popen(cmd, "r");
#endif

    if (pPipe == NULL)
        return LastError();

    retVal = new BufferedStream(new File(pPipe, true));
    retVal->set_EOL("\n");

    return 0;
}

}

