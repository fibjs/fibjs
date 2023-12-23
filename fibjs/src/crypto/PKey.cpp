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
#include "ECKey.h"
#include "PKey_rsa.h"
#include "Cipher.h"
#include "Buffer.h"
#include "ssl.h"
#include "encoding.h"

namespace fibjs {

result_t PKey_base::_new(Buffer_base* DerKey, exlib::string password, obj_ptr<PKey_base>& retVal,
    v8::Local<v8::Object> This)
{
    result_t hr = from(DerKey, password, retVal);
    if (hr >= 0)
        retVal->wrap();

    return hr;
}

result_t PKey_base::_new(exlib::string pemKey, exlib::string password, obj_ptr<PKey_base>& retVal,
    v8::Local<v8::Object> This)
{
    result_t hr = from(pemKey, password, retVal);
    if (hr >= 0)
        retVal->wrap();

    return hr;
}

result_t PKey_base::_new(v8::Local<v8::Object> jsonKey, obj_ptr<PKey_base>& retVal,
    v8::Local<v8::Object> This)
{
    result_t hr = from(jsonKey, retVal);
    if (hr >= 0)
        retVal->wrap();

    return hr;
}

result_t PKey::loadFile(exlib::string filename, obj_ptr<PKey_base>& retVal)
{
    result_t hr;
    exlib::string data;
    obj_ptr<Buffer> buf;

    hr = fs_base::ac_readTextFile(filename, data);
    if (hr < 0)
        return hr;

    if (qstrstr(data.c_str(), "BEGIN"))
        return PKey_base::_new(data, "", retVal, v8::Local<v8::Object>());

    buf = new Buffer(data.c_str(), data.length());
    return PKey_base::_new(buf, "", retVal, v8::Local<v8::Object>());
}

PKey_base* PKey::create(mbedtls_pk_context& key, bool clone)
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

            ECKey::load_group(&ecp1->grp, ecp->grp.id);
            mbedtls_mpi_copy(&ecp1->d, &ecp->d);
            mbedtls_ecp_copy(&ecp1->Q, &ecp->Q);
        }
    }

    if (type == MBEDTLS_PK_RSA)
        return new PKey_rsa(cur);

    return ECKey::create(cur, "");
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

mbedtls_pk_context& PKey::key(PKey_base* key)
{
    PKey* p = dynamic_cast<PKey*>(key);
    return p->m_key;
}

result_t PKey::isPrivate(bool& retVal)
{
    mbedtls_pk_type_t type = mbedtls_pk_get_type(&m_key);

    if (type == MBEDTLS_PK_RSA) {
        mbedtls_rsa_context* rsa = mbedtls_pk_rsa(m_key);
        retVal = mbedtls_mpi_cmp_int(&rsa->D, 0)
            && mbedtls_mpi_cmp_int(&rsa->P, 0)
            && mbedtls_mpi_cmp_int(&rsa->Q, 0)
            && mbedtls_mpi_cmp_int(&rsa->DP, 0)
            && mbedtls_mpi_cmp_int(&rsa->DQ, 0)
            && mbedtls_mpi_cmp_int(&rsa->QP, 0);
    } else {
        mbedtls_ecp_keypair* ecp = mbedtls_pk_ec(m_key);
        retVal = mbedtls_mpi_cmp_int(&ecp->d, 0);
    }

    return 0;
}

result_t PKey::get_publicKey(obj_ptr<PKey_base>& retVal)
{
    return CHECK_ERROR(CALL_E_INVALID_CALL);
}

result_t PKey::clone(obj_ptr<PKey_base>& retVal)
{
    return CHECK_ERROR(CALL_E_INVALID_CALL);
}

result_t PKey::equals(object_base* key, bool& retVal)
{
    return CHECK_ERROR(CALL_E_INVALID_CALL);
}

result_t PKey::encrypt(Buffer_base* data, obj_ptr<Buffer_base>& retVal, AsyncEvent* ac)
{
    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    int32_t ret;
    exlib::string output;
    size_t olen;

    Buffer* buf_data = Buffer::Cast(data);
    output.resize(MBEDTLS_PREMASTER_SIZE);

    ret = mbedtls_pk_encrypt(&m_key, buf_data->data(), buf_data->length(),
        (unsigned char*)output.data(), &olen, output.length(),
        mbedtls_ctr_drbg_random, &g_ssl.ctr_drbg);
    if (ret != 0)
        return CHECK_ERROR(_ssl::setError(ret));

    retVal = new Buffer(output.c_str(), olen);

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
    exlib::string output;
    size_t olen;

    Buffer* buf_data = Buffer::Cast(data);
    output.resize(MBEDTLS_PREMASTER_SIZE * 2);

    ret = mbedtls_pk_decrypt(&m_key, buf_data->data(), buf_data->length(),
        (unsigned char*)output.data(), &olen, output.length(),
        mbedtls_ctr_drbg_random, &g_ssl.ctr_drbg);
    if (ret != 0)
        return CHECK_ERROR(_ssl::setError(ret));

    retVal = new Buffer(output.c_str(), olen);

    return 0;
}

result_t PKey::sign(Buffer_base* data, v8::Local<v8::Object> opt, obj_ptr<Buffer_base>& retVal, AsyncEvent* ac)
{
    return CHECK_ERROR(CALL_E_INVALID_CALL);
}

result_t PKey::verify(Buffer_base* data, Buffer_base* sign, v8::Local<v8::Object> opt, bool& retVal, AsyncEvent* ac)
{
    return CHECK_ERROR(CALL_E_INVALID_CALL);
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

result_t PKey::get_alg(exlib::string& retVal)
{
    retVal = m_alg;
    return 0;
}

result_t PKey::toString(exlib::string& retVal)
{
    return pem(retVal);
}

result_t PKey::toJSON(exlib::string key, v8::Local<v8::Value>& retVal)
{
    v8::Local<v8::Object> o;

    result_t hr = json(v8::Local<v8::Object>(), o);
    retVal = o;

    return hr;
}
}
