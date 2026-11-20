/***************************************************************************
 *                                                                         *
 *   This file was automatically generated using idlc.js                   *
 *   PLEASE DO NOT EDIT!!!!                                                *
 *                                                                         *
 ***************************************************************************/

#pragma once

/**
 @author Leo Hoo <lion@9465.net>
 */

#include "../object.h"
#include <boost/preprocessor.hpp>

namespace fibjs {

class DnsResolver_base;
class Options_dns_lookup_options;
class Options_dns_resolve4_options;
class Options_dns_resolve6_options;

class dns_base : public object_base {
    DECLARE_CLASS(dns_base);

public:
    class LookupType : public NType {
    public:
        virtual void to_value(Isolate* isolate, v8::Local<v8::Object>& retVal)
        {
            v8::Local<v8::Context> context = isolate->context();
            retVal->Set(context, isolate->NewString("address"), GetReturnValue(isolate, address)).Check();
            retVal->Set(context, isolate->NewString("family"), GetReturnValue(isolate, family)).Check();
        }

        virtual void to_args(Isolate* isolate, std::vector<v8::Local<v8::Value>>& args)
        {
            args.push_back(GetReturnValue(isolate, address));
            args.push_back(GetReturnValue(isolate, family));
        }

    public:
        Variant address;
        int32_t family;
    };
    class LookupServiceType : public NType {
    public:
        virtual void to_value(Isolate* isolate, v8::Local<v8::Object>& retVal)
        {
            v8::Local<v8::Context> context = isolate->context();
            retVal->Set(context, isolate->NewString("hostname"), GetReturnValue(isolate, hostname)).Check();
            retVal->Set(context, isolate->NewString("service"), GetReturnValue(isolate, service)).Check();
        }

        virtual void to_args(Isolate* isolate, std::vector<v8::Local<v8::Value>>& args)
        {
            args.push_back(GetReturnValue(isolate, hostname));
            args.push_back(GetReturnValue(isolate, service));
        }

    public:
        exlib::string hostname;
        exlib::string service;
    };

public:
    // dns_base
    static result_t resolve(exlib::string name, exlib::string rrtype, Variant& retVal, AsyncHandle ac);
    static result_t lookup(exlib::string hostname, Options_dns_lookup_options* options, obj_ptr<LookupType>& retVal, AsyncHandle ac);
    static result_t lookup(exlib::string hostname, int32_t family, obj_ptr<LookupType>& retVal, AsyncHandle ac);
    static result_t resolve4(exlib::string name, Options_dns_resolve4_options* options, obj_ptr<NArray>& retVal, AsyncHandle ac);
    static result_t resolve6(exlib::string name, Options_dns_resolve6_options* options, obj_ptr<NArray>& retVal, AsyncHandle ac);
    static result_t resolveAny(exlib::string name, obj_ptr<NArray>& retVal, AsyncHandle ac);
    static result_t resolveCaa(exlib::string name, obj_ptr<NArray>& retVal, AsyncHandle ac);
    static result_t resolveCname(exlib::string name, std::vector<exlib::string>& retVal, AsyncHandle ac);
    static result_t resolveMx(exlib::string name, obj_ptr<NArray>& retVal, AsyncHandle ac);
    static result_t resolveNaptr(exlib::string name, obj_ptr<NArray>& retVal, AsyncHandle ac);
    static result_t resolveNs(exlib::string name, std::vector<exlib::string>& retVal, AsyncHandle ac);
    static result_t resolvePtr(exlib::string name, std::vector<exlib::string>& retVal, AsyncHandle ac);
    static result_t resolveSoa(exlib::string name, Variant& retVal, AsyncHandle ac);
    static result_t resolveSrv(exlib::string name, obj_ptr<NArray>& retVal, AsyncHandle ac);
    static result_t resolveTlsa(exlib::string name, obj_ptr<NArray>& retVal, AsyncHandle ac);
    static result_t resolveTxt(exlib::string name, obj_ptr<NArray>& retVal, AsyncHandle ac);
    static result_t reverse(exlib::string ip, std::vector<exlib::string>& retVal, AsyncHandle ac);
    static result_t lookupService(exlib::string address, double port, obj_ptr<LookupServiceType>& retVal, AsyncHandle ac);
    static result_t getServers(std::vector<exlib::string>& retVal);
    static result_t setServers(std::vector<exlib::string>& servers);
    static result_t getDefaultResultOrder(exlib::string& retVal);
    static result_t setDefaultResultOrder(exlib::string order);
    static result_t get_ADDRCONFIG(int32_t& retVal);
    static result_t get_ALL(int32_t& retVal);
    static result_t get_V4MAPPED(int32_t& retVal);

public:
    static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        CONSTRUCT_INIT();

