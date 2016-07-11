/*
 * SubProcess.cpp
 *
 *  Created on: Feb 11, 2016
 *      Author: lion
 */

#include "SubProcess.h"
#include "BufferedStream.h"

namespace fibjs
{

#ifdef _WIN32
extern HANDLE s_hIocp;
#endif

void SubProcess::wrap_pipe(intptr_t fd, obj_ptr<BufferedStream_base>& bs)
{
#ifdef _WIN32
	CreateIoCompletionPort((HANDLE) fd, s_hIocp, 0, 0);
#endif

	obj_ptr<Stream_base> stm = new Pipe(fd);
	bs = new BufferedStream(stm);
}

result_t SubProcess::read(int32_t bytes, obj_ptr<Buffer_base> &retVal, AsyncEvent *ac)
{
	if (m_stdout == NULL)
		return CHECK_ERROR(CALL_E_INVALID_CALL);

	return m_stdout->read(bytes, retVal, ac);
}

result_t SubProcess::write(Buffer_base *data, AsyncEvent *ac)
{
	if (m_stdin == NULL)
		return CHECK_ERROR(CALL_E_INVALID_CALL);

	return m_stdin->write(data, ac);
}

result_t SubProcess::close(AsyncEvent *ac)
{
	if (m_timer)
		m_timer->clear();

	if (m_stdout == NULL)
		return 0;

	class asyncClose: public AsyncState
	{
	public:
		asyncClose(BufferedStream_base* pipein, BufferedStream_base* pipeout,
		           AsyncEvent *ac) :
			AsyncState(ac), m_stdin(pipein), m_stdout(pipeout)
		{
			set(close_in);
		}

	public:
		static int32_t close_in(AsyncState *pState, int32_t n)
		{
			asyncClose *pThis = (asyncClose *) pState;

			pThis->set(close_out);
			return pThis->m_stdin->close(pThis);
		}

		static int32_t close_out(AsyncState *pState, int32_t n)
		{
			asyncClose *pThis = (asyncClose *) pState;

			pThis->set(0);
			return pThis->m_stdout->close(pThis);
		}

	private:
		obj_ptr<BufferedStream_base> m_stdin;
		obj_ptr<BufferedStream_base> m_stdout;
	};

	if (!ac)
		return CHECK_ERROR(CALL_E_NOSYNC);

#ifdef _WIN32
	if (m_pid != -1)
	{
		::CloseHandle((HANDLE)m_pid);
		m_pid = -1;
	}
#endif

	return (new asyncClose(m_stdin, m_stdout, ac))->post(0);
}

result_t SubProcess::copyTo(Stream_base *stm, int64_t bytes, int64_t &retVal, AsyncEvent *ac)
{
	if (m_stdout == NULL)
		return CHECK_ERROR(CALL_E_INVALID_CALL);

	return m_stdout->copyTo(stm, bytes, retVal, ac);
}

result_t SubProcess::readText(int32_t size, qstring &retVal, AsyncEvent *ac)
{
	if (m_stdout == NULL)
		return CHECK_ERROR(CALL_E_INVALID_CALL);

	return m_stdout->readText(size, retVal, ac);
}

result_t SubProcess::readLine(int32_t maxlen, qstring &retVal, AsyncEvent *ac)
{
	if (m_stdout == NULL)
		return CHECK_ERROR(CALL_E_INVALID_CALL);

	return m_stdout->readLine(maxlen, retVal, ac);
}

result_t SubProcess::readLines(int32_t maxlines, v8::Local<v8::Array> &retVal)
{
	if (m_stdout == NULL)
		return CHECK_ERROR(CALL_E_INVALID_CALL);

	return m_stdout->readLines(maxlines, retVal);
}

result_t SubProcess::readUntil(const char *mk, int32_t maxlen, qstring &retVal, AsyncEvent *ac)
{
	if (m_stdout == NULL)
		return CHECK_ERROR(CALL_E_INVALID_CALL);

	return m_stdout->readUntil(mk, maxlen, retVal, ac);
}

result_t SubProcess::writeText(const char *txt, AsyncEvent *ac)
{
	if (m_stdin == NULL)
		return CHECK_ERROR(CALL_E_INVALID_CALL);

	return m_stdin->writeText(txt, ac);
}

result_t SubProcess::writeLine(const char *txt, AsyncEvent *ac)
{
	if (m_stdin == NULL)
		return CHECK_ERROR(CALL_E_INVALID_CALL);

	return m_stdin->writeLine(txt, ac);
}

result_t SubProcess::get_stream(obj_ptr<Stream_base> &retVal)
{
	return CHECK_ERROR(CALL_E_INVALID_CALL);
}

result_t SubProcess::get_charset(qstring &retVal)
{
	if (m_stdin == NULL)
		return CHECK_ERROR(CALL_E_INVALID_CALL);

	return m_stdin->get_charset(retVal);
}

result_t SubProcess::set_charset(const char *newVal)
{
	if (m_stdin != NULL)
		m_stdin->set_charset(newVal);

	if (m_stdout != NULL)
		m_stdout->set_charset(newVal);

	return 0;
}

result_t SubProcess::get_EOL(qstring &retVal)
{
	if (m_stdin == NULL)
		return CHECK_ERROR(CALL_E_INVALID_CALL);

	return m_stdin->get_EOL(retVal);
}

result_t SubProcess::set_EOL(const char *newVal)
{
	if (m_stdin != NULL)
		m_stdin->set_EOL(newVal);

	if (m_stdout != NULL)
		m_stdout->set_EOL(newVal);

	return 0;
}

result_t SubProcess::get_stdin(obj_ptr<BufferedStream_base>& retVal)
{
	if (m_stdin == NULL)
		return CHECK_ERROR(CALL_RETURN_NULL);

	retVal = m_stdin;

	return 0;
}

result_t SubProcess::get_stdout(obj_ptr<BufferedStream_base>& retVal)
{
	if (m_stdout == NULL)
		return CHECK_ERROR(CALL_RETURN_NULL);

	retVal = m_stdout;

	return 0;
}

}
