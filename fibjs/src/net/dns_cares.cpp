/*
 * dns_cares.cpp
 *
 * The c-ares side of the dns module: resolve* and reverse, the Resolver class
 * and the per-isolate default resolver state.
 */

#include "object.h"
#include "utils.h"
#include "Isolate.h"
#include "ifs/dns.h"
#include "ifs/DnsResolver.h"
#include "cares_channel.h"
#include "dns_cares.h"
#include "errhelp.h"
#include "v8_api.h"
#include <uv/include/uv.h>

#ifndef CARES_STATICLIB
#define CARES_STATICLIB
#endif
#include <ares.h>

#include <mutex>
#include <string.h>
#include <unordered_map>

#include <cmath>
#include <cstdio>

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

// ---------------------------------------------------------------------------
// rrtype table
// ---------------------------------------------------------------------------

struct RrTypeInfo {
    int32_t type;
    const char* syscall;
};

static result_t rrtype_info(const exlib::string& rrtype, RrTypeInfo& info)
{
    const char* name = rrtype.c_str();

    if (rrtype == "A") {
        info = { ARES_REC_TYPE_A, "queryA" };
    } else if (rrtype == "AAAA") {
        info = { ARES_REC_TYPE_AAAA, "queryAaaa" };
    } else if (rrtype == "ANY") {
        info = { ARES_REC_TYPE_ANY, "queryAny" };
    } else if (rrtype == "CAA") {
        info = { ARES_REC_TYPE_CAA, "queryCaa" };
    } else if (rrtype == "CNAME") {
        info = { ARES_REC_TYPE_CNAME, "queryCname" };
    } else if (rrtype == "MX") {
        info = { ARES_REC_TYPE_MX, "queryMx" };
    } else if (rrtype == "NAPTR") {
        info = { ARES_REC_TYPE_NAPTR, "queryNaptr" };
    } else if (rrtype == "NS") {
        info = { ARES_REC_TYPE_NS, "queryNs" };
    } else if (rrtype == "PTR") {
        info = { ARES_REC_TYPE_PTR, "queryPtr" };
    } else if (rrtype == "SOA") {
        info = { ARES_REC_TYPE_SOA, "querySoa" };
    } else if (rrtype == "SRV") {
        info = { ARES_REC_TYPE_SRV, "querySrv" };
    } else if (rrtype == "TLSA") {
        info = { ARES_REC_TYPE_TLSA, "queryTlsa" };
    } else if (rrtype == "TXT") {
        info = { ARES_REC_TYPE_TXT, "queryTxt" };
    } else {
        return result_t::make(CALL_E_INVALID_DATA)
            .with_type_name(errtype::kTypeError)
            .with_code("ERR_INVALID_ARG_VALUE")
            .with_format("The argument 'rrtype' is invalid. Received '%s'",
                name)
            .arg("rrtype", rrtype);
    }

    return result_t();
}

// ---------------------------------------------------------------------------
// per-isolate default resolver
// ---------------------------------------------------------------------------

struct DnsDefaultState : public obj_base {
    obj_ptr<CaresChannel> channel;
    exlib::string order = "verbatim";
};

static std::mutex s_defaults_lock;
static std::unordered_map<Isolate*, obj_ptr<DnsDefaultState>> s_defaults;

static obj_ptr<DnsDefaultState> default_state(Isolate* isolate)
{
    std::lock_guard<std::mutex> lock(s_defaults_lock);

    auto it = s_defaults.find(isolate);
    if (it != s_defaults.end())
        return it->second;

    obj_ptr<DnsDefaultState> state = new DnsDefaultState();
    s_defaults[isolate] = state;
    return state;
}

static result_t default_channel(obj_ptr<CaresChannel>& retVal)
{
    Isolate* isolate = Isolate::current();
    obj_ptr<DnsDefaultState> state = default_state(isolate);

    {
        std::lock_guard<std::mutex> lock(s_defaults_lock);
        if (state->channel) {
            retVal = state->channel;
            return result_t();
        }
    }

    obj_ptr<CaresChannel> channel;
    result_t hr = CaresChannel::create(-1, 4, 0, channel);
    if (hr.failed())
        return hr;

    std::lock_guard<std::mutex> lock(s_defaults_lock);
    if (!state->channel)
        state->channel = channel;

    retVal = state->channel;
    return result_t();
}

result_t dns_cares_get_default_order(Isolate* isolate, exlib::string& retVal)
{
    obj_ptr<DnsDefaultState> state = default_state(isolate);
    retVal = state->order;
    return result_t();
}

result_t dns_cares_set_default_order(Isolate* isolate, exlib::string order)
{
    if (order != "verbatim" && order != "ipv4first" && order != "ipv6first")
        return result_t::make(CALL_E_INVALID_DATA)
            .with_type_name(errtype::kTypeError)
            .with_code("ERR_INVALID_ARG_VALUE")
            .with_format("The argument 'dnsOrder' must be one of: 'verbatim', 'ipv4first', 'ipv6first'. Received '%s'",
                order.c_str())
            .arg("dnsOrder", order);

    obj_ptr<DnsDefaultState> state = default_state(isolate);
    state->order = order;
    return result_t();
}

