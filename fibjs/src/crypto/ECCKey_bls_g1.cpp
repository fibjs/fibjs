/*
 * ECCKey_bls_g1.cpp
 *
 *  Created on: May 1, 2022
 *      Author: lion
 */

#include "Buffer.h"
#include "PKey_impl.h"
#include "ssl.h"
#include <blst/include/blst.h>
#include "encoding.h"

namespace fibjs {

// const unsigned char DST_G1_NUL[] = "BLS_SIG_BLS12381G2_XMD:SHA-256_SSWU_RO_NUL_";
// const unsigned char DST_G1_AUG[] = "BLS_SIG_BLS12381G2_XMD:SHA-256_SSWU_RO_AUG_";
const unsigned char DST_G1_POP[] = "BLS_SIG_BLS12381G2_XMD:SHA-256_SSWU_RO_POP_";

result_t PKey_bls_g1::mpi_load(Isolate* isolate, mbedtls_mpi* n, v8::Local<v8::Object> o)
{
    exlib::string b64, s;
    result_t hr;

    JSValue v = o->Get(isolate->context(), isolate->NewString("x", 1));
    if (IsEmpty(v))
        return CALL_E_PARAMNOTOPTIONAL;

    if (v->IsArray()) {
        v8::Local<v8::Array> a = v8::Local<v8::Array>::Cast(v);
        int32_t len = a->Length();
        blst_p1 point = {};

        for (int32_t i = 0; i < len; i++) {
            hr = GetConfigValue(isolate->m_isolate, a, i, b64);
            if (hr < 0)
                return hr;

            base64Decode(b64.c_str(), b64.length(), s);

            blst_p1_affine pk;
            blst_p1_uncompress(&pk, (const byte*)s.c_str());
            blst_p1_add_or_double_affine(&point, &point, &pk);
        }

        s.resize(48);
        blst_p1_compress((byte*)s.c_buffer(), &point);
    } else {
        hr = GetArgumentValue(isolate->m_isolate, v, b64);
        if (hr < 0)
            return hr;

        base64Decode(b64.c_str(), b64.length(), s);
    }

    int32_t ret;
    ret = mbedtls_mpi_read_binary(n, (unsigned char*)s.c_str(), s.length());
    if (ret != 0)
        return CHECK_ERROR(_ssl::setError(ret));

    return 0;
}

PKey_bls_g1::PKey_bls_g1(mbedtls_pk_context& key)
    : ECCKey_impl<ECCKey_base>(key, false)
{
    m_alg = "BLS";

    mbedtls_ecp_keypair* ecp = mbedtls_pk_ec(m_key);

    if (mbedtls_mpi_cmp_int(&ecp->d, 0) && !mbedtls_mpi_cmp_int(&ecp->Q.X, 0)) {
        unsigned char k[96];
        blst_scalar sk;
        mbedtls_ecp_keypair* ecp = mbedtls_pk_ec(m_key);

        mbedtls_mpi_write_binary(&ecp->d, k, 32);
        blst_scalar_from_bendian(&sk, k);

        blst_p1 pk;

        blst_sk_to_pk_in_g1(&pk, &sk);
        blst_p1_compress(k, &pk);

        mbedtls_mpi_read_binary(&ecp->Q.X, k, 48);
    }
}

PKey_bls_g1::PKey_bls_g1()
{
    m_alg = "BLS";

    mbedtls_ecp_keypair* ecp = mbedtls_pk_ec(m_key);
    ecp->grp.id = (mbedtls_ecp_group_id)MBEDTLS_ECP_DP_BLS12381_G1;

    unsigned char k[96];
    blst_scalar sk;
    blst_p1 pk;

    mbedtls_ctr_drbg_random(&g_ssl.ctr_drbg, k, 32);

    blst_keygen(&sk, k, 32);
    blst_bendian_from_scalar(k, &sk);

    mbedtls_mpi_read_binary(&ecp->d, k, 32);

    blst_sk_to_pk_in_g1(&pk, &sk);
    blst_p1_compress(k, &pk);

    mbedtls_mpi_read_binary(&ecp->Q.X, k, 48);
}

result_t PKey_bls_g2::check_opts(v8::Local<v8::Object> opts, AsyncEvent* ac)
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

result_t PKey_bls_g1::sign(Buffer_base* data, v8::Local<v8::Object> opts, obj_ptr<Buffer_base>& retVal, AsyncEvent* ac)
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

    exlib::string buf;
    data->toString(buf);

    blst_p2 point;
    blst_hash_to_g2(&point, (unsigned char*)buf.c_str(), buf.length(),
        DST_G1_POP, sizeof(DST_G1_POP) - 1, NULL, 0);
    blst_sign_pk_in_g1(&point, &point, &sk);
    blst_p2_compress(k, &point);

    retVal = new Buffer(k, 96);

    return 0;
}

result_t PKey_bls_g1::verify(Buffer_base* data, Buffer_base* sign, v8::Local<v8::Object> opts, bool& retVal, AsyncEvent* ac)
{
    result_t hr = check_opts(opts, ac);
    if (hr < 0)
        return hr;

    unsigned char k[96];
    mbedtls_ecp_keypair* ecp = mbedtls_pk_ec(m_key);

    exlib::string buf;
    data->toString(buf);

    exlib::string sig;
    sign->toString(sig);

    blst_p1_affine pk;
    mbedtls_mpi_write_binary(&ecp->Q.X, k, 48);
    blst_p1_uncompress(&pk, k);

    blst_p2_affine point;
    blst_p2_uncompress(&point, (const unsigned char*)sig.c_str());

    retVal = !blst_core_verify_pk_in_g1(&pk, &point, 1, (unsigned char*)buf.c_str(), buf.length(),
        DST_G1_POP, sizeof(DST_G1_POP) - 1, NULL, 0);

    return 0;
}

}
