/*
 * Ed25519Key.cpp
 *
 *  Created on: May 2, 2022
 *      Author: lion
 */

#include "Buffer.h"
#include "Ed25519Key.h"
#include "ssl.h"
#include <mbedtls/pem.h>
#include <mbedtls/asn1write.h>
#include <mbedtls/error.h>
#include <curve25519/include/ed25519_signature.h>
#include <curve25519/include/curve25519_dh.h>

namespace fibjs {

const unsigned char s_der_priv_lead[]
    = { 0x30, 0x2e, 0x02, 0x01, 0x00, 0x30, 0x05, 0x06, 0x03, 0x2b, 0x65, 0x70, 0x04, 0x22, 0x04, 0x20 };
const unsigned char s_der_priv_lead_x[]
    = { 0x30, 0x2e, 0x02, 0x01, 0x00, 0x30, 0x05, 0x06, 0x03, 0x2b, 0x65, 0x6e, 0x04, 0x22, 0x04, 0x20 };

const unsigned char s_der_pub_lead[]
    = { 0x30, 0x2a, 0x30, 0x05, 0x06, 0x03, 0x2b, 0x65, 0x70, 0x03, 0x21, 0x00 };
const unsigned char s_der_pub_lead_x[]
    = { 0x30, 0x2a, 0x30, 0x05, 0x06, 0x03, 0x2b, 0x65, 0x6e, 0x03, 0x21, 0x00 };

#define PEM_BEGIN_PRIVATE_KEY "-----BEGIN PRIVATE KEY-----"
#define PEM_END_PRIVATE_KEY "-----END PRIVATE KEY-----"
#define PEM_BEGIN_PUBLIC_KEY "-----BEGIN PUBLIC KEY-----"
#define PEM_END_PUBLIC_KEY "-----END PUBLIC KEY-----"

Ed25519Key::Ed25519Key(mbedtls_pk_context& key)
    : ECCKey_impl<Ed25519Key_base>(key, false)
{
    mbedtls_ecp_keypair* ecp = mbedtls_pk_ec(m_key);

    m_alg = (int32_t)ecp->grp.id == MBEDTLS_ECP_DP_ED25519 ? "EdDSA" : "DH";

    if (mbedtls_mpi_cmp_int(&ecp->d, 0) && !mbedtls_mpi_cmp_int(&ecp->Q.X, 0)) {
        unsigned char sk[ed25519_private_key_size];

        mbedtls_mpi_write_binary(&ecp->d, sk, ed25519_public_key_size);

        if ((int32_t)ecp->grp.id == MBEDTLS_ECP_DP_ED25519)
            ed25519_CreateKeyPair(sk + ed25519_public_key_size, sk, 0, sk);
        else
            curve25519_dh_CalculatePublicKey(sk + ed25519_public_key_size, sk);

        mbedtls_mpi_read_binary(&ecp->Q.X, sk + ed25519_public_key_size, ed25519_public_key_size);
    }
}

Ed25519Key::Ed25519Key(int32_t id)
{
    m_alg = id == MBEDTLS_ECP_DP_ED25519 ? "EdDSA" : "DH";

    mbedtls_ecp_keypair* ecp = mbedtls_pk_ec(m_key);
    load_group(&ecp->grp, id);

    unsigned char sk[ed25519_private_key_size];
    mbedtls_ctr_drbg_random(&g_ssl.ctr_drbg, sk, ed25519_public_key_size);

    if (id == MBEDTLS_ECP_DP_ED25519)
        ed25519_CreateKeyPair(sk + ed25519_public_key_size, sk, 0, sk);
    else
        curve25519_dh_CalculatePublicKey(sk + ed25519_public_key_size, sk);

    mbedtls_mpi_read_binary(&ecp->d, sk, ed25519_public_key_size);
    mbedtls_mpi_read_binary(&ecp->Q.X, sk + ed25519_public_key_size, ed25519_public_key_size);
}

result_t Ed25519Key::toX25519(obj_ptr<ECCKey_base>& retVal, AsyncEvent* ac)
{
    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    mbedtls_ecp_keypair* ecp = mbedtls_pk_ec(m_key);

    if ((int32_t)ecp->grp.id != MBEDTLS_ECP_DP_ED25519)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    mbedtls_pk_context key1;

    mbedtls_pk_init(&key1);
    mbedtls_pk_setup(&key1, m_key.pk_info);

    mbedtls_ecp_keypair* ecp1 = mbedtls_pk_ec(key1);

    load_group(&ecp1->grp, MBEDTLS_ECP_DP_CURVE25519);

    unsigned char k[ed25519_public_key_size];

    if (mbedtls_mpi_cmp_int(&ecp->d, 0)) {
        mbedtls_mpi_write_binary(&ecp->d, k, ed25519_public_key_size);
        curve25519_dh_ConvertPrivateKey(k, k);
        mbedtls_mpi_read_binary(&ecp1->d, k, ed25519_public_key_size);
    }

    if (mbedtls_mpi_cmp_int(&ecp->Q.X, 0)) {
        mbedtls_mpi_write_binary(&ecp->Q.X, k, ed25519_public_key_size);
        curve25519_dh_ConvertPublicKey(k, k);
        mbedtls_mpi_read_binary(&ecp1->Q.X, k, ed25519_public_key_size);
    }

    retVal = new Ed25519Key(key1);

    return 0;
}

static int parse_key(mbedtls_pk_context& ctx, const unsigned char* key, size_t keylen)
{
    if (keylen == sizeof(s_der_priv_lead) + ed25519_public_key_size) {
        int32_t id;

        if (!memcmp(key, s_der_priv_lead, sizeof(s_der_priv_lead)))
            id = MBEDTLS_ECP_DP_ED25519;
        else if (!memcmp(key, s_der_priv_lead_x, sizeof(s_der_priv_lead)))
            id = MBEDTLS_ECP_DP_CURVE25519;
        else
            return MBEDTLS_ERR_PK_KEY_INVALID_FORMAT;

        int32_t ret;

        ret = mbedtls_pk_setup(&ctx, mbedtls_pk_info_from_type(MBEDTLS_PK_ECKEY));
        if (ret < 0)
            return ret;

        mbedtls_ecp_keypair* ecp = mbedtls_pk_ec(ctx);
        ECCKey::load_group(&ecp->grp, id);

        ret = mbedtls_mpi_read_binary(&ecp->d, (const unsigned char*)key + sizeof(s_der_priv_lead),
            ed25519_public_key_size);
        if (ret < 0)
            return ret;

        unsigned char sk[ed25519_private_key_size];

        if (id == MBEDTLS_ECP_DP_ED25519)
            ed25519_CreateKeyPair(sk + ed25519_public_key_size, sk, 0, (const unsigned char*)key + sizeof(s_der_priv_lead));
        else
            curve25519_dh_CalculatePublicKey(sk + ed25519_public_key_size, (unsigned char*)key + sizeof(s_der_priv_lead));

        ret = mbedtls_mpi_read_binary(&ecp->Q.X, sk + ed25519_public_key_size, ed25519_public_key_size);
        if (ret < 0)
            return ret;

        return 0;
    }

    return MBEDTLS_ERR_PK_KEY_INVALID_FORMAT;
}

static int parse_pub_key(mbedtls_pk_context& ctx, const unsigned char* key, size_t keylen)
{

    if (keylen == sizeof(s_der_pub_lead) + ed25519_public_key_size) {
        int32_t id;

        if (!memcmp(key, s_der_pub_lead, sizeof(s_der_pub_lead)))
            id = MBEDTLS_ECP_DP_ED25519;
        else if (!memcmp(key, s_der_pub_lead_x, sizeof(s_der_pub_lead_x)))
            id = MBEDTLS_ECP_DP_CURVE25519;
        else
            return MBEDTLS_ERR_PK_KEY_INVALID_FORMAT;

        int32_t ret;

        ret = mbedtls_pk_setup(&ctx, mbedtls_pk_info_from_type(MBEDTLS_PK_ECKEY));
        if (ret < 0)
            return ret;

        mbedtls_ecp_keypair* ecp = mbedtls_pk_ec(ctx);
        ECCKey::load_group(&ecp->grp, id);

        ret = mbedtls_mpi_read_binary(&ecp->Q.X, (const unsigned char*)key + sizeof(s_der_pub_lead),
            ed25519_public_key_size);
        if (ret < 0)
            return ret;

        return 0;
    }

    return MBEDTLS_ERR_PK_KEY_INVALID_FORMAT;
}

static int parse_pem(mbedtls_pk_context& ctx, const unsigned char* key)
{
    int32_t ret;
    mbedtls_pem_context pem;

    do {
        size_t len;

        mbedtls_pem_init(&pem);

        ret = mbedtls_pem_read_buffer(&pem, PEM_BEGIN_PRIVATE_KEY, PEM_END_PRIVATE_KEY,
            (const unsigned char*)key, NULL, 0, &len);
        if (ret == 0) {
            ret = parse_key(ctx, pem.buf, pem.buflen);
            if (ret == 0)
                break;
        }

        ret = mbedtls_pem_read_buffer(&pem, PEM_BEGIN_PUBLIC_KEY, PEM_END_PUBLIC_KEY,
            (const unsigned char*)key, NULL, 0, &len);
        if (ret == 0) {
            ret = parse_pub_key(ctx, pem.buf, pem.buflen);
            if (ret == 0)
                break;
        }
    } while (false);
    mbedtls_pem_free(&pem);

    return ret ? MBEDTLS_ERR_PK_KEY_INVALID_FORMAT : 0;
}

result_t Ed25519Key::from(Buffer_base* DerKey, obj_ptr<PKey_base>& retVal)
{
    int32_t ret;
    mbedtls_pk_context ctx;

    mbedtls_pk_init(&ctx);

    do {
        exlib::string key;
        DerKey->toString(key);

        ret = parse_key(ctx, (unsigned char*)key.c_str(), key.length());
        if (ret != MBEDTLS_ERR_PK_KEY_INVALID_FORMAT)
            break;

        ret = parse_pub_key(ctx, (unsigned char*)key.c_str(), key.length());
    } while (false);

    if (ret != 0) {
        mbedtls_pk_free(&ctx);
        return CHECK_ERROR(_ssl::setError(ret));
    }

    retVal = new Ed25519Key(ctx);

    return 0;
}

result_t Ed25519Key::from(exlib::string pemKey, obj_ptr<PKey_base>& retVal)
{
    int32_t ret;
    mbedtls_pk_context ctx;

    mbedtls_pk_init(&ctx);

    ret = parse_pem(ctx, (unsigned char*)pemKey.c_str());
    if (ret != 0) {
        mbedtls_pk_free(&ctx);
        return CHECK_ERROR(_ssl::setError(ret));
    }

    retVal = new Ed25519Key(ctx);

    return 0;
}

static int write_key(mbedtls_pk_context& ctx, exlib::string& buf)
{
    mbedtls_ecp_keypair* ecp = mbedtls_pk_ec(ctx);
    int32_t id = ecp->grp.id;

    buf.resize(ed25519_public_key_size + sizeof(s_der_priv_lead));
    memcpy(buf.c_buffer(), id == MBEDTLS_ECP_DP_ED25519 ? s_der_priv_lead : s_der_priv_lead_x, sizeof(s_der_priv_lead));
    mbedtls_mpi_write_binary(&ecp->d, (unsigned char*)buf.c_buffer() + sizeof(s_der_priv_lead), ed25519_public_key_size);
    return 0;
}

static int write_pub_key(mbedtls_pk_context& ctx, exlib::string& buf)
{
    mbedtls_ecp_keypair* ecp = mbedtls_pk_ec(ctx);
    int32_t id = ecp->grp.id;

    buf.resize(ed25519_public_key_size + sizeof(s_der_pub_lead));
    memcpy(buf.c_buffer(), id == MBEDTLS_ECP_DP_ED25519 ? s_der_pub_lead : s_der_pub_lead_x, sizeof(s_der_pub_lead));
    mbedtls_mpi_write_binary(&ecp->Q.X, (unsigned char*)buf.c_buffer() + sizeof(s_der_pub_lead), ed25519_public_key_size);
    return 0;
}

result_t Ed25519Key::pem(exlib::string& retVal)
{
    mbedtls_ecp_keypair* ecp = mbedtls_pk_ec(m_key);
    exlib::string der_buf;
    size_t olen;
    int32_t ret;

    retVal.resize(200);

    if (mbedtls_mpi_cmp_int(&ecp->d, 0)) {
        write_key(m_key, der_buf);
        ret = mbedtls_pem_write_buffer(PEM_BEGIN_PRIVATE_KEY "\n", PEM_END_PRIVATE_KEY "\n",
            (const unsigned char*)der_buf.c_str(), der_buf.length(), (unsigned char*)retVal.c_buffer(), retVal.length(), &olen);
    } else {
        write_pub_key(m_key, der_buf);
        ret = mbedtls_pem_write_buffer(PEM_BEGIN_PUBLIC_KEY "\n", PEM_END_PUBLIC_KEY "\n",
            (const unsigned char*)der_buf.c_str(), der_buf.length(), (unsigned char*)retVal.c_buffer(), retVal.length(), &olen);
    }
    if (ret != 0)
        return CHECK_ERROR(_ssl::setError(ret));

    retVal.resize(olen - 1);
    return 0;
}

result_t Ed25519Key::der(obj_ptr<Buffer_base>& retVal)
{
    mbedtls_ecp_keypair* ecp = mbedtls_pk_ec(m_key);
    exlib::string buf;

    if (mbedtls_mpi_cmp_int(&ecp->d, 0))
        write_key(m_key, buf);
    else
        write_pub_key(m_key, buf);

    retVal = new Buffer(buf);
    return 0;
}

result_t Ed25519Key::check_opts(v8::Local<v8::Object> opts, AsyncEvent* ac)
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

