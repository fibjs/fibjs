/*
 * PKey_p256k1.cpp
 *
 *  Created on: May 10, 2022
 *      Author: lion
 */

#include "Buffer.h"
#include "PKey_impl.h"
#include "ssl.h"

#define ENABLE_MODULE_RECOVERY 1
#include "mbedtls/src/secp256k1_api.h"
#include <secp256k1/include/secp256k1_recovery.h>

extern "C" int secp256k1_ec_pubkey_decompress(const secp256k1_context* ctx, unsigned char* pubkey, int* pubkeylen);

namespace fibjs {

PKey_p256k1::PKey_p256k1(mbedtls_pk_context& key)
    : PKey_ecc(key, false)
{
    mbedtls_ecp_keypair* ecp = mbedtls_pk_ec(m_key);
    size_t ksz = (mbedtls_pk_get_bitlen(&m_key) + 7) / 8;

    if (mbedtls_mpi_cmp_int(&ecp->d, 0) && !mbedtls_mpi_cmp_int(&ecp->Q.X, 0))
        mbedtls_ecp_mul(&ecp->grp, &ecp->Q, &ecp->d, &ecp->grp.G, mbedtls_ctr_drbg_random, &g_ssl.ctr_drbg);
    else if (!mbedtls_mpi_cmp_int(&ecp->Q.Y, 0) && mbedtls_mpi_size(&ecp->Q.X) == (ksz + 1)) {
        unsigned char data[65];

        mbedtls_mpi_write_binary(&ecp->Q.X, data, ksz + 1);
        if (data[0] == 2 || data[0] == 3) {
            mbedtls_mpi_read_binary(&ecp->Q.X, data + 1, ksz);

            int32_t sz = 33;
            secp256k1_ec_pubkey_decompress(secp256k1_ctx, data, &sz);
            mbedtls_mpi_read_binary(&ecp->Q.Y, data + ksz + 1, ksz);
        }
    }
}

PKey_p256k1::PKey_p256k1()
    : PKey_ecc(MBEDTLS_ECP_DP_SECP256K1)
{
}

result_t PKey_p256k1::check_opts(v8::Local<v8::Object> opts, AsyncEvent* ac)
{
    static const char* s_keys[] = {
        "to", "format", "recoverable", NULL
    };

    if (!ac->isSync())
        return 0;

    Isolate* isolate = holder();
    result_t hr;

    hr = CheckConfig(opts, s_keys);
    if (hr < 0)
        return hr;

    ac->m_ctx.resize(3);

    obj_ptr<PKey_base> to;
    hr = GetConfigValue(isolate->m_isolate, opts, "to", to, true);
    if (hr < 0 && hr != CALL_E_PARAMNOTOPTIONAL)
        return hr;
    ac->m_ctx[0] = to;

    bool recoverable = false;
    hr = GetConfigValue(isolate->m_isolate, opts, "recoverable", recoverable, true);
    if (hr < 0 && hr != CALL_E_PARAMNOTOPTIONAL)
        return hr;
    ac->m_ctx[2] = recoverable;

    exlib::string fmt = recoverable ? "bin" : "der";
    hr = GetConfigValue(isolate->m_isolate, opts, "format", fmt, true);
    if (hr < 0 && hr != CALL_E_PARAMNOTOPTIONAL)
        return hr;
    if (fmt != "der" && fmt != "bin")
        return CHECK_ERROR(Runtime::setError(exlib::string("unsupported format \'") + fmt + "\'."));
    ac->m_ctx[1] = fmt;

    return CHECK_ERROR(CALL_E_NOSYNC);
}

result_t PKey_p256k1::sign(Buffer_base* data, v8::Local<v8::Object> opts, obj_ptr<Buffer_base>& retVal, AsyncEvent* ac)
{
    if (m_alg == "ECSDSA")
        return PKey_ecc::sign(data, opts, retVal, ac);

    result_t hr = check_opts(opts, ac);
    if (hr < 0)
        return hr;

    bool recoverable = ac->m_ctx[2].boolVal();
    if (!recoverable)
        return PKey_ecc::sign(data, opts, retVal, ac);

    bool priv;

    hr = isPrivate(priv);
    if (hr < 0)
        return hr;

    if (!priv)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    if (ac->m_ctx[0].object())
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    exlib::string fmt = ac->m_ctx[1].string();
    if (fmt != "bin")
        return CHECK_ERROR(Runtime::setError(exlib::string("unsupported format \'") + fmt + "\'."));

    exlib::string strData;
    data->toString(strData);

    const unsigned char* hash = (const unsigned char*)strData.c_str();
    size_t hlen = strData.length();
    unsigned char buffer[KEYSIZE_256];
    unsigned char key[KEYSIZE_256];
    secp256k1_ecdsa_recoverable_signature signature;

    fix_hash(hash, hlen, buffer);

    mbedtls_ecp_keypair* ecp = mbedtls_pk_ec(m_key);
    mbedtls_mpi_write_binary(&ecp->d, key, KEYSIZE_256);

    secp256k1_ecdsa_sign_recoverable(secp256k1_ctx, &signature, hash, key, NULL, NULL);

    retVal = new Buffer(signature.data, sizeof(signature));

    return 0;
}

result_t PKey_base::recover(Buffer_base* sig, Buffer_base* data, obj_ptr<PKey_base>& retVal, AsyncEvent* ac)
{
    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    exlib::string strData;
    data->toString(strData);

    exlib::string strSig;
    sig->toString(strSig);
    if (strSig.length() != 65)
        return CHECK_ERROR(CALL_E_INVALID_DATA);

    const unsigned char* hash = (const unsigned char*)strData.c_str();
    size_t hlen = strData.length();
    unsigned char buffer[KEYSIZE_256];
    const secp256k1_ecdsa_recoverable_signature* signature = (const secp256k1_ecdsa_recoverable_signature*)strSig.c_str();
    secp256k1_pubkey pubkey;

    fix_hash(hash, hlen, buffer);

    int ret = secp256k1_ecdsa_recover(secp256k1_ctx, &pubkey, signature, hash);
    if (ret == 0)
        return CHECK_ERROR(CALL_E_INVALID_DATA);

    mbedtls_pk_context ctx;

    mbedtls_pk_init(&ctx);
    mbedtls_pk_setup(&ctx, mbedtls_pk_info_from_type(MBEDTLS_PK_ECKEY));

    mbedtls_ecp_keypair* ecp = mbedtls_pk_ec(ctx);
    PKey_ecc::load_group(&ecp->grp, MBEDTLS_ECP_DP_SECP256K1);

    mbedtls_mpi_read_binary_le(&ecp->Q.X, pubkey.data, KEYSIZE_256);
    mbedtls_mpi_read_binary_le(&ecp->Q.Y, pubkey.data + KEYSIZE_256, KEYSIZE_256);
    mbedtls_mpi_lset(&ecp->Q.Z, 1);

    retVal = new PKey_p256k1(ctx);

    return 0;
}

}
