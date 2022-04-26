
/*
 * bls.cpp
 *
 *  Created on: Apr 24, 2022
 *      Author: lion
 */

#define MBEDTLS_ALLOW_PRIVATE_ACCESS

#include "object.h"
#include "PKey.h"
#include "Buffer.h"
#include "ssl.h"
#include <mbedtls/pem.h>
#include <mbedtls/asn1write.h>
#include <mbedtls/error.h>
#include <blst/include/blst.h>

namespace fibjs {

int PKey::bls_get_pubkey()
{
    unsigned char k[96];
    blst_scalar sk;
    mbedtls_ecp_keypair* ecp = mbedtls_pk_ec(m_key);

    mbedtls_mpi_write_binary(&ecp->d, k, 32);
    blst_scalar_from_bendian(&sk, k);

    if (ecp->grp.id == MBEDTLS_ECP_DP_BLS12381_G1) {
        blst_p1 pk;

        blst_sk_to_pk_in_g1(&pk, &sk);
        blst_p1_compress(k, &pk);

        return mbedtls_mpi_read_binary(&ecp->Q.X, k, 48);
    } else {
        blst_p2 pk;

        blst_sk_to_pk_in_g2(&pk, &sk);
        blst_p2_compress(k, &pk);

        return mbedtls_mpi_read_binary(&ecp->Q.X, k, 96);
    }
}

result_t PKey::bls_generateKey(int32_t type)
{
    int32_t ret;
    mbedtls_ecp_keypair* ecp = mbedtls_pk_ec(m_key);
    ecp->grp.id = (mbedtls_ecp_group_id)(type == 1 ? MBEDTLS_ECP_DP_BLS12381_G1 : MBEDTLS_ECP_DP_BLS12381_G2);

    blst_scalar sk;
    unsigned char k[32];

    mbedtls_ctr_drbg_random(&g_ssl.ctr_drbg, k, sizeof(k));

    blst_keygen(&sk, k, sizeof(k));
    blst_bendian_from_scalar(k, &sk);

    ret = mbedtls_mpi_read_binary(&ecp->d, k, sizeof(k));
    if (ret != 0)
        return CHECK_ERROR(_ssl::setError(ret));

    m_alg = "BLS";

    ret = bls_get_pubkey();
    if (ret != 0)
        return CHECK_ERROR(_ssl::setError(ret));

    return 0;
}

const unsigned char DST_G1_NUL[] = "BLS_SIG_BLS12381G2_XMD:SHA-256_SSWU_RO_NUL_";
const unsigned char DST_G2_NUL[] = "BLS_SIG_BLS12381G1_XMD:SHA-256_SSWU_RO_NUL_";

const unsigned char DST_G1_AUG[] = "BLS_SIG_BLS12381G2_XMD:SHA-256_SSWU_RO_AUG_";
const unsigned char DST_G2_AUG[] = "BLS_SIG_BLS12381G1_XMD:SHA-256_SSWU_RO_AUG_";

const unsigned char DST_G1_POP[] = "BLS_SIG_BLS12381G2_XMD:SHA-256_SSWU_RO_POP_";
const unsigned char DST_G2_POP[] = "BLS_SIG_BLS12381G1_XMD:SHA-256_SSWU_RO_POP_";

result_t PKey::bls_sign(Buffer_base* data, obj_ptr<Buffer_base>& retVal)
{
    unsigned char k[96];
    blst_scalar sk;
    mbedtls_ecp_keypair* ecp = mbedtls_pk_ec(m_key);

    mbedtls_mpi_write_binary(&ecp->d, k, 32);
    blst_scalar_from_bendian(&sk, k);

    exlib::string buf;
    data->toString(buf);

    if (ecp->grp.id == MBEDTLS_ECP_DP_BLS12381_G1) {
        blst_p2 point;
        blst_hash_to_g2(&point, (unsigned char*)buf.c_str(), buf.length(),
            DST_G1_POP, sizeof(DST_G1_POP) - 1, NULL, 0);
        blst_sign_pk_in_g1(&point, &point, &sk);
        blst_p2_compress(k, &point);

        retVal = new Buffer(k, 96);
    } else {
        blst_p1 point;
        blst_hash_to_g1(&point, (unsigned char*)buf.c_str(), buf.length(),
            DST_G2_POP, sizeof(DST_G2_POP) - 1, NULL, 0);
        blst_sign_pk_in_g2(&point, &point, &sk);
        blst_p1_compress(k, &point);

        retVal = new Buffer(k, 48);
    }

    return 0;
}

result_t PKey::bls_verify(Buffer_base* data, Buffer_base* sign, bool& retVal)
{
    unsigned char k[96];
    mbedtls_ecp_keypair* ecp = mbedtls_pk_ec(m_key);

    exlib::string buf;
    data->toString(buf);

    exlib::string sig;
    sign->toString(sig);

    if (ecp->grp.id == MBEDTLS_ECP_DP_BLS12381_G1) {
        blst_p1_affine pk;
        mbedtls_mpi_write_binary(&ecp->Q.X, k, 48);
        blst_p1_uncompress(&pk, k);

        blst_p2_affine point;
        blst_p2_uncompress(&point, (const unsigned char*)sig.c_str());

        retVal = !blst_core_verify_pk_in_g1(&pk, &point, 1, (unsigned char*)buf.c_str(), buf.length(),
            DST_G1_POP, sizeof(DST_G1_POP) - 1, NULL, 0);
    } else {
        blst_p2_affine pk;
        mbedtls_mpi_write_binary(&ecp->Q.X, k, 96);
        blst_p2_uncompress(&pk, k);

        blst_p1_affine point;
        blst_p1_uncompress(&point, (const unsigned char*)sig.c_str());

        retVal = !blst_core_verify_pk_in_g2(&pk, &point, 1, (unsigned char*)buf.c_str(), buf.length(),
            DST_G2_POP, sizeof(DST_G2_POP) - 1, NULL, 0);
    }

    return 0;
}

}
