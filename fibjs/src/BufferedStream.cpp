/*
 * BufferedStream.cpp
 *
 *  Created on: Jun 11, 2012
 *      Author: lion
 */

#include "BufferedStream.h"
#include "Stream.h"
#include "Buffer.h"

namespace fibjs
{

class asyncBuffer: public exlib::AsyncEvent
{
public:
	asyncBuffer(BufferedStream* pThis, Stream_base* stm, exlib::AsyncEvent* ac) :
			m_pThis(pThis), m_stm(stm), m_ac(ac)
	{
	}

	virtual bool process(bool end)
	{
		return true;
	}

	void do_post(int v, bool bFirst)
	{
		result_t hr = v;
		int sz;
		bool streamEnd = false;

		while (hr != CALL_E_PENDDING)
		{
			if (!bFirst)
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

					sz = (int) strBuf.length();
					m_pThis->m_size += sz;
					m_pThis->m_bufs.push_back(strBuf);
				}
				else
					streamEnd = true;
			}
			bFirst = false;

			if (process(streamEnd))
			{
				size_t i;

				for (i = 0; i < m_pThis->m_bufs.size(); i++)
				{
					sz = (int) m_pThis->m_bufs[i].length();
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

			hr = m_stm->read(-1, m_buf, this);
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
	BufferedStream* m_pThis;
	Stream_base* m_stm;
	exlib::AsyncEvent* m_ac;
	obj_ptr<Buffer_base> m_buf;
};

result_t BufferedStream_base::_new(obj_ptr<Stream_base>& stm,
		obj_ptr<BufferedStream_base>& retVal)
{
	retVal = new BufferedStream(stm);
	return 0;
}

result_t BufferedStream::read(int32_t bytes, obj_ptr<Buffer_base>& retVal,
		exlib::AsyncEvent* ac)
{
	class asyncRead: public asyncBuffer
	{
	public:
		asyncRead(BufferedStream* pThis, Stream_base* stm, int32_t bytes,
				obj_ptr<Buffer_base>& retVal, exlib::AsyncEvent* ac) :
				asyncBuffer(pThis, stm, ac), m_bytes(bytes), m_retVal(retVal)
		{
		}

		virtual bool process(bool streamEnd)
		{
			if (streamEnd)
				m_bytes = m_pThis->m_size;
			else if (m_bytes < 0)
			{
				if (m_pThis->m_size > 0)
					m_bytes = m_pThis->m_size;
				return false;
			}
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
					l = (int) d.length() - p;
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

result_t BufferedStream::asyncRead(int32_t bytes)
{
	acb_read(s_acPool, bytes);
	return 0;
}

result_t BufferedStream::onread(v8::Handle<v8::Function> func)
{
	return on("read", func);
}

result_t BufferedStream::write(obj_ptr<Buffer_base>& data,
		exlib::AsyncEvent* ac)
{
	return m_stm->write(data, ac);
}

result_t BufferedStream::asyncWrite(obj_ptr<Buffer_base>& data)
{
	acb_write(s_acPool, data);
	return 0;
}

result_t BufferedStream::onwrite(v8::Handle<v8::Function> func)
{
	return on("write", func);
}

result_t BufferedStream::copyTo(obj_ptr<Stream_base>& stm, int64_t bytes,
		int64_t& retVal, exlib::AsyncEvent* ac)
{
	if (!ac)
		return CALL_E_NOSYNC;

	return copyStream(this, stm, bytes, retVal, ac);
}

result_t BufferedStream::asyncCopyTo(obj_ptr<Stream_base>& stm, int64_t bytes)
{
	acb_copyTo(s_acPool, stm, bytes);
	return 0;
}

result_t BufferedStream::oncopyto(v8::Handle<v8::Function> func)
{
	return on("copyto", func);
}

result_t BufferedStream::stat(obj_ptr<Stat_base>& retVal, exlib::AsyncEvent* ac)
{
	return m_stm->stat(retVal, ac);
}

result_t BufferedStream::asyncStat()
{
	acb_stat(s_acPool);
	return 0;
}

result_t BufferedStream::onstat(v8::Handle<v8::Function> func)
{
	return on("stat", func);
}

result_t BufferedStream::onerror(v8::Handle<v8::Function> func)
{
	return on("error", func);
}

result_t BufferedStream::readText(int32_t size, std::string& retVal,
		exlib::AsyncEvent* ac)
{
	return 0;
}

result_t BufferedStream::readLine(std::string& retVal, exlib::AsyncEvent* ac)
{
	return 0;
}

result_t BufferedStream::readUntil(int32_t mk, obj_ptr<Buffer_base>& retVal,
		exlib::AsyncEvent* ac)
{
	return 0;
}

result_t BufferedStream::get_EOL(std::string& retVal)
{
	return 0;
}

result_t BufferedStream::set_EOL(const char* newVal)
{
	return 0;
}

} /* namespace fibjs */
