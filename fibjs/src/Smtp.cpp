/*
 * Smtp.cpp
 *
 *  Created on: Dec 5, 2013
 *      Author: lion
 */

#include "Smtp.h"
#include "Buffer.h"
#include "ifs/encoding.h"

namespace fibjs
{

result_t Smtp_base::_new(obj_ptr<Smtp_base>& retVal)
{
	retVal = new Smtp();
	return 0;
}

#define SMTP_MAX_LINE 1024

class asyncSmtp: public asyncState
{
public:
	asyncSmtp(Smtp* pThis, std::string& retVal, exlib::AsyncEvent* ac) :
			asyncState(ac), m_pThis(pThis), m_retVal(retVal)
	{
		m_stmBuffered = pThis->m_stmBuffered;
		set(ok);
	}

	asyncSmtp(Smtp* pThis, exlib::AsyncEvent* ac) :
			asyncState(ac), m_pThis(pThis), m_retVal(m_strLine)
	{
		m_stmBuffered = pThis->m_stmBuffered;
		set(ok);
	}

	static int ok(asyncState* pState, int n)
	{
		asyncSmtp* pThis = (asyncSmtp*) pState;

		pThis->set(recv);
		return pThis->m_stmBuffered->readLine(SMTP_MAX_LINE, pThis->m_strLine,
				pThis);
	}

	static int recv(asyncState* pState, int n)
	{
		asyncSmtp* pThis = (asyncSmtp*) pState;

		std::string& strLine = pThis->m_strLine;

		if (&pThis->m_retVal != &strLine)
		{
			if (pThis->m_retVal.length())
				pThis->m_retVal.append("\r\n", 2);
			pThis->m_retVal.append(strLine);
		}

		if (qisdigit(strLine[0]) && qisdigit(strLine[1]) && qisdigit(strLine[2])
				&& strLine[3] == ' ')
		{
			if (strLine[0] == '5')
				return Runtime::setError(strLine);

			return pThis->recv_ok();
		}

		return pThis->m_stmBuffered->readLine(SMTP_MAX_LINE, strLine, pThis);
	}

	int connect(const char* host, int32_t port)
	{
		int r = m_pThis->m_sock->connect(host, port, this);

		if (r == CALL_E_PENDDING)
			return r;
		return post(r);
	}

	int send(Buffer_base* data)
	{
		int r = m_stmBuffered->write(data, this);

		if (r == CALL_E_PENDDING)
			return r;
		return post(r);
	}

	int command(const char* cmd, const char* arg)
	{
		std::string s(cmd);

		s.append(" ", 1);
		s.append(arg);
		s.append("\r\n", 2);

		obj_ptr<Buffer> buf = new Buffer(s);
		return send(buf);
	}

