/*
 * export.cpp
 *
 *  Created on: Aug 25, 2014
 *      Author: lion
 */

#include "object.h"
#include "ifs/subtle.h"
#include "CryptoKey.h"

namespace fibjs {

result_t subtle_base::exportKey(exlib::string format, CryptoKey_base* key, Variant& retVal, AsyncEvent* ac)
{
    if (ac->isSync())
        return CALL_E_NOSYNC;

    CryptoKey* ckey = (CryptoKey*)key;
    KeyObject::KeyType keyType = ckey->type();

    if (!ckey->m_extractable)
        return Runtime::setError("Key is not extractable");

    if (format == "jwk")
        return ckey->m_key->export_json(retVal);

    exlib::string type;
    if (format != "raw") {
        type = format;
        format = "der";
    }

    switch (keyType) {
    case KeyObject::kKeyTypePublic:
        return ckey->m_key->ExportPublicKey(format, type, retVal);
    case KeyObject::kKeyTypePrivate:
        return ckey->m_key->ExportPrivateKey(format, type, "", nullptr, retVal);
    }

    return Runtime::setError("Invalid key type");
}

result_t subtle_base::importKey(exlib::string format, v8::Local<v8::Value> keyData, v8::Local<v8::Object> algorithm,
    bool extractable, v8::Local<v8::Array> usages, obj_ptr<CryptoKey_base>& retVal, AsyncEvent* ac)
{
    if (ac->isSync()) {
        Isolate* isolate = ac->isolate();
        result_t hr;

        ac->m_ctx.resize(2);

        obj_ptr<CryptoKey> key = new CryptoKey();
        hr = key->get_param(algorithm, extractable, usages);
        if (hr < 0)
            return hr;

        ac->m_ctx[0] = key;

        if (format == "jwk") {
            v8::Local<v8::Object> o;
            hr = GetArgumentValue(isolate, keyData, o, true);
            if (hr < 0)
                return hr;

            obj_ptr<NObject> jwk = new NObject();
            jwk->add(o);
            ac->m_ctx[1] = jwk;
        } else {
            obj_ptr<Buffer_base> buf;
            hr = GetArgumentValue(isolate, keyData, buf, true);
            if (hr < 0)
                return hr;
            ac->m_ctx[1] = buf;
        }

        return CALL_E_NOSYNC;
    }

    result_t hr;
    obj_ptr<CryptoKey> key = (CryptoKey*)ac->m_ctx[0].object();

    key->m_key = new KeyObject();
    if (format == "jwk") {
        obj_ptr<NObject> jwk = (NObject*)ac->m_ctx[1].object();
        hr = key->m_key->ImportJWKAsymmetricKey(jwk, KeyObject::kKeyTypeUnknown);
        if (hr < 0)
            return hr;

        retVal = key;

        return key->check_import_param();
    } else {
        obj_ptr<Buffer_base> buf = (Buffer_base*)ac->m_ctx[1].object();

        if (format == "pkcs8")
            hr = key->m_key->ParsePrivateKey("der", format, "", nullptr, buf);
        else if (format == "spki")
            hr = key->m_key->ParsePublicKey("der", format, "", nullptr, buf);
        else if (key->m_key_type == kKeyNameECDSA && format == "raw")
            hr = key->m_key->ParsePublicKey(format, "", key->m_algorithm->get("namedCurve").string(), nullptr, buf);
        else
            return Runtime::setError("WebCrypto: unknown key format: " + format);

        if (hr < 0)
            return hr;

        retVal = key;

        return key->check_import_param();
    }

    return Runtime::setError("WebCrypto: unknown key format: " + format);
}
}
