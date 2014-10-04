/*
 *  fbTls.cpp
 *  Created on: Mar 19, 2012
 *
 *  Copyright (c) 2012 by Leo Hoo
 *  lion@9465.net
 */

#include <stdio.h>
#include "service.h"

namespace exlib
{

int Fiber::tlsAlloc()
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

void* Fiber::tlsGet(int idx)
{
	Service* pService = Service::getFiberService();

	if (pService)
		return pService->m_running->m_tls[idx];

	return NULL;
}

void Fiber::tlsPut(int idx, void* v)
{
	Service* pService = Service::getFiberService();

	if (pService)
		pService->m_running->m_tls[idx] = v;
}

void Fiber::tlsFree(int idx)
{
	Service* pService = Service::getFiberService();

	if (pService)
		pService->m_tls[idx] = 0;
}

}

