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

result_t subtle_base::exportKey(exlib::string format, CryptoKey_base* key, Variant& retVal, AsyncHandle ac)
{
    if (ac.isSync())
        return CALL_E_NOSYNC;

    CryptoKey* ckey = (CryptoKey*)key;
    KeyObject::KeyType keyType = ckey->type();

    if (!ckey->m_extractable)
        return Runtime::setError("Key is not extractable");

    if (format == "jwk") {
        result_t hr = ckey->m_key->export_json(retVal);
        if (hr < 0)
            return hr;

        // For HMAC keys, add the 'alg' field to the JWK
        if (ckey->m_key_type == kKeyNameHMAC && keyType == KeyObject::kKeyTypeSecret) {
            // Get the hash algorithm from the key's algorithm
            obj_ptr<NObject> algorithm = ckey->m_algorithm;
            if (algorithm) {
                Variant hashValue;
                hr = algorithm->get("hash", hashValue);

                exlib::string hash;
                // Extract hash string from either object {name: "SHA-256"} or direct string "SHA-256"
                if (hr == 0 && hashValue.type() == Variant::VT_Object) {
                    obj_ptr<NObject> hashObj = (NObject*)hashValue.object();
                    if (hashObj) {
                        Variant nameValue;
                        if (hashObj->get("name", nameValue) == 0 && nameValue.type() == Variant::VT_String) {
                            hash = nameValue.string();
                        }
                    }
                } else if (hr == 0 && hashValue.type() == Variant::VT_String) {
                    hash = hashValue.string();
                }

                // Convert hash to JWK alg field
                if (!hash.empty()) {
                    exlib::string alg;
                    if (hash == "SHA-256")
                        alg = "HS256";
                    else if (hash == "SHA-384")
                        alg = "HS384";
                    else if (hash == "SHA-512")
                        alg = "HS512";

                    if (!alg.empty() && retVal.type() == Variant::VT_Object) {
                        obj_ptr<NObject> jwk = (NObject*)retVal.object();
                        if (jwk) {
                            jwk->add("alg", alg);
                        }
                    }
                }
            }
        }
        return 0;
    }

    exlib::string type;
    if (format != "raw") {
        type = format;
        format = "der";
    }

    switch (keyType) {
    case KeyObject::kKeyTypePublic:
        return ckey->m_key->ExportPublicKey(format, type, retVal, true); // Use BackingStore for webcrypto
    case KeyObject::kKeyTypePrivate:
        return ckey->m_key->ExportPrivateKey(format, type, "", nullptr, retVal, true); // Use BackingStore for webcrypto
    case KeyObject::kKeyTypeSecret:
        if (format == "raw") {
            // Export secret key as raw bytes
            KeyObject* key_obj = (KeyObject*)ckey->m_key.get();
            std::shared_ptr<v8::BackingStore> store = NewBackingStore(key_obj->length());
            if (key_obj->length() > 0 && store->Data() && key_obj->data()) {
                memcpy(store->Data(), key_obj->data(), key_obj->length());
            }
            retVal = store;
            return 0;
        }
        return Runtime::setError("WebCrypto: HMAC keys only support 'raw' and 'jwk' export formats, got '%s'.", format.c_str());
    }

    return Runtime::setError("Invalid key type: %d.", keyType);
}

result_t subtle_base::importKey(exlib::string format, v8::Local<v8::Value> keyData, Union_importKey_algorithm algorithm,
    bool extractable, v8::Local<v8::Array> usages, obj_ptr<CryptoKey_base>& retVal, AsyncHandle ac)
{
    if (ac.isSync()) {
        Isolate* isolate = ac.isolate();
        result_t hr;

        ac.ctxv().resize(3);

        v8::Local<v8::Object> algObj;

        if (std::holds_alternative<v8::Local<v8::Object>>(algorithm))
            algObj = std::get<v8::Local<v8::Object>>(algorithm);
        else {
            v8::Local<v8::Context> context = isolate->context();
            algObj = v8::Object::New(isolate->m_isolate);
            algObj->Set(context, isolate->NewString("name"), isolate->NewString(std::get<exlib::string>(algorithm))).IsJust();
        }

        obj_ptr<CryptoKey> key = new CryptoKey();
        hr = key->get_param(algObj, extractable, usages);
        if (hr < 0)
            return hr;

        ac.ctxv()[0] = key;

        if (format == "jwk") {
            v8::Local<v8::Object> o;
            hr = GetArgumentValue(isolate, keyData, o, true);
            if (hr < 0)
                return hr;

            obj_ptr<NObject> jwk = new NObject();
            jwk->add(o);
            ac.ctxv()[1] = jwk;
        } else {
            obj_ptr<Buffer_base> buf;
            hr = GetArgumentValue(isolate, keyData, buf);
            if (hr < 0)
                return hr;
            ac.ctxv()[2] = buf;
        }

        return CALL_E_NOSYNC;
    }

    // the algorithm object and the key data were read in the sync phase
    result_t ctx_hr = ac.ctx(format == "jwk" ? 1 : 2);
    if (ctx_hr < 0)
        return ctx_hr;

    result_t hr;
    obj_ptr<CryptoKey> key = (CryptoKey*)ac.ctxv()[0].object();
    if (key == NULL)
        return Runtime::setError("WebCrypto: the key parameters were not read");

    key->m_key = new KeyObject();
    if (format == "jwk") {
        obj_ptr<NObject> jwk = (NObject*)ac.ctxv()[1].object();
        if (jwk == NULL)
            return Runtime::setError("WebCrypto: the key data was not read");
        hr = key->m_key->ImportJWKKey(jwk, KeyObject::kKeyTypeUnknown);
        if (hr < 0)
            return hr;

        retVal = key;

        return key->check_import_param();
    } else {
        obj_ptr<Buffer_base> buf = (Buffer_base*)ac.ctxv()[2].object();
        if (buf == NULL)
            return Runtime::setError("WebCrypto: the key data was not read");

        if (format == "pkcs8")
            hr = key->m_key->ParsePrivateKey("der", format, "", nullptr, buf);
        else if (format == "spki")
            hr = key->m_key->ParsePublicKey("der", format, "", nullptr, buf);
        else if (key->m_key_type == kKeyNameECDSA && format == "raw")
            hr = key->m_key->ParsePublicKey(format, "", key->m_algorithm->get("namedCurve").string(), nullptr, buf);
        else if (key->m_key_type == kKeyNameECDH && format == "raw")
            hr = key->m_key->ParsePublicKey(format, "", key->m_algorithm->get("namedCurve").string(), nullptr, buf);
        else if (key->m_key_type == kKeyNameEd25519 && format == "raw")
            hr = key->m_key->ParsePublicKey(format, "", "Ed25519", nullptr, buf);
        else if (key->m_key_type == kKeyNameHMAC && format == "raw") {
            Buffer* b = Buffer::Cast(buf);
            hr = ((KeyObject*)key->m_key.get())->createSecretKey(b->data(), b->length());
        } else
            return Runtime::setError("WebCrypto: unknown key format: " + format);

        if (hr < 0)
            return hr;

        retVal = key;

        return key->check_import_param();
    }

    return Runtime::setError("WebCrypto: unknown key format: " + format);
}


}
