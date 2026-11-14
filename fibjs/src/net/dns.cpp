/*
 * dns.cpp
 *
 * lookup / lookupService: the libuv side of the dns module, plus the
 * getaddrinfo flags.  The c-ares side lives in dns_cares.cpp.
 */

#include "object.h"
#include "ifs/dns.h"
#include "inetAddr.h"
#include "AsyncUV.h"
#include "dns_cares.h"
#include "errhelp.h"

#include <ada.h>
#include <cmath>
#ifndef _WIN32
#include <netdb.h>
#endif

#ifndef AI_ADDRCONFIG
#define AI_ADDRCONFIG 0
#endif
#ifndef AI_ALL
#define AI_ALL 0
#endif
#ifndef AI_V4MAPPED
#define AI_V4MAPPED 0
#endif

namespace fibjs {

DECLARE_MODULE(dns);

using LookupOptions = Options_dns_lookup_options;

// The generated tuple renders {address, family} and (address, family).  With
// all:true Node returns the bare address array and a two-argument callback, so
// the lookup case overrides the two virtual renderers.
class DnsLookupResult : public dns_base::LookupType {
public:
    virtual void to_args(Isolate* isolate,
        std::vector<v8::Local<v8::Value>>& args)
    {
        if (m_all) {
            args.push_back(GetReturnValue(isolate, address));
            return;
        }

        LookupType::to_args(isolate, args);
    }

