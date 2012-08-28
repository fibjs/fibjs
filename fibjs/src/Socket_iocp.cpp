/*
 * Socket_iocp.cpp
 *
 *  Created on: May 22, 2012
 *      Author: lion
 */

#ifdef _WIN32

#include "Socket.h"
#include "ifs/net.h"
#include "Buffer.h"
#include <fcntl.h>
#include <mswsock.h>
#include <mstcpip.h>
#include <exlib/thread.h>

namespace fibjs
{

void setOption(SOCKET s)
{
	int keepAlive = 1;

	tcp_keepalive Settings =
	{ 1, KEEPALIVE_TIMEOUT * 1000, KEEPALIVE_TIMEOUT * 1000 };
	DWORD dwBytes = 0L;

	setsockopt(s, SOL_SOCKET, SO_KEEPALIVE, (const char *) &keepAlive,
			sizeof(keepAlive));
	if (WSAIoctl(s, SIO_KEEPALIVE_VALS, &Settings, sizeof(Settings), NULL, 0,
			&dwBytes, NULL, NULL))
		puts("WSAIoctl");

	int noDelay = 1;

	setsockopt(s, IPPROTO_TCP, TCP_NODELAY, (const char *) &noDelay,
			sizeof(noDelay));
}

HANDLE s_hIocp;

class asyncProc: public OVERLAPPED
{
public:
	asyncProc(SOCKET s, exlib::AsyncEvent* ac) :
			m_s(s), m_ac(ac), m_next(NULL)
	{
		memset((OVERLAPPED*) this, 0, sizeof(OVERLAPPED));
	}

	virtual ~asyncProc()
	{
	}

	result_t call()
	{
		PostQueuedCompletionStatus(s_hIocp, 0, (ULONG_PTR) s_hIocp, this);
		return CALL_E_PENDDING;
	}

	void proc()
	{
		result_t hr = process();
		if (hr != CALL_E_PENDDING)
			asyncProc::ready(0, hr);
	}

	virtual result_t process()
	{
		return 0;
	}

	virtual void ready(DWORD dwBytes, int nError)
	{
		m_ac->post(nError);
		delete this;
	}

public:
	SOCKET m_s;
	exlib::AsyncEvent* m_ac;
	asyncProc* m_next;
};

static class _acSocket: public exlib::OSThread
{
public:
	_acSocket()
	{
		WSADATA wsaData;
		int iResult;

		iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
		if (iResult != 0)
		{
			printf("WSAStartup failed: %d\n", iResult);
			exit(-1);
		}

		s_hIocp = ::CreateIoCompletionPort(INVALID_HANDLE_VALUE, 0, 0, 0);

		start();
	}

