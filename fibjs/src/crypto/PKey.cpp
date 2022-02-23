/*
 * PKey.cpp
 *
 *  Created on: Apr 11, 2014
 *      Author: lion
 */

#include "object.h"
#include "ifs/fs.h"
#include "ifs/crypto.h"
#include "PKey.h"
#include "Cipher.h"
#include "Buffer.h"
#include "ssl.h"
#include "encoding.h"

namespace fibjs {

struct curve_info {
    mbedtls_ecp_group_id id;
    const char* name;
};

static const curve_info supported_curves[] = {
    { MBEDTLS_ECP_DP_SECP192R1, "P-192" },
    { MBEDTLS_ECP_DP_SECP192R1, "secp192r1" },
    { MBEDTLS_ECP_DP_SECP224R1, "P-224" },
    { MBEDTLS_ECP_DP_SECP224R1, "secp224r1" },
    { MBEDTLS_ECP_DP_SECP256R1, "P-256" },
    { MBEDTLS_ECP_DP_SECP256R1, "secp256r1" },
    { MBEDTLS_ECP_DP_SECP384R1, "P-384" },
    { MBEDTLS_ECP_DP_SECP384R1, "secp384r1" },
    { MBEDTLS_ECP_DP_SECP521R1, "P-521" },
    { MBEDTLS_ECP_DP_SECP521R1, "secp521r1" },
    { MBEDTLS_ECP_DP_BP256R1, "brainpoolP256r1" },
    { MBEDTLS_ECP_DP_BP384R1, "brainpoolP384r1" },
    { MBEDTLS_ECP_DP_BP512R1, "brainpoolP512r1" },
    { MBEDTLS_ECP_DP_SECP192K1, "secp192k1" },
    { MBEDTLS_ECP_DP_SECP224K1, "secp224k1" },
    { MBEDTLS_ECP_DP_SECP256K1, "P-256K" },
    { MBEDTLS_ECP_DP_SECP256K1, "secp256k1" },
    { MBEDTLS_ECP_DP_SM2P256R1, "sm2p256r1" },
};

mbedtls_ecp_group_id get_curve_id(exlib::string& curve)
{
    int32_t i;

    for (i = 0; i < ARRAYSIZE(supported_curves); i++)
        if (!strcmp(curve.c_str(), supported_curves[i].name))
            return supported_curves[i].id;

    return MBEDTLS_ECP_DP_NONE;
}

const char* get_curve_name(mbedtls_ecp_group_id id)
{
    int32_t i;

    for (i = 0; i < ARRAYSIZE(supported_curves); i++)
        if (supported_curves[i].id == id)
            return supported_curves[i].name;

    return NULL;
}

result_t PKey_base::_new(obj_ptr<PKey_base>& retVal, v8::Local<v8::Object> This)
{
    retVal = new PKey();
    return 0;
}

result_t PKey_base::_new(Buffer_base* DerKey, exlib::string password, obj_ptr<PKey_base>& retVal,
    v8::Local<v8::Object> This)
{
    obj_ptr<PKey> pk = new PKey();
    result_t hr = pk->importKey(DerKey, password);
    if (hr < 0)
        return hr;

    retVal = pk;
    return 0;
}

result_t PKey_base::_new(exlib::string pemKey, exlib::string password, obj_ptr<PKey_base>& retVal,
    v8::Local<v8::Object> This)
{
    obj_ptr<PKey> pk = new PKey();
    result_t hr = pk->importKey(pemKey, password);
    if (hr < 0)
        return hr;

    retVal = pk;
    return 0;
}

result_t PKey_base::_new(v8::Local<v8::Object> jsonKey, obj_ptr<PKey_base>& retVal,
    v8::Local<v8::Object> This)
{
    obj_ptr<PKey> pk = new PKey();
    result_t hr = pk->importKey(jsonKey);
    if (hr < 0)
        return hr;

    retVal = pk;
    return 0;
}

PKey::PKey()
{
    mbedtls_pk_init(&m_key);
}

PKey::~PKey()
{
    clear();
}

void PKey::clear()
{
    if (mbedtls_pk_get_type(&m_key))
        mbedtls_pk_free(&m_key);
}

result_t PKey::genRsaKey(int32_t size, AsyncEvent* ac)
{
    if (size < 128 || size > 8192)
        return CHECK_ERROR(Runtime::setError("PKey: Invalid key size"));

    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    int32_t ret;

    clear();

    ret = mbedtls_pk_setup(&m_key, mbedtls_pk_info_from_type(MBEDTLS_PK_RSA));
    if (ret != 0)
        return CHECK_ERROR(_ssl::setError(ret));

    ret = mbedtls_rsa_gen_key(mbedtls_pk_rsa(m_key), mbedtls_ctr_drbg_random,
        &g_ssl.ctr_drbg, size, 65537);

    if (ret != 0)
        return CHECK_ERROR(_ssl::setError(ret));

    return 0;
}

result_t PKey::genEcKey(exlib::string curve, AsyncEvent* ac)
{
    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    mbedtls_ecp_group_id id = get_curve_id(curve);
    if (id == MBEDTLS_ECP_DP_NONE || id == MBEDTLS_ECP_DP_SM2P256R1)
        return CHECK_ERROR(Runtime::setError("PKey: Unknown curve"));

    int32_t ret;

    clear();

    ret = mbedtls_pk_setup(&m_key, mbedtls_pk_info_from_type(MBEDTLS_PK_ECKEY));
    if (ret != 0)
        return CHECK_ERROR(_ssl::setError(ret));

    ret = mbedtls_ecp_gen_key(id, mbedtls_pk_ec(m_key),
        mbedtls_ctr_drbg_random, &g_ssl.ctr_drbg);
    if (ret != 0)
        return CHECK_ERROR(_ssl::setError(ret));

    return 0;
}

result_t PKey::genSm2Key(AsyncEvent* ac)
{
    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    int32_t ret;

    clear();

    ret = mbedtls_pk_setup(&m_key, mbedtls_pk_info_from_type(MBEDTLS_PK_SM2));
    if (ret != 0)
        return CHECK_ERROR(_ssl::setError(ret));

    ret = mbedtls_ecp_gen_key(MBEDTLS_ECP_DP_SM2P256R1, mbedtls_pk_ec(m_key),
        mbedtls_ctr_drbg_random, &g_ssl.ctr_drbg);
    if (ret != 0)
        return CHECK_ERROR(_ssl::setError(ret));

    return 0;
}

result_t PKey::isPrivate(bool& retVal)
{
    mbedtls_pk_type_t type = mbedtls_pk_get_type(&m_key);

    if (type == MBEDTLS_PK_RSA) {
        retVal = mbedtls_rsa_check_privkey(mbedtls_pk_rsa(m_key)) == 0;
        return 0;
    }

    if (type == MBEDTLS_PK_ECKEY || type == MBEDTLS_PK_SM2) {
        mbedtls_ecp_keypair* ecp = mbedtls_pk_ec(m_key);
        retVal = mbedtls_ecp_check_privkey(&ecp->grp, &ecp->d) == 0;
        return 0;
    }

    return CHECK_ERROR(CALL_E_INVALID_CALL);
}

result_t PKey::get_publicKey(obj_ptr<PKey_base>& retVal)
{
    result_t hr;
    bool priv;

    hr = isPrivate(priv);
    if (hr < 0)
        return hr;

    if (!priv)
        return CALL_RETURN_NULL;

    mbedtls_pk_type_t type = mbedtls_pk_get_type(&m_key);
    int32_t ret;

    if (type == MBEDTLS_PK_RSA) {
        mbedtls_rsa_context* rsa = mbedtls_pk_rsa(m_key);
        obj_ptr<PKey> pk1 = new PKey();

        ret = mbedtls_pk_setup(&pk1->m_key, mbedtls_pk_info_from_type(MBEDTLS_PK_RSA));
        if (ret != 0)
            return CHECK_ERROR(_ssl::setError(ret));

        mbedtls_rsa_context* rsa1 = mbedtls_pk_rsa(pk1->m_key);

        ret = mbedtls_mpi_copy(&rsa1->N, &rsa->N);
        if (ret != 0)
            return CHECK_ERROR(_ssl::setError(ret));

        ret = mbedtls_mpi_copy(&rsa1->E, &rsa->E);
        if (ret != 0)
            return CHECK_ERROR(_ssl::setError(ret));

        rsa1->len = mbedtls_mpi_size(&rsa1->N);

        retVal = pk1;

        return 0;
    }

    if (type == MBEDTLS_PK_ECKEY || type == MBEDTLS_PK_SM2) {
        mbedtls_ecp_keypair* ecp = mbedtls_pk_ec(m_key);

        obj_ptr<PKey> pk1 = new PKey();

        ret = mbedtls_pk_setup(&pk1->m_key, mbedtls_pk_info_from_type(type));
        if (ret != 0)
            return CHECK_ERROR(_ssl::setError(ret));

        mbedtls_ecp_keypair* ecp1 = mbedtls_pk_ec(pk1->m_key);

        ret = mbedtls_ecp_group_copy(&ecp1->grp, &ecp->grp);
        if (ret != 0)
            return CHECK_ERROR(_ssl::setError(ret));

        ret = mbedtls_ecp_copy(&ecp1->Q, &ecp->Q);
        if (ret != 0)
            return CHECK_ERROR(_ssl::setError(ret));

        retVal = pk1;

        return 0;
    }

    return CHECK_ERROR(CALL_E_INVALID_CALL);
}

result_t PKey::copy(const mbedtls_pk_context& key)
{
    mbedtls_pk_type_t type = mbedtls_pk_get_type(&key);
    int32_t ret;

    if (type == MBEDTLS_PK_RSA) {
        mbedtls_rsa_context* rsa = mbedtls_pk_rsa(key);

        ret = mbedtls_pk_setup(&m_key, mbedtls_pk_info_from_type(MBEDTLS_PK_RSA));
        if (ret != 0)
            return CHECK_ERROR(_ssl::setError(ret));

        mbedtls_rsa_context* rsa1 = mbedtls_pk_rsa(m_key);

        ret = mbedtls_rsa_copy(rsa1, rsa);
        if (ret != 0)
            return CHECK_ERROR(_ssl::setError(ret));

        return 0;
    }

    if (type == MBEDTLS_PK_ECKEY || type == MBEDTLS_PK_SM2) {
        mbedtls_ecp_keypair* ecp = mbedtls_pk_ec(key);

        ret = mbedtls_pk_setup(&m_key, mbedtls_pk_info_from_type(type));
        if (ret != 0)
            return CHECK_ERROR(_ssl::setError(ret));

        mbedtls_ecp_keypair* ecp1 = mbedtls_pk_ec(m_key);

        ret = mbedtls_ecp_group_copy(&ecp1->grp, &ecp->grp);
        if (ret != 0)
            return CHECK_ERROR(_ssl::setError(ret));

        ret = mbedtls_mpi_copy(&ecp1->d, &ecp->d);
        if (ret != 0)
            return CHECK_ERROR(_ssl::setError(ret));

        ret = mbedtls_ecp_copy(&ecp1->Q, &ecp->Q);
        if (ret != 0)
            return CHECK_ERROR(_ssl::setError(ret));

        return 0;
    }

    return CHECK_ERROR(CALL_E_INVALID_CALL);
}

result_t PKey::clone(obj_ptr<PKey_base>& retVal)
{
    obj_ptr<PKey> pk1 = new PKey();
    result_t hr;

    hr = pk1->copy(m_key);
    if (hr < 0)
        return 0;

    retVal = pk1;
    return 0;
}

result_t PKey::importKey(Buffer_base* DerKey, exlib::string password)
{
    int32_t ret;

    exlib::string key;
    DerKey->toString(key);

    clear();

    ret = mbedtls_pk_parse_key(&m_key, (unsigned char*)key.c_str(), key.length(),
        !password.empty() ? (unsigned char*)password.c_str() : NULL,
        password.length());

    if (ret == MBEDTLS_ERR_PK_KEY_INVALID_FORMAT)
        ret = mbedtls_pk_parse_public_key(&m_key, (unsigned char*)key.c_str(), key.length());

    if (ret != 0)
        return CHECK_ERROR(_ssl::setError(ret));

    return 0;
}

result_t PKey::importKey(exlib::string pemKey, exlib::string password)
{
    int32_t ret;

    clear();

    ret = mbedtls_pk_parse_key(&m_key, (unsigned char*)pemKey.c_str(), pemKey.length() + 1,
        !password.empty() ? (unsigned char*)password.c_str() : NULL,
        password.length());

    if (ret == MBEDTLS_ERR_PK_KEY_INVALID_FORMAT) {
        size_t pos1 = pemKey.find("-----BEGIN RSA PUBLIC KEY-----", 0, 30);
        size_t pos2 = pemKey.find("-----END RSA PUBLIC KEY-----", 0, 28);

        if (pos1 != exlib::string::npos && pos2 != exlib::string::npos) {
            exlib::string pemKey1;
            pemKey1.assign("-----BEGIN PUBLIC KEY-----\nMIIBIjANBgkqhkiG9w0BAQEFAAOCAQ8A", 59);
            pemKey1.append(pemKey.substr(pos1 + 30, pos2 - pos1 - 30));
            pemKey1.append("-----END PUBLIC KEY-----", 24);

            pemKey = pemKey1;
        }

        ret = mbedtls_pk_parse_public_key(&m_key, (unsigned char*)pemKey.c_str(), pemKey.length() + 1);
    }

    if (ret != 0)
        return CHECK_ERROR(_ssl::setError(ret));

    return 0;
}

inline result_t mpi_load(Isolate* isolate, mbedtls_mpi* n, v8::Local<v8::Object> o, const char* key)
{
    exlib::string b64, s;
    result_t hr;

    hr = GetConfigValue(isolate->m_isolate, o, key, b64);
    if (hr < 0)
        return hr;

    base64Decode(b64, s);

    int32_t ret;
    ret = mbedtls_mpi_read_binary(n, (unsigned char*)s.c_str(), s.length());
    if (ret != 0)
        return CHECK_ERROR(_ssl::setError(ret));

    return 0;
}

result_t PKey::importKey(v8::Local<v8::Object> jsonKey)
{
    Isolate* isolate = holder();
    int32_t ret;
    result_t hr;
    exlib::string kty;

    clear();

    hr = GetConfigValue(isolate->m_isolate, jsonKey, "kty", kty);
    if (hr < 0)
        return hr;

    if (kty == "RSA") {
        ret = mbedtls_pk_setup(&m_key, mbedtls_pk_info_from_type(MBEDTLS_PK_RSA));
        if (ret != 0)
            return CHECK_ERROR(_ssl::setError(ret));

        mbedtls_rsa_context* rsa = mbedtls_pk_rsa(m_key);

        hr = mpi_load(isolate, &rsa->N, jsonKey, "n");
        if (hr < 0)
            return hr;

        hr = mpi_load(isolate, &rsa->E, jsonKey, "e");
        if (hr < 0)
            return hr;

        rsa->len = mbedtls_mpi_size(&rsa->N);

        ret = mbedtls_rsa_check_pubkey(rsa);
        if (ret != 0)
            return CHECK_ERROR(_ssl::setError(ret));

        hr = mpi_load(isolate, &rsa->D, jsonKey, "d");
        if (hr >= 0) {
            hr = mpi_load(isolate, &rsa->P, jsonKey, "p");
            if (hr < 0)
                return hr;

            hr = mpi_load(isolate, &rsa->Q, jsonKey, "q");
            if (hr < 0)
                return hr;

            hr = mpi_load(isolate, &rsa->DP, jsonKey, "dp");
            if (hr < 0)
                return hr;

            hr = mpi_load(isolate, &rsa->DQ, jsonKey, "dq");
            if (hr < 0)
                return hr;

            hr = mpi_load(isolate, &rsa->QP, jsonKey, "qi");
            if (hr < 0)
                return hr;

            ret = mbedtls_rsa_check_privkey(rsa);
            if (ret != 0)
                return CHECK_ERROR(_ssl::setError(ret));
        } else if (hr != CALL_E_PARAMNOTOPTIONAL)
            return hr;

        return 0;
    }

    if (kty == "EC" || kty == "SM2") {
        exlib::string curve;
        bool is_priv = false;

        hr = GetConfigValue(isolate->m_isolate, jsonKey, "crv", curve);
        if (hr < 0)
            return hr;

        mbedtls_ecp_group_id id = get_curve_id(curve);
        if (id == MBEDTLS_ECP_DP_NONE)
            return CHECK_ERROR(Runtime::setError("PKey: Unknown curve"));
        if (kty == "SM2")
            ret = mbedtls_pk_setup(&m_key, mbedtls_pk_info_from_type(MBEDTLS_PK_SM2));
        else
            ret = mbedtls_pk_setup(&m_key, mbedtls_pk_info_from_type(MBEDTLS_PK_ECKEY));
        if (ret != 0)
            return CHECK_ERROR(_ssl::setError(ret));

        mbedtls_ecp_keypair* ecp = mbedtls_pk_ec(m_key);

        mbedtls_ecp_group_load(&ecp->grp, id);

        hr = mpi_load(isolate, &ecp->d, jsonKey, "d");
        if (hr >= 0) {
            ret = mbedtls_ecp_check_privkey(&ecp->grp, &ecp->d);
            if (ret != 0)
                return CHECK_ERROR(_ssl::setError(ret));
            is_priv = true;
        } else if (hr != CALL_E_PARAMNOTOPTIONAL)
            return hr;

        do {
            hr = mpi_load(isolate, &ecp->Q.X, jsonKey, "x");
            if (hr < 0)
                break;

            hr = mpi_load(isolate, &ecp->Q.Y, jsonKey, "y");
            if (hr < 0)
                break;

            ret = mbedtls_mpi_lset(&ecp->Q.Z, 1);
            if (ret != 0)
                return CHECK_ERROR(_ssl::setError(ret));

            ret = mbedtls_ecp_check_pubkey(&ecp->grp, &ecp->Q);
            if (ret != 0)
                return CHECK_ERROR(_ssl::setError(ret));
        } while (false);

        if (!is_priv || hr != CALL_E_PARAMNOTOPTIONAL)
            return hr;

        ret = mbedtls_ecp_mul(&ecp->grp, &ecp->Q, &ecp->d, &ecp->grp.G,
            mbedtls_ctr_drbg_random, &g_ssl.ctr_drbg);
        if (ret != 0)
            return CHECK_ERROR(_ssl::setError(ret));

        return 0;
    }

    return CHECK_ERROR(CALL_E_INVALIDARG);
}

result_t PKey::importFile(exlib::string filename, exlib::string password)
{
    result_t hr;
    exlib::string data;
    obj_ptr<Buffer> buf;

    hr = fs_base::ac_readTextFile(filename, data);
    if (hr < 0)
        return hr;

    if (qstrstr(data.c_str(), "BEGIN"))
        return importKey(data, password);

    buf = new Buffer(data);
    return importKey(buf, password);
}

result_t PKey::exportPem(exlib::string& retVal)
{
    result_t hr;
    bool priv;

    hr = isPrivate(priv);
    if (hr < 0)
        return hr;

    int32_t ret;
    exlib::string buf;

    buf.resize(mbedtls_pk_get_len(&m_key) * 8 + 128);
    if (priv)
        ret = mbedtls_pk_write_key_pem(&m_key, (unsigned char*)buf.c_buffer(), buf.length());
    else
        ret = mbedtls_pk_write_pubkey_pem(&m_key, (unsigned char*)buf.c_buffer(), buf.length());
    if (ret != 0)
        return CHECK_ERROR(_ssl::setError(ret));

    buf.resize(qstrlen(buf.c_str()));
    retVal = buf;

    return 0;
}

result_t PKey::exportDer(obj_ptr<Buffer_base>& retVal)
{
    result_t hr;
    bool priv;

    hr = isPrivate(priv);
    if (hr < 0)
        return hr;

    int32_t ret;
    exlib::string buf;

    buf.resize(8192);
    if (priv)
        ret = mbedtls_pk_write_key_der(&m_key, (unsigned char*)buf.c_buffer(), buf.length());
    else
        ret = mbedtls_pk_write_pubkey_der(&m_key, (unsigned char*)buf.c_buffer(), buf.length());
    if (ret < 0)
        return CHECK_ERROR(_ssl::setError(ret));

    retVal = new Buffer(buf.substr(buf.length() - ret));

    return 0;
}

inline void mpi_dump(Isolate* isolate, v8::Local<v8::Object> o, exlib::string key, const mbedtls_mpi* n)
{
    exlib::string data;
    int32_t sz = (int32_t)mbedtls_mpi_size(n);

    data.resize(sz);
    mbedtls_mpi_write_binary(n, (unsigned char*)data.c_buffer(), sz);

    exlib::string b64;
    base64Encode(data.c_str(), data.length(), true, b64);

    o->Set(isolate->context(), isolate->NewString(key), isolate->NewString(b64));
}

result_t PKey::exportJson(v8::Local<v8::Object>& retVal)
{
    result_t hr;
    bool priv;

    hr = isPrivate(priv);
    if (hr < 0)
        return hr;

    Isolate* isolate = holder();
    v8::Local<v8::Context> context = isolate->context();
    mbedtls_pk_type_t type = mbedtls_pk_get_type(&m_key);

    if (type == MBEDTLS_PK_RSA) {
        mbedtls_rsa_context* rsa = mbedtls_pk_rsa(m_key);
        v8::Local<v8::Object> o = v8::Object::New(isolate->m_isolate);

        o->Set(context, isolate->NewString("kty"), isolate->NewString("RSA"));
        mpi_dump(isolate, o, "n", &rsa->N);
        mpi_dump(isolate, o, "e", &rsa->E);

        if (priv) {
            mpi_dump(isolate, o, "d", &rsa->D);
            mpi_dump(isolate, o, "p", &rsa->P);
            mpi_dump(isolate, o, "q", &rsa->Q);
            mpi_dump(isolate, o, "dp", &rsa->DP);
            mpi_dump(isolate, o, "dq", &rsa->DQ);
            mpi_dump(isolate, o, "qi", &rsa->QP);
        }

        retVal = o;
        return 0;
    }

    if (type == MBEDTLS_PK_ECKEY || type == MBEDTLS_PK_SM2) {
        mbedtls_ecp_keypair* ecp = mbedtls_pk_ec(m_key);
        v8::Local<v8::Object> o = v8::Object::New(isolate->m_isolate);
        const char* _name = get_curve_name(ecp->grp.id);

        o->Set(context, isolate->NewString("kty"), isolate->NewString(mbedtls_pk_get_name(&m_key)));

        if (_name)
            o->Set(context, isolate->NewString("crv"), isolate->NewString(_name));

        mpi_dump(isolate, o, "x", &ecp->Q.X);
        mpi_dump(isolate, o, "y", &ecp->Q.Y);

        if (priv)
            mpi_dump(isolate, o, "d", &ecp->d);

        retVal = o;
        return 0;
    }

    return 0;
}

result_t PKey::equal(PKey_base* key, bool& retVal)
{
    result_t hr;
    bool priv, priv1;

    retVal = false;

    hr = isPrivate(priv);
    if (hr < 0)
        return hr;

    hr = key->isPrivate(priv1);
    if (hr < 0)
        return hr;

    if (priv != priv1)
        return 0;

    obj_ptr<PKey> pkey = (PKey*)key;

    mbedtls_pk_type_t type = mbedtls_pk_get_type(&m_key);
    mbedtls_pk_type_t type1 = mbedtls_pk_get_type(&pkey->m_key);
    if (type != type1)
        return 0;

    if (type == MBEDTLS_PK_RSA) {
        mbedtls_rsa_context* rsa = mbedtls_pk_rsa(m_key);
        mbedtls_rsa_context* rsa1 = mbedtls_pk_rsa(pkey->m_key);

        if (mbedtls_mpi_cmp_mpi(&rsa->N, &rsa1->N)
            || mbedtls_mpi_cmp_mpi(&rsa->E, &rsa1->E))
            return 0;

        if (priv) {
            if (mbedtls_mpi_cmp_mpi(&rsa->D, &rsa1->D)
                || mbedtls_mpi_cmp_mpi(&rsa->P, &rsa1->P)
                || mbedtls_mpi_cmp_mpi(&rsa->Q, &rsa1->Q)
                || mbedtls_mpi_cmp_mpi(&rsa->DP, &rsa1->DP)
                || mbedtls_mpi_cmp_mpi(&rsa->DQ, &rsa1->DQ)
                || mbedtls_mpi_cmp_mpi(&rsa->QP, &rsa1->QP))
                return 0;
        }

        retVal = true;
        return 0;
    }

    if (type == MBEDTLS_PK_ECKEY || type == MBEDTLS_PK_SM2) {
        mbedtls_ecp_keypair* ecp = mbedtls_pk_ec(m_key);
        mbedtls_ecp_keypair* ecp1 = mbedtls_pk_ec(pkey->m_key);

        if (ecp->grp.id != ecp1->grp.id)
            return 0;

        if (mbedtls_mpi_cmp_mpi(&ecp->Q.X, &ecp1->Q.X)
            || mbedtls_mpi_cmp_mpi(&ecp->Q.Y, &ecp1->Q.Y))
            return 0;

        if (priv)
            if (mbedtls_mpi_cmp_mpi(&ecp->d, &ecp1->d))
                return 0;

        retVal = true;
        return 0;
    }

    return 0;
}

result_t PKey::encrypt(Buffer_base* data, obj_ptr<Buffer_base>& retVal,
    AsyncEvent* ac)
{
    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    int32_t ret;
    exlib::string str;
    exlib::string output;
    size_t olen;

    data->toString(str);
    output.resize(MBEDTLS_PREMASTER_SIZE);

    ret = mbedtls_pk_encrypt(&m_key, (const unsigned char*)str.c_str(), str.length(),
        (unsigned char*)output.c_buffer(), &olen, output.length(),
        mbedtls_ctr_drbg_random, &g_ssl.ctr_drbg);
    if (ret != 0)
        return CHECK_ERROR(_ssl::setError(ret));

    output.resize(olen);
    retVal = new Buffer(output);

    return 0;
}

result_t PKey::decrypt(Buffer_base* data, obj_ptr<Buffer_base>& retVal,
    AsyncEvent* ac)
{
    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    result_t hr;
    bool priv;

    hr = isPrivate(priv);
    if (hr < 0)
        return hr;

    if (!priv)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    int32_t ret;
    exlib::string str;
    exlib::string output;
    size_t olen;

    data->toString(str);
    output.resize(MBEDTLS_PREMASTER_SIZE * 2);

    ret = mbedtls_pk_decrypt(&m_key, (const unsigned char*)str.c_str(), str.length(),
        (unsigned char*)output.c_buffer(), &olen, output.length(),
        mbedtls_ctr_drbg_random, &g_ssl.ctr_drbg);
    if (ret != 0)
        return CHECK_ERROR(_ssl::setError(ret));

    output.resize(olen);
    retVal = new Buffer(output);

    return 0;
}

result_t PKey::sign(Buffer_base* data, int32_t alg, obj_ptr<Buffer_base>& retVal,
    AsyncEvent* ac)
{
    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    result_t hr;
    bool priv;

    hr = isPrivate(priv);
    if (hr < 0)
        return hr;

    if (!priv)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    int32_t ret;
    exlib::string str;
    exlib::string output;
    size_t olen;

    data->toString(str);
    output.resize(MBEDTLS_PREMASTER_SIZE);

    // alg=0~9  see https://tls.mbed.org/api/md_8h.html  enum mbedtls_md_type_t
    ret = mbedtls_pk_sign(&m_key, (mbedtls_md_type_t)alg,
        (const unsigned char*)str.c_str(), str.length(),
        (unsigned char*)output.c_buffer(), &olen,
        mbedtls_ctr_drbg_random, &g_ssl.ctr_drbg);
    if (ret != 0)
        return CHECK_ERROR(_ssl::setError(ret));

    output.resize(olen);
    retVal = new Buffer(output);

    return 0;
}

result_t PKey::verify(Buffer_base* data, Buffer_base* sign,
    int32_t alg, bool& retVal, AsyncEvent* ac)
{
    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    int32_t ret;
    exlib::string str;
    exlib::string strsign;

    data->toString(str);
    sign->toString(strsign);

    ret = mbedtls_pk_verify(&m_key, (mbedtls_md_type_t)alg,
        (const unsigned char*)str.c_str(), str.length(),
        (const unsigned char*)strsign.c_str(), strsign.length());
    if (ret == MBEDTLS_ERR_ECP_VERIFY_FAILED || ret == MBEDTLS_ERR_RSA_VERIFY_FAILED || ret == MBEDTLS_ERR_SM2_BAD_SIGNATURE) {
        retVal = false;
        return 0;
    }

    if (ret != 0)
        return CHECK_ERROR(_ssl::setError(ret));

    retVal = true;

    return 0;
}

result_t PKey::computeSecret(PKey_base* publicKey, obj_ptr<Buffer_base>& retVal, AsyncEvent* ac)
{
    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    result_t hr;
    bool priv;
    mbedtls_pk_type_t type;

    obj_ptr<PKey> pubkey = (PKey*)publicKey;
    type = mbedtls_pk_get_type(&pubkey->m_key);
    if (type != MBEDTLS_PK_ECKEY && type != MBEDTLS_PK_SM2)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    type = mbedtls_pk_get_type(&m_key);
    if (type != MBEDTLS_PK_ECKEY && type != MBEDTLS_PK_SM2)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    hr = isPrivate(priv);
    if (hr < 0)
        return hr;

    if (!priv)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    mbedtls_ecp_keypair* ecp1 = mbedtls_pk_ec(m_key);
    mbedtls_ecp_keypair* ecp2 = mbedtls_pk_ec(pubkey->m_key);
    if (ecp1->grp.id != ecp2->grp.id)
        return CHECK_ERROR(Runtime::setError("Public key is not valid for specified curve"));

    int32_t ret;
    mbedtls_mpi z;
    mbedtls_mpi_init(&z);
    ret = mbedtls_ecdh_compute_shared(&ecp1->grp, &z, &ecp2->Q, &ecp1->d,
        mbedtls_ctr_drbg_random, &g_ssl.ctr_drbg);
    if (ret != 0)
        return CHECK_ERROR(_ssl::setError(ret));

    exlib::string data;
    int32_t sz = (int32_t)mbedtls_mpi_size(&z);

    data.resize(sz);
    mbedtls_mpi_write_binary(&z, (unsigned char*)data.c_buffer(), sz);
    mbedtls_mpi_free(&z);

    retVal = new Buffer(data);

    return 0;
}

result_t PKey::get_name(exlib::string& retVal)
{
    retVal = mbedtls_pk_get_name(&m_key);
    return 0;
}

result_t PKey::get_keySize(int32_t& retVal)
{
    retVal = (int32_t)mbedtls_pk_get_bitlen(&m_key);
    return 0;
}

result_t PKey::get_curve(exlib::string& retVal)
{
    mbedtls_pk_type_t type = mbedtls_pk_get_type(&m_key);
    if (type == MBEDTLS_PK_ECKEY || type == MBEDTLS_PK_SM2) {
        mbedtls_ecp_keypair* ecp = mbedtls_pk_ec(m_key);
        const char* _name = get_curve_name(ecp->grp.id);
        if (_name)
            retVal = _name;
    }

    return 0;
}

result_t PKey::toString(exlib::string& retVal)
{
    return exportPem(retVal);
}

result_t PKey::toJSON(exlib::string key, v8::Local<v8::Value>& retVal)
{
    v8::Local<v8::Object> o;

    result_t hr = exportJson(o);
    retVal = o;

    return hr;
}
}