    virtual result_t valueOf(v8::Local<v8::Value>& retVal)
    {
        if (m_all) {
            retVal = GetReturnValue(Isolate::current(), address);
            return result_t();
        }

        return LookupType::valueOf(retVal);
    }

public:
    bool m_all = false;
};

// ---------------------------------------------------------------------------
// option validation
// ---------------------------------------------------------------------------

static result_t invalid_family(const char* name, const exlib::string& value)
{
    return result_t::make(CALL_E_INVALID_DATA)
        .with_type_name(errtype::kTypeError)
        .with_code("ERR_INVALID_ARG_VALUE")
        .with_format("The %s '%s' must be one of: 0, 4, 6. Received %s",
            name[0] == 'o' ? "property" : "argument", name, value.c_str())
        .arg(name, value);
}

static result_t invalid_hints(int32_t value)
{
    return result_t::make(CALL_E_INVALID_DATA)
        .with_type_name(errtype::kTypeError)
        .with_code("ERR_INVALID_ARG_VALUE")
        .with_format("The argument 'hints' is invalid. Received %d", value)
        .arg("hints", value);
}

static result_t invalid_order(const exlib::string& value)
{
    return result_t::make(CALL_E_INVALID_DATA)
        .with_type_name(errtype::kTypeError)
        .with_code("ERR_INVALID_ARG_VALUE")
        .with_format("The property 'options.order' must be one of: 'verbatim', 'ipv4first', 'ipv6first'. Received '%s'",
            value.c_str())
        .arg("order", value);
}

static result_t normalize_lookup_options(LookupOptions* options, int32_t& family,
    int32_t& hints, bool& all, exlib::string& order)
{
    family = 0;
    hints = 0;
    all = false;

    if (!options)
        return result_t();

    if (options->family.has_value()) {
        auto& value = *options->family;

        if (std::holds_alternative<int32_t>(value)) {
            family = std::get<int32_t>(value);
        } else {
            const exlib::string& name = std::get<exlib::string>(value);
            if (name == "IPv4")
                family = 4;
            else if (name == "IPv6")
                family = 6;
            else
                return invalid_family("options.family", name);
        }
    }

    if (family != 0 && family != 4 && family != 6)
        return invalid_family("options.family",
            exlib::string(std::to_string(family).c_str()));

    if (options->hints.has_value())
        hints = *options->hints;

    int32_t valid_hints = AI_ADDRCONFIG | AI_ALL | AI_V4MAPPED;
    if ((hints & ~valid_hints) != 0)
        return invalid_hints(hints);

    all = options->all.value_or(false);

    if (options->order.has_value()) {
        order = *options->order;
        if (order != "verbatim" && order != "ipv4first" && order != "ipv6first")
            return invalid_order(order);
    } else if (options->verbatim.has_value()) {
        order = *options->verbatim ? "verbatim" : "ipv4first";
    }

    return result_t();
}

// ---------------------------------------------------------------------------
// lookup
// ---------------------------------------------------------------------------

class lookup_data : public uv_getaddrinfo_t {
public:
    lookup_data(obj_ptr<dns_base::LookupType>& retVal, int32_t family,
        bool all, exlib::string order, exlib::string name, AsyncHandle ac)
        : _retVal(retVal)
        , _family(family)
        , _all(all)
        , _order(order)
        , _name(name)
        , _ac(std::move(ac))
    {
    }

public:
    obj_ptr<dns_base::LookupType>& _retVal;
    int32_t _family;
    bool _all;
    exlib::string _order;
    exlib::string _name;
    AsyncHandle _ac;
};

static result_t literal_result(const exlib::string& name, bool all,
    obj_ptr<dns_base::LookupType>& retVal)
{
    struct in_addr addr4;
    struct in6_addr addr6;
    int32_t family;

    if (uv_inet_pton(AF_INET, name.c_str(), &addr4) == 0)
        family = 4;
    else if (uv_inet_pton(AF_INET6, name.c_str(), &addr6) == 0)
        family = 6;
    else
        return result_t::make(CALL_E_INVALID_DATA).with_code("EINVAL");

    obj_ptr<DnsLookupResult> result = new DnsLookupResult();
    result->m_all = all;

    if (all) {
        obj_ptr<NObject> item = new NObject();
        item->add("address", name);
        item->add("family", family);

        obj_ptr<NArray> addresses = new NArray();
        addresses->append(item);

        result->address = addresses;
        result->family = 0;
    } else {
        result->address = name;
        result->family = family;
    }

    retVal = result;
    return result_t();
}

static result_t run_lookup(const exlib::string& name, LookupOptions* options,
    obj_ptr<dns_base::LookupType>& retVal, AsyncHandle ac)
{
    int32_t family;
    int32_t hints;
    bool all;
    exlib::string order;

    if (name.empty())
        return result_t::make(CALL_E_INVALID_DATA)
            .with_type_name(errtype::kTypeError)
            .with_code("ERR_INVALID_ARG_VALUE")
            .with_format("The argument 'hostname' must be a non-empty string. Received ''")
            .arg("hostname", name);

    result_t hr = normalize_lookup_options(options, family, hints, all, order);
    if (hr.failed())
        return hr;

    if (ac.isSync())
        return result_t::nosync();

    hr = literal_result(name, all, retVal);
    if (hr.ok())
        return result_t();

    if (order.empty())
        dns_cares_get_default_order(Isolate::current(), order);

    if (order.empty())
        order = "verbatim";

    std::string idna_name = ada::idna::to_ascii(name);
    exlib::string query_name = idna_name.empty() ? name : exlib::string(idna_name);

    addrinfo addrinfo_hints = { 0 };
    addrinfo_hints.ai_family = family == 4 ? AF_INET : (family == 6 ? AF_INET6 : AF_UNSPEC);
    addrinfo_hints.ai_socktype = SOCK_STREAM;
    addrinfo_hints.ai_flags = hints;

    lookup_data* resolver = new lookup_data(retVal, family, all, order, name,
        std::move(ac));

    int32_t r = uv_getaddrinfo(
        s_uv_loop, resolver,
        [](uv_getaddrinfo_t* _resolver, int status, struct addrinfo* res) {
            lookup_data* resolver = (lookup_data*)_resolver;

            int32_t family = resolver->_family;
            bool all = resolver->_all;
            exlib::string hostname = resolver->_name;

            if (status < 0) {
                if (res)
                    uv_freeaddrinfo(res);

                exlib::string code;
                if (status == UV_EAI_NODATA || status == UV_EAI_NONAME)
                    code = "ENOTFOUND";

                resolver->_ac.post(result_t::from_system(status)
                    .with_type_name(errtype::kError)
                    .with_syscall("getaddrinfo")
                    .with_hostname(hostname)
                    .with_code(code)
                    .arg("hostname", hostname)
                    .arg("family", family)
                    .arg("all", all));
                delete resolver;
                return;
            }

            std::vector<std::pair<exlib::string, int32_t>> values;

            for (struct addrinfo* ptr = res; ptr; ptr = ptr->ai_next) {
                if (ptr->ai_family != AF_INET && ptr->ai_family != AF_INET6)
                    continue;
                if (family == 4 && ptr->ai_family != AF_INET)
                    continue;
                if (family == 6 && ptr->ai_family != AF_INET6)
                    continue;

                inetAddr addr_info;
                addr_info.init(ptr->ai_addr);
                values.push_back({ addr_info.str(),
                    ptr->ai_family == AF_INET ? 4 : 6 });
            }

            if (resolver->_order == "ipv4first"
                || resolver->_order == "ipv6first") {
                int32_t first = resolver->_order == "ipv4first" ? 4 : 6;
                std::vector<std::pair<exlib::string, int32_t>> sorted;
                sorted.reserve(values.size());

                for (auto& item : values)
                    if (item.second == first)
                        sorted.push_back(item);
                for (auto& item : values)
                    if (item.second != first)
                        sorted.push_back(item);

                values.swap(sorted);
            }

            if (values.empty()) {
                resolver->_ac.post(result_t::make(CALL_E_INVALID_DATA)
                    .with_type_name(errtype::kError)
                    .with_code("ENOTFOUND")
                    .with_syscall("getaddrinfo")
                    .with_hostname(resolver->_name)
                    .with_message("No address found"));
                uv_freeaddrinfo(res);
                delete resolver;
                return;
            }

            obj_ptr<DnsLookupResult> result = new DnsLookupResult();
            result->m_all = resolver->_all;

            if (resolver->_all) {
                obj_ptr<NArray> addresses = new NArray();
                for (auto& item : values) {
                    obj_ptr<NObject> entry = new NObject();
                    entry->add("address", item.first);
                    entry->add("family", item.second);
                    addresses->append(entry);
                }
                result->address = addresses;
                result->family = 0;
            } else {
                result->address = values[0].first;
                result->family = values[0].second;
            }

            resolver->_retVal = result;
            resolver->_ac.post(result_t());

            uv_freeaddrinfo(res);
            delete resolver;
        },
        query_name.c_str(), NULL, &addrinfo_hints);

    if (r < 0) {
        delete resolver;

        exlib::string code;
        if (r == UV_EAI_NODATA || r == UV_EAI_NONAME)
            code = "ENOTFOUND";

        return result_t::from_system(r)
            .with_syscall("getaddrinfo")
            .with_hostname(name)
            .with_code(code)
            .arg("hostname", name)
            .arg("family", family)
            .arg("all", all);
    }

    return result_t::pending();
}

result_t dns_base::lookup(exlib::string name, LookupOptions* options,
    obj_ptr<LookupType>& retVal, AsyncHandle ac)
{
    return run_lookup(name, options, retVal, std::move(ac));
}

result_t dns_base::lookup(exlib::string name, int32_t family,
    obj_ptr<LookupType>& retVal, AsyncHandle ac)
{
    if (family != 0 && family != 4 && family != 6)
        return invalid_family("family",
            exlib::string(std::to_string(family).c_str()));

    obj_ptr<LookupOptions> options = new LookupOptions();
    options->family = family;
    return run_lookup(name, options, retVal, std::move(ac));
}

// ---------------------------------------------------------------------------
// lookupService
// ---------------------------------------------------------------------------

class lookup_service_data : public uv_getnameinfo_t {
public:
    lookup_service_data(obj_ptr<dns_base::LookupServiceType>& retVal,
        exlib::string address, AsyncHandle ac)
        : _retVal(retVal)
        , _address(address)
        , _ac(std::move(ac))
    {
    }

public:
    obj_ptr<dns_base::LookupServiceType>& _retVal;
    exlib::string _address;
    AsyncHandle _ac;
    inetAddr _addr;
};

result_t dns_base::lookupService(exlib::string address, double port,
    obj_ptr<LookupServiceType>& retVal, AsyncHandle ac)
{
    struct in_addr addr4;
    struct in6_addr addr6;
    int32_t family;

    if (uv_inet_pton(AF_INET, address.c_str(), &addr4) == 0)
        family = AF_INET;
    else if (uv_inet_pton(AF_INET6, address.c_str(), &addr6) == 0)
        family = AF_INET6;
    else
        return result_t::make(CALL_E_INVALID_DATA)
            .with_type_name(errtype::kTypeError)
            .with_code("ERR_INVALID_ARG_VALUE")
            .with_format("The argument 'address' is invalid. Received '%s'",
                address.c_str())
            .arg("address", address);

    if (std::isnan(port) || std::floor(port) != port || port < 0
        || port > 65535)
        return result_t::make(CALL_E_INVALID_DATA)
            .with_type_name(errtype::kRangeError)
            .with_code("ERR_SOCKET_BAD_PORT")
            .with_format("Port should be >= 0 and < 65536. Received %s.",
                errhelp::describe_received(Variant(port)).c_str())
            .arg("port", port);

    if (ac.isSync())
        return result_t::nosync();

    lookup_service_data* resolver = new lookup_service_data(retVal, address,
        std::move(ac));

    resolver->_addr.init(family);
    if (family == AF_INET) {
        resolver->_addr.addr4.sin_addr = addr4;
        resolver->_addr.addr4.sin_port = htons((uint16_t)port);
    } else {
        resolver->_addr.addr6.sin6_addr = addr6;
        resolver->_addr.addr6.sin6_port = htons((uint16_t)port);
    }

    int32_t r = uv_getnameinfo(
        s_uv_loop, resolver,
        [](uv_getnameinfo_t* _resolver, int status, const char* hostname,
            const char* service) {
            lookup_service_data* resolver = (lookup_service_data*)_resolver;

            if (status < 0) {
                exlib::string code;
                if (status == UV_EAI_NODATA || status == UV_EAI_NONAME)
                    code = "ENOTFOUND";

                resolver->_ac.post(result_t::from_system(status)
                    .with_type_name(errtype::kError)
                    .with_syscall("getnameinfo")
                    .with_hostname(resolver->_address)
                    .with_code(code)
                    .arg("hostname", resolver->_address));
                delete resolver;
                return;
            }

            obj_ptr<LookupServiceType> result = new LookupServiceType();
            result->hostname = hostname ? hostname : "";
            result->service = service ? service : "";

            resolver->_retVal = result;
            resolver->_ac.post(result_t());
            delete resolver;
        },
        (struct sockaddr*)&resolver->_addr, NI_NAMEREQD);

    if (r < 0) {
        delete resolver;

        exlib::string code;
        if (r == UV_EAI_NODATA || r == UV_EAI_NONAME)
            code = "ENOTFOUND";

        return result_t::from_system(r)
            .with_syscall("getnameinfo")
            .with_hostname(address)
            .with_code(code)
            .arg("hostname", address);
    }

    return result_t::pending();
}

} // namespace fibjs
