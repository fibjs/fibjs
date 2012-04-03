/*
 *  fbTls.cpp
 *  Created on: Mar 19, 2012
 *
 *  Copyright (c) 2012 by Leo Hoo
 *  lion@9465.net
 */

#include <exlib/fiber.h>
#include <stdio.h>

namespace exlib
{

int Service::tlsAlloc()
{
    Service* pService = Service::getTLSService();
    int i;

    for(i = 0; i < TLS_SIZE; i ++)
        if(pService->m_tls[i] == 0)
        {
            pService->m_tls[i] = 1;
            return i;
        }

    return -1;
}

void* Service::tlsGet(int idx)
{
    return Service::getTLSService()->m_running->m_tls[idx];
}

void Service::tlsPut(int idx, void* v)
{
    Service::getTLSService()->m_running->m_tls[idx] = v;
}

void Service::tlsFree(int idx)
{
    getTLSService()->m_tls[idx] = 0;
}

}

