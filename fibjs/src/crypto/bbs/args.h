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

static result_t bbs_get_args(v8::Local<v8::Object> opts, bool priv, AsyncHandle& ac)
{
    result_t hr;

    ac.ctxv().resize(4);

    obj_ptr<KeyObject_base> key_;
    hr = priv ? crypto_base::createPrivateKey(opts, key_) : crypto_base::createPublicKey(opts, key_);
    if (hr.failed())
        return hr;

    if (EVP_PKEY_get_id(key_.As<KeyObject>()->pkey()) != EVP_PKEY_BLS12_381_G2)
        return result_t::make(CALL_E_EXCEPTION).with_message("crypto: key must be a BLS12-381 G2 key");

    ac.ctxv()[0] = key_;

    exlib::string suite = "Bls12381Sha256";
    hr = GetConfigValue(opts, "suite", suite);
    if (hr.failed_other_than(CALL_E_PARAMNOTOPTIONAL))
        return hr;
    if (suite == "Bls12381Sha256")
        ac.ctxv()[1] = Bls12381Sha256;
    else if (suite == "Bls12381Shake256")
        ac.ctxv()[1] = Bls12381Shake256;
    else
        return result_t::make(CALL_E_EXCEPTION).with_message("crypto: suite must be 'Bls12381Sha256' or 'Bls12381Shake256'");

    obj_ptr<Buffer_base> header;
    hr = GetConfigValue(opts, "header", header);
    if (hr.failed_other_than(CALL_E_PARAMNOTOPTIONAL))
        return hr;
    ac.ctxv()[2] = header;

    obj_ptr<Buffer_base> proof_header;
    hr = GetConfigValue(opts, "proof_header", proof_header);
    if (hr.failed_other_than(CALL_E_PARAMNOTOPTIONAL))
        return hr;
    ac.ctxv()[3] = proof_header;

    return result_t::nosync();
}

static result_t bbs_get_args(exlib::string key, bool priv, AsyncHandle& ac)
{
    Isolate* isolate = ac.isolate();
    v8::Local<v8::Context> context = isolate->context();
    v8::Local<v8::Object> key_ = v8::Object::New(isolate->m_isolate);

    // the key parser reads a string key as utf8, exactly like Buffer.from(key)
    key_->Set(context, isolate->NewString("key"), isolate->NewString(key)).IsJust();
    key_->Set(context, isolate->NewString("format"), isolate->NewString("raw")).IsJust();
    key_->Set(context, isolate->NewString("namedCurve"), isolate->NewString("Bls12381G2")).IsJust();

    return bbs_get_args(key_, priv, ac);
}

static result_t bbs_get_args(Buffer_base* key, bool priv, AsyncHandle& ac)
{
    Isolate* isolate = ac.isolate();
    v8::Local<v8::Context> context = isolate->context();
    v8::Local<v8::Object> key_ = v8::Object::New(isolate->m_isolate);

    key_->Set(context, isolate->NewString("key"), key->wrap(isolate)).IsJust();
    key_->Set(context, isolate->NewString("format"), isolate->NewString("raw")).IsJust();
    key_->Set(context, isolate->NewString("namedCurve"), isolate->NewString("Bls12381G2")).IsJust();

    return bbs_get_args(key_, priv, ac);
}

static result_t bbs_get_args(KeyObject_base* key, bool priv, AsyncHandle& ac)
{
    result_t hr;

    ac.ctxv().resize(4);

    obj_ptr<KeyObject> key_ = static_cast<KeyObject*>(key);
    if ((key_->type() != (priv ? KeyObject::kKeyTypePrivate : KeyObject::kKeyTypePublic))
        || EVP_PKEY_get_id(key_->pkey()) != EVP_PKEY_BLS12_381_G2)
        return result_t::make(CALL_E_EXCEPTION).with_message("crypto: key must be a BLS12-381 G2 private key");

    ac.ctxv()[0] = key;

    ac.ctxv()[1] = Bls12381Sha256;

    ac.ctxv()[2] = (Buffer_base*)nullptr;
    ac.ctxv()[3] = (Buffer_base*)nullptr;

    return result_t::nosync();
}

// the key of a BBS operation is a union argument (Buffer|KeyObject|Object|String):
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

// The async-aware entry: Buffer/String/Object build a JS options object (the
// Buffer form also wraps the buffer), so they stay in the sync phase; the
// KeyObject alternative is pure C++ and is resolved in the async phase, so a
// cc_ caller can pass it directly. No V8 is touched in the async branch.
template <typename... Ts>
static result_t bbs_prepare_key(std::variant<Ts...>& key, bool priv, AsyncHandle& ac)
{
    if (ac.isSync()) {
        if (std::holds_alternative<obj_ptr<KeyObject_base>>(key))
            return result_t::nosync(); // pure C++: left to the async phase

        return bbs_get_args(key, priv, ac);
    }

    // async / cc_: the slot prepared by the sync phase comes first
    if (ac.ctxv().size() > 0 && ac.ctxv()[0].object() != NULL)
        return result_t();

    // otherwise the KeyObject alternative, parsed without touching V8
    if (std::holds_alternative<obj_ptr<KeyObject_base>>(key)) {
        result_t hr = bbs_get_args(std::get<obj_ptr<KeyObject_base>>(key), priv, ac);
        return hr.is_nosync() ? result_t() : hr;
    }

    return result_t::make(CALL_E_EXCEPTION).with_message("the key union was not prepared: this entry requires the synchronous phase.").with_result(result_t::type_mismatch());
}

}