        ThrowTypeError("not a constructor");
    }

    static result_t load(v8::Local<v8::Value> v, obj_ptr<dns_base>& retVal)
    { return result_t::type_mismatch(); }

public:
    static void s_static_resolve(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_lookup(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_resolve4(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_resolve6(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_resolveAny(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_resolveCaa(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_resolveCname(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_resolveMx(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_resolveNaptr(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_resolveNs(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_resolvePtr(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_resolveSoa(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_resolveSrv(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_resolveTlsa(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_resolveTxt(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_reverse(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_lookupService(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_getServers(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_setServers(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_getDefaultResultOrder(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_setDefaultResultOrder(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_get_ADDRCONFIG(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_get_ALL(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_get_V4MAPPED(const v8::FunctionCallbackInfo<v8::Value>& args);

public:
    ASYNC_STATICVALUE3(dns_base, resolve, exlib::string, exlib::string, Variant);
    ASYNC_STATICVALUE3(dns_base, lookup, exlib::string, obj_ptr<Options_dns_lookup_options>, obj_ptr<LookupType>);
    ASYNC_STATICVALUE3(dns_base, lookup, exlib::string, int32_t, obj_ptr<LookupType>);
    ASYNC_STATICVALUE3(dns_base, resolve4, exlib::string, obj_ptr<Options_dns_resolve4_options>, obj_ptr<NArray>);
    ASYNC_STATICVALUE3(dns_base, resolve6, exlib::string, obj_ptr<Options_dns_resolve6_options>, obj_ptr<NArray>);
    ASYNC_STATICVALUE2(dns_base, resolveAny, exlib::string, obj_ptr<NArray>);
    ASYNC_STATICVALUE2(dns_base, resolveCaa, exlib::string, obj_ptr<NArray>);
    ASYNC_STATICVALUE2(dns_base, resolveCname, exlib::string, std::vector<exlib::string>);
    ASYNC_STATICVALUE2(dns_base, resolveMx, exlib::string, obj_ptr<NArray>);
    ASYNC_STATICVALUE2(dns_base, resolveNaptr, exlib::string, obj_ptr<NArray>);
    ASYNC_STATICVALUE2(dns_base, resolveNs, exlib::string, std::vector<exlib::string>);
    ASYNC_STATICVALUE2(dns_base, resolvePtr, exlib::string, std::vector<exlib::string>);
    ASYNC_STATICVALUE2(dns_base, resolveSoa, exlib::string, Variant);
    ASYNC_STATICVALUE2(dns_base, resolveSrv, exlib::string, obj_ptr<NArray>);
    ASYNC_STATICVALUE2(dns_base, resolveTlsa, exlib::string, obj_ptr<NArray>);
    ASYNC_STATICVALUE2(dns_base, resolveTxt, exlib::string, obj_ptr<NArray>);
    ASYNC_STATICVALUE2(dns_base, reverse, exlib::string, std::vector<exlib::string>);
    ASYNC_STATICVALUE3(dns_base, lookupService, exlib::string, double, obj_ptr<LookupServiceType>);
};
}

#include "ifs/DnsResolver.h"

namespace fibjs {
class Options_dns_lookup_options : public obj_base {
public:
    LOAD_OPTIONS(Options_dns_lookup_options, (family)(all)(hints)(verbatim)(order));

public:
    std::optional<std::variant<exlib::string, int32_t>> family = 0;
    std::optional<bool> all = false;
    std::optional<int32_t> hints = 0;
    std::optional<bool> verbatim;
    std::optional<exlib::string> order;
};

class Options_dns_resolve4_options : public obj_base {
public:
    LOAD_OPTIONS(Options_dns_resolve4_options, (ttl));

public:
    std::optional<bool> ttl = false;
};

class Options_dns_resolve6_options : public obj_base {
public:
    LOAD_OPTIONS(Options_dns_resolve6_options, (ttl));

public:
    std::optional<bool> ttl = false;
};

inline ClassInfo& dns_base::class_info()
{
    static ClassData::ClassMethod s_method[] = {
        { "resolve", s_static_resolve, true, ClassData::ASYNC_ASYNC },
        { "lookup", s_static_lookup, true, ClassData::ASYNC_ASYNC },
        { "resolve4", s_static_resolve4, true, ClassData::ASYNC_ASYNC },
        { "resolve6", s_static_resolve6, true, ClassData::ASYNC_ASYNC },
        { "resolveAny", s_static_resolveAny, true, ClassData::ASYNC_ASYNC },
        { "resolveCaa", s_static_resolveCaa, true, ClassData::ASYNC_ASYNC },
        { "resolveCname", s_static_resolveCname, true, ClassData::ASYNC_ASYNC },
        { "resolveMx", s_static_resolveMx, true, ClassData::ASYNC_ASYNC },
        { "resolveNaptr", s_static_resolveNaptr, true, ClassData::ASYNC_ASYNC },
        { "resolveNs", s_static_resolveNs, true, ClassData::ASYNC_ASYNC },
        { "resolvePtr", s_static_resolvePtr, true, ClassData::ASYNC_ASYNC },
        { "resolveSoa", s_static_resolveSoa, true, ClassData::ASYNC_ASYNC },
        { "resolveSrv", s_static_resolveSrv, true, ClassData::ASYNC_ASYNC },
        { "resolveTlsa", s_static_resolveTlsa, true, ClassData::ASYNC_ASYNC },
        { "resolveTxt", s_static_resolveTxt, true, ClassData::ASYNC_ASYNC },
        { "reverse", s_static_reverse, true, ClassData::ASYNC_ASYNC },
        { "lookupService", s_static_lookupService, true, ClassData::ASYNC_ASYNC },
        { "getServers", s_static_getServers, true, ClassData::ASYNC_SYNC },
        { "setServers", s_static_setServers, true, ClassData::ASYNC_SYNC },
        { "getDefaultResultOrder", s_static_getDefaultResultOrder, true, ClassData::ASYNC_SYNC },
        { "setDefaultResultOrder", s_static_setDefaultResultOrder, true, ClassData::ASYNC_SYNC }
    };

    static ClassData::ClassObject s_object[] = {
        { "Resolver", DnsResolver_base::class_info }
    };

    static ClassData::ClassProperty s_property[] = {
        { "ADDRCONFIG", s_static_get_ADDRCONFIG, block_set, true },
        { "ALL", s_static_get_ALL, block_set, true },
        { "V4MAPPED", s_static_get_V4MAPPED, block_set, true }
    };

    static ClassData::ClassConst s_const[] = {
        { "NODATA", ClassData::CONST_String, { .stringValue = "ENODATA" } },
        { "FORMERR", ClassData::CONST_String, { .stringValue = "EFORMERR" } },
        { "SERVFAIL", ClassData::CONST_String, { .stringValue = "ESERVFAIL" } },
        { "NOTFOUND", ClassData::CONST_String, { .stringValue = "ENOTFOUND" } },
        { "NOTIMP", ClassData::CONST_String, { .stringValue = "ENOTIMP" } },
        { "REFUSED", ClassData::CONST_String, { .stringValue = "EREFUSED" } },
        { "BADQUERY", ClassData::CONST_String, { .stringValue = "EBADQUERY" } },
        { "BADNAME", ClassData::CONST_String, { .stringValue = "EBADNAME" } },
        { "BADFAMILY", ClassData::CONST_String, { .stringValue = "EBADFAMILY" } },
        { "BADRESP", ClassData::CONST_String, { .stringValue = "EBADRESP" } },
        { "CONNREFUSED", ClassData::CONST_String, { .stringValue = "ECONNREFUSED" } },
        { "TIMEOUT", ClassData::CONST_String, { .stringValue = "ETIMEOUT" } },
        { "EOF", ClassData::CONST_String, { .stringValue = "EOF" } },
        { "FILE", ClassData::CONST_String, { .stringValue = "EFILE" } },
        { "NOMEM", ClassData::CONST_String, { .stringValue = "ENOMEM" } },
        { "DESTRUCTION", ClassData::CONST_String, { .stringValue = "EDESTRUCTION" } },
        { "BADSTR", ClassData::CONST_String, { .stringValue = "EBADSTR" } },
        { "BADFLAGS", ClassData::CONST_String, { .stringValue = "EBADFLAGS" } },
        { "NONAME", ClassData::CONST_String, { .stringValue = "ENONAME" } },
        { "BADHINTS", ClassData::CONST_String, { .stringValue = "EBADHINTS" } },
        { "NOTINITIALIZED", ClassData::CONST_String, { .stringValue = "ENOTINITIALIZED" } },
        { "LOADIPHLPAPI", ClassData::CONST_String, { .stringValue = "ELOADIPHLPAPI" } },
        { "ADDRGETNETWORKPARAMS", ClassData::CONST_String, { .stringValue = "EADDRGETNETWORKPARAMS" } },
        { "CANCELLED", ClassData::CONST_String, { .stringValue = "ECANCELLED" } }
    };

    static ClassData s_cd = {
        "dns", true, s__new, NULL,
        ARRAYSIZE(s_method), s_method, ARRAYSIZE(s_object), s_object, ARRAYSIZE(s_property), s_property, ARRAYSIZE(s_const), s_const, NULL, NULL,
        &object_base::class_info(),
        true
    };

    static ClassInfo s_ci(s_cd);
    return s_ci;
}

inline void dns_base::s_static_resolve(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    Variant vr;

    ASYNC_METHOD_ENTER("dns.resolve");

    METHOD_OVER_NAMED(2, 1, "name");

    ARG_NAMED(exlib::string, 0, "name");
    OPT_ARG_NAMED(exlib::string, 1, "A", "rrtype");

    if (!cb.IsEmpty())
        hr = acb_resolve(v0, v1, cb, args);
    else
        hr = ac_resolve(v0, v1, vr);

    ASYNC_METHOD_RETURN();
}

inline void dns_base::s_static_lookup(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<LookupType> vr;

    ASYNC_METHOD_ENTER("dns.lookup");

    METHOD_OVER_NAMED(2, 1, "hostname");

    ARG_NAMED(exlib::string, 0, "hostname");
    OPT_ARG_NAMED(obj_ptr<Options_dns_lookup_options>, 1, new Options_dns_lookup_options(), "options");

    if (!cb.IsEmpty())
        hr = acb_lookup(v0, v1, cb, args);
    else
        hr = ac_lookup(v0, v1, vr);

    METHOD_OVER_NAMED(2, 2, "family");

    ARG_NAMED(exlib::string, 0, "hostname");
    ARG_NAMED(int32_t, 1, "family");

    if (!cb.IsEmpty())
        hr = acb_lookup(v0, v1, cb, args);
    else
        hr = ac_lookup(v0, v1, vr);

    ASYNC_METHOD_RETURN();
}

inline void dns_base::s_static_resolve4(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<NArray> vr;

    ASYNC_METHOD_ENTER("dns.resolve4");

    METHOD_OVER_NAMED(2, 1, "name");

    ARG_NAMED(exlib::string, 0, "name");
    OPT_ARG_NAMED(obj_ptr<Options_dns_resolve4_options>, 1, new Options_dns_resolve4_options(), "options");

    if (!cb.IsEmpty())
        hr = acb_resolve4(v0, v1, cb, args);
    else
        hr = ac_resolve4(v0, v1, vr);

    ASYNC_METHOD_RETURN();
}

inline void dns_base::s_static_resolve6(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<NArray> vr;

    ASYNC_METHOD_ENTER("dns.resolve6");

    METHOD_OVER_NAMED(2, 1, "name");

    ARG_NAMED(exlib::string, 0, "name");
    OPT_ARG_NAMED(obj_ptr<Options_dns_resolve6_options>, 1, new Options_dns_resolve6_options(), "options");

    if (!cb.IsEmpty())
        hr = acb_resolve6(v0, v1, cb, args);
    else
        hr = ac_resolve6(v0, v1, vr);

    ASYNC_METHOD_RETURN();
}

inline void dns_base::s_static_resolveAny(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<NArray> vr;

    ASYNC_METHOD_ENTER("dns.resolveAny");

    METHOD_OVER_NAMED(1, 1, "name");

    ARG_NAMED(exlib::string, 0, "name");

    if (!cb.IsEmpty())
        hr = acb_resolveAny(v0, cb, args);
    else
        hr = ac_resolveAny(v0, vr);

    ASYNC_METHOD_RETURN();
}

inline void dns_base::s_static_resolveCaa(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<NArray> vr;

    ASYNC_METHOD_ENTER("dns.resolveCaa");

    METHOD_OVER_NAMED(1, 1, "name");

    ARG_NAMED(exlib::string, 0, "name");

    if (!cb.IsEmpty())
        hr = acb_resolveCaa(v0, cb, args);
    else
        hr = ac_resolveCaa(v0, vr);

    ASYNC_METHOD_RETURN();
}

inline void dns_base::s_static_resolveCname(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    std::vector<exlib::string> vr;

    ASYNC_METHOD_ENTER("dns.resolveCname");

    METHOD_OVER_NAMED(1, 1, "name");

    ARG_NAMED(exlib::string, 0, "name");

    if (!cb.IsEmpty())
        hr = acb_resolveCname(v0, cb, args);
    else
        hr = ac_resolveCname(v0, vr);

    ASYNC_METHOD_RETURN();
}

inline void dns_base::s_static_resolveMx(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<NArray> vr;

    ASYNC_METHOD_ENTER("dns.resolveMx");

    METHOD_OVER_NAMED(1, 1, "name");

    ARG_NAMED(exlib::string, 0, "name");

    if (!cb.IsEmpty())
        hr = acb_resolveMx(v0, cb, args);
    else
        hr = ac_resolveMx(v0, vr);

    ASYNC_METHOD_RETURN();
}

inline void dns_base::s_static_resolveNaptr(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<NArray> vr;

    ASYNC_METHOD_ENTER("dns.resolveNaptr");

    METHOD_OVER_NAMED(1, 1, "name");

    ARG_NAMED(exlib::string, 0, "name");

    if (!cb.IsEmpty())
        hr = acb_resolveNaptr(v0, cb, args);
    else
        hr = ac_resolveNaptr(v0, vr);

    ASYNC_METHOD_RETURN();
}

inline void dns_base::s_static_resolveNs(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    std::vector<exlib::string> vr;

    ASYNC_METHOD_ENTER("dns.resolveNs");

    METHOD_OVER_NAMED(1, 1, "name");

    ARG_NAMED(exlib::string, 0, "name");

    if (!cb.IsEmpty())
        hr = acb_resolveNs(v0, cb, args);
    else
        hr = ac_resolveNs(v0, vr);

    ASYNC_METHOD_RETURN();
}

inline void dns_base::s_static_resolvePtr(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    std::vector<exlib::string> vr;

    ASYNC_METHOD_ENTER("dns.resolvePtr");

    METHOD_OVER_NAMED(1, 1, "name");

    ARG_NAMED(exlib::string, 0, "name");

    if (!cb.IsEmpty())
        hr = acb_resolvePtr(v0, cb, args);
    else
        hr = ac_resolvePtr(v0, vr);

    ASYNC_METHOD_RETURN();
}

inline void dns_base::s_static_resolveSoa(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    Variant vr;

    ASYNC_METHOD_ENTER("dns.resolveSoa");

    METHOD_OVER_NAMED(1, 1, "name");

    ARG_NAMED(exlib::string, 0, "name");

    if (!cb.IsEmpty())
        hr = acb_resolveSoa(v0, cb, args);
    else
        hr = ac_resolveSoa(v0, vr);

    ASYNC_METHOD_RETURN();
}

inline void dns_base::s_static_resolveSrv(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<NArray> vr;

    ASYNC_METHOD_ENTER("dns.resolveSrv");

    METHOD_OVER_NAMED(1, 1, "name");

    ARG_NAMED(exlib::string, 0, "name");

    if (!cb.IsEmpty())
        hr = acb_resolveSrv(v0, cb, args);
    else
        hr = ac_resolveSrv(v0, vr);

    ASYNC_METHOD_RETURN();
}

inline void dns_base::s_static_resolveTlsa(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<NArray> vr;

    ASYNC_METHOD_ENTER("dns.resolveTlsa");

    METHOD_OVER_NAMED(1, 1, "name");

    ARG_NAMED(exlib::string, 0, "name");

    if (!cb.IsEmpty())
        hr = acb_resolveTlsa(v0, cb, args);
    else
        hr = ac_resolveTlsa(v0, vr);

    ASYNC_METHOD_RETURN();
}

inline void dns_base::s_static_resolveTxt(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<NArray> vr;

    ASYNC_METHOD_ENTER("dns.resolveTxt");

    METHOD_OVER_NAMED(1, 1, "name");

    ARG_NAMED(exlib::string, 0, "name");

    if (!cb.IsEmpty())
        hr = acb_resolveTxt(v0, cb, args);
    else
        hr = ac_resolveTxt(v0, vr);

    ASYNC_METHOD_RETURN();
}

inline void dns_base::s_static_reverse(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    std::vector<exlib::string> vr;

    ASYNC_METHOD_ENTER("dns.reverse");

    METHOD_OVER_NAMED(1, 1, "ip");

    ARG_NAMED(exlib::string, 0, "ip");

    if (!cb.IsEmpty())
        hr = acb_reverse(v0, cb, args);
    else
        hr = ac_reverse(v0, vr);

    ASYNC_METHOD_RETURN();
}

inline void dns_base::s_static_lookupService(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<LookupServiceType> vr;

    ASYNC_METHOD_ENTER("dns.lookupService");

    METHOD_OVER_NAMED(2, 2, "port");

    ARG_NAMED(exlib::string, 0, "address");
    ARG_NAMED(double, 1, "port");

    if (!cb.IsEmpty())
        hr = acb_lookupService(v0, v1, cb, args);
    else
        hr = ac_lookupService(v0, v1, vr);

    ASYNC_METHOD_RETURN();
}

inline void dns_base::s_static_getServers(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    std::vector<exlib::string> vr;

    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = getServers(vr);

    METHOD_RETURN();
}

inline void dns_base::s_static_setServers(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_ENTER();

    METHOD_OVER_NAMED(1, 1, "servers");

    ARG_NAMED(std::vector<exlib::string>, 0, "servers");

    hr = setServers(v0);

    METHOD_VOID();
}

inline void dns_base::s_static_getDefaultResultOrder(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = getDefaultResultOrder(vr);

    METHOD_RETURN();
}

inline void dns_base::s_static_setDefaultResultOrder(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_ENTER();

    METHOD_OVER_NAMED(1, 1, "order");

    ARG_NAMED(exlib::string, 0, "order");

    hr = setDefaultResultOrder(v0);

    METHOD_VOID();
}

inline void dns_base::s_static_get_ADDRCONFIG(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    int32_t vr;

    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = get_ADDRCONFIG(vr);

    METHOD_RETURN();
}

inline void dns_base::s_static_get_ALL(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    int32_t vr;

    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = get_ALL(vr);

    METHOD_RETURN();
}

inline void dns_base::s_static_get_V4MAPPED(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    int32_t vr;

    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = get_V4MAPPED(vr);

    METHOD_RETURN();
}
}
