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

#ifdef __cplusplus
extern "C" {
#endif

int secp256k1_ec_pubkey_decompress(const secp256k1_context* ctx, unsigned char* pubkey, int* pubkeylen);

#ifdef __cplusplus
}
#endif

int secp256k1_ec_pubkey_decompress(const secp256k1_context* ctx, unsigned char* pubkey, int* pubkeylen)
{
    secp256k1_ge p;
    int ret = 0;
    (void)ctx;

    if (secp256k1_eckey_pubkey_parse(&p, pubkey, *pubkeylen)) {
        size_t sz = *pubkeylen;
        ret = secp256k1_eckey_pubkey_serialize(&p, pubkey, &sz, 0);
        *pubkeylen = (int)sz;
    }
    return ret;
}

static int ecdh_hash_function_X(unsigned char* output, const unsigned char* x32, const unsigned char* y32,
    void* data)
{
    (void)data;
    memcpy(output, x32, KEYSIZE_256);

    return 1;
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

static int secp256k1_ecsdsa_sign_to(const secp256k1_context* ctx, const secp256k1_pubkey* to_pubkey, unsigned char* sig64,
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

static int secp256k1_ecsdsa_verify_to(const secp256k1_context* ctx, const unsigned char* to_key, const unsigned char* sig64,
    const unsigned char* msg, size_t msglen, const secp256k1_pubkey* pubkey)
{
    secp256k1_scalar s;
    secp256k1_scalar e;
    secp256k1_scalar e1;
    secp256k1_gej rj;
    secp256k1_ge pk;
    secp256k1_gej pkj;
    secp256k1_ge r;
    unsigned char buf[32];
    unsigned char hash64[64];

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
    secp256k1_fe_get_b32(&hash64[0], &r.x);
    secp256k1_fe_normalize_var(&r.y);
    secp256k1_fe_get_b32(&hash64[32], &r.y);

    secp256k1_ecsdsa_hash(&e1, &hash64[0], msg, msglen);

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
