/*
 * SubProcess.h
 *
 *  Created on: Feb 11, 2016
 *      Author: lion
 */

#ifndef SUBPROCESS_H_
#define SUBPROCESS_H_

#include "ifs/SubProcess.h"
#include "AsyncIO.h"
#include "Stream.h"
#include "Timer.h"

namespace fibjs
{

class SubProcess: public SubProcess_base
{
public:
	SubProcess(intptr_t pid) : m_pid(pid)
	{}

#ifdef _WIN32
	~SubProcess()
	{
		::CloseHandle((HANDLE)m_pid);
	}
#endif

public:
	// Stream_base
	virtual result_t read(int32_t bytes, obj_ptr<Buffer_base> &retVal, AsyncEvent *ac);
	virtual result_t write(Buffer_base *data, AsyncEvent *ac);
	virtual result_t close(AsyncEvent *ac);
	virtual result_t copyTo(Stream_base *stm, int64_t bytes, int64_t &retVal, AsyncEvent *ac);

public:
	// BufferedStream_base
	virtual result_t readText(int32_t size, exlib::string &retVal, AsyncEvent *ac);
	virtual result_t readLine(int32_t maxlen, exlib::string &retVal, AsyncEvent *ac);
	virtual result_t readLines(int32_t maxlines, v8::Local<v8::Array> &retVal);
	virtual result_t readUntil(exlib::string mk, int32_t maxlen, exlib::string &retVal, AsyncEvent *ac);
	virtual result_t writeText(exlib::string txt, AsyncEvent *ac);
	virtual result_t writeLine(exlib::string txt, AsyncEvent *ac);
	virtual result_t get_stream(obj_ptr<Stream_base> &retVal);
	virtual result_t get_charset(exlib::string &retVal);
	virtual result_t set_charset(exlib::string newVal);
	virtual result_t get_EOL(exlib::string &retVal);
	virtual result_t set_EOL(exlib::string newVal);

public:
	// SubProcess_base
	virtual result_t kill(int32_t signal);
	virtual result_t wait(int32_t& retVal, AsyncEvent* ac);
	virtual result_t findWindow(exlib::string name, v8::Local<v8::Value>& retVal);
	virtual result_t get_pid(int32_t& retVal);
	virtual result_t get_stdin(obj_ptr<BufferedStream_base>& retVal);
	virtual result_t get_stdout(obj_ptr<BufferedStream_base>& retVal);

public:
	static result_t create(exlib::string command, v8::Local<v8::Array> args, v8::Local<v8::Object> opts,
	                       bool redirect, obj_ptr<SubProcess_base>& retVal);

public:
	class Pipe: public Stream_base
	{
		FIBER_FREE();

	public:
		Pipe(intptr_t fd) :
			m_aio(fd, 0, 0)
		{
		}

		~Pipe()
		{
			if (m_aio.m_fd != INVALID_SOCKET)
				asyncCall(close_pipe, m_aio.m_fd);
		}

	public:
		// Stream_base
		virtual result_t read(int32_t bytes, obj_ptr<Buffer_base> &retVal,
		                      AsyncEvent *ac)
		{
			return m_aio.read(bytes, retVal, ac, bytes > 0, NULL);
		}

		virtual result_t write(Buffer_base *data, AsyncEvent *ac)
		{
			return m_aio.write(data, ac);
		}

		virtual result_t close(AsyncEvent *ac)
		{
			if (m_aio.m_fd == INVALID_SOCKET)
				return 0;

			if (!ac)
				return CHECK_ERROR(CALL_E_NOSYNC);

			if (m_aio.m_fd != INVALID_SOCKET)
				close_pipe(m_aio.m_fd);

			m_aio.m_fd = INVALID_SOCKET;

#ifndef _WIN32
			return m_aio.cancel(ac);
#else
			return 0;
#endif
		}

		virtual result_t copyTo(Stream_base *stm, int64_t bytes,
		                        int64_t &retVal, AsyncEvent *ac)
		{
			if (m_aio.m_fd == INVALID_SOCKET)
				return CHECK_ERROR(CALL_E_INVALID_CALL);

			return copyStream(this, stm, bytes, retVal, ac);
		}

	private:
		static void close_pipe(intptr_t fd)
		{
#ifdef _WIN32
			::CloseHandle((HANDLE)fd);
#else
			::close(fd);
#endif
		}

	private:
		AsyncIO m_aio;
	};

	static void wrap_pipe(intptr_t fd, obj_ptr<BufferedStream_base>& bs);

private:
	obj_ptr<BufferedStream_base> m_stdin;
	obj_ptr<BufferedStream_base> m_stdout;
	obj_ptr<Timer> m_timer;

	intptr_t m_pid;
};

} /* namespace fibjs */

#endif /* SUBPROCESS_H_ */
