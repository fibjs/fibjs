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

static result_t GetConfigValue(NObject* obj, const char* key, exlib::string& retVal)
{
    Variant v;
    result_t hr = obj->get(key, v);
    if (hr == CALL_RETURN_NULL)
        return setRuntimeError(CALL_E_PARAMNOTOPTIONAL, key);
    if (hr < 0)
        return hr;

    if (v.type() != Variant::VT_String)
        return CALL_E_BADVARTYPE;

    retVal = v.string();

    return 0;
}

static result_t GetJwkValue(NObject* obj, const char* key, exlib::string& retVal)
{
    exlib::string s;
    result_t hr = GetConfigValue(obj, key, s);
    if (hr < 0)
        return hr;

    base64Decode(s.c_str(), s.length(), retVal);
    return 0;
}

static result_t GetJwkValue(NObject* obj, const char* key, BIGNUM*& n)
{
    exlib::string s;
    result_t hr = GetJwkValue(obj, key, s);
    if (hr < 0)
        return hr;

    n = BN_bin2bn((const unsigned char*)s.c_str(), s.length(), nullptr);
    return 0;
}

result_t KeyObject::ImportJWKRsaKey(NObject* key, KeyType type)
{
    result_t hr;

    BignumPointer n;
    BignumPointer e;

    hr = GetJwkValue(key, "e", e);
    if (hr < 0)
        return hr;

    hr = GetJwkValue(key, "n", n);
    if (hr < 0)
        return hr;

    RsaPointer rsa = RSA_new();
    if (RSA_set0_key(rsa, n, e, nullptr) != 1)
        return openssl_error();

    n.release();
    e.release();

    if (type == kKeyTypePrivate || type == kKeyTypeUnknown) {
        int32_t he = 0;

        BignumPointer d;
        BignumPointer p;
        BignumPointer q;
        BignumPointer dp;
        BignumPointer dq;
        BignumPointer qi;

        hr = GetJwkValue(key, "d", d);
        if (hr == CALL_E_PARAMNOTOPTIONAL && type == kKeyTypeUnknown)
            he = 1;
        else if (hr < 0)
            return hr;

        hr = GetJwkValue(key, "p", p);
        if (hr == CALL_E_PARAMNOTOPTIONAL && type == kKeyTypeUnknown) {
            if (he++ != 1)
                return hr;
        } else if (hr < 0)
            return hr;

        hr = GetJwkValue(key, "q", q);
        if (hr == CALL_E_PARAMNOTOPTIONAL && type == kKeyTypeUnknown) {
            if (he++ != 2)
                return hr;
        } else if (hr < 0)
            return hr;

        hr = GetJwkValue(key, "dp", dp);
        if (hr == CALL_E_PARAMNOTOPTIONAL && type == kKeyTypeUnknown) {
            if (he++ != 3)
                return hr;
        } else if (hr < 0)
            return hr;

        hr = GetJwkValue(key, "dq", dq);
        if (hr == CALL_E_PARAMNOTOPTIONAL && type == kKeyTypeUnknown) {
            if (he++ != 4)
                return hr;
        } else if (hr < 0)
            return hr;

        hr = GetJwkValue(key, "qi", qi);
        if (hr == CALL_E_PARAMNOTOPTIONAL && type == kKeyTypeUnknown) {
            if (he++ != 5)
                return hr;
        } else if (hr < 0)
            return hr;

        if (he == 0) {
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

            type = kKeyTypePrivate;
        } else
            type = kKeyTypePublic;
    }

    m_pkey = EVP_PKEY_new();
    if (EVP_PKEY_set1_RSA(m_pkey, rsa) != 1)
        return openssl_error();

    if (type == kKeyTypePrivate)
        if (m_pkey->ameth->pkey_check(m_pkey) != 1)
            return Runtime::setError("Invalid private key");

    m_keyType = type;

    return 0;
}

int GetCurveFromName(const char* name)
{
    if (qstricmp(name, "secp256r1") == 0)
        return NID_X9_62_prime256v1;
    if (qstricmp(name, "sm2") == 0)
        return NID_sm2;

    int nid = EC_curve_nist2nid(name);
    if (nid == NID_undef)
        nid = OBJ_sn2nid(name);
    return nid;
}

