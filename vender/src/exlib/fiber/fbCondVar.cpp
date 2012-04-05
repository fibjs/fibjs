/*
 *  fbCondVar.cpp
 *  Created on: Mar 19, 2012
 *
 *  Copyright (c) 2012 by Leo Hoo
 *  lion@9465.net
 */

#include <exlib/fiber.h>

namespace exlib
{

void CondVar::wait(Locker& l)
{
    Service* pService = Service::getTLSService();

    m_blocks.put(pService->m_running);

    l.unlock();
    pService->switchtonext();
    l.lock();
}

void CondVar::notify_one()
{
    if(!m_blocks.empty())
    {
        Service* pService = Service::getTLSService();
        Fiber* cntxt;

        cntxt = m_blocks.get();
        pService->m_resume.put(cntxt);
    }
}

void CondVar::notify_all()
{
    if(!m_blocks.empty())
    {
        Service* pService = Service::getTLSService();
        Fiber* cntxt;

        while(!m_blocks.empty())
        {
            cntxt = m_blocks.get();
            pService->m_resume.put(cntxt);
        }
    }
}

}
