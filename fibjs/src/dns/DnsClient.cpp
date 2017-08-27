/*
 * DnsClient.h
 *
 *  Created on: Aug 16, 2017
 *      Author: Rube
 */

#include "DnsClient.h"
#include "List.h"
#include "Map.h"
#include "SimpleObject.h"
#include <cares/ares.h>

#if defined(__ANDROID__) || \
    defined(__MINGW32__) || \
    defined(__OpenBSD__) || \
    defined(_MSC_VER)

#include <cares/include/nameser.h>
#else
# include <arpa/nameser.h>
#endif

namespace fibjs {
    inline const char* ToDNSErrorCodeString(int status) {
        switch (status) {
        #define V(code) case ARES_##code: return #code;
            V(EADDRGETNETWORKPARAMS)
            V(EBADFAMILY)
            V(EBADFLAGS)
            V(EBADHINTS)
            V(EBADNAME)
            V(EBADQUERY)
            V(EBADRESP)
            V(EBADSTR)
            V(ECANCELLED)
            V(ECONNREFUSED)
            V(EDESTRUCTION)
            V(EFILE)
            V(EFORMERR)
            V(ELOADIPHLPAPI)
            V(ENODATA)
            V(ENOMEM)
            V(ENONAME)
            V(ENOTFOUND)
            V(ENOTIMP)
            V(ENOTINITIALIZED)
            V(EOF)
            V(EREFUSED)
            V(ESERVFAIL)
            V(ETIMEOUT)
        #undef V
        }

        return "UNKNOWN_ARES_ERROR";
    }

    class asyncDNSQuery : public AsyncState
    {
    public:
        asyncDNSQuery(exlib::string host, obj_ptr<List_base>& retVal, int dnsclass, int type, ares_callback callback, AsyncEvent *ac)
                : AsyncState(ac)
                , m_retVal((obj_ptr<object_base>&)retVal)
                , m_type(type)
                , m_host(host)
                , m_dnsclass(dnsclass)
                , m_callback(callback)
        {
            set(query);
        }

        asyncDNSQuery(exlib::string host, obj_ptr<object_base>& retVal, int dnsclass, int type, ares_callback callback, AsyncEvent *ac)
                : AsyncState(ac)
                , m_retVal(retVal)
                , m_type(type)
                , m_host(host)
                , m_dnsclass(dnsclass)
                , m_callback(callback)
        {
            set(query);
        }

        static int32_t query(AsyncState* pState, int32_t n)
        {
            int res, rs_status;
            ares_channel channel = nullptr;
            asyncDNSQuery* pThis = (asyncDNSQuery*)pState;

            rs_status = ares_library_init(ARES_LIB_INIT_ALL);
            if (rs_status != ARES_SUCCESS){
                ares_destroy(channel);
                ares_library_cleanup();
                return CHECK_ERROR(Runtime::setError(ToDNSErrorCodeString(rs_status)));
            }

            res = ares_init(&channel);
            if(res != ARES_SUCCESS){
                ares_destroy(channel);
                ares_library_cleanup();
                return CHECK_ERROR(Runtime::setError(ToDNSErrorCodeString(res)));
            }

            pThis->m_channel = channel;
            ares_query(channel, pThis->m_host.c_str(), pThis->m_dnsclass, pThis->m_type, pThis->m_callback, pThis);
            pThis->set(loop);

            return 0;
        }

        static int32_t loop(AsyncState* pState, int32_t n)
        {
            asyncDNSQuery* pThis = (asyncDNSQuery*)pState;

            int nfds;
            fd_set readers, writers;
            timeval tv, *tvp;

            for(;;)
            {
                FD_ZERO(&readers);
                FD_ZERO(&writers);
                nfds = ares_fds(pThis->m_channel, &readers, &writers);
                if (nfds == 0)
                    break;
                tvp = ares_timeout(pThis->m_channel, NULL, &tv);
                select(nfds, &readers, &writers, NULL, tvp);
                ares_process(pThis->m_channel, &readers, &writers);
            }

            return 0;
        }

        static int32_t error(AsyncState* pState, int32_t n){
            asyncDNSQuery* pThis = (asyncDNSQuery*)pState;

            if (pThis->errorno != 0){
                if (pThis->m_channel) {
                    ares_destroy(pThis->m_channel);
                }
                ares_library_cleanup();
                pThis->done();
                return CHECK_ERROR(Runtime::setError(ToDNSErrorCodeString(pThis->errorno)));
            }

            pThis->done();
            return 0;
        }

