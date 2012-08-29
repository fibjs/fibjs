/*
 * Stream.cpp
 *
 *  Created on: Jun 6, 2012
 *      Author: lion
 */

#include "Stream.h"

namespace fibjs
{

result_t copyStream(Stream_base* from, Stream_base* to, int64_t bytes,
		int64_t& retVal, exlib::AsyncEvent* ac)
{
	class asyncCopy: public asyncState
	{
	public:
		asyncCopy(Stream_base* from, Stream_base* to, int64_t bytes,
				int64_t& retVal, exlib::AsyncEvent* ac) :
				asyncState(ac), m_from(from), m_to(to), m_bytes(bytes), m_retVal(
						retVal)
		{
			m_retVal = 0;
			set(read);
		}

		static int read(asyncState* pState, int n)
		{
			asyncCopy* pThis = (asyncCopy*) pState;
			int64_t len;

			pThis->set(write);

			if (pThis->m_bytes == 0)
				return pThis->done(0);

			if (pThis->m_bytes > STREAM_BUFF_SIZE)
				len = STREAM_BUFF_SIZE;
			else
				len = pThis->m_bytes;

			pThis->m_buf.Release();
			return pThis->m_from->read((int32_t) len, pThis->m_buf, pThis);
		}

		static int write(asyncState* pState, int n)
		{
			asyncCopy* pThis = (asyncCopy*) pState;
			int blen;

			pThis->set(read);

			if (n == CALL_RETURN_NULL)
				return pThis->done(0);

			pThis->m_buf->get_length(blen);
			pThis->m_retVal += blen;

			if (pThis->m_bytes > 0)
				pThis->m_bytes -= blen;

			return pThis->m_to->write(pThis->m_buf, pThis);
		}

	public:
		obj_ptr<Stream_base> m_from;
		obj_ptr<Stream_base> m_to;
		int64_t m_bytes;
		int64_t& m_retVal;
		obj_ptr<Buffer_base> m_buf;
	};

	if (!ac)
		return CALL_E_NOSYNC;

	return (new asyncCopy(from, to, bytes, retVal, ac))->post(0);
}

}

