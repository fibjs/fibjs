/*
 *  fbCondVar.cpp
 *  Created on: Mar 19, 2012
 *
 *  Copyright (c) 2012 by Leo Hoo
 *  lion@9465.net
 */

#include <fiber.h>

namespace fiber
{

void CondVar::wait(Locker& l)
{
    Service* pService = Service::getTLSService();

    m_blocks.push_back(pService->m_running);

    l.unlock();
    pService->switchto(&pService->m_main, FB_SUSPEND);
    l.lock();
}

void CondVar::notify_one()
{
    if(!m_blocks.empty())
    {
        Service* pService = Service::getTLSService();
        Fiber* cntxt;

        cntxt = m_blocks.front();
        m_blocks.pop_front();
        pService->m_resume.push_back(cntxt);
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
            cntxt = m_blocks.front();
            m_blocks.pop_front();
            pService->m_resume.push_back(cntxt);
        }
    }
}

}
