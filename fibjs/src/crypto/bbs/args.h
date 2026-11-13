/*
 * args.h
 *
 *  Created on: Ayg 5, 2024
 *      Author: lion
 */

#pragma once

#include "object.h"
#include "crypto_util.h"
#include "ifs/crypto.h"
#include "KeyObject.h"

#include "defs.h"

namespace fibjs {

static result_t bbs_get_args(const KeyObjectOptions& opts, exlib::string suite,
    Buffer_base* header, Buffer_base* proof_header, bool priv, AsyncHandle& ac)
{
    result_t hr;

    ac.ctxv().resize(4);

    obj_ptr<KeyObject_base> key_;
    hr = create_key_object(nullptr, opts,
        priv ? KeyObject::kKeyTypePrivate : KeyObject::kKeyTypePublic, key_);
    if (hr.failed())
        return hr;

    if (EVP_PKEY_get_id(key_.As<KeyObject>()->pkey()) != EVP_PKEY_BLS12_381_G2)
        return result_t::make(CALL_E_INVALID_DATA).with_type_name(errtype::kTypeError).with_code("ERR_INVALID_ARG_TYPE").with_message("crypto: key must be a BLS12-381 G2 key");

    ac.ctxv()[0] = key_;

    if (suite == "Bls12381Sha256")
        ac.ctxv()[1] = Bls12381Sha256;
    else if (suite == "Bls12381Shake256")
        ac.ctxv()[1] = Bls12381Shake256;
    else
        return result_t::make(CALL_E_INVALID_DATA).with_type_name(errtype::kTypeError).with_code("ERR_INVALID_ARG_VALUE").with_message("crypto: suite must be 'Bls12381Sha256' or 'Bls12381Shake256'");

    ac.ctxv()[2] = header;
    ac.ctxv()[3] = proof_header;

    return result_t::nosync();
}

static result_t bbs_get_args(BbsOptions* options, bool priv, AsyncHandle& ac)
{
    if (!options)
        return result_t(CALL_E_PARAMNOTOPTIONAL);

    KeyObjectOptions opts;
    result_t hr = extract_key_options(*options, opts);
    if (hr.failed())
        return hr;

    return bbs_get_args(opts, options->suite.value_or("Bls12381Sha256"),
        options->header.has_value() ? options->header.value().get() : nullptr,
        options->proof_header.has_value() ? options->proof_header.value().get() : nullptr,
        priv, ac);
}

static result_t bbs_get_args(exlib::string key, bool priv, AsyncHandle& ac)
{
    // the key parser reads a string key as utf8, exactly like Buffer.from(key)
    KeyObjectOptions opts;
    opts.key = Variant(key);
    opts.format = "raw";
    opts.namedCurve = "Bls12381G2";

    return bbs_get_args(opts, "Bls12381Sha256", nullptr, nullptr, priv, ac);
}

static result_t bbs_get_args(Buffer_base* key, bool priv, AsyncHandle& ac)
{
    KeyObjectOptions opts;
    opts.key = Variant((obj_base*)key);
    opts.format = "raw";
    opts.namedCurve = "Bls12381G2";

    return bbs_get_args(opts, "Bls12381Sha256", nullptr, nullptr, priv, ac);
}

static result_t bbs_get_args(KeyObject_base* key, bool priv, AsyncHandle& ac)
{
    KeyObjectOptions opts;
    opts.key = Variant((obj_base*)key);

    return bbs_get_args(opts, "Bls12381Sha256", nullptr, nullptr, priv, ac);
}

// the key of a BBS operation is a union argument (Buffer|KeyObject|String):
// the alternative that was given is parsed by the matching overload above
template <typename... Ts>
static result_t bbs_get_args(std::variant<Ts...>& key, bool priv, AsyncHandle& ac)
{
    result_t hr;

    std::visit([&hr, priv, &ac](auto& val) {
        if (hr.ok())
            hr = bbs_get_args(val, priv, ac);
    }, key);

    return hr;
}

// The key alternatives are all owned values now, so the preparation can run
// in either phase; the slot prepared by the sync phase still wins.
template <typename... Ts>
static result_t bbs_prepare_key(std::variant<Ts...>& key, bool priv, AsyncHandle& ac)
{
    if (ac.isSync())
        return bbs_get_args(key, priv, ac);

    if (ac.ctxv().size() > 0 && ac.ctxv()[0].object() != NULL)
        return result_t();

    result_t hr = bbs_get_args(key, priv, ac);
    return hr.is_nosync() ? result_t() : hr;
}

static result_t bbs_prepare_key(BbsOptions* options, bool priv, AsyncHandle& ac)
{
    if (ac.isSync())
        return bbs_get_args(options, priv, ac);

    if (ac.ctxv().size() > 0 && ac.ctxv()[0].object() != NULL)
        return result_t();

    result_t hr = bbs_get_args(options, priv, ac);
    return hr.is_nosync() ? result_t() : hr;
}

}