        obj_ptr<object_base>& m_retVal;
        int errorno = 0;
    private:
        int m_type;
        exlib::string m_host;
        ares_channel m_channel;
        int m_dnsclass;
        ares_callback m_callback;
    };

    class asyncMxQuery: public asyncDNSQuery
    {
    public:
        class ResolveMxResult: public SimpleObject {
        public:
            ResolveMxResult(ares_mx_reply* mx_reply)
            {
                add("exchange", mx_reply->host);
                add("priority", mx_reply->priority);
            }
        };

        static void callback(void *arg, int status, int timeouts, unsigned char *abuf, int alen)
        {
            struct ares_mx_reply * mx_reply = nullptr;
            obj_ptr<List> mx_list = new List();
            asyncDNSQuery* pThis = (asyncDNSQuery*)arg;

            int rs_status = ares_parse_mx_reply(abuf, alen, &mx_reply);
            if (rs_status != ARES_SUCCESS){
                pThis->errorno = rs_status;
                pThis->set(error);
                return;
            }

            ares_mx_reply* current = mx_reply;
            for (uint32_t i = 0; current != nullptr; ++i, current = current->next) {
                mx_list->append(new ResolveMxResult(current));
            }

            ares_free_data(mx_reply);
            pThis->m_retVal = mx_list;
            pThis->done();
        }

        asyncMxQuery(exlib::string hostName, obj_ptr<List_base>& retVal, AsyncEvent *ac)
                : asyncDNSQuery(hostName, retVal, ns_c_in, ns_t_mx, callback, ac){};
    };

    class asyncSrvQuery: public asyncDNSQuery
    {
    public:
        class ResolveSrvResult: public SimpleObject {
        public:
            ResolveSrvResult(ares_srv_reply * srv_reply)
            {
                add("name", srv_reply->host);
                add("port", srv_reply->port);
                add("priority", srv_reply->priority);
                add("weight", srv_reply->weight);
            }
        };

        static void callback(void *arg, int status, int timeouts, unsigned char *abuf, int alen)
        {
            struct ares_srv_reply* srv_start;
            asyncDNSQuery* pThis = (asyncDNSQuery*)arg;
            obj_ptr<List> srv_list = new List();

            int rs_status = ares_parse_srv_reply(abuf, alen, &srv_start);
            if (rs_status != ARES_SUCCESS){
                pThis->errorno = rs_status;
                pThis->set(error);
                return;
            }

            ares_srv_reply* current = srv_start;
            for (uint32_t i = 0; current != nullptr; ++i, current = current->next) {
                srv_list->append(new ResolveSrvResult(current));
            }

            ares_free_data(srv_start);
            pThis->m_retVal = srv_list;
            pThis->done();
        }

        asyncSrvQuery(exlib::string host, obj_ptr<List_base>& retVal, AsyncEvent *ac)
                : asyncDNSQuery(host, retVal, ns_c_in, ns_t_srv, callback, ac){};
    };

    class asyncTxtQuery: public asyncDNSQuery
    {
    public:
        static void callback(void *arg, int status, int timeouts, unsigned char *abuf, int alen)
        {
            struct ares_txt_ext* txt_out;
            asyncDNSQuery* pThis = (asyncDNSQuery*)arg;
            obj_ptr<List> txt_list = new List();

            int rs_status = ares_parse_txt_reply_ext(abuf, alen, &txt_out);
            if (rs_status != ARES_SUCCESS){
                pThis->errorno = rs_status;
                pThis->set(error);
                return;
            }

            struct ares_txt_ext* current = txt_out;
            int32_t len;
            obj_ptr<List> txt_result = new List();

            for (uint32_t i = 0; current != nullptr; current = current->next) {

                if (current->record_start) {
                    txt_result->get_length(len);
                    if (len != 0){
                        txt_list->append(txt_result);
                    }

                    txt_result = new List();
                    i = 0;
                }

                txt_result->append((char *)current->txt);
            }

            txt_result->get_length(len);
            if (len != 0){
                txt_list->append(txt_result);
            }

            ares_free_data(txt_out);
            pThis->m_retVal = txt_list;
            pThis->done();
        }

        asyncTxtQuery(exlib::string host, obj_ptr<List_base>& retVal, AsyncEvent *ac)
        : asyncDNSQuery(host, retVal, ns_c_in, ns_t_txt, callback, ac){};
    };

