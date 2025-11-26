/*
 * net.cpp
 *
 *  Created on: Apr 22, 2012
 *      Author: lion
 */

#include "object.h"
#include "ifs/dns.h"
#include "inetAddr.h"
#include "AsyncUV.h"
#include <boost/preprocessor.hpp>

namespace fibjs {

DECLARE_MODULE(dns);

result_t dns_base::resolve(exlib::string name, obj_ptr<NArray>& retVal, AsyncEvent* ac)
{
    class resolve_data : public uv_getaddrinfo_t {
    public:
        resolve_data(obj_ptr<NArray>& retVal, AsyncEvent* ac)
            : _retVal(retVal)
            , _ac(ac)
        {
        }

    public:
        obj_ptr<NArray>& _retVal;
        AsyncEvent* _ac;
    };

    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    addrinfo hints = { 0, AF_UNSPEC, SOCK_STREAM, IPPROTO_TCP, 0, 0, 0, 0 };

    resolve_data* resolver = new resolve_data(retVal, ac);
    int r = uv_getaddrinfo(
        s_uv_loop, resolver,
        [](uv_getaddrinfo_t* _resolver, int status, struct addrinfo* res) {
            resolve_data* resolver = (resolve_data*)_resolver;

            if (status < 0) {
                uv_freeaddrinfo(res);
                delete resolver;

                resolver->_ac->post(status);
                return;
            }

            obj_ptr<NArray> arr = new NArray();
            for (struct addrinfo* ptr = res; ptr != NULL; ptr = ptr->ai_next) {
                inetAddr addr_info;
                addr_info.init(ptr->ai_addr);
                arr->append(addr_info.str());
            }

            resolver->_retVal = arr;
            resolver->_ac->post(0);

            uv_freeaddrinfo(res);
            delete resolver;
        },
        name.c_str(), NULL, &hints);

    if (r < 0) {
        delete resolver;
        return CHECK_ERROR(r);
    }

    return CALL_E_PENDDING;
}

result_t dns_base::lookup(exlib::string name, v8::Local<v8::Object> options, Variant& retVal, AsyncEvent* ac)
{
    class LookupOptions : public obj_base {
    public:
        LOAD_OPTIONS(LookupOptions, (family)(all));

    public:
        std::optional<std::variant<exlib::string, int32_t>> family = 0;
        std::optional<bool> all = false;
    };

    class resolve_data : public uv_getaddrinfo_t {
    public:
        resolve_data(LookupOptions* opt, Variant& retVal, AsyncEvent* ac)
            : _opt(opt)
            , _retVal(retVal)
            , _ac(ac)
        {
        }

    public:
        obj_ptr<LookupOptions> _opt;
        Variant& _retVal;
        AsyncEvent* _ac;
    };

    if (ac->isSync()) {
        obj_ptr<LookupOptions> opt;
        Isolate* isolate = Isolate::current(options);
        result_t hr = LookupOptions::load(options, opt);
        if (hr < 0)
            return hr;

        if (opt->family->index() == 0) {
            exlib::string family = std::get<exlib::string>(opt->family.value());
            if (family == "IPv4")
                opt->family = 4;
            else if (family == "IPv6")
                opt->family = 6;
            else
                return Runtime::setError("Invalid family: " + family);
        } else {
            int32_t family = std::get<int32_t>(opt->family.value());
            if (family != 0 && family != 4 && family != 6)
                return Runtime::setError("Invalid family: " + std::to_string(family));
        }

        ac->m_ctx.resize(1);
        ac->m_ctx[0] = opt;

        return CHECK_ERROR(CALL_E_NOSYNC);
    }

    addrinfo hints = { 0, AF_UNSPEC, SOCK_STREAM, IPPROTO_TCP, 0, 0, 0, 0 };

    resolve_data* resolver = new resolve_data((LookupOptions*)ac->m_ctx[0].object(), retVal, ac);
    int r = uv_getaddrinfo(
        s_uv_loop, resolver,
        [](uv_getaddrinfo_t* _resolver, int status, struct addrinfo* res) {
            resolve_data* resolver = (resolve_data*)_resolver;
            int32_t family = std::get<int32_t>(resolver->_opt->family.value());

            if (status < 0) {
                uv_freeaddrinfo(res);
                delete resolver;

                resolver->_ac->post(status);
                return;
            }

            if (resolver->_opt->all.value()) {
                obj_ptr<NArray> arr = new NArray();
                for (struct addrinfo* ptr = res; ptr != NULL; ptr = ptr->ai_next) {
                    inetAddr addr_info;
                    addr_info.init(ptr->ai_addr);

                    if (family == 0
                        || (family == 4 && ptr->ai_family == AF_INET)
                        || (family == 6 && ptr->ai_family == AF_INET6)) {
                        obj_ptr<NObject> obj = new NObject();

                        obj->add("address", addr_info.str());
                        obj->add("family", ptr->ai_family == AF_INET ? 4 : 6);

                        arr->append(obj);
                    }
                }

                resolver->_retVal = arr;
                resolver->_ac->post(0);
            } else {
                struct addrinfo* ptr = NULL;
                for (ptr = res; ptr != NULL; ptr = ptr->ai_next) {
                    if (family == 0
                        || (family == 4 && ptr->ai_family == AF_INET)
                        || (family == 6 && ptr->ai_family == AF_INET6)) {
                        inetAddr addr_info;
                        addr_info.init(ptr->ai_addr);
                        resolver->_retVal = addr_info.str();
                        break;
                    }
                }

                if (ptr != NULL)
                    resolver->_ac->post(0);
                else
                    resolver->_ac->post(Runtime::setError("No address found"));
            }

            uv_freeaddrinfo(res);
            delete resolver;
        },
        name.c_str(), NULL, &hints);

    if (r < 0) {
        delete resolver;
        return CHECK_ERROR(r);
    }

    return CALL_E_PENDDING;
}

}
