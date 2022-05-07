/*
 * PKey_rsa.cpp
 *
 *  Created on: May 2, 2022
 *      Author: lion
 */

#include "Buffer.h"
#include "PKey_impl.h"
#include "ssl.h"

namespace fibjs {

PKey_rsa::PKey_rsa(mbedtls_pk_context& key)
    : PKey(key)
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

result_t PKey_rsa::isPrivate(bool& retVal)
{
    mbedtls_rsa_context* rsa = mbedtls_pk_rsa(m_key);
    retVal = mbedtls_mpi_cmp_int(&rsa->D, 0)
        && mbedtls_mpi_cmp_int(&rsa->P, 0)
        && mbedtls_mpi_cmp_int(&rsa->Q, 0)
        && mbedtls_mpi_cmp_int(&rsa->DP, 0)
        && mbedtls_mpi_cmp_int(&rsa->DQ, 0)
        && mbedtls_mpi_cmp_int(&rsa->QP, 0);
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

    obj_ptr<PKey> pkey = (PKey*)key;

    mbedtls_pk_type_t type1 = mbedtls_pk_get_type(&pkey->m_key);
    if (MBEDTLS_PK_RSA != type1)
        return 0;

    mbedtls_rsa_context* rsa = mbedtls_pk_rsa(m_key);
    mbedtls_rsa_context* rsa1 = mbedtls_pk_rsa(pkey->m_key);

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
}
