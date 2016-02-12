/*
 * SubProcess_posix.cpp
 *
 *  Created on: Feb 11, 2016
 *      Author: lion
 */

#ifndef _WIN32

#include "SubProcess.h"
#include <spawn.h>
#include <vector>
#include <sys/wait.h>

namespace fibjs
{

result_t SubProcess::create(const char* command, v8::Local<v8::Array> args, v8::Local<v8::Object> opts,
                            v8::Local<v8::Object> envs, bool redirect, obj_ptr<SubProcess_base>& retVal)
{
	int32_t err = 0;
	pid_t pid;
	result_t hr;
	posix_spawn_file_actions_t fops;
	posix_spawnattr_t attr;
	std::vector<std::string> argStr;
	std::vector<char*> _args;
	int32_t len = args->Length();
	int32_t i;
	int32_t cin_pipe[2];
	int32_t cout_pipe[2];

	Isolate* isolate = Isolate::current();

	argStr.resize(len);
	_args.resize(len + 2);

	_args[0] = (char*)command;
	for (i = 0; i < len; i ++)
	{
		hr = GetArgumentValue(isolate->m_isolate, args->Get(i), argStr[i]);
		if (hr < 0)
			return hr;

		_args[i + 1] = &argStr[i][0];
	}
	_args[i + 1] = NULL;

	posix_spawnattr_init(&attr);
	posix_spawn_file_actions_init(&fops);

	if (redirect)
	{
		if (pipe(cout_pipe))
		{
			posix_spawn_file_actions_destroy(&fops);
			posix_spawnattr_destroy(&attr);

			return CHECK_ERROR(LastError());
		}

		if (pipe(cin_pipe))
		{
			::close(cout_pipe[0]);
			::close(cout_pipe[1]);

			posix_spawn_file_actions_destroy(&fops);
			posix_spawnattr_destroy(&attr);

			return CHECK_ERROR(LastError());
		}

		posix_spawn_file_actions_adddup2(&fops, cin_pipe[0], 0);
		posix_spawn_file_actions_adddup2(&fops, cout_pipe[1], 1);

		posix_spawn_file_actions_addclose(&fops, cin_pipe[0]);
		posix_spawn_file_actions_addclose(&fops, cin_pipe[1]);

		posix_spawn_file_actions_addclose(&fops, cout_pipe[0]);
		posix_spawn_file_actions_addclose(&fops, cout_pipe[1]);
	}

	err = posix_spawnp(&pid, command, &fops, &attr, _args.data(), NULL);

	if (redirect)
	{
		::close(cin_pipe[0]);
		::close(cout_pipe[1]);
	}

	posix_spawn_file_actions_destroy(&fops);
	posix_spawnattr_destroy(&attr);

	if (err != 0)
	{
		if (redirect)
		{
			::close(cin_pipe[1]);
			::close(cout_pipe[0]);
		}

		errno = err;
		return CHECK_ERROR(LastError());
	}

	obj_ptr<SubProcess> sub = new SubProcess(pid);
	if (redirect)
	{
		wrap_pipe(cin_pipe[1], sub->m_stdin);
		wrap_pipe(cout_pipe[0], sub->m_stdout);
	}

	retVal = sub;

	return 0;
}

result_t SubProcess::get_pid(int32_t& retVal)
{
	retVal = m_pid;
	return 0;
}

result_t SubProcess::kill(int32_t signal)
{
	return 0;
}

result_t SubProcess::wait(int32_t& retVal, AsyncEvent* ac)
{
	if (!ac)
		return CHECK_ERROR(CALL_E_NOSYNC);

	int32_t status;
	waitpid(m_pid, &status, 0);
	retVal = (int16_t)status / 256;

	return 0;
}

}

#endif
