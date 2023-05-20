/*
 * PKey_rsa.cpp
 *
 *  Created on: May 2, 2022
 *      Author: lion
 */

#include "Buffer.h"
#include "PKey_rsa.h"
#include "ssl.h"

namespace fibjs {

PKey_rsa::PKey_rsa(mbedtls_pk_context& key)
    : PKey_impl<PKey_base>(key)
{
    m_alg = "RSA";
}

PKey_rsa::PKey_rsa(int32_t size)
{
    m_alg = "RSA";
    mbedtls_pk_setup(&m_key, mbedtls_pk_info_from_type(MBEDTLS_PK_RSA));
    mbedtls_rsa_gen_key(mbedtls_pk_rsa(m_key), mbedtls_ctr_drbg_random,
        &g_ssl.ctr_drbg, size, 65537);
}

result_t PKey_rsa::get_publicKey(obj_ptr<PKey_base>& retVal)
{
    result_t hr;
    bool priv;

    hr = isPrivate(priv);
    if (hr < 0)
        return hr;

    if (!priv)
        return CALL_RETURN_NULL;

    mbedtls_pk_context ctx;

    mbedtls_pk_init(&ctx);
    mbedtls_pk_setup(&ctx, m_key.pk_info);

    mbedtls_rsa_context* rsa = mbedtls_pk_rsa(m_key);
    mbedtls_rsa_context* rsa1 = mbedtls_pk_rsa(ctx);

    mbedtls_mpi_copy(&rsa1->N, &rsa->N);
    mbedtls_mpi_copy(&rsa1->E, &rsa->E);

    rsa1->len = mbedtls_mpi_size(&rsa1->N);

    retVal = new PKey_rsa(ctx);

    return 0;
}

result_t PKey_rsa::clone(obj_ptr<PKey_base>& retVal)
{
    mbedtls_pk_context ctx;

    mbedtls_pk_init(&ctx);
    mbedtls_pk_setup(&ctx, m_key.pk_info);

    mbedtls_rsa_context* rsa = mbedtls_pk_rsa(m_key);
    mbedtls_rsa_context* rsa1 = mbedtls_pk_rsa(ctx);

    mbedtls_rsa_copy(rsa1, rsa);

    retVal = new PKey_rsa(ctx);

    return 0;
}

result_t PKey_rsa::equals(PKey_base* key, bool& retVal)
{
    retVal = false;

    mbedtls_pk_context& mkey = PKey::key(key);

    mbedtls_pk_type_t type1 = mbedtls_pk_get_type(&mkey);
    if (MBEDTLS_PK_RSA != type1)
        return 0;

    mbedtls_rsa_context* rsa = mbedtls_pk_rsa(m_key);
    mbedtls_rsa_context* rsa1 = mbedtls_pk_rsa(mkey);

    if (mbedtls_mpi_cmp_mpi(&rsa->N, &rsa1->N)
        || mbedtls_mpi_cmp_mpi(&rsa->E, &rsa1->E)
        || mbedtls_mpi_cmp_mpi(&rsa->D, &rsa1->D)
        || mbedtls_mpi_cmp_mpi(&rsa->P, &rsa1->P)
        || mbedtls_mpi_cmp_mpi(&rsa->Q, &rsa1->Q)
        || mbedtls_mpi_cmp_mpi(&rsa->DP, &rsa1->DP)
        || mbedtls_mpi_cmp_mpi(&rsa->DQ, &rsa1->DQ)
        || mbedtls_mpi_cmp_mpi(&rsa->QP, &rsa1->QP))
        return 0;

    retVal = true;
    return 0;
}

result_t PKey_rsa::generateKey(int32_t size, obj_ptr<PKey_base>& retVal)
{
    if (size < 128 || size > 8192)
        return CHECK_ERROR(Runtime::setError("PKey: Invalid key size"));

    retVal = new PKey_rsa(size);

    return 0;
}

result_t PKey_rsa::check_opts(v8::Local<v8::Object> opts, AsyncEvent* ac)
{
    static const char* s_keys[] = {
        "alg", "format", NULL
    };

    if (!ac->isSync())
        return 0;

    Isolate* isolate = holder();
    result_t hr;

    hr = CheckConfig(opts, s_keys);
    if (hr < 0)
        return hr;

    exlib::string fmt = "raw";
    hr = GetConfigValue(isolate->m_isolate, opts, "format", fmt, true);
    if (hr < 0 && hr != CALL_E_PARAMNOTOPTIONAL)
        return hr;
    if (fmt != "raw")
        return CHECK_ERROR(Runtime::setError(exlib::string("unsupported format \'") + fmt + "\'."));

    ac->m_ctx.resize(1);

    int32_t alg = 0;
    hr = GetConfigValue(isolate->m_isolate, opts, "alg", alg, true);
    if (hr < 0 && hr != CALL_E_PARAMNOTOPTIONAL)
        return hr;
    ac->m_ctx[0] = alg;

    return CHECK_ERROR(CALL_E_NOSYNC);
}

result_t PKey_rsa::sign(Buffer_base* data, v8::Local<v8::Object> opts, obj_ptr<Buffer_base>& retVal, AsyncEvent* ac)
{
    result_t hr = check_opts(opts, ac);
    if (hr < 0)
        return hr;

    bool priv;

    hr = isPrivate(priv);
    if (hr < 0)
        return hr;

    if (!priv)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    int32_t alg = ac->m_ctx[0].intVal();

    int32_t ret;
    exlib::string output;
    size_t olen = (mbedtls_pk_get_bitlen(&m_key) + 7) / 8;

    obj_ptr<Buffer> buf_data = Buffer::Cast(data);
    output.resize(olen);

    // alg=0~9  see https://tls.mbed.org/api/md_8h.html  enum mbedtls_md_type_t
    ret = mbedtls_pk_sign(&m_key, (mbedtls_md_type_t)alg, buf_data->data(), buf_data->length(),
        (unsigned char*)output.c_buffer(), olen, &olen, mbedtls_ctr_drbg_random, &g_ssl.ctr_drbg);
    if (ret != 0)
        return CHECK_ERROR(_ssl::setError(ret));

    retVal = new Buffer(output.c_str(), olen);

    return 0;
}

result_t PKey_rsa::verify(Buffer_base* data, Buffer_base* sign, v8::Local<v8::Object> opts, bool& retVal, AsyncEvent* ac)
{
    result_t hr = check_opts(opts, ac);
    if (hr < 0)
        return hr;

    int32_t alg = ac->m_ctx[0].intVal();

    int32_t ret;

    obj_ptr<Buffer> buf_data = Buffer::Cast(data);
    obj_ptr<Buffer> buf_sign = Buffer::Cast(sign);

    ret = mbedtls_pk_verify(&m_key, (mbedtls_md_type_t)alg, buf_data->data(), buf_data->length(),
        buf_sign->data(), buf_sign->length());
    if (ret == MBEDTLS_ERR_RSA_VERIFY_FAILED) {
        retVal = false;
        return 0;
    }

    if (ret != 0)
        return CHECK_ERROR(_ssl::setError(ret));

    retVal = true;

    return 0;
}

}
