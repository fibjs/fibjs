/*
 *  fbSemaphore.cpp
 *  Created on: Mar 19, 2012
 *
 *  Copyright (c) 2012 by Leo Hoo
 *  lion@9465.net
 */

#include <fiber.h>

namespace fiber
{

void Semaphore::wait()
{
    if(m_count == 0)
    {
        Service* pService = Service::getTLSService();

        m_blocks.push_back(pService->m_running);
        pService->switchto(&pService->m_main, FB_SUSPEND);
    }else
        m_count --;
}

void Semaphore::post()
{
    if(m_count == 0 && !m_blocks.empty())
    {
        Service* pService = Service::getTLSService();
        Fiber* cntxt;

        cntxt = m_blocks.front();
        m_blocks.pop_front();
        pService->m_resume.push_back(cntxt);
    }else m_count ++;
}

bool Semaphore::trywait()
{
    if(m_count == 0)
        return false;

    m_count --;
    return true;
}

}
