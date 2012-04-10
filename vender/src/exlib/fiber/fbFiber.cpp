/*
 *  fbService.cpp
 *  Created on: Mar 19, 2012
 *
 *  Copyright (c) 2012 by Leo Hoo
 *  lion@9465.net
 */

#include <osconfig.h>
#include <exlib/fiber.h>
#include <stdio.h>
#include <stdlib.h>

namespace exlib
{

Fiber* Fiber::Current()
{
    return Service::getFiberService()->m_running;
}

void Fiber::join()
{
    Service* pService = Service::getFiberService();

    m_join = pService->m_running;
    pService->switchtonext();
}

void Fiber::yield()
{
    Service::getFiberService()->yield();
}

}
