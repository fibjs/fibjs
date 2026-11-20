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

class Options_DnsResolver_DnsResolver_options;
class Options_DnsResolver_resolve4_options;
class Options_DnsResolver_resolve6_options;

class DnsResolver_base : public object_base {
    DECLARE_CLASS(DnsResolver_base);

public:
    // DnsResolver_base
    static result_t _new(Options_DnsResolver_DnsResolver_options* options, obj_ptr<DnsResolver_base>& retVal, v8::Local<v8::Object> This = v8::Local<v8::Object>());
    virtual result_t resolve(exlib::string name, exlib::string rrtype, Variant& retVal, AsyncHandle ac) = 0;
    virtual result_t resolve4(exlib::string name, Options_DnsResolver_resolve4_options* options, obj_ptr<NArray>& retVal, AsyncHandle ac) = 0;
    virtual result_t resolve6(exlib::string name, Options_DnsResolver_resolve6_options* options, obj_ptr<NArray>& retVal, AsyncHandle ac) = 0;
    virtual result_t resolveAny(exlib::string name, obj_ptr<NArray>& retVal, AsyncHandle ac) = 0;
    virtual result_t resolveCaa(exlib::string name, obj_ptr<NArray>& retVal, AsyncHandle ac) = 0;
    virtual result_t resolveCname(exlib::string name, std::vector<exlib::string>& retVal, AsyncHandle ac) = 0;
    virtual result_t resolveMx(exlib::string name, obj_ptr<NArray>& retVal, AsyncHandle ac) = 0;
    virtual result_t resolveNaptr(exlib::string name, obj_ptr<NArray>& retVal, AsyncHandle ac) = 0;
    virtual result_t resolveNs(exlib::string name, std::vector<exlib::string>& retVal, AsyncHandle ac) = 0;
    virtual result_t resolvePtr(exlib::string name, std::vector<exlib::string>& retVal, AsyncHandle ac) = 0;
    virtual result_t resolveSoa(exlib::string name, Variant& retVal, AsyncHandle ac) = 0;
    virtual result_t resolveSrv(exlib::string name, obj_ptr<NArray>& retVal, AsyncHandle ac) = 0;
    virtual result_t resolveTlsa(exlib::string name, obj_ptr<NArray>& retVal, AsyncHandle ac) = 0;
    virtual result_t resolveTxt(exlib::string name, obj_ptr<NArray>& retVal, AsyncHandle ac) = 0;
    virtual result_t reverse(exlib::string ip, std::vector<exlib::string>& retVal, AsyncHandle ac) = 0;
    virtual result_t cancel() = 0;
    virtual result_t setLocalAddress(exlib::string ipv4, exlib::string ipv6) = 0;
    virtual result_t getServers(std::vector<exlib::string>& retVal) = 0;
    virtual result_t setServers(std::vector<exlib::string>& servers) = 0;

public:
    static void __new(const v8::FunctionCallbackInfo<v8::Value>& args);
    static result_t load(v8::Local<v8::Value> v, obj_ptr<DnsResolver_base>& retVal);

public:
    static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_resolve(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_resolve4(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_resolve6(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_resolveAny(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_resolveCaa(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_resolveCname(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_resolveMx(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_resolveNaptr(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_resolveNs(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_resolvePtr(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_resolveSoa(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_resolveSrv(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_resolveTlsa(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_resolveTxt(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_reverse(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_cancel(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_setLocalAddress(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_getServers(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_setServers(const v8::FunctionCallbackInfo<v8::Value>& args);

public:
    ASYNC_MEMBERVALUE3(DnsResolver_base, resolve, exlib::string, exlib::string, Variant);
    ASYNC_MEMBERVALUE3(DnsResolver_base, resolve4, exlib::string, obj_ptr<Options_DnsResolver_resolve4_options>, obj_ptr<NArray>);
    ASYNC_MEMBERVALUE3(DnsResolver_base, resolve6, exlib::string, obj_ptr<Options_DnsResolver_resolve6_options>, obj_ptr<NArray>);
    ASYNC_MEMBERVALUE2(DnsResolver_base, resolveAny, exlib::string, obj_ptr<NArray>);
    ASYNC_MEMBERVALUE2(DnsResolver_base, resolveCaa, exlib::string, obj_ptr<NArray>);
    ASYNC_MEMBERVALUE2(DnsResolver_base, resolveCname, exlib::string, std::vector<exlib::string>);
    ASYNC_MEMBERVALUE2(DnsResolver_base, resolveMx, exlib::string, obj_ptr<NArray>);
    ASYNC_MEMBERVALUE2(DnsResolver_base, resolveNaptr, exlib::string, obj_ptr<NArray>);
    ASYNC_MEMBERVALUE2(DnsResolver_base, resolveNs, exlib::string, std::vector<exlib::string>);
    ASYNC_MEMBERVALUE2(DnsResolver_base, resolvePtr, exlib::string, std::vector<exlib::string>);
    ASYNC_MEMBERVALUE2(DnsResolver_base, resolveSoa, exlib::string, Variant);
    ASYNC_MEMBERVALUE2(DnsResolver_base, resolveSrv, exlib::string, obj_ptr<NArray>);
    ASYNC_MEMBERVALUE2(DnsResolver_base, resolveTlsa, exlib::string, obj_ptr<NArray>);
    ASYNC_MEMBERVALUE2(DnsResolver_base, resolveTxt, exlib::string, obj_ptr<NArray>);
    ASYNC_MEMBERVALUE2(DnsResolver_base, reverse, exlib::string, std::vector<exlib::string>);
};
}

namespace fibjs {
class Options_DnsResolver_DnsResolver_options : public obj_base {
public:
    LOAD_OPTIONS(Options_DnsResolver_DnsResolver_options, (timeout)(tries)(maxTimeout));

public:
    std::optional<double> timeout = -1;
    std::optional<double> tries = 4;
    std::optional<double> maxTimeout = 0;
};

class Options_DnsResolver_resolve4_options : public obj_base {
public:
    LOAD_OPTIONS(Options_DnsResolver_resolve4_options, (ttl));

public:
    std::optional<bool> ttl = false;
};

class Options_DnsResolver_resolve6_options : public obj_base {
public:
    LOAD_OPTIONS(Options_DnsResolver_resolve6_options, (ttl));

public:
    std::optional<bool> ttl = false;
};

inline ClassInfo& DnsResolver_base::class_info()
{
    static ClassData::ClassMethod s_method[] = {
        { "resolve", s_resolve, false, ClassData::ASYNC_ASYNC },
        { "resolve4", s_resolve4, false, ClassData::ASYNC_ASYNC },
        { "resolve6", s_resolve6, false, ClassData::ASYNC_ASYNC },
        { "resolveAny", s_resolveAny, false, ClassData::ASYNC_ASYNC },
        { "resolveCaa", s_resolveCaa, false, ClassData::ASYNC_ASYNC },
        { "resolveCname", s_resolveCname, false, ClassData::ASYNC_ASYNC },
        { "resolveMx", s_resolveMx, false, ClassData::ASYNC_ASYNC },
        { "resolveNaptr", s_resolveNaptr, false, ClassData::ASYNC_ASYNC },
        { "resolveNs", s_resolveNs, false, ClassData::ASYNC_ASYNC },
        { "resolvePtr", s_resolvePtr, false, ClassData::ASYNC_ASYNC },
        { "resolveSoa", s_resolveSoa, false, ClassData::ASYNC_ASYNC },
        { "resolveSrv", s_resolveSrv, false, ClassData::ASYNC_ASYNC },
        { "resolveTlsa", s_resolveTlsa, false, ClassData::ASYNC_ASYNC },
        { "resolveTxt", s_resolveTxt, false, ClassData::ASYNC_ASYNC },
        { "reverse", s_reverse, false, ClassData::ASYNC_ASYNC },
        { "cancel", s_cancel, false, ClassData::ASYNC_SYNC },
        { "setLocalAddress", s_setLocalAddress, false, ClassData::ASYNC_SYNC },
        { "getServers", s_getServers, false, ClassData::ASYNC_SYNC },
        { "setServers", s_setServers, false, ClassData::ASYNC_SYNC }
    };

    static ClassData s_cd = {
        "DnsResolver", false, s__new, NULL,
        ARRAYSIZE(s_method), s_method, 0, NULL, 0, NULL, 0, NULL, NULL, NULL,
        &object_base::class_info(),
        true
    };

    static ClassInfo s_ci(s_cd);
    return s_ci;
}

inline void DnsResolver_base::s__new(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    CONSTRUCT_INIT();
    __new(args);
}

inline void DnsResolver_base::__new(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<DnsResolver_base> vr;

    CONSTRUCT_ENTER();

    METHOD_OVER(1, 0);

    OPT_ARG_NAMED(obj_ptr<Options_DnsResolver_DnsResolver_options>, 0, new Options_DnsResolver_DnsResolver_options(), "options");

    hr = _new((Options_DnsResolver_DnsResolver_options*)v0, vr, args.This());

    CONSTRUCT_RETURN();
}

inline result_t DnsResolver_base::load(v8::Local<v8::Value> v, obj_ptr<DnsResolver_base>& retVal)
{
    obj_ptr<DnsResolver_base> vr;

    LOAD_ENTER();

    METHOD_OVER(1, 0);

    OPT_ARG_NAMED(obj_ptr<Options_DnsResolver_DnsResolver_options>, 0, new Options_DnsResolver_DnsResolver_options(), "options");

    hr = _new((Options_DnsResolver_DnsResolver_options*)v0, vr, args.This());

    LOAD_RETURN();
}

inline void DnsResolver_base::s_resolve(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    Variant vr;

    ASYNC_METHOD_INSTANCE(DnsResolver_base);
    ASYNC_METHOD_ENTER("DnsResolver.resolve");

    METHOD_OVER_NAMED(2, 1, "name");

    ARG_NAMED(exlib::string, 0, "name");
    OPT_ARG_NAMED(exlib::string, 1, "A", "rrtype");

    if (!cb.IsEmpty())
        hr = pInst->acb_resolve(v0, v1, cb, args);
    else
        hr = pInst->ac_resolve(v0, v1, vr);

    ASYNC_METHOD_RETURN();
}

inline void DnsResolver_base::s_resolve4(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<NArray> vr;

    ASYNC_METHOD_INSTANCE(DnsResolver_base);
    ASYNC_METHOD_ENTER("DnsResolver.resolve4");

    METHOD_OVER_NAMED(2, 1, "name");

    ARG_NAMED(exlib::string, 0, "name");
    OPT_ARG_NAMED(obj_ptr<Options_DnsResolver_resolve4_options>, 1, new Options_DnsResolver_resolve4_options(), "options");

    if (!cb.IsEmpty())
        hr = pInst->acb_resolve4(v0, v1, cb, args);
    else
        hr = pInst->ac_resolve4(v0, v1, vr);

    ASYNC_METHOD_RETURN();
}

inline void DnsResolver_base::s_resolve6(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<NArray> vr;

    ASYNC_METHOD_INSTANCE(DnsResolver_base);
    ASYNC_METHOD_ENTER("DnsResolver.resolve6");

    METHOD_OVER_NAMED(2, 1, "name");

    ARG_NAMED(exlib::string, 0, "name");
    OPT_ARG_NAMED(obj_ptr<Options_DnsResolver_resolve6_options>, 1, new Options_DnsResolver_resolve6_options(), "options");

    if (!cb.IsEmpty())
        hr = pInst->acb_resolve6(v0, v1, cb, args);
    else
        hr = pInst->ac_resolve6(v0, v1, vr);

    ASYNC_METHOD_RETURN();
}

inline void DnsResolver_base::s_resolveAny(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<NArray> vr;

    ASYNC_METHOD_INSTANCE(DnsResolver_base);
    ASYNC_METHOD_ENTER("DnsResolver.resolveAny");

    METHOD_OVER_NAMED(1, 1, "name");

    ARG_NAMED(exlib::string, 0, "name");

    if (!cb.IsEmpty())
        hr = pInst->acb_resolveAny(v0, cb, args);
    else
        hr = pInst->ac_resolveAny(v0, vr);

    ASYNC_METHOD_RETURN();
}

inline void DnsResolver_base::s_resolveCaa(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<NArray> vr;

    ASYNC_METHOD_INSTANCE(DnsResolver_base);
    ASYNC_METHOD_ENTER("DnsResolver.resolveCaa");

    METHOD_OVER_NAMED(1, 1, "name");

    ARG_NAMED(exlib::string, 0, "name");

    if (!cb.IsEmpty())
        hr = pInst->acb_resolveCaa(v0, cb, args);
    else
        hr = pInst->ac_resolveCaa(v0, vr);

    ASYNC_METHOD_RETURN();
}

inline void DnsResolver_base::s_resolveCname(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    std::vector<exlib::string> vr;

    ASYNC_METHOD_INSTANCE(DnsResolver_base);
    ASYNC_METHOD_ENTER("DnsResolver.resolveCname");

    METHOD_OVER_NAMED(1, 1, "name");

    ARG_NAMED(exlib::string, 0, "name");

    if (!cb.IsEmpty())
        hr = pInst->acb_resolveCname(v0, cb, args);
    else
        hr = pInst->ac_resolveCname(v0, vr);

    ASYNC_METHOD_RETURN();
}

inline void DnsResolver_base::s_resolveMx(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<NArray> vr;

    ASYNC_METHOD_INSTANCE(DnsResolver_base);
    ASYNC_METHOD_ENTER("DnsResolver.resolveMx");

    METHOD_OVER_NAMED(1, 1, "name");

    ARG_NAMED(exlib::string, 0, "name");

    if (!cb.IsEmpty())
        hr = pInst->acb_resolveMx(v0, cb, args);
    else
        hr = pInst->ac_resolveMx(v0, vr);

    ASYNC_METHOD_RETURN();
}

inline void DnsResolver_base::s_resolveNaptr(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<NArray> vr;

    ASYNC_METHOD_INSTANCE(DnsResolver_base);
    ASYNC_METHOD_ENTER("DnsResolver.resolveNaptr");

    METHOD_OVER_NAMED(1, 1, "name");

    ARG_NAMED(exlib::string, 0, "name");

    if (!cb.IsEmpty())
        hr = pInst->acb_resolveNaptr(v0, cb, args);
    else
        hr = pInst->ac_resolveNaptr(v0, vr);

    ASYNC_METHOD_RETURN();
}

inline void DnsResolver_base::s_resolveNs(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    std::vector<exlib::string> vr;

    ASYNC_METHOD_INSTANCE(DnsResolver_base);
    ASYNC_METHOD_ENTER("DnsResolver.resolveNs");

    METHOD_OVER_NAMED(1, 1, "name");

    ARG_NAMED(exlib::string, 0, "name");

    if (!cb.IsEmpty())
        hr = pInst->acb_resolveNs(v0, cb, args);
    else
        hr = pInst->ac_resolveNs(v0, vr);

    ASYNC_METHOD_RETURN();
}

inline void DnsResolver_base::s_resolvePtr(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    std::vector<exlib::string> vr;

    ASYNC_METHOD_INSTANCE(DnsResolver_base);
    ASYNC_METHOD_ENTER("DnsResolver.resolvePtr");

    METHOD_OVER_NAMED(1, 1, "name");

    ARG_NAMED(exlib::string, 0, "name");

    if (!cb.IsEmpty())
        hr = pInst->acb_resolvePtr(v0, cb, args);
    else
        hr = pInst->ac_resolvePtr(v0, vr);

    ASYNC_METHOD_RETURN();
}

inline void DnsResolver_base::s_resolveSoa(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    Variant vr;

    ASYNC_METHOD_INSTANCE(DnsResolver_base);
    ASYNC_METHOD_ENTER("DnsResolver.resolveSoa");

    METHOD_OVER_NAMED(1, 1, "name");

    ARG_NAMED(exlib::string, 0, "name");

    if (!cb.IsEmpty())
        hr = pInst->acb_resolveSoa(v0, cb, args);
    else
        hr = pInst->ac_resolveSoa(v0, vr);

    ASYNC_METHOD_RETURN();
}

inline void DnsResolver_base::s_resolveSrv(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<NArray> vr;

    ASYNC_METHOD_INSTANCE(DnsResolver_base);
    ASYNC_METHOD_ENTER("DnsResolver.resolveSrv");

    METHOD_OVER_NAMED(1, 1, "name");

    ARG_NAMED(exlib::string, 0, "name");

    if (!cb.IsEmpty())
        hr = pInst->acb_resolveSrv(v0, cb, args);
    else
        hr = pInst->ac_resolveSrv(v0, vr);

    ASYNC_METHOD_RETURN();
}

inline void DnsResolver_base::s_resolveTlsa(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<NArray> vr;

    ASYNC_METHOD_INSTANCE(DnsResolver_base);
    ASYNC_METHOD_ENTER("DnsResolver.resolveTlsa");

    METHOD_OVER_NAMED(1, 1, "name");

    ARG_NAMED(exlib::string, 0, "name");

    if (!cb.IsEmpty())
        hr = pInst->acb_resolveTlsa(v0, cb, args);
    else
        hr = pInst->ac_resolveTlsa(v0, vr);

    ASYNC_METHOD_RETURN();
}

inline void DnsResolver_base::s_resolveTxt(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<NArray> vr;

    ASYNC_METHOD_INSTANCE(DnsResolver_base);
    ASYNC_METHOD_ENTER("DnsResolver.resolveTxt");

    METHOD_OVER_NAMED(1, 1, "name");

    ARG_NAMED(exlib::string, 0, "name");

    if (!cb.IsEmpty())
        hr = pInst->acb_resolveTxt(v0, cb, args);
    else
        hr = pInst->ac_resolveTxt(v0, vr);

    ASYNC_METHOD_RETURN();
}

inline void DnsResolver_base::s_reverse(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    std::vector<exlib::string> vr;

    ASYNC_METHOD_INSTANCE(DnsResolver_base);
    ASYNC_METHOD_ENTER("DnsResolver.reverse");

    METHOD_OVER_NAMED(1, 1, "ip");

    ARG_NAMED(exlib::string, 0, "ip");

    if (!cb.IsEmpty())
        hr = pInst->acb_reverse(v0, cb, args);
    else
        hr = pInst->ac_reverse(v0, vr);

    ASYNC_METHOD_RETURN();
}

inline void DnsResolver_base::s_cancel(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_INSTANCE(DnsResolver_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->cancel();

    METHOD_VOID();
}

inline void DnsResolver_base::s_setLocalAddress(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_INSTANCE(DnsResolver_base);
    METHOD_ENTER();

    METHOD_OVER_NAMED(2, 1, "ipv4");

    ARG_NAMED(exlib::string, 0, "ipv4");
    OPT_ARG_NAMED(exlib::string, 1, "", "ipv6");

    hr = pInst->setLocalAddress(v0, v1);

    METHOD_VOID();
}

inline void DnsResolver_base::s_getServers(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    std::vector<exlib::string> vr;

    METHOD_INSTANCE(DnsResolver_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->getServers(vr);

    METHOD_RETURN();
}

inline void DnsResolver_base::s_setServers(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_INSTANCE(DnsResolver_base);
    METHOD_ENTER();

    METHOD_OVER_NAMED(1, 1, "servers");

    ARG_NAMED(std::vector<exlib::string>, 0, "servers");

    hr = pInst->setServers(v0);

    METHOD_VOID();
}
}
