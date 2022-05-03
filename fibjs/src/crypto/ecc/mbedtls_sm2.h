#include "mbedtls/pk.h"
#include "mbedtls/asn1write.h"

static int derive_mpi(const mbedtls_ecp_group* grp, mbedtls_mpi* x,
    const unsigned char* buf, size_t blen)
{
    int ret;
    size_t n_size = (grp->nbits + 7) / 8;
    size_t use_size = blen > n_size ? n_size : blen;

    MBEDTLS_MPI_CHK(mbedtls_mpi_read_binary(x, buf, use_size));
    if (use_size * 8 > grp->nbits)
        MBEDTLS_MPI_CHK(mbedtls_mpi_shift_r(x, use_size * 8 - grp->nbits));

    /* While at it, reduce modulo N */
    if (mbedtls_mpi_cmp_mpi(x, &grp->N) >= 0)
        MBEDTLS_MPI_CHK(mbedtls_mpi_sub_mpi(x, x, &grp->N));

cleanup:
    return (ret);
}

static int mbedtls_sm2_sign_to(mbedtls_ecp_group* grp, mbedtls_mpi* r, mbedtls_mpi* s,
    const mbedtls_mpi* d, const mbedtls_ecp_point* to_pubkey, const unsigned char* buf, size_t blen,
    int (*f_rng)(void*, unsigned char*, size_t), void* p_rng)
{
    int ret, key_tries, sign_tries, blind_tries;
    mbedtls_ecp_point R;
    mbedtls_mpi k, e, t, l, m;
    /* Fail cleanly on curves such as Curve25519 that can't be used for ECDSA */
    if (grp->N.p == NULL)
        return (MBEDTLS_ERR_ECP_BAD_INPUT_DATA);

    mbedtls_ecp_point_init(&R);
    mbedtls_mpi_init(&k);
    mbedtls_mpi_init(&e);
    mbedtls_mpi_init(&t);
    mbedtls_mpi_init(&l);
    mbedtls_mpi_init(&m);

    sign_tries = 0;
    do {
        /*
         * Step 0: derive MPI from hashed message
         */
        MBEDTLS_MPI_CHK(derive_mpi(grp, &e, buf, blen));
        /*
         *		Step 1-3:
         *		set r = (e+x) mod n
         */
        key_tries = 0;
        do {
            MBEDTLS_MPI_CHK(mbedtls_ecp_gen_keypair(grp, &k, &R, f_rng, p_rng));
            MBEDTLS_MPI_CHK(mbedtls_mpi_add_mpi(&l, &e, &R.X));
            MBEDTLS_MPI_CHK(mbedtls_mpi_mod_mpi(r, &l, &grp->N));

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

            if (key_tries++ > 10) {
                ret = MBEDTLS_ERR_ECP_RANDOM_FAILED;
                goto cleanup;
            }
            // r+k != n
            MBEDTLS_MPI_CHK((mbedtls_mpi_add_mpi(&m, r, &k)));
        } while ((mbedtls_mpi_cmp_int(r, 0) == 0) || (mbedtls_mpi_cmp_mpi(&m, &grp->N) == 0));

        /*
         * Generate a random value to blind inv_mod in next step,
         * avoiding a potential timing leak.
         */
        blind_tries = 0;
        do {
            size_t n_size = (grp->nbits + 7) / 8;
            MBEDTLS_MPI_CHK(mbedtls_mpi_fill_random(&t, n_size, f_rng, p_rng));
            MBEDTLS_MPI_CHK(mbedtls_mpi_shift_r(&t, 8 * n_size - grp->nbits));

            /* See mbedtls_ecp_gen_keypair() */
            if (++blind_tries > 30)
                return (MBEDTLS_ERR_ECP_RANDOM_FAILED);
        } while (mbedtls_mpi_cmp_int(&t, 1) < 0 || mbedtls_mpi_cmp_mpi(&t, &grp->N) >= 0);

        /*
         * Step 6: compute  s = ((1+d)^-1)*(k-r*d) mod n
         *
         */
        MBEDTLS_MPI_CHK(mbedtls_mpi_mul_mpi(s, r, d)); // s = r*d
        MBEDTLS_MPI_CHK(mbedtls_mpi_sub_mpi(s, &k, s)); // s = k - s
        MBEDTLS_MPI_CHK(mbedtls_mpi_mul_mpi(s, s, &t)); // s = s*t
        MBEDTLS_MPI_CHK(mbedtls_mpi_add_int(&l, d, 1)); // l = 1+d
        MBEDTLS_MPI_CHK(mbedtls_mpi_mul_mpi(&l, &l, &t)); // l=l*t
        MBEDTLS_MPI_CHK(mbedtls_mpi_inv_mod(&l, &l, &grp->N)); // l = l^-1
        MBEDTLS_MPI_CHK(mbedtls_mpi_mul_mpi(s, s, &l)); // s = s * l
        MBEDTLS_MPI_CHK(mbedtls_mpi_mod_mpi(s, s, &grp->N)); // s mod n

        if (sign_tries++ > 10) {
            ret = MBEDTLS_ERR_ECP_RANDOM_FAILED;
            goto cleanup;
        }
        //
    } while (mbedtls_mpi_cmp_int(&t, 1) < 0 || mbedtls_mpi_cmp_mpi(&t, &grp->N) >= 0);
cleanup:
    mbedtls_ecp_point_free(&R);
    mbedtls_mpi_free(&k);
    mbedtls_mpi_free(&e);
    mbedtls_mpi_free(&t);
    mbedtls_mpi_free(&l);
    mbedtls_mpi_free(&m);
    return (ret);
}

