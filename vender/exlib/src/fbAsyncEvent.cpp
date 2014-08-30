/*
 *  fbAsyncEvent.cpp
 *  Created on: Mar 19, 2012
 *
 *  Copyright (c) 2012 by Leo Hoo
 *  lion@9465.net
 */

#include "fiber.h"

namespace exlib
{

int AsyncEvent::post(int v)
{
	m_v = v;
    Service::root->m_aEvents.put(this);

    return 0;
}

}