    class asyncNsQuery: public asyncDNSQuery
    {
    public:
        static void callback(void *arg, int status, int timeouts, unsigned char *abuf, int alen)
        {
            asyncDNSQuery* pThis = (asyncDNSQuery*)arg;
            obj_ptr<List> ns_list = new List();
            hostent* host;

            int rs_status = ares_parse_ns_reply(abuf, alen, &host);
            if (rs_status != ARES_SUCCESS){
                pThis->errorno = rs_status;
                pThis->set(error);
                return;
            }

            for (uint32_t i = 0; host->h_aliases[i] != nullptr; ++i){
                ns_list->append(host->h_aliases[i]);
            }

            ares_free_hostent(host);
            pThis->m_retVal = ns_list;
            pThis->done();
        }

        asyncNsQuery(exlib::string host, obj_ptr<List_base>& retVal, AsyncEvent *ac)
                : asyncDNSQuery(host, retVal, ns_c_in, ns_t_ns, callback, ac){};
    };

    class asyncSoaQuery: public asyncDNSQuery
    {
    public:
        class ResolveSoaResult: public SimpleObject {
        public:
            ResolveSoaResult(ares_soa_reply* soa_out)
            {
                add("nsname", soa_out->nsname);
                add("hostmaster", soa_out->hostmaster);
                add("serial", (int)soa_out->serial);
                add("refresh", (int)soa_out->refresh);
                add("retry", (int)soa_out->retry);
                add("expire", (int)soa_out->expire);
                add("minttl", (int)soa_out->minttl);
            }
        };

        static void callback(void *arg, int status, int timeouts, unsigned char *abuf, int alen)
        {
            asyncDNSQuery* pThis = (asyncDNSQuery*)arg;
            ares_soa_reply* soa_out;

            int rs_status = ares_parse_soa_reply(abuf, alen, &soa_out);
            if (rs_status != ARES_SUCCESS){
                pThis->errorno = rs_status;
                pThis->set(error);
                return;
            }

            pThis->m_retVal = new ResolveSoaResult(soa_out);
            ares_free_data(soa_out);
            pThis->done();
        }

        asyncSoaQuery(exlib::string host, obj_ptr<object_base>& retVal, AsyncEvent *ac)
        : asyncDNSQuery(host, retVal, ns_c_in, ns_t_soa, callback, ac){};
    };

    class asyncPtrQuery: public asyncDNSQuery
    {
    public:

        static void callback(void *arg, int status, int timeouts, unsigned char *abuf, int alen)
        {
            asyncDNSQuery* pThis = (asyncDNSQuery*)arg;
            hostent* host;
            obj_ptr<List> ptr_list = new List();

            int rs_status = ares_parse_ptr_reply(abuf, alen, NULL, 0, AF_INET, &host);
            if (rs_status != ARES_SUCCESS){
                pThis->errorno = rs_status;
                pThis->set(error);
                return;
            }

            for (uint32_t i = 0; host->h_aliases[i] != NULL; i++) {
                ptr_list->append(host->h_aliases[i]);
            }

            ares_free_hostent(host);
            pThis->m_retVal = ptr_list;
            pThis->done();
        }

        asyncPtrQuery(exlib::string host, obj_ptr<List_base>& retVal, AsyncEvent *ac)
        : asyncDNSQuery(host, retVal, ns_c_in, ns_t_ptr, callback, ac) {};
    };

    class asyncNaptrQuery: public asyncDNSQuery
    {
    public:

        class ResolveNaptrResult: public SimpleObject {
        public:
            ResolveNaptrResult(ares_naptr_reply* naptr_reply)
            {
                add("flags", (char *)naptr_reply->flags);
                add("service", (char *)naptr_reply->service);
                add("regexp", (char *)naptr_reply->regexp);
                add("replacement", naptr_reply->replacement);
                add("order", naptr_reply->order);
                add("preference", naptr_reply->preference);
            }
        };

        static void callback(void * arg, int status, int timeouts, unsigned char *abuf, int alen)
        {
            asyncDNSQuery* pThis = (asyncDNSQuery*)arg;
            ares_naptr_reply* naptr_start;
            obj_ptr<List> naptr_list = new List();

            int rs_status = ares_parse_naptr_reply(abuf, alen, &naptr_start);
            if (rs_status != ARES_SUCCESS){
                pThis->errorno = rs_status;
                pThis->set(error);
                return;
            }

            ares_naptr_reply* current = naptr_start;
            for (uint32_t i = 0; current != nullptr; ++i, current = current->next) {
                naptr_list->append(new ResolveNaptrResult(current));
            }

            ares_free_data(naptr_start);
            pThis->m_retVal = naptr_list;
            pThis->done();
        }