    ac->m_ctx.resize(1);

    exlib::string fmt = "der";
    hr = GetConfigValue(isolate->m_isolate, opts, "format", fmt, true);
    if (hr < 0 && hr != CALL_E_PARAMNOTOPTIONAL)
        return hr;
    if (fmt != "der" && fmt != "raw")
        return CHECK_ERROR(Runtime::setError(exlib::string("unsupported format \'") + fmt + "\'."));
    ac->m_ctx[0] = fmt;

    return CHECK_ERROR(CALL_E_NOSYNC);
}

result_t Ed25519Key::sign(Buffer_base* data, v8::Local<v8::Object> opts, obj_ptr<Buffer_base>& retVal, AsyncEvent* ac)
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

    mbedtls_ecp_keypair* ecp = mbedtls_pk_ec(m_key);

    if ((int32_t)ecp->grp.id != MBEDTLS_ECP_DP_ED25519)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    unsigned char sk[ed25519_private_key_size];
    exlib::string buf;
    exlib::string sig;

    mbedtls_mpi_write_binary(&ecp->d, sk, ed25519_public_key_size);
    mbedtls_mpi_write_binary(&ecp->Q.X, sk + ed25519_public_key_size, ed25519_public_key_size);

    data->toString(buf);

    sig.resize(ed25519_signature_size);
    ed25519_SignMessage((unsigned char*)sig.c_buffer(), sk, NULL, (const unsigned char*)buf.c_str(), buf.length());

    if (ac->m_ctx[0].string() == "der") {
        hr = bin2der(sig, sig);
        if (hr < 0)
            return hr;
    }

    retVal = new Buffer(sig);

    return 0;
}

