/*
 * PKey.cpp
 *
 *  Created on: Apr 11, 2014
 *      Author: lion
 */

#include "ifs/fs.h"
#include "ifs/crypto.h"
#include "PKey.h"
#include "Cipher.h"
#include "Buffer.h"
#include "ssl.h"

namespace fibjs
{

result_t PKey_base::_new(obj_ptr<PKey_base> &retVal, v8::Local<v8::Object> This)
{
    retVal = new PKey();
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

result_t PKey::genRsaKey(int32_t size, AsyncEvent *ac)
{
    if (size < 128 || size > 8192)
        return CHECK_ERROR(Runtime::setError("PKey: Invalid key size"));

    if (!ac)
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

result_t PKey::genEcKey(const char *curve, AsyncEvent *ac)
{
    if (!ac)
        return CHECK_ERROR(CALL_E_NOSYNC);

    const mbedtls_ecp_curve_info *curve_info;
    curve_info = mbedtls_ecp_curve_info_from_name(curve);
    if (curve_info == NULL)
        return CHECK_ERROR(Runtime::setError("PKey: Unknown curve"));

    int32_t ret;

    clear();

    ret = mbedtls_pk_setup(&m_key, mbedtls_pk_info_from_type(MBEDTLS_PK_ECKEY));
    if (ret != 0)
        return CHECK_ERROR(_ssl::setError(ret));

    ret = mbedtls_ecp_gen_key(curve_info->grp_id, mbedtls_pk_ec(m_key),
                              mbedtls_ctr_drbg_random, &g_ssl.ctr_drbg);

    if (ret != 0)
        return CHECK_ERROR(_ssl::setError(ret));

    return 0;
}

result_t PKey::isPrivate(bool &retVal)
{
    mbedtls_pk_type_t type = mbedtls_pk_get_type(&m_key);

    if (type == MBEDTLS_PK_RSA)
    {
        retVal = mbedtls_rsa_check_privkey(mbedtls_pk_rsa(m_key)) == 0;
        return 0;
    }

    if (type == MBEDTLS_PK_ECKEY)
    {
        mbedtls_ecp_keypair *ecp = mbedtls_pk_ec(m_key);
        retVal = mbedtls_ecp_check_privkey(&ecp->grp, &ecp->d) == 0;
        return 0;
    }

    return CHECK_ERROR(CALL_E_INVALID_CALL);
}

result_t PKey::get_publicKey(obj_ptr<PKey_base> &retVal)
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

    if (type == MBEDTLS_PK_RSA)
    {
        mbedtls_rsa_context *rsa = mbedtls_pk_rsa(m_key);
        obj_ptr<PKey> pk1 = new PKey();

        ret = mbedtls_pk_setup(&pk1->m_key, mbedtls_pk_info_from_type(MBEDTLS_PK_RSA));
        if (ret != 0)
            return CHECK_ERROR(_ssl::setError(ret));

        mbedtls_rsa_context *rsa1 = mbedtls_pk_rsa(pk1->m_key);

        rsa1->len = rsa->len;
        rsa1->padding = rsa->padding;
        rsa1->hash_id = rsa->hash_id;

        ret = mbedtls_mpi_copy(&rsa1->N, &rsa->N);
        if (ret != 0)
            return CHECK_ERROR(_ssl::setError(ret));

        ret = mbedtls_mpi_copy(&rsa1->E, &rsa->E);
        if (ret != 0)
            return CHECK_ERROR(_ssl::setError(ret));

        retVal = pk1;

        return 0;
    }

    if (type == MBEDTLS_PK_ECKEY)
    {
        mbedtls_ecp_keypair *ecp = mbedtls_pk_ec(m_key);

        obj_ptr<PKey> pk1 = new PKey();

        ret = mbedtls_pk_setup(&pk1->m_key, mbedtls_pk_info_from_type(MBEDTLS_PK_ECKEY));
        if (ret != 0)
            return CHECK_ERROR(_ssl::setError(ret));

        mbedtls_ecp_keypair *ecp1 = mbedtls_pk_ec(pk1->m_key);

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

result_t PKey::copy(const mbedtls_pk_context &key)
{
    mbedtls_pk_type_t type = mbedtls_pk_get_type(&key);
    int32_t ret;

    if (type == MBEDTLS_PK_RSA)
    {
        mbedtls_rsa_context *rsa = mbedtls_pk_rsa(key);

        ret = mbedtls_pk_setup(&m_key, mbedtls_pk_info_from_type(MBEDTLS_PK_RSA));
        if (ret != 0)
            return CHECK_ERROR(_ssl::setError(ret));

        mbedtls_rsa_context *rsa1 = mbedtls_pk_rsa(m_key);

        ret = mbedtls_rsa_copy(rsa1, rsa);
        if (ret != 0)
            return CHECK_ERROR(_ssl::setError(ret));

        return 0;
    }

    if (type == MBEDTLS_PK_ECKEY)
    {
        mbedtls_ecp_keypair *ecp = mbedtls_pk_ec(key);

        ret = mbedtls_pk_setup(&m_key, mbedtls_pk_info_from_type(MBEDTLS_PK_ECKEY));
        if (ret != 0)
            return CHECK_ERROR(_ssl::setError(ret));

        mbedtls_ecp_keypair *ecp1 = mbedtls_pk_ec(m_key);

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

result_t PKey::clone(obj_ptr<PKey_base> &retVal)
{
    obj_ptr<PKey> pk1 = new PKey();
    result_t hr;

    hr = pk1->copy(m_key);
    if (hr < 0)
        return 0;

    retVal = pk1;
    return 0;
}

result_t PKey::importKey(Buffer_base *DerKey, const char *password)
{
    int32_t ret;

    qstring key;
    DerKey->toString(key);

    clear();

    ret = mbedtls_pk_parse_key(&m_key, (unsigned char *)key.c_str(), key.length() + 1,
                               *password ? (unsigned char *)password : NULL,
                               qstrlen(password));

    if (ret == MBEDTLS_ERR_PK_KEY_INVALID_FORMAT)
        ret = mbedtls_pk_parse_public_key(&m_key, (unsigned char *)key.c_str(), key.length() + 1);

    if (ret != 0)
        return CHECK_ERROR(_ssl::setError(ret));

    return 0;
}

result_t PKey::importKey(const char *pemKey, const char *password)
{
    int32_t ret;

    clear();

    ret = mbedtls_pk_parse_key(&m_key, (unsigned char *)pemKey, qstrlen(pemKey) + 1,
                               *password ? (unsigned char *)password : NULL,
                               qstrlen(password) );

    if (ret == MBEDTLS_ERR_PK_KEY_INVALID_FORMAT)
        ret = mbedtls_pk_parse_public_key(&m_key, (unsigned char *)pemKey, qstrlen(pemKey) + 1);

    if (ret != 0)
        return CHECK_ERROR(_ssl::setError(ret));

    return 0;
}

result_t PKey::importFile(const char* filename, const char* password)
{
    result_t hr;
    qstring data;
    int32_t ret;

    hr = fs_base::ac_readFile(filename, data);
    if (hr < 0)
        return hr;

    clear();

    ret = mbedtls_pk_parse_key(&m_key, (const unsigned char *)data.c_str(),
                               data.length() + 1, *password ? (unsigned char *)password : NULL,
                               qstrlen(password));

    if (ret == MBEDTLS_ERR_PK_KEY_INVALID_FORMAT)
        ret = mbedtls_pk_parse_public_key(&m_key, (const unsigned char *)data.c_str(),
                                          data.length() + 1);

    if (ret != 0)
        return CHECK_ERROR(_ssl::setError(ret));

    return 0;
}

result_t PKey::exportPem(qstring &retVal)
{
    result_t hr;
    bool priv;

    hr = isPrivate(priv);
    if (hr < 0)
        return hr;

    int32_t ret;
    qstring buf;

    buf.resize(mbedtls_pk_get_len(&m_key) * 8 + 128);
    if (priv)
        ret = mbedtls_pk_write_key_pem(&m_key, (unsigned char *)&buf[0], buf.length());
    else
        ret = mbedtls_pk_write_pubkey_pem(&m_key, (unsigned char *)&buf[0], buf.length());
    if (ret != 0)
        return CHECK_ERROR(_ssl::setError(ret));

    buf.resize(qstrlen(buf.c_str()));
    retVal = buf;

    return 0;
}

result_t PKey::exportDer(obj_ptr<Buffer_base> &retVal)
{
    result_t hr;
    bool priv;

    hr = isPrivate(priv);
    if (hr < 0)
        return hr;

    int32_t ret;
    qstring buf;

    buf.resize(8192);
    if (priv)
        ret = mbedtls_pk_write_key_der(&m_key, (unsigned char *)&buf[0], buf.length());
    else
        ret = mbedtls_pk_write_pubkey_der(&m_key, (unsigned char *)&buf[0], buf.length());
    if (ret < 0)
        return CHECK_ERROR(_ssl::setError(ret));

    retVal = new Buffer(buf.substr(buf.length() - ret));

    return 0;
}

result_t PKey::encrypt(Buffer_base *data, obj_ptr<Buffer_base> &retVal,
                       AsyncEvent *ac)
{
    if (!ac)
        return CHECK_ERROR(CALL_E_NOSYNC);

    int32_t ret;
    qstring str;
    qstring output;
    size_t olen;

    data->toString(str);
    output.resize(MBEDTLS_PREMASTER_SIZE);

    ret = mbedtls_pk_encrypt(&m_key, (const unsigned char *)str.c_str(), str.length(),
                             (unsigned char *)&output[0], &olen, output.length(),
                             mbedtls_ctr_drbg_random, &g_ssl.ctr_drbg);
    if (ret != 0)
        return CHECK_ERROR(_ssl::setError(ret));

    output.resize(olen);
    retVal = new Buffer(output);

    return 0;
}

result_t PKey::decrypt(Buffer_base *data, obj_ptr<Buffer_base> &retVal,
                       AsyncEvent *ac)
{
    if (!ac)
        return CHECK_ERROR(CALL_E_NOSYNC);

    result_t hr;
    bool priv;

    hr = isPrivate(priv);
    if (hr < 0)
        return hr;

    if (!priv)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    int32_t ret;
    qstring str;
    qstring output;
    size_t olen;

    data->toString(str);
    output.resize(MBEDTLS_PREMASTER_SIZE * 2);

    ret = mbedtls_pk_decrypt(&m_key, (const unsigned char *)str.c_str(), str.length(),
                             (unsigned char *)&output[0], &olen, output.length(),
                             mbedtls_ctr_drbg_random, &g_ssl.ctr_drbg);
    if (ret != 0)
        return CHECK_ERROR(_ssl::setError(ret));

    output.resize(olen);
    retVal = new Buffer(output);

    return 0;
}

result_t PKey::sign(Buffer_base *data, int32_t alg, obj_ptr<Buffer_base> &retVal,
                    AsyncEvent *ac)
{
    if (!ac)
        return CHECK_ERROR(CALL_E_NOSYNC);

    result_t hr;
    bool priv;

    hr = isPrivate(priv);
    if (hr < 0)
        return hr;

    if (!priv)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    int32_t ret;
    qstring str;
    qstring output;
    size_t olen;

    data->toString(str);
    output.resize(MBEDTLS_PREMASTER_SIZE);

    //alg=0~9  see https://tls.mbed.org/api/md_8h.html  enum mbedtls_md_type_t
    ret = mbedtls_pk_sign(&m_key, (mbedtls_md_type_t)alg,
                          (const unsigned char *)str.c_str(), str.length(),
                          (unsigned char *)&output[0], &olen,
                          mbedtls_ctr_drbg_random, &g_ssl.ctr_drbg);
    if (ret != 0)
        return CHECK_ERROR(_ssl::setError(ret));

    output.resize(olen);
    retVal = new Buffer(output);

    return 0;
}

result_t PKey::verify(Buffer_base *sign, Buffer_base *data, bool &retVal,
                      AsyncEvent *ac)
{
    if (!ac)
        return CHECK_ERROR(CALL_E_NOSYNC);

    int32_t ret;
    qstring str;
    qstring strsign;

    data->toString(str);
    sign->toString(strsign);

    ret = mbedtls_pk_verify(&m_key, MBEDTLS_MD_NONE,
                            (const unsigned char *)str.c_str(), str.length(),
                            (const unsigned char *)strsign.c_str(), strsign.length());
    if (ret == MBEDTLS_ERR_ECP_VERIFY_FAILED ||
            ret == MBEDTLS_ERR_RSA_VERIFY_FAILED)
    {
        retVal = false;
        return 0;
    }

    if (ret != 0)
        return CHECK_ERROR(_ssl::setError(ret));

    retVal = true;

    return 0;
}

result_t PKey::get_name(qstring &retVal)
{
    retVal = mbedtls_pk_get_name(&m_key);
    return 0;
}

result_t PKey::get_keySize(int32_t &retVal)
{
    retVal = (int32_t)mbedtls_pk_get_bitlen(&m_key);
    return 0;
}

result_t PKey::toString(qstring &retVal)
{
    return exportPem(retVal);
}

}
