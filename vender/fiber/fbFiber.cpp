/*
 *  fbService.cpp
 *  Created on: Mar 19, 2012
 *
 *  Copyright (c) 2012 by Leo Hoo
 *  lion@9465.net
 */

#include <osconfig.h>
#include <fiber.h>
#include <stdio.h>
#include <stdlib.h>

namespace fiber
{

Fiber* Fiber::Current()
{
    return Service::getTLSService()->m_running;
}

void Fiber::join()
{
    Service* pService = Service::getTLSService();

    m_join = pService->m_running;
    pService->switchtonext();
}

void Fiber::yield()
{
    Service* pService = Service::getTLSService();

    if(!pService->m_resume.empty())
    {
        pService->m_resume.put(pService->m_running);
        pService->switchtonext();
    }
}

}