void dns_cares_cleanup_isolate(Isolate* isolate)
{
    std::lock_guard<std::mutex> lock(s_defaults_lock);
    s_defaults.erase(isolate);
}

// ---------------------------------------------------------------------------
// record parsing helpers
// ---------------------------------------------------------------------------

static const ares_dns_rr_t* answer_rr(const ares_dns_record* rec, size_t i)
{
    return ares_dns_record_rr_get_const(rec, ARES_SECTION_ANSWER, i);
}

static exlib::string rr_str(const ares_dns_rr_t* rr, ares_dns_rr_key_t key)
{
    const char* s = ares_dns_rr_get_str(rr, key);
    return s ? exlib::string(s) : exlib::string();
}

static exlib::string rr_bin(const ares_dns_rr_t* rr, ares_dns_rr_key_t key)
{
    size_t len = 0;
    const unsigned char* p = ares_dns_rr_get_bin(rr, key, &len);
    if (!p || len == 0)
        return exlib::string();
    return exlib::string((const char*)p, len);
}

static exlib::string rr_addr(const ares_dns_rr_t* rr, bool v6)
{
    char buf[INET6_ADDRSTRLEN];

    if (v6) {
        const struct ares_in6_addr* addr = ares_dns_rr_get_addr6(rr, ARES_RR_AAAA_ADDR);
        if (!addr || !ares_inet_ntop(AF_INET6, addr, buf, sizeof(buf)))
            return exlib::string();
    } else {
        const struct in_addr* addr = ares_dns_rr_get_addr(rr, ARES_RR_A_ADDR);
        if (!addr || !ares_inet_ntop(AF_INET, addr, buf, sizeof(buf)))
            return exlib::string();
    }

    return exlib::string(buf);
}

static const char* rr_type_name(ares_dns_rec_type_t type)
{
    switch (type) {
    case ARES_REC_TYPE_A:
        return "A";
    case ARES_REC_TYPE_AAAA:
        return "AAAA";
    case ARES_REC_TYPE_CAA:
        return "CAA";
    case ARES_REC_TYPE_CNAME:
        return "CNAME";
    case ARES_REC_TYPE_MX:
        return "MX";
    case ARES_REC_TYPE_NAPTR:
        return "NAPTR";
    case ARES_REC_TYPE_NS:
        return "NS";
    case ARES_REC_TYPE_PTR:
        return "PTR";
    case ARES_REC_TYPE_SOA:
        return "SOA";
    case ARES_REC_TYPE_SRV:
        return "SRV";
    case ARES_REC_TYPE_TLSA:
        return "TLSA";
    case ARES_REC_TYPE_TXT:
        return "TXT";
    default:
        return "UNKNOWN";
    }
}

static void add_address_record(const ares_dns_rr_t* rr, bool ttl, NArray* out,
    bool any)
{
    bool v6 = ares_dns_rr_get_type(rr) == ARES_REC_TYPE_AAAA;
    exlib::string addr = rr_addr(rr, v6);

    if (addr.empty())
        return;

    if (ttl || any) {
        obj_ptr<NObject> obj = new NObject();
        obj->add("address", addr);
        obj->add("ttl", (int32_t)ares_dns_rr_get_ttl(rr));
        if (any)
            obj->add("type", rr_type_name(ares_dns_rr_get_type(rr)));
        out->append(obj);
    } else {
        out->append(addr);
    }
}

static void add_txt_record(const ares_dns_rr_t* rr, NArray* out, bool any)
{
    size_t count = ares_dns_rr_get_abin_cnt(rr, ARES_RR_TXT_DATA);
    obj_ptr<NArray> chunks = new NArray();

    for (size_t i = 0; i < count; i++) {
        size_t len = 0;
        const unsigned char* p = ares_dns_rr_get_abin(rr, ARES_RR_TXT_DATA, i, &len);
        chunks->append(p ? exlib::string((const char*)p, len) : exlib::string());
    }

    if (any) {
        obj_ptr<NObject> obj = new NObject();
        obj->add("entries", chunks);
        obj->add("type", "TXT");
        out->append(obj);
    } else {
        out->append(chunks);
    }
}

static void add_soa_record(const ares_dns_rr_t* rr, NArray* out, bool any)
{
    obj_ptr<NObject> obj = new NObject();
    obj->add("nsname", rr_str(rr, ARES_RR_SOA_MNAME));
    obj->add("hostmaster", rr_str(rr, ARES_RR_SOA_RNAME));
    // DNS SOA counters are uint32; use Variant number instead of int64 so the
    // JS value is a Number (Node's shape) rather than a BigInt.
    obj->add("serial", (double)ares_dns_rr_get_u32(rr, ARES_RR_SOA_SERIAL));
    obj->add("refresh", (double)ares_dns_rr_get_u32(rr, ARES_RR_SOA_REFRESH));
    obj->add("retry", (double)ares_dns_rr_get_u32(rr, ARES_RR_SOA_RETRY));
    obj->add("expire", (double)ares_dns_rr_get_u32(rr, ARES_RR_SOA_EXPIRE));
    obj->add("minttl", (double)ares_dns_rr_get_u32(rr, ARES_RR_SOA_MINIMUM));
    if (any)
        obj->add("type", "SOA");
    out->append(obj);
}

