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

static int s_argc;
static char **s_argv;

void init_argv(int argc, char **argv)
{
    s_argc = argc;
    s_argv = argv;
}

result_t process_base::get_argv(v8::Local<v8::Array> &retVal)
{
    Isolate &isolate = Isolate::now();
    v8::Local<v8::Array> args = v8::Array::New(isolate.isolate, s_argc);

    for (int i = 0; i < s_argc; i ++)
        args->Set(i, v8::String::NewFromUtf8(isolate.isolate, s_argv[i]));

    retVal = args;

    return 0;
}

result_t process_base::get_execPath(std::string &retVal)
{
    return os_base::get_execPath(retVal);
}

result_t process_base::exit(int32_t code)
{
    flushLog(false);

    ::_exit(code);
    return 0;
}

result_t process_base::memoryUsage(v8::Local<v8::Object> &retVal)
{
    return os_base::memoryUsage(retVal);
}

result_t process_base::system(const char *cmd, int32_t &retVal,
                              exlib::AsyncEvent *ac)
{
    if (switchToAsync(ac))
        return CHECK_ERROR(CALL_E_NOSYNC);

#ifdef _WIN32
    retVal = ::_wsystem(UTF8_W(cmd));
#else
    retVal = ::system(cmd) >> 8;
#endif

    return 0;
}

result_t process_base::popen(const char *cmd,
                             obj_ptr<BufferedStream_base> &retVal, exlib::AsyncEvent *ac)
{
    if (switchToAsync(ac))
        return CHECK_ERROR(CALL_E_NOSYNC);

#ifdef _WIN32
    FILE *pPipe = _wpopen(UTF8_W(cmd), L"r");
#else
    FILE *pPipe = ::popen(cmd, "r");
#endif

    if (pPipe == NULL)
        return CHECK_ERROR(LastError());

    retVal = new BufferedStream(new File(pPipe));
    retVal->set_EOL("\n");

    return 0;
}

result_t process_base::exec(const char *cmd)
{
#ifdef _WIN32
    PROCESS_INFORMATION pi;
    STARTUPINFOW si;

    ZeroMemory(&si, sizeof(STARTUPINFO));
    si.cb = sizeof(STARTUPINFO);

    wstring wstr(L"cmd /C ");
    wstr.append(utf8to16String(cmd));

    CreateProcessW(NULL, &wstr[0], NULL, NULL, 0, 0, NULL, NULL, &si, &pi);
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
#else
    if (fork() == 0)
    {
        execl("/bin/sh", "sh", "-c", cmd, (char *)0);
        _exit(127);
    }
#endif
    return 0;
}

}

