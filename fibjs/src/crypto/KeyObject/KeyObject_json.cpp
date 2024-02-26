/*
 * KeyObject_json.cpp
 *
 *  Created on: Feb 19, 2024
 *      Author: lion
 */

#include "encoding.h"
#include "ifs/crypto.h"
#include "KeyObject.h"
#include "Buffer.h"
#include <crypto/evp.h>
#include <crypto/asn1.h>

namespace fibjs {

static result_t GetJwkValue(Isolate* isolate, v8::Local<v8::Object> obj, const char* key, exlib::string& retVal)
{
    exlib::string s;
    result_t hr = GetConfigValue(isolate, obj, key, s, true);
    if (hr < 0)
        return hr;

    base64Decode(s.c_str(), s.length(), retVal);
    return 0;
}

static result_t GetJwkValue(Isolate* isolate, v8::Local<v8::Object> obj, const char* key, BIGNUM*& n)
{
    exlib::string s;
    result_t hr = GetJwkValue(isolate, obj, key, s);
    if (hr < 0)
        return hr;

    n = BN_bin2bn((const unsigned char*)s.c_str(), s.length(), nullptr);
    return 0;
}

result_t KeyObject::ImportJWKRsaKey(v8::Local<v8::Object> key, KeyType type)
{
    Isolate* isolate = holder();
    result_t hr;

    BignumPointer n;
    BignumPointer e;

    hr = GetJwkValue(isolate, key, "e", e);
    if (hr < 0)
        return hr;

    hr = GetJwkValue(isolate, key, "n", n);
    if (hr < 0)
        return hr;

    RsaPointer rsa = RSA_new();
    if (RSA_set0_key(rsa, n, e, nullptr) != 1)
        return openssl_error();

    n.release();
    e.release();

    if (type == kKeyTypePrivate) {
        BignumPointer d;
        BignumPointer p;
        BignumPointer q;
        BignumPointer dp;
        BignumPointer dq;
        BignumPointer qi;

        hr = GetJwkValue(isolate, key, "d", d);
        if (hr < 0)
            return hr;

        hr = GetJwkValue(isolate, key, "p", p);
        if (hr < 0)
            return hr;

        hr = GetJwkValue(isolate, key, "q", q);
        if (hr < 0)
            return hr;

        hr = GetJwkValue(isolate, key, "dp", dp);
        if (hr < 0)
            return hr;

        hr = GetJwkValue(isolate, key, "dq", dq);
        if (hr < 0)
            return hr;

        hr = GetJwkValue(isolate, key, "qi", qi);
        if (hr < 0)
            return hr;

        if (RSA_set0_key(rsa, nullptr, nullptr, d) != 1)
            return openssl_error();

        if (RSA_set0_factors(rsa, p, q) != 1)
            return openssl_error();

        if (RSA_set0_crt_params(rsa, dp, dq, qi) != 1)
            return openssl_error();

        d.release();
        p.release();
        q.release();
        dp.release();
        dq.release();
        qi.release();
    }

    m_pkey = EVP_PKEY_new();
    if (EVP_PKEY_set1_RSA(m_pkey, rsa.release()) != 1)
        return openssl_error();

    if (type == kKeyTypePrivate)
        if (m_pkey->ameth->pkey_check(m_pkey) != 1)
            return Runtime::setError("Invalid private key");

    m_keyType = type;

    return 0;
}

int GetCurveFromName(const char* name)
{
    int nid = EC_curve_nist2nid(name);
    if (nid == NID_undef)
        nid = OBJ_sn2nid(name);
    return nid;
}

result_t KeyObject::ImportJWKEcKey(v8::Local<v8::Object> key, KeyType type)
{
    Isolate* isolate = holder();
    result_t hr;

    exlib::string crv_name;
    hr = GetConfigValue(isolate, key, "crv", crv_name, true);
    if (hr < 0)
        return hr;

    int nid = GetCurveFromName(crv_name.c_str());
    if (nid == NID_undef)
        return Runtime::setError("Invalid JWK EC key");

    BignumPointer x;
    BignumPointer y;

    hr = GetJwkValue(isolate, key, "x", x);
    if (hr < 0)
        return hr;

    hr = GetJwkValue(isolate, key, "y", y);
    if (hr < 0)
        return hr;

    ECKeyPointer ec = EC_KEY_new_by_curve_name(nid);
    if (EC_KEY_set_public_key_affine_coordinates(ec, x, y) != 1)
        return openssl_error();

    x.release();
    y.release();

    if (type == kKeyTypePrivate) {
        BignumPointer d;
        hr = GetJwkValue(isolate, key, "d", d);
        if (hr < 0)
            return hr;

        if (EC_KEY_set_private_key(ec, d) != 1)
            return openssl_error();

        d.release();
    }

    m_pkey = EVP_PKEY_new();
    if (EVP_PKEY_set1_EC_KEY(m_pkey, ec.release()) != 1)
        return openssl_error();

    if (type == kKeyTypePrivate)
        if (m_pkey->ameth->pkey_check(m_pkey) != 1)
            return Runtime::setError("Invalid private key");

    m_keyType = type;

    return 0;
}

static int GetOKPCurveFromName(const char* name)
{
    int nid;
    if (strcmp(name, "Ed25519") == 0) {
        nid = EVP_PKEY_ED25519;
    } else if (strcmp(name, "Ed448") == 0) {
        nid = EVP_PKEY_ED448;
    } else if (strcmp(name, "X25519") == 0) {
        nid = EVP_PKEY_X25519;
    } else if (strcmp(name, "X448") == 0) {
        nid = EVP_PKEY_X448;
    } else {
        nid = NID_undef;
    }
    return nid;
}

result_t KeyObject::ImportJWKEdKey(v8::Local<v8::Object> key, KeyType type)
{
    Isolate* isolate = holder();
    result_t hr;

    exlib::string crv_name;
    hr = GetConfigValue(isolate, key, "crv", crv_name, true);
    if (hr < 0)
        return hr;

    int nid = GetOKPCurveFromName(crv_name.c_str());
    if (nid == NID_undef)
        return Runtime::setError("Invalid JWK OKP key");

    if (type == kKeyTypePrivate) {
        exlib::string d;
        hr = GetJwkValue(isolate, key, "d", d);
        if (hr < 0)
            return hr;

        m_pkey = EVP_PKEY_new_raw_private_key(nid, nullptr, (const unsigned char*)d.c_str(), d.length());
        if (!m_pkey)
            return openssl_error();
    } else {
        exlib::string x;
        hr = GetJwkValue(isolate, key, "x", x);
        if (hr < 0)
            return hr;

        m_pkey = EVP_PKEY_new_raw_public_key(nid, nullptr, (const unsigned char*)x.c_str(), x.length());
        if (!m_pkey)
            return openssl_error();
    }

    m_keyType = type;

    return 0;
}

result_t KeyObject::ImportJWKAsymmetricKey(v8::Local<v8::Object> jwk, KeyType type)
{
    Isolate* isolate = holder();
    result_t hr;

    exlib::string kty;
    hr = GetConfigValue(isolate, jwk, "kty", kty, true);
    if (hr < 0)
        return hr;

    if (kty == "RSA")
        return ImportJWKRsaKey(jwk, type);
    else if (kty == "EC")
        return ImportJWKEcKey(jwk, type);
    else if (kty == "OKP")
        return ImportJWKEdKey(jwk, type);

    return CALL_E_INVALID_CALL;
}

static v8::Local<v8::String> base64Encode(Isolate* isolate, const char* data, size_t size)
{
    exlib::string oct;
    base64Encode(data, size, true, oct);
    return isolate->NewString(oct);
}

static v8::Local<v8::String> base64Encode(Isolate* isolate, const BIGNUM* n, size_t size = 0)
{
    if (size == 0)
        size = BN_num_bytes(n);
    std::vector<unsigned char> public_exponent(size);
    BN_bn2binpad(n, public_exponent.data(), size);
    return base64Encode(isolate, (const char*)public_exponent.data(), size);
}

result_t KeyObject::ExportJWKSecretKey(v8::Local<v8::Value>& retVal)
{
    Isolate* isolate = holder();
    v8::Local<v8::Context> context = isolate->context();

    v8::Local<v8::Object> jwk = v8::Object::New(isolate->m_isolate);

    jwk->Set(context, isolate->NewString("kty", 3), isolate->NewString("oct", 3)).FromJust();
    jwk->Set(context, isolate->NewString("k", 1),
           base64Encode(isolate, (const char*)m_key.data(), m_key.size()))
        .FromJust();

    retVal = jwk;

    return 0;
}

result_t KeyObject::ExportJWKRsaKey(v8::Local<v8::Value>& retVal)
{
    Isolate* isolate = holder();
    v8::Local<v8::Context> context = isolate->context();

    v8::Local<v8::Object> jwk = v8::Object::New(isolate->m_isolate);
    jwk->Set(context, isolate->NewString("kty", 3), isolate->NewString("RSA", 3)).FromJust();

    const RSA* rsa = EVP_PKEY_get0_RSA(m_pkey);

    const BIGNUM* n;
    const BIGNUM* e;
    const BIGNUM* d;

    RSA_get0_key(rsa, &n, &e, &d);
    jwk->Set(context, isolate->NewString("e", 1), base64Encode(isolate, e)).FromJust();
    jwk->Set(context, isolate->NewString("n", 1), base64Encode(isolate, n)).FromJust();

    if (m_keyType == kKeyTypePrivate) {
        const BIGNUM* p;
        const BIGNUM* q;
        const BIGNUM* dp;
        const BIGNUM* dq;
        const BIGNUM* qi;

        RSA_get0_factors(rsa, &p, &q);
        RSA_get0_crt_params(rsa, &dp, &dq, &qi);
        jwk->Set(context, isolate->NewString("d", 1), base64Encode(isolate, d)).FromJust();
        jwk->Set(context, isolate->NewString("p", 1), base64Encode(isolate, p)).FromJust();
        jwk->Set(context, isolate->NewString("q", 1), base64Encode(isolate, q)).FromJust();
        jwk->Set(context, isolate->NewString("dp", 2), base64Encode(isolate, dp)).FromJust();
        jwk->Set(context, isolate->NewString("dq", 2), base64Encode(isolate, dq)).FromJust();
        jwk->Set(context, isolate->NewString("qi", 2), base64Encode(isolate, qi)).FromJust();
    }

    retVal = jwk;

    return 0;
}

static const char* GetNameFromCurve(int nid)
{
    const char* name;
    name = EC_curve_nid2nist(nid);
    if (!name)
        name = OBJ_nid2sn(nid);
    return name;
}

result_t KeyObject::ExportJWKEcKey(v8::Local<v8::Value>& retVal)
{
    Isolate* isolate = holder();
    v8::Local<v8::Context> context = isolate->context();

    const EC_KEY* ec = EVP_PKEY_get0_EC_KEY(m_pkey);
    const EC_GROUP* group = EC_KEY_get0_group(ec);

    v8::Local<v8::Object> jwk = v8::Object::New(isolate->m_isolate);
    jwk->Set(context, isolate->NewString("kty", 3), isolate->NewString("EC", 2)).FromJust();
    jwk->Set(context, isolate->NewString("crv", 3), isolate->NewString(GetNameFromCurve(EC_GROUP_get_curve_name(group)))).FromJust();

    const EC_POINT* pub = EC_KEY_get0_public_key(ec);

    int degree_bits = EC_GROUP_get_degree(group);
    int degree_bytes = (degree_bits / CHAR_BIT) + (7 + (degree_bits % CHAR_BIT)) / 8;

    BignumPointer x(BN_new());
    BignumPointer y(BN_new());

    EC_POINT_get_affine_coordinates(group, pub, x, y, nullptr);

    jwk->Set(context, isolate->NewString("x", 1), base64Encode(isolate, x, degree_bytes)).FromJust();
    jwk->Set(context, isolate->NewString("y", 1), base64Encode(isolate, y, degree_bytes)).FromJust();

    if (m_keyType == kKeyTypePrivate) {
        const BIGNUM* d = EC_KEY_get0_private_key(ec);
        jwk->Set(context, isolate->NewString("d", 1), base64Encode(isolate, d, degree_bytes)).FromJust();
    }

    retVal = jwk;

    return 0;
}

static const char* GetOKPNameFromCurve(int nid)
{
    const char* name = nullptr;
    switch (nid) {
    case EVP_PKEY_ED25519:
        name = "Ed25519";
        break;
    case EVP_PKEY_ED448:
        name = "Ed448";
        break;
    case EVP_PKEY_X25519:
        name = "X25519";
        break;
    case EVP_PKEY_X448:
        name = "X448";
        break;
    }
    return name;
}

result_t KeyObject::ExportJWKEdKey(v8::Local<v8::Value>& retVal)
{
    Isolate* isolate = holder();
    v8::Local<v8::Context> context = isolate->context();

    v8::Local<v8::Object> jwk = v8::Object::New(isolate->m_isolate);
    jwk->Set(context, isolate->NewString("kty", 3), isolate->NewString("OKP", 3)).FromJust();
    jwk->Set(context, isolate->NewString("crv", 3), isolate->NewString(GetOKPNameFromCurve(EVP_PKEY_id(m_pkey)))).FromJust();

    size_t len = 0;
    EVP_PKEY_get_raw_public_key(m_pkey, nullptr, &len);

    std::vector<unsigned char> key(len);
    EVP_PKEY_get_raw_public_key(m_pkey, key.data(), &len);
    jwk->Set(context, isolate->NewString("x", 1), base64Encode(isolate, (const char*)key.data(), len)).FromJust();

    if (m_keyType == kKeyTypePrivate) {
        len = key.size();
        EVP_PKEY_get_raw_private_key(m_pkey, key.data(), &len);
        jwk->Set(context, isolate->NewString("d", 1), base64Encode(isolate, (const char*)key.data(), len)).FromJust();
    }

    retVal = jwk;

    return 0;
}

result_t KeyObject::export_json(v8::Local<v8::Value>& retVal)
{
    if (m_keyType == kKeyTypeSecret)
        return ExportJWKSecretKey(retVal);
    else
        switch (EVP_PKEY_id(m_pkey)) {
        case EVP_PKEY_RSA:
            return ExportJWKRsaKey(retVal);
        case EVP_PKEY_SM2:
        case EVP_PKEY_EC:
            return ExportJWKEcKey(retVal);
        case EVP_PKEY_ED25519:
        case EVP_PKEY_ED448:
        case EVP_PKEY_X25519:
        case EVP_PKEY_X448:
            return ExportJWKEdKey(retVal);
        }

    return Runtime::setError("Not supported key type");
}

}