result_t Ed25519Key::verify(Buffer_base* data, Buffer_base* sign, v8::Local<v8::Object> opts, bool& retVal, AsyncEvent* ac)
{
    result_t hr = check_opts(opts, ac);
    if (hr < 0)
        return hr;

    mbedtls_ecp_keypair* ecp = mbedtls_pk_ec(m_key);

    if ((int32_t)ecp->grp.id != MBEDTLS_ECP_DP_ED25519)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    unsigned char pk[ed25519_public_key_size];
    exlib::string buf;
    exlib::string sig;

    sign->toString(sig);
    if (ac->m_ctx[0].string() == "der") {
        hr = der2bin(sig, sig);
        if (hr < 0) {
            retVal = false;
            return 0;
        }
    }

    data->toString(buf);
    mbedtls_mpi_write_binary(&ecp->Q.X, pk, ed25519_public_key_size);

    retVal = ed25519_VerifySignature((const unsigned char*)sig.c_str(), pk, (const unsigned char*)buf.c_str(), buf.length());

    return 0;
}

result_t Ed25519Key::computeSecret(ECCKey_base* publicKey, obj_ptr<Buffer_base>& retVal, AsyncEvent* ac)
{
    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    result_t hr;
    bool priv;
    mbedtls_pk_type_t type;

    mbedtls_pk_context& mkey = PKey::key(publicKey);

    type = mbedtls_pk_get_type(&mkey);
    if (type != MBEDTLS_PK_ECKEY)
        return CHECK_ERROR(CALL_E_INVALIDARG);

    type = mbedtls_pk_get_type(&m_key);
    if (type != MBEDTLS_PK_ECKEY)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    hr = isPrivate(priv);
    if (hr < 0)
        return hr;

    if (!priv)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    mbedtls_ecp_keypair* ecp1 = mbedtls_pk_ec(m_key);

    if (ecp1->grp.id != MBEDTLS_ECP_DP_CURVE25519)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    mbedtls_ecp_keypair* ecp2 = mbedtls_pk_ec(mkey);
    if (ecp1->grp.id != ecp2->grp.id)
        return CHECK_ERROR(Runtime::setError("Public key is not valid for specified curve"));

    unsigned char sk[ed25519_public_key_size];
    unsigned char pk[ed25519_public_key_size];
    unsigned char secret[ed25519_secret_key_size];

    mbedtls_mpi_write_binary(&ecp1->d, sk, ed25519_public_key_size);
    mbedtls_mpi_write_binary(&ecp2->Q.X, pk, ed25519_public_key_size);

    curve25519_dh_CreateSharedKey(secret, pk, sk);

    retVal = new Buffer(secret, ed25519_public_key_size);

    return 0;
}

}