        asyncNaptrQuery(exlib::string host, obj_ptr<List_base>& retVal, AsyncEvent *ac)
        : asyncDNSQuery(host, retVal, ns_c_in, ns_t_naptr, callback, ac) {};
    };

    class asyncCnameQuery: public asyncDNSQuery
    {
    public:

        static void callback(void * arg, int status, int timeouts, unsigned char *abuf, int alen)
        {
            asyncDNSQuery *pThis = (asyncDNSQuery *) arg;
            hostent* host;
            obj_ptr<List> cname_list = new List();

            int rs_status = ares_parse_a_reply(abuf, alen, &host, nullptr, nullptr);
            if (rs_status != ARES_SUCCESS){
                pThis->errorno = rs_status;
                pThis->set(error);
                return;
            }

            cname_list->append(host->h_name);
            ares_free_hostent(host);
            pThis->m_retVal = cname_list;
            pThis->done();
        }

        asyncCnameQuery(exlib::string host, obj_ptr<List_base>& retVal, AsyncEvent *ac)
        : asyncDNSQuery(host, retVal, ns_c_in, ns_t_cname, callback, ac) {};
    };


    class asyncAQuery: public asyncDNSQuery
    {
    public:
        class ResolveAResult: public SimpleObject {
        public:
            ResolveAResult(int ttl, char* ip)
            {
                add("address", ip);
                add("ttl", ttl);
            }
        };

        static int inet_ntop4(const unsigned char *src, char *dst, size_t size) {
            static const char fmt[] = "%u.%u.%u.%u";
            char tmp[16];
            int l;

            l = snprintf(tmp, sizeof(tmp), fmt, src[0], src[1], src[2], src[3]);
            if (l <= 0 || (size_t) l >= size) {
                return 1;
            }
            strncpy(dst, tmp, size);
            dst[size - 1] = '\0';
            return 0;
        }

        static void callback(void * arg, int status, int timeouts, unsigned char *abuf, int alen)
        {
            asyncAQuery *pThis = (asyncAQuery *) arg;
            hostent* host;
            ares_addrttl addrttls[256];
            int naddrttls = sizeof(addrttls);
            obj_ptr<List> a_list = new List();

            int rs_status = ares_parse_a_reply(abuf, alen, &host, (ares_addrttl*)addrttls, &naddrttls);
            if (rs_status != ARES_SUCCESS){
                pThis->errorno = rs_status;
                pThis->set(error);
                return;
            }

            for (uint32_t i = 0; host->h_addr_list[i] != nullptr; ++i) {
                char ip[INET6_ADDRSTRLEN];
                inet_ntop4((const unsigned char *) host->h_addr_list[i], ip, sizeof(ip));

                if (pThis->m_ttl) {
                    a_list->append(new ResolveAResult(addrttls[i].ttl, ip));
                } else {
                    a_list->append(ip);
                }
            }

            ares_free_hostent(host);
            pThis->m_retVal = a_list;
            pThis->done();
        }

        asyncAQuery(exlib::string host, bool ttl, obj_ptr<List_base>& retVal, AsyncEvent *ac)
                : asyncDNSQuery(host, retVal, ns_c_in, ns_t_a, callback, ac),
                  m_ttl(ttl){};

    public:
        bool m_ttl = false;
    };

    class asyncAAAAQuery: public asyncDNSQuery
    {
    public:
        class ResolveAAAAResult: public SimpleObject {
        public:
            ResolveAAAAResult(int ttl, char* ip)
            {
                add("address", ip);
                add("ttl", ttl);
            }
        };

