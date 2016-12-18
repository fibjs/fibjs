/*
 * fs.cpp
 *
 *  Created on: Apr 7, 2012
 *      Author: lion
 */

#include <string.h>

#include "object.h"
#include "ifs/io.h"
#include "File.h"

namespace fibjs
{

DECLARE_MODULE(io);

result_t io_base::copyStream(Stream_base* from, Stream_base* to, int64_t bytes,
                             int64_t& retVal, AsyncEvent* ac)
{
	class asyncCopy: public AsyncState
	{
	public:
		asyncCopy(Stream_base *from, Stream_base *to, int64_t bytes,
		          int64_t &retVal, AsyncEvent *ac) :
			AsyncState(ac), m_from(from), m_to(to), m_bytes(bytes), m_retVal(
			    retVal)
		{
			m_retVal = 0;
			set(read);
		}

		static int32_t read(AsyncState *pState, int32_t n)
		{
			asyncCopy *pThis = (asyncCopy *) pState;
			int64_t len;

			pThis->set(write);

			if (pThis->m_bytes == 0)
				return pThis->done();

			if (pThis->m_bytes > STREAM_BUFF_SIZE)
				len = STREAM_BUFF_SIZE;
			else
				len = pThis->m_bytes;

			pThis->m_buf.Release();
			return pThis->m_from->read((int32_t) len, pThis->m_buf, pThis);
		}

		static int32_t write(AsyncState *pState, int32_t n)
		{
			asyncCopy *pThis = (asyncCopy *) pState;
			int32_t blen;

			pThis->set(read);

			if (n == CALL_RETURN_NULL)
				return pThis->done();

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
		int64_t &m_retVal;
		obj_ptr<Buffer_base> m_buf;
	};

	if (!ac)
		return CHECK_ERROR(CALL_E_NOSYNC);

	return (new asyncCopy(from, to, bytes, retVal, ac))->post(0);
}

}
