/*
 * ECKey_bls_g2.cpp
 *
 *  Created on: May 1, 2022
 *      Author: lion
 */

#include "Buffer.h"
#include "BlsKey.h"
#include "ssl.h"
#include <blst/include/blst.h>
#include "encoding.h"

namespace fibjs {

// const unsigned char DST_G2_NUL[] = "BLS_SIG_BLS12381G1_XMD:SHA-256_SSWU_RO_NUL_";
// const unsigned char DST_G2_AUG[] = "BLS_SIG_BLS12381G1_XMD:SHA-256_SSWU_RO_AUG_";
const unsigned char DST_G2_POP[] = "BLS_SIG_BLS12381G1_XMD:SHA-256_SSWU_RO_POP_";

BlsKey_g2::BlsKey_g2(mbedtls_pk_context& key)
    : ECKey_impl<BlsKey_base>(key, false)
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

BlsKey_g2::BlsKey_g2()
{
    m_alg = "BLS";

    mbedtls_ecp_keypair* ecp = mbedtls_pk_ec(m_key);
    ecp->grp.id = (mbedtls_ecp_group_id)MBEDTLS_ECP_DP_BLS12381_G2;

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

result_t BlsKey_g1::check_opts(v8::Local<v8::Object> opts, AsyncEvent* ac)
{
    static const char* s_keys[] = {
        "format", NULL
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

    return CHECK_ERROR(CALL_E_NOSYNC);
}

result_t BlsKey_g2::sign(Buffer_base* data, v8::Local<v8::Object> opts, obj_ptr<Buffer_base>& retVal, AsyncEvent* ac)
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

    unsigned char k[96];
    blst_scalar sk;
    mbedtls_ecp_keypair* ecp = mbedtls_pk_ec(m_key);

    mbedtls_mpi_write_binary(&ecp->d, k, 32);
    blst_scalar_from_bendian(&sk, k);

    obj_ptr<Buffer> buf = Buffer::Cast(data);

    blst_p1 point;
    blst_hash_to_g1(&point, buf->data(), buf->length(),
        DST_G2_POP, sizeof(DST_G2_POP) - 1, NULL, 0);
    blst_sign_pk_in_g2(&point, &point, &sk);
    blst_p1_compress(k, &point);

    retVal = new Buffer(k, 48);

    return 0;
}

result_t BlsKey_g2::verify(Buffer_base* data, Buffer_base* sign, v8::Local<v8::Object> opts, bool& retVal, AsyncEvent* ac)
{
    result_t hr = check_opts(opts, ac);
    if (hr < 0)
        return hr;

    unsigned char k[96];
    mbedtls_ecp_keypair* ecp = mbedtls_pk_ec(m_key);

    obj_ptr<Buffer> buf = Buffer::Cast(data);
    obj_ptr<Buffer> sig = Buffer::Cast(sign);

    blst_p2_affine pk;
    mbedtls_mpi_write_binary(&ecp->Q.X, k, 96);
    blst_p2_uncompress(&pk, k);

    blst_p1_affine point;
    blst_p1_uncompress(&point, sig->data());

    retVal = !blst_core_verify_pk_in_g2(&pk, &point, 1, buf->data(), buf->length(),
        DST_G2_POP, sizeof(DST_G2_POP) - 1, NULL, 0);

    return 0;
}

}