        static int inet_ntop6(const unsigned char *src, char *dst, size_t size) {

            char tmp[46], *tp;
            struct { int base, len; } best, cur;
            unsigned int words[sizeof(struct in6_addr) / sizeof(uint16_t)];
            int i;

            memset(words, '\0', sizeof words);
            for (i = 0; i < (int) sizeof(struct in6_addr); i++)
                words[i / 2] |= (src[i] << ((1 - (i % 2)) << 3));
            best.base = -1;
            best.len = 0;
            cur.base = -1;
            cur.len = 0;
            for (i = 0; i < (int) (sizeof(words) / sizeof((words)[0])); i++) {
                if (words[i] == 0) {
                    if (cur.base == -1)
                        cur.base = i, cur.len = 1;
                    else
                        cur.len++;
                } else {
                    if (cur.base != -1) {
                        if (best.base == -1 || cur.len > best.len)
                            best = cur;
                        cur.base = -1;
                    }
                }
            }
            if (cur.base != -1) {
                if (best.base == -1 || cur.len > best.len)
                    best = cur;
            }
            if (best.base != -1 && best.len < 2)
                best.base = -1;

            tp = tmp;
            for (i = 0; i < (int) (sizeof(words) / sizeof((words)[0])); i++) {
                if (best.base != -1 && i >= best.base &&
                    i < (best.base + best.len)) {
                    if (i == best.base)
                        *tp++ = ':';
                    continue;
                }

                if (i != 0)
                    *tp++ = ':';

                if (i == 6 && best.base == 0 && (best.len == 6 ||
                                                 (best.len == 7 && words[7] != 0x0001) ||
                                                 (best.len == 5 && words[5] == 0xffff))) {
                    int err = asyncAQuery::inet_ntop4(src+12, tp, sizeof tmp - (tp - tmp));
                    if (err)
                        return err;
                    tp += strlen(tp);
                    break;
                }
                tp += sprintf(tp, "%x", words[i]);
            }

            if (best.base != -1 && (best.base + best.len) == (sizeof(words) / sizeof((words)[0])))
                *tp++ = ':';
            *tp++ = '\0';

            if ((size_t)(tp - tmp) > size) {
                return 1;
            }
            strcpy(dst, tmp);
            return 0;
        }

        static void callback(void * arg, int status, int timeouts, unsigned char *abuf, int alen)
        {
            asyncAQuery *pThis = (asyncAQuery *) arg;
            hostent* host;
            ares_addr6ttl addrttls[256];
            int naddrttls = sizeof(addrttls);
            obj_ptr<List> aaaa_list = new List();

            int rs_status = ares_parse_aaaa_reply(abuf, alen, &host, (ares_addr6ttl*)addrttls, &naddrttls);
            if (rs_status != ARES_SUCCESS){
                pThis->errorno = rs_status;
                pThis->set(error);
                return;
            }

            for (uint32_t i = 0; host->h_addr_list[i] != nullptr; ++i) {
                char ip[INET6_ADDRSTRLEN];
                inet_ntop6((const unsigned char *) host->h_addr_list[i], ip, sizeof(ip));

                if (pThis->m_ttl) {
                    aaaa_list->append(new ResolveAAAAResult(addrttls[i].ttl, ip));
                } else {
                    aaaa_list->append(ip);
                }
            }

            ares_free_hostent(host);
            pThis->m_retVal = aaaa_list;
            pThis->done();
        }

        asyncAAAAQuery(exlib::string host, bool ttl, obj_ptr<List_base>& retVal, AsyncEvent *ac)
                : asyncDNSQuery(host, retVal, ns_c_in, ns_t_aaaa, callback, ac),
                  m_ttl(ttl){};

    public:
        bool m_ttl = false;
    };

    result_t DnsClient_base::_new(obj_ptr<DnsClient_base>& retVal, v8::Local<v8::Object> This)
    {
        retVal = new DnsClient();
        return 0;
    }

    result_t DnsClient::resolve(exlib::string host, exlib::string type, obj_ptr<object_base>& retVal,
                                     AsyncEvent *ac) {

        if (ac->isSync())
            return CHECK_ERROR(CALL_E_NOSYNC);

        if (strcmp("MX", type.c_str()) == 0){
            return resolveMx(host, (obj_ptr<List_base>&)retVal, ac);
        } else if (strcmp("TXT", type.c_str()) == 0){
            return resolveTxt(host, (obj_ptr<List_base>&)retVal, ac);
        } else if (strcmp("SRV", type.c_str()) == 0){
            return resolveSrv(host, (obj_ptr<List_base>&)retVal, ac);
        } else if (strcmp("A", type.c_str()) == 0){
            return resolve4(host, new Map() , (obj_ptr<List_base>&)retVal, ac);
        } else if (strcmp("AAAA", type.c_str()) == 0){
            return resolve6(host, new Map(), (obj_ptr<List_base>&)retVal, ac);
        } else if (strcmp("CNAME", type.c_str()) == 0){
            return resolveCname(host, (obj_ptr<List_base>&)retVal, ac);
        } else if (strcmp("NAPTR", type.c_str()) == 0){
            return resolveNaptr(host, (obj_ptr<List_base>&)retVal, ac);
        } else if (strcmp("PTR", type.c_str()) == 0){
            return resolvePtr(host, (obj_ptr<List_base>&)retVal, ac);
        } else if (strcmp("NS", type.c_str()) == 0){
            return resolveNs(host, (obj_ptr<List_base>&)retVal, ac);
        } else if (strcmp("SOA", type.c_str()) == 0){
            return resolveSoa(host, retVal, ac);
        }

        return CHECK_ERROR(CALL_E_INVALIDARG);
    }

