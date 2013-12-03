/*
 * inetAddr.h
 *
 *  Created on: Jul 10, 2012
 *      Author: lion
 */

#include "ifs/net.h"
#include <string.h>

#ifndef INETADDR_H_
#define INETADDR_H_

namespace fibjs
{

union inetAddr
{
	struct sockaddr_in addr4;
	struct sockaddr_in6 addr6;

	int size()
	{
		return addr6.sin6_family == PF_INET6 ?
				(int) sizeof(addr6) : (int) sizeof(addr4);
	}

	int family()
	{
		return addr6.sin6_family == PF_INET6 ?
				net_base::_AF_INET6 : net_base::_AF_INET;
	}

	int port()
	{
		return addr4.sin_port;
	}

	void init(int32_t family)
	{
		memset(this, 0, sizeof(inetAddr));

		if (family == net_base::_AF_INET)
			addr4.sin_family = PF_INET;
		else
			addr6.sin6_family = PF_INET6;
	}

	void setPort(int32_t port)
	{
		addr4.sin_port = htons(port);
	}

	int addr(const char* s);
	std::string str();
};

} /* namespace fibjs */
#endif /* INETADDR_H_ */