static void add_value_record(const ares_dns_rr_t* rr, ares_dns_rr_key_t key,
    NArray* out)
{
    obj_ptr<NObject> obj = new NObject();
    obj->add("value", rr_str(rr, key));
    obj->add("type", rr_type_name(ares_dns_rr_get_type(rr)));
    out->append(obj);
}

static void add_mx_record(const ares_dns_rr_t* rr, NArray* out, bool any)
{
    obj_ptr<NObject> obj = new NObject();
    obj->add("exchange", rr_str(rr, ARES_RR_MX_EXCHANGE));
    obj->add("priority",
        (int32_t)ares_dns_rr_get_u16(rr, ARES_RR_MX_PREFERENCE));
    if (any)
        obj->add("type", "MX");
    out->append(obj);
}

static void add_srv_record(const ares_dns_rr_t* rr, NArray* out, bool any)
{
    obj_ptr<NObject> obj = new NObject();
    obj->add("name", rr_str(rr, ARES_RR_SRV_TARGET));
    obj->add("port", (int32_t)ares_dns_rr_get_u16(rr, ARES_RR_SRV_PORT));
    obj->add("priority",
        (int32_t)ares_dns_rr_get_u16(rr, ARES_RR_SRV_PRIORITY));
    obj->add("weight",
        (int32_t)ares_dns_rr_get_u16(rr, ARES_RR_SRV_WEIGHT));
    if (any)
        obj->add("type", "SRV");
    out->append(obj);
}

static void add_naptr_record(const ares_dns_rr_t* rr, NArray* out, bool any)
{
    obj_ptr<NObject> obj = new NObject();
    obj->add("flags", rr_str(rr, ARES_RR_NAPTR_FLAGS));
    obj->add("service", rr_str(rr, ARES_RR_NAPTR_SERVICES));
    obj->add("regexp", rr_str(rr, ARES_RR_NAPTR_REGEXP));
    obj->add("replacement", rr_str(rr, ARES_RR_NAPTR_REPLACEMENT));
    obj->add("order", (int32_t)ares_dns_rr_get_u16(rr, ARES_RR_NAPTR_ORDER));
    obj->add("preference",
        (int32_t)ares_dns_rr_get_u16(rr, ARES_RR_NAPTR_PREFERENCE));
    if (any)
        obj->add("type", "NAPTR");
    out->append(obj);
}

static void add_caa_record(const ares_dns_rr_t* rr, NArray* out, bool any)
{
    obj_ptr<NObject> obj = new NObject();
    obj->add("critical",
        (int32_t)ares_dns_rr_get_u8(rr, ARES_RR_CAA_CRITICAL));
    // Node's ANY parser adds the discriminator before the dynamic tag.
    if (any)
        obj->add("type", "CAA");
    obj->add(rr_str(rr, ARES_RR_CAA_TAG), rr_bin(rr, ARES_RR_CAA_VALUE));
    out->append(obj);
}

static void add_tlsa_record(const ares_dns_rr_t* rr, NArray* out, bool any)
{
    size_t len = 0;
    const unsigned char* data = ares_dns_rr_get_bin(rr, ARES_RR_TLSA_DATA,
        &len);

    obj_ptr<NObject> obj = new NObject();
    obj->add("certUsage",
        (int32_t)ares_dns_rr_get_u8(rr, ARES_RR_TLSA_CERT_USAGE));
    obj->add("selector",
        (int32_t)ares_dns_rr_get_u8(rr, ARES_RR_TLSA_SELECTOR));
    obj->add("match", (int32_t)ares_dns_rr_get_u8(rr, ARES_RR_TLSA_MATCH));

    std::shared_ptr<v8::BackingStore> store = NewBackingStore(len);
    if (len > 0 && data && store->Data())
        memcpy(store->Data(), data, len);
    obj->add("data", store);

    // Node v25's ANY parser forgets the type discriminator for TLSA; keep it
    // here (resolveAny documents a type field on every record) and test it.
    if (any)
        obj->add("type", "TLSA");
    out->append(obj);
}

// Node's AnyTraits parses one record family at a time and appends the groups
// in a fixed order.  A CNAME answer suppresses the A group (the address in an
// ANY response belongs to the CNAME target), while AAAA and the other types
// are kept.  The parser also appends only the first SOA.
template <typename Fn>
static void for_each_answer(const ares_dns_record* rec,
    ares_dns_rec_type_t type, const Fn& fn)
{
    size_t count = ares_dns_record_rr_cnt(rec, ARES_SECTION_ANSWER);

    for (size_t i = 0; i < count; i++) {
        const ares_dns_rr_t* rr = answer_rr(rec, i);
        if (rr && ares_dns_rr_get_type(rr) == type)
            fn(rr);
    }
}

