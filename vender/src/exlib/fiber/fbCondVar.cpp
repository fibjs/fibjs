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
	l.unlock();

	Service* pService = Service::getFiberService();
	m_blocks.put(pService->m_running);
	pService->switchtonext();

	l.lock();
}

void CondVar::notify_one()
{
	if (!m_blocks.empty())
	{
		Service* pService = Service::getFiberService();
		Fiber* cntxt;

		cntxt = m_blocks.get();
		pService->m_resume.put(cntxt);
	}
}

void CondVar::notify_all()
{
	if (!m_blocks.empty())
	{
		Service* pService = Service::getFiberService();
		Fiber* cntxt;

		while (!m_blocks.empty())
		{
			cntxt = m_blocks.get();
			pService->m_resume.put(cntxt);
		}
	}
}

}
