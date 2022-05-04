/*
 * PKey_bls.cpp
 *
 *  Created on: May 1, 2022
 *      Author: lion
 */

#include "Buffer.h"
#include "PKey_impl.h"
#include "ssl.h"
#include <blst/include/blst.h>

namespace fibjs {

const unsigned char DST_G2_NUL[] = "BLS_SIG_BLS12381G1_XMD:SHA-256_SSWU_RO_NUL_";
const unsigned char DST_G2_AUG[] = "BLS_SIG_BLS12381G1_XMD:SHA-256_SSWU_RO_AUG_";
const unsigned char DST_G2_POP[] = "BLS_SIG_BLS12381G1_XMD:SHA-256_SSWU_RO_POP_";

PKey_bls_g2::PKey_bls_g2(mbedtls_pk_context& key)
    : PKey_ecc(key)
{
    m_alg = "BLS";

    mbedtls_ecp_keypair* ecp = mbedtls_pk_ec(m_key);

    if (mbedtls_mpi_cmp_int(&ecp->d, 0) && !mbedtls_mpi_cmp_int(&ecp->Q.X, 0)) {
        unsigned char k[96];
        blst_scalar sk;
        mbedtls_ecp_keypair* ecp = mbedtls_pk_ec(m_key);

        mbedtls_mpi_write_binary(&ecp->d, k, 32);
        blst_scalar_from_bendian(&sk, k);

        blst_p2 pk;

        blst_sk_to_pk_in_g2(&pk, &sk);
        blst_p2_compress(k, &pk);

        mbedtls_mpi_read_binary(&ecp->Q.X, k, 96);
    }
}

PKey_bls_g2::PKey_bls_g2()
{
    m_alg = "BLS";

    mbedtls_ecp_keypair* ecp = mbedtls_pk_ec(m_key);
    ecp->grp.id = (mbedtls_ecp_group_id)MBEDTLS_ECP_DP_BLS12381_G1;

    unsigned char k[96];
    blst_scalar sk;
    blst_p2 pk;

    mbedtls_ctr_drbg_random(&g_ssl.ctr_drbg, k, 32);

    blst_keygen(&sk, k, 32);
    blst_bendian_from_scalar(k, &sk);

    mbedtls_mpi_read_binary(&ecp->d, k, 32);

    blst_sk_to_pk_in_g2(&pk, &sk);
    blst_p2_compress(k, &pk);

    mbedtls_mpi_read_binary(&ecp->Q.X, k, 96);
}

result_t PKey_bls_g2::sign(Buffer_base* data, int32_t alg, obj_ptr<Buffer_base>& retVal, AsyncEvent* ac)
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

    unsigned char k[96];
    blst_scalar sk;
    mbedtls_ecp_keypair* ecp = mbedtls_pk_ec(m_key);

    mbedtls_mpi_write_binary(&ecp->d, k, 32);
    blst_scalar_from_bendian(&sk, k);

    exlib::string buf;
    data->toString(buf);

    blst_p1 point;
    blst_hash_to_g1(&point, (unsigned char*)buf.c_str(), buf.length(),
        DST_G2_POP, sizeof(DST_G2_POP) - 1, NULL, 0);
    blst_sign_pk_in_g2(&point, &point, &sk);
    blst_p1_compress(k, &point);

    retVal = new Buffer(k, 48);

    return 0;
}

result_t PKey_bls_g2::verify(Buffer_base* data, Buffer_base* sign, int32_t alg, bool& retVal, AsyncEvent* ac)
{
    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    unsigned char k[96];
    mbedtls_ecp_keypair* ecp = mbedtls_pk_ec(m_key);

    exlib::string buf;
    data->toString(buf);

    exlib::string sig;
    sign->toString(sig);

    blst_p2_affine pk;
    mbedtls_mpi_write_binary(&ecp->Q.X, k, 96);
    blst_p2_uncompress(&pk, k);

    blst_p1_affine point;
    blst_p1_uncompress(&point, (const unsigned char*)sig.c_str());

    retVal = !blst_core_verify_pk_in_g2(&pk, &point, 1, (unsigned char*)buf.c_str(), buf.length(),
        DST_G2_POP, sizeof(DST_G2_POP) - 1, NULL, 0);

    return 0;
}

}
