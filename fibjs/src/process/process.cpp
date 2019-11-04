/*
 * process.cpp
 *
 *  Created on: Sep 19, 2012
 *      Author: lion
 */

#include "object.h"
#include "version.h"
#include "exlib/include/hrtime.h"
#include "ifs/process.h"
#include "ifs/os.h"
#include "ifs/global.h"
#include "ifs/util.h"
#include "Fiber.h"
#include "EventEmitter.h"
#include "File.h"
#include "BufferedStream.h"
#include "SubProcess.h"
#include <vector>
#include "options.h"

#ifdef _WIN32
#include <psapi.h>
#include "utf8.h"
#include "process_win.h"
#else

#include <unistd.h>
#include "editline/include/editline.h"

inline int32_t _umask(int32_t m)
{
    return ::umask(m);
}

#endif

extern "C" char** environ;

namespace fibjs {

DECLARE_MODULE(process);

static std::vector<char*> s_argv;
static std::vector<char*> s_start_argv;

void init_start_argv(int32_t argc, char** argv)
{
    int32_t i;

    s_start_argv.resize(argc);
    for (i = 0; i < argc; i++)
        s_start_argv[i] = argv[i];
}

void init_argv(int32_t argc, char** argv)
{
    int32_t i, j;

    s_argv.resize(argc);
    for (i = 0; i < argc; i++)
        s_argv[i] = argv[i];

    int32_t df = 0;

    for (i = 0; i < (int32_t)s_start_argv.size(); i++) {
        char* arg = s_start_argv[i];

        if (df)
            s_start_argv[i - df] = arg;

        for (j = 0; j < argc; j++)
            if (arg == argv[j]) {
                df++;
                break;
            }
    }

    if (df)
        s_start_argv.resize(s_start_argv.size() - df);
}

result_t process_base::get_argv(v8::Local<v8::Array>& retVal)
{
    Isolate* isolate = Isolate::current();
    v8::Local<v8::Array> args = v8::Array::New(isolate->m_isolate, (int32_t)s_argv.size());

    for (int32_t i = 0; i < (int32_t)s_argv.size(); i++)
        args->Set(i, isolate->NewString(s_argv[i]));

    retVal = args;

    return 0;
}

result_t process_base::get_execArgv(v8::Local<v8::Array>& retVal)
{
    Isolate* isolate = Isolate::current();
    v8::Local<v8::Array> args = v8::Array::New(isolate->m_isolate, (int32_t)s_start_argv.size());
    int32_t i;

    for (i = 0; i < (int32_t)s_start_argv.size(); i++)
        args->Set(i, isolate->NewString(s_start_argv[i]));

    retVal = args;

    return 0;
}

result_t process_base::get_version(exlib::string& retVal)
{
    retVal = fibjs_version;
    return 0;
}

result_t process_base::get_versions(v8::Local<v8::Object>& retVal)
{
    return util_base::buildInfo(retVal);
}

result_t process_base::get_exitCode(int32_t& retVal)
{
    retVal = Isolate::current()->m_exitCode;
    return 0;
}

result_t process_base::set_exitCode(int32_t newVal)
{
    Isolate::current()->m_exitCode = newVal;
    return 0;
}

result_t process_base::umask(int32_t mask, int32_t& retVal)
{
    retVal = _umask(mask);
    return 0;
}

result_t process_base::umask(exlib::string mask, int32_t& retVal)
{
    int oct = 0;
    // Parse the octal string.
    for (size_t i = 0; i < mask.length(); i++) {
        char c = mask[i];

        if (c > '7' || c < '0')
            return CHECK_ERROR(Runtime::setError("process: invalid octal string"));

        oct *= 8;
        oct += c - '0';
    }

    retVal = _umask(oct);
    return 0;
}

result_t process_base::umask(int32_t& retVal)
{
    int32_t old = _umask(0);
    retVal = old;
    _umask(old);
    return 0;
}

result_t process_base::hrtime(v8::Local<v8::Array> diff, v8::Local<v8::Array>& retVal)
{
    uint64_t t = exlib::_hrtime();

    if (diff->Length() == 2) {
        uint64_t seconds = JSValue(diff->Get(0))->Uint32Value();
        uint64_t nanos = JSValue(diff->Get(1))->Uint32Value();
        t -= (seconds * NANOS_PER_SEC) + nanos;
    }

    Isolate* isolate = Isolate::current();

    v8::Local<v8::Array> tuple = v8::Array::New(isolate->m_isolate, 2);
    tuple->Set(0, v8::Integer::NewFromUnsigned(isolate->m_isolate, (uint32_t)(t / NANOS_PER_SEC)));
    tuple->Set(1, v8::Integer::NewFromUnsigned(isolate->m_isolate, t % NANOS_PER_SEC));

    retVal = tuple;

    return 0;
}

result_t process_base::get_execPath(exlib::string& retVal)
{
    return os_base::get_execPath(retVal);
}

result_t process_base::get_env(v8::Local<v8::Object>& retVal)
{
    Isolate* isolate = Isolate::current();

    if (isolate->m_env.IsEmpty()) {
        v8::Local<v8::Object> o = v8::Object::New(isolate->m_isolate);
        char** env = environ;
        const char *p, *p1;

        while ((p = *env++) != NULL) {
            p1 = qstrchr(p, '=');
            if (p1)
                o->Set(isolate->NewString(p, (int32_t)(p1 - p)), isolate->NewString(p1 + 1));
        }

        isolate->m_env.Reset(isolate->m_isolate, o);
        retVal = o;
    } else
        retVal = v8::Local<v8::Object>::New(isolate->m_isolate, isolate->m_env);

    return 0;
}

result_t process_base::get_arch(exlib::string& retVal)
{
    return os_base::arch(retVal);
}

result_t process_base::get_platform(exlib::string& retVal)
{
    return os_base::platform(retVal);
}

result_t process_base::get_pid(int32_t& retVal)
{
#ifdef _WIN32
    retVal = GetCurrentProcessId();
#else
    retVal = getpid();
#endif

    return 0;
}

result_t process_base::get_ppid(int32_t& retVal)
{
#ifdef _WIN32
    retVal = (int32_t)GetParentProcessID();
#else
    retVal = getppid();
#endif

    return 0;
}

result_t process_base::get_stdin(obj_ptr<File_base>& retVal)
{
    Isolate* isolate = Isolate::current();

    if (!isolate->m_stdin)
        isolate->m_stdin = new File(stdin);
    retVal = isolate->m_stdin;

    return 0;
}

result_t process_base::get_stdout(obj_ptr<File_base>& retVal)
{
    Isolate* isolate = Isolate::current();

    if (!isolate->m_stdout)
        isolate->m_stdout = new File(stdout);
    retVal = isolate->m_stdout;

    return 0;
}

result_t process_base::get_stderr(obj_ptr<File_base>& retVal)
{
    Isolate* isolate = Isolate::current();

    if (!isolate->m_stderr)
        isolate->m_stderr = new File(stderr);
    retVal = isolate->m_stderr;

    return 0;
}

extern bool g_in_readline;

result_t process_base::exit()
{
    Isolate* isolate = Isolate::current();
    int32_t code = isolate->m_exitCode;

    JSTrigger t(isolate->m_isolate, class_info().getModule(isolate));
    v8::Local<v8::Value> v = v8::Number::New(isolate->m_isolate, code);
    bool r;

    isolate->Ref();
    t._emit("exit", &v, 1, r);

    flushLog();

    if (g_cov != nullptr && isolate->m_id == 1) {
        WriteLcovData(isolate->m_isolate, g_cov);
    }

#ifdef _WIN32
    TerminateProcess(GetCurrentProcess(), code);
#else
    if (g_in_readline && isatty(fileno(stdin)))
        rl_deprep_terminal();
    ::_exit(code);
#endif

    return 0;
}

result_t process_base::exit(int32_t code)
{
    Isolate* isolate = Isolate::current();
    isolate->m_exitCode = code;
    return process_base::exit();
}

result_t process_base::memoryUsage(v8::Local<v8::Object>& retVal)
{
    return os_base::memoryUsage(retVal);
}

result_t process_base::uptime(double& retVal)
{
    return os_base::uptime(retVal);
}

result_t process_base::nextTick(v8::Local<v8::Function> func, OptArgs args)
{
    obj_ptr<Fiber_base> retVal;
    return JSFiber::New(func, args, retVal);
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
    v8::Local<v8::Object> opts, int32_t& retVal, AsyncEvent* ac)
{
    obj_ptr<SubProcess_base> _sub;

    if (ac->isSync()) {
        result_t hr;

        hr = SubProcess::create(command, args, opts, false, _sub);
        if (hr < 0)
            return hr;

        ac->m_ctx.resize(1);
        ac->m_ctx[0] = _sub;

        return CHECK_ERROR(CALL_E_NOSYNC);
    }

    _sub = SubProcess_base::getInstance(ac->m_ctx[0].object());
    return _sub->wait(retVal, ac);
}

result_t process_base::run(exlib::string command, v8::Local<v8::Object> opts,
    int32_t& retVal, AsyncEvent* ac)
{
    v8::Local<v8::Array> args;

    if (ac->isSync()) {
        Isolate* isolate = Isolate::current();
        args = v8::Array::New(isolate->m_isolate);
    }

    return run(command, args, opts, retVal, ac);
}
}
