/*
 * DnsClient.h
 *
 *  Created on: May 10, 2019
 *      Author: Asion
 */

#include "DnsClient.h"
#include "Timer.h"
#include "AresChannel.h"
#include "inetAddr.h"
#include "parse.h"
#include <cares/ares.h>
#include <unordered_set>

#if defined(__ANDROID__) || defined(__MINGW32__) || defined(__OpenBSD__) || defined(_MSC_VER)

#include <cares/include/nameser.h>
#else
#include <arpa/nameser.h>
#endif

#ifndef IANA_DNS_PORT
#define IANA_DNS_PORT 53
#endif

namespace fibjs {

exlib::spinlock s_ares_library_lock;
static exlib::LockedList<asyncDNSQuery> s_aresQueryWait;
static exlib::Semaphore s_aresQuerySem;

AresChannel::~AresChannel()
{
    if (m_bLibraryInited) {
        s_ares_library_lock.lock();
        ares_library_cleanup();
        s_ares_library_lock.unlock();
    }

    if (m_channel) {
        ares_destroy(m_channel);
        m_channel = NULL;
    }
}

int32_t AresChannel::Setup()
{
    struct ares_options options;
    memset(&options, 0, sizeof(options));
    options.flags = ARES_FLAG_NOCHECKRESP;
    options.sock_state_cb = ares_sockstate_cb;
    options.sock_state_cb_data = this;

    int r;
    if (!m_bLibraryInited) {
        s_ares_library_lock.lock();
        r = ares_library_init(ARES_LIB_INIT_ALL);
        s_ares_library_lock.unlock();
        if (r != ARES_SUCCESS)
            return CHECK_ERROR(Runtime::setError(ares_strerror(r)));
    }

    r = ares_init_options(&m_channel, &options, ARES_OPT_FLAGS | ARES_OPT_SOCK_STATE_CB);

    if (r != ARES_SUCCESS) {
        s_ares_library_lock.lock();
        ares_library_cleanup();
        s_ares_library_lock.unlock();
        return CHECK_ERROR(Runtime::setError(ares_strerror(r)));
    }

    m_bLibraryInited = true;
    return 0;
}

void AresChannel::EnsureServers() {
  if (m_queryLastOk || !m_bDefaultServer) {
    return;
  }

  ares_addr_port_node* servers = NULL;

  ares_get_servers_ports(m_channel, &servers);

  if (servers == NULL) return;
  if (servers->next != NULL) {
    ares_free_data(servers);
    m_bDefaultServer = false;
    return;
  }

  if (servers[0].family != AF_INET ||
      servers[0].addr.addr4.s_addr != htonl(INADDR_LOOPBACK) ||
      servers[0].tcp_port != 0 ||
      servers[0].udp_port != 0) {
    ares_free_data(servers);
    m_bDefaultServer = false;
    return;
  }

  ares_free_data(servers);
  servers = NULL;

  ares_destroy(m_channel);

  CloseTimer();
  Setup();
}

void AresChannel::StartTimer()
{
    if (!m_timer) {
        m_timer = new QueryTimer(this);
        m_timer->sleep();
    }
}

void AresChannel::CloseTimer()
{
    if (m_timer) {
        m_timer->clear();
        m_timer = NULL;
    }
}
void AresChannel::ResetTimer()
{
    if (m_timer) {
        m_timer->clear();
        m_timer = new QueryTimer(this);
        m_timer->sleep();
    }
}

void AresChannel::AresTimeout()
{
    ares_process_fd(this->m_channel, ARES_SOCKET_BAD, ARES_SOCKET_BAD);
}

void AresChannel::ares_sockstate_cb(void* data, ares_socket_t sock, int read, int write)
{
    AresChannel* channel = (AresChannel*)data;
    ChannelTask* task;

    ChannelTask lookup_task;
    lookup_task.m_sock = sock;
    fib_ares_task_list_iterator it = channel->getTaskList()->find(&lookup_task);

    task = (it == channel->getTaskList()->end()) ? NULL : *it;

    if (read || write) {
        if (!task) {
            channel->StartTimer();
            task = channel->AsyncSend(&lookup_task, read, write, false);
            channel->getTaskList()->insert(task);
        } else
            channel->AsyncSend(task, read, write, true);
    } else {
        channel->getTaskList()->erase(it);
        channel->AsyncClose(task);
        if (channel->getTaskList()->empty()) {
            channel->CloseTimer();
        }
    }
}

result_t asyncDNSQuery::call()
{
    s_aresQueryWait.putTail(this);
    s_aresQuerySem.post();
    return CHECK_ERROR(CALL_E_PENDDING);
}

static exlib::string AddressToString(const void* vaddr, int len)
{
    const uint8_t* addr = (const uint8_t*)vaddr;
    exlib::string s;
    if (len == 4) {
        char buffer[4 * 4 + 3 + 1] = { 0 };
        sprintf(buffer, "%u.%u.%u.%u",
            (unsigned char)addr[0],
            (unsigned char)addr[1],
            (unsigned char)addr[2],
            (unsigned char)addr[3]);
        s += buffer;
    } else if (len == 16) {
        for (int ii = 0; ii < 16; ii += 2) {
            if (ii > 0)
                s += ':';
            char buffer[4 + 1];
            sprintf(buffer, "%02x%02x", (unsigned char)addr[ii], (unsigned char)addr[ii + 1]);
            s += buffer;
        }
    } else {
        exlib::string s1;
        for (int ii = 0; ii < len; ii++) {
            char buffer[2 + 1] = { 0 };
            sprintf(buffer, "%02x", addr[ii]);
            s1 += buffer;
        }
        s = s + "!" + s1 + "!";
    }
    return s;
}

class _acAres : public exlib::OSThread {
public:
    _acAres()
    {
    }

