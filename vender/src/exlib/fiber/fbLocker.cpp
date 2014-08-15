/*
 *  fbLocker.cpp
 *  Created on: Mar 19, 2012
 *
 *  Copyright (c) 2012 by Leo Hoo
 *  lion@9465.net
 */

#include <exlib/fiber.h>

namespace exlib
{

void Locker::lock()
{
    Service *pService = Service::getFiberService();

    if (pService)
    {
        Fiber *current = pService->m_running;

        if (m_locked && current != m_locker)
        {
            m_blocks.put(current);
            pService->switchtonext();
        }
        else if (++m_count == 1)
        {
            m_locked = true;
            m_locker = current;
        }
    }
}

bool Locker::trylock()
{
    Service *pService = Service::getFiberService();

    if (pService)
    {
        Fiber *current = pService->m_running;

        if (m_locked && current != m_locker)
            return false;

        if (++m_count == 1)
        {
            m_locked = true;
            m_locker = current;
        }
    }

    return true;
}

void Locker::unlock()
{
    Service *pService = Service::getFiberService();

    if (pService)
    {
        Fiber *current = pService->m_running;

        if (current == m_locker)
        {
            if (--m_count == 0)
            {
                if (m_blocks.empty())
                {
                    m_locked = false;
                    m_locker = 0;
                }
                else
                {
                    m_count++;
                    m_locker = m_blocks.get();
                    pService->m_resume.put(m_locker);
                }
            }
        }
    }
}

bool Locker::owned()
{

    Service *pService = Service::getFiberService();

    if (pService)
        return pService->m_running == m_locker;

    return false;
}

}
