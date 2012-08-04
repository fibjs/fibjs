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
	asyncBuffer(BufferedStream* pThis, exlib::AsyncEvent* ac) :
			m_bFirst(true), m_pThis(pThis), m_ac(ac)
	{
	}

	virtual bool process(bool end)
	{
		return true;
	}

	virtual void post(int v)
	{
		result_t hr = v;

		while (hr != CALL_E_PENDDING)
		{
			bool streamEnd = false;
			bool done = false;

			if (!m_bFirst)
			{
				if (hr < 0)
				{
					m_ac->post(hr);
					delete this;
					return;
				}

				if (m_buf)
				{
					m_buf->toString(m_pThis->m_buf);
					m_buf.Release();
				}
				else
					streamEnd = true;
			}
			else
				m_bFirst = false;

			done = process(streamEnd);

			if (done)
			{
				m_ac->post(0);
				delete this;
				return;
			}

			m_pThis->m_buf.resize(0);
			m_pThis->m_pos = 0;

			hr = m_pThis->m_stm->read(-1, m_buf, this);
		}
	}

public:
	bool m_bFirst;
	BufferedStream* m_pThis;
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
		asyncRead(BufferedStream* pThis, int32_t bytes,
				obj_ptr<Buffer_base>& retVal, exlib::AsyncEvent* ac) :
				asyncBuffer(pThis, ac), m_bytes(bytes), m_retVal(retVal)
		{
		}

		static bool process(BufferedStream* pThis, int32_t bytes,
				obj_ptr<Buffer_base>& retVal, bool streamEnd)
		{
			int n = bytes - (int) pThis->m_strbuf.size();
			int n1 = (int) pThis->m_buf.length() - pThis->m_pos;

			if (n > n1)
				n = n1;

			if (n > 0)
			{
				if (pThis->m_pos == 0 && n == (int) pThis->m_buf.length())
					pThis->m_strbuf.append(pThis->m_buf);
				else
				{
					std::string s1(pThis->m_buf.substr(pThis->m_pos, n));
					pThis->m_strbuf.append(s1);
				}
				pThis->m_pos += n;
			}

			if (streamEnd || bytes == (int) pThis->m_strbuf.size())
			{
				std::string s = pThis->m_strbuf.str();

				if (s.length())
					retVal = new Buffer(s);

				return true;
			}

			return false;
		}

		virtual bool process(bool streamEnd)
		{
			return process(m_pThis, m_bytes, m_retVal, streamEnd);
		}

	public:
		int32_t m_bytes;
		obj_ptr<Buffer_base>& m_retVal;
	};

	if (bytes < 0)
	{
		int n = (int) m_buf.length() - m_pos;
		if (n > 0)
		{
			if (m_pos == 0)
				retVal = new Buffer(m_buf);
			else
			{
				std::string s1(m_buf.substr(m_pos, n));
				retVal = new Buffer(s1);
			}
			m_pos += n;

			return 0;
		}
		else
			return m_stm->read(bytes, retVal, ac);
	}

	if (!ac)
	{
		if (asyncRead::process(this, bytes, retVal, false))
			return 0;

		return CALL_E_NOSYNC;
	}

	(new asyncRead(this, bytes, retVal, ac))->post(0);
	return CALL_E_PENDDING;
}

result_t BufferedStream::asyncRead(int32_t bytes)
{
	acb_read(bytes);
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
	acb_write(data);
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
	acb_copyTo(stm, bytes);
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
	acb_stat();
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
	return readTextUntil(m_eol.c_str(), retVal, ac);
}

result_t BufferedStream::readTextUntil(const char* mk, std::string& retVal,
		exlib::AsyncEvent* ac)
{
	class asyncRead: public asyncBuffer
	{
	public:
		asyncRead(BufferedStream* pThis, const char* mk, std::string& retVal,
				exlib::AsyncEvent* ac) :
				asyncBuffer(pThis, ac), m_mk(mk), m_retVal(retVal)
		{
		}

		virtual bool process(bool streamEnd)
		{
			return false;
		}

	public:
		const char* m_mk;
		std::string& m_retVal;
	};

	if (!ac)
		return CALL_E_NOSYNC;

	(new asyncRead(this, mk, retVal, ac))->post(0);
	return CALL_E_PENDDING;
}

result_t BufferedStream::readUntil(const char* mk, obj_ptr<Buffer_base>& retVal,
		exlib::AsyncEvent* ac)
{
	std::string buf;

	result_t hr = readTextUntil(mk, buf, ac);
	if (hr < 0)
		return hr;

	retVal = new Buffer(buf);

	return 0;
}

result_t BufferedStream::get_EOL(std::string& retVal)
{
	retVal = m_eol;
	return 0;
}

result_t BufferedStream::set_EOL(const char* newVal)
{
	if (newVal[0] == '\r' && newVal[1] == '\2')
		m_eol.assign(newVal, 2);
	else if (newVal[1] == '\0' && (newVal[0] == '\r' || newVal[0] == '\n'))
		m_eol.assign(newVal, 1);
	else
		return CALL_E_INVALIDARG;

	return 0;
}

} /* namespace fibjs */
