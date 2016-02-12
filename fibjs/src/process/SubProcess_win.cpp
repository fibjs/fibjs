/*
 * SubProcess_win.cpp
 *
 *  Created on: Feb 11, 2016
 *      Author: lion
 */

#ifdef _WIN32

#include "SubProcess.h"
#include <vector>
#include <psapi.h>
#include "utf8.h"

namespace fibjs
{

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
                            v8::Local<v8::Object> envs, bool redirect, obj_ptr<SubProcess_base>& retVal)
{
	result_t hr;
	PROCESS_INFORMATION pi;
	STARTUPINFOW si;
	int32_t len = args->Length();
	int32_t i;
	HANDLE cin_pipe[2] = {0};
	HANDLE cout_pipe[2] = {0};

	wstring wstr(L"cmd /C ");
	wstr.append(utf8to16String(command));

	Isolate* isolate = Isolate::current();
	for (i = 0; i < len; i ++)
	{
		std::string str;

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

	if (!CreateProcessW(NULL, &wstr[0], NULL, NULL, TRUE, 0, NULL, NULL, &si, &pi))
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

	return 0;
}

}

#endif
