/*
 *  fbEvent.cpp
 *  Created on: Mar 19, 2012
 *
 *  Copyright (c) 2012 by Leo Hoo
 *  lion@9465.net
 */

#include <fiber.h>

namespace fiber
{
void Event::wait()
{
    if(!m_set)
    {
        Service* pService = Service::getTLSService();

        m_blocks.put(pService->m_running);
        pService->switchtonext();
    }
}

void Event::pulse()
{
    Service* pService = Service::getTLSService();
    Fiber* cntxt;

    while(!m_blocks.empty())
    {
        cntxt = m_blocks.get();
        pService->m_resume.put(cntxt);
    }
}

void Event::set()
{
    m_set = true;
    pulse();
}

void Event::reset()
{
    m_set = false;
}

bool Event::trywait()
{
    return m_set;
}

}
