/*
 * process.cpp
 *
 *  Created on: Sep 19, 2012
 *      Author: lion
 */

#include "object.h"
#include "ifs/process.h"
#include "ifs/os.h"
#include "ifs/global.h"
#include "File.h"
#include "BufferedStream.h"
#include "SubProcess.h"
#include <vector>

#ifdef _WIN32
#include <psapi.h>
#include "utf8.h"

#define pclose _pclose
#endif

extern "C" char** environ;

namespace fibjs
{

DECLARE_MODULE(process);

static std::vector<char*> s_argv;
static std::vector<char*> s_start_argv;

void init_start_argv(int32_t argc, char **argv)
{
    int32_t i;

    s_start_argv.resize(argc);
    for (i = 0; i < argc; i ++)
        s_start_argv[i] = argv[i];
}

void init_argv(int32_t argc, char **argv)
{
    int32_t i, j;

    s_argv.resize(argc);
    for (i = 0; i < argc; i ++)
        s_argv[i] = argv[i];

    int32_t df = 0;

    for (i = 0; i < (int32_t)s_start_argv.size(); i ++)
    {
        char* arg = s_start_argv[i];

        if (df)
            s_start_argv[i - df] = arg;

        for (j = 0; j < argc; j ++)
            if (arg == argv[j])
            {
                df ++;
                break;
            }
    }

    if (df)
        s_start_argv.resize(s_start_argv.size() - df);
}

result_t process_base::get_argv(v8::Local<v8::Array> &retVal)
{
    Isolate* isolate = Isolate::current();
    v8::Local<v8::Array> args = v8::Array::New(isolate->m_isolate, (int32_t)s_argv.size());

    for (int32_t i = 0; i < (int32_t)s_argv.size(); i ++)
        args->Set(i, isolate->NewFromUtf8(s_argv[i]));

    retVal = args;

    return 0;
}

result_t process_base::get_execArgv(v8::Local<v8::Array>& retVal)
{
    Isolate* isolate = Isolate::current();
    v8::Local<v8::Array> args = v8::Array::New(isolate->m_isolate, (int32_t)s_start_argv.size());
    int32_t i;

    for (i = 0; i < (int32_t)s_start_argv.size(); i ++)
        args->Set(i, isolate->NewFromUtf8(s_start_argv[i]));

    retVal = args;

    return 0;
}

result_t process_base::get_execPath(exlib::string &retVal)
{
    return os_base::get_execPath(retVal);
}

result_t process_base::get_env(v8::Local<v8::Object>& retVal)
{
    Isolate* isolate = Isolate::current();

    if (isolate->m_env.IsEmpty())
    {
        v8::Local<v8::Object> o = v8::Object::New(isolate->m_isolate);
        char** env = environ;
        const char *p, *p1;

        while ((p = *env++) != NULL)
        {
            p1 = qstrchr(p, '=');
            if (p1)
                o->Set(isolate->NewFromUtf8(p, (int32_t)(p1 - p)), isolate->NewFromUtf8(p1 + 1));
        }

        isolate->m_env.Reset(isolate->m_isolate, o);
        retVal = o;
    } else
        retVal = v8::Local<v8::Object>::New(isolate->m_isolate, isolate->m_env);

    return 0;
}

void dump_memory();

result_t process_base::exit(int32_t code)
{
    flushLog(true);

#ifdef DEBUG
    global_base::GC();
    dump_memory();
#endif

    ::_exit(code);
    return 0;
}

result_t process_base::memoryUsage(v8::Local<v8::Object> &retVal)
{
    return os_base::memoryUsage(retVal);
}

result_t process_base::open(exlib::string command, v8::Local<v8::Array> args,
                            v8::Local<v8::Object> opts, obj_ptr<SubProcess_base>& retVal)
{
    return SubProcess::create(command, args, opts, true, retVal);
}

result_t process_base::open(exlib::string command, v8::Local<v8::Object> opts,
                            obj_ptr<SubProcess_base>& retVal)
{
    Isolate* isolate = Isolate::current();
    v8::Local<v8::Array> args = v8::Array::New(isolate->m_isolate);

    return open(command, args, opts, retVal);
}

result_t process_base::start(exlib::string command, v8::Local<v8::Array> args,
                             v8::Local<v8::Object> opts, obj_ptr<SubProcess_base>& retVal)
{
    return SubProcess::create(command, args, opts, false, retVal);
}

result_t process_base::start(exlib::string command, v8::Local<v8::Object> opts,
                             obj_ptr<SubProcess_base>& retVal)
{
    Isolate* isolate = Isolate::current();
    v8::Local<v8::Array> args = v8::Array::New(isolate->m_isolate);

    return start(command, args, opts, retVal);
}

result_t process_base::run(exlib::string command, v8::Local<v8::Array> args,
                           v8::Local<v8::Object> opts, int32_t& retVal)
{
    result_t hr;
    obj_ptr<SubProcess_base> _sub;

    hr = SubProcess::create(command, args, opts, false, _sub);
    if (hr < 0)
        return hr;

    return _sub->ac_wait(retVal);
}

result_t process_base::run(exlib::string command, v8::Local<v8::Object> opts,
                           int32_t& retVal)
{
    Isolate* isolate = Isolate::current();
    v8::Local<v8::Array> args = v8::Array::New(isolate->m_isolate);

    return run(command, args, opts, retVal);
}

}

