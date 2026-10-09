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

result_t dns_base::resolve(exlib::string name, std::vector<exlib::string>& retVal, AsyncHandle ac)
{
    class resolve_data : public uv_getaddrinfo_t {
    public:
        resolve_data(std::vector<exlib::string>& retVal, AsyncHandle ac)
            : _retVal(retVal)
            , _ac(std::move(ac))
        {
        }

    public:
        std::vector<exlib::string>& _retVal;
        AsyncHandle _ac;
    };

    if (ac.isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    addrinfo hints = { 0, AF_UNSPEC, SOCK_STREAM, IPPROTO_TCP, 0, 0, 0, 0 };

    resolve_data* resolver = new resolve_data(retVal, std::move(ac));
    int r = uv_getaddrinfo(
        s_uv_loop, resolver,
        [](uv_getaddrinfo_t* _resolver, int status, struct addrinfo* res) {
            resolve_data* resolver = (resolve_data*)_resolver;

            if (status < 0) {
                uv_freeaddrinfo(res);
                // 先投递再释放：resolver 在 post 之前保持有效；投递即移交，无需中转
                resolver->_ac.post(status);
                delete resolver;
                return;
            }

            for (struct addrinfo* ptr = res; ptr != NULL; ptr = ptr->ai_next) {
                inetAddr addr_info;
                addr_info.init(ptr->ai_addr);
                resolver->_retVal.push_back(addr_info.str());
            }

            resolver->_ac.post(0);

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

result_t dns_base::lookup(exlib::string name, v8::Local<v8::Object> options, Variant& retVal, AsyncHandle ac)
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
        resolve_data(LookupOptions* opt, exlib::string name, Variant& retVal, AsyncHandle ac)
            : _opt(opt)
            , _name(name)
            , _retVal(retVal)
            , _ac(std::move(ac))
        {
        }

    public:
        obj_ptr<LookupOptions> _opt;
        exlib::string _name;
        Variant& _retVal;
        AsyncHandle _ac;
    };

    if (ac.isSync()) {
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
                return Runtime::setError(ErrorPayload::make("TypeError", CALL_E_INVALIDARG)
                                             .format("Invalid family: %s", family.c_str())
                                             .arg("family", family));
        } else {
            int32_t family = std::get<int32_t>(opt->family.value());
            if (family != 0 && family != 4 && family != 6)
                return Runtime::setError(ErrorPayload::make("TypeError", CALL_E_INVALIDARG)
                                             .format("Invalid family: %d", family)
                                             .arg("family", family));
        }

        ac.ctxv().resize(1);
        ac.ctxv()[0] = opt;

        return CHECK_ERROR(CALL_E_NOSYNC);
    }

    result_t ctx_hr = ac.ctx(0);
    if (ctx_hr < 0)
        return ctx_hr;

    addrinfo hints = { 0, AF_UNSPEC, SOCK_STREAM, IPPROTO_TCP, 0, 0, 0, 0 };

    LookupOptions* opt = (LookupOptions*)ac.ctxv()[0].object();
    resolve_data* resolver = new resolve_data(opt, name, retVal, std::move(ac));
    int r = uv_getaddrinfo(
        s_uv_loop, resolver,
        [](uv_getaddrinfo_t* _resolver, int status, struct addrinfo* res) {
            resolve_data* resolver = (resolve_data*)_resolver;
            int32_t family = std::get<int32_t>(resolver->_opt->family.value());

            if (status < 0) {
                uv_freeaddrinfo(res);
                // 先投递再释放：resolver 在 post 之前保持有效；投递即移交，无需中转
                exlib::string hostname = resolver->_name;
                bool all = resolver->_opt->all.value();

                exlib::string code;
                if (status == UV_EAI_NODATA || status == UV_EAI_NONAME)
                    code = "ENOTFOUND";
                ErrorPayload payload = ErrorPayload::from_uv(status)
                                           .with_syscall("getaddrinfo")
                                           .with_hostname(hostname)
                                           .arg("hostname", hostname)
                                           .arg("family", family)
                                           .arg("all", all);
                if (!code.empty())
                    payload.with_code(code);
                setErrorPayload(payload);
                resolver->_ac.post(status);
                delete resolver;
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
                resolver->_ac.post(0);
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
                    resolver->_ac.post(0);
                else
                    resolver->_ac.post(Runtime::setError("No address found"));
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
