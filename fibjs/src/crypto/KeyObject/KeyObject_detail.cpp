/*
 * KeyObject_detail.cpp
 *
 *  Created on: Feb 19, 2024
 *      Author: lion
 */

#include "encoding.h"
#include "ifs/crypto.h"
#include "KeyObject.h"
#include "Buffer.h"

namespace fibjs {

v8::Local<v8::BigInt> nb2BigInt(v8::Local<v8::Context> context, const BIGNUM* e)
{
    int e_len = BN_num_bytes(e);
    int fe_len = (e_len + 7) & ~7;
    std::vector<unsigned char> public_exponent(fe_len);

    BN_bn2binpad(e, public_exponent.data() + fe_len - e_len, e_len);
    if (fe_len > e_len)
        memset(public_exponent.data(), 0, fe_len - e_len);

    for (int i = 0; i < fe_len; i += 8) {
        unsigned char t;

        t = public_exponent[i];
        public_exponent[i] = public_exponent[i + 7];
        public_exponent[i + 7] = t;

        t = public_exponent[i + 1];
        public_exponent[i + 1] = public_exponent[i + 6];
        public_exponent[i + 6] = t;

        t = public_exponent[i + 2];
        public_exponent[i + 2] = public_exponent[i + 5];
        public_exponent[i + 5] = t;

        t = public_exponent[i + 3];
        public_exponent[i + 3] = public_exponent[i + 4];
        public_exponent[i + 4] = t;
    }

    return v8::BigInt::NewFromWords(context, 0, fe_len / 8, (const uint64_t*)public_exponent.data()).FromMaybe(v8::Local<v8::BigInt>());
}

result_t KeyObject::GetRsaKeyDetail(v8::Local<v8::Object>& retVal, bool is_pss)
{
    Isolate* isolate = holder();
    v8::Local<v8::Context> context = isolate->context();

    const BIGNUM* e;
    const BIGNUM* n;
    const RSA* rsa = EVP_PKEY_get0_RSA(m_pkey);

    RSA_get0_key(rsa, &n, &e, nullptr);
    size_t modulus_length = BN_num_bits(n);

    retVal->Set(context, isolate->NewString("modulusLength"), v8::Number::New(isolate->m_isolate, modulus_length))
        .IsNothing();

    retVal->Set(context, isolate->NewString("publicExponent"), nb2BigInt(context, e))
        .IsNothing();

    if (is_pss) {
        const RSA_PSS_PARAMS* params = RSA_get0_pss_params(rsa);

        if (params != nullptr) {
            int hash_nid = NID_sha1;
            int mgf_nid = NID_mgf1;
            int mgf1_hash_nid = NID_sha1;
            int64_t salt_length = 20;

            if (params->hashAlgorithm != nullptr) {
                const ASN1_OBJECT* hash_obj;
                X509_ALGOR_get0(&hash_obj, nullptr, nullptr, params->hashAlgorithm);
                hash_nid = OBJ_obj2nid(hash_obj);
            }

            retVal->Set(context, isolate->NewString("hashAlgorithm"), isolate->NewString(OBJ_nid2ln(hash_nid)))
                .IsNothing();

            if (params->maskGenAlgorithm != nullptr) {
                const ASN1_OBJECT* mgf_obj;
                X509_ALGOR_get0(&mgf_obj, nullptr, nullptr, params->maskGenAlgorithm);
                mgf_nid = OBJ_obj2nid(mgf_obj);
                if (mgf_nid == NID_mgf1) {
                    const ASN1_OBJECT* mgf1_hash_obj;
                    X509_ALGOR_get0(&mgf1_hash_obj, nullptr, nullptr, params->maskHash);
                    mgf1_hash_nid = OBJ_obj2nid(mgf1_hash_obj);
                }
            }

            if (mgf_nid == NID_mgf1)
                retVal->Set(context, isolate->NewString("mgf1HashAlgorithm"), isolate->NewString(OBJ_nid2ln(mgf1_hash_nid)))
                    .IsNothing();

            if (params->saltLength != nullptr) {
                if (ASN1_INTEGER_get_int64(&salt_length, params->saltLength) != 1)
                    return openssl_error();
            }

            retVal->Set(context, isolate->NewString("saltLength"), v8::Number::New(isolate->m_isolate, salt_length))
                .IsNothing();
        }
    }

    return 0;
}

result_t KeyObject::GetDsaKeyDetail(v8::Local<v8::Object>& retVal)
{
    Isolate* isolate = holder();
    v8::Local<v8::Context> context = isolate->context();

    const BIGNUM* p;
    const BIGNUM* q;
    const DSA* dsa = EVP_PKEY_get0_DSA(m_pkey);

    DSA_get0_pqg(dsa, &p, &q, nullptr);

    size_t modulus_length = BN_num_bits(p);
    size_t divisor_length = BN_num_bits(q);

    retVal->Set(context, isolate->NewString("modulusLength"), v8::Number::New(isolate->m_isolate, modulus_length))
        .IsNothing();
    retVal->Set(context, isolate->NewString("divisorLength"), v8::Number::New(isolate->m_isolate, divisor_length))
        .IsNothing();

    return 0;
}

result_t KeyObject::GetEcKeyDetail(v8::Local<v8::Object>& retVal)
{
    Isolate* isolate = holder();
    v8::Local<v8::Context> context = isolate->context();

    const EC_KEY* ec = EVP_PKEY_get0_EC_KEY(m_pkey);

    const EC_GROUP* group = EC_KEY_get0_group(ec);
    int nid = EC_GROUP_get_curve_name(group);

    retVal->Set(context, isolate->NewString("namedCurve"), isolate->NewString(OBJ_nid2ln(nid)))
        .IsNothing();

    return 0;
}

result_t KeyObject::GetDhKeyDetail(v8::Local<v8::Object>& retVal)
{
    return 0;
}

result_t KeyObject::get_asymmetricKeyDetails(v8::Local<v8::Object>& retVal)
{
    if (m_keyType == kKeyTypeSecret)
        return CALL_RETURN_UNDEFINED;

    retVal = v8::Object::New(holder()->m_isolate);

    switch (EVP_PKEY_id(m_pkey)) {
    case EVP_PKEY_RSA:
        return GetRsaKeyDetail(retVal, false);
    case EVP_PKEY_RSA_PSS:
        return GetRsaKeyDetail(retVal, true);
    case EVP_PKEY_DSA:
        return GetDsaKeyDetail(retVal);
    case EVP_PKEY_EC:
    case EVP_PKEY_SM2:
        return GetEcKeyDetail(retVal);
    case EVP_PKEY_DH:
        return GetDhKeyDetail(retVal);
    }

    return CALL_RETURN_UNDEFINED;
}

}
