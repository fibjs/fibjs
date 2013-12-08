/*
 * TCPServer.h
 *
 *  Created on: Aug 8, 2012
 *      Author: lion
 */

#include "ifs/net.h"
#include "ifs/TCPServer.h"
#include "Socket.h"
#include "Stats.h"

#ifndef TCPSERVER_H_
#define TCPSERVER_H_

namespace fibjs
{

class TCPServer: public TCPServer_base
{
public:
	TCPServer();

	FIBER_FREE();

public:
	// TCPServer_base
	virtual result_t run(exlib::AsyncEvent* ac);
	virtual result_t asyncRun();
	virtual result_t get_socket(obj_ptr<Socket_base>& retVal);
	virtual result_t get_stats(obj_ptr<Stats_base>& retVal);

public:
	result_t create(const char* addr, int32_t port, Handler_base* listener);

private:
	bool m_running;
	obj_ptr<Socket_base> m_socket;
	obj_ptr<Handler_base> m_hdlr;
	obj_ptr<Stats> m_stats;
};

} /* namespace fibjs */
#endif /* TCPSERVER_H_ */
