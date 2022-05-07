/*
 * PKey.cpp
 *
 *  Created on: Apr 11, 2014
 *      Author: lion
 */

#define MBEDTLS_ALLOW_PRIVATE_ACCESS

#include "object.h"
#include "ifs/fs.h"
#include "ifs/crypto.h"
#include "PKey.h"
#include "PKey_impl.h"
#include "Cipher.h"
#include "Buffer.h"
#include "ssl.h"
#include "encoding.h"

namespace fibjs {

static int ecp_group_load(mbedtls_ecp_group* grp, int32_t id)
{
    if (id >= MBEDTLS_ECP_DP_ED25519) {
        grp->id = (mbedtls_ecp_group_id)id;
        return 0;
    }

    return mbedtls_ecp_group_load(grp, (mbedtls_ecp_group_id)id);
}

PKey* PKey::create(mbedtls_pk_context& key, bool clone)
{
    mbedtls_pk_context key1;
    mbedtls_pk_context& cur = clone ? key1 : key;
    mbedtls_pk_type_t type = mbedtls_pk_get_type(&key);

    if (clone) {
        mbedtls_pk_init(&key1);
        mbedtls_pk_setup(&key1, key.pk_info);

        if (type == MBEDTLS_PK_RSA) {
            mbedtls_rsa_context* rsa = mbedtls_pk_rsa(key);
            mbedtls_rsa_context* rsa1 = mbedtls_pk_rsa(key1);

            mbedtls_rsa_copy(rsa1, rsa);
        } else {
            mbedtls_ecp_keypair* ecp = mbedtls_pk_ec(key);
            mbedtls_ecp_keypair* ecp1 = mbedtls_pk_ec(key1);

            ecp_group_load(&ecp1->grp, ecp->grp.id);
            mbedtls_mpi_copy(&ecp1->d, &ecp->d);
            mbedtls_ecp_copy(&ecp1->Q, &ecp->Q);
        }
    }

    if (type == MBEDTLS_PK_RSA)
        return new PKey_rsa(cur);

    return PKey_ecc::create(cur);
}

PKey::PKey()
{
    mbedtls_pk_init(&m_key);
}

PKey::PKey(mbedtls_pk_context& key)
{
    m_key.pk_info = key.pk_info;
    key.pk_info = NULL;

    m_key.pk_ctx = key.pk_ctx;
    key.pk_ctx = NULL;
}

PKey::~PKey()
{
    mbedtls_pk_free(&m_key);
}

result_t PKey::isPrivate(bool& retVal)
{
    return CHECK_ERROR(CALL_E_INVALID_CALL);
}

result_t PKey::get_publicKey(obj_ptr<PKey_base>& retVal)
{
    return CHECK_ERROR(CALL_E_INVALID_CALL);
}

result_t PKey::clone(obj_ptr<PKey_base>& retVal)
{
    return CHECK_ERROR(CALL_E_INVALID_CALL);
}

result_t PKey::equals(PKey_base* key, bool& retVal)
{
    return CHECK_ERROR(CALL_E_INVALID_CALL);
}

result_t PKey::encrypt(Buffer_base* data, obj_ptr<Buffer_base>& retVal, AsyncEvent* ac)
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

result_t PKey::decrypt(Buffer_base* data, obj_ptr<Buffer_base>& retVal, AsyncEvent* ac)
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

result_t PKey::sign(Buffer_base* data, int32_t alg, obj_ptr<Buffer_base>& retVal, AsyncEvent* ac)
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
    size_t olen = MBEDTLS_PREMASTER_SIZE;

    data->toString(str);
    output.resize(MBEDTLS_PREMASTER_SIZE);

    // alg=0~9  see https://tls.mbed.org/api/md_8h.html  enum mbedtls_md_type_t
    ret = mbedtls_pk_sign(&m_key, (mbedtls_md_type_t)alg,
        (const unsigned char*)str.c_str(), str.length(),
        (unsigned char*)output.c_buffer(), olen, &olen,
        mbedtls_ctr_drbg_random, &g_ssl.ctr_drbg);
    if (ret != 0)
        return CHECK_ERROR(_ssl::setError(ret));

    output.resize(olen);
    retVal = new Buffer(output);

    return 0;
}

result_t PKey::sign(Buffer_base* data, PKey_base* key, obj_ptr<Buffer_base>& retVal, AsyncEvent* ac)
{
    return CHECK_ERROR(CALL_E_INVALID_CALL);
}

result_t PKey::verify(Buffer_base* data, Buffer_base* sign, int32_t alg, bool& retVal, AsyncEvent* ac)
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

result_t PKey::verify(Buffer_base* data, Buffer_base* sign, PKey_base* key, bool& retVal, AsyncEvent* ac)
{
    return CHECK_ERROR(CALL_E_INVALID_CALL);
}

result_t PKey::computeSecret(PKey_base* publicKey, obj_ptr<Buffer_base>& retVal, AsyncEvent* ac)
{
    return CHECK_ERROR(CALL_E_INVALIDARG);
}

result_t PKey::get_name(exlib::string& retVal)
{
    retVal = mbedtls_pk_get_name(&m_key);
    return 0;
}

result_t PKey::get_curve(exlib::string& retVal)
{
    return 0;
}

result_t PKey::get_keySize(int32_t& retVal)
{
    retVal = (int32_t)mbedtls_pk_get_bitlen(&m_key);
    return 0;
}

result_t PKey::get_alg(exlib::string& retVal)
{
    retVal = m_alg;
    return 0;
}

result_t PKey::set_alg(exlib::string newVal)
{
    if (newVal != m_alg)
        return CHECK_ERROR(CALL_E_INVALIDARG);
    return 0;
}

result_t PKey::toString(exlib::string& retVal)
{
    return pem(retVal);
}

result_t PKey::toJSON(exlib::string key, v8::Local<v8::Value>& retVal)
{
    v8::Local<v8::Object> o;

    result_t hr = json(o);
    retVal = o;

    return hr;
}
}
