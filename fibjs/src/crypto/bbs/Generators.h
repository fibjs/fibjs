/*
 * Generators.h
 *
 *  Created on: Jun 29, 2023
 *      Author: lion
 */

#pragma once

#include "defs.h"
#include <span>
#include <exlib/include/utils.h>
#include <blst/include/bbs.h>

namespace fibjs {

static void add_mul(blst_p1& b, blst_p1& p, blst_scalar& s)
{
    blst_p1 g;

    blst_p1_mult(&g, &p, (const byte*)&s, 256);
    blst_p1_add(&b, &b, &g);
}

class Generators {
public:
    Generators(const Generators& g)
        : h_ptr(g.h_ptr)
        , q1(g.q1)
        , h(g.h)
    {
    }

    Generators(size_t sz)
        : h_ptr(get(sz))
        , q1(h_ptr.get()[0])
        , h(h_ptr.get() + 1, sz)
    {
    }

public:
    blst_p1 compute_b(blst_scalar* msgs, blst_scalar& domain)
    {
        blst_p1 b;

        blst_p1_from_affine(&b, &BLS12_381_G1_P1);

        add_mul(b, q1, domain);
        for (size_t i = 0; i < h.size(); i++)
            add_mul(b, h[i], msgs[i]);

        return b;
    }

private:
    std::shared_ptr<blst_p1> h_ptr;

public:
    blst_p1& q1;
    std::span<blst_p1> h;

private:
    static std::shared_ptr<blst_p1> get(size_t sz)
    {
        static int32_t num = 0;
        static unsigned char v[G1_COMPRESSED_SIZE + sizeof(num)];
        static exlib::spinlock s_lock;

        static size_t pos = 0;
        static std::shared_ptr<blst_p1> h_ptr;

        s_lock.lock();

        if (num == 0) {
            blst_expand_message_xmd(v, G1_COMPRESSED_SIZE, DST_G1_BBS_MSG_SEED, sizeof(DST_G1_BBS_MSG_SEED) - 1,
                DST_G1_BBS_SIG_SEED, sizeof(DST_G1_BBS_SIG_SEED) - 1);
            num++;
        }

        if (pos < sz + 1) {
            blst_p1* h_ptr_new = new blst_p1[sz + 1];
            if (pos)
                memcpy(h_ptr_new, h_ptr.get(), pos * sizeof(blst_p1));

            h_ptr.reset(h_ptr_new);

            while (pos < sz + 1) {
                v[G1_COMPRESSED_SIZE] = (num >> 24) & 0xff;
                v[G1_COMPRESSED_SIZE + 1] = (num >> 16) & 0xff;
                v[G1_COMPRESSED_SIZE + 2] = (num >> 8) & 0xff;
                v[G1_COMPRESSED_SIZE + 3] = num & 0xff;

                blst_expand_message_xmd(v, G1_COMPRESSED_SIZE, v, sizeof(v),
                    DST_G1_BBS_SIG_SEED, sizeof(DST_G1_BBS_SIG_SEED) - 1);
                num++;

                blst_p1 k;
                blst_hash_to_g1(&k, v, G1_COMPRESSED_SIZE, DST_G1_BBS_SIG, sizeof(DST_G1_BBS_SIG) - 1);

                size_t i;
                for (i = 0; i < pos; i++)
                    if (memcmp(&k, &h_ptr.get()[i], sizeof(blst_p1)) == 0)
                        break;
                if (i < pos)
                    continue;

                h_ptr.get()[pos++] = k;
            }
        }

        std::shared_ptr<blst_p1> gens = h_ptr;

        s_lock.unlock();

        return gens;
    }
};

}
