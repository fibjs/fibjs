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

        m_blocks.put(pService->m_running);
        pService->switchtonext();
    }else
        m_count --;
}

void Semaphore::post()
{
    if(m_count == 0 && !m_blocks.empty())
    {
        Service* pService = Service::getTLSService();
        Fiber* cntxt;

        cntxt = m_blocks.get();
        pService->m_resume.put(cntxt);
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
