/*
 * SubProcess_win.cpp
 *
 *  Created on: Feb 11, 2016
 *      Author: lion
 */

#ifdef _WIN32

#include "object.h"
#include "ifs/process.h"
#include "SubProcess.h"
#include <vector>
#include <psapi.h>
#include "utf8.h"

namespace fibjs
{

class PSTimer : public Timer
{
public:
	PSTimer(int32_t timeout, HANDLE pid) : Timer(timeout), m_pid(pid)
	{}

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

	do
	{
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

	hChild = CreateFile( fname, in ? GENERIC_READ : GENERIC_WRITE, 0, &sa, OPEN_EXISTING,
	                     FILE_ATTRIBUTE_NORMAL, 0);
	if (hChild == INVALID_HANDLE_VALUE)
	{
		CloseHandle(hMine);
		return CHECK_ERROR(LastError());
	}

	*hd1 = hMine;
	*hd2 = hChild;

	return 0;
}

result_t SubProcess::create(const char* command, v8::Local<v8::Array> args, v8::Local<v8::Object> opts,
                            bool redirect, obj_ptr<SubProcess_base>& retVal)
{
	result_t hr;
	PROCESS_INFORMATION pi;
	STARTUPINFOW si;
	int32_t len = args->Length();
	int32_t i;
	HANDLE cin_pipe[2] = {0};
	HANDLE cout_pipe[2] = {0};
	int32_t timeout;

	exlib::wstring wstr(L"cmd /C ");
	wstr.append(utf8to16String(command));

	Isolate* isolate = Isolate::current();

	hr = GetConfigValue(isolate->m_isolate, opts, "timeout", timeout);
	if (hr == CALL_E_PARAMNOTOPTIONAL)
		timeout = 0;
	else if (hr < 0)
		return CHECK_ERROR(hr);

	for (i = 0; i < len; i ++)
	{
		exlib::string str;

		hr = GetArgumentValue(isolate->m_isolate, args->Get(i), str);
		if (hr < 0)
			return hr;

		wstr.append(1, ' ');
		wstr.append(utf8to16String(str));
	}

	ZeroMemory(&si, sizeof(STARTUPINFO));
	si.cb = sizeof(STARTUPINFO);

	if (redirect)
	{
		hr = create_name_pipe(&cin_pipe[0], &cin_pipe[1], true);
		if (hr < 0)
			return hr;

		hr = create_name_pipe(&cout_pipe[0], &cout_pipe[1], false);
		if (hr < 0)
		{
			::CloseHandle(cin_pipe[0]);
			::CloseHandle(cin_pipe[1]);

			return hr;
		}

		si.hStdError = GetStdHandle(STD_ERROR_HANDLE);
		si.hStdOutput = cout_pipe[1];
		si.hStdInput = cin_pipe[1];
		si.dwFlags |= STARTF_USESTDHANDLES;
	}

	exlib::wstring envstr;

	v8::Local<v8::Object> envs;

	hr = GetConfigValue(isolate->m_isolate, opts, "env", envs, true);
	if (hr == CALL_E_PARAMNOTOPTIONAL)
		hr = process_base::get_env(envs);
	if (hr < 0)
		return hr;

	v8::Local<v8::Array> keys = envs->GetPropertyNames();
	len = (int32_t) keys->Length();

	for (i = 0; i < len; i++)
	{
		v8::Local<v8::Value> k = keys->Get(i);
		v8::Local<v8::Value> v = envs->Get(k);
		exlib::string ks, vs;

		hr = GetArgumentValue(k, ks);
		if (hr < 0)
			return hr;

		hr = GetArgumentValue(v, vs);
		if (hr < 0)
			return hr;

		ks.append(1, '=');
		ks.append(vs);

		envstr.append(utf8to16String(ks));
		envstr.append(1, 0);
	}
	envstr.append(1, 0);

	if (!CreateProcessW(NULL, &wstr[0], NULL, NULL, TRUE, CREATE_UNICODE_ENVIRONMENT,
	                    &envstr[0], NULL, &si, &pi))
	{
		::CloseHandle(cin_pipe[0]);
		::CloseHandle(cin_pipe[1]);

		::CloseHandle(cout_pipe[0]);
		::CloseHandle(cout_pipe[1]);

		return CHECK_ERROR(LastError());
	}

	::CloseHandle(pi.hThread);

	obj_ptr<SubProcess> sub = new SubProcess((intptr_t)pi.hProcess);
	if (redirect)
	{
		::CloseHandle(cin_pipe[1]);
		::CloseHandle(cout_pipe[1]);

		wrap_pipe((intptr_t)cin_pipe[0], sub->m_stdin);
		wrap_pipe((intptr_t)cout_pipe[0], sub->m_stdout);
	}

	if (timeout > 0)
		sub->m_timer = new PSTimer(timeout, pi.hProcess);

	retVal = sub;

	return 0;
}

result_t SubProcess::get_pid(int32_t& retVal)
{
	retVal = GetProcessId((HANDLE)m_pid);
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
	if (m_pid == -1)
		return CHECK_ERROR(CALL_E_INVALID_CALL);

	if (!ac)
		return CHECK_ERROR(CALL_E_LONGSYNC);

	DWORD dwCode;

	WaitForSingleObject((HANDLE)m_pid, INFINITE);
	GetExitCodeProcess((HANDLE)m_pid, &dwCode);
	retVal = (int32_t)dwCode;

	if (m_timer)
		m_timer->clear();

	return 0;
}

}

#endif
