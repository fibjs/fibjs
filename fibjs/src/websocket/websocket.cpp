/*
 * WebSocketMessage.cpp
 *
 *  Created on: Sep 11, 2015
 *      Author: lion
 */

#include "ifs/websocket.h"

namespace fibjs
{

result_t websocket_base::connect(const char* url, obj_ptr<Socket_base>& retVal, AsyncEvent* ac)
{
	return 0;
}

}
