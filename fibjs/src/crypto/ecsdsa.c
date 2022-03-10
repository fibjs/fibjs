
#include "mbedtls/config.h"

#include "mbedtls/ecdsa.h"
#include "mbedtls/asn1write.h"
#include "mbedtls/pk.h"
#include "mbedtls/md.h"

#include <string.h>

#include "mbedtls/hmac_drbg.h"

#define SECP256K1_WIDEMUL_INT64 1
#define ECMULT_WINDOW_SIZE 15
#define ECMULT_GEN_PREC_BITS 4
#define SECP256K1_API

#define ENABLE_MODULE_ECDH 1
#define ENABLE_MODULE_RECOVERY 1
#define ENABLE_MODULE_EXTRAKEYS 1
#define ENABLE_MODULE_SCHNORRSIG 1

#include "secp256k1/src/secp256k1.c"
#include "mbedtls/src/secp256k1_api.h"

static int ecdh_hash_function_X(unsigned char* output, const unsigned char* x32, const unsigned char* y32,
    void* data)
{
    (void)data;
    memcpy(output, x32, KEYSIZE_256);

    return 1;
}

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

static void secp256k1_reverse_sign(unsigned char* data)
{
    unsigned char x;

    for (int i = 0; i < KEYSIZE_256 / 2; i++) {
        x = data[i];
        data[i] = data[KEYSIZE_256 - i - 1];
        data[KEYSIZE_256 - i - 1] = x;

        x = data[KEYSIZE_256 + i];
        data[KEYSIZE_256 + i] = data[KEYSIZE_256 + KEYSIZE_256 - i - 1];
        data[KEYSIZE_256 + KEYSIZE_256 - i - 1] = x;
    }
}

static void secp256k1_ecsdsa_hash(secp256k1_scalar* e, const unsigned char* r64, const unsigned char* msg, size_t msglen)
{
    unsigned char buf[32];
    secp256k1_sha256 sha;

    /* tagged hash(r.x, pk.x, msg) */
    secp256k1_sha256_initialize(&sha);
    secp256k1_sha256_write(&sha, r64, 64);
    secp256k1_sha256_write(&sha, msg, msglen);
    secp256k1_sha256_finalize(&sha, buf);
    /* Set scalar e to the challenge hash modulo the curve order as per
     * BIP340. */
    secp256k1_scalar_set_b32(e, buf, NULL);
}

struct curve_hash {
    mbedtls_ecp_group_id id;
    mbedtls_md_type_t hash_id;
    size_t sz;
};

static const struct curve_hash supported_curves[] = {
    { MBEDTLS_ECP_DP_SECP192R1, MBEDTLS_MD_SHA224, 24 },
    { MBEDTLS_ECP_DP_SECP224R1, MBEDTLS_MD_SHA224, 28 },
    { MBEDTLS_ECP_DP_SECP256R1, MBEDTLS_MD_SHA256, 32 },
    { MBEDTLS_ECP_DP_SECP384R1, MBEDTLS_MD_SHA384, 48 },
    { MBEDTLS_ECP_DP_SECP521R1, MBEDTLS_MD_SHA512, 64 },
    { MBEDTLS_ECP_DP_BP256R1, MBEDTLS_MD_SHA256, 32 },
    { MBEDTLS_ECP_DP_BP384R1, MBEDTLS_MD_SHA384, 48 },
    { MBEDTLS_ECP_DP_BP512R1, MBEDTLS_MD_SHA512, 64 },
    { MBEDTLS_ECP_DP_SECP192K1, MBEDTLS_MD_SHA224, 24 },
    { MBEDTLS_ECP_DP_SECP224K1, MBEDTLS_MD_SHA224, 28 },
    { MBEDTLS_ECP_DP_SECP256K1, MBEDTLS_MD_SHA256, 32 },
    { MBEDTLS_ECP_DP_SM2P256R1, MBEDTLS_MD_SM3, 32 }
};
#define CURVE_COUNT 12