static void add_any_records(const ares_dns_record* rec, NArray* out)
{
    bool has_cname = false;

    for_each_answer(rec, ARES_REC_TYPE_CNAME,
        [&](const ares_dns_rr_t*) { has_cname = true; });

    if (has_cname) {
        for_each_answer(rec, ARES_REC_TYPE_CNAME,
            [&](const ares_dns_rr_t* rr) {
                add_value_record(rr, ARES_RR_CNAME_CNAME, out);
            });
    } else {
        for_each_answer(rec, ARES_REC_TYPE_A,
            [&](const ares_dns_rr_t* rr) {
                add_address_record(rr, false, out, true);
            });
    }

    for_each_answer(rec, ARES_REC_TYPE_AAAA,
        [&](const ares_dns_rr_t* rr) {
            add_address_record(rr, false, out, true);
        });
    for_each_answer(rec, ARES_REC_TYPE_MX,
        [&](const ares_dns_rr_t* rr) { add_mx_record(rr, out, true); });
    for_each_answer(rec, ARES_REC_TYPE_NS,
        [&](const ares_dns_rr_t* rr) {
            add_value_record(rr, ARES_RR_NS_NSDNAME, out);
        });
    for_each_answer(rec, ARES_REC_TYPE_TXT,
        [&](const ares_dns_rr_t* rr) { add_txt_record(rr, out, true); });
    for_each_answer(rec, ARES_REC_TYPE_SRV,
        [&](const ares_dns_rr_t* rr) { add_srv_record(rr, out, true); });
    for_each_answer(rec, ARES_REC_TYPE_PTR,
        [&](const ares_dns_rr_t* rr) {
            add_value_record(rr, ARES_RR_PTR_DNAME, out);
        });
    for_each_answer(rec, ARES_REC_TYPE_NAPTR,
        [&](const ares_dns_rr_t* rr) { add_naptr_record(rr, out, true); });

    bool has_soa = false;
    for_each_answer(rec, ARES_REC_TYPE_SOA,
        [&](const ares_dns_rr_t* rr) {
            if (!has_soa) {
                add_soa_record(rr, out, true);
                has_soa = true;
            }
        });

    for_each_answer(rec, ARES_REC_TYPE_TLSA,
        [&](const ares_dns_rr_t* rr) { add_tlsa_record(rr, out, true); });
    for_each_answer(rec, ARES_REC_TYPE_CAA,
        [&](const ares_dns_rr_t* rr) { add_caa_record(rr, out, true); });
}

static result_t fill_answer(int32_t qtype, bool ttl,
    const ares_dns_record* rec, NArray* out)
{
    if (qtype == ARES_REC_TYPE_ANY) {
        add_any_records(rec, out);
        return result_t();
    }

    size_t count = ares_dns_record_rr_cnt(rec, ARES_SECTION_ANSWER);

    for (size_t i = 0; i < count; i++) {
        const ares_dns_rr_t* rr = answer_rr(rec, i);
        if (!rr)
            continue;

        ares_dns_rec_type_t type = ares_dns_rr_get_type(rr);

        if ((int32_t)type != qtype)
            continue;

        switch (type) {
        case ARES_REC_TYPE_A:
        case ARES_REC_TYPE_AAAA:
            add_address_record(rr, ttl, out, false);
            break;
        case ARES_REC_TYPE_CNAME:
            out->append(rr_str(rr, ARES_RR_CNAME_CNAME));
            break;
        case ARES_REC_TYPE_NS:
            out->append(rr_str(rr, ARES_RR_NS_NSDNAME));
            break;
        case ARES_REC_TYPE_PTR:
            out->append(rr_str(rr, ARES_RR_PTR_DNAME));
            break;
        case ARES_REC_TYPE_MX:
            add_mx_record(rr, out, false);
            break;
        case ARES_REC_TYPE_TXT:
            add_txt_record(rr, out, false);
            break;
        case ARES_REC_TYPE_SRV:
            add_srv_record(rr, out, false);
            break;
        case ARES_REC_TYPE_NAPTR:
            add_naptr_record(rr, out, false);
            break;
        case ARES_REC_TYPE_SOA:
            add_soa_record(rr, out, false);
            break;
        case ARES_REC_TYPE_CAA:
            add_caa_record(rr, out, false);
            break;
        case ARES_REC_TYPE_TLSA:
            add_tlsa_record(rr, out, false);
            break;
        default:
            break;
        }
    }

    return result_t();
}

static result_t parse_names(int32_t type, const ares_dns_record* rec,
    std::vector<exlib::string>& retVal)
{
    ares_dns_rr_key_t key;

    switch (type) {
    case ARES_REC_TYPE_CNAME:
        key = ARES_RR_CNAME_CNAME;
        break;
    case ARES_REC_TYPE_NS:
        key = ARES_RR_NS_NSDNAME;
        break;
    case ARES_REC_TYPE_PTR:
        key = ARES_RR_PTR_DNAME;
        break;
    default:
        return result_t();
    }

    size_t count = ares_dns_record_rr_cnt(rec, ARES_SECTION_ANSWER);
    for (size_t i = 0; i < count; i++) {
        const ares_dns_rr_t* rr = answer_rr(rec, i);
        if (!rr || (int32_t)ares_dns_rr_get_type(rr) != type)
            continue;

        exlib::string name = rr_str(rr, key);
        if (!name.empty())
            retVal.push_back(name);
    }

    return result_t();
}

