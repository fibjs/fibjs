/*
 *  fbTls.cpp
 *  Created on: Mar 19, 2012
 *
 *  Copyright (c) 2012 by Leo Hoo
 *  lion@9465.net
 */

#include <stdio.h>
#include "fiber.h"

namespace exlib
{

int Service::tlsAlloc()
{
	Service* pService = Service::getFiberService();
	int i;

	if (pService)
	{
		for (i = 0; i < TLS_SIZE; i++)
			if (pService->m_tls[i] == 0)
			{
				pService->m_tls[i] = 1;
				return i;
			}
	}

	return -1;
}

void* Service::tlsGet(int idx)
{
	Service* pService = Service::getFiberService();

	if (pService)
		return pService->m_running->m_tls[idx];

	return NULL;
}

void Service::tlsPut(int idx, void* v)
{
	Service* pService = Service::getFiberService();

	if (pService)
		pService->m_running->m_tls[idx] = v;
}

void Service::tlsFree(int idx)
{
	Service* pService = Service::getFiberService();

	if (pService)
		pService->m_tls[idx] = 0;
}

}

