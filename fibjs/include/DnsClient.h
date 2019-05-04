/*
 * DnsClient.h
 *
 *  Created on: May 10, 2019
 *      Author: Asion
 */

#ifndef DNSCLIENT_H_
#define DNSCLIENT_H_

#include "ifs/DnsClient.h"
#include "AresChannel.h"

namespace fibjs {
    

class DnsClient : public DnsClient_base {
    FIBER_FREE();

public:
    DnsClient()
    {
        m_channel = new AresChannel();
    }
    class ResolveResult : public NObject {
    public:
        ResolveResult(int ttl, exlib::string ip)
        {
            add("address", ip);
            add("ttl", ttl);
        }
    };

public:
    virtual result_t resolve(exlib::string host, exlib::string type, Variant& retVal, AsyncEvent* ac);
    virtual result_t resolve4(exlib::string host, v8::Local<v8::Object> options, obj_ptr<NArray>& retVal, AsyncEvent* ac);
    virtual result_t resolve4(exlib::string host, obj_ptr<NArray>& retVal, AsyncEvent* ac);
    virtual result_t resolve6(exlib::string host, v8::Local<v8::Object> options, obj_ptr<NArray>& retVal, AsyncEvent* ac);
    virtual result_t resolve6(exlib::string host, obj_ptr<NArray>& retVal, AsyncEvent* ac);
    virtual result_t resolveAny(exlib::string host, obj_ptr<NArray>& retVal, AsyncEvent* ac);
    virtual result_t resolveMx(exlib::string host, obj_ptr<NArray>& retVal, AsyncEvent* ac);
    virtual result_t resolveTxt(exlib::string host, obj_ptr<NArray>& retVal, AsyncEvent* ac);
    virtual result_t resolveSrv(exlib::string host, obj_ptr<NArray>& retVal, AsyncEvent* ac);
    virtual result_t resolveSoa(exlib::string host, obj_ptr<NObject>& retVal, AsyncEvent* ac);
    virtual result_t resolveNs(exlib::string host, obj_ptr<NArray>& retVal, AsyncEvent* ac);
    virtual result_t resolveCname(exlib::string host, obj_ptr<NArray>& retVal, AsyncEvent* ac);
    virtual result_t resolveNaptr(exlib::string host, obj_ptr<NArray>& retVal, AsyncEvent* ac);
    virtual result_t resolvePtr(exlib::string host, obj_ptr<NArray>& retVal, AsyncEvent* ac);
    virtual result_t reverse(exlib::string ip, obj_ptr<NArray>& retVal, AsyncEvent* ac);
    virtual result_t cancel();
    virtual result_t setServers(v8::Local<v8::Array> servers, AsyncEvent* ac);
    virtual result_t getServers(obj_ptr<NArray>& retVal);

private:
    result_t resolve4(exlib::string host, bool ttl, obj_ptr<NArray>& retVal, AsyncEvent* ac);
    result_t resolve6(exlib::string host, bool ttl, obj_ptr<NArray>& retVal, AsyncEvent* ac);

public:
    obj_ptr<AresChannel> m_channel;
};
}

#endif //DNSCLIENT_H_