static result_t parse_soa(int32_t type, bool ttl, const ares_dns_record* rec,
    Variant& retVal)
{
    (void)ttl;
    size_t count = ares_dns_record_rr_cnt(rec, ARES_SECTION_ANSWER);

    for (size_t i = 0; i < count; i++) {
        const ares_dns_rr_t* rr = answer_rr(rec, i);
        if (!rr || ares_dns_rr_get_type(rr) != ARES_REC_TYPE_SOA)
            continue;

        NArray tmp;
        add_soa_record(rr, &tmp, false);
        retVal = tmp.m_array[0];
        return result_t();
    }

    return result_t::make(CALL_E_INVALID_DATA).with_code("ENODATA");
}

// ---------------------------------------------------------------------------
// module-level resolve methods
// ---------------------------------------------------------------------------

static result_t module_query(const exlib::string& name, const RrTypeInfo& info,
    bool ttl, obj_ptr<NArray>& retVal, AsyncHandle ac)
{
    obj_ptr<CaresChannel> channel;
    result_t hr = default_channel(channel);
    if (hr.failed())
        return hr;

    return channel->query_array(name, info.type, ttl, fill_answer, info.syscall,
        retVal, std::move(ac));
}

static result_t module_query_names(const exlib::string& name,
    const RrTypeInfo& info, std::vector<exlib::string>& retVal, AsyncHandle ac)
{
    obj_ptr<CaresChannel> channel;
    result_t hr = default_channel(channel);
    if (hr.failed())
        return hr;

    return channel->query_names(name, info.type, parse_names, info.syscall,
        retVal, std::move(ac));
}

result_t dns_base::resolve(exlib::string name, exlib::string rrtype,
    Variant& retVal, AsyncHandle ac)
{
    RrTypeInfo info;
    result_t hr = rrtype_info(rrtype, info);
    if (hr.failed())
        return hr;

    if (ac.isSync())
        return result_t::nosync();

    obj_ptr<CaresChannel> channel;
    hr = default_channel(channel);
    if (hr.failed())
        return hr;

    if (info.type == ARES_REC_TYPE_SOA)
        return channel->query(name, info.type, false, parse_soa, info.syscall,
            retVal, std::move(ac));

    return channel->query_variant_array(name, info.type, false, fill_answer,
        info.syscall, retVal, std::move(ac));
}

result_t dns_base::resolve4(exlib::string name,
    Options_dns_resolve4_options* options, obj_ptr<NArray>& retVal,
    AsyncHandle ac)
{
    if (ac.isSync())
        return result_t::nosync();

    RrTypeInfo info = { ARES_REC_TYPE_A, "queryA" };
    bool ttl = options && options->ttl.value_or(false);
    return module_query(name, info, ttl, retVal, std::move(ac));
}

result_t dns_base::resolve6(exlib::string name,
    Options_dns_resolve6_options* options, obj_ptr<NArray>& retVal,
    AsyncHandle ac)
{
    if (ac.isSync())
        return result_t::nosync();

    RrTypeInfo info = { ARES_REC_TYPE_AAAA, "queryAaaa" };
    bool ttl = options && options->ttl.value_or(false);
    return module_query(name, info, ttl, retVal, std::move(ac));
}

result_t dns_base::resolveAny(exlib::string name, obj_ptr<NArray>& retVal,
    AsyncHandle ac)
{
    if (ac.isSync())
        return result_t::nosync();

    RrTypeInfo info = { ARES_REC_TYPE_ANY, "queryAny" };
    return module_query(name, info, false, retVal, std::move(ac));
}

result_t dns_base::resolveCaa(exlib::string name, obj_ptr<NArray>& retVal,
    AsyncHandle ac)
{
    if (ac.isSync())
        return result_t::nosync();

    RrTypeInfo info = { ARES_REC_TYPE_CAA, "queryCaa" };
    return module_query(name, info, false, retVal, std::move(ac));
}

result_t dns_base::resolveCname(exlib::string name,
    std::vector<exlib::string>& retVal, AsyncHandle ac)
{
    if (ac.isSync())
        return result_t::nosync();

    RrTypeInfo info = { ARES_REC_TYPE_CNAME, "queryCname" };
    return module_query_names(name, info, retVal, std::move(ac));
}

result_t dns_base::resolveMx(exlib::string name, obj_ptr<NArray>& retVal,
    AsyncHandle ac)
{
    if (ac.isSync())
        return result_t::nosync();

    RrTypeInfo info = { ARES_REC_TYPE_MX, "queryMx" };
    return module_query(name, info, false, retVal, std::move(ac));
}

result_t dns_base::resolveNaptr(exlib::string name, obj_ptr<NArray>& retVal,
    AsyncHandle ac)
{
    if (ac.isSync())
        return result_t::nosync();

    RrTypeInfo info = { ARES_REC_TYPE_NAPTR, "queryNaptr" };
    return module_query(name, info, false, retVal, std::move(ac));
}

result_t dns_base::resolveNs(exlib::string name,
    std::vector<exlib::string>& retVal, AsyncHandle ac)
{
    if (ac.isSync())
        return result_t::nosync();

    RrTypeInfo info = { ARES_REC_TYPE_NS, "queryNs" };
    return module_query_names(name, info, retVal, std::move(ac));
}