	virtual int recv_ok()
	{
		return done(0);
	}

protected:
	obj_ptr<Smtp> m_pThis;
	std::string& m_retVal;
	obj_ptr<BufferedStream_base> m_stmBuffered;
	std::string m_strLine;
};

result_t Smtp::connect(const char* host, int32_t port, int32_t family,
		exlib::AsyncEvent* ac)
{
	if (m_sock)
		return CALL_E_INVALID_CALL;

	if (family != net_base::_AF_INET && family != net_base::_AF_INET6)
		return CALL_E_INVALIDARG;

	if (!ac)
		return CALL_E_NOSYNC;

	result_t hr;

	hr = Socket_base::_new(family, net_base::_SOCK_STREAM, m_sock);
	if (hr < 0)
		return hr;

	m_stmBuffered = new BufferedStream(m_sock);
	m_stmBuffered->set_EOL("\r\n");

	return (new asyncSmtp(this, ac))->connect(host, port);
}

result_t Smtp::command(const char* cmd, const char* arg, std::string& retVal,
		exlib::AsyncEvent* ac)
{
	if (!m_sock)
		return CALL_E_INVALID_CALL;

	if (!ac)
		return CALL_E_NOSYNC;

	return (new asyncSmtp(this, retVal, ac))->command(cmd, arg);
}

result_t Smtp::command(const char* cmd, const char* arg, exlib::AsyncEvent* ac)
{
	if (!m_sock)
		return CALL_E_INVALID_CALL;

	if (!ac)
		return CALL_E_NOSYNC;

	return (new asyncSmtp(this, ac))->command(cmd, arg);
}

result_t Smtp::hello(const char* hostname, exlib::AsyncEvent* ac)
{
	return command("HELO", hostname, ac);
}

result_t Smtp::login(const char* username, const char* password,
		exlib::AsyncEvent* ac)
{
	class asyncLogin: public asyncSmtp
	{
	public:
		asyncLogin(Smtp* pThis, const char* username, const char* password,
				exlib::AsyncEvent* ac) :
				asyncSmtp(pThis, ac), m_username(username), m_password(
						password), step(0)
		{
			set(begin);
		}

		static int begin(asyncState* pState, int n)
		{
			asyncLogin* pThis = (asyncLogin*) pState;

			std::string s("AUTH LOGIN\r\n", 12);

			obj_ptr < Buffer > buf = new Buffer(s);
			pThis->set(ok);
			return pThis->m_stmBuffered->write(buf, pThis);
		}

		int send_base64(const char* str)
		{
			obj_ptr < Buffer > buf = new Buffer(str);
			std::string s;

			encoding_base::base64Encode(buf, s);
			s.append("\r\n", 2);

			buf = new Buffer(s);

			set (ok);
			return m_stmBuffered->write(buf, this);
		}

		static int send_username(asyncState* pState, int n)
		{
			asyncLogin* pThis = (asyncLogin*) pState;
			return pThis->send_base64(pThis->m_username);
		}

		static int send_password(asyncState* pState, int n)
		{
			asyncLogin* pThis = (asyncLogin*) pState;
			return pThis->send_base64(pThis->m_password);
		}

		virtual int recv_ok()
		{
			switch (step)
			{
			case 0:
				step++;
				set(send_username);
				return 0;
			case 1:
				step++;
				set(send_password);
				return 0;
			}
			return done(0);
		}

	private:
		const char* m_username;
		const char* m_password;
		int step;
	};

	if (!m_sock)
		return CALL_E_INVALID_CALL;

	if (!ac)
		return CALL_E_NOSYNC;

	return (new asyncLogin(this, username, password, ac))->post(0);
}

result_t Smtp::from(const char* adress, exlib::AsyncEvent* ac)
{
	return command("MAIL FROM", adress, ac);
}

result_t Smtp::to(const char* adress, exlib::AsyncEvent* ac)
{
	return command("RCPT TO", adress, ac);
}

result_t Smtp::data(const char* txt, exlib::AsyncEvent* ac)
{
	class asyncData: public asyncSmtp
	{
	public:
		asyncData(Smtp* pThis, const char* txt, exlib::AsyncEvent* ac) :
				asyncSmtp(pThis, ac), m_txt(txt), step(0)
		{
			set(begin);
		}

		static int begin(asyncState* pState, int n)
		{
			asyncData* pThis = (asyncData*) pState;

			std::string s("DATA\r\n", 6);

			obj_ptr < Buffer > buf = new Buffer(s);
			pThis->set(ok);
			return pThis->m_stmBuffered->write(buf, pThis);
		}

		static int send_data(asyncState* pState, int n)
		{
			asyncData* pThis = (asyncData*) pState;

			std::string s(pThis->m_txt);

			s.append("\r\n.\r\n", 5);

			obj_ptr < Buffer > buf = new Buffer(s);
			pThis->set(ok);
			return pThis->m_stmBuffered->write(buf, pThis);
		}

		virtual int recv_ok()
		{
			switch (step)
			{
			case 0:
				step++;
				set(send_data);
				return 0;
			}
			return done(0);
		}

	private:
		const char* m_txt;
		int step;
	};

	if (!m_sock)
		return CALL_E_INVALID_CALL;

	if (!ac)
		return CALL_E_NOSYNC;

	return (new asyncData(this, txt, ac))->post(0);
}

result_t Smtp::quit(exlib::AsyncEvent* ac)
{
	if (!m_sock)
		return CALL_E_INVALID_CALL;

	if (!ac)
		return CALL_E_NOSYNC;

	asyncSmtp* pSmtp = new asyncSmtp(this, ac);

	m_sock.Release();
	m_stmBuffered.Release();

	return pSmtp->command("QUIT", "");
}

result_t Smtp::get_socket(obj_ptr<Socket_base>& retVal)
{
	retVal = m_sock;
	return 0;
}

} /* namespace fibjs */
