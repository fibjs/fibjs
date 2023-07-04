/*
 * Signature.h
 *
 *  Created on: Jun 29, 2023
 *      Author: lion
 */

#pragma once

#include "defs.h"
#include "bls_func.h"
#include "Buffer.h"
#include <blst/include/blst.h>
#include "ssl.h"

namespace fibjs {

class Signature {
public:
    bool parse(Buffer_base* sig)
    {
        Buffer* buf = Buffer::Cast(sig);

        decoder dec(buf->data(), buf->length());
        return dec.decode(&a, &e) == 0;
    }

    void serialize(obj_ptr<Buffer_base>& retVal)
    {
        Buffer* buf = new Buffer(NULL, G1_COMPRESSED_SIZE + FR_BYTE_SIZE);

        blst_p1_compress(buf->data(), &a);
        blst_bendian_from_scalar(buf->data() + G1_COMPRESSED_SIZE, &e);

        retVal = buf;
    }

    bool verify(const blst_p2& pk, const blst_p1& b)
    {
        blst_p1 p2 = b;
        blst_p1_affine p2a;

        blst_p1_cneg(&p2, 1);
        blst_p1_to_affine(&p2a, &p2);

        blst_p2 q1;
        blst_p2_affine q1a;

        blst_p2_from_affine(&q1, &BLS12_381_G2);
        blst_p2_mult(&q1, &q1, (const byte*)&e, 256);

        blst_p2_add(&q1, &q1, &pk);

        blst_p2_to_affine(&q1a, &q1);

        blst_p1_affine aa;
        blst_p1_to_affine(&aa, &a);

        blst_pairing* pairing = (blst_pairing*)alloca(blst_pairing_sizeof());

        blst_pairing_init(pairing, false);

        blst_pairing_raw_aggregate(pairing, &q1a, &aa);
        blst_pairing_raw_aggregate(pairing, &BLS12_381_G2, &p2a);

        blst_pairing_commit(pairing);
        return blst_pairing_finalverify(pairing);
    }

    void sign(const blst_scalar& sk, const blst_p1& b)
    {
        blst_fr exp;
        blst_fr e1;
        blst_scalar s;

        mbedtls_ctr_drbg_random(&g_ssl.ctr_drbg, (byte*)&e, sizeof(e));
        blst_fr_from_scalar(&e1, &e);

        blst_fr_from_scalar(&exp, &sk);
        blst_fr_add(&exp, &exp, &e1);

        blst_fr_eucl_inverse(&exp, &exp);

        blst_scalar_from_fr(&s, &exp);
        blst_sign_pk_in_g2(&a, &b, &s);
    }

public:
    static blst_scalar random_scalar()
    {
        byte bin[32];
        blst_scalar s;

        mbedtls_ctr_drbg_random(&g_ssl.ctr_drbg, bin, sizeof(bin));
        blst_scalar_from_le_bytes(&s, bin, sizeof(bin));

        return s;
    }

public:
    blst_p1 a;
    blst_scalar e;
};

}