result_t dns_base::resolvePtr(exlib::string name,
    std::vector<exlib::string>& retVal, AsyncHandle ac)
{
    if (ac.isSync())
        return result_t::nosync();

    RrTypeInfo info = { ARES_REC_TYPE_PTR, "queryPtr" };
    return module_query_names(name, info, retVal, std::move(ac));
}

result_t dns_base::resolveSoa(exlib::string name, Variant& retVal,
    AsyncHandle ac)
{
    if (ac.isSync())
        return result_t::nosync();

    obj_ptr<CaresChannel> channel;
    result_t hr = default_channel(channel);
    if (hr.failed())
        return hr;

    return channel->query(name, ARES_REC_TYPE_SOA, false, parse_soa, "querySoa",
        retVal, std::move(ac));
}

result_t dns_base::resolveSrv(exlib::string name, obj_ptr<NArray>& retVal,
    AsyncHandle ac)
{
    if (ac.isSync())
        return result_t::nosync();

    RrTypeInfo info = { ARES_REC_TYPE_SRV, "querySrv" };
    return module_query(name, info, false, retVal, std::move(ac));
}

result_t dns_base::resolveTlsa(exlib::string name, obj_ptr<NArray>& retVal,
    AsyncHandle ac)
{
    if (ac.isSync())
        return result_t::nosync();

    RrTypeInfo info = { ARES_REC_TYPE_TLSA, "queryTlsa" };
    return module_query(name, info, false, retVal, std::move(ac));
}

result_t dns_base::resolveTxt(exlib::string name, obj_ptr<NArray>& retVal,
    AsyncHandle ac)
{
    if (ac.isSync())
        return result_t::nosync();

    RrTypeInfo info = { ARES_REC_TYPE_TXT, "queryTxt" };
    return module_query(name, info, false, retVal, std::move(ac));
}

result_t dns_base::reverse(exlib::string ip,
    std::vector<exlib::string>& retVal, AsyncHandle ac)
{
    struct in_addr addr4;
    struct ares_in6_addr addr6;

    if (uv_inet_pton(AF_INET, ip.c_str(), &addr4) != 0
        && uv_inet_pton(AF_INET6, ip.c_str(), &addr6) != 0)
        return result_t::from_system(UV_EINVAL)
            .with_syscall("getHostByAddr")
            .with_hostname(ip)
            .arg("hostname", ip);

    if (ac.isSync())
        return result_t::nosync();

    obj_ptr<CaresChannel> channel;
    result_t hr = default_channel(channel);
    if (hr.failed())
        return hr;

    return channel->reverse(ip, retVal, std::move(ac));
}

result_t dns_base::getServers(std::vector<exlib::string>& retVal)
{
    obj_ptr<CaresChannel> channel;
    result_t hr = default_channel(channel);
    if (hr.failed())
        return hr;

    return channel->getServers(retVal);
}

static result_t array_to_strings(v8::Local<v8::Array> arr,
    std::vector<exlib::string>& retVal)
{
    int32_t len = arr->Length();

    for (int32_t i = 0; i < len; i++) {
        v8::Local<v8::Value> item = arr->Get(Isolate::current()->context(), i)
                                       .ToLocalChecked();

        if (!item->IsString()) {
            char name[32];
            snprintf(name, sizeof(name), "servers[%d]", i);
            return errhelp::invalid_arg_type(name, "of type string",
                Variant(item));
        }

        exlib::string s;
        result_t hr = GetConfigValue(arr, i, s, true);
        if (hr.failed())
            return hr;
        retVal.push_back(s);
    }

    return result_t();
}

result_t dns_base::setServers(v8::Local<v8::Array> servers)
{
    std::vector<exlib::string> list;
    result_t hr = array_to_strings(servers, list);
    if (hr.failed())
        return hr;

    // Node creates a new default Resolver and swaps it in.  Pending queries on
    // the old channel keep it alive and still complete; they must not make the
    // module-level call fail (only Resolver.prototype.setServers checks pending).
    obj_ptr<CaresChannel> channel;
    hr = CaresChannel::create(-1, 4, 0, channel);
    if (hr.failed())
        return hr;

    hr = channel->setServers(list);
    if (hr.failed())
        return hr;

    Isolate* isolate = Isolate::current(servers);
    obj_ptr<DnsDefaultState> state = default_state(isolate);

    std::lock_guard<std::mutex> lock(s_defaults_lock);
    state->channel = channel;
    return result_t();
}

result_t dns_base::getDefaultResultOrder(exlib::string& retVal)
{
    return dns_cares_get_default_order(Isolate::current(), retVal);
}

result_t dns_base::setDefaultResultOrder(exlib::string order)
{
    return dns_cares_set_default_order(Isolate::current(), order);
}

result_t dns_base::get_ADDRCONFIG(int32_t& retVal)
{
    retVal = AI_ADDRCONFIG;
    return result_t();
}

result_t dns_base::get_ALL(int32_t& retVal)
{
    retVal = AI_ALL;
    return result_t();
}

result_t dns_base::get_V4MAPPED(int32_t& retVal)
{
    retVal = AI_V4MAPPED;
    return result_t();
}

