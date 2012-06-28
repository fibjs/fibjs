/*
 * BufferedReader.cpp
 *
 *  Created on: Jun 11, 2012
 *      Author: lion
 */

#include "BufferedReader.h"
#include "Stream.h"
#include "Buffer.h"

namespace fibjs
{

class asyncBuffer: public exlib::AsyncEvent
{
public:
	asyncBuffer(BufferedReader* pThis, Stream_base* stm, exlib::AsyncEvent* ac) :
			m_pThis(pThis), m_stm(stm), m_ac(ac)
	{
	}

	virtual bool process(bool end)
	{
		return true;
	}

	void do_post(int v, bool bStart)
	{
		result_t hr = v;
		int sz;
		bool end = false;

		while (hr != CALL_E_PENDDING)
		{
			if (!bStart)
			{
				if (hr < 0)
				{
					m_ac->post(hr);
					delete this;
					return;
				}

				if (m_buf)
				{
					std::string strBuf;

					m_buf->toString(strBuf);
					m_buf.Release();

					sz = (int)strBuf.length();
					m_pThis->m_size += sz;
					m_pThis->m_bufs.push_back(strBuf);
				}
				else
					end = true;
			}
			bStart = false;

			if (process(end))
			{
				size_t i;

				for (i = 0; i < m_pThis->m_bufs.size(); i++)
				{
					sz = (int)m_pThis->m_bufs[i].length();
					if (sz > m_pThis->m_pos)
						break;

					m_pThis->m_pos -= sz;
				}

				if (i == m_pThis->m_bufs.size())
				{
					m_pThis->m_pos = m_pThis->m_size = 0;
					m_pThis->m_bufs.clear();
				}
				else if (i > 0)
					m_pThis->m_bufs.erase(m_pThis->m_bufs.begin(),
							m_pThis->m_bufs.begin() + i);

				m_ac->post(0);
				delete this;
				return;
			}

			hr = m_stm->read(STREAM_BUFF_SIZE, m_buf, this);
		}
	}

	virtual void post(int v)
	{
		do_post(v, false);
	}

	void start()
	{
		do_post(0, true);
	}

public:
	BufferedReader* m_pThis;
	Stream_base* m_stm;
	exlib::AsyncEvent* m_ac;
	obj_ptr<Buffer_base> m_buf;
};

result_t BufferedReader_base::_new(obj_ptr<Stream_base>& stm,
		obj_ptr<BufferedReader_base>& retVal)
{
	retVal = new BufferedReader(stm);
	return 0;
}

result_t BufferedReader::readText(int32_t size, std::string& retVal,
		exlib::AsyncEvent* ac)
{
	return 0;
}

result_t BufferedReader::readLine(std::string& retVal, exlib::AsyncEvent* ac)
{
	return 0;
}

result_t BufferedReader::read(int32_t bytes, obj_ptr<Buffer_base>& retVal,
		exlib::AsyncEvent* ac)
{
	class asyncRead: public asyncBuffer
	{
	public:
		asyncRead(BufferedReader* pThis, Stream_base* stm, int32_t bytes,
				obj_ptr<Buffer_base>& retVal, exlib::AsyncEvent* ac) :
				asyncBuffer(pThis, stm, ac), m_bytes(bytes), m_retVal(retVal)
		{
		}

		virtual bool process(bool end)
		{
			if (end)
				m_bytes = m_pThis->m_size;
			else if (m_bytes > m_pThis->m_size)
				return false;

			if (m_bytes)
			{
				int i, p, p1, l;
				std::string strBuf;

				strBuf.resize(m_bytes);

				p = m_pThis->m_pos;
				p1 = 0;

				for (i = 0; p1 < m_bytes && i < (int) m_pThis->m_bufs.size();
						i++)
				{
					std::string& d = m_pThis->m_bufs[i];
					l = (int)d.length() - p;
					if (l > m_bytes - p1)
						l = m_bytes - p1;

					memcpy(&strBuf[p1], &d[p], l);
					p = 0;
					p1 += l;
				}

				m_pThis->m_pos += m_bytes;
				m_pThis->m_size -= m_bytes;

				m_retVal = new Buffer(strBuf);
			}

			return true;
		}

	public:
		int32_t m_bytes;
		obj_ptr<Buffer_base>& m_retVal;
	};

	if (!ac)
		return CALL_E_NOSYNC;

	(new asyncRead(this, m_stm, bytes, retVal, ac))->start();
	return CALL_E_PENDDING;
}

result_t BufferedReader::readUntil(const char* mk, obj_ptr<Buffer_base>& retVal,
		exlib::AsyncEvent* ac)
{
	return 0;
}

} /* namespace fibjs */