static void mbedtls_ecsdsa_hash(mbedtls_ecp_group* grp, mbedtls_mpi* e, const mbedtls_ecp_point* R, const unsigned char* msg, size_t msglen)
{
    unsigned char buf[MBEDTLS_MD_MAX_SIZE];
    mbedtls_md_context_t hash;
    unsigned char data[MBEDTLS_ECP_MAX_BYTES];
    size_t grp_len = (grp->nbits + 7) / 8;
    int32_t i;
    const struct curve_hash* hash_info;

    for (i = 0; i < CURVE_COUNT; i++)
        if (grp->id == supported_curves[i].id) {
            hash_info = &supported_curves[i];
            break;
        }

    if (i == CURVE_COUNT) {
        mbedtls_mpi_lset(e, 0);
        return;
    }

    mbedtls_md_init(&hash);
    mbedtls_md_setup(&hash, mbedtls_md_info_from_type(hash_info->hash_id), 0);
    mbedtls_md_starts(&hash);

    mbedtls_mpi_write_binary(&R->X, data, grp_len);
    mbedtls_md_update(&hash, data, grp_len);

    mbedtls_mpi_write_binary(&R->Y, data, grp_len);
    mbedtls_md_update(&hash, data, grp_len);

    mbedtls_md_update(&hash, msg, msglen);

    mbedtls_md_finish(&hash, buf);
    mbedtls_md_free(&hash);

    mbedtls_mpi_read_binary(e, buf, hash_info->sz);
    mbedtls_mpi_mod_mpi(e, e, &grp->N);
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

static int secp256k1_ecsdsa_sign(const secp256k1_context* ctx, const secp256k1_pubkey* to_pubkey, unsigned char* sig64,
    const unsigned char* msg, size_t msglen, const secp256k1_keypair* keypair)
{
    secp256k1_scalar sk;
    secp256k1_scalar e;
    secp256k1_scalar k;
    secp256k1_gej rj;
    secp256k1_ge pk;
    secp256k1_ge r;
    unsigned char buf[32] = { 0 };
    unsigned char seckey[32];
    int ret = 1;

    VERIFY_CHECK(ctx != NULL);
    ARG_CHECK(secp256k1_ecmult_gen_context_is_built(&ctx->ecmult_gen_ctx));
    ARG_CHECK(sig64 != NULL);
    ARG_CHECK(msg != NULL || msglen == 0);
    ARG_CHECK(keypair != NULL);

    ret &= secp256k1_keypair_load(ctx, &sk, &pk, keypair);

    /*
     * Steps 1: k = RNG({1,2,...,n−1})
     */
    secp256k1_scalar_get_b32(seckey, &sk);
    secp256k1_fe_get_b32(buf, &pk.x);
    ret &= !!secp256k1_nonce_function_bip340(buf, msg, msglen, seckey, buf, bip340_algo, sizeof(bip340_algo), NULL);
    secp256k1_scalar_set_b32(&k, buf, NULL);
    ret &= !secp256k1_scalar_is_zero(&k);
    secp256k1_scalar_cmov(&k, &secp256k1_scalar_one, !ret);

    /*
     * Steps 2: Q = k * G
     */
    secp256k1_ecmult_gen(&ctx->ecmult_gen_ctx, &rj, &k);
    secp256k1_ge_set_gej(&r, &rj);

    secp256k1_declassify(ctx, &r, sizeof(r));

    /*
     * Steps 3: r = OS2I(hash(FE2OS(xQ) ∥ FE2OS(yQ) ∥ M))
     */
    secp256k1_fe_normalize_var(&r.x);
    secp256k1_fe_get_b32(&sig64[0], &r.x);
    secp256k1_fe_normalize_var(&r.y);
    secp256k1_fe_get_b32(&sig64[32], &r.y);

    secp256k1_ecsdsa_hash(&e, &sig64[0], msg, msglen);

    if (to_pubkey) {
        secp256k1_scalar c;

        /*
         * Steps 3.1: r = r + (k * PB).x
         */
        secp256k1_ecdh(ctx, buf, to_pubkey, buf, ecdh_hash_function_X, NULL);
        secp256k1_scalar_set_b32(&c, buf, NULL);

        secp256k1_scalar_add(&e, &e, &c);
    }

    secp256k1_scalar_get_b32(&sig64[0], &e);

    /*
     * Steps 4: s = k + r * dA mod n
     */
    secp256k1_scalar_mul(&e, &e, &sk);
    secp256k1_scalar_add(&e, &e, &k);
    secp256k1_scalar_get_b32(&sig64[32], &e);

    secp256k1_memczero(sig64, 64, !ret);
    secp256k1_scalar_clear(&k);
    secp256k1_scalar_clear(&sk);
    memset(seckey, 0, sizeof(seckey));

    return ret;
}

int mbedtls_ecsdsa_sign(mbedtls_ecp_group* grp, mbedtls_mpi* r, mbedtls_mpi* s,
    const mbedtls_mpi* d, const mbedtls_ecp_point* to_pubkey, const unsigned char* buf, size_t blen,
    int (*f_rng)(void*, unsigned char*, size_t), void* p_rng)
{
    int ret, key_tries, sign_tries;
    mbedtls_ecp_point Q;
    mbedtls_mpi k, e;

    /* Fail cleanly on curves such as Curve25519 that can't be used for ECDSA */
    if (grp->N.p == NULL)
        return (MBEDTLS_ERR_ECP_BAD_INPUT_DATA);

    /* Make sure d is in range 1..n-1 */
    if (mbedtls_mpi_cmp_int(d, 1) < 0 || mbedtls_mpi_cmp_mpi(d, &grp->N) >= 0)
        return (MBEDTLS_ERR_ECP_INVALID_KEY);

    mbedtls_ecp_point_init(&Q);
    mbedtls_mpi_init(&k);
    mbedtls_mpi_init(&e);

    sign_tries = 0;
    do {
        if (sign_tries++ > 10) {
            ret = MBEDTLS_ERR_ECP_RANDOM_FAILED;
            goto cleanup;
        }

        key_tries = 0;
        do {
            if (key_tries++ > 10) {
                ret = MBEDTLS_ERR_ECP_RANDOM_FAILED;
                goto cleanup;
            }

            /*
             * Steps 1: k = RNG({1,2,...,n−1})
             */
            MBEDTLS_MPI_CHK(mbedtls_ecp_gen_privkey(grp, &k, f_rng, p_rng));

            /*
             * Steps 2: Q = k * G
             */
            MBEDTLS_MPI_CHK(mbedtls_ecp_mul(grp, &Q, &k, &grp->G, f_rng, p_rng));
        } while (mbedtls_mpi_cmp_int(&Q.X, 0) == 0);

        /*
         * Steps 3: r = OS2I(hash(FE2OS(xQ) ∥ FE2OS(yQ) ∥ M))
         */
        mbedtls_ecsdsa_hash(grp, r, &Q, buf, blen);

        if (to_pubkey) {
            mbedtls_ecp_point C;

            /*
             * Steps 3.1: r = r + (k * PB).x
             */
            mbedtls_ecp_point_init(&C);
            MBEDTLS_MPI_CHK(mbedtls_ecp_mul(grp, &C, &k, to_pubkey, NULL, NULL));
            MBEDTLS_MPI_CHK(mbedtls_mpi_add_mpi(r, r, &C.X));
            mbedtls_ecp_point_free(&C);

            mbedtls_mpi_mod_mpi(r, r, &grp->N);
        }

        /*
         * Steps 4: s = k + r * dA mod n
         */
        MBEDTLS_MPI_CHK(mbedtls_mpi_mul_mpi(&e, r, d));
        MBEDTLS_MPI_CHK(mbedtls_mpi_add_mpi(&e, &e, &k));
        MBEDTLS_MPI_CHK(mbedtls_mpi_mod_mpi(s, &e, &grp->N));
    } while (mbedtls_mpi_cmp_int(s, 0) == 0);

cleanup:
    mbedtls_ecp_point_free(&Q);
    mbedtls_mpi_free(&k);
    mbedtls_mpi_free(&e);

    return (ret);
}

int ecsdsa_sign(mbedtls_ecp_keypair* ctx, mbedtls_ecp_keypair* to_ctx, const unsigned char* hash, size_t hlen,
    unsigned char* sig, size_t* slen, int (*f_rng)(void*, unsigned char*, size_t), void* p_rng)
{
    if (ctx->grp.id == MBEDTLS_ECP_DP_SECP256K1) {
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

        secp256k1_ecsdsa_sign(secp256k1_ctx, to_ctx ? &to_pubkey : NULL, signature.data, hash, hlen, &keypair);
        secp256k1_reverse_sign(signature.data);

        *slen = 80;
        secp256k1_ecdsa_signature_serialize_der(secp256k1_ctx, sig, slen, &signature);

        return 0;
    }

    int ret;
    mbedtls_mpi r, s;

    mbedtls_mpi_init(&r);
    mbedtls_mpi_init(&s);

    MBEDTLS_MPI_CHK(mbedtls_ecsdsa_sign(&ctx->grp, &r, &s, &ctx->d, to_ctx ? &to_ctx->Q : NULL,
        hash, hlen, f_rng, p_rng));

    *slen = 80;
    MBEDTLS_MPI_CHK(ecdsa_signature_to_asn1(&r, &s, sig, slen));

cleanup:
    mbedtls_mpi_free(&r);
    mbedtls_mpi_free(&s);

    return 0;
}

static int secp256k1_ecsdsa_verify(const secp256k1_context* ctx, const unsigned char* to_key, const unsigned char* sig64,
    const unsigned char* msg, size_t msglen, const secp256k1_pubkey* pubkey)
{
    secp256k1_scalar s;
    secp256k1_scalar e;
    secp256k1_scalar e1;
    secp256k1_gej rj;
    secp256k1_ge pk;
    secp256k1_gej pkj;
    secp256k1_fe rx;
    secp256k1_ge r;
    unsigned char buf[32];
    int overflow;

    VERIFY_CHECK(ctx != NULL);
    ARG_CHECK(sig64 != NULL);
    ARG_CHECK(msg != NULL || msglen == 0);
    ARG_CHECK(pubkey != NULL);

    secp256k1_scalar_set_b32(&e, &sig64[0], NULL);
    secp256k1_scalar_set_b32(&s, &sig64[32], NULL);

    if (!secp256k1_pubkey_load(ctx, &pk, pubkey)) {
        return 0;
    }

    /*
     * Step 2: Q = s * G − r * PA
     */
    secp256k1_scalar_negate(&e1, &e);
    secp256k1_gej_set_ge(&pkj, &pk);
    secp256k1_ecmult(&rj, &pkj, &e1, &s);

    secp256k1_ge_set_gej_var(&r, &rj);
    if (secp256k1_ge_is_infinity(&r)) {
        return 0;
    }

    /*
     * Steps 3: e = OS2I(hash(FE2OS(xQ) ∥ FE2OS(yQ) ∥ M))
     */
    secp256k1_fe_normalize_var(&r.x);
    secp256k1_fe_get_b32(&sig64[0], &r.x);
    secp256k1_fe_normalize_var(&r.y);
    secp256k1_fe_get_b32(&sig64[32], &r.y);

    secp256k1_ecsdsa_hash(&e1, &sig64[0], msg, msglen);

    if (to_key) {
        secp256k1_scalar c;
        secp256k1_pubkey r_pubkey;

        /*
         * Steps 3.1: e = e + (dB * Q).x
         */
        secp256k1_pubkey_save(&r_pubkey, &r);

        secp256k1_ecdh(ctx, buf, &r_pubkey, to_key, ecdh_hash_function_X, NULL);
        secp256k1_scalar_set_b32(&c, buf, NULL);

        secp256k1_scalar_add(&e1, &e1, &c);
    }

    /*
     * Steps 4: e ?= r
     */
    return secp256k1_scalar_eq(&e, &e1);
}

int mbedtls_ecsdsa_verify(const mbedtls_ecp_group* grp, const unsigned char* buf, size_t blen,
    const mbedtls_ecp_point* P, const mbedtls_mpi* to_key, const mbedtls_mpi* r, const mbedtls_mpi* s)
{
    int ret = 0;
    mbedtls_mpi e;
    mbedtls_ecp_point Q;

    mbedtls_ecp_point_init(&Q);
    mbedtls_mpi_init(&e);

    /*
     * Step 1: make sure r and s are in range 1..n-1
     */
    if (mbedtls_mpi_cmp_int(r, 1) < 0 || mbedtls_mpi_cmp_mpi(r, &grp->N) >= 0
        || mbedtls_mpi_cmp_int(s, 1) < 0 || mbedtls_mpi_cmp_mpi(s, &grp->N) >= 0) {
        ret = MBEDTLS_ERR_ECP_VERIFY_FAILED;
        goto cleanup;
    }

    /*
     * Additional precaution: make sure P is valid
     */
    MBEDTLS_MPI_CHK(mbedtls_ecp_check_pubkey(grp, P));

    /*
     * Step 2: Q = s * G − r * PA
     */
    MBEDTLS_MPI_CHK(mbedtls_mpi_sub_mpi(&e, &grp->N, r));
    MBEDTLS_MPI_CHK(mbedtls_ecp_muladd(grp, &Q, s, &grp->G, &e, P));

    /*
     * Steps 3: e = OS2I(hash(FE2OS(xQ) ∥ FE2OS(yQ) ∥ M))
     */
    mbedtls_ecsdsa_hash(grp, &e, &Q, buf, blen);

    if (to_key) {
        mbedtls_ecp_point C;

        /*
         * Steps 3.1: e = e + (dB * Q).x
         */
        mbedtls_ecp_point_init(&C);
        MBEDTLS_MPI_CHK(mbedtls_ecp_mul(grp, &C, to_key, &Q, NULL, NULL));
        MBEDTLS_MPI_CHK(mbedtls_mpi_add_mpi(&e, &e, &C.X));
        mbedtls_ecp_point_free(&C);

        mbedtls_mpi_mod_mpi(&e, &e, &grp->N);
    }

    /*
     * Steps 4: e ?= r
     */
    if (mbedtls_mpi_cmp_mpi(&e, r) != 0) {
        ret = MBEDTLS_ERR_ECP_VERIFY_FAILED;
        goto cleanup;
    }

cleanup:
    mbedtls_ecp_point_free(&Q);
    mbedtls_mpi_free(&e);

    return (ret);
}

int ecsdsa_verify(mbedtls_ecp_keypair* ctx, mbedtls_ecp_keypair* to_ctx, const unsigned char* hash, size_t hlen,
    const unsigned char* sig, size_t slen)
{
    if (ctx->grp.id == MBEDTLS_ECP_DP_SECP256K1) {
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
        if (!secp256k1_ecsdsa_verify(secp256k1_ctx, to_ctx ? &to_key : NULL, signature.data, hash, hlen, &pubkey))
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

    if ((ret = mbedtls_asn1_get_tag(&p, end, &len,
             MBEDTLS_ASN1_CONSTRUCTED | MBEDTLS_ASN1_SEQUENCE))
        != 0) {
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

    if ((ret = mbedtls_ecsdsa_verify(&ctx->grp, hash, hlen, &ctx->Q, to_ctx ? &to_ctx->d : NULL, &r, &s)) != 0) {
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