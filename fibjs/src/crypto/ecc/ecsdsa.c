
#include "mbedtls/config.h"

#include <string.h>

#include "secp256k1_sdsa.h"
#include "mbedtls_sdsa.h"
#include "mbedtls_sm2.h"

/*
 * Convert a signature (given by context) to ASN.1
 */
static int ecdsa_signature_to_asn1(const mbedtls_mpi* r, const mbedtls_mpi* s,
    unsigned char* sig, size_t* slen)
{
    int ret;
    unsigned char buf[MBEDTLS_ECDSA_MAX_LEN];
    unsigned char* p = buf + sizeof(buf);
    size_t len = 0;

    MBEDTLS_ASN1_CHK_ADD(len, mbedtls_asn1_write_mpi(&p, buf, s));
    MBEDTLS_ASN1_CHK_ADD(len, mbedtls_asn1_write_mpi(&p, buf, r));

    MBEDTLS_ASN1_CHK_ADD(len, mbedtls_asn1_write_len(&p, buf, len));
    MBEDTLS_ASN1_CHK_ADD(len, mbedtls_asn1_write_tag(&p, buf, MBEDTLS_ASN1_CONSTRUCTED | MBEDTLS_ASN1_SEQUENCE));

    memcpy(sig, p, len);
    *slen = len;

    return (0);
}

static void print_mpi(const mbedtls_mpi* e)
{
    unsigned char data[MBEDTLS_ECP_MAX_BYTES * 2 + 1];
    size_t len = 0;
    static char hex_char[] = "0123456789abcdef";

    len = mbedtls_mpi_size(e);
    mbedtls_mpi_write_binary(e, data, len);
    for (int i = 0; i < len; i++) {
        printf("0x%02x,", data[i]);
    }

    puts("");
}

int ecsdsa_sign(mbedtls_ecp_keypair* ctx, int sdsa, mbedtls_ecp_keypair* to_ctx, const unsigned char* hash, size_t hlen,
    unsigned char* sig, size_t* slen, int (*f_rng)(void*, unsigned char*, size_t), void* p_rng)
{
    if (sdsa && ctx->grp.id == MBEDTLS_ECP_DP_SECP256K1) {
        secp256k1_keypair keypair;
        secp256k1_pubkey* pubkey = (secp256k1_pubkey*)&keypair.data[32];
        secp256k1_ecdsa_signature signature;
        secp256k1_pubkey to_pubkey;

        mpi_write_key(&ctx->Q.X, pubkey->data);
        mpi_write_key(&ctx->Q.Y, pubkey->data + KEYSIZE_256);
        mbedtls_mpi_write_binary(&ctx->d, keypair.data, KEYSIZE_256);

        if (to_ctx) {
            mpi_write_key(&to_ctx->Q.X, to_pubkey.data);
            mpi_write_key(&to_ctx->Q.Y, to_pubkey.data + KEYSIZE_256);
        }

        secp256k1_ecsdsa_sign_to(secp256k1_ctx, to_ctx ? &to_pubkey : NULL, signature.data, hash, hlen, &keypair);
        secp256k1_reverse_sign(signature.data);

        secp256k1_ecdsa_signature_serialize_der(secp256k1_ctx, sig, slen, &signature);

        return 0;
    }

    int ret;
    mbedtls_mpi r, s;

    mbedtls_mpi_init(&r);
    mbedtls_mpi_init(&s);

    if (sdsa)
        MBEDTLS_MPI_CHK(mbedtls_ecsdsa_sign_to(&ctx->grp, &r, &s, &ctx->d, to_ctx ? &to_ctx->Q : NULL,
            hash, hlen, f_rng, p_rng));
    else if (ctx->grp.id == MBEDTLS_ECP_DP_SM2P256R1)
        MBEDTLS_MPI_CHK(mbedtls_sm2_sign_to(&ctx->grp, &r, &s, &ctx->d, to_ctx ? &to_ctx->Q : NULL,
            hash, hlen, f_rng, p_rng));
    else {
        ret = MBEDTLS_ERR_ECP_FEATURE_UNAVAILABLE;
        goto cleanup;
    }

    MBEDTLS_MPI_CHK(ecdsa_signature_to_asn1(&r, &s, sig, slen));

cleanup:
    mbedtls_mpi_free(&r);
    mbedtls_mpi_free(&s);

    return 0;
}

int ecsdsa_verify(mbedtls_ecp_keypair* ctx, int sdsa, mbedtls_ecp_keypair* to_ctx, const unsigned char* hash, size_t hlen,
    const unsigned char* sig, size_t slen)
{
    if (sdsa && ctx->grp.id == MBEDTLS_ECP_DP_SECP256K1) {
        secp256k1_pubkey pubkey;
        secp256k1_ecdsa_signature signature;
        unsigned char to_key[KEYSIZE_256];

        mpi_write_key(&ctx->Q.X, pubkey.data);
        mpi_write_key(&ctx->Q.Y, pubkey.data + KEYSIZE_256);

        if (to_ctx)
            mbedtls_mpi_write_binary(&to_ctx->d, to_key, KEYSIZE_256);

        if (!secp256k1_ecdsa_signature_parse_der(secp256k1_ctx, &signature, sig, slen))
            return MBEDTLS_ERR_ECP_BAD_INPUT_DATA;

        secp256k1_reverse_sign(signature.data);
        if (!secp256k1_ecsdsa_verify_to(secp256k1_ctx, to_ctx ? &to_key : NULL, signature.data, hash, hlen, &pubkey))
            return MBEDTLS_ERR_ECP_VERIFY_FAILED;

        return 0;
    }

    int ret;
    unsigned char* p = (unsigned char*)sig;
    const unsigned char* end = sig + slen;
    size_t len;
    mbedtls_mpi r, s;

    mbedtls_mpi_init(&r);
    mbedtls_mpi_init(&s);

    if ((ret = mbedtls_asn1_get_tag(&p, end, &len, MBEDTLS_ASN1_CONSTRUCTED | MBEDTLS_ASN1_SEQUENCE)) != 0) {
        ret += MBEDTLS_ERR_ECP_BAD_INPUT_DATA;
        goto cleanup;
    }

    if (p + len != end) {
        ret = MBEDTLS_ERR_ECP_BAD_INPUT_DATA + MBEDTLS_ERR_ASN1_LENGTH_MISMATCH;
        goto cleanup;
    }

    if ((ret = mbedtls_asn1_get_mpi(&p, end, &r)) != 0 || (ret = mbedtls_asn1_get_mpi(&p, end, &s)) != 0) {
        ret += MBEDTLS_ERR_ECP_BAD_INPUT_DATA;
        goto cleanup;
    }

    if (sdsa)
        ret = mbedtls_ecsdsa_verify_to(&ctx->grp, hash, hlen, &ctx->Q, to_ctx ? &to_ctx->d : NULL, &r, &s);
    else if (ctx->grp.id == MBEDTLS_ECP_DP_SM2P256R1)
        ret = mbedtls_sm2_verify_to(&ctx->grp, hash, hlen, &ctx->Q, to_ctx ? &to_ctx->d : NULL, &r, &s);
    else
        ret = MBEDTLS_ERR_ECP_VERIFY_FAILED;

    if (ret != 0) {
        ret = MBEDTLS_ERR_ECP_VERIFY_FAILED;
        goto cleanup;
    }

    if (p != end)
        ret = MBEDTLS_ERR_ECP_SIG_LEN_MISMATCH;

cleanup:
    mbedtls_mpi_free(&r);
    mbedtls_mpi_free(&s);

    return (ret);
}