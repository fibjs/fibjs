/*
 * Stream.cpp
 *
 *  Created on: Jun 6, 2012
 *      Author: lion
 */

#include "Stream.h"

namespace fibjs
{

result_t copyStream(Stream_base* from, Stream_base* to, int32_t bytes,
		int32_t& retVal, exlib::AsyncEvent* ac)
{
	class asyncCopy: public exlib::AsyncEvent
	{
	public:
		asyncCopy(Stream_base* from, Stream_base* to, int32_t bytes,
				int32_t& retVal, exlib::AsyncEvent* ac) :
				m_from(from), m_to(to), m_bytes(bytes), m_retVal(retVal), m_ac(
						ac), m_state(0)
		{
			m_retVal = 0;
		}

		virtual void post(int v)
		{
			result_t hr = v;
			int32_t len;

			while (hr != CALL_E_PENDDING)
			{
				if (hr < 0)
				{
					m_ac->post(hr);
					delete this;
					return;
				}

				switch (m_state)
				{
				case 0:
					m_state = 1;

					if (m_bytes == 0)
					{
						m_ac->post(0);
						delete this;
						return;
					}

					if (m_bytes > STREAM_BUFF_SIZE || m_bytes < 0)
						len = STREAM_BUFF_SIZE;
					else
						len = m_bytes;

					m_buf.Release();
					hr = m_from->read(len, m_buf, this);
					break;
				case 1:
					m_state = 0;

					if (m_buf == NULL)
					{
						m_ac->post(0);
						delete this;
						return;
					}

					m_buf->get_length(len);
					m_retVal += len;

					if (m_bytes > 0)
						m_bytes -= len;

					hr = m_to->write(m_buf, this);
					break;
				}
			}
		}

	public:
		Stream_base* m_from;
		Stream_base* m_to;
		int32_t m_bytes;
		int32_t& m_retVal;
		exlib::AsyncEvent* m_ac;
		int m_state;
		obj_ptr<Buffer_base> m_buf;
	};

	(new asyncCopy(from, to, bytes, retVal, ac))->post(0);
	return CALL_E_PENDDING;
}

}

