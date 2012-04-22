/*
 * net.cpp
 *
 *  Created on: Apr 22, 2012
 *      Author: lion
 */

#include "Socket.h"
#include "ifs/net.h"

namespace fibjs
{

result_t net_base::socket(int32_t family, int32_t type, obj_ptr<Socket_base>& retVal)
{
	obj_ptr<Socket> sock = new Socket();

	result_t hr = sock->create(family, type);
	if(hr < 0)
		return hr;

	retVal = sock;
	return 0;
}

}