result_t KeyObject::ImportJWKEcKey(NObject* key, KeyType type)
{
    result_t hr;

    exlib::string crv_name;
    hr = GetConfigValue(key, "crv", crv_name);
    if (hr < 0)
        return hr;

    int nid = GetCurveFromName(crv_name.c_str());
    if (nid == NID_undef)
        return Runtime::setError("Invalid JWK EC key");

    BignumPointer x;
    BignumPointer y;

    hr = GetJwkValue(key, "x", x);
    if (hr < 0)
        return hr;

    hr = GetJwkValue(key, "y", y);
    if (hr < 0)
        return hr;

    ECKeyPointer ec = EC_KEY_new_by_curve_name(nid);
    if (EC_KEY_set_public_key_affine_coordinates(ec, x, y) != 1)
        return openssl_error();

    x.release();
    y.release();

    if (type == kKeyTypePrivate || type == kKeyTypeUnknown) {
        BignumPointer d;
        hr = GetJwkValue(key, "d", d);
        if (hr < 0 && type == kKeyTypePrivate)
            return hr;

        if (hr >= 0) {
            if (EC_KEY_set_private_key(ec, d) != 1)
                return openssl_error();

            d.release();

            type = kKeyTypePrivate;
        } else
            type = kKeyTypePublic;
    }

    m_pkey = EVP_PKEY_new();
    if (EVP_PKEY_set1_EC_KEY(m_pkey, ec) != 1)
        return openssl_error();

    if (type == kKeyTypePrivate)
        if (m_pkey->ameth->pkey_check(m_pkey) != 1)
            return Runtime::setError("Invalid private key");

    m_keyType = type;

    return 0;
}

result_t KeyObject::ImportJWKOKPKey(NObject* key, KeyType type)
{
    result_t hr;

    exlib::string crv_name;
    hr = GetConfigValue(key, "crv", crv_name);
    if (hr < 0)
        return hr;

    int nid = okp_curve_nid(crv_name.c_str());
    if (nid == NID_undef)
        return Runtime::setError("Invalid JWK OKP key");

    if (type == kKeyTypeUnknown) {
        exlib::string d;
        hr = GetJwkValue(key, "d", d);
        if (hr == CALL_E_PARAMNOTOPTIONAL) {
            exlib::string x;
            hr = GetJwkValue(key, "x", x);
            if (hr < 0)
                return hr;

            m_pkey = EVP_PKEY_new_raw_public_key(nid, nullptr, (const unsigned char*)x.c_str(), x.length());
            if (!m_pkey)
                return openssl_error();

            type = kKeyTypePublic;
        }
        if (hr < 0)
            return hr;

        m_pkey = EVP_PKEY_new_raw_private_key(nid, nullptr, (const unsigned char*)d.c_str(), d.length());
        if (!m_pkey)
            return openssl_error();

        type = kKeyTypePrivate;
    } else if (type == kKeyTypePrivate) {
        exlib::string d;
        hr = GetJwkValue(key, "d", d);
        if (hr < 0)
            return hr;

        m_pkey = EVP_PKEY_new_raw_private_key(nid, nullptr, (const unsigned char*)d.c_str(), d.length());
        if (!m_pkey)
            return openssl_error();
    } else {
        exlib::string x;
        hr = GetJwkValue(key, "x", x);
        if (hr < 0)
            return hr;

        m_pkey = EVP_PKEY_new_raw_public_key(nid, nullptr, (const unsigned char*)x.c_str(), x.length());
        if (!m_pkey)
            return openssl_error();
    }

    m_keyType = type;

    return 0;
}

result_t KeyObject::ImportJWKAsymmetricKey(NObject* jwk, KeyType type)
{
    result_t hr;

    exlib::string kty;
    hr = GetConfigValue(jwk, "kty", kty);
    if (hr < 0)
        return hr;

    if (kty == "RSA")
        return ImportJWKRsaKey(jwk, type);
    else if (kty == "EC")
        return ImportJWKEcKey(jwk, type);
    else if (kty == "OKP")
        return ImportJWKOKPKey(jwk, type);

    return CALL_E_INVALID_CALL;
}

static exlib::string base64Encode(const char* data, size_t size)
{
    exlib::string oct;
    base64Encode(data, size, true, oct);
    return oct;
}

static exlib::string base64Encode(const BIGNUM* n, size_t size = 0)
{
    if (size == 0)
        size = BN_num_bytes(n);
    std::vector<unsigned char> public_exponent(size);
    BN_bn2binpad(n, public_exponent.data(), size);
    return base64Encode((const char*)public_exponent.data(), size);
}

