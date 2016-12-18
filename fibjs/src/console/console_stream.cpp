/*
 * console_net.cpp
 *
 *  Created on: Aug 22, 2015
 *      Author: lion
 */

#include "console.h"
#include "Buffer.h"

namespace fibjs
{

result_t stream_logger::write(AsyncEvent *ac)
{
	class asyncWrite: public AsyncState
	{
	public:
		asyncWrite(Stream_base* out, exlib::List<item>& logs, AsyncEvent *ac) :
			AsyncState(ac), m_out(out), m_logs(logs)
		{
			set(write);
		}

	public:
		static int32_t write(AsyncState *pState, int32_t n)
		{
			asyncWrite *pThis = (asyncWrite *) pState;
			exlib::string outBuffer;
			item *p1;

			while ((p1 = pThis->m_logs.getHead()) != 0)
			{
				exlib::string txt;

				if (p1->m_priority == console_base::_NOTICE)
					txt = logger::notice() + p1->m_msg + COLOR_RESET + "\n";
				else if (p1->m_priority == console_base::_WARN)
					txt = logger::warn() + p1->m_msg + COLOR_RESET + "\n";
				else if (p1->m_priority <= console_base::_ERROR)
					txt = logger::error() + p1->m_msg + COLOR_RESET + "\n";
				else if (p1->m_priority == console_base::_PRINT)
					txt = p1->m_msg;
				else
					txt = p1->m_msg + "\n";

				outBuffer.append(txt);

				delete p1;

				if (outBuffer.length() > STREAM_BUFF_SIZE)
					break;
			}

			if (outBuffer.empty())
				return pThis->done();

			pThis->m_buffer = new Buffer(outBuffer);

			return pThis->m_out->write(pThis->m_buffer, pThis);
		}

		virtual int32_t error(int32_t v)
		{
			item *p1;

			while ((p1 = m_logs.getHead()) != 0)
				delete p1;

			return v;
		}

	private:
		obj_ptr<Stream_base> m_out;
		exlib::List<item>& m_logs;
		obj_ptr<Buffer_base> m_buffer;
	};

	return (new asyncWrite(m_out, m_workinglogs, ac))->post(0);
}

}