// ---------------------------------------------------------------------------
// DnsResolver
// ---------------------------------------------------------------------------

class DnsResolver : public DnsResolver_base {
public:
    virtual result_t resolve(exlib::string name, exlib::string rrtype,
        Variant& retVal, AsyncHandle ac)
    {
        RrTypeInfo info;
        result_t hr = rrtype_info(rrtype, info);
        if (hr.failed())
            return hr;

        if (ac.isSync())
            return result_t::nosync();

        if (!m_channel)
            return cares_error(ARES_EDESTRUCTION, info.syscall, name);

        if (info.type == ARES_REC_TYPE_SOA)
            return m_channel->query(name, info.type, false, parse_soa,
                info.syscall, retVal, std::move(ac));

        return m_channel->query_variant_array(name, info.type, false,
            fill_answer, info.syscall, retVal, std::move(ac));
    }

    virtual result_t resolve4(exlib::string name,
        Options_DnsResolver_resolve4_options* options, obj_ptr<NArray>& retVal,
        AsyncHandle ac)
    {
        if (ac.isSync())
            return result_t::nosync();

        bool ttl = options && options->ttl.value_or(false);
        return query_array(name, ARES_REC_TYPE_A, ttl, "queryA", retVal,
            std::move(ac));
    }

    virtual result_t resolve6(exlib::string name,
        Options_DnsResolver_resolve6_options* options, obj_ptr<NArray>& retVal,
        AsyncHandle ac)
    {
        if (ac.isSync())
            return result_t::nosync();

        bool ttl = options && options->ttl.value_or(false);
        return query_array(name, ARES_REC_TYPE_AAAA, ttl, "queryAaaa", retVal,
            std::move(ac));
    }

    virtual result_t resolveAny(exlib::string name, obj_ptr<NArray>& retVal,
        AsyncHandle ac)
    {
        if (ac.isSync())
            return result_t::nosync();

        return query_array(name, ARES_REC_TYPE_ANY, false, "queryAny", retVal,
            std::move(ac));
    }

    virtual result_t resolveCaa(exlib::string name, obj_ptr<NArray>& retVal,
        AsyncHandle ac)
    {
        if (ac.isSync())
            return result_t::nosync();

        return query_array(name, ARES_REC_TYPE_CAA, false, "queryCaa", retVal,
            std::move(ac));
    }

    virtual result_t resolveCname(exlib::string name,
        std::vector<exlib::string>& retVal, AsyncHandle ac)
    {
        if (ac.isSync())
            return result_t::nosync();

        return query_names(name, ARES_REC_TYPE_CNAME, "queryCname", retVal,
            std::move(ac));
    }

    virtual result_t resolveMx(exlib::string name, obj_ptr<NArray>& retVal,
        AsyncHandle ac)
    {
        if (ac.isSync())
            return result_t::nosync();

        return query_array(name, ARES_REC_TYPE_MX, false, "queryMx", retVal,
            std::move(ac));
    }

    virtual result_t resolveNaptr(exlib::string name, obj_ptr<NArray>& retVal,
        AsyncHandle ac)
    {
        if (ac.isSync())
            return result_t::nosync();

        return query_array(name, ARES_REC_TYPE_NAPTR, false, "queryNaptr", retVal,
            std::move(ac));
    }

    virtual result_t resolveNs(exlib::string name,
        std::vector<exlib::string>& retVal, AsyncHandle ac)
    {
        if (ac.isSync())
            return result_t::nosync();

        return query_names(name, ARES_REC_TYPE_NS, "queryNs", retVal,
            std::move(ac));
    }

    virtual result_t resolvePtr(exlib::string name,
        std::vector<exlib::string>& retVal, AsyncHandle ac)
    {
        if (ac.isSync())
            return result_t::nosync();

        return query_names(name, ARES_REC_TYPE_PTR, "queryPtr", retVal,
            std::move(ac));
    }

    virtual result_t resolveSoa(exlib::string name, Variant& retVal,
        AsyncHandle ac)
    {
        if (ac.isSync())
            return result_t::nosync();

        if (!m_channel)
            return cares_error(ARES_EDESTRUCTION, "querySoa", name);

        return m_channel->query(name, ARES_REC_TYPE_SOA, false, parse_soa,
            "querySoa", retVal, std::move(ac));
    }

    virtual result_t resolveSrv(exlib::string name, obj_ptr<NArray>& retVal,
        AsyncHandle ac)
    {
        if (ac.isSync())
            return result_t::nosync();

        return query_array(name, ARES_REC_TYPE_SRV, false, "querySrv", retVal,
            std::move(ac));
    }

    virtual result_t resolveTlsa(exlib::string name, obj_ptr<NArray>& retVal,
        AsyncHandle ac)
    {
        if (ac.isSync())
            return result_t::nosync();

        return query_array(name, ARES_REC_TYPE_TLSA, false, "queryTlsa", retVal,
            std::move(ac));
    }

    virtual result_t resolveTxt(exlib::string name, obj_ptr<NArray>& retVal,
        AsyncHandle ac)
    {
        if (ac.isSync())
            return result_t::nosync();

        return query_array(name, ARES_REC_TYPE_TXT, false, "queryTxt", retVal,
            std::move(ac));
    }

