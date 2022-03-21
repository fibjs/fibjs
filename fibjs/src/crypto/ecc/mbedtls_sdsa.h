#include "mbedtls/md.h"
#include "mbedtls/pk.h"
#include "mbedtls/asn1write.h"

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

static int mbedtls_ecsdsa_sign_to(mbedtls_ecp_group* grp, mbedtls_mpi* r, mbedtls_mpi* s,
    const mbedtls_mpi* d, const mbedtls_ecp_point* to_pubkey, const unsigned char* buf, size_t blen,
    int (*f_rng)(void*, unsigned char*, size_t), void* p_rng)
{
    int ret = 0;
    int key_tries, sign_tries;
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
             * Steps 2: Q = k * G
             */
            MBEDTLS_MPI_CHK(mbedtls_ecp_gen_keypair(grp, &k, &Q, f_rng, p_rng));
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
            MBEDTLS_MPI_CHK(mbedtls_ecp_mul(grp, &C, &k, to_pubkey, f_rng, p_rng));
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

static int mbedtls_ecsdsa_verify_to(const mbedtls_ecp_group* grp, const unsigned char* buf, size_t blen,
    const mbedtls_ecp_point* P, const mbedtls_mpi* to_key, const mbedtls_mpi* r, const mbedtls_mpi* s,
    int (*f_rng)(void*, unsigned char*, size_t), void* p_rng)
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
        MBEDTLS_MPI_CHK(mbedtls_ecp_mul(grp, &C, to_key, &Q, f_rng, p_rng));
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
