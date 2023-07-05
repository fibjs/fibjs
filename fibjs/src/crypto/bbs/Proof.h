/*
 * Proof.h
 *
 *  Created on: Jul 3, 2023
 *      Author: lion
 */

#pragma once

#include "defs.h"
#include <blst/include/blst.h>
#include <vector>

namespace fibjs {

class Proof {
public:
    bool parse(Buffer_base* sig)
    {
        Buffer* buf = Buffer::Cast(sig);

        decoder dec(buf->data(), buf->length());
        intptr_t sz = dec.decode(&abar, &bbar, &c, &r2hat, &r3hat);
        if (sz < 0)
            return false;

        size_t mlen = sz / FR_BYTE_SIZE;
        mhat.resize(mlen);
        for (size_t i = 0; i < mlen; i++)
            dec.decode(&mhat[i]);

        return true;
    }

    void serialize(obj_ptr<Buffer_base>& retVal)
    {
        std::vector<uint8_t> data = encode(abar, bbar, c, r2hat, r3hat, mhat);
        retVal = new Buffer(data.data(), data.size());
    }

    bool verify(const blst_p2& pk)
    {
        blst_p1 p2 = bbar;
        blst_p1_affine p2a;

        blst_p1_cneg(&p2, 1);
        blst_p1_to_affine(&p2a, &p2);

        blst_p2_affine q1a;
        blst_p2_to_affine(&q1a, &pk);

        blst_p1_affine aa;
        blst_p1_to_affine(&aa, &abar);

        blst_pairing* pairing = (blst_pairing*)alloca(blst_pairing_sizeof());

        blst_pairing_init(pairing, false);

        blst_pairing_raw_aggregate(pairing, &q1a, &aa);
        blst_pairing_raw_aggregate(pairing, &BLS12_381_G2, &p2a);

        blst_pairing_commit(pairing);
        return blst_pairing_finalverify(pairing);
    }

public:
    blst_p1 abar;
    blst_p1 bbar;
    blst_scalar c;
    blst_scalar r2hat;
    blst_scalar r3hat;
    std::vector<blst_scalar> mhat;
};

}
