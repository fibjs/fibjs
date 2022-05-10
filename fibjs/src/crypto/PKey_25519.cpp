/*
 * PKey_25519.cpp
 *
 *  Created on: May 2, 2022
 *      Author: lion
 */

#include "Buffer.h"
#include "PKey_impl.h"
#include "ssl.h"
#include <mbedtls/pem.h>
#include <mbedtls/asn1write.h>
#include <mbedtls/error.h>
#include <curve25519/include/ed25519_signature.h>

namespace fibjs {

const unsigned char s_der_priv_lead[]
    = { 0x30, 0x2e, 0x02, 0x01, 0x00, 0x30, 0x05, 0x06, 0x03, 0x2b, 0x65, 0x70, 0x04, 0x22, 0x04, 0x20 };

const unsigned char s_der_pub_lead[]
    = { 0x30, 0x2a, 0x30, 0x05, 0x06, 0x03, 0x2b, 0x65, 0x70, 0x03, 0x21, 0x00 };

#define PEM_BEGIN_PRIVATE_KEY "-----BEGIN PRIVATE KEY-----"
#define PEM_END_PRIVATE_KEY "-----END PRIVATE KEY-----"
#define PEM_BEGIN_PUBLIC_KEY "-----BEGIN PUBLIC KEY-----"
#define PEM_END_PUBLIC_KEY "-----END PUBLIC KEY-----"

PKey_25519::PKey_25519(mbedtls_pk_context& key)
    : PKey_ecc(key)
{
    m_alg = "EdDSA";

    mbedtls_ecp_keypair* ecp = mbedtls_pk_ec(m_key);

    if (mbedtls_mpi_cmp_int(&ecp->d, 0) && !mbedtls_mpi_cmp_int(&ecp->Q.X, 0)) {
        unsigned char sk[ed25519_private_key_size];
        mbedtls_ecp_keypair* ecp = mbedtls_pk_ec(m_key);

        mbedtls_mpi_write_binary(&ecp->d, sk, ed25519_public_key_size);
        ed25519_CreateKeyPair(sk + ed25519_public_key_size, sk, 0, sk);
        mbedtls_mpi_read_binary(&ecp->Q.X, sk + ed25519_public_key_size, ed25519_public_key_size);
    }
}

PKey_25519::PKey_25519()
{
    m_alg = "EdDSA";

    mbedtls_ecp_keypair* ecp = mbedtls_pk_ec(m_key);
    load_group(&ecp->grp, MBEDTLS_ECP_DP_ED25519);

    unsigned char sk[ed25519_private_key_size];
    mbedtls_ctr_drbg_random(&g_ssl.ctr_drbg, sk, ed25519_public_key_size);
    ed25519_CreateKeyPair(sk + ed25519_public_key_size, sk, 0, sk);

    mbedtls_mpi_read_binary(&ecp->d, sk, ed25519_public_key_size);
    mbedtls_mpi_read_binary(&ecp->Q.X, sk + ed25519_public_key_size, ed25519_public_key_size);
}

static int parse_key(mbedtls_pk_context& ctx, const unsigned char* key, size_t keylen)
{
    if (keylen == sizeof(s_der_priv_lead) + ed25519_public_key_size
        && !memcmp(key, s_der_priv_lead, sizeof(s_der_priv_lead))) {
        obj_ptr<PKey> pk1 = new PKey();
        int32_t ret;

        ret = mbedtls_pk_setup(&ctx, mbedtls_pk_info_from_type(MBEDTLS_PK_ECKEY));
        if (ret < 0)
            return ret;

        mbedtls_ecp_keypair* ecp = mbedtls_pk_ec(ctx);
        PKey_ecc::load_group(&ecp->grp, MBEDTLS_ECP_DP_ED25519);

        ret = mbedtls_mpi_read_binary(&ecp->d, (const unsigned char*)key + sizeof(s_der_priv_lead),
            ed25519_public_key_size);
        if (ret < 0)
            return ret;

        unsigned char sk[ed25519_private_key_size];

        ed25519_CreateKeyPair(sk + ed25519_public_key_size, sk, 0, (const unsigned char*)key + sizeof(s_der_priv_lead));
        ret = mbedtls_mpi_read_binary(&ecp->Q.X, sk + ed25519_public_key_size, ed25519_public_key_size);
        if (ret < 0)
            return ret;

        return 0;
    }

    return MBEDTLS_ERR_PK_KEY_INVALID_FORMAT;
}

static int parse_pub_key(mbedtls_pk_context& ctx, const unsigned char* key, size_t keylen)
{

    if (keylen == sizeof(s_der_pub_lead) + ed25519_public_key_size
        && !memcmp(key, s_der_pub_lead, sizeof(s_der_pub_lead))) {
        obj_ptr<PKey> pk1 = new PKey();
        int32_t ret;

        ret = mbedtls_pk_setup(&ctx, mbedtls_pk_info_from_type(MBEDTLS_PK_ECKEY));
        if (ret < 0)
            return ret;

        mbedtls_ecp_keypair* ecp = mbedtls_pk_ec(ctx);
        PKey_ecc::load_group(&ecp->grp, MBEDTLS_ECP_DP_ED25519);

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

result_t PKey_25519::from(Buffer_base* DerKey, obj_ptr<PKey_base>& retVal)
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

    retVal = new PKey_25519(ctx);

    return 0;
}

result_t PKey_25519::from(exlib::string pemKey, obj_ptr<PKey_base>& retVal)
{
    int32_t ret;
    mbedtls_pk_context ctx;

    mbedtls_pk_init(&ctx);

    ret = parse_pem(ctx, (unsigned char*)pemKey.c_str());
    if (ret != 0) {
        mbedtls_pk_free(&ctx);
        return CHECK_ERROR(_ssl::setError(ret));
    }

    retVal = new PKey_25519(ctx);

    return 0;
}

static int write_key(mbedtls_pk_context& ctx, exlib::string& buf)
{
    mbedtls_ecp_keypair* ecp = mbedtls_pk_ec(ctx);

    buf.resize(ed25519_public_key_size + sizeof(s_der_priv_lead));
    memcpy(buf.c_buffer(), s_der_priv_lead, sizeof(s_der_priv_lead));
    mbedtls_mpi_write_binary(&ecp->d, (unsigned char*)buf.c_buffer() + sizeof(s_der_priv_lead), ed25519_public_key_size);
    return 0;
}

static int write_pub_key(mbedtls_pk_context& ctx, exlib::string& buf)
{
    mbedtls_ecp_keypair* ecp = mbedtls_pk_ec(ctx);

    buf.resize(ed25519_public_key_size + sizeof(s_der_pub_lead));
    memcpy(buf.c_buffer(), s_der_pub_lead, sizeof(s_der_pub_lead));
    mbedtls_mpi_write_binary(&ecp->Q.X, (unsigned char*)buf.c_buffer() + sizeof(s_der_pub_lead), ed25519_public_key_size);
    return 0;
}

result_t PKey_25519::pem(exlib::string& retVal)
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

result_t PKey_25519::der(obj_ptr<Buffer_base>& retVal)
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

result_t PKey_25519::check_opts(v8::Local<v8::Object> opts, AsyncEvent* ac)
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
    if (fmt != "der" && fmt != "bin")
        return CHECK_ERROR(Runtime::setError(exlib::string("unsupported format \'") + fmt + "\'."));
    ac->m_ctx[0] = fmt;

    return CHECK_ERROR(CALL_E_NOSYNC);
}

result_t PKey_25519::sign(Buffer_base* data, v8::Local<v8::Object> opts, obj_ptr<Buffer_base>& retVal, AsyncEvent* ac)
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
    unsigned char sk[ed25519_private_key_size];
    exlib::string buf;
    exlib::string sig;

    mbedtls_mpi_write_binary(&ecp->d, (unsigned char*)sk, ed25519_public_key_size);
    mbedtls_mpi_write_binary(&ecp->Q.X, (unsigned char*)sk + ed25519_public_key_size, ed25519_public_key_size);

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

result_t PKey_25519::verify(Buffer_base* data, Buffer_base* sign, v8::Local<v8::Object> opts, bool& retVal, AsyncEvent* ac)
{
    result_t hr = check_opts(opts, ac);
    if (hr < 0)
        return hr;

    mbedtls_ecp_keypair* ecp = mbedtls_pk_ec(m_key);
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
    mbedtls_mpi_write_binary(&ecp->Q.X, (unsigned char*)pk, ed25519_public_key_size);

    retVal = ed25519_VerifySignature((const unsigned char*)sig.c_str(), pk, (const unsigned char*)buf.c_str(), buf.length());

    return 0;
}

}
