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

static result_t bbs_get_args(v8::Local<v8::Array> messages, v8::Local<v8::Array> index, v8::Local<v8::Object> opts, bool priv, AsyncEvent* ac)
{
    Isolate* isolate = ac->isolate();
    result_t hr;

    ac->m_ctx.resize(6);

    obj_ptr<KeyObject_base> key_;
    hr = priv ? crypto_base::createPrivateKey(opts, key_) : crypto_base::createPublicKey(opts, key_);
    if (hr != 0)
        return hr;

    if (EVP_PKEY_get_id(key_.As<KeyObject>()->pkey()) != EVP_PKEY_BLS12_381_G2)
        return Runtime::setError("crypto: key must be a BLS12-381 G2 key");

    ac->m_ctx[0] = key_;

    exlib::string suite = "Bls12381Sha256";
    hr = GetConfigValue(isolate, opts, "suite", suite);
    if (hr < 0 && hr != CALL_E_PARAMNOTOPTIONAL)
        return hr;
    if (suite == "Bls12381Sha256")
        ac->m_ctx[1] = Bls12381Sha256;
    else if (suite == "Bls12381Shake256")
        ac->m_ctx[1] = Bls12381Shake256;
    else
        return Runtime::setError("crypto: suite must be 'Bls12381Sha256' or 'Bls12381Shake256'");

    obj_ptr<NArray> _msg = new NArray();
    hr = _msg->append_array<obj_ptr<Buffer_base>>(messages);
    if (hr < 0)
        return hr;

    ac->m_ctx[2] = _msg;

    obj_ptr<NArray> _idx = new NArray();
    if (!index.IsEmpty()) {
        hr = _idx->append_array<int32_t>(index);
        if (hr < 0)
            return hr;
    }

    ac->m_ctx[3] = _idx;

    obj_ptr<Buffer_base> header;
    hr = GetConfigValue(isolate, opts, "header", header);
    if (hr < 0 && hr != CALL_E_PARAMNOTOPTIONAL)
        return hr;
    ac->m_ctx[4] = header;

    obj_ptr<Buffer_base> proof_header;
    hr = GetConfigValue(isolate, opts, "proof_header", proof_header);
    if (hr < 0 && hr != CALL_E_PARAMNOTOPTIONAL)
        return hr;
    ac->m_ctx[5] = proof_header;

    return CALL_E_NOSYNC;
}

static result_t bbs_get_args(v8::Local<v8::Array> messages, v8::Local<v8::Array> index, Buffer_base* key, bool priv, AsyncEvent* ac)
{
    Isolate* isolate = ac->isolate();
    v8::Local<v8::Context> context = isolate->context();
    v8::Local<v8::Object> key_ = v8::Object::New(isolate->m_isolate);

    key_->Set(context, isolate->NewString("key"), key->wrap(isolate)).IsJust();
    key_->Set(context, isolate->NewString("format"), isolate->NewString("raw")).IsJust();
    key_->Set(context, isolate->NewString("namedCurve"), isolate->NewString("Bls12381G2")).IsJust();

    return bbs_get_args(messages, index, key_, priv, ac);
}

static result_t bbs_get_args(v8::Local<v8::Array> messages, v8::Local<v8::Array> index, KeyObject_base* key, bool priv, AsyncEvent* ac)
{
    result_t hr;

    ac->m_ctx.resize(6);

    obj_ptr<KeyObject> key_ = static_cast<KeyObject*>(key);
    if ((key_->type() != (priv ? KeyObject::kKeyTypePrivate : KeyObject::kKeyTypePublic))
        || EVP_PKEY_get_id(key_->pkey()) != EVP_PKEY_BLS12_381_G2)
        return Runtime::setError("crypto: key must be a BLS12-381 G2 private key");

    ac->m_ctx[0] = key;

    ac->m_ctx[1] = Bls12381Sha256;

    obj_ptr<NArray> _msg = new NArray();
    hr = _msg->append_array<obj_ptr<Buffer_base>>(messages);
    if (hr < 0)
        return hr;

    ac->m_ctx[2] = _msg;

    obj_ptr<NArray> _idx = new NArray();
    if (!index.IsEmpty()) {
        hr = _idx->append_array<int32_t>(index);
        if (hr < 0)
            return hr;
    }

    ac->m_ctx[3] = _idx;

    ac->m_ctx[4] = (Buffer_base*)nullptr;
    ac->m_ctx[5] = (Buffer_base*)nullptr;

    return CALL_E_NOSYNC;
}

}