	virtual void Run()
	{
		int bRet;
		ULONG_PTR v;
		DWORD dwBytes, dwError;
		LPOVERLAPPED pOverlap;

		while (true)
		{
			v = 0;
			pOverlap = NULL;

			bRet = GetQueuedCompletionStatus(s_hIocp, &dwBytes, &v, &pOverlap,
					10000);
			if (!bRet)
				dwError = ::GetLastError();
			else
				dwError = 0;

			if (bRet || (dwError != WAIT_TIMEOUT))
			{
				if ((HANDLE) v == s_hIocp)
					((asyncProc*) pOverlap)->proc();
				else
					((asyncProc*) pOverlap)->ready(dwBytes, -(int) dwError);
			}
		}
	}

} s_acSock;

result_t net_base::backend(std::string& retVal)
{
	retVal = "IOCP";
	return 0;
}

result_t Socket::connect(const char* host, int32_t port, exlib::AsyncEvent* ac)
{
	class asyncConnect: public asyncProc
	{
	public:
		asyncConnect(SOCKET s, inetAddr& ai, exlib::AsyncEvent* ac) :
				asyncProc(s, ac), m_ai(ai)
		{
		}

		virtual result_t process()
		{
			static LPFN_CONNECTEX ConnectEx;
			int nError;

			if (!ConnectEx)
			{
				GUID guidConnectEx = WSAID_CONNECTEX;
				DWORD dwBytes;

				if (SOCKET_ERROR
						== WSAIoctl(m_s, SIO_GET_EXTENSION_FUNCTION_POINTER,
								&guidConnectEx, sizeof(guidConnectEx),
								&ConnectEx, sizeof(ConnectEx), &dwBytes, NULL,
								NULL))
					return SocketError();
			}

			if (ConnectEx(m_s, (sockaddr*) &m_ai, (int) m_ai.size(), NULL, 0,
					NULL, this))
				return CALL_E_PENDDING;

			nError = WSAGetLastError();
			return (nError == WSA_IO_PENDING) ? CALL_E_PENDDING : -nError;
		}

		virtual void ready(DWORD dwBytes, int nError)
		{
			if (!nError)
			{
				setsockopt(m_s, SOL_SOCKET, SO_UPDATE_CONNECT_CONTEXT, NULL, 0);
				setOption(m_s);
			}
			asyncProc::ready(dwBytes, nError);
		}

	public:
		inetAddr m_ai;
	};

	if (m_sock == INVALID_SOCKET)
		return CALL_E_INVALID_CALL;

	if (!ac)
		return CALL_E_NOSYNC;

	result_t hr;
	inetAddr addr_info;

	addr_info.init(m_family);
	addr_info.setPort(port);
	if (addr_info.addr(host) < 0)
	{
		std::string strAddr;
		result_t hr = net_base::resolve(host, m_family, strAddr, ac);
		if (hr < 0)
			return hr;

		if (addr_info.addr(strAddr.c_str()) < 0)
			return CALL_E_INVALIDARG;
	}

	if (!m_bBind)
	{
		hr = bind(0, TRUE);
		if (hr < 0)
			return hr;
	}

	(new asyncConnect(m_sock, addr_info, ac))->proc();
	return CALL_E_PENDDING;
}

result_t Socket::accept(obj_ptr<Socket_base>& retVal, exlib::AsyncEvent* ac)
{
	class asyncAccept: public asyncProc
	{
	public:
		asyncAccept(SOCKET s, SOCKET sListen, obj_ptr<Socket_base>& retVal,
				exlib::AsyncEvent* ac) :
				asyncProc(s, ac), m_sListen(sListen), m_retVal(retVal)
		{
		}

		virtual result_t process()
		{
			static LPFN_ACCEPTEX AcceptEx;
			int nError;

			if (!AcceptEx)
			{
				GUID guidAcceptEx = WSAID_ACCEPTEX;
				DWORD dwBytes;

				if (SOCKET_ERROR
						== WSAIoctl(m_s, SIO_GET_EXTENSION_FUNCTION_POINTER,
								&guidAcceptEx, sizeof(guidAcceptEx), &AcceptEx,
								sizeof(AcceptEx), &dwBytes, NULL, NULL))
					return SocketError();
			}

			if (AcceptEx(m_sListen, m_s, &m_Buf, 0, sizeof(inetAddr) + 16,
					sizeof(inetAddr) + 16, NULL, this))
				return CALL_E_PENDDING;

			nError = GetLastError();
			return (nError == ERROR_IO_PENDING) ? CALL_E_PENDDING : -nError;
		}

		virtual void ready(DWORD dwBytes, int nError)
		{
			if (!nError)
			{
				setsockopt(m_s, SOL_SOCKET, SO_UPDATE_ACCEPT_CONTEXT,
						(char *) &m_sListen, sizeof(m_sListen));
				setOption(m_s);
			}
			asyncProc::ready(dwBytes, 0);
		}

	public:
		SOCKET m_sListen;
		obj_ptr<Socket_base>& m_retVal;
		char m_Buf[(sizeof(inetAddr) + 16) * 2];
	};

	if (m_sock == INVALID_SOCKET)
		return CALL_E_INVALID_CALL;

	if (!ac)
		return CALL_E_NOSYNC;

	obj_ptr<Socket> s = new Socket();
	result_t hr = s->create(m_family, m_type);
	if (hr < 0)
		return hr;

	retVal = s;

	asyncAccept* pa = new asyncAccept(s->m_sock, m_sock, retVal, ac);
	s.Release();

	pa->proc();
	return CALL_E_PENDDING;
}

result_t Socket::recv(int32_t bytes, obj_ptr<Buffer_base>& retVal,
		exlib::AsyncEvent* ac, bool bRead)
{
	class asyncRecv: public asyncProc
	{
	public:
		asyncRecv(SOCKET s, int32_t bytes, obj_ptr<Buffer_base>& retVal,
				exlib::AsyncEvent* ac, bool bRead) :
				asyncProc(s, ac), m_retVal(retVal), m_pos(0), m_bRead(bRead)
		{
			m_buf.resize(bytes > 1 ? bytes : STREAM_BUFF_SIZE);
		}

		virtual result_t process()
		{
			int nError;

			if (ReadFile((HANDLE) m_s, &m_buf[m_pos],
					(DWORD) m_buf.length() - m_pos, NULL, this))
				return CALL_E_PENDDING;

			nError = GetLastError();

			if (nError == ERROR_NETNAME_DELETED)
				return CALL_RETURN_NULL;

			return (nError == ERROR_IO_PENDING) ? CALL_E_PENDDING : -nError;
		}

		virtual void ready(DWORD dwBytes, int nError)
		{
			if (nError == -ERROR_NETNAME_DELETED)
			{
				nError = 0;
				dwBytes = 0;
			}

			if(dwBytes == 0)
				m_bRead = false;

			if (!nError)
			{
				m_pos += dwBytes;

				if (m_bRead && m_pos < m_buf.length())
				{
					proc();
					return;
				}

				if (m_pos)
				{
					m_buf.resize(m_pos);
					m_retVal = new Buffer(m_buf);
				}
				else
					nError = CALL_RETURN_NULL;
			}

			asyncProc::ready(dwBytes, nError);
		}

	public:
		obj_ptr<Buffer_base>& m_retVal;
		int m_pos;
		bool m_bRead;
		std::string m_buf;
	};

	if (m_sock == INVALID_SOCKET)
		return CALL_E_INVALID_CALL;

	if (!ac)
		return CALL_E_NOSYNC;

	return (new asyncRecv(m_sock, bytes, retVal, ac, bRead))->call();
}

result_t Socket::send(obj_ptr<Buffer_base>& data, exlib::AsyncEvent* ac)
{
	class asyncSend: public asyncProc
	{
	public:
		asyncSend(SOCKET s, obj_ptr<Buffer_base>& data, exlib::AsyncEvent* ac) :
				asyncProc(s, ac)
		{
			data->toString(m_buf);
			m_p = m_buf.c_str();
			m_sz = (int) m_buf.length();
		}

		virtual result_t process()
		{
			int nError;

			if (WriteFile((HANDLE) m_s, (LPCSTR) m_p, m_sz, NULL, this))
				return CALL_E_PENDDING;

			nError = GetLastError();
			return (nError == ERROR_IO_PENDING) ? CALL_E_PENDDING : -nError;
		}

		virtual void ready(DWORD dwBytes, int nError)
		{
			if (!nError)
			{
				m_p += dwBytes;
				m_sz -= dwBytes;

				if (m_sz > 0)
				{
					proc();
					return;
				}
			}

			asyncProc::ready(dwBytes, nError);
		}

	public:
		std::string m_buf;
		const char* m_p;
		int m_sz;
	};

	if (m_sock == INVALID_SOCKET)
		return CALL_E_INVALID_CALL;

	if (!ac)
		return CALL_E_NOSYNC;

	return (new asyncSend(m_sock, data, ac))->call();
}

}

#endif
