/*
 * DnsClient.h
 *
 *  Created on: May 10, 2019
 *      Author: Asion
 */

#include "ifs/dns.h"
#include "DnsClient.h"
#include "inetAddr.h"

namespace fibjs {
DECLARE_MODULE(dns);

static DnsClient* get_DnsClient(Isolate* isolate = NULL)
{
    if (isolate == NULL)
        isolate = Isolate::current();

    if (!isolate->m_dnsclient) {
        DnsClient *c = new DnsClient();
        result_t hr;
        hr = c->m_channel->Setup();
        if(hr < 0)
            return NULL;
        isolate->m_dnsclient = c;
    }
    return (DnsClient*)(obj_base*)isolate->m_dnsclient;
}

result_t dns_base::resolve(exlib::string host, exlib::string type, Variant& retVal, AsyncEvent* ac)
{
    DnsClient *c = get_DnsClient(ac->isolate());
    if(c == NULL)
        return CALL_E_EXCEPTION;
    return c->resolve(host, type, retVal, ac);
}

result_t dns_base::resolve4(exlib::string host, v8::Local<v8::Object> options, obj_ptr<NArray>& retVal, AsyncEvent* ac)
{
    DnsClient *c = get_DnsClient(ac->isolate());
    if(c == NULL)
        return CALL_E_EXCEPTION;
    return c->resolve4(host, options, retVal, ac);
}

result_t dns_base::resolve4(exlib::string host, obj_ptr<NArray>& retVal, AsyncEvent* ac)
{
    DnsClient *c = get_DnsClient(ac->isolate());
    if(c == NULL)
        return CALL_E_EXCEPTION;
    return c->resolve4(host, retVal, ac);
}

result_t dns_base::resolve6(exlib::string host, v8::Local<v8::Object> options, obj_ptr<NArray>& retVal, AsyncEvent* ac)
{
    DnsClient *c = get_DnsClient(ac->isolate());
    if(c == NULL)
        return CALL_E_EXCEPTION;
    return c->resolve6(host, options, retVal, ac);
}

result_t dns_base::resolve6(exlib::string host, obj_ptr<NArray>& retVal, AsyncEvent* ac)
{
    DnsClient *c = get_DnsClient(ac->isolate());
    if(c == NULL)
        return CALL_E_EXCEPTION;
    return c->resolve6(host, retVal, ac);
}

result_t dns_base::resolveSrv(exlib::string host, obj_ptr<NArray>& retVal, AsyncEvent* ac)
{
    DnsClient *c = get_DnsClient(ac->isolate());
    if(c == NULL)
        return CALL_E_EXCEPTION;
    return c->resolveSrv(host, retVal, ac);
}

result_t dns_base::resolveMx(exlib::string host, obj_ptr<NArray>& retVal, AsyncEvent* ac)
{
    DnsClient *c = get_DnsClient(ac->isolate());
    if(c == NULL)
        return CALL_E_EXCEPTION;
    return c->resolveMx(host, retVal, ac);
}

result_t dns_base::resolveTxt(exlib::string host, obj_ptr<NArray>& retVal, AsyncEvent* ac)
{
    DnsClient *c = get_DnsClient(ac->isolate());
    if(c == NULL)
        return CALL_E_EXCEPTION;
    return c->resolveTxt(host, retVal, ac);
}

result_t dns_base::resolveSoa(exlib::string host, obj_ptr<NObject>& retVal, AsyncEvent* ac)
{
    DnsClient *c = get_DnsClient(ac->isolate());
    if(c == NULL)
        return CALL_E_EXCEPTION;
    return c->resolveSoa(host, retVal, ac);
}

result_t dns_base::resolvePtr(exlib::string host, obj_ptr<NArray>& retVal, AsyncEvent* ac)
{
    DnsClient *c = get_DnsClient(ac->isolate());
    if(c == NULL)
        return CALL_E_EXCEPTION;
    return c->resolvePtr(host, retVal, ac);
}

result_t dns_base::resolveNs(exlib::string host, obj_ptr<NArray>& retVal, AsyncEvent* ac)
{
    DnsClient *c = get_DnsClient(ac->isolate());
    if(c == NULL)
        return CALL_E_EXCEPTION;
    return c->resolveNs(host, retVal, ac);
}

result_t dns_base::resolveNaptr(exlib::string host, obj_ptr<NArray>& retVal, AsyncEvent* ac)
{
    DnsClient *c = get_DnsClient(ac->isolate());
    if(c == NULL)
        return CALL_E_EXCEPTION;
    return c->resolveNaptr(host, retVal, ac);
}

result_t dns_base::resolveCname(exlib::string host, obj_ptr<NArray>& retVal, AsyncEvent* ac)
{
    DnsClient *c = get_DnsClient(ac->isolate());
    if(c == NULL)
        return CALL_E_EXCEPTION;
    return c->resolveCname(host, retVal, ac);
}

result_t dns_base::resolveAny(exlib::string host, obj_ptr<NArray>& retVal, AsyncEvent* ac)
{
    DnsClient *c = get_DnsClient(ac->isolate());
    if(c == NULL)
        return CALL_E_EXCEPTION;
    return c->resolveAny(host, retVal, ac);
}
result_t dns_base::reverse(exlib::string ip, obj_ptr<NArray>& retVal, AsyncEvent* ac)
{
    DnsClient *c = get_DnsClient(ac->isolate());
    if(c == NULL)
        return CALL_E_EXCEPTION;
    return c->reverse(ip, retVal, ac);
}
result_t dns_base::cancel()
{
    DnsClient *c = get_DnsClient(Isolate::current());
    if(c == NULL)
        return CALL_E_EXCEPTION;
    return c->cancel();
}
result_t dns_base::setServers(v8::Local<v8::Array> servers, AsyncEvent* ac)
{
    DnsClient *c = get_DnsClient(ac->isolate());
    if(c == NULL)
        return CALL_E_EXCEPTION;
    return c->setServers(servers, ac);
}
result_t dns_base::getServers(obj_ptr<NArray>& retVal)
{
    DnsClient *c = get_DnsClient(Isolate::current());
    if(c == NULL)
        return CALL_E_EXCEPTION;
    return c->getServers(retVal);
}
result_t dns_base::lookup(exlib::string name, exlib::string& retVal, AsyncEvent* ac)
{
    if (ac->isSync())
        return CHECK_ERROR(CALL_E_LONGSYNC);

    addrinfo hints = { 0, AF_UNSPEC, SOCK_STREAM, IPPROTO_TCP, 0, 0, 0, 0 };
    addrinfo* result = NULL;
    addrinfo* ptr = NULL;

    int res = getaddrinfo(name.c_str(), NULL, &hints, &result);
    if (res)
        return CHECK_ERROR(Runtime::setError(gai_strerror(res)));

    for (ptr = result; ptr != NULL; ptr = ptr->ai_next) {
        inetAddr addr_info;
        addr_info.init(ptr->ai_addr);
        retVal = addr_info.str();
        break;
    }

    freeaddrinfo(result);

    return 0;
}


}