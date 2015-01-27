/*
 *  fbEvent.cpp
 *  Created on: Mar 19, 2012
 *
 *  Copyright (c) 2012 by Leo Hoo
 *  lion@9465.net
 */

#include "service.h"

namespace exlib
{
void Event::wait()
{
    if (!m_set)
    {
        Service *pService = Service::getFiberService();

        if (pService)
        {
            m_blocks.put(pService->m_running);
            pService->switchConext();
        }
    }
}

void Event::pulse()
{
    Service *pService = Service::getFiberService();

    if (pService)
        while (!m_blocks.empty())
            pService->m_resume.put(m_blocks.get());
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

bool Event::isSet()
{
    return m_set;
}

}