result_t KeyObject::ExportJWKSecretKey(Variant& retVal)
{
    obj_ptr<NObject> jwk = new NObject();

    jwk->add("kty", "oct");
    jwk->add("k", base64Encode((const char*)m_key.data(), m_key.size()));

    retVal = jwk;

    return 0;
}

result_t KeyObject::ExportJWKRsaKey(Variant& retVal)
{
    obj_ptr<NObject> jwk = new NObject();
    jwk->add("kty", "RSA");

    const RSA* rsa = EVP_PKEY_get0_RSA(m_pkey);

    const BIGNUM* n;
    const BIGNUM* e;
    const BIGNUM* d;

    RSA_get0_key(rsa, &n, &e, &d);
    jwk->add("e", base64Encode(e));
    jwk->add("n", base64Encode(n));

    if (m_keyType == kKeyTypePrivate) {
        const BIGNUM* p;
        const BIGNUM* q;
        const BIGNUM* dp;
        const BIGNUM* dq;
        const BIGNUM* qi;

        RSA_get0_factors(rsa, &p, &q);
        RSA_get0_crt_params(rsa, &dp, &dq, &qi);
        jwk->add("d", base64Encode(d));
        jwk->add("p", base64Encode(p));
        jwk->add("q", base64Encode(q));
        jwk->add("dp", base64Encode(dp));
        jwk->add("dq", base64Encode(dq));
        jwk->add("qi", base64Encode(qi));
    }

    retVal = jwk;

    return 0;
}

result_t KeyObject::ExportJWKEcKey(Variant& retVal)
{
    const EC_KEY* ec = EVP_PKEY_get0_EC_KEY(m_pkey);
    const EC_GROUP* group = EC_KEY_get0_group(ec);

    obj_ptr<NObject> jwk = new NObject();

    jwk->add("kty", "EC");
    jwk->add("crv", namedCurve());

    const EC_POINT* pub = EC_KEY_get0_public_key(ec);

    int degree_bits = EC_GROUP_get_degree(group);
    int degree_bytes = (degree_bits / CHAR_BIT) + (7 + (degree_bits % CHAR_BIT)) / 8;

    BignumPointer x(BN_new());
    BignumPointer y(BN_new());

    EC_POINT_get_affine_coordinates(group, pub, x, y, nullptr);

    jwk->add("x", base64Encode(x, degree_bytes));
    jwk->add("y", base64Encode(y, degree_bytes));

    if (m_keyType == kKeyTypePrivate) {
        const BIGNUM* d = EC_KEY_get0_private_key(ec);
        jwk->add("d", base64Encode(d, degree_bytes));
    }

    retVal = jwk;

    return 0;
}

result_t KeyObject::ExportJWKOKPKey(Variant& retVal)
{
    obj_ptr<NObject> jwk = new NObject();

    jwk->add("kty", "OKP");
    jwk->add("crv", okp_curve_name(EVP_PKEY_id(m_pkey)));

    size_t len = 0;
    std::vector<unsigned char> key;

    EVP_PKEY_get_raw_public_key(m_pkey, nullptr, &len);
    key.resize(len);
    EVP_PKEY_get_raw_public_key(m_pkey, key.data(), &len);
    jwk->add("x", base64Encode((const char*)key.data(), len));

    if (m_keyType == kKeyTypePrivate) {
        EVP_PKEY_get_raw_private_key(m_pkey, nullptr, &len);
        key.resize(len);
        EVP_PKEY_get_raw_private_key(m_pkey, key.data(), &len);
        jwk->add("d", base64Encode((const char*)key.data(), len));
    }

    retVal = jwk;

    return 0;
}

result_t KeyObject::export_json(Variant& retVal)
{
    if (m_keyType == kKeyTypeSecret)
        return ExportJWKSecretKey(retVal);
    else if (is_okp_curve(EVP_PKEY_id(m_pkey)))
        return ExportJWKOKPKey(retVal);
    else
        switch (EVP_PKEY_id(m_pkey)) {
        case EVP_PKEY_RSA:
            return ExportJWKRsaKey(retVal);
        case EVP_PKEY_SM2:
        case EVP_PKEY_EC:
            return ExportJWKEcKey(retVal);
        }

    return Runtime::setError("Not supported key type");
}

}
