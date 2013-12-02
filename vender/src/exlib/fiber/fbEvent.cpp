/*
 *  fbEvent.cpp
 *  Created on: Mar 19, 2012
 *
 *  Copyright (c) 2012 by Leo Hoo
 *  lion@9465.net
 */

#include <exlib/fiber.h>

namespace exlib
{
void Event::wait()
{
	if (!m_set)
	{
		Service* pService = Service::getFiberService();

		if (pService)
		{
			m_blocks.put(pService->m_running);
			pService->switchtonext();
		}
	}
}

void Event::pulse()
{
	Service* pService = Service::getFiberService();
	Fiber* cntxt;

	if (pService)
		while (!m_blocks.empty())
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

bool Event::isSet()
{
	return m_set;
}

}