    virtual result_t reverse(exlib::string ip,
        std::vector<exlib::string>& retVal, AsyncHandle ac)
    {
        struct in_addr addr4;
        struct ares_in6_addr addr6;

        if (uv_inet_pton(AF_INET, ip.c_str(), &addr4) != 0
            && uv_inet_pton(AF_INET6, ip.c_str(), &addr6) != 0)
            return result_t::from_system(UV_EINVAL)
                .with_syscall("getHostByAddr")
                .with_hostname(ip)
                .arg("hostname", ip);

        if (ac.isSync())
            return result_t::nosync();

        if (!m_channel)
            return cares_error(ARES_EDESTRUCTION, "getHostByAddr", ip);

        return m_channel->reverse(ip, retVal, std::move(ac));
    }

    virtual result_t cancel()
    {
        if (!m_channel)
            return cares_error(ARES_EDESTRUCTION, "cancel", "");
        return m_channel->cancel();
    }

    virtual result_t setLocalAddress(exlib::string ipv4, exlib::string ipv6)
    {
        if (!m_channel)
            return cares_error(ARES_EDESTRUCTION, "setLocalAddress", "");
        return m_channel->setLocalAddress(ipv4, ipv6);
    }

    virtual result_t getServers(std::vector<exlib::string>& retVal)
    {
        if (!m_channel)
            return cares_error(ARES_EDESTRUCTION, "getServers", "");
        return m_channel->getServers(retVal);
    }

    virtual result_t setServers(v8::Local<v8::Array> servers)
    {
        std::vector<exlib::string> list;
        result_t hr = array_to_strings(servers, list);
        if (hr.failed())
            return hr;

        if (!m_channel)
            return cares_error(ARES_EDESTRUCTION, "setServers", "");

        return m_channel->setServers(list);
    }

private:
    result_t query_array(const exlib::string& name, int32_t type, bool ttl,
        const char* syscall, obj_ptr<NArray>& retVal, AsyncHandle ac)
    {
        if (!m_channel)
            return cares_error(ARES_EDESTRUCTION, syscall, name);

        return m_channel->query_array(name, type, ttl, fill_answer, syscall,
            retVal, std::move(ac));
    }

    result_t query_names(const exlib::string& name, int32_t type,
        const char* syscall, std::vector<exlib::string>& retVal, AsyncHandle ac)
    {
        if (!m_channel)
            return cares_error(ARES_EDESTRUCTION, syscall, name);

        return m_channel->query_names(name, type, parse_names, syscall, retVal,
            std::move(ac));
    }

public:
    obj_ptr<CaresChannel> m_channel;
};

static result_t out_of_range(const char* name, const char* range, double value)
{
    return result_t::make(CALL_E_INVALID_DATA)
        .with_type_name(errtype::kRangeError)
        .with_code("ERR_OUT_OF_RANGE")
        .with_format("The value of \"%s\" is out of range. It must be %s. Received %s",
            name, range, errhelp::describe_number(value).c_str())
        .arg(name, value);
}

static bool is_integer(double value)
{
    return std::isfinite(value) && std::floor(value) == value;
}

result_t DnsResolver_base::_new(
    Options_DnsResolver_DnsResolver_options* options,
    obj_ptr<DnsResolver_base>& retVal, v8::Local<v8::Object> This)
{
    double timeout_value = options ? options->timeout.value_or(-1) : -1;
    double tries_value = options ? options->tries.value_or(4) : 4;
    double max_timeout_value = options ? options->maxTimeout.value_or(0) : 0;

    if (!is_integer(timeout_value))
        return out_of_range("options.timeout", "an integer", timeout_value);
    if (!is_integer(tries_value))
        return out_of_range("options.tries", "an integer", tries_value);
    if (!is_integer(max_timeout_value))
        return out_of_range("options.maxTimeout", "an integer",
            max_timeout_value);

    if (timeout_value < -1 || timeout_value > 2147483647)
        return out_of_range("options.timeout",
            ">= -1 && <= 2147483647", timeout_value);
    if (tries_value < 1 || tries_value > 2147483647)
        return out_of_range("options.tries", ">= 1 && <= 2147483647",
            tries_value);
    if (max_timeout_value < 0 || max_timeout_value > 4294967295.0)
        return out_of_range("options.maxTimeout",
            ">= 0 && <= 4294967295", max_timeout_value);

    int32_t timeout = (int32_t)timeout_value;
    int32_t tries = (int32_t)tries_value;
    // Node passes the validated uint32 through Int32Value; c-ares only uses a
    // positive maxtimeout, so values above INT32_MAX behave as "disabled".
    int32_t maxTimeout = (int32_t)(uint32_t)max_timeout_value;

    obj_ptr<CaresChannel> channel;
    result_t hr = CaresChannel::create(timeout, tries, maxTimeout, channel);
    if (hr.failed())
        return hr;

    obj_ptr<DnsResolver> resolver = new DnsResolver();
    resolver->m_channel = channel;
    retVal = resolver;
    return result_t();
}

} // namespace fibjs
