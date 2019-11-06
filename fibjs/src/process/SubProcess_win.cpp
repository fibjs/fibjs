/*
 * SubProcess_win.cpp
 *
 *  Created on: Feb 11, 2016
 *      Author: lion
 */

#ifdef _WIN32

#include "object.h"
#include "ifs/process.h"
#include "ifs/util.h"
#include "SubProcess.h"
#include <vector>
#include <psapi.h>
#include <tlhelp32.h>
#include "utf8.h"

namespace fibjs {

static const char* DEFT_ENV_KEYS[] = {
    "SYSTEMROOT",
    "SystemRoot",
    "TEMP",
    "TMP",
    // for registry :start
    "CommonProgramFiles",
    "CommonProgramFiles(x86)",
    "CommonProgramW6432",
    "ProgramFiles",
    "ProgramFiles(x86)",
    "ProgramW6432"
    // for registry :end
};

void init_signal();

void init_process()
{
    init_signal();
}

class PSTimer : public Timer {
public:
    PSTimer(int32_t timeout, HANDLE pid)
        : Timer(timeout)
        , m_pid(pid)
    {
    }

public:
    virtual void on_timer()
    {
        TerminateProcess(m_pid, -1);
    }

private:
    HANDLE m_pid;
};

result_t create_name_pipe(HANDLE* hd1, HANDLE* hd2, bool in)
{
    HANDLE hMine, hChild;
    char fname[128];

    do {
        sprintf(fname, "\\\\.\\pipe\\fibs.namedpipe.%d", rand());
        hMine = CreateNamedPipe(fname, (in ? PIPE_ACCESS_OUTBOUND : PIPE_ACCESS_INBOUND) | FILE_FLAG_OVERLAPPED,
            PIPE_TYPE_BYTE | PIPE_READMODE_BYTE | PIPE_WAIT,
            1, 4096, 4096, 0, NULL);
    } while ((hMine == INVALID_HANDLE_VALUE) && (GetLastError() == ERROR_PIPE_BUSY));
    if (hMine == INVALID_HANDLE_VALUE)
        return CHECK_ERROR(LastError());

    SECURITY_ATTRIBUTES sa;
    sa.nLength = sizeof(SECURITY_ATTRIBUTES);
    sa.bInheritHandle = TRUE;
    sa.lpSecurityDescriptor = NULL;

    hChild = CreateFile(fname, in ? GENERIC_READ : GENERIC_WRITE, 0, &sa, OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL, 0);
    if (hChild == INVALID_HANDLE_VALUE) {
        CloseHandle(hMine);
        return CHECK_ERROR(LastError());
    }

    *hd1 = hMine;
    *hd2 = hChild;

    return 0;
}

result_t SubProcess::create(exlib::string command, v8::Local<v8::Array> args, v8::Local<v8::Object> opts,
    bool redirect, obj_ptr<SubProcess_base>& retVal)
{
    result_t hr;
    PROCESS_INFORMATION pi;
    STARTUPINFOW si;
    int32_t len = args->Length();
    int32_t i;
    HANDLE cin_pipe[2] = { 0 };
    HANDLE cout_pipe[2] = { 0 };
    int32_t timeout;

    exlib::wstring wstr;
    wstr.append(1, '\"');
    wstr.append(utf8to16String(command));
    wstr.append(1, '\"');

    Isolate* isolate = Isolate::current();

    hr = GetConfigValue(isolate->m_isolate, opts, "timeout", timeout);
    if (hr == CALL_E_PARAMNOTOPTIONAL)
        timeout = 0;
    else if (hr < 0)
        return CHECK_ERROR(hr);

    for (i = 0; i < len; i++) {
        exlib::string str;

        hr = GetArgumentValue(isolate->m_isolate, args->Get(i), str);
        if (hr < 0)
            return hr;

        wstr.append(1, ' ');
        wstr.append(utf8to16String(str));
    }

    ZeroMemory(&si, sizeof(STARTUPINFO));
    si.cb = sizeof(STARTUPINFO);

    if (redirect) {
        hr = create_name_pipe(&cin_pipe[0], &cin_pipe[1], true);
        if (hr < 0)
            return hr;

        hr = create_name_pipe(&cout_pipe[0], &cout_pipe[1], false);
        if (hr < 0) {
            ::CloseHandle(cin_pipe[0]);
            ::CloseHandle(cin_pipe[1]);

            return hr;
        }

        si.hStdError = GetStdHandle(STD_ERROR_HANDLE);
        si.hStdOutput = cout_pipe[1];
        si.hStdInput = cin_pipe[1];
        si.dwFlags |= STARTF_USESTDHANDLES;
    }

    v8::Local<v8::Object> cur_envs;
    hr = process_base::get_env(cur_envs);
    if (hr < 0)
        return hr;

    v8::Local<v8::Value> opt_envs_v;
    hr = GetConfigValue(isolate->m_isolate, opts, "env", opt_envs_v, true);
    if (hr == CALL_E_PARAMNOTOPTIONAL)
        util_base::clone(cur_envs, opt_envs_v);
    else if (hr < 0)
        return hr;

    v8::Local<v8::Object> opt_envs = opt_envs_v->ToObject();

    JSArray cur_keys = cur_envs->GetPropertyNames();
    len = (int32_t)cur_keys->Length();
    bool has_reverse_key;
    exlib::string envs_k;
    v8::Local<v8::String> js_envs_k;
    v8::Local<v8::Value> js_v_in_cur;

    for (int32_t i = 0, reverse_key_len = (int32_t)ARRAYSIZE(DEFT_ENV_KEYS); i < len; i++) {
        GetArgumentValue(cur_keys->Get(i), envs_k);

        for (int32_t j = 0; j < reverse_key_len; j++) {
            if (qstricmp(DEFT_ENV_KEYS[j], envs_k.c_str()))
                continue;

            util_base::has(opt_envs, DEFT_ENV_KEYS[j], has_reverse_key);
            js_envs_k = isolate->NewString(DEFT_ENV_KEYS[j]);
            js_v_in_cur = cur_envs->Get(js_envs_k);

            if (!js_v_in_cur->IsNullOrUndefined()) {
                if (has_reverse_key)
                    continue;
                opt_envs->Set(js_envs_k, js_v_in_cur);
            }
        }
    }

    JSArray keys = opt_envs->GetPropertyNames();
    len = (int32_t)keys->Length();

    exlib::wstring envstr;
    for (i = 0; i < len; i++) {
        JSValue k = keys->Get(i);
        v8::Local<v8::Value> v = opt_envs->Get(k);
        exlib::string ks, vs;

        hr = GetArgumentValue(k, ks);
        if (hr < 0)
            return hr;

        if (!v->IsNullOrUndefined()) {
            hr = GetArgumentValue(v, vs);
            if (hr < 0)
                return hr;
        } else
            vs = exlib::string("");

        ks.append(1, '=');
        ks.append(vs);

        envstr.append(utf8to16String(ks));
        envstr.append(1, 0);
    }
    envstr.append(1, 0);

    if (!CreateProcessW(NULL, &wstr[0], NULL, NULL, TRUE, CREATE_UNICODE_ENVIRONMENT,
            &envstr[0], NULL, &si, &pi)) {
        ::CloseHandle(cin_pipe[0]);
        ::CloseHandle(cin_pipe[1]);

        ::CloseHandle(cout_pipe[0]);
        ::CloseHandle(cout_pipe[1]);

        return CHECK_ERROR(LastError());
    }

    ::CloseHandle(pi.hThread);

    int32_t _current_pid = -1;
    process_base::get_pid(_current_pid);
    obj_ptr<SubProcess> sub = new SubProcess((intptr_t)pi.hProcess, (intptr_t)_current_pid);
    if (redirect) {
        ::CloseHandle(cin_pipe[1]);
        ::CloseHandle(cout_pipe[1]);

        wrap_pipe((intptr_t)cin_pipe[0], sub->m_stdin);
        wrap_pipe((intptr_t)cout_pipe[0], sub->m_stdout);
    }

    if (timeout > 0) {
        sub->m_timer = new PSTimer(timeout, pi.hProcess);
        sub->m_timer->sleep();
    }

    retVal = sub;

    return 0;
}

result_t SubProcess::get_pid(int32_t& retVal)
{
    retVal = GetProcessId((HANDLE)m_pid);
    return 0;
}

result_t SubProcess::get_ppid(int32_t& retVal)
{
    retVal = (int32_t)m_ppid;
    return 0;
}

result_t SubProcess::kill(int32_t signal)
{
    if (m_timer)
        m_timer->clear();

    if (!TerminateProcess((HANDLE)m_pid, -1))
        return CHECK_ERROR(LastError());

    return 0;
}

result_t SubProcess::wait(int32_t& retVal, AsyncEvent* ac)
{
    class asyncWaitHandle : public AsyncState {
    public:
        asyncWaitHandle(intptr_t pid, Timer* timer, int32_t& retVal, AsyncEvent* ac)
            : AsyncState(ac)
            , m_hProcess((HANDLE)pid)
            , m_timer(timer)
            , m_retVal(retVal)
        {
            next(wait);
        }

    public:
        ON_STATE(asyncWaitHandle, wait)
        {
            next(result);
            BOOL result = RegisterWaitForSingleObject(&m_hWait, m_hProcess, OnExited, this, INFINITE, WT_EXECUTEONLYONCE);
            if (!result)
                return CHECK_ERROR(LastError());

            return CALL_E_PENDDING;
        }

        ON_STATE(asyncWaitHandle, result)
        {
            ::UnregisterWaitEx(m_hWait, INVALID_HANDLE_VALUE);

            DWORD dwCode;

            GetExitCodeProcess(m_hProcess, &dwCode);
            m_retVal = (int32_t)dwCode;

            if (m_timer)
                m_timer->clear();

            return next();
        }

    private:
        static void CALLBACK OnExited(void* context, BOOLEAN isTimeOut)
        {
            ((asyncWaitHandle*)context)->apost(0);
        }

    private:
        HANDLE m_hProcess;
        HANDLE m_hWait;
        obj_ptr<Timer> m_timer;
        int32_t& m_retVal;
    };

    if (m_pid == -1)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    return (new asyncWaitHandle(m_pid, m_timer, retVal, ac))->post(0);
}

struct enumdata {
    LPCWSTR name;
    wchar_t buf[1024];
    bool found;
    RECT rect;
};

static BOOL CALLBACK find_window(HWND hwnd, LPARAM lParam)
{
    enumdata& ed = *(enumdata*)lParam;

    if (IsWindowVisible(hwnd)) {
        GetWindowTextW(hwnd, ed.buf, sizeof(ed.buf));
        if (!qstrcmp(ed.name, ed.buf)) {
            GetWindowRect(hwnd, &ed.rect);
            ed.found = TRUE;
            return FALSE;
        }

        EnumChildWindows(hwnd, find_window, lParam);
    }

    return TRUE;
}

result_t SubProcess::findWindow(exlib::string name, v8::Local<v8::Value>& retVal)
{
    DWORD pid = GetProcessId((HANDLE)m_pid);
    HANDLE h = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
    if (h == INVALID_HANDLE_VALUE)
        return CHECK_ERROR(LastError());

    enumdata ed = { UTF8_W(name), false };

    THREADENTRY32 te;
    te.dwSize = sizeof(te);
    if (Thread32First(h, &te)) {
        do {
            if (te.dwSize >= FIELD_OFFSET(THREADENTRY32, th32OwnerProcessID) + sizeof(te.th32OwnerProcessID) && te.th32OwnerProcessID == pid)
                EnumThreadWindows(te.th32ThreadID, find_window, (LPARAM)&ed);

            te.dwSize = sizeof(te);
        } while (Thread32Next(h, &te) && !ed.found);
    }
    CloseHandle(h);

    Isolate* isolate = holder();

    if (ed.found) {
        v8::Local<v8::Object> o = v8::Object::New(isolate->m_isolate);

        o->Set(isolate->NewString("left"),
            v8::Number::New(isolate->m_isolate, ed.rect.left));
        o->Set(isolate->NewString("top"),
            v8::Number::New(isolate->m_isolate, ed.rect.top));
        o->Set(isolate->NewString("right"),
            v8::Number::New(isolate->m_isolate, ed.rect.right));
        o->Set(isolate->NewString("bottom"),
            v8::Number::New(isolate->m_isolate, ed.rect.bottom));

        retVal = o;
    } else {
        retVal = v8::Undefined(isolate->m_isolate);
    }

    // retVal = ed.found;

    return 0;
}
}

#endif
