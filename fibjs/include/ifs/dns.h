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

class Options_dns_lookup_options;

class dns_base : public object_base {
    DECLARE_CLASS(dns_base);

public:
    // dns_base
    static result_t resolve(exlib::string name, std::vector<exlib::string>& retVal, AsyncHandle ac);
    static result_t lookup(exlib::string name, Options_dns_lookup_options* options, Variant& retVal, AsyncHandle ac);

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

public:
    ASYNC_STATICVALUE2(dns_base, resolve, exlib::string, std::vector<exlib::string>);
    ASYNC_STATICVALUE3(dns_base, lookup, exlib::string, obj_ptr<Options_dns_lookup_options>, Variant);
};
}

namespace fibjs {
class Options_dns_lookup_options : public obj_base {
public:
    LOAD_OPTIONS(Options_dns_lookup_options, (family)(all));

public:
    std::optional<std::variant<exlib::string, int32_t>> family = 0;
    std::optional<bool> all = false;
};

inline ClassInfo& dns_base::class_info()
{
    static ClassData::ClassMethod s_method[] = {
        { "resolve", s_static_resolve, true, ClassData::ASYNC_ASYNC },
        { "lookup", s_static_lookup, true, ClassData::ASYNC_ASYNC }
    };

    static ClassData s_cd = {
        "dns", true, s__new, NULL,
        ARRAYSIZE(s_method), s_method, 0, NULL, 0, NULL, 0, NULL, NULL, NULL,
        &object_base::class_info(),
        true
    };

    static ClassInfo s_ci(s_cd);
    return s_ci;
}

inline void dns_base::s_static_resolve(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    std::vector<exlib::string> vr;

    ASYNC_METHOD_ENTER("dns.resolve");

    METHOD_OVER_NAMED(1, 1, "name");

    ARG_NAMED(exlib::string, 0, "name");

    if (!cb.IsEmpty())
        hr = acb_resolve(v0, cb, args);
    else
        hr = ac_resolve(v0, vr);

    ASYNC_METHOD_RETURN();
}

inline void dns_base::s_static_lookup(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    Variant vr;

    ASYNC_METHOD_ENTER("dns.lookup");

    METHOD_OVER_NAMED(2, 1, "name");

    ARG_NAMED(exlib::string, 0, "name");
    OPT_ARG_NAMED(obj_ptr<Options_dns_lookup_options>, 1, new Options_dns_lookup_options(), "options");

    if (!cb.IsEmpty())
        hr = acb_lookup(v0, v1, cb, args);
    else
        hr = ac_lookup(v0, v1, vr);

    ASYNC_METHOD_RETURN();
}
}