    virtual void Run()
    {
        // Runtime rt(NULL);
        asyncDNSQuery* p1;

        while (true) {
            s_aresQuerySem.wait();
            while ((p1 = s_aresQueryWait.getHead()) != 0) {
                p1->process();
            }
        }
    }

public:
    fd_set m_readers;
    fd_set m_writers;
};

void init_ares()
{
    static _acAres s_acAres;

    s_acAres.start();
}

result_t DnsClient_base::_new(obj_ptr<DnsClient_base>& retVal, v8::Local<v8::Object> This)
{
    result_t hr;
    DnsClient* c = new DnsClient();
    retVal = c;
    hr = c->m_channel->Setup();
    if (hr < 0)
        return hr;
    return 0;
}

result_t DnsClient::resolve(exlib::string host, exlib::string type, Variant& retVal, AsyncEvent* ac)
{
    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    result_t hr;

    obj_ptr<NObject> oRet = new NObject();
    obj_ptr<NArray> aRet = new NArray();

    if (type == "MX") {
        hr = resolveMx(host, aRet, ac);
        retVal = aRet;
        return hr;
    } else if (type == "TXT") {
        hr = resolveTxt(host, aRet, ac);
        retVal = aRet;
        return hr;
    } else if (type == "SRV") {
        hr = resolveSrv(host, aRet, ac);
        retVal = aRet;
        return hr;
    } else if (type == "A") {
        hr = resolve4(host, false, aRet, ac);
        retVal = aRet;
        return hr;
    } else if (type == "AAAA") {
        hr = resolve6(host, false, aRet, ac);
        retVal = aRet;
        return hr;
    } else if (type == "CNAME") {
        hr = resolveCname(host, aRet, ac);
        retVal = aRet;
        return hr;
    } else if (type == "NAPTR") {
        hr = resolveNaptr(host, aRet, ac);
        retVal = aRet;
        return hr;
    } else if (type == "PTR") {
        hr = resolvePtr(host, aRet, ac);
        retVal = aRet;
        return hr;
    } else if (type == "NS") {
        hr = resolveNs(host, aRet, ac);
        retVal = aRet;
        return hr;
    } else if (type == "SOA") {
        hr = resolveSoa(host, oRet, ac);
        retVal = oRet;
        return hr;
    }

    return CHECK_ERROR(CALL_E_INVALIDARG);
}

result_t DnsClient::resolve4(exlib::string host, obj_ptr<NArray>& retVal, AsyncEvent* ac)
{
    return resolve4(host, false, retVal, ac);
}

result_t DnsClient::resolve4(exlib::string host, v8::Local<v8::Object> options, obj_ptr<NArray>& retVal, AsyncEvent* ac)
{
    bool v;
    if (ac->isSync()) {
        result_t hr;
        Isolate* isolate = Isolate::current();

        hr = GetConfigValue(isolate->m_isolate, options, "ttl", v);
        if (hr == CALL_E_PARAMNOTOPTIONAL)
            v = false;
        else if (hr < 0)
            return CHECK_ERROR(hr);

        ac->m_ctx.resize(1);
        ac->m_ctx[0] = v;
        return CHECK_ERROR(CALL_E_NOSYNC);
    }

    v = ac->m_ctx[0].boolVal();
    return resolve4(host, v, retVal, ac);
}

result_t DnsClient::resolve6(exlib::string host, obj_ptr<NArray>& retVal, AsyncEvent* ac)
{
    return resolve6(host, false, retVal, ac);
}

result_t DnsClient::resolve6(exlib::string host, v8::Local<v8::Object> options, obj_ptr<NArray>& retVal, AsyncEvent* ac)
{
    bool v;
    if (ac->isSync()) {
        result_t hr;
        Isolate* isolate = Isolate::current();

        hr = GetConfigValue(isolate->m_isolate, options, "ttl", v);
        if (hr == CALL_E_PARAMNOTOPTIONAL)
            v = false;
        else if (hr < 0)
            return CHECK_ERROR(hr);

        ac->m_ctx.resize(1);
        ac->m_ctx[0] = v;
        return CHECK_ERROR(CALL_E_NOSYNC);
    }

    v = ac->m_ctx[0].boolVal();

    return resolve6(host, v, retVal, ac);
}

result_t DnsClient::resolve4(exlib::string host, bool ttl, obj_ptr<NArray>& retVal, AsyncEvent* ac)
{
    class asyncResolve4 : public asyncDNSQuery {
    public:
        asyncResolve4(exlib::string host, bool ttl, obj_ptr<NArray>& retVal, AsyncEvent* ac, DnsClient* pThis)
            : asyncDNSQuery(ac)
            , m_errorno(0)
            , m_pThis(pThis)
            , m_ttl(ttl)
            , m_host(host)
            , m_retVal(retVal)
        {
        }

        virtual result_t process()
        {
            m_pThis->m_channel->EnsureServers();
            m_pThis->m_channel->m_activeQueryCnt.inc();
            ares_query(m_pThis->m_channel->m_channel, m_host.c_str(), ns_c_in, ns_t_a, callback, this);

            return CHECK_ERROR(CALL_E_PENDDING);
        }

        virtual void proc()
        {
            m_pThis->m_channel->m_queryLastOk = m_errorno != ARES_ECONNREFUSED;
            m_pThis->m_channel->m_activeQueryCnt.dec();

            if (m_errorno != 0)
                ready(CHECK_ERROR(Runtime::setError(ares_strerror(m_errorno))));
            else {
                ready(0);
            }
        }

        static void callback(void* arg, int status, int timeouts, unsigned char* abuf, int alen)
        {
            asyncResolve4* pThis = (asyncResolve4*)arg;
            struct hostent* host;
            struct ares_addrttl addrttls[256];
            int naddrttls = sizeof(addrttls);

            if (status != ARES_SUCCESS) {
                pThis->m_errorno = status;
                pThis->m_pThis->m_channel->AsyncCallback(pThis);
                return;
            }

            int rs_status = ares_parse_a_reply(abuf, alen, &host, (ares_addrttl*)addrttls, &naddrttls);
            if (rs_status != ARES_SUCCESS) {
                pThis->m_errorno = rs_status;
                pThis->m_pThis->m_channel->AsyncCallback(pThis);
                return;
            }
            for (uint32_t i = 0; host->h_addr_list[i] != NULL; ++i) {
                exlib::string ip;
                ip = AddressToString(host->h_addr_list[i], host->h_length);

                if (pThis->m_ttl) {
                    pThis->m_retVal->append(new ResolveResult(addrttls[i].ttl, ip));
                } else {
                    pThis->m_retVal->append(ip);
                }
            }
            ares_free_hostent(host);
            pThis->m_pThis->m_channel->AsyncCallback(pThis);
            return;
        }

    public:
        int32_t m_errorno;
        obj_ptr<DnsClient> m_pThis;
        bool m_ttl;
        exlib::string m_host;
        obj_ptr<NArray> m_retVal;
    };

    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    retVal = new NArray();
    return (new asyncResolve4(host, ttl, retVal, ac, this))->call();
}

result_t DnsClient::resolve6(exlib::string host, bool ttl, obj_ptr<NArray>& retVal, AsyncEvent* ac)
{
    class asyncResolve6 : public asyncDNSQuery {
    public:
        asyncResolve6(exlib::string host, bool ttl, obj_ptr<NArray>& retVal, AsyncEvent* ac, DnsClient* pThis)
            : asyncDNSQuery(ac)
            , m_errorno(0)
            , m_pThis(pThis)
            , m_ttl(ttl)
            , m_host(host)
            , m_retVal(retVal)
        {
        }

        virtual result_t process()
        {
            m_pThis->m_channel->EnsureServers();
            m_pThis->m_channel->m_activeQueryCnt.inc();
            ares_query(m_pThis->m_channel->m_channel, m_host.c_str(), ns_c_in, ns_t_aaaa, callback, this);

            return CHECK_ERROR(CALL_E_PENDDING);
        }

        virtual void proc()
        {
            m_pThis->m_channel->m_queryLastOk = m_errorno != ARES_ECONNREFUSED;
            m_pThis->m_channel->m_activeQueryCnt.dec();

            if (m_errorno != 0)
                ready(CHECK_ERROR(Runtime::setError(ares_strerror(m_errorno))));
            else {
                ready(0);
            }
        }

        static void callback(void* arg, int status, int timeouts, unsigned char* abuf, int alen)
        {
            asyncResolve6* pThis = (asyncResolve6*)arg;
            struct hostent* host;
            struct ares_addr6ttl addrttls[256];
            int naddrttls = sizeof(addrttls);

            if (status != ARES_SUCCESS) {
                pThis->m_errorno = status;
                pThis->m_pThis->m_channel->AsyncCallback(pThis);
                return;
            }

            int rs_status = ares_parse_aaaa_reply(abuf, alen, &host, (ares_addr6ttl*)addrttls, &naddrttls);
            if (rs_status != ARES_SUCCESS) {
                pThis->m_errorno = rs_status;
                pThis->m_pThis->m_channel->AsyncCallback(pThis);
                return;
            }
            for (uint32_t i = 0; host->h_addr_list[i] != NULL; ++i) {
                exlib::string ip;
                ip = AddressToString(host->h_addr_list[i], host->h_length);

                if (pThis->m_ttl) {
                    pThis->m_retVal->append(new ResolveResult(addrttls[i].ttl, ip));
                } else {
                    pThis->m_retVal->append(ip);
                }
            }
            ares_free_hostent(host);
            pThis->m_pThis->m_channel->AsyncCallback(pThis);
            return;
        }

    public:
        int32_t m_errorno;
        obj_ptr<DnsClient> m_pThis;
        bool m_ttl;
        exlib::string m_host;
        obj_ptr<NArray> m_retVal;
    };

    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    retVal = new NArray();
    return (new asyncResolve6(host, ttl, retVal, ac, this))->call();
}

result_t DnsClient::resolveCname(exlib::string host, obj_ptr<NArray>& retVal, AsyncEvent* ac)
{
    class asyncResolveCname : public asyncDNSQuery {
    public:
        asyncResolveCname(exlib::string host, obj_ptr<NArray>& retVal, AsyncEvent* ac, DnsClient* pThis)
            : asyncDNSQuery(ac)
            , m_errorno(0)
            , m_pThis(pThis)
            , m_host(host)
            , m_retVal(retVal)
        {
        }

        virtual result_t process()
        {
            m_pThis->m_channel->EnsureServers();
            m_pThis->m_channel->m_activeQueryCnt.inc();
            ares_query(m_pThis->m_channel->m_channel, m_host.c_str(), ns_c_in, ns_t_cname, callback, this);

            return CHECK_ERROR(CALL_E_PENDDING);
        }

        virtual void proc()
        {
            m_pThis->m_channel->m_queryLastOk = m_errorno != ARES_ECONNREFUSED;
            m_pThis->m_channel->m_activeQueryCnt.dec();

            if (m_errorno != 0)
                ready(CHECK_ERROR(Runtime::setError(ares_strerror(m_errorno))));
            else {
                ready(0);
            }
        }

        static void callback(void* arg, int status, int timeouts, unsigned char* abuf, int alen)
        {
            asyncResolveCname* pThis = (asyncResolveCname*)arg;
            struct hostent* host;
            struct ares_addrttl addrttls[256];
            int naddrttls = sizeof(addrttls);

            if (status != ARES_SUCCESS) {
                pThis->m_errorno = status;
                pThis->m_pThis->m_channel->AsyncCallback(pThis);
                return;
            }

            int rs_status = ares_parse_a_reply(abuf, alen, &host, (ares_addrttl*)addrttls, &naddrttls);
            if (rs_status != ARES_SUCCESS) {
                pThis->m_errorno = rs_status;
                pThis->m_pThis->m_channel->AsyncCallback(pThis);
                return;
            }

            pThis->m_retVal->append(host->h_name);
            ares_free_hostent(host);
            pThis->m_pThis->m_channel->AsyncCallback(pThis);
            return;
        }

    public:
        int32_t m_errorno;
        obj_ptr<DnsClient> m_pThis;
        exlib::string m_host;
        obj_ptr<NArray> m_retVal;
    };

    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    retVal = new NArray();
    return (new asyncResolveCname(host, retVal, ac, this))->call();
}

result_t DnsClient::resolveNaptr(exlib::string host, obj_ptr<NArray>& retVal, AsyncEvent* ac)
{
    class asyncResolveNaptr : public asyncDNSQuery {
    public:
        class ResolveNaptrResult : public NObject {
        public:
            ResolveNaptrResult(ares_naptr_reply* naptr_reply)
            {
                add("flags", (char*)naptr_reply->flags);
                add("service", (char*)naptr_reply->service);
                add("regexp", (char*)naptr_reply->regexp);
                add("replacement", naptr_reply->replacement);
                add("order", naptr_reply->order);
                add("preference", naptr_reply->preference);
            }
        };

    public:
        asyncResolveNaptr(exlib::string host, obj_ptr<NArray>& retVal, AsyncEvent* ac, DnsClient* pThis)
            : asyncDNSQuery(ac)
            , m_errorno(0)
            , m_pThis(pThis)
            , m_host(host)
            , m_retVal(retVal)
        {
        }

        virtual result_t process()
        {
            m_pThis->m_channel->EnsureServers();
            m_pThis->m_channel->m_activeQueryCnt.inc();
            ares_query(m_pThis->m_channel->m_channel, m_host.c_str(), ns_c_in, ns_t_naptr, callback, this);

            return CHECK_ERROR(CALL_E_PENDDING);
        }

        virtual void proc()
        {
            m_pThis->m_channel->m_queryLastOk = m_errorno != ARES_ECONNREFUSED;
            m_pThis->m_channel->m_activeQueryCnt.dec();

            if (m_errorno != 0)
                ready(CHECK_ERROR(Runtime::setError(ares_strerror(m_errorno))));
            else {
                ready(0);
            }
        }

        static void callback(void* arg, int status, int timeouts, unsigned char* abuf, int alen)
        {
            asyncResolveNaptr* pThis = (asyncResolveNaptr*)arg;
            struct ares_naptr_reply* naptr_start;

            if (status != ARES_SUCCESS) {
                pThis->m_errorno = status;
                pThis->m_pThis->m_channel->AsyncCallback(pThis);
                return;
            }

            int rs_status = ares_parse_naptr_reply(abuf, alen, &naptr_start);
            if (rs_status != ARES_SUCCESS) {
                pThis->m_errorno = rs_status;
                pThis->m_pThis->m_channel->AsyncCallback(pThis);
                return;
            }

            for (struct ares_naptr_reply* current = naptr_start; current != NULL; current = current->next) {
                pThis->m_retVal->append(new ResolveNaptrResult(current));
            }
            ares_free_data(naptr_start);

            pThis->m_pThis->m_channel->AsyncCallback(pThis);
            return;
        }

    public:
        int32_t m_errorno;
        obj_ptr<DnsClient> m_pThis;
        exlib::string m_host;
        obj_ptr<NArray> m_retVal;
    };

    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    retVal = new NArray();
    return (new asyncResolveNaptr(host, retVal, ac, this))->call();
}

result_t DnsClient::resolveNs(exlib::string host, obj_ptr<NArray>& retVal, AsyncEvent* ac)
{
    class asyncResolveNs : public asyncDNSQuery {
    public:
        asyncResolveNs(exlib::string host, obj_ptr<NArray>& retVal, AsyncEvent* ac, DnsClient* pThis)
            : asyncDNSQuery(ac)
            , m_errorno(0)
            , m_pThis(pThis)
            , m_host(host)
            , m_retVal(retVal)
        {
        }

        virtual result_t process()
        {
            m_pThis->m_channel->EnsureServers();
            m_pThis->m_channel->m_activeQueryCnt.inc();
            ares_query(m_pThis->m_channel->m_channel, m_host.c_str(), ns_c_in, ns_t_ns, callback, this);

            return CHECK_ERROR(CALL_E_PENDDING);
        }

        virtual void proc()
        {
            m_pThis->m_channel->m_queryLastOk = m_errorno != ARES_ECONNREFUSED;
            m_pThis->m_channel->m_activeQueryCnt.dec();

            if (m_errorno != 0)
                ready(CHECK_ERROR(Runtime::setError(ares_strerror(m_errorno))));
            else {
                ready(0);
            }
        }

        static void callback(void* arg, int status, int timeouts, unsigned char* abuf, int alen)
        {
            asyncResolveNs* pThis = (asyncResolveNs*)arg;
            struct hostent* host;

            if (status != ARES_SUCCESS) {
                pThis->m_errorno = status;
                pThis->m_pThis->m_channel->AsyncCallback(pThis);
                return;
            }

            int rs_status = ares_parse_ns_reply(abuf, alen, &host);
            if (rs_status != ARES_SUCCESS) {
                pThis->m_errorno = rs_status;
                pThis->m_pThis->m_channel->AsyncCallback(pThis);
                return;
            }

            for (uint32_t i = 0; host->h_aliases[i] != NULL; ++i) {
                pThis->m_retVal->append(host->h_aliases[i]);
            }
            ares_free_hostent(host);
            pThis->m_pThis->m_channel->AsyncCallback(pThis);
            return;
        }

    public:
        int32_t m_errorno;
        obj_ptr<DnsClient> m_pThis;
        exlib::string m_host;
        obj_ptr<NArray> m_retVal;
    };

    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    retVal = new NArray();
    return (new asyncResolveNs(host, retVal, ac, this))->call();
}

result_t DnsClient::resolvePtr(exlib::string host, obj_ptr<NArray>& retVal, AsyncEvent* ac)
{
    class asyncResolvePtr : public asyncDNSQuery {
    public:
        asyncResolvePtr(exlib::string host, obj_ptr<NArray>& retVal, AsyncEvent* ac, DnsClient* pThis)
            : asyncDNSQuery(ac)
            , m_errorno(0)
            , m_pThis(pThis)
            , m_host(host)
            , m_retVal(retVal)
        {
        }

        virtual result_t process()
        {
            m_pThis->m_channel->EnsureServers();
            m_pThis->m_channel->m_activeQueryCnt.inc();
            ares_query(m_pThis->m_channel->m_channel, m_host.c_str(), ns_c_in, ns_t_ptr, callback, this);

            return CHECK_ERROR(CALL_E_PENDDING);
        }

        virtual void proc()
        {
            m_pThis->m_channel->m_queryLastOk = m_errorno != ARES_ECONNREFUSED;
            m_pThis->m_channel->m_activeQueryCnt.dec();

            if (m_errorno != 0)
                ready(CHECK_ERROR(Runtime::setError(ares_strerror(m_errorno))));
            else {
                ready(0);
            }
        }

        static void callback(void* arg, int status, int timeouts, unsigned char* abuf, int alen)
        {
            asyncResolvePtr* pThis = (asyncResolvePtr*)arg;
            struct hostent* host;

            if (status != ARES_SUCCESS) {
                pThis->m_errorno = status;
                pThis->m_pThis->m_channel->AsyncCallback(pThis);
                return;
            }

            int rs_status = ares_parse_ptr_reply(abuf, alen, NULL, 0, AF_INET, &host);
            if (rs_status != ARES_SUCCESS) {
                pThis->m_errorno = rs_status;
                pThis->m_pThis->m_channel->AsyncCallback(pThis);
                return;
            }

            for (uint32_t i = 0; host->h_aliases[i] != NULL; ++i) {
                pThis->m_retVal->append(host->h_aliases[i]);
            }
            ares_free_hostent(host);
            pThis->m_pThis->m_channel->AsyncCallback(pThis);
            return;
        }

    public:
        int32_t m_errorno;
        obj_ptr<DnsClient> m_pThis;
        exlib::string m_host;
        obj_ptr<NArray> m_retVal;
    };

    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    retVal = new NArray();
    return (new asyncResolvePtr(host, retVal, ac, this))->call();
}

result_t DnsClient::resolveSoa(exlib::string host, obj_ptr<NObject>& retVal, AsyncEvent* ac)
{
    class asyncResolveSoa : public asyncDNSQuery {
    public:
        asyncResolveSoa(exlib::string host, obj_ptr<NObject>& retVal, AsyncEvent* ac, DnsClient* pThis)
            : asyncDNSQuery(ac)
            , m_errorno(0)
            , m_pThis(pThis)
            , m_host(host)
            , m_retVal(retVal)
        {
        }

        virtual result_t process()
        {
            m_pThis->m_channel->EnsureServers();
            m_pThis->m_channel->m_activeQueryCnt.inc();
            ares_query(m_pThis->m_channel->m_channel, m_host.c_str(), ns_c_in, ns_t_soa, callback, this);

            return CHECK_ERROR(CALL_E_PENDDING);
        }

        virtual void proc()
        {
            m_pThis->m_channel->m_queryLastOk = m_errorno != ARES_ECONNREFUSED;
            m_pThis->m_channel->m_activeQueryCnt.dec();

            if (m_errorno != 0)
                ready(CHECK_ERROR(Runtime::setError(ares_strerror(m_errorno))));
            else {
                ready(0);
            }
        }

        static void callback(void* arg, int status, int timeouts, unsigned char* abuf, int alen)
        {
            asyncResolveSoa* pThis = (asyncResolveSoa*)arg;
            struct ares_soa_reply* soa_out;

            if (status != ARES_SUCCESS) {
                pThis->m_errorno = status;
                pThis->m_pThis->m_channel->AsyncCallback(pThis);
                return;
            }

            int rs_status = ares_parse_soa_reply(abuf, alen, &soa_out);
            if (rs_status != ARES_SUCCESS) {
                pThis->m_errorno = rs_status;
                pThis->m_pThis->m_channel->AsyncCallback(pThis);
                return;
            }

            pThis->m_retVal->add("nsname", soa_out->nsname);
            pThis->m_retVal->add("hostmaster", soa_out->hostmaster);
            pThis->m_retVal->add("serial", (int)soa_out->serial);
            pThis->m_retVal->add("refresh", (int)soa_out->refresh);
            pThis->m_retVal->add("retry", (int)soa_out->retry);
            pThis->m_retVal->add("expire", (int)soa_out->expire);
            pThis->m_retVal->add("minttl", (int)soa_out->minttl);

            ares_free_data(soa_out);

            pThis->m_pThis->m_channel->AsyncCallback(pThis);
            return;
        }

    public:
        int32_t m_errorno;
        obj_ptr<DnsClient> m_pThis;
        exlib::string m_host;
        obj_ptr<NObject> m_retVal;
    };

    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    retVal = new NObject();
    return (new asyncResolveSoa(host, retVal, ac, this))->call();
}

result_t DnsClient::resolveTxt(exlib::string host, obj_ptr<NArray>& retVal, AsyncEvent* ac)
{
    class asyncResolveTxt : public asyncDNSQuery {
    public:
        asyncResolveTxt(exlib::string host, obj_ptr<NArray>& retVal, AsyncEvent* ac, DnsClient* pThis)
            : asyncDNSQuery(ac)
            , m_errorno(0)
            , m_pThis(pThis)
            , m_host(host)
            , m_retVal(retVal)
        {
        }

        virtual result_t process()
        {
            m_pThis->m_channel->EnsureServers();
            m_pThis->m_channel->m_activeQueryCnt.inc();
            ares_query(m_pThis->m_channel->m_channel, m_host.c_str(), ns_c_in, ns_t_txt, callback, this);

            return CHECK_ERROR(CALL_E_PENDDING);
        }

        virtual void proc()
        {
            m_pThis->m_channel->m_queryLastOk = m_errorno != ARES_ECONNREFUSED;
            m_pThis->m_channel->m_activeQueryCnt.dec();

            if (m_errorno != 0)
                ready(CHECK_ERROR(Runtime::setError(ares_strerror(m_errorno))));
            else {
                ready(0);
            }
        }

        static void callback(void* arg, int status, int timeouts, unsigned char* abuf, int alen)
        {
            asyncResolveTxt* pThis = (asyncResolveTxt*)arg;
            struct ares_txt_ext* txt_out;

            if (status != ARES_SUCCESS) {
                pThis->m_errorno = status;
                pThis->m_pThis->m_channel->AsyncCallback(pThis);
                return;
            }

            int rs_status = ares_parse_txt_reply_ext(abuf, alen, &txt_out);
            if (rs_status != ARES_SUCCESS) {
                pThis->m_errorno = rs_status;
                pThis->m_pThis->m_channel->AsyncCallback(pThis);
                return;
            }

            int32_t len;
            obj_ptr<NArray> txt_result = new NArray();
            for (struct ares_txt_ext* current = txt_out; current != NULL; current = current->next) {
                if (current->record_start) {
                    txt_result->get_length(len);
                    if (len > 0) {
                        pThis->m_retVal->append(txt_result);
                    }

                    txt_result = new NArray();
                }

                txt_result->append((char*)current->txt);
            }

            txt_result->get_length(len);
            if (len > 0) {
                pThis->m_retVal->append(txt_result);
            }
            ares_free_data(txt_out);
            pThis->m_pThis->m_channel->AsyncCallback(pThis);
            return;
        }

    public:
        int32_t m_errorno;
        obj_ptr<DnsClient> m_pThis;
        exlib::string m_host;
        obj_ptr<NArray> m_retVal;
    };

    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    retVal = new NArray();
    return (new asyncResolveTxt(host, retVal, ac, this))->call();
}

result_t DnsClient::resolveSrv(exlib::string host, obj_ptr<NArray>& retVal, AsyncEvent* ac)
{
    class asyncResolveSrv : public asyncDNSQuery {
    public:
        class ResolveSrvResult : public NObject {
        public:
            ResolveSrvResult(ares_srv_reply* srv_reply)
            {
                add("name", srv_reply->host);
                add("port", srv_reply->port);
                add("priority", srv_reply->priority);
                add("weight", srv_reply->weight);
            }
        };

    public:
        asyncResolveSrv(exlib::string host, obj_ptr<NArray>& retVal, AsyncEvent* ac, DnsClient* pThis)
            : asyncDNSQuery(ac)
            , m_errorno(0)
            , m_pThis(pThis)
            , m_host(host)
            , m_retVal(retVal)
        {
        }

        virtual result_t process()
        {
            m_pThis->m_channel->EnsureServers();
            m_pThis->m_channel->m_activeQueryCnt.inc();
            ares_query(m_pThis->m_channel->m_channel, m_host.c_str(), ns_c_in, ns_t_srv, callback, this);

            return CHECK_ERROR(CALL_E_PENDDING);
        }

        virtual void proc()
        {
            m_pThis->m_channel->m_queryLastOk = m_errorno != ARES_ECONNREFUSED;
            m_pThis->m_channel->m_activeQueryCnt.dec();

            if (m_errorno != 0)
                ready(CHECK_ERROR(Runtime::setError(ares_strerror(m_errorno))));
            else {
                ready(0);
            }
        }

        static void callback(void* arg, int status, int timeouts, unsigned char* abuf, int alen)
        {
            asyncResolveSrv* pThis = (asyncResolveSrv*)arg;
            struct ares_srv_reply* srv_start;

            if (status != ARES_SUCCESS) {
                pThis->m_errorno = status;
                pThis->m_pThis->m_channel->AsyncCallback(pThis);
                return;
            }

            int rs_status = ares_parse_srv_reply(abuf, alen, &srv_start);
            if (rs_status != ARES_SUCCESS) {
                pThis->m_errorno = rs_status;
                pThis->m_pThis->m_channel->AsyncCallback(pThis);
                return;
            }

            for (struct ares_srv_reply* current = srv_start; current != NULL; current = current->next) {
                pThis->m_retVal->append(new ResolveSrvResult(current));
            }
            ares_free_data(srv_start);

            pThis->m_pThis->m_channel->AsyncCallback(pThis);
            return;
        }

    public:
        int32_t m_errorno;
        obj_ptr<DnsClient> m_pThis;
        exlib::string m_host;
        obj_ptr<NArray> m_retVal;
    };

    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    retVal = new NArray();
    return (new asyncResolveSrv(host, retVal, ac, this))->call();
}

result_t DnsClient::resolveMx(exlib::string host, obj_ptr<NArray>& retVal, AsyncEvent* ac)
{
    class asyncResolveMx : public asyncDNSQuery {
    public:
        class ResolveMxResult : public NObject {
        public:
            ResolveMxResult(ares_mx_reply* mx_reply)
            {
                add("exchange", mx_reply->host);
                add("priority", mx_reply->priority);
            }
        };

    public:
        asyncResolveMx(exlib::string host, obj_ptr<NArray>& retVal, AsyncEvent* ac, DnsClient* pThis)
            : asyncDNSQuery(ac)
            , m_errorno(0)
            , m_pThis(pThis)
            , m_host(host)
            , m_retVal(retVal)
        {
        }

        virtual result_t process()
        {
            m_pThis->m_channel->EnsureServers();
            m_pThis->m_channel->m_activeQueryCnt.inc();
            ares_query(m_pThis->m_channel->m_channel, m_host.c_str(), ns_c_in, ns_t_mx, callback, this);

            return CHECK_ERROR(CALL_E_PENDDING);
        }

        virtual void proc()
        {
            m_pThis->m_channel->m_queryLastOk = m_errorno != ARES_ECONNREFUSED;
            m_pThis->m_channel->m_activeQueryCnt.dec();

            if (m_errorno != 0)
                ready(CHECK_ERROR(Runtime::setError(ares_strerror(m_errorno))));
            else {
                ready(0);
            }
        }

        static void callback(void* arg, int status, int timeouts, unsigned char* abuf, int alen)
        {
            asyncResolveMx* pThis = (asyncResolveMx*)arg;
            struct ares_mx_reply* mx_reply = NULL;

            if (status != ARES_SUCCESS) {
                pThis->m_errorno = status;
                pThis->m_pThis->m_channel->AsyncCallback(pThis);
                return;
            }

            int rs_status = ares_parse_mx_reply(abuf, alen, &mx_reply);
            if (rs_status != ARES_SUCCESS) {
                pThis->m_errorno = rs_status;
                pThis->m_pThis->m_channel->AsyncCallback(pThis);
                return;
            }

            for (struct ares_mx_reply* current = mx_reply; current != NULL; current = current->next) {
                pThis->m_retVal->append(new ResolveMxResult(current));
            }
            ares_free_data(mx_reply);

            pThis->m_pThis->m_channel->AsyncCallback(pThis);
            return;
        }

    public:
        int32_t m_errorno;
        obj_ptr<DnsClient> m_pThis;
        exlib::string m_host;
        obj_ptr<NArray> m_retVal;
    };

    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    retVal = new NArray();
    return (new asyncResolveMx(host, retVal, ac, this))->call();
}

result_t DnsClient::resolveAny(exlib::string host, obj_ptr<NArray>& retVal, AsyncEvent* ac)
{
    class asyncResolveAny : public AsyncState {
    public:
        asyncResolveAny(DnsClient* dc, exlib::string host, obj_ptr<NArray>& retVal, AsyncEvent* ac)
            : AsyncState(ac)
            , m_error(false)
            , m_dc(dc)
            , m_host(host)
            , m_retVal(retVal)
        {
            set(resolve4);
        }

        static int32_t resolve4(AsyncState* pState, int32_t n)
        {
            asyncResolveAny* pThis = (asyncResolveAny*)pState;
            pThis->set(resolve6);
            return pThis->m_dc->resolve4(pThis->m_host, true, pThis->m_retVal4, pThis);
        }

        static int32_t resolve6(AsyncState* pState, int32_t n)
        {
            asyncResolveAny* pThis = (asyncResolveAny*)pState;
            pThis->set(resolveMx);
            pThis->m_error = false;
            return pThis->m_dc->resolve6(pThis->m_host, true, pThis->m_retVal6, pThis);
        }

        static int32_t resolveMx(AsyncState* pState, int32_t n)
        {
            asyncResolveAny* pThis = (asyncResolveAny*)pState;
            pThis->set(resolveTxt);
            pThis->m_error = false;
            return pThis->m_dc->resolveMx(pThis->m_host, pThis->m_retValMx, pThis);
        }

        static int32_t resolveTxt(AsyncState* pState, int32_t n)
        {
            asyncResolveAny* pThis = (asyncResolveAny*)pState;
            pThis->set(resolveSrv);
            pThis->m_error = false;
            return pThis->m_dc->resolveTxt(pThis->m_host, pThis->m_retValTxt, pThis);
        }

        static int32_t resolveSrv(AsyncState* pState, int32_t n)
        {
            asyncResolveAny* pThis = (asyncResolveAny*)pState;
            pThis->set(resolveNs);
            pThis->m_error = false;
            return pThis->m_dc->resolveSrv(pThis->m_host, pThis->m_retValSrv, pThis);
        }

        static int32_t resolveNs(AsyncState* pState, int32_t n)
        {
            asyncResolveAny* pThis = (asyncResolveAny*)pState;
            pThis->set(resolveSoa);
            pThis->m_error = false;
            return pThis->m_dc->resolveNs(pThis->m_host, pThis->m_retValNs, pThis);
        }

        static int32_t resolveSoa(AsyncState* pState, int32_t n)
        {
            asyncResolveAny* pThis = (asyncResolveAny*)pState;
            pThis->set(resolveCname);
            pThis->m_error = false;
            return pThis->m_dc->resolveSoa(pThis->m_host, pThis->m_retValSoa, pThis);
        }

        static int32_t resolveCname(AsyncState* pState, int32_t n)
        {
            asyncResolveAny* pThis = (asyncResolveAny*)pState;
            pThis->set(resolveNaptr);
            if (pThis->m_error) {
                pThis->m_retValSoa = NULL;
                pThis->m_error = false;
            }
            return pThis->m_dc->resolveCname(pThis->m_host, pThis->m_retValCname, pThis);
        }

        static int32_t resolveNaptr(AsyncState* pState, int32_t n)
        {
            asyncResolveAny* pThis = (asyncResolveAny*)pState;
            pThis->set(resolvePtr);
            pThis->m_error = false;
            return pThis->m_dc->resolveNaptr(pThis->m_host, pThis->m_retValNaptr, pThis);
        }

        static int32_t resolvePtr(AsyncState* pState, int32_t n)
        {
            asyncResolveAny* pThis = (asyncResolveAny*)pState;
            pThis->set(end);
            pThis->m_error = false;
            return pThis->m_dc->resolvePtr(pThis->m_host, pThis->m_retValPtr, pThis);
        }

        static int32_t end(AsyncState* pState, int32_t n)
        {
            int32_t len;
            asyncResolveAny* pThis = (asyncResolveAny*)pState;
            Variant v;
            obj_ptr<NObject> t_o;
            obj_ptr<NArray> t_a;

            pThis->m_retVal4->get_length(len);
            for (int32_t i = 0; i < len; i++) {
                pThis->m_retVal4->_indexed_getter(i, v);
                t_o = (NObject*)v.object();
                t_o->add("type", "A");
                pThis->m_retVal->append(t_o);
            }

            pThis->m_retVal6->get_length(len);
            for (int32_t i = 0; i < len; i++) {
                pThis->m_retVal6->_indexed_getter(i, v);
                t_o = (NObject*)v.object();
                t_o->add("type", "AAAA");
                pThis->m_retVal->append(t_o);
            }

            pThis->m_retValMx->get_length(len);
            for (int32_t i = 0; i < len; i++) {
                pThis->m_retValMx->_indexed_getter(i, v);
                t_o = (NObject*)v.object();
                t_o->add("type", "MX");
                pThis->m_retVal->append(t_o);
            }

            pThis->m_retValTxt->get_length(len);
            for (int32_t i = 0; i < len; i++) {
                pThis->m_retValTxt->_indexed_getter(i, v);
                t_a = (NArray*)v.object();
                t_o = new NObject();
                t_o->add("type", "TXT");
                t_o->add("entries", t_a);
                pThis->m_retVal->append(t_o);
            }

            pThis->m_retValSrv->get_length(len);
            for (int32_t i = 0; i < len; i++) {
                pThis->m_retValSrv->_indexed_getter(i, v);
                t_o = (NObject*)v.object();
                t_o->add("type", "SRV");
                pThis->m_retVal->append(t_o);
            }

            pThis->m_retValNs->get_length(len);
            for (int32_t i = 0; i < len; i++) {
                pThis->m_retValNs->_indexed_getter(i, v);
                exlib::string str = v.string();
                t_o = new NObject();
                t_o->add("type", "NS");
                t_o->add("value", str);
                pThis->m_retVal->append(t_o);
            }

            pThis->m_retValCname->get_length(len);
            for (int32_t i = 0; i < len; i++) {
                pThis->m_retValCname->_indexed_getter(i, v);
                exlib::string str = v.string();
                t_o = new NObject();
                t_o->add("type", "CNAME");
                t_o->add("value", str);
                pThis->m_retVal->append(t_o);
            }

            pThis->m_retValPtr->get_length(len);
            for (int32_t i = 0; i < len; i++) {
                pThis->m_retValPtr->_indexed_getter(i, v);
                exlib::string str = v.string();
                t_o = new NObject();
                t_o->add("type", "PTR");
                t_o->add("value", str);
                pThis->m_retVal->append(t_o);
            }

            pThis->m_retValNaptr->get_length(len);
            for (int32_t i = 0; i < len; i++) {
                pThis->m_retValNaptr->_indexed_getter(i, v);
                t_o = (NObject*)v.object();
                t_o->add("type", "NAPTR");
                pThis->m_retVal->append(t_o);
            }

            if (pThis->m_retValSoa) {
                pThis->m_retValSoa->add("type", "SOA");
                pThis->m_retVal->append(pThis->m_retValSoa);
            }

            return pThis->done();
        }

        int32_t error(int32_t v)
        {
            m_error = true;
            return 0;
        }

    public:
        bool m_error;
        obj_ptr<DnsClient> m_dc;
        exlib::string m_host;
        obj_ptr<NArray> m_retVal;
        obj_ptr<NArray> m_retVal4;
        obj_ptr<NArray> m_retVal6;
        obj_ptr<NArray> m_retValMx;
        obj_ptr<NArray> m_retValTxt;
        obj_ptr<NArray> m_retValSrv;
        obj_ptr<NArray> m_retValNs;
        obj_ptr<NObject> m_retValSoa;
        obj_ptr<NArray> m_retValCname;
        obj_ptr<NArray> m_retValNaptr;
        obj_ptr<NArray> m_retValPtr;
    };

    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    retVal = new NArray();
    return (new asyncResolveAny(this, host, retVal, ac))->post(0);
}

result_t DnsClient::reverse(exlib::string ip, obj_ptr<NArray>& retVal, AsyncEvent* ac)
{
        class asyncReverse : public asyncDNSQuery {
    public:
        asyncReverse(exlib::string ip, obj_ptr<NArray>& retVal, AsyncEvent* ac, DnsClient* pThis)
            : asyncDNSQuery(ac)
            , m_errorno(0)
            , m_pThis(pThis)
            , m_ip(ip)
            , m_retVal(retVal)
        {
        }

        virtual result_t process()
        {
            m_pThis->m_channel->EnsureServers();
            m_pThis->m_channel->m_activeQueryCnt.inc();

            int length, family;
            char address_buffer[sizeof(struct in6_addr)];

            if (inet_pton4(m_ip.c_str(), &address_buffer) == 0) {
                length = sizeof(struct in_addr);
                family = AF_INET;
            } else if (inet_pton6(m_ip.c_str(), &address_buffer) == 0) {
                length = sizeof(struct in6_addr);
                family = AF_INET6;
            } else {
                m_errorno = CALL_E_INVALID_CALL;
                proc();
                return m_errorno;
            }

            ares_gethostbyaddr(m_pThis->m_channel->m_channel,
                               address_buffer,
                               length,
                               family,
                               callback,
                               this);

            return CHECK_ERROR(CALL_E_PENDDING);
        }

        virtual void proc()
        {
            m_pThis->m_channel->m_queryLastOk = m_errorno != ARES_ECONNREFUSED;
            m_pThis->m_channel->m_activeQueryCnt.dec();

            if(m_errorno == CALL_E_INVALID_CALL) {
                ready(CHECK_ERROR(m_errorno));
                return;
            }

            if (m_errorno != 0)
                ready(CHECK_ERROR(Runtime::setError(ares_strerror(m_errorno))));
            else {
                ready(0);
            }
        }

        static void callback(void* arg, int status, int timeouts, struct hostent* host)
        {
            asyncReverse* pThis = (asyncReverse*)arg;

            if (status != ARES_SUCCESS) {
                pThis->m_errorno = status;
                pThis->m_pThis->m_channel->AsyncCallback(pThis);
                return;
            }

            for (uint32_t i = 0; host->h_aliases[i] != NULL; ++i) {
                pThis->m_retVal->append(host->h_aliases[i]);
            }
            pThis->m_pThis->m_channel->AsyncCallback(pThis);
            return;
        }

    public:
        int32_t m_errorno;
        obj_ptr<DnsClient> m_pThis;
        exlib::string m_ip;
        obj_ptr<NArray> m_retVal;
    };

    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    retVal = new NArray();
    return (new asyncReverse(ip, retVal, ac, this))->call();
}

result_t DnsClient::cancel()
{
    if (m_channel)
        ares_cancel(m_channel->m_channel);
    return 0;
}

static bool isValidIp(const char* url, bool &ipv6, exlib::string& ip, exlib::string& port) {
    size_t uLen = qstrlen(url);
    if(uLen > INET6_ADDRSTRLEN + 8)
        return false;

    _parser p(url, uLen);
    int32_t p1, p2;

    if(p.get() == '[') {
        p.skipUntil(']');
        if(p.get() != ']')
            return false;
        p1 = p.pos;
        p.skip();
        if(p.get() == 0) {
            ip.append(url + 1, p1 - 1);
        } else if(p.get() == ':') {
            p.skip();
            p2 = p.pos;
            char ch;
            while(0 != (ch = p.get())) {
                if(!qisdigit(ch))
                    return false;
                p.skip();
            }
            port.append(url + p2);
            ip.append(url + 1, p1 - 1);
        } else
            return false;

        struct sockaddr_in6 addr;
        int result = inet_pton6(ip.c_str(), &(addr.sin6_addr));
        if(result < 0)
            return false;
        ipv6 = true;
    } else {
        p.skipUntil(':');
        if(p.get() != ':') {
            ip.append(url);
        } else {
            p1 = p.pos;
            p.skip();
            p2 = p.pos;
            p.skipUntil(':');
            if(p.get() == ':') {
                ip.append(url);
                struct sockaddr_in6 addr;
                int result = inet_pton6(ip.c_str(), &(addr.sin6_addr));
                if(result < 0)
                    return false;
                ipv6 = true;
                return true;
            }
            p.pos = p2;
            char ch;
            while(0 != (ch = p.get())) {
                if(!qisdigit(ch))
                    return false;
                p.skip();
            }
            port.append(url + p2);
            ip.append(url, p1);
        }
        struct sockaddr_in addr;
        int result = inet_pton4(ip.c_str(), &(addr.sin_addr));
        if(result < 0)
            return false;
    }
    return true;
}

result_t DnsClient::setServers(v8::Local<v8::Array> servers, AsyncEvent* ac)
{
    if ((intptr_t)m_channel->m_activeQueryCnt > 0)
        return CHECK_ERROR(Runtime::setError("Error: set DNS server pending"));

    uint32_t len = servers->Length();

    if (len == 0) {
        ares_set_servers(m_channel->m_channel, NULL);
        return 0;
    }

    std::vector<ares_addr_port_node> vServers(len);
    ares_addr_port_node* last = NULL;

    int err;
    uint32_t j = 0;

    for (uint32_t i = 0; i < len; i++) {
        bool ipv6 = false;
        int nPort = 0;
        exlib::string ip, sPort;
        v8::Local<v8::Value> v8StrAddr = servers->Get(i);
        exlib::string strAddr;

        if(v8StrAddr->IsUndefined() || v8StrAddr->IsNull())
            continue;
        GetArgumentValue(v8StrAddr, strAddr);
        if(strAddr.length() == 0)
            continue;

        if(!isValidIp(strAddr.c_str(), ipv6, ip, sPort))
            return CHECK_ERROR(Runtime::setError("Error: invalid ip address"));

        if(ip.length() <= 0)
            return CHECK_ERROR(Runtime::setError("Error: invalid ip address"));

        if(sPort.length() > 0)
            nPort = atoi(sPort.c_str());
        if(nPort == 0)
            nPort = IANA_DNS_PORT;

        int fam = ipv6 ? 6: 4;

        ares_addr_port_node* cur = &vServers[j++];

        cur->tcp_port = cur->udp_port = nPort;
        switch (fam) {
        case 4:
            cur->family = AF_INET;
            err = inet_pton4(ip.c_str(), &cur->addr);
            break;
        case 6:
            cur->family = AF_INET6;
            err = inet_pton6(ip.c_str(), &cur->addr);
            break;
        default:
            break;
        }

        if (err)
            break;

        cur->next = NULL;

        if (last != NULL)
            last->next = cur;

        last = cur;
    }

    if (err == 0)
        err = ares_set_servers_ports(m_channel->m_channel, &vServers[0]);
    else
        err = ARES_EBADSTR;

    if (err == ARES_SUCCESS) {
        m_channel->m_bDefaultServer = false;
        return 0;
    }

    return CHECK_ERROR(Runtime::setError(ares_strerror(err)));
}

result_t DnsClient::getServers(obj_ptr<NArray>& retVal)
{
    int32_t err;
    ares_addr_port_node* servers;
    err = ares_get_servers_ports(m_channel->m_channel, &servers);
    if(ARES_SUCCESS != err)
        return CHECK_ERROR(Runtime::setError(ares_strerror(err)));
    
    ares_addr_port_node* cur = servers;

    retVal = new NArray();
    for (uint32_t i = 0; cur != nullptr; ++i, cur = cur->next) {
        exlib::string addr, ipv6AddrPrefix = "[";
        char ip[INET6_ADDRSTRLEN];
        char sPort[6] = {0};

        const void* caddr = (const void*)(&cur->addr);
        inet_ntop(cur->family, caddr, ip, sizeof(ip));

        addr += ip;
        if(!cur->udp_port || cur->udp_port == IANA_DNS_PORT) {
            retVal->append(addr);
            continue;
        }
        if(cur->family == AF_INET6) {
            addr.append("]");
            ipv6AddrPrefix.append(addr);
            addr = ipv6AddrPrefix;
        }
        sprintf(sPort, "%d", cur->udp_port);
        addr.append(":");
        addr.append(sPort);
        retVal->append(addr);
    }

    ares_free_data(servers);
    return 0;
}
}