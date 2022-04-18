/*
 * PKey_ed25519.cpp
 *
 *  Created on: Apr 17, 2022
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

result_t PKey::ed25519_generateKey()
{
    int32_t ret;
    mbedtls_ecp_keypair* ecp = mbedtls_pk_ec(m_key);
    ecp->grp.id = (mbedtls_ecp_group_id)MBEDTLS_ECP_DP_ED25519;

    unsigned char sk[ed25519_private_key_size];

    mbedtls_ctr_drbg_random(&g_ssl.ctr_drbg, sk, ed25519_public_key_size);
    ed25519_CreateKeyPair(sk + ed25519_public_key_size, sk, 0, sk);

    ret = mbedtls_mpi_read_binary(&ecp->d, sk, ed25519_public_key_size);
    if (ret != 0)
        return CHECK_ERROR(_ssl::setError(ret));

    ret = mbedtls_mpi_read_binary(&ecp->Q.X, sk + ed25519_public_key_size, ed25519_public_key_size);
    if (ret != 0)
        return CHECK_ERROR(_ssl::setError(ret));

    m_alg = "EdDSA";

    return 0;
}

int PKey::ed25519_get_pubkey()
{
    unsigned char sk[ed25519_private_key_size];
    mbedtls_ecp_keypair* ecp = mbedtls_pk_ec(m_key);

    mbedtls_mpi_write_binary(&ecp->d, sk, ed25519_public_key_size);
    ed25519_CreateKeyPair(sk + ed25519_public_key_size, sk, 0, sk);
    return mbedtls_mpi_read_binary(&ecp->Q.X, sk + ed25519_public_key_size, ed25519_public_key_size);
}

int PKey::ed25519_parse_key(const unsigned char* key, size_t keylen)
{
    if (keylen == sizeof(s_der_priv_lead) + ed25519_public_key_size
        && !memcmp(key, s_der_priv_lead, sizeof(s_der_priv_lead))) {
        obj_ptr<PKey> pk1 = new PKey();
        int32_t ret;

        ret = mbedtls_pk_setup(&m_key, mbedtls_pk_info_from_type(MBEDTLS_PK_ECKEY));
        if (ret < 0)
            return ret;

        mbedtls_ecp_keypair* ecp = mbedtls_pk_ec(m_key);
        ecp->grp.id = (mbedtls_ecp_group_id)MBEDTLS_ECP_DP_ED25519;

        ret = mbedtls_mpi_read_binary(&ecp->d, (const unsigned char*)key + sizeof(s_der_priv_lead),
            ed25519_public_key_size);
        if (ret < 0)
            return ret;

        unsigned char sk[ed25519_private_key_size];

        ed25519_CreateKeyPair(sk + ed25519_public_key_size, sk, 0, (const unsigned char*)key + sizeof(s_der_priv_lead));
        ret = mbedtls_mpi_read_binary(&ecp->Q.X, sk + ed25519_public_key_size, ed25519_public_key_size);
        if (ret < 0)
            return ret;

        m_alg = "EdDSA";

        return 0;
    }

    return MBEDTLS_ERR_PK_KEY_INVALID_FORMAT;
}

int PKey::ed25519_parse_pub_key(const unsigned char* key, size_t keylen)
{

    if (keylen == sizeof(s_der_pub_lead) + ed25519_public_key_size
        && !memcmp(key, s_der_pub_lead, sizeof(s_der_pub_lead))) {
        obj_ptr<PKey> pk1 = new PKey();
        int32_t ret;

        ret = mbedtls_pk_setup(&m_key, mbedtls_pk_info_from_type(MBEDTLS_PK_ECKEY));
        if (ret < 0)
            return ret;

        mbedtls_ecp_keypair* ecp = mbedtls_pk_ec(m_key);
        ecp->grp.id = (mbedtls_ecp_group_id)MBEDTLS_ECP_DP_ED25519;

        ret = mbedtls_mpi_read_binary(&ecp->Q.X, (const unsigned char*)key + sizeof(s_der_pub_lead),
            ed25519_public_key_size);
        if (ret < 0)
            return ret;

        m_alg = "EdDSA";

        return 0;
    }

    return MBEDTLS_ERR_PK_KEY_INVALID_FORMAT;
}

int PKey::ed25519_parse_pem(const unsigned char* key)
{
    int32_t ret;
    mbedtls_pem_context pem;

    do {
        size_t len;

        mbedtls_pem_init(&pem);

        ret = mbedtls_pem_read_buffer(&pem, PEM_BEGIN_PRIVATE_KEY, PEM_END_PRIVATE_KEY,
            (const unsigned char*)key, NULL, 0, &len);
        if (ret == 0) {
            ret = ed25519_parse_key(pem.buf, pem.buflen);
            if (ret == 0)
                break;
        }

        ret = mbedtls_pem_read_buffer(&pem, PEM_BEGIN_PUBLIC_KEY, PEM_END_PUBLIC_KEY,
            (const unsigned char*)key, NULL, 0, &len);
        if (ret == 0) {
            ret = ed25519_parse_pub_key(pem.buf, pem.buflen);
            if (ret == 0)
                break;
        }
    } while (false);
    mbedtls_pem_free(&pem);

    return ret ? MBEDTLS_ERR_PK_KEY_INVALID_FORMAT : 0;
}

int PKey::ed25519_write_key(exlib::string& buf)
{
    mbedtls_ecp_keypair* ecp = mbedtls_pk_ec(m_key);

    buf.resize(ed25519_public_key_size + sizeof(s_der_priv_lead));
    memcpy(buf.c_buffer(), s_der_priv_lead, sizeof(s_der_priv_lead));
    mbedtls_mpi_write_binary(&ecp->d, (unsigned char*)buf.c_buffer() + sizeof(s_der_priv_lead), ed25519_public_key_size);
    return 0;
}

int PKey::ed25519_write_pub_key(exlib::string& buf)
{
    mbedtls_ecp_keypair* ecp = mbedtls_pk_ec(m_key);

    buf.resize(ed25519_public_key_size + sizeof(s_der_pub_lead));
    memcpy(buf.c_buffer(), s_der_pub_lead, sizeof(s_der_pub_lead));
    mbedtls_mpi_write_binary(&ecp->Q.X, (unsigned char*)buf.c_buffer() + sizeof(s_der_pub_lead), ed25519_public_key_size);
    return 0;
}

result_t PKey::ed25519_write_der(obj_ptr<Buffer_base>& retVal)
{
    mbedtls_ecp_keypair* ecp = mbedtls_pk_ec(m_key);
    exlib::string buf;

    if (mbedtls_mpi_cmp_int(&ecp->d, 0))
        ed25519_write_key(buf);
    else
        ed25519_write_pub_key(buf);

    retVal = new Buffer(buf);
    return 0;
}

int PKey::ed25519_write_pem(exlib::string& buf)
{
    mbedtls_ecp_keypair* ecp = mbedtls_pk_ec(m_key);
    exlib::string der_buf;
    size_t olen;
    int32_t ret;

    buf.resize(200);

    if (mbedtls_mpi_cmp_int(&ecp->d, 0)) {
        ed25519_write_key(der_buf);
        ret = mbedtls_pem_write_buffer(PEM_BEGIN_PRIVATE_KEY "\n", PEM_END_PRIVATE_KEY "\n",
            (const unsigned char*)der_buf.c_str(), der_buf.length(), (unsigned char*)buf.c_buffer(), buf.length(), &olen);
    } else {
        ed25519_write_pub_key(der_buf);
        ret = mbedtls_pem_write_buffer(PEM_BEGIN_PUBLIC_KEY "\n", PEM_END_PUBLIC_KEY "\n",
            (const unsigned char*)der_buf.c_str(), der_buf.length(), (unsigned char*)buf.c_buffer(), buf.length(), &olen);
    }
    if (ret != 0)
        return CHECK_ERROR(_ssl::setError(ret));

    buf.resize(olen - 1);
    return 0;
}

static int asn1_write_data(unsigned char** p, const unsigned char* start, const unsigned char* data, size_t datlen)
{
    int ret = MBEDTLS_ERR_ERROR_CORRUPTION_DETECTED;
    size_t len = datlen;

    if (*p < start || (size_t)(*p - start) < len)
        return (MBEDTLS_ERR_ASN1_BUF_TOO_SMALL);

    (*p) -= len;
    memcpy(*p, data, datlen);

    MBEDTLS_ASN1_CHK_ADD(len, mbedtls_asn1_write_len(p, start, len));
    MBEDTLS_ASN1_CHK_ADD(len, mbedtls_asn1_write_tag(p, start, MBEDTLS_ASN1_INTEGER));

    ret = (int)len;

    return (ret);
}

static int signature_to_asn1(const unsigned char* sig64, unsigned char* sig, size_t sig_size, size_t* slen)
{
    int ret = MBEDTLS_ERR_ERROR_CORRUPTION_DETECTED;
    unsigned char buf[MBEDTLS_ECDSA_MAX_LEN] = { 0 };
    unsigned char* p = buf + sizeof(buf);
    size_t len = 0;

    MBEDTLS_ASN1_CHK_ADD(len, asn1_write_data(&p, buf, sig64 + ed25519_public_key_size, ed25519_public_key_size));
    MBEDTLS_ASN1_CHK_ADD(len, asn1_write_data(&p, buf, sig64, ed25519_public_key_size));

    MBEDTLS_ASN1_CHK_ADD(len, mbedtls_asn1_write_len(&p, buf, len));
    MBEDTLS_ASN1_CHK_ADD(len, mbedtls_asn1_write_tag(&p, buf, MBEDTLS_ASN1_CONSTRUCTED | MBEDTLS_ASN1_SEQUENCE));

    if (len > sig_size)
        return (MBEDTLS_ERR_ECP_BUFFER_TOO_SMALL);

    memcpy(sig, p, len);
    *slen = len;

    return (0);
}

result_t PKey::ed25519_sign(Buffer_base* data, obj_ptr<Buffer_base>& retVal)
{
    mbedtls_ecp_keypair* ecp = mbedtls_pk_ec(m_key);
    unsigned char signature[ed25519_signature_size];
    unsigned char sk[ed25519_private_key_size];
    exlib::string buf;

    mbedtls_mpi_write_binary(&ecp->d, (unsigned char*)sk, ed25519_public_key_size);
    mbedtls_mpi_write_binary(&ecp->Q.X, (unsigned char*)sk + ed25519_public_key_size, ed25519_public_key_size);

    data->toString(buf);

    ed25519_SignMessage(signature, sk, NULL, (const unsigned char*)buf.c_str(), buf.length());

    size_t olen;
    buf.resize(70);
    signature_to_asn1(signature, (unsigned char*)buf.c_buffer(), buf.length(), &olen);
    buf.resize(olen);

    retVal = new Buffer(buf);

    return 0;
}

static int get_data(unsigned char* data, size_t sz, const unsigned char* p, size_t len)
{
    while (len && !*p) {
        len--;
        p++;
    }

    if (len > sz)
        return MBEDTLS_ERR_ECP_BUFFER_TOO_SMALL;

    if (len < sz)
        memset(data, 0, sz - len);
    memcpy(data + sz - len, p, len);

    return 0;
}

static int asn1_get_data(unsigned char** p, const unsigned char* end, unsigned char* data, size_t sz)
{
    int ret = MBEDTLS_ERR_ERROR_CORRUPTION_DETECTED;
    size_t len;

    if ((ret = mbedtls_asn1_get_tag(p, end, &len, MBEDTLS_ASN1_INTEGER)) != 0)
        return (ret);

    if ((ret = get_data(data, sz, *p, len)) != 0)
        return (ret);

    *p += len;

    return (ret);
}

static int asn1_to_signature(const unsigned char* date, size_t datlen, unsigned char* sig64)
{
    unsigned char* p = (unsigned char*)date;
    const unsigned char* end = date + datlen;
    size_t len;

    if (mbedtls_asn1_get_tag(&p, end, &len, MBEDTLS_ASN1_CONSTRUCTED | MBEDTLS_ASN1_SEQUENCE))
        return MBEDTLS_ERR_ECP_BAD_INPUT_DATA;

    if (p + len != end)
        return MBEDTLS_ERR_ECP_BAD_INPUT_DATA + MBEDTLS_ERR_ASN1_LENGTH_MISMATCH;

    if (asn1_get_data(&p, end, sig64, ed25519_public_key_size)
        || asn1_get_data(&p, end, sig64 + ed25519_public_key_size, ed25519_public_key_size))
        return MBEDTLS_ERR_ECP_BAD_INPUT_DATA;

    return (0);
}

result_t PKey::ed25519_verify(Buffer_base* data, Buffer_base* sign, bool& retVal)
{
    mbedtls_ecp_keypair* ecp = mbedtls_pk_ec(m_key);
    unsigned char signature[ed25519_signature_size];
    unsigned char pk[ed25519_public_key_size];
    exlib::string buf;

    mbedtls_mpi_write_binary(&ecp->Q.X, (unsigned char*)pk, ed25519_public_key_size);

    sign->toString(buf);
    asn1_to_signature((const unsigned char*)buf.c_str(), buf.length(), signature);

    data->toString(buf);
    retVal = ed25519_VerifySignature(signature, pk, (const unsigned char*)buf.c_str(), buf.length());

    return 0;
}

}