    result_t DnsClient::resolve4(exlib::string host, Map_base* options, obj_ptr<List_base> &retVal,
                                      AsyncEvent *ac) {

        if (ac->isSync())
            return CHECK_ERROR(CALL_E_NOSYNC);

        Variant v;
        options->get("ttl", v);

        return (new asyncAQuery(host, v.boolVal(), retVal, ac))->post(0);
    }

    result_t DnsClient::resolve4(exlib::string host, obj_ptr<List_base> &retVal,
                                 AsyncEvent *ac) {

        if (ac->isSync())
            return CHECK_ERROR(CALL_E_NOSYNC);

        return (new asyncAQuery(host, false, retVal, ac))->post(0);
    }

    result_t DnsClient::resolve6(exlib::string host, Map_base* options, obj_ptr<List_base> &retVal,
                                      AsyncEvent *ac) {

        if (ac->isSync())
            return CHECK_ERROR(CALL_E_NOSYNC);

        Variant v;
        options->get("ttl", v);

        return (new asyncAAAAQuery(host, v.boolVal(), retVal, ac))->post(0);
    }

    result_t DnsClient::resolve6(exlib::string host, obj_ptr<List_base> &retVal,
                                 AsyncEvent *ac) {

        if (ac->isSync())
            return CHECK_ERROR(CALL_E_NOSYNC);

        return (new asyncAAAAQuery(host, false, retVal, ac))->post(0);
    }

    result_t DnsClient::resolveCname(exlib::string host, obj_ptr<List_base> &retVal, AsyncEvent *ac) {

        if (ac->isSync())
            return CHECK_ERROR(CALL_E_NOSYNC);

        return (new asyncCnameQuery(host, retVal, ac))->post(0);
    }

    result_t DnsClient::resolveNaptr(exlib::string host, obj_ptr<List_base> &retVal, AsyncEvent *ac) {

        if (ac->isSync())
            return CHECK_ERROR(CALL_E_NOSYNC);

        return (new asyncNaptrQuery(host, retVal, ac))->post(0);
    }

    result_t DnsClient::resolveNs(exlib::string host, obj_ptr<List_base> &retVal, AsyncEvent *ac) {

        if (ac->isSync())
            return CHECK_ERROR(CALL_E_NOSYNC);

        return (new asyncNsQuery(host, retVal, ac))->post(0);
    }

    result_t DnsClient::resolvePtr(exlib::string host, obj_ptr<List_base> &retVal, AsyncEvent *ac) {

        if (ac->isSync())
            return CHECK_ERROR(CALL_E_NOSYNC);

        return (new asyncPtrQuery(host, retVal, ac))->post(0);
    }

    result_t DnsClient::resolveSoa(exlib::string host, obj_ptr<object_base> &retVal, AsyncEvent *ac) {

        if (ac->isSync())
            return CHECK_ERROR(CALL_E_NOSYNC);

        return (new asyncSoaQuery(host, retVal, ac))->post(0);
    }

    result_t DnsClient::resolveTxt(exlib::string host, obj_ptr<List_base> &retVal, AsyncEvent *ac) {

        if (ac->isSync())
            return CHECK_ERROR(CALL_E_NOSYNC);

        return (new asyncTxtQuery(host, retVal, ac))->post(0);
    }

    result_t DnsClient::resolveSrv(exlib::string host, obj_ptr<List_base> &retVal, AsyncEvent *ac) {

        if (ac->isSync())
            return CHECK_ERROR(CALL_E_NOSYNC);

        return (new asyncSrvQuery(host, retVal, ac))->post(0);
    }

    result_t DnsClient::resolveMx(exlib::string host, obj_ptr<List_base> &retVal, AsyncEvent *ac)
    {
        if (ac->isSync())
            return CHECK_ERROR(CALL_E_NOSYNC);

        return (new asyncMxQuery(host, retVal, ac))->post(0);
    }
}