static int mbedtls_sm2_verify_to(mbedtls_ecp_group* grp, const unsigned char* buf, size_t blen,
    const mbedtls_ecp_point* Q, const mbedtls_mpi* to_key, const mbedtls_mpi* r, const mbedtls_mpi* s,
    int (*f_rng)(void*, unsigned char*, size_t), void* p_rng)
{
    int ret;
    mbedtls_mpi e, s_inv, u1, u2, t, result;
    mbedtls_ecp_point R;

    mbedtls_ecp_point_init(&R);
    mbedtls_mpi_init(&e);
    mbedtls_mpi_init(&s_inv);
    mbedtls_mpi_init(&u1);
    mbedtls_mpi_init(&u2);
    mbedtls_mpi_init(&t);
    mbedtls_mpi_init(&result);

    /* Fail cleanly on curves such as Curve25519 that can't be used for ECDSA */
    if (grp->N.p == NULL)
        return (MBEDTLS_ERR_ECP_BAD_INPUT_DATA);

    /*
     * Step 1: make sure r and s are in range 1..n-1
     */
    if (mbedtls_mpi_cmp_int(r, 1) < 0 || mbedtls_mpi_cmp_mpi(r, &grp->N) >= 0 || mbedtls_mpi_cmp_int(s, 1) < 0 || mbedtls_mpi_cmp_mpi(s, &grp->N) >= 0) {
        ret = MBEDTLS_ERR_ECP_VERIFY_FAILED;
        goto cleanup;
    }

    /*
     * Additional precaution: make sure Q is valid
     */
    MBEDTLS_MPI_CHK(mbedtls_ecp_check_pubkey(grp, Q));

    /*
     * Step 4: t = (r+s) mod n
     */
    MBEDTLS_MPI_CHK(mbedtls_mpi_add_mpi(&t, r, s));
    MBEDTLS_MPI_CHK(mbedtls_mpi_mod_mpi(&t, &t, &grp->N));
    if (mbedtls_mpi_cmp_int(&t, 0) == 0) {
        ret = MBEDTLS_ERR_ECP_VERIFY_FAILED;
        goto cleanup;
    }

    /*
     * Step 5: (x,y) = sG + tQ
     */
    MBEDTLS_MPI_CHK(mbedtls_ecp_muladd(grp, &R, s, &grp->G, &t, Q));

    /*
     * Step 3: derive MPI from hashed message
     */
    MBEDTLS_MPI_CHK(derive_mpi(grp, &e, buf, blen));

    if (to_key) {
        mbedtls_ecp_point C;

        /*
         * Steps 3.1: e = e + (dB * R).x
         */
        mbedtls_ecp_point_init(&C);
        MBEDTLS_MPI_CHK(mbedtls_ecp_mul(grp, &C, to_key, &R, f_rng, p_rng));
        MBEDTLS_MPI_CHK(mbedtls_mpi_add_mpi(&e, &e, &C.X));
        mbedtls_ecp_point_free(&C);

        mbedtls_mpi_mod_mpi(&e, &e, &grp->N);
    }

    /*
     * Step 6: result = (e+x) mod n
     */
    MBEDTLS_MPI_CHK(mbedtls_mpi_add_mpi(&e, &e, &R.X));
    MBEDTLS_MPI_CHK(mbedtls_mpi_mod_mpi(&result, &e, &grp->N));
    /*
     * Step 7: check if result.X (that is, result.X) is equal to r
     **/
    if (mbedtls_mpi_cmp_mpi(&result, r) != 0) {
        ret = MBEDTLS_ERR_ECP_VERIFY_FAILED;
        goto cleanup;
    }
    //
cleanup:
    mbedtls_ecp_point_free(&R);
    mbedtls_mpi_free(&e);
    mbedtls_mpi_free(&s_inv);
    mbedtls_mpi_free(&u1);
    mbedtls_mpi_free(&u2);
    mbedtls_mpi_free(&t);
    mbedtls_mpi_free(&result);
    return (ret);
}
