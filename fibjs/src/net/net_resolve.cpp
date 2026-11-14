#include "object.h"
#include "net_resolve.h"
#include "ifs/dns.h"
#include "Url.h"

#include <uv/include/uv.h>

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

static exlib::string strip_brackets(const exlib::string& host)
{
    if (host.length() >= 2 && host[0] == '[' && host[host.length() - 1] == ']')
        return host.substr(1, host.length() - 2);
    return host;
}

static result_t no_address(const exlib::string& host, int32_t family)
{
    return result_t::from_system(UV_EAI_NONAME)
        .with_syscall("getaddrinfo")
        .with_hostname(host)
        .with_code("ENOTFOUND")
        .arg("hostname", host)
        .arg("family", family);
}

bool valid_lookup_hints(int32_t hints)
{
    int32_t valid = AI_ADDRCONFIG | AI_ALL | AI_V4MAPPED;
    return (hints & ~valid) == 0;
}

int32_t connect_lookup_hints(int32_t family, int32_t hints)
{
#ifdef _WIN32
    (void)family;
    return hints;
#else
    if (family == 0 && hints == 0)
        return AI_ADDRCONFIG;
    return hints;
#endif
}

result_t resolve_host(const exlib::string& host, int32_t family, int32_t hints,
    const exlib::string& order, ResolvedHost& retVal, Isolate* isolate)
{
    if (family != 0 && family != 4 && family != 6)
        return result_t::make(CALL_E_INVALID_DATA)
            .with_type_name(errtype::kTypeError)
            .with_code("ERR_INVALID_ARG_VALUE")
            .with_format("The property 'family' must be one of: 0, 4, 6. Received %d",
                family)
            .arg("family", family);

    if (!valid_lookup_hints(hints))
        return result_t::make(CALL_E_INVALID_DATA)
            .with_type_name(errtype::kTypeError)
            .with_code("ERR_INVALID_ARG_VALUE")
            .with_format("The argument 'hints' is invalid. Received %d", hints)
            .arg("hints", hints);

    exlib::string target = strip_brackets(host);

    if (Url::isIPv4(target)) {
        if (family == 6)
            return no_address(host, family);
        retVal.address = target;
        retVal.family = 4;
        return result_t();
    }

    if (Url::isIPv6(target)) {
        if (family == 4)
            return no_address(host, family);
        retVal.address = target;
        retVal.family = 6;
        return result_t();
    }

    obj_ptr<Options_dns_lookup_options> options = new Options_dns_lookup_options();
    if (family != 0)
        options->family = family;
    options->hints = hints;
    options->all = false;
    if (!order.empty())
        options->order = order;

    obj_ptr<dns_base::LookupType> result;
    result_t hr = dns_base::cc_lookup(target, options, result, isolate);
    if (hr.failed())
        return hr;

    if (result->family != 4 && result->family != 6)
        return no_address(host, family);
    if (result->address.type() != Variant::VT_String)
        return no_address(host, family);

    exlib::string address = result->address.string();
    if (address.empty())
        return no_address(host, family);

    retVal.address = address;
    retVal.family = result->family;
    return result_t();
}

} // namespace fibjs
